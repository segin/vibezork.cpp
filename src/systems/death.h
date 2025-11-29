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

// Set test mode (disables interactive prompts for testing)
void setTestMode(bool enabled);

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

// Offer resurrection to player (Requirement 59.1)
// Returns true if player accepts, false if they decline
bool offerResurrection();

// Perform resurrection (Requirement 59.2, 59.3, 59.4)
// Moves player to Entrance to Hades
// Scatters inventory
// Restores player health
void performResurrection();

// Reset death state for new game
void reset();

} // namespace DeathSystem
