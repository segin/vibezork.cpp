#include "combat.h"
#include "timer.h"
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
    int enemyHealth = enemy->getProperty(P_STRENGTH);
    if (enemyHealth == 0) {
        enemyHealth = 10;  // Default enemy health
    }
    enemy_ = Combatant(enemy, enemyHealth, nullptr);
    
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
        printLine("You have been defeated!");
        handleDeath(*player_);
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
            printLine("You have been defeated!");
            handleDeath(*player_);
            endCombat();
            return;
        }
    } else {
        printLine("The " + enemy_->object->getDesc() + " attacks but misses!");
    }
}

int CombatManager::calculateDamage(const Combatant& attacker, const Combatant& defender) {
    // Base damage calculation
    int baseDamage = attacker.strength / 2;
    
    // Add weapon strength if present
    if (attacker.weapon) {
        int weaponStrength = attacker.weapon->getProperty(P_STRENGTH);
        if (weaponStrength > 0) {
            baseDamage += weaponStrength;
        }
    }
    
    // Add some randomness (±25%)
    int variance = baseDamage / 4;
    int randomFactor = (rand() % (variance * 2 + 1)) - variance;
    int damage = baseDamage + randomFactor;
    
    // Ensure minimum damage of 1
    if (damage < 1) {
        damage = 1;
    }
    
    return damage;
}

bool CombatManager::attackSucceeds(const Combatant& attacker, const Combatant& defender) {
    // Calculate hit chance based on weapon effectiveness
    int attackPower = attacker.strength;
    
    if (attacker.weapon) {
        int weaponStrength = attacker.weapon->getProperty(P_STRENGTH);
        if (weaponStrength > 0) {
            attackPower += weaponStrength;
        }
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
    // Enemy flees if badly wounded (below 25% health)
    if (enemy.health < enemy.maxHealth / 4) {
        // 50% chance to flee each round when badly wounded
        return (rand() % 100) < 50;
    }
    return false;
}

void CombatManager::handleEnemyFlee() {
    if (!enemy_.has_value()) {
        return;
    }
    
    printLine("The " + enemy_->object->getDesc() + " flees in terror!");
    
    // For now, just mark the enemy as having fled
    // In a full implementation, we would move the enemy to an adjacent room
    // but that requires more complex room navigation logic
    enemy_->object->setFlag(ObjectFlag::INVISIBLE);
    printLine("The " + enemy_->object->getDesc() + " escapes into the shadows!");
}

void CombatManager::handleDeath(Combatant& combatant) {
    if (!combatant.object) {
        return;
    }
    
    // Mark as dead
    combatant.object->setFlag(ObjectFlag::DEADBIT);
    
    // Remove FIGHTBIT so it can't be attacked again
    combatant.object->clearFlag(ObjectFlag::FIGHTBIT);
    
    // Drop all carried items
    auto contents = combatant.object->getContents();
    for (auto* item : contents) {
        item->moveTo(combatant.object->getLocation());
    }
}

} // namespace CombatSystem
