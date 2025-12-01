/**
 * @file timer.cpp
 * @brief Timer/interrupt system implementation
 * 
 * Implements the game's timer system based on ZIL's GCLOCK.ZIL.
 * Timers are used for:
 * - Lamp battery drain (I-LANTERN)
 * - Candle burning (I-CANDLES)
 * - Sword glow checking (I-SWORD)
 * - Thief movement (I-THIEF)
 * - Combat rounds (I-FIGHT)
 * - Various puzzle timers
 * 
 * Each timer has an interval, counter, and callback. The tick()
 * method is called once per turn from the main loop.
 */

#include "timer.h"
#include <algorithm>

namespace TimerSystem {

TimerManager& TimerManager::instance() {
    static TimerManager instance;
    return instance;
}

void TimerManager::registerTimer(std::string_view name, int interval, 
                                 TimerCallback callback, bool repeating) {
    // Create timer with specified parameters
    // Based on INT routine in GCLOCK.ZIL
    std::string key(name);
    
    // Check if timer already exists
    auto it = timers_.find(key);
    if (it != timers_.end()) {
        // Update existing timer
        it->second.interval = interval;
        it->second.counter = interval;
        it->second.callback = callback;
        it->second.repeating = repeating;
        it->second.enabled = true;
    } else {
        // Create new timer
        timers_[key] = Timer(interval, callback, repeating);
    }
}

void TimerManager::enableTimer(std::string_view name) {
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        it->second.enabled = true;
    }
}

void TimerManager::disableTimer(std::string_view name) {
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        it->second.enabled = false;
    }
}

bool TimerManager::isTimerEnabled(std::string_view name) const {
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        return it->second.enabled;
    }
    return false;
}

void TimerManager::resetTimer(std::string_view name) {
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        it->second.counter = it->second.interval;
    }
}

void TimerManager::queueTimer(std::string_view name, int ticks) {
    // Based on QUEUE routine in GCLOCK.ZIL
    // Sets the timer's counter to a specific value
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        it->second.counter = ticks;
    }
}

bool TimerManager::tick() {
    // Based on CLOCKER routine in GCLOCK.ZIL
    // Process all enabled timers:
    // 1. Decrement counter
    // 2. If counter reaches 0, fire callback
    // 3. If repeating, reset counter; otherwise disable
    
    bool anyFired = false;
    
    for (auto& [name, timer] : timers_) {
        // Skip disabled timers
        if (!timer.enabled) {
            continue;
        }
        
        // Skip timers with counter at 0 (shouldn't happen, but be safe)
        if (timer.counter <= 0) {
            continue;
        }
        
        // Decrement counter
        timer.counter--;
        
        // Check if timer should fire
        if (timer.counter == 0) {
            // Fire the callback
            if (timer.callback) {
                timer.callback();
                anyFired = true;
            }
            
            // Handle repeating vs one-shot
            if (timer.repeating) {
                // Reset counter for next interval
                timer.counter = timer.interval;
            } else {
                // One-shot timer - disable after firing
                timer.enabled = false;
            }
        }
    }
    
    return anyFired;
}

void TimerManager::clear() {
    timers_.clear();
}

void TimerManager::setTimerState(std::string_view name, bool enabled, int counter) {
    if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
        it->second.enabled = enabled;
        it->second.counter = counter;
    }
}

} // namespace TimerSystem

