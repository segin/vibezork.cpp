#pragma once
#include "../core/types.h"
#include <string_view>

// Death and Game Over System
// Implements the JIGS-UP function and resurrection mechanics
// Requirements: 58, 59

namespace DeathSystem {

// Death scenarios (Requirement 58.4)
enum class DeathCause {
    COMBAT,         // Killed in combat
    GRUE,           // Eaten by a grue
    FALLING,        // Fell to death
    DROWNING,       // Drowned
    EXPLOSION,      // Blown up
    SUICIDE,        // Killed self
    OTHER           // Other death scenarios
};

// Initialize death system
void initialize();

// Main death function (Requirement 58.1)
// Called when player dies
// Displays death message, offers resurrection or restart
void jigsUp(std::string_view deathMessage, DeathCause cause = DeathCause::OTHER);

// Get death count (Requirement 58.5)
int getDeathCount();

// Check if player is dead
bool isDead();

// Check if resurrection is available (Requirement 59.2)
bool canResurrect();

// Perform resurrection (Requirement 59)
// Moves player to Entrance to Hades
// Scatters inventory
void resurrect();

// Reset death state for new game
void reset();

} // namespace DeathSystem
