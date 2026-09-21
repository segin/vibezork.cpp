#pragma once

// Sword Glow System - handles sword glowing when enemies are nearby
// Based on I-SWORD interrupt from GCLOCK.ZIL
// 
// The sword glows with a faint blue light when enemies are near.
// This provides warning to the player of danger.
// The glow is checked periodically by the I-SWORD timer.
//
// Requirement 49: Sword Glow Timer

namespace SwordSystem {

// Register the I-SWORD routine in C-TABLE (GO queues it: <QUEUE I-SWORD -1>)
void initialize();

// ZIL: I-SWORD interrupt routine (1actions.zil:3851-3889); glow body
// replaced by the TVALUE model in TODO F4
bool iSword();

// Sword glow timer callback
// Called each turn by the timer system
// Checks for nearby enemies and updates sword glow state
void swordTimerCallback();

// Enable the sword timer (when player has sword)
void enableSwordTimer();

// Disable the sword timer (when player doesn't have sword)
void disableSwordTimer();

// Check if there are enemies nearby
// Checks current room and adjacent rooms
// Returns true if enemies are present
bool areEnemiesNearby();

// Update sword glow state based on enemy proximity
// Sets or clears ONBIT flag on sword
// Displays messages when glow state changes
void updateSwordGlow();

} // namespace SwordSystem
