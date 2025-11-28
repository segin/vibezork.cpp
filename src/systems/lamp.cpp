#include "lamp.h"
#include "timer.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/objects.h"

namespace LampSystem {

// Lamp timer callback
// Based on I-LANTERN from GCLOCK.ZIL
// Requirement 47: Lamp battery drain
void lampTimerCallback() {
    auto& g = Globals::instance();
    
    // Get the lamp object
    ZObject* lamp = g.getObject(ObjectIds::LAMP);
    if (!lamp) {
        return;  // Lamp doesn't exist yet
    }
    
    // Only drain battery if lamp is on (Requirement 47.2)
    if (!lamp->hasFlag(ObjectFlag::ONBIT)) {
        return;  // Lamp is off, no drain
    }
    
    // Decrement battery counter (Requirement 47.2)
    g.lampBattery--;
    
    // Check battery level for warnings and depletion
    
    // Requirement 47.4: Turn off lamp when battery depleted
    if (g.lampBattery <= 0) {
        // Battery is depleted
        g.lampBattery = 0;
        
        // Turn off the lamp
        lamp->clearFlag(ObjectFlag::ONBIT);
        
        // Display depletion message
        printLine("The lamp has gone out.");
        
        // Make lamp unusable by setting capacity to 0
        // This is checked in vLampOn()
        lamp->setProperty(P_CAPACITY, 0);
        
        // Disable the timer since lamp is now unusable
        disableLampTimer();
        
        return;
    }
    
    // Requirement 47.3: Warn when battery is low
    // Warning at 50 turns remaining
    if (g.lampBattery == 50 && !g.lampWarned) {
        printLine("The lamp is getting dim.");
        g.lampWarned = true;
    }
    
    // Additional warning at 30 turns
    if (g.lampBattery == 30) {
        printLine("The lamp is nearly out.");
    }
    
    // Final warning at 10 turns
    if (g.lampBattery == 10) {
        printLine("The lamp is flickering and will go out soon.");
    }
}

// Initialize the lamp timer
// Requirement 47: Register lamp battery timer
void initialize() {
    // Register the I-LANTERN timer
    // Fires every turn (interval = 1)
    // Repeating timer
    TimerSystem::registerTimer("I-LANTERN", 1, lampTimerCallback, true);
    
    // Start disabled - will be enabled when lamp is turned on
    TimerSystem::disableTimer("I-LANTERN");
}

// Enable the lamp timer (Requirement 47: Enable when lamp is turned on)
void enableLampTimer() {
    TimerSystem::enableTimer("I-LANTERN");
}

// Disable the lamp timer (Requirement 47: Disable when lamp is turned off)
void disableLampTimer() {
    TimerSystem::disableTimer("I-LANTERN");
}

// Check if lamp has battery remaining
bool hasLampBattery() {
    auto& g = Globals::instance();
    return g.lampBattery > 0;
}

// Get current battery level
int getLampBattery() {
    auto& g = Globals::instance();
    return g.lampBattery;
}

} // namespace LampSystem
