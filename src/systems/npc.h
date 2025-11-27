#pragma once
#include "core/types.h"
#include "core/object.h"
#include "world/rooms.h"
#include <vector>
#include <random>

// NPC System - handles NPC behaviors like thief wandering, stealing, combat
// Based on Zork I's NPC mechanics from 1actions.zil

namespace NPCSystem {

// ============================================================================
// THIEF SYSTEM
// Based on THIEF-FCN and I-THIEF from 1actions.zil
// ============================================================================

// Thief state tracking
struct ThiefState {
    bool isAlive = true;           // Is thief alive?
    bool isEngaged = false;        // Is thief in combat with player?
    bool isAwake = true;           // Is thief conscious?
    int turnsUntilMove = 3;        // Turns until thief moves
    int turnsInRoom = 0;           // How long thief has been in current room
    int health = 5;                // Thief's health (for combat)
    
    // Rooms the thief can wander to (underground rooms, not sacred)
    std::vector<ObjectId> accessibleRooms;
};

// Get the global thief state
ThiefState& getThiefState();

// Initialize thief system - call during world initialization
void initializeThief();

// Process thief actions - call each turn from main loop
// Returns true if thief did something visible to player
bool processThiefTurn();

// Thief wandering behavior
// Moves thief to a random accessible room
void thiefWander();

// Thief stealing behavior
// Attempts to steal from player or room
// Returns true if thief stole something
bool thiefSteal();

// Thief combat behavior
// Called when player attacks thief or thief attacks player
// Returns true if combat occurred
bool thiefCombat();

// Thief treasure room behavior
// Special behavior when thief is in treasure room
void thiefTreasureRoom();

// Thief death behavior
// Called when thief is killed
void thiefDeath();

// Check if thief is in same room as player
bool isThiefWithPlayer();

// Get thief object
ZObject* getThief();

// Get thief's bag (contains stolen items)
ZObject* getThiefBag();

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Check if a room is accessible to the thief
bool isRoomAccessibleToThief(ObjectId roomId);

// Get a random accessible room for thief
ObjectId getRandomThiefRoom();

// Check if an object is a treasure (has VALUE property > 0)
bool isTreasure(ZObject* obj);

// Get random number in range [min, max]
int randomRange(int min, int max);

// ============================================================================
// TROLL SYSTEM
// Based on TROLL-FCN from 1actions.zil
// ============================================================================

// Troll state tracking
struct TrollState {
    bool isAlive = true;           // Is troll alive?
    bool isUnconscious = false;    // Is troll knocked out?
    int health = 3;                // Troll's health (for combat)
    int unconsciousTurns = 0;      // Turns remaining unconscious
};

// Get the global troll state
TrollState& getTrollState();

// Initialize troll system - call during world initialization
void initializeTroll();

// Process troll actions - call each turn from main loop
// Returns true if troll did something visible to player
bool processTrollTurn();

// Troll blocking behavior
// Prevents player from passing through troll room exits
// Returns true if troll blocks the player
bool trollBlocks(Direction dir);

// Troll combat behavior
// Called when player attacks troll
// Returns true if combat occurred
bool trollCombat();

// Troll death behavior
// Called when troll is killed
void trollDeath();

// Check if troll is in same room as player
bool isTrollWithPlayer();

// Check if troll is alive and conscious
bool isTrollActive();

// Get troll object
ZObject* getTroll();

// Get troll's axe
ZObject* getTrollAxe();

// Troll action handler - called when player interacts with troll
bool trollAction();

// ============================================================================
// CYCLOPS SYSTEM
// Based on CYCLOPS-FCN and I-CYCLOPS from 1actions.zil
// ============================================================================

// Cyclops state tracking
struct CyclopsState {
    bool isAsleep = false;         // Is cyclops asleep (fed water after peppers)?
    bool hasFled = false;          // Has cyclops fled (said Odysseus)?
    bool hasEatenPeppers = false;  // Has cyclops eaten the hot peppers?
    int wrathLevel = 0;            // Cyclops anger level (-5 to 5, negative = thirsty)
    int turnsInRoom = 0;           // Turns player has been in cyclops room
};

// Get the global cyclops state
CyclopsState& getCyclopsState();

// Initialize cyclops system - call during world initialization
void initializeCyclops();

// Process cyclops actions - call each turn from main loop
// Returns true if cyclops did something visible to player
bool processCyclopsTurn();

// Cyclops blocking behavior
// Prevents player from going up stairs until cyclops is dealt with
// Returns true if cyclops blocks the player
bool cyclopsBlocks(Direction dir);

// Cyclops hunger behavior
// Cyclops is hungry and will accept food
// Returns true if cyclops accepts the food
bool cyclopsAcceptsFood(ZObject* food);

// Cyclops eating behavior
// Called when player gives food to cyclops
// Returns true if cyclops ate the food
bool cyclopsEat(ZObject* food);

// Cyclops leaving behavior
// Called when player says "Odysseus" or "Ulysses"
// Returns true if cyclops fled
bool cyclopsFlee();

// Cyclops combat behavior
// Called when player attacks cyclops
// Returns true if combat occurred
bool cyclopsCombat();

// Check if cyclops is in same room as player
bool isCyclopsWithPlayer();

// Check if cyclops is active (not asleep, not fled)
bool isCyclopsActive();

// Get cyclops object
ZObject* getCyclops();

// Cyclops action handler - called when player interacts with cyclops
bool cyclopsAction();

// Handle the ODYSSEUS/ULYSSES verb
bool handleOdysseus();

} // namespace NPCSystem
