#include "sword.h"
#include "timer.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/objects.h"
#include "world/rooms.h"

namespace SwordSystem {

// Track previous glow state to detect changes
static bool previousGlowState = false;

// Sword timer callback
// Based on I-SWORD from GCLOCK.ZIL
// Requirement 49: Sword glow timer
void swordTimerCallback() {
    auto& g = Globals::instance();
    
    // Get the sword object
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    if (!sword) {
        return;  // Sword doesn't exist yet
    }
    
    // Check if player has the sword
    // Only glow if player is carrying it or it's in the current room
    bool playerHasSword = (sword->getLocation() == g.winner) || 
                          (sword->getLocation() == g.here);
    
    if (!playerHasSword) {
        // Sword is not with player, disable glow
        if (sword->hasFlag(ObjectFlag::ONBIT)) {
            sword->clearFlag(ObjectFlag::ONBIT);
            previousGlowState = false;
        }
        return;
    }
    
    // Update sword glow based on enemy proximity
    updateSwordGlow();
}

// Check if there are enemies nearby
// Requirement 49.2: Check for nearby enemies
bool areEnemiesNearby() {
    auto& g = Globals::instance();
    
    if (!g.here) {
        return false;
    }
    
    // Check current room for enemies
    // An enemy is an object with FIGHTBIT flag that is not dead
    const auto& contents = g.here->getContents();
    for (const auto* obj : contents) {
        if (obj && obj->hasFlag(ObjectFlag::FIGHTBIT) && 
            !obj->hasFlag(ObjectFlag::DEADBIT)) {
            return true;  // Enemy in current room
        }
    }
    
    // Check adjacent rooms for enemies
    // Get all exits from current room
    if (auto* room = dynamic_cast<ZRoom*>(g.here)) {
        // Check all possible directions
        Direction directions[] = {
            Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST,
            Direction::NE, Direction::NW, Direction::SE, Direction::SW,
            Direction::UP, Direction::DOWN, Direction::IN, Direction::OUT
        };
        
        for (Direction dir : directions) {
            RoomExit* exit = room->getExit(dir);
            if (!exit || exit->targetRoom == 0) {
                continue;
            }
            
            // Get the adjacent room
            ZObject* adjacentRoom = g.getObject(exit->targetRoom);
            if (!adjacentRoom) {
                continue;
            }
            
            // Check adjacent room for enemies
            const auto& adjacentContents = adjacentRoom->getContents();
            for (const auto* obj : adjacentContents) {
                if (obj && obj->hasFlag(ObjectFlag::FIGHTBIT) && 
                    !obj->hasFlag(ObjectFlag::DEADBIT)) {
                    return true;  // Enemy in adjacent room
                }
            }
        }
    }
    
    return false;  // No enemies nearby
}

// Update sword glow state
// Requirement 49.3: Set/clear glow flag and display messages
void updateSwordGlow() {
    auto& g = Globals::instance();
    
    // Get the sword object
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    if (!sword) {
        return;
    }
    
    // Check for nearby enemies
    bool enemiesNearby = areEnemiesNearby();
    
    // Get current glow state
    bool currentlyGlowing = sword->hasFlag(ObjectFlag::ONBIT);
    
    // Update glow state if changed
    if (enemiesNearby && !currentlyGlowing) {
        // Enemies appeared - start glowing
        sword->setFlag(ObjectFlag::ONBIT);
        
        // Display message if player can see the sword
        // Only show message if sword is in inventory or current room
        bool playerCanSeeSword = (sword->getLocation() == g.winner) || 
                                 (sword->getLocation() == g.here);
        
        if (playerCanSeeSword && !previousGlowState) {
            printLine("Your sword is glowing with a faint blue light.");
        }
        
        previousGlowState = true;
    } else if (!enemiesNearby && currentlyGlowing) {
        // Enemies left - stop glowing
        sword->clearFlag(ObjectFlag::ONBIT);
        
        // Display message if player can see the sword
        bool playerCanSeeSword = (sword->getLocation() == g.winner) || 
                                 (sword->getLocation() == g.here);
        
        if (playerCanSeeSword && previousGlowState) {
            printLine("Your sword stops glowing.");
        }
        
        previousGlowState = false;
    }
}

// Initialize the sword timer
// Requirement 49: Register sword glow timer
void initialize() {
    // Register the I-SWORD timer
    // Fires every turn (interval = 1)
    // Repeating timer
    TimerSystem::registerTimer("I-SWORD", 1, swordTimerCallback, true);
    
    // Start enabled - will check if player has sword in callback
    TimerSystem::enableTimer("I-SWORD");
}

// Enable the sword timer
void enableSwordTimer() {
    TimerSystem::enableTimer("I-SWORD");
}

// Disable the sword timer
void disableSwordTimer() {
    TimerSystem::disableTimer("I-SWORD");
}

} // namespace SwordSystem
