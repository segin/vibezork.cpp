#include "test_framework.h"
#include "systems/timer.h"
#include <iostream>
#include <stdexcept>

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

// Test timer registration
void testTimerRegistration() {
    TimerSystem::TimerManager::instance().clear();
    
    bool fired = false;
    auto callback = [&fired]() { fired = true; };
    
    // Register a timer
    TimerSystem::registerTimer("test-timer", 5, callback, true);
    
    // Verify timer is registered and enabled
    TEST_ASSERT(TimerSystem::isTimerEnabled("test-timer"), 
                "Timer should be enabled after registration");
    
    std::cout << "✓ Timer registration test passed" << std::endl;
}

// Test timer firing
void testTimerFiring() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a timer with interval of 3
    TimerSystem::registerTimer("test-timer", 3, callback, true);
    
    // Tick 1 - should not fire (counter: 3 -> 2)
    bool fired = TimerSystem::tick();
    TEST_ASSERT(!fired, "Timer should not fire on tick 1");
    TEST_ASSERT(fireCount == 0, "Fire count should be 0 after tick 1");
    
    // Tick 2 - should not fire (counter: 2 -> 1)
    fired = TimerSystem::tick();
    TEST_ASSERT(!fired, "Timer should not fire on tick 2");
    TEST_ASSERT(fireCount == 0, "Fire count should be 0 after tick 2");
    
    // Tick 3 - should fire (counter: 1 -> 0, fire, reset to 3)
    fired = TimerSystem::tick();
    TEST_ASSERT(fired, "Timer should fire on tick 3");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1 after tick 3");
    
    // Tick 4 - should not fire (counter: 3 -> 2)
    fired = TimerSystem::tick();
    TEST_ASSERT(!fired, "Timer should not fire on tick 4");
    TEST_ASSERT(fireCount == 1, "Fire count should still be 1 after tick 4");
    
    std::cout << "✓ Timer firing test passed" << std::endl;
}

// Test enable/disable
void testTimerEnableDisable() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a timer with interval of 2
    TimerSystem::registerTimer("test-timer", 2, callback, true);
    
    // Tick 1 - should count down
    TimerSystem::tick();
    
    // Disable the timer
    TimerSystem::disableTimer("test-timer");
    TEST_ASSERT(!TimerSystem::isTimerEnabled("test-timer"), 
                "Timer should be disabled");
    
    // Tick 2 - should not fire because disabled
    bool fired = TimerSystem::tick();
    TEST_ASSERT(!fired, "Disabled timer should not fire");
    TEST_ASSERT(fireCount == 0, "Fire count should be 0 for disabled timer");
    
    // Re-enable the timer
    TimerSystem::enableTimer("test-timer");
    TEST_ASSERT(TimerSystem::isTimerEnabled("test-timer"), 
                "Timer should be enabled");
    
    // Tick 3 - should fire now (counter was at 1 when disabled)
    fired = TimerSystem::tick();
    TEST_ASSERT(fired, "Re-enabled timer should fire");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1 after re-enabling");
    
    std::cout << "✓ Timer enable/disable test passed" << std::endl;
}

// Test repeating timers
void testRepeatingTimer() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a repeating timer with interval of 2
    TimerSystem::registerTimer("test-timer", 2, callback, true);
    
    // Fire multiple times
    for (int i = 0; i < 10; i++) {
        TimerSystem::tick();
    }
    
    // Should have fired 5 times (every 2 ticks)
    TEST_ASSERT(fireCount == 5, "Repeating timer should fire 5 times in 10 ticks");
    
    std::cout << "✓ Repeating timer test passed" << std::endl;
}

// Test one-shot timers
void testOneShotTimer() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a one-shot timer with interval of 2
    TimerSystem::registerTimer("test-timer", 2, callback, false);
    
    // Tick until it fires
    TimerSystem::tick();  // Tick 1
    TimerSystem::tick();  // Tick 2 - should fire
    
    TEST_ASSERT(fireCount == 1, "One-shot timer should fire once");
    TEST_ASSERT(!TimerSystem::isTimerEnabled("test-timer"), 
                "One-shot timer should be disabled after firing");
    
    // Continue ticking - should not fire again
    for (int i = 0; i < 10; i++) {
        TimerSystem::tick();
    }
    
    TEST_ASSERT(fireCount == 1, "One-shot timer should only fire once");
    
    std::cout << "✓ One-shot timer test passed" << std::endl;
}

// Test queue operation
void testQueueTimer() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a timer with interval of 10
    TimerSystem::registerTimer("test-timer", 10, callback, true);
    
    // Queue it to fire in 2 ticks instead
    TimerSystem::queueTimer("test-timer", 2);
    
    // Tick 1 - should not fire
    TimerSystem::tick();
    TEST_ASSERT(fireCount == 0, "Timer should not fire on tick 1");
    
    // Tick 2 - should fire
    bool fired = TimerSystem::tick();
    TEST_ASSERT(fired, "Timer should fire on tick 2 after queue");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1 after queue");
    
    // After firing, should reset to original interval (10)
    for (int i = 0; i < 9; i++) {
        TimerSystem::tick();
    }
    TEST_ASSERT(fireCount == 1, "Timer should not fire again for 10 ticks");
    
    TimerSystem::tick();  // 10th tick
    TEST_ASSERT(fireCount == 2, "Timer should fire again after 10 ticks");
    
    std::cout << "✓ Queue timer test passed" << std::endl;
}

// Test reset operation
void testResetTimer() {
    TimerSystem::TimerManager::instance().clear();
    
    int fireCount = 0;
    auto callback = [&fireCount]() { fireCount++; };
    
    // Register a timer with interval of 3
    TimerSystem::registerTimer("test-timer", 3, callback, true);
    
    // Tick twice
    TimerSystem::tick();  // Counter: 3 -> 2
    TimerSystem::tick();  // Counter: 2 -> 1
    
    // Reset the timer
    TimerSystem::resetTimer("test-timer");
    
    // Should now take 3 more ticks to fire
    TimerSystem::tick();  // Counter: 3 -> 2
    TimerSystem::tick();  // Counter: 2 -> 1
    TEST_ASSERT(fireCount == 0, "Timer should not have fired yet after reset");
    
    TimerSystem::tick();  // Counter: 1 -> 0, fire
    TEST_ASSERT(fireCount == 1, "Timer should fire after 3 ticks from reset");
    
    std::cout << "✓ Reset timer test passed" << std::endl;
}

// Test multiple timers
void testMultipleTimers() {
    TimerSystem::TimerManager::instance().clear();
    
    int fire1 = 0, fire2 = 0, fire3 = 0;
    auto callback1 = [&fire1]() { fire1++; };
    auto callback2 = [&fire2]() { fire2++; };
    auto callback3 = [&fire3]() { fire3++; };
    
    // Register three timers with different intervals
    TimerSystem::registerTimer("timer1", 2, callback1, true);
    TimerSystem::registerTimer("timer2", 3, callback2, true);
    TimerSystem::registerTimer("timer3", 5, callback3, true);
    
    // Tick 10 times
    for (int i = 0; i < 10; i++) {
        TimerSystem::tick();
    }
    
    // Timer1 should fire 5 times (every 2 ticks)
    TEST_ASSERT(fire1 == 5, "Timer1 should fire 5 times");
    
    // Timer2 should fire 3 times (every 3 ticks)
    TEST_ASSERT(fire2 == 3, "Timer2 should fire 3 times");
    
    // Timer3 should fire 2 times (every 5 ticks)
    TEST_ASSERT(fire3 == 2, "Timer3 should fire 2 times");
    
    std::cout << "✓ Multiple timers test passed" << std::endl;
}

int main() {
    std::cout << "Running Timer System Tests..." << std::endl;
    std::cout << std::endl;
    
    try {
        testTimerRegistration();
        testTimerFiring();
        testTimerEnableDisable();
        testRepeatingTimer();
        testOneShotTimer();
        testQueueTimer();
        testResetTimer();
        testMultipleTimers();
        
        std::cout << std::endl;
        std::cout << "All timer system tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

