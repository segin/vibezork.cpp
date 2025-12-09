#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "../src/parser/parser.h"
#include "../src/systems/score.h"
#include "../src/verbs/verbs.h"

// Helper to reset game state for tests
void resetGame() {
    auto& g = Globals::instance();
    initializeWorld(); // Initialize all objects
    // For now we just move player to a known location
    g.here = g.getObject(RoomIds::LIVING_ROOM);
    g.prso = nullptr;
    g.prsi = nullptr;
}

// Helper to execute commands
void executeCommand(const std::string& command) {
    auto& p = getGlobalParser();
    ParsedCommand cmd = p.parse(command);
    // std::cout << "Debug: executeCommand parse result: verb=" << cmd.verb << " words[0]=" << (cmd.words.empty()?"":cmd.words[0]) << std::endl;
    
    // Update Globals
    auto& g = Globals::instance();
    g.prsa = cmd.verb;
    g.prso = cmd.directObj;
    g.prsi = cmd.indirectObj;
    
    // Dispatch (Simplified for test)
    if (cmd.verb == V_OPEN) Verbs::vOpen();
    else if (cmd.verb == V_WIND) Verbs::vWind();
    else if (cmd.verb == V_RAISE) Verbs::vRaise();
    else if (cmd.verb == V_LOWER) Verbs::vLower();
    else if (cmd.verb == V_TAKE) Verbs::vTake();
    // Add others if needed
}

// Print helper
void printLine(const std::string& msg) {
    std::cout << msg << std::endl;
}

// Test 1: Egg Puzzle Logic (Breaking the egg)
bool testEggPuzzle() {
    printLine("Testing Egg Puzzle...");
    auto& g = Globals::instance();
    resetGame();
    
    // Setup: Player has Egg, Axe (as tool/weapon)
    ZObject* player = g.getObject(ObjectIds::ADVENTURER);
    ZObject* egg = g.getObject(ObjectIds::EGG);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* brokenEgg = g.getObject(ObjectIds::BROKEN_EGG);
    ZObject* brokenCanary = g.getObject(ObjectIds::BROKEN_CANARY);
    
    if (!egg || !player || !axe || !brokenEgg) return false;
    
    // Move egg and axe to player
    egg->moveTo(player);
    axe->moveTo(player);
    
    // 1. Try to open egg with hands (should fail/warn)
    executeCommand("open egg"); 
    
    // Verify egg is NOT open/broken yet
    if (egg->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("✗ Egg opened without tool!");
        return false;
    }
    
    // 2. Open egg with Axe -> Should break it
    executeCommand("open egg with axe");
    
    // Check results
    // Logic: brokenEgg replaces egg at player location
    if (brokenEgg->getLocation() == player) {
         printLine("✓ Egg broken correctly with axe");
         
         // Helper check: Canary logic
         if (brokenCanary && brokenCanary->getLocation() == player) {
             printLine("✓ Broken canary appeared");
         }
         return true;
    } else {
         printLine("✗ Egg failed to break with axe");
         printLine("  Egg Loc: " + (egg->getLocation() ? egg->getLocation()->getDesc() : "null"));
         printLine("  Broken Egg Loc: " + (brokenEgg->getLocation() ? brokenEgg->getLocation()->getDesc() : "null"));
         return false;
    }
}

// Test 2: Canary Puzzle
bool testCanaryPuzzle() {
    printLine("\nTesting Canary Puzzle...");
    auto& g = Globals::instance();
    resetGame();
    
    // Setup: Adventurer has canary. In Forest 1.
    ZObject* canary = g.getObject(ObjectIds::CANARY);
    ZObject* bauble = g.getObject(ObjectIds::BAUBLE);
    
    if (!canary || !bauble) {
        printLine("Error: Missing objects for Canary Test");
        return false;
    }
    
    auto forest1Obj = g.getObject(RoomIds::FOREST_1);
    g.player->moveTo(forest1Obj);
    g.here = forest1Obj;
    canary->moveTo(g.player); // Holding canary
    
    // Wind canary (Should drop bauble)
    executeCommand("wind canary");
    
    // Verify
    bool baubleDropped = (bauble->getLocation() == g.player || bauble->getLocation() == g.here);
    if (baubleDropped) {
         printLine("✓ Canary dropped bauble");
         return true;
    } else {
         printLine("✗ Canary failed to drop bauble");
         printLine("  Bauble Loc: " + (bauble->getLocation() ? bauble->getLocation()->getDesc() : "null"));
         return false;
    }
}

// Test 3: Basket Puzzle (Shaft)
bool testBasketPuzzle() {
    printLine("\nTesting Basket Puzzle...");
    auto& g = Globals::instance();
    resetGame();
    
    // Setup
    ZObject* basket = g.getObject(ObjectIds::BASKET); // 302
    ZObject* raisedBasket = g.getObject(ObjectIds::RAISED_BASKET); // 303
    ZObject* shaftRoom = g.getObject(RoomIds::SHAFT_ROOM);
    ZObject* lowerShaft = g.getObject(RoomIds::LOWER_SHAFT);
    
    if (!basket || !raisedBasket || !shaftRoom || !lowerShaft) {
        printLine("Error: Missing objects/rooms for Basket Test");
        return false;
    }
    
    // Start at Shaft Room (Top)
    auto shaftRoomObj = g.getObject(RoomIds::SHAFT_ROOM);
    g.player->moveTo(shaftRoomObj);
    g.here = shaftRoomObj;
    
    // Ensure basket is at top (RAISED_BASKET should be visible)
    // Note: Depends on world_init. Assuming initialized correctly.
    // If not, we forcibly set it for test.
    raisedBasket->moveTo(shaftRoom);
    basket->moveTo(nullptr); // Basket valid only at bottom usually?
    // Or logic handles IDs.
    
    // Lower basket
    executeCommand("lower basket");
    
    // Verify:
    // 1. Raised basket logic in actions.cpp actually moves raisedBasket to lowerShaft?
    // And loweredBasket to shaftRoom? (Code seems to swap them or move both?)
    // Line 165: if (raisedBasket && lowerShaft) raisedBasket->moveTo(lowerShaft);
    // So raisedBasket (303) should be at LOWER_SHAFT.
    
    if (raisedBasket->getLocation() == lowerShaft) {
        printLine("✓ Basket lowered to Lower Shaft (RaisedBasket obj)");
    } else if (basket->getLocation() == lowerShaft) {
        printLine("✓ Basket lowered to Lower Shaft (Basket obj)");
    } else {
        printLine("✗ Basket not found in Lower Shaft");
        // Debug
        printLine("  RaisedBasket Loc: " + (raisedBasket->getLocation() ? raisedBasket->getLocation()->getDesc() : "null"));
        printLine("  Basket Loc: " + (basket->getLocation() ? basket->getLocation()->getDesc() : "null"));
        return false;
    }
    
    // Test Raising
    auto lowerShaftObj = g.getObject(RoomIds::LOWER_SHAFT);
    g.player->moveTo(lowerShaftObj);
    g.here = lowerShaftObj;
    executeCommand("raise basket");
    
    if (raisedBasket->getLocation() == shaftRoom) {
        printLine("✓ Basket raised to Shaft Room");
    } else {
        printLine("✗ Basket failed to raise");
        return false;
    }

    return true;
}

int main() {
    bool allPassed = true;
    allPassed &= testEggPuzzle();
    allPassed &= testCanaryPuzzle();
    allPassed &= testBasketPuzzle();
    
    if (allPassed) {
        std::cout << "All Critical Puzzle Tests Passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}
