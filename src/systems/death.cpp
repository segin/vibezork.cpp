#include "death.h"
#include "score.h"
#include "timer.h"
#include "../core/globals.h"
#include "../core/io.h"
#include "../world/rooms.h"
#include "../world/objects.h"
#include <cstdlib>
#include <vector>

namespace DeathSystem {

// Static state
static int deathCount_ = 0;
static bool dead_ = false;
static bool alwaysLit_ = false;  // After resurrection, player can see in dark
static bool testMode_ = false;   // Disable interactive prompts for testing

// Initialize death system
void initialize() {
    deathCount_ = 0;
    dead_ = false;
    alwaysLit_ = false;
    testMode_ = false;
}

// Set test mode (disables interactive prompts for testing)
void setTestMode(bool enabled) {
    testMode_ = enabled;
}

// Get death count (Requirement 58.5)
int getDeathCount() {
    return deathCount_;
}

// Check if player is dead
bool isDead() {
    return dead_;
}

// Check if resurrection is available (Requirement 59.2)
bool canResurrect() {
    // Based on ZIL: resurrection available if deaths < 2
    // After 2 deaths, player is sent to Land of Living Dead permanently
    return deathCount_ < 2;
}

// Scatter player's inventory randomly (Requirement 59.4)
// Based on ZIL RANDOMIZE-OBJECTS routine
static void randomizeObjects() {
    auto& g = Globals::instance();
    
    if (!g.winner) {
        return;
    }
    
    // Special handling for lamp - always goes to living room
    auto* lamp = g.getObject(ObjectIds::LAMP);
    if (lamp && lamp->getLocation() == g.winner) {
        auto* livingRoom = g.getObject(RoomIds::LIVING_ROOM);
        if (livingRoom) {
            lamp->moveTo(livingRoom);
        }
    }
    
    // Special handling for coffin - always goes to Egypt room
    auto* coffin = g.getObject(ObjectIds::COFFIN);
    if (coffin && coffin->getLocation() == g.winner) {
        auto* egyptRoom = g.getObject(RoomIds::EGYPT_ROOM);
        if (egyptRoom) {
            coffin->moveTo(egyptRoom);
        }
    }
    
    // Collect all rooms that are on land and not lit
    // Rooms have IDs >= 1000 (see rooms.h)
    std::vector<ZObject*> validRooms;
    for (const auto& [id, obj] : g.getAllObjects()) {
        if (id >= 1000 &&  // Room IDs start at 1000
            obj->hasFlag(ObjectFlag::RLANDBIT) &&
            !obj->hasFlag(ObjectFlag::ONBIT)) {
            validRooms.push_back(obj.get());
        }
    }
    
    if (validRooms.empty()) {
        return;
    }
    
    // Scatter player's inventory
    auto contents = g.winner->getContents();
    for (auto* item : contents) {
        if (!item) continue;
        
        // Treasures go to random dark land rooms
        int tvalue = item->getProperty(P_TVALUE);
        if (tvalue > 0) {
            // Pick random dark room
            int idx = rand() % validRooms.size();
            item->moveTo(validRooms[idx]);
        } else {
            // Non-treasures go to random above-ground rooms
            // For simplicity, use any valid room
            int idx = rand() % validRooms.size();
            item->moveTo(validRooms[idx]);
        }
    }
}

// Kill all active timers
// Based on ZIL KILL-INTERRUPTS routine
static void killInterrupts() {
    // Disable all timers
    TimerSystem::disableTimer("I-THIEF");
    TimerSystem::disableTimer("I-TROLL");
    TimerSystem::disableTimer("I-CYCLOPS");
    TimerSystem::disableTimer("I-LANTERN");
    TimerSystem::disableTimer("I-CANDLES");
    TimerSystem::disableTimer("I-SWORD");
    TimerSystem::disableTimer("I-FOREST-ROOM");
    TimerSystem::disableTimer("I-MATCH");
    TimerSystem::disableTimer("I-FIGHT");
    
    // Turn off match if lit
    auto& g = Globals::instance();
    auto* match = g.getObject(ObjectIds::MATCH);
    if (match) {
        match->clearFlag(ObjectFlag::ONBIT);
    }
}

// Offer resurrection to player (Requirement 59.1)
// Returns true if player accepts, false if they decline
bool offerResurrection() {
    auto& g = Globals::instance();
    
    // In test mode, automatically accept resurrection
    if (testMode_) {
        return true;
    }
    
    // Check if player has visited South Temple (determines resurrection type)
    auto* southTemple = g.getObject(RoomIds::SOUTH_TEMPLE);
    bool visitedTemple = southTemple && southTemple->hasFlag(ObjectFlag::TOUCHBIT);
    
    if (visitedTemple) {
        // Offer full resurrection at Entrance to Hades
        printLine("As you take your last breath, you feel relieved of your burdens. The");
        printLine("feeling passes as you find yourself before the gates of Hell, where");
        printLine("the spirits jeer at you and deny you entry.  Your senses are");
        printLine("disturbed.  The objects in the dungeon appear indistinct, bleached of");
        printLine("color, even unreal.");
        printLine("");
        print("Do you wish to be resurrected? (Y/N) ");
        
        std::string response;
        std::getline(std::cin, response);
        
        // Convert to lowercase for comparison
        for (auto& c : response) {
            c = std::tolower(c);
        }
        
        return response == "y" || response == "yes";
    } else {
        // Offer simple resurrection in forest
        printLine("Now, let's take a look here...");
        printLine("Well, you probably deserve another chance.  I can't quite fix you");
        printLine("up completely, but you can't have everything.");
        printLine("");
        print("Do you wish to continue? (Y/N) ");
        
        std::string response;
        std::getline(std::cin, response);
        
        // Convert to lowercase for comparison
        for (auto& c : response) {
            c = std::tolower(c);
        }
        
        return response == "y" || response == "yes";
    }
}

// Perform resurrection (Requirement 59.2, 59.3, 59.4)
// Moves player to Entrance to Hades
// Scatters inventory
// Restores player health
void performResurrection() {
    auto& g = Globals::instance();
    
    // Check if player has visited South Temple (determines resurrection type)
    auto* southTemple = g.getObject(RoomIds::SOUTH_TEMPLE);
    bool visitedTemple = southTemple && southTemple->hasFlag(ObjectFlag::TOUCHBIT);
    
    if (visitedTemple) {
        // Full resurrection at Entrance to Hades (Requirement 59.3)
        // Mark as dead (ghost mode)
        dead_ = true;
        alwaysLit_ = true;  // Can see in darkness as a ghost
        
        // Set troll flag (troll disappears after player dies)
        auto* troll = g.getObject(ObjectIds::TROLL);
        if (troll) {
            troll->setFlag(ObjectFlag::INVISIBLE);
        }
        
        // Move to Entrance to Hades
        auto* hades = g.getObject(RoomIds::ENTRANCE_TO_HADES);
        if (hades && g.winner) {
            g.winner->moveTo(hades);
            g.here = hades;
        }
    } else {
        // Simple resurrection in forest
        // Move to Forest-1
        auto* forest = g.getObject(RoomIds::FOREST_1);
        if (forest && g.winner) {
            g.winner->moveTo(forest);
            g.here = forest;
        }
    }
    
    // Clear trap door touch bit
    auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
    if (trapDoor) {
        trapDoor->clearFlag(ObjectFlag::TOUCHBIT);
    }
    
    // Scatter inventory (Requirement 59.4)
    randomizeObjects();
    
    // Kill all timers
    killInterrupts();
    
    // Clear parser continuation flag
    g.pCont = false;
}

// Main death function (Requirement 58.1, 58.2, 58.3)
// Based on ZIL JIGS-UP routine
void jigsUp(std::string_view deathMessage, DeathCause cause) {
    auto& g = Globals::instance();
    
    // Set winner back to player
    g.winner = g.player;
    
    // Check if already dead (double death)
    if (dead_) {
        printLine("");
        printLine("It takes a talented person to be killed while already dead. YOU are such");
        printLine("a talent. Unfortunately, it takes a talented person to deal with it.");
        printLine("I am not such a talent. Sorry.");
        printLine("");
        printLine("The game is over.");
        return;
    }
    
    // Display death message (Requirement 58.2)
    if (!deathMessage.empty()) {
        printLine(std::string(deathMessage));
    }
    
    // Deduct 10 points for dying
    auto& score = ScoreSystem::instance();
    score.addScore(-10);
    
    // Display death banner
    printLine("");
    printLine("    ****  You have died  ****");
    printLine("");
    
    // Move player out of vehicle if in one
    if (g.winner && g.winner->getLocation()) {
        if (g.winner->getLocation()->hasFlag(ObjectFlag::VEHBIT)) {
            g.winner->moveTo(g.here);
        }
    }
    
    // Increment death counter (Requirement 58.5)
    deathCount_++;
    
    // Check if resurrection is available (Requirement 59.2, 59.5)
    if (!canResurrect()) {
        // Too many deaths - game over (Requirement 59.5)
        printLine("You clearly are a suicidal maniac.  We don't allow psychotics in the");
        printLine("cave, since they may harm other adventurers.  Your remains will be");
        printLine("installed in the Land of the Living Dead, where your fellow");
        printLine("adventurers may gloat over them.");
        printLine("");
        printLine("The game is over.");
        return;
    }
    
    // Offer resurrection (Requirement 59.1)
    if (offerResurrection()) {
        // Player accepted resurrection
        performResurrection();
    } else {
        // Player declined resurrection
        printLine("");
        printLine("Very well. The game is over.");
    }
}

// Reset death state for new game
void reset() {
    deathCount_ = 0;
    dead_ = false;
    alwaysLit_ = false;
}

} // namespace DeathSystem
