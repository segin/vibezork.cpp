#include "candle.h"
#include "timer.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/objects.h"

namespace CandleSystem {

// Candle timer callback
// Based on I-CANDLES from GCLOCK.ZIL
// Requirement 48: Candle burning timer
void candleTimerCallback() {
    auto& g = Globals::instance();
    
    // Get the candles object
    ZObject* candles = g.getObject(ObjectIds::CANDLES);
    if (!candles) {
        return;  // Candles don't exist yet
    }
    
    // Only burn wax if candles are lit (Requirement 48.2)
    if (!candles->hasFlag(ObjectFlag::ONBIT)) {
        return;  // Candles are not lit, no burning
    }
    
    // Get current wax level (stored in P_STRENGTH property)
    int wax = candles->getProperty(P_STRENGTH);
    
    // Decrement wax counter (Requirement 48.2)
    wax--;
    candles->setProperty(P_STRENGTH, wax);
    
    // Check wax level for warnings and burnout
    
    // Requirement 48.4: Remove candles when burned out
    if (wax <= 0) {
        // Candles are burned out
        candles->setProperty(P_STRENGTH, 0);
        
        // Turn off the candles
        candles->clearFlag(ObjectFlag::ONBIT);
        
        // Display burnout message
        printLine("The candles have burned down to nothing and gone out.");
        
        // Remove candles from the game
        // Move to a "nowhere" location (null parent)
        candles->moveTo(nullptr);
        
        // Disable the timer since candles are gone
        disableCandleTimer();
        
        return;
    }
    
    // Requirement 48.3: Warn when candles are nearly gone
    // Warning at 10 units remaining
    if (wax == 10) {
        printLine("The candles are getting short.");
    }
    
    // Additional warning at 5 units
    if (wax == 5) {
        printLine("The candles are very short now.");
    }
    
    // Final warning at 2 units
    if (wax == 2) {
        printLine("The candles are flickering and will burn out soon.");
    }
}

// Initialize the candle timer
// Requirement 48: Register candle burning timer
void initialize() {
    // Register the I-CANDLES timer
    // Fires every turn (interval = 1)
    // Repeating timer
    TimerSystem::registerTimer("I-CANDLES", 1, candleTimerCallback, true);
    
    // Check if candles exist and are lit
    auto& g = Globals::instance();
    ZObject* candles = g.getObject(ObjectIds::CANDLES);
    
    // Enable if candles exist and are lit, otherwise disable
    if (candles && candles->hasFlag(ObjectFlag::ONBIT)) {
        TimerSystem::enableTimer("I-CANDLES");
    } else {
        TimerSystem::disableTimer("I-CANDLES");
    }
}

// Enable the candle timer (Requirement 48: Enable when candles are lit)
void enableCandleTimer() {
    TimerSystem::enableTimer("I-CANDLES");
}

// Disable the candle timer (Requirement 48: Disable when candles burn out)
void disableCandleTimer() {
    TimerSystem::disableTimer("I-CANDLES");
}

// Check if candles have wax remaining
bool hasCandleWax() {
    auto& g = Globals::instance();
    ZObject* candles = g.getObject(ObjectIds::CANDLES);
    if (!candles) {
        return false;
    }
    
    int wax = candles->getProperty(P_STRENGTH);
    return wax > 0;
}

// Get current wax level
int getCandleWax() {
    auto& g = Globals::instance();
    ZObject* candles = g.getObject(ObjectIds::CANDLES);
    if (!candles) {
        return 0;
    }
    
    return candles->getProperty(P_STRENGTH);
}

} // namespace CandleSystem
