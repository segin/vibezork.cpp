#include "combat.h"
#include "timer.h"
#include "death.h"
#include "core/globals.h"
#include "core/io.h"
#include <cstdlib>
#include <ctime>

namespace CombatSystem {

CombatManager& CombatManager::instance() {
    static CombatManager instance;
    return instance;
}

void CombatManager::startCombat(ZObject* enemy, ZObject* weapon) {
    if (!enemy) {
        return;
    }
    
    auto& g = Globals::instance();
    
    // Initialize player combatant
    int playerHealth = 20;  // Default player health
    player_ = Combatant(g.winner, playerHealth, weapon);
    
    // Initialize enemy combatant
    // Enemy health is based on STRENGTH property
    int enemyStrength = enemy->getProperty(P_STRENGTH);
    if (enemyStrength == 0) {
        enemyStrength = 5;  // Default enemy strength
    }
    
    // Health is proportional to strength
    // Thief: STRENGTH 2 -> 10 HP
    // Troll: STRENGTH 2 -> 10 HP  
    // Cyclops: STRENGTH 10000 -> very high HP (special case)
    int enemyHealth = enemyStrength * 5;
    if (enemyHealth > 100) {
        enemyHealth = 100;  // Cap at reasonable value
    }
    
    // Find enemy's weapon
    ZObject* enemyWeapon = nullptr;
    for (auto* item : enemy->getContents()) {
        if (item->hasFlag(ObjectFlag::WEAPONBIT)) {
            enemyWeapon = item;
            break;
        }
    }
    
    enemy_ = Combatant(enemy, enemyHealth, enemyWeapon);
    
    inCombat_ = true;
    
    // Register and enable the I-FIGHT timer
    // Combat round every turn (interval = 1)
    TimerSystem::registerTimer("I-FIGHT", 1, []() {
        CombatManager::instance().processCombatRound();
    }, true);
    
    TimerSystem::enableTimer("I-FIGHT");
    
    printLine("Combat begins!");
}

void CombatManager::endCombat() {
    if (!inCombat_) {
        return;
    }
    
    // Disable the I-FIGHT timer
    TimerSystem::disableTimer("I-FIGHT");
    
    // Clear combat state
    player_.reset();
    enemy_.reset();
    inCombat_ = false;
    
    printLine("Combat ends.");
}

bool CombatManager::isInCombat() const {
    return inCombat_;
}

ZObject* CombatManager::getCurrentEnemy() const {
    if (enemy_.has_value()) {
        return enemy_->object;
    }
    return nullptr;
}

void CombatManager::processCombatRound() {
    if (!inCombat_ || !player_.has_value() || !enemy_.has_value()) {
        endCombat();
        return;
    }
    
    // Check if combatants are still alive
    if (!player_->isAlive()) {
        // Player died in combat - use death system (Requirement 58.4)
        DeathSystem::jigsUp("You have been defeated in combat!", DeathSystem::DeathCause::COMBAT);
        endCombat();
        return;
    }
    
    if (!enemy_->isAlive()) {
        printLine("The " + enemy_->object->getDesc() + " has been defeated!");
        handleDeath(*enemy_);
        endCombat();
        return;
    }
    
    // Player attacks enemy
    if (attackSucceeds(*player_, *enemy_)) {
        int damage = calculateDamage(*player_, *enemy_);
        applyDamage(*enemy_, damage);
        
        std::string weaponName = player_->weapon ? player_->weapon->getDesc() : "fists";
        printLine("You strike the " + enemy_->object->getDesc() + " with your " + weaponName + "!");
        
        if (damage > 0) {
            printLine("You deal " + std::to_string(damage) + " damage!");
        }
        
        // Check if enemy died
        if (!enemy_->isAlive()) {
            printLine("The " + enemy_->object->getDesc() + " has been defeated!");
            handleDeath(*enemy_);
            endCombat();
            return;
        }
    } else {
        printLine("You swing at the " + enemy_->object->getDesc() + " but miss!");
    }
    
    // Check if enemy should flee
    if (shouldEnemyFlee(*enemy_)) {
        handleEnemyFlee();
        endCombat();
        return;
    }
    
    // Enemy attacks player
    if (attackSucceeds(*enemy_, *player_)) {
        int damage = calculateDamage(*enemy_, *player_);
        applyDamage(*player_, damage);
        
        printLine("The " + enemy_->object->getDesc() + " counterattacks!");
        
        if (damage > 0) {
            printLine("You take " + std::to_string(damage) + " damage!");
        }
        
        // Check if player died
        if (!player_->isAlive()) {
            // Player died in combat - use death system (Requirement 58.4)
            DeathSystem::jigsUp("You have been defeated in combat!", DeathSystem::DeathCause::COMBAT);
            endCombat();
            return;
        }
    } else {
        printLine("The " + enemy_->object->getDesc() + " attacks but misses!");
    }
}

int CombatManager::calculateDamage(const Combatant& attacker, const Combatant& defender) {
    // Base damage from attacker strength
    // Stronger enemies deal more damage
    int baseDamage = attacker.strength;
    
    // Add weapon effectiveness
    if (attacker.weapon) {
        // Weapon has STRENGTH property indicating effectiveness
        int weaponStrength = attacker.weapon->getProperty(P_STRENGTH);
        if (weaponStrength > 0) {
            baseDamage += weaponStrength * 2;
        }
    } else {
        // Bare hands - low effectiveness
        baseDamage += 1;
    }
    
    // Add some randomness (±30%)
    int variance = baseDamage / 3;
    if (variance < 1) {
        variance = 1;
    }
    int randomFactor = (rand() % (variance * 2 + 1)) - variance;
    int damage = baseDamage + randomFactor;
    
    // Ensure minimum damage of 1
    if (damage < 1) {
        damage = 1;
    }
    
    return damage;
}

bool CombatManager::attackSucceeds(const Combatant& attacker, const Combatant& defender) {
    // Calculate attack power based on strength and weapon
    int attackPower = attacker.strength;
    
    if (attacker.weapon) {
        // Add weapon effectiveness
        int weaponStrength = attacker.weapon->getProperty(P_STRENGTH);
        if (weaponStrength > 0) {
            attackPower += weaponStrength;
        }
    } else {
        // Bare hands - low effectiveness (2 points)
        attackPower += 2;
    }
    
    int defensePower = defender.strength;
    
    // Base hit chance is 60%
    int hitChance = 60;
    
    // Modify based on power difference
    int powerDiff = attackPower - defensePower;
    hitChance += powerDiff * 5;  // ±5% per point difference
    
    // Clamp between 20% and 95%
    if (hitChance < 20) {
        hitChance = 20;
    }
    if (hitChance > 95) {
        hitChance = 95;
    }
    
    // Roll for hit
    int roll = rand() % 100;
    return roll < hitChance;
}

void CombatManager::applyDamage(Combatant& target, int damage) {
    target.health -= damage;
    if (target.health < 0) {
        target.health = 0;
    }
}

bool CombatManager::shouldEnemyFlee(const Combatant& enemy) {
    // Enemy flees if badly wounded (below 30% health)
    // Based on ZIL WINNING? function - enemies flee when losing badly
    if (enemy.health < enemy.maxHealth * 3 / 10) {
        // Higher chance to flee when more wounded
        int fleeChance = 40 + ((enemy.maxHealth - enemy.health) * 20 / enemy.maxHealth);
        return (rand() % 100) < fleeChance;
    }
    return false;
}

void CombatManager::handleEnemyFlee() {
    if (!enemy_.has_value()) {
        return;
    }
    
    // Enemy flees - based on ZIL thief/troll behavior
    printLine("Your opponent, determining discretion to be the better part of valor,");
    printLine("decides to terminate this little contretemps. With a rueful nod,");
    printLine("the " + enemy_->object->getDesc() + " steps backward into the gloom and disappears.");
    
    // Clear FIGHTBIT so enemy won't attack again immediately
    enemy_->object->clearFlag(ObjectFlag::FIGHTBIT);
    
    // Make enemy invisible (fled from combat)
    // In full implementation, would move to adjacent room
    enemy_->object->setFlag(ObjectFlag::INVISIBLE);
}

void CombatManager::handleDeath(Combatant& combatant) {
    if (!combatant.object) {
        return;
    }
    
    auto& g = Globals::instance();
    
    // Mark as dead
    combatant.object->setFlag(ObjectFlag::DEADBIT);
    
    // Remove FIGHTBIT so it can't be attacked again
    combatant.object->clearFlag(ObjectFlag::FIGHTBIT);
    
    // Drop all carried items to current location
    auto* location = combatant.object->getLocation();
    if (!location) {
        location = g.here;
    }
    
    auto contents = combatant.object->getContents();
    for (auto* item : contents) {
        if (location) {
            item->moveTo(location);
            // Make items visible and accessible
            item->clearFlag(ObjectFlag::NDESCBIT);
            if (item->hasFlag(ObjectFlag::WEAPONBIT)) {
                // Weapons become available again
                printLine("The " + item->getDesc() + " falls to the ground.");
            }
        }
    }
    
    // Based on ZIL: enemy disappears in sinister fog
    if (combatant.object != g.winner) {
        printLine("Almost as soon as the " + combatant.object->getDesc() + 
                  " breathes his last breath, a cloud of sinister black fog envelops him,");
        printLine("and when the fog lifts, the carcass has disappeared.");
        
        // Remove enemy from game
        combatant.object->setFlag(ObjectFlag::INVISIBLE);
    }
}

} // namespace CombatSystem
