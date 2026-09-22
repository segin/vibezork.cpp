#include "core/gmacros.h"
#include "death.h"
#include "score.h"
#include "timer.h"
#include "../core/globals.h"
#include "../core/io.h"
#include "../world/rooms.h"
#include "../world/objects.h"
#include "../world/dungeon.h"
#include "../verbs/verbs.h"
#include <cstdlib>
#include <vector>

extern bool deadFunction();

namespace DeathSystem {

// State variables
static int deathCount_ = 0;
static bool dead_ = false;
static bool alwaysLit_ = false;
static bool testMode_ = false;

// Initialize death system

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


void setDead(bool dead) {
    dead_ = dead;
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

// ZIL: <ROUTINE RANDOMIZE-OBJECTS ("AUX" (R <>) F N L) ...>
// Source: zil/1actions.zil:4101-4123
//
// The lamp and the coffin go to fixed places; every other treasure is put in
// a random dark land room, and everything else in a random above-ground room.
static void randomizeObjects() {
    auto& g = Globals::instance();
    if (!g.winner) return;

    auto* lamp = g.getObject(ObjectIds::LAMP);
    if (lamp && lamp->getLocation() == g.winner) {
        lamp->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    }
    auto* coffin = g.getObject(ObjectIds::COFFIN);
    if (coffin && coffin->getLocation() == g.winner) {
        coffin->moveTo(g.getObject(RoomIds::EGYPT_ROOM));
    }
    if (auto* sword = g.getObject(ObjectIds::SWORD)) {
        sword->setProperty(P_TVALUE, 0);
    }

    // ZIL walks the child chain of ROOMS, keeping its place between items, and
    // takes the first room that is land, unlit and passes a 50 percent roll.
    auto* rooms = g.getObject(ObjectIds::ROOMS);
    const std::vector<ZObject*> roomList =
        rooms ? rooms->getContents() : std::vector<ZObject*>{};
    const auto aboveGround = Dungeon::aboveGround();
    std::size_t r = 0;
    bool started = false;

    auto contents = g.winner->getContents();
    for (auto* f : contents) {
        if (!f) continue;
        if (f->getProperty(P_TVALUE) > 0) {
            if (roomList.empty()) continue;
            for (std::size_t tries = 0; tries < roomList.size() * 4; ++tries) {
                if (!started) { r = 0; started = true; }
                ZObject* room = roomList[r % roomList.size()];
                if (room && room->hasFlag(ObjectFlag::RLANDBIT) &&
                    !room->hasFlag(ObjectFlag::ONBIT) && GMacros::prob(50)) {
                    f->moveTo(room);
                    break;
                }
                r = (r + 1) % roomList.size();
            }
        } else if (!aboveGround.empty()) {
            const int idx = GMacros::random(static_cast<int>(aboveGround.size()));
            ZObject* room = g.getObject(aboveGround[static_cast<std::size_t>(idx - 1)]);
            if (room) f->moveTo(room);
        }
    }
}

// ZIL: <ROUTINE KILL-INTERRUPTS () ...>
// Source: zil/1actions.zil:4125-4135
//
// Exactly these eight, and the match goes out. I-THIEF, I-FIGHT and the
// troll are deliberately left running.
static void killInterrupts() {
    TimerSystem::disable("I-XB");
    TimerSystem::disable("I-XC");
    TimerSystem::disable("I-CYCLOPS");
    TimerSystem::disable("I-LANTERN");
    TimerSystem::disable("I-CANDLES");
    TimerSystem::disable("I-SWORD");
    TimerSystem::disable("I-FOREST-ROOM");
    TimerSystem::disable("I-MATCH");
    if (auto* match = Globals::instance().getObject(ObjectIds::MATCH)) {
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
        printLine("the spirits jeer at you and deny you entry. Your senses are");
        printLine("disturbed. The objects in the dungeon appear indistinct, bleached of");
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
        printLine("Well, you probably deserve another chance. I can't quite fix you");
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
        
        // ZIL: Set action to DEAD-FUNCTION
        if (g.player) {
            g.player->setAction([](int) { return ::deadFunction() ? M_HANDLED : M_NOT_HANDLED; });
        }
        
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
        // Reset action (clear DEAD-FUNCTION if present)
        if (g.player) {
            g.player->setAction(nullptr);
        }
        
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
// ZIL: <ROUTINE JIGS-UP (DESC "OPTIONAL" (PLAYER? <>)) ...>
// Source: zil/1actions.zil:4046-4099
//
// The ZIL never asks whether you want to be resurrected. It resurrects you
// twice and ends the game on the third death, testing DEATHS before the
// increment.
void jigsUp(std::string_view deathMessage, DeathCause cause) {
    (void)cause;
    auto& g = Globals::instance();

    // ZIL: <SETG WINNER ,ADVENTURER>. The adventurer always exists in the
    // game; a stub world used by a test may not have one.
    if (g.player) g.winner = g.player;

    // ZIL: killed while already dead.
    if (dead_) {
        tell(CR, "It takes a talented person to be killed while already dead. "
                 "YOU are such a talent. Unfortunately, it takes a talented "
                 "person to deal with it. I am not such a talent. Sorry.", CR);
        Verbs::finish();
        return;
    }

    tell(deathMessage, CR);
    if (!g.lucky) {
        tell("Bad luck, huh?", CR);
    }

    Verbs::scoreUpd(-10);
    // ZIL: <TELL "|    ****  You have died  ****|  |"> - the source newlines
    // become spaces and each "|" a newline.
    tell(" ", CR, "    ****  You have died  **** ", CR, CR);

    // ZIL: climb out of a vehicle before being moved.
    if (g.winner && g.winner->getLocation() && g.here &&
        g.winner->getLocation()->hasFlag(ObjectFlag::VEHBIT)) {
        g.winner->moveTo(g.here);
    }

    // ZIL: <COND (<NOT <L? ,DEATHS 2>> ...)> - tested before the increment,
    // so the third death is the last.
    if (deathCount_ >= 2) {
        tell("You clearly are a suicidal maniac. We don't allow psychotics in "
             "the cave, since they may harm other adventurers. Your remains "
             "will be installed in the Land of the Living Dead, where your "
             "fellow adventurers may gloat over them.", CR);
        Verbs::finish();
        return;
    }

    deathCount_++;
    if (g.winner && g.here) g.winner->moveTo(g.here);

    auto* southTemple = g.getObject(RoomIds::SOUTH_TEMPLE);
    if (southTemple && southTemple->hasFlag(ObjectFlag::TOUCHBIT)) {
        tell("As you take your last breath, you feel relieved of your burdens. "
             "The feeling passes as you find yourself before the gates of Hell, "
             "where the spirits jeer at you and deny you entry. Your senses "
             "are disturbed. The objects in the dungeon appear indistinct, "
             "bleached of color, even unreal.", CR, CR);
        dead_ = true;
        g.trollFlag = true;
        alwaysLit_ = true;
        if (g.winner) {
            g.winner->setAction([](int) {
                return ::deadFunction() ? M_HANDLED : M_NOT_HANDLED;
            });
        }
        Verbs::goTo(g.getObject(RoomIds::ENTRANCE_TO_HADES));
    } else {
        tell("Now, let's take a look here... Well, you probably deserve another "
             "chance. I can't quite fix you up completely, but you can't have "
             "everything.", CR, CR);
        Verbs::goTo(g.getObject(RoomIds::FOREST_1));
    }

    if (auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR)) {
        trapDoor->clearFlag(ObjectFlag::TOUCHBIT);
    }
    g.pCont = 0;
    randomizeObjects();
    killInterrupts();
}

bool inTestMode() { return testMode_; }

void setAlwaysLit(bool on) { alwaysLit_ = on; }
bool alwaysLit() { return alwaysLit_; }

// Reset death state for new game
void reset() {
    deathCount_ = 0;
    dead_ = false;
    alwaysLit_ = false;
}

} // namespace DeathSystem
