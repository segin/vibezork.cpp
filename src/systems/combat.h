#pragma once
#include "core/types.h"
#include "core/object.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <optional>

// Combat System - handles turn-based combat
// Based on combat mechanics from the original Zork I
//
// Combat in Zork is turn-based:
// - Player attacks enemy
// - Enemy attacks player
// - Combat continues until one combatant dies or flees
//
// The I-FIGHT timer processes one combat round per turn

namespace CombatSystem {

// Combatant structure
// Tracks health, weapon, and strength for a combatant
struct Combatant {
    ZObject* object;        // The combatant object (player or NPC)
    int health;             // Current health points
    int maxHealth;          // Maximum health points
    ZObject* weapon;        // Equipped weapon (nullptr for bare hands)
    int strength;           // Base strength value
    
    Combatant() 
        : object(nullptr), health(0), maxHealth(0), weapon(nullptr), strength(0) {}
    
    Combatant(ZObject* obj, int hp, ZObject* wpn = nullptr)
        : object(obj), health(hp), maxHealth(hp), weapon(wpn), strength(0) {
        if (obj) {
            strength = obj->getProperty(P_STRENGTH);
            if (strength == 0) {
                strength = 5;  // Default strength
            }
        }
    }
    
    bool isAlive() const { return health > 0; }
    bool isWounded() const { return health < maxHealth / 2; }
};

// Combat System class
// Manages combat state and processes combat rounds
class CombatManager {
public:
    // Get singleton instance
    static CombatManager& instance();
    
    // Start combat between player and enemy
    // Registers the I-FIGHT timer to process combat rounds
    void startCombat(ZObject* enemy, ZObject* weapon = nullptr);
    
    // End combat
    // Disables the I-FIGHT timer
    void endCombat();
    
    // Check if currently in combat
    bool isInCombat() const;
    
    // Get current enemy
    ZObject* getCurrentEnemy() const;
    
    // Process one round of combat
    // Called by the I-FIGHT timer each turn
    // - Player attacks enemy
    // - Enemy attacks player
    // - Check for death or fleeing
    void processCombatRound();
    
    // Get player combatant (for health tracking)
    const std::optional<Combatant>& getPlayerCombatant() const { return player_; }
    
    // Get enemy combatant (for health tracking)
    const std::optional<Combatant>& getEnemyCombatant() const { return enemy_; }
    
private:
    CombatManager() = default;
    CombatManager(const CombatManager&) = delete;
    CombatManager& operator=(const CombatManager&) = delete;
    
    // Calculate damage for an attack
    // Factors in weapon strength and attacker strength
    int calculateDamage(const Combatant& attacker, const Combatant& defender);
    
    // Check if an attack succeeds
    // Based on weapon effectiveness and random chance
    bool attackSucceeds(const Combatant& attacker, const Combatant& defender);
    
    // Apply damage to a combatant
    void applyDamage(Combatant& target, int damage);
    
    // Check if enemy should flee
    bool shouldEnemyFlee(const Combatant& enemy);
    
    // Handle enemy fleeing
    void handleEnemyFlee();
    
    // Handle combatant death
    void handleDeath(Combatant& combatant);
    
    // Combat state
    std::optional<Combatant> player_;
    std::optional<Combatant> enemy_;
    bool inCombat_ = false;
};

// Convenience functions

// Start combat
inline void startCombat(ZObject* enemy, ZObject* weapon = nullptr) {
    CombatManager::instance().startCombat(enemy, weapon);
}

// End combat
inline void endCombat() {
    CombatManager::instance().endCombat();
}

// Check if in combat
inline bool isInCombat() {
    return CombatManager::instance().isInCombat();
}

// Get current enemy
inline ZObject* getCurrentEnemy() {
    return CombatManager::instance().getCurrentEnemy();
}

// Process combat round
inline void processCombatRound() {
    CombatManager::instance().processCombatRound();
}

} // namespace CombatSystem
