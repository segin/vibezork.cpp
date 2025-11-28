#include "light.h"
#include "../core/globals.h"
#include "../core/object.h"
#include "../core/io.h"

// Static member initialization
int LightSystem::darknessTurns_ = 0;
bool LightSystem::warnedAboutGrue_ = false;

// Check if a room is lit (Requirement 50)
// Based on ZIL LIT? routine in gparser.zil
bool LightSystem::isRoomLit(ZObject* room) {
    if (!room) {
        return false;
    }
    
    auto& g = Globals::instance();
    
    // Check if room has ONBIT flag (naturally lit)
    if (room->hasFlag(ObjectFlag::ONBIT)) {
        return true;
    }
    
    // Check for light sources in the room
    // Look through objects in the room
    for (auto* obj : room->getContents()) {
        if (obj && obj->hasFlag(ObjectFlag::LIGHTBIT) && obj->hasFlag(ObjectFlag::ONBIT)) {
            return true;
        }
        
        // Check inside open containers in the room
        if (obj && obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
            for (auto* contained : obj->getContents()) {
                if (contained && contained->hasFlag(ObjectFlag::LIGHTBIT) && 
                    contained->hasFlag(ObjectFlag::ONBIT)) {
                    return true;
                }
            }
        }
    }
    
    // Check player's inventory if player is in this room
    if (g.winner && g.winner->getLocation() == room) {
        for (auto* obj : g.winner->getContents()) {
            if (obj && obj->hasFlag(ObjectFlag::LIGHTBIT) && obj->hasFlag(ObjectFlag::ONBIT)) {
                return true;
            }
            
            // Check inside open containers in inventory
            if (obj && obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
                for (auto* contained : obj->getContents()) {
                    if (contained && contained->hasFlag(ObjectFlag::LIGHTBIT) && 
                        contained->hasFlag(ObjectFlag::ONBIT)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

// Check if player has a light source (Requirement 50)
bool LightSystem::hasLightSource() {
    auto& g = Globals::instance();
    
    if (!g.winner) {
        return false;
    }
    
    // Check player's inventory
    for (auto* obj : g.winner->getContents()) {
        if (obj && obj->hasFlag(ObjectFlag::LIGHTBIT) && obj->hasFlag(ObjectFlag::ONBIT)) {
            return true;
        }
        
        // Check inside open containers in inventory
        if (obj && obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
            for (auto* contained : obj->getContents()) {
                if (contained && contained->hasFlag(ObjectFlag::LIGHTBIT) && 
                    contained->hasFlag(ObjectFlag::ONBIT)) {
                    return true;
                }
            }
        }
    }
    
    // Check current room
    if (g.here) {
        for (auto* obj : g.here->getContents()) {
            if (obj && obj->hasFlag(ObjectFlag::LIGHTBIT) && obj->hasFlag(ObjectFlag::ONBIT)) {
                return true;
            }
            
            // Check inside open containers in the room
            if (obj && obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
                for (auto* contained : obj->getContents()) {
                    if (contained && contained->hasFlag(ObjectFlag::LIGHTBIT) && 
                        contained->hasFlag(ObjectFlag::ONBIT)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

// Update lighting state (Requirement 50)
void LightSystem::updateLighting() {
    auto& g = Globals::instance();
    
    if (!g.here) {
        g.lit = false;
        return;
    }
    
    bool wasLit = g.lit;
    g.lit = isRoomLit(g.here);
    
    // If we just went dark, warn the player
    if (wasLit && !g.lit) {
        printLine("It is now pitch black.");
        darknessTurns_ = 0;
        warnedAboutGrue_ = false;
    }
    // If we just got light, reset darkness tracking
    else if (!wasLit && g.lit) {
        darknessTurns_ = 0;
        warnedAboutGrue_ = false;
    }
}

// Check for grue attack (Requirement 51)
void LightSystem::checkGrue() {
    auto& g = Globals::instance();
    
    // Only check if we're in darkness
    if (g.lit) {
        darknessTurns_ = 0;
        warnedAboutGrue_ = false;
        return;
    }
    
    darknessTurns_++;
    
    // First turn in darkness: general warning
    if (darknessTurns_ == 1) {
        printLine("It is pitch dark. You are likely to be eaten by a grue.");
        warnedAboutGrue_ = false;
        return;
    }
    
    // After a few turns, give a more urgent warning
    if (darknessTurns_ == 2 && !warnedAboutGrue_) {
        printLine("You hear a faint rustling in the darkness.");
        warnedAboutGrue_ = true;
        return;
    }
    
    // After more turns, the grue attacks (Requirement 51)
    if (darknessTurns_ >= 4) {
        printLine("Oh, no! You have walked into the slavering fangs of a lurking grue!");
        printLine("    ****  You have died  ****");
        printLine("");
        printLine("Would you like to start over, restore a saved game, or end this session?");
        // TODO: Full JIGS-UP implementation in task 59 will handle resurrection/restart
        // For now, this provides the death message as required
        darknessTurns_ = 0;  // Reset to prevent repeated death messages
    }
}
