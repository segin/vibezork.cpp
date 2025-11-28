#pragma once

// Lamp System - handles lamp battery drain and warnings
// Based on I-LANTERN interrupt from GCLOCK.ZIL
// 
// The lamp has a battery that drains when the lamp is on.
// The battery starts at 330 turns and drains 1 per turn when lit.
// Warnings are given at specific battery levels.
// When the battery reaches 0, the lamp turns off and becomes unusable.

namespace LampSystem {

// Initialize the lamp timer
// This registers the I-LANTERN timer with the timer system
void initialize();

// Lamp timer callback
// Called each turn by the timer system
// Decrements battery, warns when low, turns off when depleted
void lampTimerCallback();

// Enable the lamp timer (when lamp is turned on)
void enableLampTimer();

// Disable the lamp timer (when lamp is turned off)
void disableLampTimer();

// Check if lamp has battery remaining
bool hasLampBattery();

// Get current battery level
int getLampBattery();

} // namespace LampSystem
