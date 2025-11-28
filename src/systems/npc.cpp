#include "npc.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include "world/rooms.h"
#include "world/objects.h"
#include "systems/timer.h"
#include <algorithm>
#include <random>

namespace NPCSystem {

// Global thief state
static ThiefState thiefState;

// Random number generator
static std::mt19937 rng(std::random_device{}());

ThiefState& getThiefState() {
    return thiefState;
}

int randomRange(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

ZObject* getThief() {
    return Globals::instance().getObject(ObjectIds::THIEF);
}

ZObject* getThiefBag() {
    return Globals::instance().getObject(ObjectIds::BAG);
}

bool isTreasure(ZObject* obj) {
    if (!obj) return false;
    // Check if object has VALUE or TVALUE property > 0
    int value = obj->getProperty(P_VALUE);
    int tvalue = obj->getProperty(P_TVALUE);
    return (value > 0 || tvalue > 0);
}

bool isRoomAccessibleToThief(ObjectId roomId) {
    auto& g = Globals::instance();
    ZObject* room = g.getObject(roomId);
    if (!room) return false;
    
    // Thief can't enter sacred rooms
    if (room->hasFlag(ObjectFlag::SACREDBIT)) return false;
    
    // Thief can't enter water rooms (no RWATERBIT in current flags, skip this check)
    
    // Thief prefers underground rooms (not ONBIT - naturally lit)
    // But can go anywhere that's not sacred or water
    return true;
}

void initializeThief() {
    auto& g = Globals::instance();
    thiefState = ThiefState();
    
    // Build list of accessible rooms for thief
    // Thief can wander through most underground rooms
    thiefState.accessibleRooms.clear();
    
    // Add underground rooms (maze, passages, special areas)
    // These are rooms the thief can wander to
    const std::vector<ObjectId> potentialRooms = {
        RoomIds::TROLL_ROOM,
        RoomIds::EAST_OF_CHASM,
        RoomIds::EW_PASSAGE,
        RoomIds::ROUND_ROOM,
        RoomIds::NS_PASSAGE,
        RoomIds::CHASM_ROOM,
        RoomIds::DEEP_CANYON,
        RoomIds::DAMP_CAVE,
        RoomIds::COLD_PASSAGE,
        RoomIds::NARROW_PASSAGE,
        RoomIds::MINE_ENTRANCE,
        RoomIds::BAT_ROOM,
        RoomIds::SHAFT_ROOM,
        RoomIds::SMELLY_ROOM,
        RoomIds::LADDER_TOP,
        RoomIds::LADDER_BOTTOM,
        RoomIds::TIMBER_ROOM,
        RoomIds::MACHINE_ROOM,
        // Maze rooms
        RoomIds::MAZE_1,
        RoomIds::MAZE_2,
        RoomIds::MAZE_3,
        RoomIds::MAZE_4,
        RoomIds::MAZE_5,
        RoomIds::MAZE_6,
        RoomIds::MAZE_7,
        RoomIds::MAZE_8,
        RoomIds::MAZE_9,
        RoomIds::MAZE_10,
        RoomIds::MAZE_11,
        RoomIds::MAZE_12,
        RoomIds::MAZE_13,
        RoomIds::MAZE_14,
        RoomIds::MAZE_15,
        RoomIds::GRATING_ROOM,
        // Special areas
        RoomIds::CYCLOPS_ROOM,
        RoomIds::STRANGE_PASSAGE,
        RoomIds::TREASURE_ROOM,
        RoomIds::ENGRAVINGS_CAVE,
        RoomIds::DOME_ROOM,
        RoomIds::TORCH_ROOM,
        // Temple area (not sacred in original)
        RoomIds::EGYPT_ROOM,
        // Mirror rooms
        RoomIds::MIRROR_ROOM_1,
        RoomIds::MIRROR_ROOM_2,
        RoomIds::SMALL_CAVE,
        RoomIds::TINY_CAVE,
        RoomIds::TWISTING_PASSAGE,
        RoomIds::WINDING_PASSAGE,
        // Reservoir area
        RoomIds::RESERVOIR_SOUTH,
        RoomIds::RESERVOIR_NORTH,
        RoomIds::STREAM_VIEW,
        RoomIds::ATLANTIS_ROOM,
        RoomIds::DAM_ROOM,
        RoomIds::DAM_LOBBY,
        RoomIds::MAINTENANCE_ROOM,
        RoomIds::DAM_BASE,
        // Cellar and house underground
        RoomIds::CELLAR,
        RoomIds::LIVING_ROOM,
        RoomIds::KITCHEN,
        RoomIds::ATTIC,
        RoomIds::GALLERY,
        RoomIds::STUDIO
    };
    
    for (ObjectId roomId : potentialRooms) {
        if (isRoomAccessibleToThief(roomId)) {
            thiefState.accessibleRooms.push_back(roomId);
        }
    }
    
    // Place thief in a random starting location
    ZObject* thief = getThief();
    if (thief && !thiefState.accessibleRooms.empty()) {
        ObjectId startRoom = getRandomThiefRoom();
        ZObject* room = g.getObject(startRoom);
        if (room) {
            thief->moveTo(room);
        }
    }
    
    // Place thief's bag with thief
    ZObject* bag = getThiefBag();
    if (bag && thief) {
        bag->moveTo(thief);
    }
    
    // Register thief timer (I-THIEF)
    // Based on original ZIL: fires every 3-5 turns
    // We'll use an interval of 4 turns as a middle ground
    TimerSystem::registerTimer("I-THIEF", 4, thiefTimerCallback, true);
    TimerSystem::enableTimer("I-THIEF");
}

void thiefTimerCallback() {
    // This is called by the timer system every N turns
    // Process thief actions: wandering, stealing, attacking
    if (!thiefState.isAlive) {
        // Thief is dead, disable the timer
        TimerSystem::disableTimer("I-THIEF");
        return;
    }
    
    // Call the main thief processing function
    processThiefTurn();
}

ObjectId getRandomThiefRoom() {
    if (thiefState.accessibleRooms.empty()) {
        return RoomIds::TREASURE_ROOM;  // Default to treasure room
    }
    int index = randomRange(0, static_cast<int>(thiefState.accessibleRooms.size()) - 1);
    return thiefState.accessibleRooms[index];
}

bool isThiefWithPlayer() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    if (!thief || !g.here) return false;
    
    return thief->getLocation() == g.here;
}

void thiefWander() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    if (!thief || !thiefState.isAlive) return;
    
    // Don't wander if engaged in combat
    if (thiefState.isEngaged) return;
    
    // Get current location
    ZObject* currentRoom = thief->getLocation();
    bool wasWithPlayer = (currentRoom == g.here);
    
    // Choose a new random room
    ObjectId newRoomId = getRandomThiefRoom();
    ZObject* newRoom = g.getObject(newRoomId);
    
    // Avoid staying in same room too often
    if (newRoom == currentRoom && thiefState.turnsInRoom < 3) {
        // Try again for a different room
        for (int i = 0; i < 3; i++) {
            newRoomId = getRandomThiefRoom();
            newRoom = g.getObject(newRoomId);
            if (newRoom != currentRoom) break;
        }
    }
    
    if (newRoom && newRoom != currentRoom) {
        // Move thief to new room
        thief->moveTo(newRoom);
        thiefState.turnsInRoom = 0;
        
        // If player was in old room, show departure message
        if (wasWithPlayer) {
            printLine("The thief, seemingly convinced that you have nothing of value, slips out of the room.");
        }
        
        // If player is in new room, show arrival message
        if (newRoom == g.here) {
            // Random arrival messages
            int msg = randomRange(1, 3);
            switch (msg) {
                case 1:
                    printLine("Someone carrying a large bag is creeping through the shadows.");
                    break;
                case 2:
                    printLine("A seedy-looking individual with a large bag just wandered through the room.");
                    break;
                case 3:
                    printLine("You hear a rustling sound behind you, and turn to see a sinister figure in the shadows.");
                    break;
            }
        }
    } else {
        thiefState.turnsInRoom++;
    }
}


bool thiefSteal() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    ZObject* bag = getThiefBag();
    
    if (!thief || !bag || !thiefState.isAlive) return false;
    if (!isThiefWithPlayer()) return false;
    
    // Thief prefers to steal treasures
    // First, look for treasures in the room
    std::vector<ZObject*> roomTreasures;
    std::vector<ZObject*> playerTreasures;
    std::vector<ZObject*> roomItems;
    std::vector<ZObject*> playerItems;
    
    // Check room contents
    for (auto* obj : g.here->getContents()) {
        if (obj == thief || obj == g.winner) continue;
        if (!obj->hasFlag(ObjectFlag::TAKEBIT)) continue;
        if (obj->hasFlag(ObjectFlag::TRYTAKEBIT)) continue;
        
        if (isTreasure(obj)) {
            roomTreasures.push_back(obj);
        } else {
            roomItems.push_back(obj);
        }
    }
    
    // Check player inventory
    for (auto* obj : g.winner->getContents()) {
        if (!obj->hasFlag(ObjectFlag::TAKEBIT)) continue;
        
        if (isTreasure(obj)) {
            playerTreasures.push_back(obj);
        } else {
            playerItems.push_back(obj);
        }
    }
    
    // Priority: player treasures > room treasures > player items > room items
    ZObject* target = nullptr;
    bool fromPlayer = false;
    
    if (!playerTreasures.empty() && randomRange(1, 100) <= 70) {
        // 70% chance to steal from player if they have treasures
        int idx = randomRange(0, static_cast<int>(playerTreasures.size()) - 1);
        target = playerTreasures[idx];
        fromPlayer = true;
    } else if (!roomTreasures.empty() && randomRange(1, 100) <= 50) {
        // 50% chance to take room treasures
        int idx = randomRange(0, static_cast<int>(roomTreasures.size()) - 1);
        target = roomTreasures[idx];
    } else if (!playerItems.empty() && randomRange(1, 100) <= 30) {
        // 30% chance to steal non-treasure from player
        int idx = randomRange(0, static_cast<int>(playerItems.size()) - 1);
        target = playerItems[idx];
        fromPlayer = true;
    } else if (!roomItems.empty() && randomRange(1, 100) <= 20) {
        // 20% chance to take room items
        int idx = randomRange(0, static_cast<int>(roomItems.size()) - 1);
        target = roomItems[idx];
    }
    
    if (target) {
        // Steal the item
        target->moveTo(bag);
        
        if (fromPlayer) {
            // Stealing from player - show message
            if (isTreasure(target)) {
                printLine("The thief, noticing your " + target->getDesc() + ", snatches it from you!");
            } else {
                printLine("The thief deftly removes the " + target->getDesc() + " from your possession.");
            }
        } else {
            // Taking from room - sometimes show message
            if (randomRange(1, 100) <= 50) {
                printLine("The thief picks up the " + target->getDesc() + " and places it in his bag.");
            }
        }
        return true;
    }
    
    return false;
}

bool thiefCombat() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    
    if (!thief || !thiefState.isAlive) return false;
    if (!isThiefWithPlayer()) return false;
    
    // Check if player is attacking thief
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        if (g.prso == thief) {
            thiefState.isEngaged = true;
            
            // Check if player has a weapon
            ZObject* weapon = g.prsi;
            if (!weapon) {
                // Look for weapon in inventory
                for (auto* obj : g.winner->getContents()) {
                    if (obj->hasFlag(ObjectFlag::WEAPONBIT)) {
                        weapon = obj;
                        break;
                    }
                }
            }
            
            if (!weapon) {
                printLine("Strangle the thief with your bare hands? You must be joking.");
                return true;
            }
            
            // Combat resolution
            int playerStrength = weapon->getProperty(P_STRENGTH);
            if (playerStrength == 0) playerStrength = 2;
            
            int thiefStrength = thief->getProperty(P_STRENGTH);
            if (thiefStrength == 0) thiefStrength = 5;
            
            // Random combat outcome
            int roll = randomRange(1, 100);
            int hitChance = 30 + (playerStrength * 10) - (thiefStrength * 5);
            
            if (roll <= hitChance) {
                // Player hits
                thiefState.health--;
                
                if (thiefState.health <= 0) {
                    // Thief is killed
                    printLine("You strike the thief with the " + weapon->getDesc() + "!");
                    printLine("The thief staggers and falls to the ground, mortally wounded.");
                    thiefDeath();
                } else if (thiefState.health == 1) {
                    printLine("You wound the thief badly! He looks like he's about to flee.");
                } else {
                    printLine("You strike the thief with the " + weapon->getDesc() + "!");
                    printLine("The thief is wounded but continues to fight.");
                }
            } else {
                // Player misses
                printLine("You swing at the thief but miss!");
                
                // Thief counterattacks
                int counterRoll = randomRange(1, 100);
                if (counterRoll <= 40) {
                    printLine("The thief slashes at you with his stiletto!");
                    printLine("You are wounded!");
                    // TODO: Track player health when health system is implemented
                } else {
                    printLine("The thief parries your attack and dances away.");
                }
            }
            
            // Check if thief should flee
            if (thiefState.health <= 1 && randomRange(1, 100) <= 60) {
                printLine("The thief, badly wounded, flees into the darkness!");
                thiefState.isEngaged = false;
                thiefWander();  // Move thief away
            }
            
            return true;
        }
    }
    
    // Thief initiates combat occasionally
    if (!thiefState.isEngaged && randomRange(1, 100) <= 15) {
        thiefState.isEngaged = true;
        printLine("The thief attacks you with his stiletto!");
        
        int roll = randomRange(1, 100);
        if (roll <= 30) {
            printLine("The stiletto slashes your arm!");
            // TODO: Track player health
        } else {
            printLine("You dodge the attack!");
        }
        return true;
    }
    
    return false;
}

void thiefTreasureRoom() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    ZObject* bag = getThiefBag();
    
    if (!thief || !bag || !thiefState.isAlive) return;
    
    // Check if thief is in treasure room
    ZObject* treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
    if (!treasureRoom || thief->getLocation() != treasureRoom) return;
    
    // When in treasure room, thief empties bag
    // Move all items from bag to treasure room
    std::vector<ZObject*> bagContents;
    for (auto* obj : bag->getContents()) {
        bagContents.push_back(obj);
    }
    
    for (auto* obj : bagContents) {
        obj->moveTo(treasureRoom);
    }
    
    // If player is in treasure room, special behavior
    if (g.here == treasureRoom) {
        if (!bagContents.empty()) {
            printLine("The thief empties his bag, scattering treasures about the room.");
        }
        
        // Thief is more aggressive in his lair
        if (!thiefState.isEngaged && randomRange(1, 100) <= 40) {
            printLine("The thief, angered by your intrusion into his lair, attacks!");
            thiefState.isEngaged = true;
        }
    }
}

void thiefDeath() {
    auto& g = Globals::instance();
    ZObject* thief = getThief();
    ZObject* bag = getThiefBag();
    
    if (!thief) return;
    
    thiefState.isAlive = false;
    thiefState.isEngaged = false;
    
    // Mark thief as dead
    thief->setFlag(ObjectFlag::DEADBIT);
    thief->clearFlag(ObjectFlag::FIGHTBIT);
    
    // Drop all items from bag to current room
    if (bag) {
        std::vector<ZObject*> bagContents;
        for (auto* obj : bag->getContents()) {
            bagContents.push_back(obj);
        }
        
        for (auto* obj : bagContents) {
            obj->moveTo(g.here);
        }
        
        if (!bagContents.empty()) {
            printLine("The thief's bag falls open, spilling its contents:");
            for (auto* obj : bagContents) {
                print("  ");
                printLine(obj->getDesc());
            }
        }
    }
    
    // Drop thief's stiletto
    ZObject* stiletto = g.getObject(ObjectIds::STILETTO);
    if (stiletto) {
        stiletto->moveTo(g.here);
        printLine("The thief's stiletto clatters to the ground.");
    }
    
    // Move thief body to current room (or remove from game)
    thief->moveTo(g.here);
}

bool processThiefTurn() {
    auto& g = Globals::instance();
    
    if (!thiefState.isAlive) return false;
    
    ZObject* thief = getThief();
    if (!thief) return false;
    
    bool didSomething = false;
    
    // Decrement turns until move
    thiefState.turnsUntilMove--;
    
    // Check if thief is in treasure room
    ZObject* treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
    if (thief->getLocation() == treasureRoom) {
        thiefTreasureRoom();
    }
    
    // If thief is with player
    if (isThiefWithPlayer()) {
        // Try to steal
        if (!thiefState.isEngaged && randomRange(1, 100) <= 40) {
            if (thiefSteal()) {
                didSomething = true;
            }
        }
        
        // Maybe attack
        if (!didSomething && randomRange(1, 100) <= 10) {
            if (thiefCombat()) {
                didSomething = true;
            }
        }
        
        // Maybe leave
        if (!didSomething && !thiefState.isEngaged && thiefState.turnsInRoom >= 2) {
            if (randomRange(1, 100) <= 50) {
                thiefWander();
                didSomething = true;
            }
        }
    } else {
        // Thief is not with player - wander
        if (thiefState.turnsUntilMove <= 0) {
            thiefWander();
            thiefState.turnsUntilMove = randomRange(2, 5);
            
            // Check if thief arrived where player is
            if (isThiefWithPlayer()) {
                didSomething = true;
            }
        }
    }
    
    thiefState.turnsInRoom++;
    
    return didSomething;
}

// ============================================================================
// TROLL SYSTEM IMPLEMENTATION
// Based on TROLL-FCN from 1actions.zil
// ============================================================================

// Global troll state
static TrollState trollState;

TrollState& getTrollState() {
    return trollState;
}

ZObject* getTroll() {
    return Globals::instance().getObject(ObjectIds::TROLL);
}

ZObject* getTrollAxe() {
    return Globals::instance().getObject(ObjectIds::AXE);
}

void initializeTroll() {
    trollState = TrollState();
    trollState.isAlive = true;
    trollState.isUnconscious = false;
    trollState.health = 3;
    trollState.unconsciousTurns = 0;
}

bool isTrollWithPlayer() {
    auto& g = Globals::instance();
    ZObject* troll = getTroll();
    if (!troll || !g.here) return false;
    
    return troll->getLocation() == g.here;
}

bool isTrollActive() {
    ZObject* troll = getTroll();
    if (!troll) return false;
    
    return trollState.isAlive && !trollState.isUnconscious && 
           !troll->hasFlag(ObjectFlag::DEADBIT);
}

bool trollBlocks(Direction dir) {
    auto& g = Globals::instance();
    
    // Check if troll is alive and in the same room
    if (!isTrollWithPlayer() || !isTrollActive()) {
        return false;
    }
    
    // Troll only blocks in the Troll Room
    if (!g.here || g.here->getId() != RoomIds::TROLL_ROOM) {
        return false;
    }
    
    // Troll blocks east and west exits (the passages he guards)
    if (dir == Direction::EAST || dir == Direction::WEST) {
        // Random chance troll attacks when player tries to pass
        int roll = randomRange(1, 100);
        if (roll <= 50) {
            printLine("The troll swings his axe and blocks your way!");
            printLine("The troll's axe barely misses your head.");
        } else {
            printLine("A menacing troll blocks your way.");
        }
        return true;
    }
    
    return false;
}

bool trollCombat() {
    auto& g = Globals::instance();
    ZObject* troll = getTroll();
    
    if (!troll || !trollState.isAlive) return false;
    if (!isTrollWithPlayer()) return false;
    
    // Check if player is attacking troll
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        if (g.prso == troll) {
            // Check if player has a weapon
            ZObject* weapon = g.prsi;
            if (!weapon) {
                // Look for weapon in inventory
                for (auto* obj : g.winner->getContents()) {
                    if (obj->hasFlag(ObjectFlag::WEAPONBIT)) {
                        weapon = obj;
                        break;
                    }
                }
            }
            
            if (!weapon) {
                printLine("Attacking the troll with your bare hands is suicidal.");
                return true;
            }
            
            // Combat resolution
            int weaponStrength = weapon->getProperty(P_STRENGTH);
            if (weaponStrength == 0) weaponStrength = 2;
            
            // Random combat outcome
            int roll = randomRange(1, 100);
            int hitChance = 25 + (weaponStrength * 10);
            
            if (roll <= hitChance) {
                // Player hits
                trollState.health--;
                
                if (trollState.health <= 0) {
                    // Troll is killed
                    printLine("You strike the troll with a mighty blow!");
                    printLine("The troll staggers back, drops his axe, and disappears in a cloud of black smoke.");
                    trollDeath();
                } else if (trollState.health == 1) {
                    printLine("You wound the troll badly! He looks very unsteady.");
                } else {
                    printLine("You hit the troll with the " + weapon->getDesc() + "!");
                    printLine("The troll is wounded but continues to fight.");
                }
            } else {
                // Player misses
                printLine("You swing at the troll but miss!");
                
                // Troll counterattacks
                int counterRoll = randomRange(1, 100);
                if (counterRoll <= 50) {
                    printLine("The troll swings his axe at you!");
                    if (counterRoll <= 25) {
                        printLine("The axe grazes your arm. You are wounded!");
                        // TODO: Track player health when health system is implemented
                    } else {
                        printLine("You barely dodge the blow!");
                    }
                } else {
                    printLine("The troll prepares for another attack.");
                }
            }
            
            return true;
        }
    }
    
    return false;
}

void trollDeath() {
    auto& g = Globals::instance();
    ZObject* troll = getTroll();
    ZObject* axe = getTrollAxe();
    
    if (!troll) return;
    
    trollState.isAlive = false;
    
    // Mark troll as dead
    troll->setFlag(ObjectFlag::DEADBIT);
    troll->clearFlag(ObjectFlag::FIGHTBIT);
    
    // Troll vanishes (move to nowhere)
    troll->moveTo(nullptr);
    
    // Drop the axe in the current room
    if (axe) {
        axe->moveTo(g.here);
        axe->setFlag(ObjectFlag::TAKEBIT);  // Axe can now be taken
    }
}

bool processTrollTurn() {
    auto& g = Globals::instance();
    
    if (!trollState.isAlive) return false;
    
    ZObject* troll = getTroll();
    if (!troll) return false;
    
    // Handle unconscious state
    if (trollState.isUnconscious) {
        trollState.unconsciousTurns--;
        if (trollState.unconsciousTurns <= 0) {
            trollState.isUnconscious = false;
            if (isTrollWithPlayer()) {
                printLine("The troll regains consciousness and stands up, looking very angry.");
            }
        }
        return false;
    }
    
    // If troll is with player and active, occasionally attack
    if (isTrollWithPlayer() && isTrollActive()) {
        int roll = randomRange(1, 100);
        if (roll <= 20) {
            // Troll attacks
            printLine("The troll swings his axe at you!");
            int hitRoll = randomRange(1, 100);
            if (hitRoll <= 30) {
                printLine("The axe hits you! You are wounded!");
                // TODO: Track player health
            } else {
                printLine("You dodge the blow!");
            }
            return true;
        } else if (roll <= 30) {
            // Troll growls menacingly
            printLine("The troll growls menacingly.");
            return true;
        }
    }
    
    return false;
}

bool trollAction() {
    auto& g = Globals::instance();
    ZObject* troll = getTroll();
    
    if (!troll || g.prso != troll) return false;
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        if (!trollState.isAlive || troll->hasFlag(ObjectFlag::DEADBIT)) {
            printLine("The troll is dead.");
        } else if (trollState.isUnconscious) {
            printLine("The troll is unconscious on the ground.");
        } else {
            printLine("A nasty-looking troll, brandishing a bloody axe, blocks all passages out of the room.");
        }
        return true;
    }
    
    // Handle ATTACK/KILL
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        return trollCombat();
    }
    
    // Handle GIVE - troll doesn't accept gifts
    if (g.prsa == V_GIVE) {
        printLine("The troll is not interested in your gift.");
        return true;
    }
    
    // Handle THROW at troll
    if (g.prsa == V_THROW && g.prsi == troll) {
        if (g.prso && g.prso->hasFlag(ObjectFlag::WEAPONBIT)) {
            // Throwing a weapon at troll
            printLine("The troll catches the " + g.prso->getDesc() + " and throws it back at you!");
            g.prso->moveTo(g.here);
        } else {
            printLine("The troll deflects the " + (g.prso ? g.prso->getDesc() : "object") + " with his axe.");
            if (g.prso) g.prso->moveTo(g.here);
        }
        return true;
    }
    
    return false;
}

// ============================================================================
// CYCLOPS SYSTEM IMPLEMENTATION
// Based on CYCLOPS-FCN and I-CYCLOPS from 1actions.zil
// ============================================================================

// Global cyclops state
static CyclopsState cyclopsState;

// Cyclops anger messages (from CYCLOMAD table in ZIL)
static const std::vector<std::string> cyclopsAngerMessages = {
    "The cyclops seems somewhat agitated.",
    "The cyclops appears to be getting more agitated.",
    "The cyclops is moving about the room, looking for something.",
    "The cyclops was looking for salt and pepper. No doubt they are condiments for his upcoming snack."
};

CyclopsState& getCyclopsState() {
    return cyclopsState;
}

ZObject* getCyclops() {
    return Globals::instance().getObject(ObjectIds::CYCLOPS);
}

void initializeCyclops() {
    cyclopsState = CyclopsState();
    cyclopsState.isAsleep = false;
    cyclopsState.hasFled = false;
    cyclopsState.hasEatenPeppers = false;
    cyclopsState.wrathLevel = 0;
    cyclopsState.turnsInRoom = 0;
}

bool isCyclopsWithPlayer() {
    auto& g = Globals::instance();
    ZObject* cyclops = getCyclops();
    if (!cyclops || !g.here) return false;
    
    return cyclops->getLocation() == g.here;
}

bool isCyclopsActive() {
    ZObject* cyclops = getCyclops();
    if (!cyclops) return false;
    
    // Cyclops is active if not asleep and not fled
    return !cyclopsState.isAsleep && !cyclopsState.hasFled;
}

bool cyclopsBlocks(Direction dir) {
    auto& g = Globals::instance();
    
    // Check if cyclops is active and in the same room
    if (!isCyclopsWithPlayer() || !isCyclopsActive()) {
        return false;
    }
    
    // Cyclops only blocks in the Cyclops Room
    if (!g.here || g.here->getId() != RoomIds::CYCLOPS_ROOM) {
        return false;
    }
    
    // Cyclops blocks the UP exit (stairs to treasure room)
    if (dir == Direction::UP) {
        if (cyclopsState.wrathLevel == 0) {
            printLine("A cyclops, who looks prepared to eat horses (much less mere adventurers), blocks the staircase.");
        } else if (cyclopsState.wrathLevel > 0) {
            printLine("The cyclops is standing in the corner, eyeing you closely. He doesn't look like he'll let you pass.");
        } else {
            // Negative wrath = thirsty after eating peppers
            printLine("The cyclops, gasping from the hot peppers, still blocks your way up the stairs.");
        }
        return true;
    }
    
    return false;
}

bool cyclopsAcceptsFood(ZObject* food) {
    if (!food) return false;
    
    // Cyclops accepts the lunch (hot pepper sandwich)
    if (food->getId() == ObjectIds::LUNCH) {
        return true;
    }
    
    // Cyclops accepts water (or bottle with water)
    if (food->getId() == ObjectIds::WATER) {
        return true;
    }
    if (food->getId() == ObjectIds::BOTTLE) {
        // Check if bottle contains water
        auto& g = Globals::instance();
        ZObject* water = g.getObject(ObjectIds::WATER);
        if (water && water->getLocation() == food) {
            return true;
        }
    }
    
    return false;
}

bool cyclopsEat(ZObject* food) {
    auto& g = Globals::instance();
    ZObject* cyclops = getCyclops();
    
    if (!cyclops || !food || !isCyclopsWithPlayer() || !isCyclopsActive()) {
        return false;
    }
    
    // Handle hot pepper sandwich (lunch)
    if (food->getId() == ObjectIds::LUNCH) {
        // Only accept if not already thirsty (wrath >= 0)
        if (cyclopsState.wrathLevel >= 0) {
            // Remove the lunch
            food->moveTo(nullptr);
            
            printLine("The cyclops says \"Mmm Mmm. I love hot peppers! But oh, could I use a drink. Perhaps I could drink the blood of that thing.\" From the gleam in his eye, it could be surmised that you are \"that thing\".");
            
            // Set wrath to negative (thirsty state)
            cyclopsState.wrathLevel = -1;
            cyclopsState.hasEatenPeppers = true;
            
            return true;
        }
        return false;
    }
    
    // Handle water
    if (food->getId() == ObjectIds::WATER || food->getId() == ObjectIds::BOTTLE) {
        // Only accept water if cyclops has eaten peppers (wrath < 0)
        if (cyclopsState.wrathLevel < 0) {
            // Remove the water
            ZObject* water = g.getObject(ObjectIds::WATER);
            if (water) {
                water->moveTo(nullptr);
            }
            
            // If bottle was given, drop it in room and open it
            if (food->getId() == ObjectIds::BOTTLE) {
                food->moveTo(g.here);
                food->setFlag(ObjectFlag::OPENBIT);
            }
            
            // Cyclops falls asleep
            cyclopsState.isAsleep = true;
            cyclops->clearFlag(ObjectFlag::FIGHTBIT);
            
            printLine("The cyclops takes the bottle, checks that it's open, and drinks the water. A moment later, he lets out a yawn that nearly blows you over, and then falls fast asleep (what did you put in that drink, anyway?).");
            
            return true;
        } else {
            printLine("The cyclops apparently is not thirsty and refuses your generous offer.");
            return true;
        }
    }
    
    // Handle garlic - cyclops refuses
    if (food->getId() == ObjectIds::GARLIC) {
        printLine("The cyclops may be hungry, but there is a limit.");
        return true;
    }
    
    // Cyclops refuses other food
    printLine("The cyclops is not so stupid as to eat THAT!");
    return true;
}

bool cyclopsFlee() {
    auto& g = Globals::instance();
    ZObject* cyclops = getCyclops();
    
    if (!cyclops || !isCyclopsWithPlayer()) {
        return false;
    }
    
    // Only works if cyclops is not asleep
    if (cyclopsState.isAsleep) {
        printLine("The cyclops is fast asleep and doesn't hear you.");
        return true;
    }
    
    // Cyclops flees when hearing Odysseus/Ulysses
    cyclopsState.hasFled = true;
    cyclops->clearFlag(ObjectFlag::FIGHTBIT);
    cyclops->moveTo(nullptr);  // Remove from game
    
    printLine("The cyclops, hearing the name of his father's deadly nemesis, flees the room by knocking down the wall on the east of the room.");
    
    // This opens a passage to the east (Strange Passage)
    // The room action will handle showing the new exit
    
    return true;
}

bool cyclopsCombat() {
    auto& g = Globals::instance();
    ZObject* cyclops = getCyclops();
    
    if (!cyclops || !isCyclopsWithPlayer()) return false;
    
    // Can't attack sleeping cyclops without waking it
    if (cyclopsState.isAsleep) {
        printLine("The cyclops yawns and stares at the thing that woke him up.");
        cyclopsState.isAsleep = false;
        cyclops->setFlag(ObjectFlag::FIGHTBIT);
        // Reset wrath based on previous state
        if (cyclopsState.wrathLevel < 0) {
            cyclopsState.wrathLevel = -cyclopsState.wrathLevel;
        }
        return true;
    }
    
    // Check if player is attacking cyclops
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        if (g.prso == cyclops) {
            // Check if player has a weapon
            ZObject* weapon = g.prsi;
            if (!weapon) {
                // Look for weapon in inventory
                for (auto* obj : g.winner->getContents()) {
                    if (obj->hasFlag(ObjectFlag::WEAPONBIT)) {
                        weapon = obj;
                        break;
                    }
                }
            }
            
            if (!weapon) {
                printLine("The cyclops laughs at your puny attempt to hurt him with your bare hands.");
                // Increase wrath
                if (cyclopsState.wrathLevel >= 0) {
                    cyclopsState.wrathLevel++;
                } else {
                    cyclopsState.wrathLevel--;
                }
                return true;
            }
            
            // Cyclops is very strong - attacks mostly fail
            int roll = randomRange(1, 100);
            
            if (roll <= 10) {
                // Very rare hit
                printLine("You manage to wound the cyclops slightly, but it only makes him angrier!");
                if (cyclopsState.wrathLevel >= 0) {
                    cyclopsState.wrathLevel += 2;
                } else {
                    cyclopsState.wrathLevel -= 2;
                }
            } else {
                // Miss - cyclops dodges or shrugs it off
                printLine("The cyclops shrugs but otherwise ignores your pitiful attempt.");
                if (cyclopsState.wrathLevel >= 0) {
                    cyclopsState.wrathLevel++;
                } else {
                    cyclopsState.wrathLevel--;
                }
            }
            
            return true;
        }
    }
    
    return false;
}

bool processCyclopsTurn() {
    auto& g = Globals::instance();
    
    // Check if player is in cyclops room
    if (!g.here || g.here->getId() != RoomIds::CYCLOPS_ROOM) {
        cyclopsState.turnsInRoom = 0;
        return false;
    }
    
    ZObject* cyclops = getCyclops();
    if (!cyclops || cyclopsState.hasFled) return false;
    
    // If cyclops is asleep, nothing happens
    if (cyclopsState.isAsleep) {
        return false;
    }
    
    // Track turns in room
    cyclopsState.turnsInRoom++;
    
    // Check if cyclops should attack (wrath level too high)
    int absWrath = cyclopsState.wrathLevel >= 0 ? cyclopsState.wrathLevel : -cyclopsState.wrathLevel;
    
    if (absWrath > 5) {
        // Cyclops eats the player!
        printLine("The cyclops, tired of all of your games and trickery, grabs you firmly. As he licks his chops, he says \"Mmm. Just like Mom used to make 'em.\" It's nice to be appreciated.");
        // This should trigger player death - for now just print the message
        // TODO: Integrate with death system when implemented
        return true;
    }
    
    // Show anger messages as wrath increases
    if (absWrath > 0 && absWrath <= 4) {
        // Show appropriate anger message
        int msgIndex = absWrath - 1;
        if (msgIndex < static_cast<int>(cyclopsAngerMessages.size())) {
            printLine(cyclopsAngerMessages[msgIndex]);
            return true;
        }
    }
    
    // Increase wrath over time if player lingers
    if (cyclopsState.turnsInRoom > 3 && randomRange(1, 100) <= 30) {
        if (cyclopsState.wrathLevel >= 0) {
            cyclopsState.wrathLevel++;
        } else {
            cyclopsState.wrathLevel--;
        }
    }
    
    return false;
}

bool cyclopsAction() {
    auto& g = Globals::instance();
    ZObject* cyclops = getCyclops();
    
    if (!cyclops || g.prso != cyclops) return false;
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        if (cyclopsState.hasFled) {
            printLine("The cyclops has fled.");
        } else if (cyclopsState.isAsleep) {
            printLine("The cyclops is sleeping like a baby, albeit a very ugly one.");
        } else if (cyclopsState.wrathLevel == 0) {
            printLine("A hungry cyclops is standing at the foot of the stairs.");
        } else if (cyclopsState.wrathLevel > 0) {
            printLine("The cyclops is standing in the corner, eyeing you closely. I don't think he likes you very much. He looks extremely hungry, even for a cyclops.");
        } else {
            // Negative wrath = thirsty
            printLine("The cyclops, having eaten the hot peppers, appears to be gasping. His enflamed tongue protrudes from his man-sized mouth.");
        }
        return true;
    }
    
    // Handle ATTACK/KILL
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        return cyclopsCombat();
    }
    
    // Handle GIVE
    if (g.prsa == V_GIVE && g.prsi == cyclops) {
        return cyclopsEat(g.prso);
    }
    
    // Handle THROW at cyclops
    if (g.prsa == V_THROW && g.prsi == cyclops) {
        if (cyclopsState.isAsleep) {
            printLine("The cyclops yawns and stares at the thing that woke him up.");
            cyclopsState.isAsleep = false;
            cyclops->setFlag(ObjectFlag::FIGHTBIT);
            if (cyclopsState.wrathLevel < 0) {
                cyclopsState.wrathLevel = -cyclopsState.wrathLevel;
            }
        } else {
            printLine("The cyclops shrugs but otherwise ignores your pitiful attempt.");
            if (g.prso) g.prso->moveTo(g.here);
        }
        // Increase wrath
        if (cyclopsState.wrathLevel >= 0) {
            cyclopsState.wrathLevel++;
        } else {
            cyclopsState.wrathLevel--;
        }
        return true;
    }
    
    // Handle TAKE - can't take the cyclops
    if (g.prsa == V_TAKE) {
        printLine("The cyclops doesn't take kindly to being grabbed.");
        return true;
    }
    
    // Handle TIE
    if (g.prsa == V_TIE) {
        printLine("You cannot tie the cyclops, though he is fit to be tied.");
        return true;
    }
    
    // Handle LISTEN
    if (g.prsa == V_LISTEN) {
        if (cyclopsState.isAsleep) {
            printLine("You hear loud snoring.");
        } else {
            printLine("You can hear his stomach rumbling.");
        }
        return true;
    }
    
    // Handle talking to cyclops
    if (g.prsa == V_TALK || g.prsa == V_ASK || g.prsa == V_TELL) {
        if (cyclopsState.isAsleep) {
            printLine("No use talking to him. He's fast asleep.");
        } else {
            printLine("The cyclops prefers eating to making conversation.");
        }
        return true;
    }
    
    return false;
}

bool handleOdysseus() {
    auto& g = Globals::instance();
    
    // Only works in cyclops room with cyclops present
    if (!g.here || g.here->getId() != RoomIds::CYCLOPS_ROOM) {
        printLine("Wasn't he a sailor?");
        return true;
    }
    
    ZObject* cyclops = getCyclops();
    if (!cyclops || cyclopsState.hasFled) {
        printLine("Wasn't he a sailor?");
        return true;
    }
    
    if (cyclopsState.isAsleep) {
        printLine("The cyclops is fast asleep and doesn't hear you.");
        return true;
    }
    
    // Cyclops flees!
    return cyclopsFlee();
}

} // namespace NPCSystem
