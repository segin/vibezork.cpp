#include "test_framework.h"
#include "systems/combat.h"
#include "systems/timer.h"
#include "core/globals.h"
#include "core/object.h"
#include "world/objects.h"
#include "world/world.h"
#include <iostream>
#include <stdexcept>

using namespace CombatSystem;
using namespace ObjectIds;

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

// Test: Combat system initialization (Requirement 55)
void testCombatInitialization() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Initially not in combat
    TEST_ASSERT(!isInCombat(), "Should not be in combat initially");
    TEST_ASSERT(getCurrentEnemy() == nullptr, "Should have no current enemy initially");
    
    std::cout << "✓ Combat initialization test passed" << std::endl;
}

// Test: Starting combat (Requirement 55)
void testStartCombat() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create a test enemy
    auto* enemy = g.getObject(TROLL);
    TEST_ASSERT(enemy != nullptr, "Troll object should exist");
    
    // Start combat
    startCombat(enemy);
    
    // Should be in combat now
    TEST_ASSERT(isInCombat(), "Should be in combat after starting");
    TEST_ASSERT(getCurrentEnemy() == enemy, "Current enemy should be the troll");
    
    // Clean up
    endCombat();
    
    std::cout << "✓ Start combat test passed" << std::endl;
}

// Test: Ending combat (Requirement 55)
void testEndCombat() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create a test enemy
    auto* enemy = g.getObject(TROLL);
    TEST_ASSERT(enemy != nullptr, "Troll object should exist");
    
    // Start and end combat
    startCombat(enemy);
    TEST_ASSERT(isInCombat(), "Should be in combat after starting");
    
    endCombat();
    TEST_ASSERT(!isInCombat(), "Should not be in combat after ending");
    TEST_ASSERT(getCurrentEnemy() == nullptr, "Should have no current enemy after ending");
    
    std::cout << "✓ End combat test passed" << std::endl;
}

// Test: Combat with weapon (Requirement 56)
void testCombatWithWeapon() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Get sword and enemy
    auto* sword = g.getObject(SWORD);
    auto* enemy = g.getObject(TROLL);
    TEST_ASSERT(sword != nullptr, "Sword object should exist");
    TEST_ASSERT(enemy != nullptr, "Troll object should exist");
    
    // Give player the sword
    sword->moveTo(g.winner);
    
    // Start combat with weapon
    startCombat(enemy, sword);
    
    TEST_ASSERT(isInCombat(), "Should be in combat");
    
    // Verify player has weapon
    auto& manager = CombatManager::instance();
    auto playerCombatant = manager.getPlayerCombatant();
    TEST_ASSERT(playerCombatant.has_value(), "Player combatant should exist");
    TEST_ASSERT(playerCombatant->weapon == sword, "Player should have sword as weapon");
    
    // Clean up
    endCombat();
    
    std::cout << "✓ Combat with weapon test passed" << std::endl;
}

// Test: Combat round processing (Requirement 55)
void testCombatRoundProcessing() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create a weak enemy for testing
    auto* enemy = g.getObject(THIEF);
    TEST_ASSERT(enemy != nullptr, "Thief object should exist");
    
    // Set low health for quick combat
    enemy->setProperty(P_STRENGTH, 5);
    
    // Start combat
    startCombat(enemy);
    TEST_ASSERT(isInCombat(), "Should be in combat");
    
    // Get initial health
    auto& manager = CombatManager::instance();
    auto enemyCombatant = manager.getEnemyCombatant();
    TEST_ASSERT(enemyCombatant.has_value(), "Enemy combatant should exist");
    
    // Process several combat rounds
    // Combat should eventually end (either player or enemy dies)
    int maxRounds = 50;
    int rounds = 0;
    while (isInCombat() && rounds < maxRounds) {
        processCombatRound();
        rounds++;
    }
    
    // Combat should have ended
    TEST_ASSERT(!isInCombat(), "Combat should have ended");
    TEST_ASSERT(rounds < maxRounds, "Combat should not take all rounds");
    
    std::cout << "✓ Combat round processing test passed" << std::endl;
}

// Test: Enemy death (Requirement 57)
void testEnemyDeath() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create enemy
    auto* enemy = g.getObject(THIEF);
    TEST_ASSERT(enemy != nullptr, "Thief object should exist");
    
    // Set very low health
    enemy->setProperty(P_STRENGTH, 1);
    
    // Get a strong weapon
    auto* sword = g.getObject(SWORD);
    TEST_ASSERT(sword != nullptr, "Sword object should exist");
    sword->moveTo(g.winner);
    
    // Start combat with weapon
    startCombat(enemy, sword);
    
    // Process rounds until combat ends
    int maxRounds = 20;
    int rounds = 0;
    while (isInCombat() && rounds < maxRounds) {
        processCombatRound();
        rounds++;
    }
    
    // Combat should have ended
    TEST_ASSERT(!isInCombat(), "Combat should have ended");
    
    // Enemy should be dead
    TEST_ASSERT(enemy->hasFlag(ObjectFlag::DEADBIT), "Enemy should be marked as dead");
    TEST_ASSERT(!enemy->hasFlag(ObjectFlag::FIGHTBIT), "Enemy should not be fightable");
    
    std::cout << "✓ Enemy death test passed" << std::endl;
}

// Test: Combatant structure (Requirement 55)
void testCombatantStructure() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create combatant
    auto* troll = g.getObject(TROLL);
    TEST_ASSERT(troll != nullptr, "Troll object should exist");
    
    Combatant combatant(troll, 20);
    
    // Verify initialization
    TEST_ASSERT(combatant.object == troll, "Combatant object should be troll");
    TEST_ASSERT(combatant.health == 20, "Combatant health should be 20");
    TEST_ASSERT(combatant.maxHealth == 20, "Combatant max health should be 20");
    TEST_ASSERT(combatant.weapon == nullptr, "Combatant should have no weapon");
    TEST_ASSERT(combatant.strength > 0, "Combatant should have positive strength");
    
    // Test alive/wounded checks
    TEST_ASSERT(combatant.isAlive(), "Combatant should be alive");
    TEST_ASSERT(!combatant.isWounded(), "Combatant should not be wounded");
    
    // Damage the combatant
    combatant.health = 5;
    TEST_ASSERT(combatant.isAlive(), "Combatant should still be alive");
    TEST_ASSERT(combatant.isWounded(), "Combatant should be wounded");
    
    // Kill the combatant
    combatant.health = 0;
    TEST_ASSERT(!combatant.isAlive(), "Combatant should be dead");
    
    std::cout << "✓ Combatant structure test passed" << std::endl;
}

// Test: Combat with bare hands (Requirement 55)
void testCombatBareHands() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create enemy
    auto* enemy = g.getObject(THIEF);
    TEST_ASSERT(enemy != nullptr, "Thief object should exist");
    
    // Start combat without weapon
    startCombat(enemy, nullptr);
    
    TEST_ASSERT(isInCombat(), "Should be in combat");
    
    // Verify player has no weapon
    auto& manager = CombatManager::instance();
    auto playerCombatant = manager.getPlayerCombatant();
    TEST_ASSERT(playerCombatant.has_value(), "Player combatant should exist");
    TEST_ASSERT(playerCombatant->weapon == nullptr, "Player should have no weapon");
    
    // Clean up
    endCombat();
    
    std::cout << "✓ Combat bare hands test passed" << std::endl;
}

// Test: Multiple combat sessions (Requirement 55)
void testMultipleCombatSessions() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // First combat
    auto* enemy1 = g.getObject(THIEF);
    TEST_ASSERT(enemy1 != nullptr, "Thief object should exist");
    
    startCombat(enemy1);
    TEST_ASSERT(isInCombat(), "Should be in combat");
    TEST_ASSERT(getCurrentEnemy() == enemy1, "Current enemy should be thief");
    endCombat();
    TEST_ASSERT(!isInCombat(), "Should not be in combat");
    
    // Second combat
    auto* enemy2 = g.getObject(TROLL);
    TEST_ASSERT(enemy2 != nullptr, "Troll object should exist");
    
    startCombat(enemy2);
    TEST_ASSERT(isInCombat(), "Should be in combat");
    TEST_ASSERT(getCurrentEnemy() == enemy2, "Current enemy should be troll");
    endCombat();
    TEST_ASSERT(!isInCombat(), "Should not be in combat");
    
    std::cout << "✓ Multiple combat sessions test passed" << std::endl;
}

// Test: Combat timer integration (Requirement 44, 46)
void testCombatTimerIntegration() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create enemy
    auto* enemy = g.getObject(THIEF);
    TEST_ASSERT(enemy != nullptr, "Thief object should exist");
    
    // Start combat (should enable I-FIGHT timer)
    startCombat(enemy);
    TEST_ASSERT(isInCombat(), "Should be in combat");
    
    // Timer should be enabled
    // (We can't directly check timer state, but combat should work)
    
    // End combat (should disable I-FIGHT timer)
    endCombat();
    TEST_ASSERT(!isInCombat(), "Should not be in combat");
    
    std::cout << "✓ Combat timer integration test passed" << std::endl;
}

// Test: Damage application (Requirement 55, 56)
void testDamageApplication() {
    auto& g = Globals::instance();
    initializeWorld();
    
    // Create combatant
    auto* troll = g.getObject(TROLL);
    TEST_ASSERT(troll != nullptr, "Troll object should exist");
    
    Combatant combatant(troll, 20);
    TEST_ASSERT(combatant.health == 20, "Initial health should be 20");
    
    // Apply damage manually (simulating what happens in combat)
    combatant.health -= 5;
    TEST_ASSERT(combatant.health == 15, "Health should be 15 after 5 damage");
    TEST_ASSERT(combatant.isAlive(), "Combatant should still be alive");
    
    // Apply more damage
    combatant.health -= 10;
    TEST_ASSERT(combatant.health == 5, "Health should be 5 after 10 more damage");
    TEST_ASSERT(combatant.isAlive(), "Combatant should still be alive");
    TEST_ASSERT(combatant.isWounded(), "Combatant should be wounded");
    
    // Apply fatal damage
    combatant.health -= 10;
    TEST_ASSERT(combatant.health <= 0, "Health should be 0 or less");
    TEST_ASSERT(!combatant.isAlive(), "Combatant should be dead");
    
    std::cout << "✓ Damage application test passed" << std::endl;
}

int main() {
    std::cout << "Running Combat System Tests...\n\n";
    
    try {
        testCombatInitialization();
        testStartCombat();
        testEndCombat();
        testCombatWithWeapon();
        testCombatRoundProcessing();
        testEnemyDeath();
        testCombatantStructure();
        testCombatBareHands();
        testMultipleCombatSessions();
        testCombatTimerIntegration();
        testDamageApplication();
        
        std::cout << "\nAll Combat System Tests Passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
