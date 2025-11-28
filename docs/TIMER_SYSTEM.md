# Timer System Documentation

## Overview

The Timer System manages scheduled events and interrupts in Zork I. It is based on the GCLOCK.ZIL implementation from the original game. Timers fire periodically during gameplay to handle events like NPC movement, combat rounds, lamp battery drain, and other time-based mechanics.

## Architecture

The timer system consists of:

- **TimerManager**: Singleton class that manages all timers
- **Timer**: Structure containing interval, counter, callback, enabled flag, and repeating flag
- **TimerCallback**: Function type for timer callbacks (`std::function<void()>`)

## Basic Usage

### Including the Timer System

```cpp
#include "systems/timer.h"
```

### Registering a Timer

```cpp
// Register a repeating timer that fires every 5 turns
TimerSystem::registerTimer("my-timer", 5, []() {
    // This code runs every 5 turns
    std::cout << "Timer fired!" << std::endl;
}, true);  // true = repeating
```

### One-Shot Timers

```cpp
// Register a one-shot timer that fires once after 10 turns
TimerSystem::registerTimer("one-shot", 10, []() {
    std::cout << "This fires once!" << std::endl;
}, false);  // false = one-shot
```

### Enabling and Disabling Timers

```cpp
// Disable a timer (stops counting down)
TimerSystem::disableTimer("my-timer");

// Re-enable a timer (resumes counting down)
TimerSystem::enableTimer("my-timer");

// Check if a timer is enabled
if (TimerSystem::isTimerEnabled("my-timer")) {
    // Timer is active
}
```

### Processing Timers

Call `tick()` once per game turn in the main loop:

```cpp
void mainLoop() {
    while (true) {
        // ... process player input ...
        
        // Process all timers
        TimerSystem::tick();
    }
}
```

### Queue Operation

Set a timer to fire after a specific number of ticks (like QUEUE in ZIL):

```cpp
// Make timer fire in exactly 3 turns
TimerSystem::queueTimer("my-timer", 3);
```

### Reset Operation

Reset a timer's counter back to its original interval:

```cpp
// Reset timer to start counting from the beginning
TimerSystem::resetTimer("my-timer");
```

## Example: Thief Wandering Timer

Here's how to implement the thief wandering timer (I-THIEF from the original):

```cpp
void initializeThiefTimer() {
    // Thief moves every 3-5 turns (randomized)
    TimerSystem::registerTimer("I-THIEF", 3, []() {
        // Move thief to a random room
        NPCSystem::thiefWander();
        
        // Randomize next interval
        int nextInterval = randomRange(3, 5);
        TimerSystem::queueTimer("I-THIEF", nextInterval);
    }, true);
}
```

## Example: Lamp Battery Timer

Here's how to implement the lamp battery drain (I-LANTERN):

```cpp
void initializeLampTimer() {
    // Lamp battery drains every turn when lit
    TimerSystem::registerTimer("I-LANTERN", 1, []() {
        auto& g = Globals::instance();
        ZObject* lamp = g.getObject(ObjectIds::LAMP);
        
        if (lamp && lamp->hasFlag(ObjectFlag::ONBIT)) {
            // Decrement battery
            int battery = lamp->getProperty(P_BATTERY);
            battery--;
            lamp->setProperty(P_BATTERY, battery);
            
            if (battery <= 0) {
                // Battery depleted
                lamp->clearFlag(ObjectFlag::ONBIT);
                printLine("The lamp has run out of power.");
                TimerSystem::disableTimer("I-LANTERN");
            } else if (battery <= 10) {
                // Low battery warning
                printLine("The lamp is getting dim.");
            }
        }
    }, true);
}
```

## Example: Combat Round Timer

Here's how to implement combat rounds (I-FIGHT):

```cpp
void startCombat(ZObject* enemy) {
    // Combat round every turn
    TimerSystem::registerTimer("I-FIGHT", 1, [enemy]() {
        // Process one round of combat
        bool combatContinues = processCombatRound(enemy);
        
        if (!combatContinues) {
            // Combat ended
            TimerSystem::disableTimer("I-FIGHT");
        }
    }, true);
    
    TimerSystem::enableTimer("I-FIGHT");
}

void endCombat() {
    TimerSystem::disableTimer("I-FIGHT");
}
```

## Timer Naming Conventions

Follow the original ZIL naming convention for timer names:

- `I-THIEF` - Thief wandering and actions
- `I-FIGHT` - Combat rounds
- `I-LANTERN` - Lamp battery drain
- `I-CANDLES` - Candle burning
- `I-SWORD` - Sword glow checking
- `I-CYCLOPS` - Cyclops behavior
- etc.

The `I-` prefix stands for "Interrupt" in the original ZIL code.

## Implementation Details

### How Timers Work

1. Each timer has an **interval** (initial tick count) and a **counter** (current countdown)
2. When registered, the counter is set to the interval
3. Each call to `tick()` decrements all enabled timers' counters
4. When a counter reaches 0, the timer's callback fires
5. For repeating timers, the counter resets to the interval
6. For one-shot timers, the timer is disabled after firing

### Performance

- Timer lookup is O(1) using hash map
- Timer processing is O(n) where n is the number of timers
- Typical games have 5-10 active timers, so performance is excellent

### Thread Safety

The timer system is **not thread-safe**. It is designed to be called from the main game loop only.

## Testing

The timer system includes comprehensive unit tests in `tests/timer_system_tests.cpp`:

- Timer registration
- Timer firing at correct intervals
- Enable/disable functionality
- Repeating vs one-shot timers
- Queue operation
- Reset operation
- Multiple timers running simultaneously

Run tests with:

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make timer_system_tests
./timer_system_tests
```

## Integration with Main Loop

Add timer processing to your main loop:

```cpp
void mainLoop1() {
    auto& g = Globals::instance();
    
    // ... parse and execute player command ...
    
    // Process NPC actions
    NPCSystem::processThiefTurn();
    NPCSystem::processTrollTurn();
    NPCSystem::processCyclopsTurn();
    
    // Process all timers
    TimerSystem::tick();
}
```

## Clearing Timers

When restarting the game, clear all timers:

```cpp
void restartGame() {
    TimerSystem::clear();
    // ... reset other game state ...
}
```

## References

- Original implementation: `zil/gclock.zil`
- Design document: `.kiro/specs/zork-complete-port/design.md`
- Requirements: Requirement 44 (Timer System Foundation)

