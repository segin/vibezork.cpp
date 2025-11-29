#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <string_view>

// Timer System - handles scheduled events and interrupts
// Based on GCLOCK.ZIL from the original Zork I
// 
// The timer system manages periodic events like:
// - Thief wandering (I-THIEF)
// - Combat rounds (I-FIGHT)
// - Lamp battery drain (I-LANTERN)
// - Candle burning (I-CANDLES)
// - Sword glow checking (I-SWORD)
//
// Timers are called "interrupts" in the original ZIL code.
// Each timer has:
// - An interval (tick count)
// - A callback function to execute
// - An enabled flag
// - A repeating flag (one-shot vs repeating)

namespace TimerSystem {

// Timer callback function type
using TimerCallback = std::function<void()>;

// Timer structure
// Based on the C-TABLE structure in GCLOCK.ZIL
struct Timer {
    int interval;           // Initial interval (ticks between fires)
    int counter;            // Current countdown counter
    TimerCallback callback; // Function to call when timer fires
    bool enabled;           // Is timer currently enabled?
    bool repeating;         // Does timer repeat after firing?
    
    Timer() 
        : interval(0), counter(0), callback(nullptr), enabled(false), repeating(true) {}
    
    Timer(int interval_, TimerCallback callback_, bool repeating_ = true)
        : interval(interval_), counter(interval_), callback(callback_), 
          enabled(true), repeating(repeating_) {}
};

// Timer System class
// Manages all game timers and processes them each turn
class TimerManager {
public:
    // Get singleton instance
    static TimerManager& instance();
    
    // Register a new timer
    // name: Unique identifier for the timer (e.g., "I-THIEF", "I-LANTERN")
    // interval: Number of turns between timer fires
    // callback: Function to call when timer fires
    // repeating: If true, timer resets after firing; if false, fires once and disables
    void registerTimer(std::string_view name, int interval, 
                      TimerCallback callback, bool repeating = true);
    
    // Enable a timer (starts counting down)
    void enableTimer(std::string_view name);
    
    // Disable a timer (stops counting down)
    void disableTimer(std::string_view name);
    
    // Check if a timer is enabled
    bool isTimerEnabled(std::string_view name) const;
    
    // Reset a timer's counter to its initial interval
    void resetTimer(std::string_view name);
    
    // Set a timer's counter to a specific value (for QUEUE operation)
    void queueTimer(std::string_view name, int ticks);
    
    // Process all timers - call this once per game turn
    // This is equivalent to CLOCKER in GCLOCK.ZIL
    // Returns true if any timer fired
    bool tick();
    
    // Clear all timers (for game restart)
    void clear();
    
    // Get timer count (for debugging)
    size_t getTimerCount() const { return timers_.size(); }
    
    // Get all timers (for serialization)
    const std::unordered_map<std::string, Timer>& getAllTimers() const { return timers_; }
    
    // Set timer state (for deserialization)
    void setTimerState(std::string_view name, bool enabled, int counter);
    
private:
    TimerManager() = default;
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;
    
    std::unordered_map<std::string, Timer> timers_;
};

// Convenience functions for common operations

// Register a timer
inline void registerTimer(std::string_view name, int interval, 
                         TimerCallback callback, bool repeating = true) {
    TimerManager::instance().registerTimer(name, interval, callback, repeating);
}

// Enable a timer
inline void enableTimer(std::string_view name) {
    TimerManager::instance().enableTimer(name);
}

// Disable a timer
inline void disableTimer(std::string_view name) {
    TimerManager::instance().disableTimer(name);
}

// Check if timer is enabled
inline bool isTimerEnabled(std::string_view name) {
    return TimerManager::instance().isTimerEnabled(name);
}

// Reset a timer
inline void resetTimer(std::string_view name) {
    TimerManager::instance().resetTimer(name);
}

// Queue a timer with specific tick count
inline void queueTimer(std::string_view name, int ticks) {
    TimerManager::instance().queueTimer(name, ticks);
}

// Process all timers (call once per turn)
inline bool tick() {
    return TimerManager::instance().tick();
}

// Clear all timers
inline void clear() {
    TimerManager::instance().clear();
}

} // namespace TimerSystem

