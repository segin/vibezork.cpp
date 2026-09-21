#include "test_framework.h"
#include "core/go.h"
#include "systems/sword.h"
#include "systems/timer.h"
#include "core/globals.h"
#include "world/world.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <iostream>
#include <stdexcept>

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

// Test sword glow initialization
void testSwordGlowInitialization() {
    // Initialize the world and sword system
    initializeWorld();
    goSetup();
    SwordSystem::initialize();
    
    // ZIL: GO queues I-SWORD but does NOT enable it (<QUEUE I-SWORD -1>,
    // 1dungeon.zil:2639); SWORD-FCN enables it when the sword is taken
    TEST_ASSERT(TimerSystem::TimerManager::instance().find("I-SWORD") != nullptr,
                "I-SWORD interrupt should be registered after initialization");
    TEST_ASSERT(!TimerSystem::isEnabled("I-SWORD"),
                "I-SWORD interrupt starts disabled");
    
    std::cout << "✓ Sword glow initialization test passed" << std::endl;
}

// Test enemy proximity detection in current room
void testEnemyProximityCurrentRoom() {
    auto& g = Globals::instance();
    
    // Initialize the world
    initializeWorld();
    goSetup();
    SwordSystem::initialize();
    
    // Get the sword and a test room
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    ZObject* testRoom = g.getObject(RoomIds::WEST_OF_HOUSE);
    
    TEST_ASSERT(sword != nullptr, "Sword should exist");
    TEST_ASSERT(testRoom != nullptr, "Test room should exist");
    
    // Move player and sword to test room
    g.here = testRoom;
    g.winner = g.getObject(ObjectIds::ADVENTURER);
    sword->moveTo(g.winner);  // Player has sword
    
    // Initially no enemies, sword should not glow
    TEST_ASSERT(!SwordSystem::areEnemiesNearby(), 
                "Should detect no enemies initially");
    TEST_ASSERT(!sword->hasFlag(ObjectFlag::ONBIT), 
                "Sword should not glow without enemies");
    
    // Create a test enemy in the room
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    if (troll) {
        troll->moveTo(testRoom);
        troll->setFlag(ObjectFlag::FIGHTBIT);
        
        // Now there should be an enemy nearby
        TEST_ASSERT(SwordSystem::areEnemiesNearby(), 
                    "Should detect enemy in current room");
        
        // Update sword glow
        SwordSystem::updateSwordGlow();
        
        // Sword should now be glowing
        TEST_ASSERT(sword->hasFlag(ObjectFlag::ONBIT), 
                    "Sword should glow when enemy is present");
        
        // Remove enemy
        troll->moveTo(nullptr);
        
        // Enemy should no longer be nearby
        TEST_ASSERT(!SwordSystem::areEnemiesNearby(), 
                    "Should detect no enemies after removal");
        
        // Update sword glow
        SwordSystem::updateSwordGlow();
        
        // Sword should stop glowing
        TEST_ASSERT(!sword->hasFlag(ObjectFlag::ONBIT), 
                    "Sword should stop glowing when enemy leaves");
    }
    
    std::cout << "✓ Enemy proximity detection test passed" << std::endl;
}

// Test that dead enemies don't trigger glow
void testDeadEnemyNoGlow() {
    auto& g = Globals::instance();
    
    // Initialize the world
    initializeWorld();
    goSetup();
    SwordSystem::initialize();
    
    // Get the sword and a test room
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    ZObject* testRoom = g.getObject(RoomIds::WEST_OF_HOUSE);
    
    // Move player and sword to test room
    g.here = testRoom;
    g.winner = g.getObject(ObjectIds::ADVENTURER);
    sword->moveTo(g.winner);
    
    // A dead villain is REMOVEd from the game in ZIL (VILLAIN-RESULT);
    // nothing with FIGHTBIT remains in the room
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    if (troll) {
        troll->setFlag(ObjectFlag::FIGHTBIT);
        troll->moveTo(nullptr);  // Dead: removed
        
        // Dead enemy should not trigger glow
        TEST_ASSERT(!SwordSystem::areEnemiesNearby(), 
                    "Dead enemies should not be detected");
        
        SwordSystem::updateSwordGlow();
        
        TEST_ASSERT(!sword->hasFlag(ObjectFlag::ONBIT), 
                    "Sword should not glow for dead enemies");
    }
    
    std::cout << "✓ Dead enemy no glow test passed" << std::endl;
}

// Test sword glow timer callback
void testSwordGlowTimerCallback() {
    auto& g = Globals::instance();
    
    // Initialize the world
    initializeWorld();
    goSetup();
    SwordSystem::initialize();
    
    // Get the sword and a test room
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    ZObject* testRoom = g.getObject(RoomIds::WEST_OF_HOUSE);
    
    // Move player and sword to test room
    g.here = testRoom;
    g.winner = g.getObject(ObjectIds::ADVENTURER);
    sword->moveTo(g.winner);
    
    // Add an enemy
    ZObject* thief = g.getObject(ObjectIds::THIEF);
    if (thief) {
        thief->moveTo(testRoom);
        thief->setFlag(ObjectFlag::FIGHTBIT);
        
        // Call the timer callback directly
        SwordSystem::swordTimerCallback();
        
        // Sword should be glowing
        TEST_ASSERT(sword->hasFlag(ObjectFlag::ONBIT), 
                    "Sword should glow after timer callback with enemy present");
        
        // Remove enemy
        thief->moveTo(nullptr);
        
        // Call timer callback again
        SwordSystem::swordTimerCallback();
        
        // Sword should stop glowing
        TEST_ASSERT(!sword->hasFlag(ObjectFlag::ONBIT), 
                    "Sword should stop glowing after timer callback with no enemy");
    }
    
    std::cout << "✓ Sword glow timer callback test passed" << std::endl;
}

int main() {
    std::cout << "Running Sword Glow System Tests..." << std::endl;
    std::cout << "===================================" << std::endl;
    
    try {
        testSwordGlowInitialization();
        testEnemyProximityCurrentRoom();
        testDeadEnemyNoGlow();
        testSwordGlowTimerCallback();
        
        std::cout << std::endl;
        std::cout << "All sword glow tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
