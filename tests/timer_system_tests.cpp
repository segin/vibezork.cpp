#include "test_framework.h"
#include "core/globals.h"
#include "systems/timer.h"
#include <iostream>
#include <stdexcept>

// Behavioural tests for the interrupt API used by the game systems
// ZIL: gclock.zil:21-60 (INT, QUEUE, CLOCKER), gmacros.zil:141-143 (ENABLE/DISABLE)

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

static void fresh() {
    Globals::instance().reset();
    Globals::instance().pWon = true;
    TimerSystem::clear();
}

// INT registers a routine; the entry starts disabled with tick 0
void testInterruptRegistration() {
    fresh();

    bool fired = false;
    TimerSystem::interrupt("test-timer", [&fired]() { fired = true; });

    TEST_ASSERT(!TimerSystem::isEnabled("test-timer"),
                "A freshly registered interrupt is disabled");
    TimerSystem::queue("test-timer", 5);
    TimerSystem::enable("test-timer");
    TEST_ASSERT(TimerSystem::isEnabled("test-timer"),
                "ENABLE turns the interrupt on");
    TEST_ASSERT(!fired, "Nothing fires before CLOCKER runs");

    std::cout << "✓ Interrupt registration test passed" << std::endl;
}

// QUEUE n fires on the n-th CLOCKER pass and then goes inert
void testInterruptFiring() {
    fresh();

    int fireCount = 0;
    TimerSystem::interrupt("test-timer", [&fireCount]() { fireCount++; return true; });
    TimerSystem::queue("test-timer", 3);
    TimerSystem::enable("test-timer");

    bool fired = TimerSystem::clocker();
    TEST_ASSERT(!fired, "Should not fire on pass 1");
    TEST_ASSERT(fireCount == 0, "Fire count should be 0 after pass 1");

    fired = TimerSystem::clocker();
    TEST_ASSERT(!fired, "Should not fire on pass 2");

    fired = TimerSystem::clocker();
    TEST_ASSERT(fired, "Should fire on pass 3");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1 after pass 3");

    fired = TimerSystem::clocker();
    TEST_ASSERT(!fired, "Tick 0 is inert: no re-arm");
    TEST_ASSERT(fireCount == 1, "Fire count should still be 1");

    std::cout << "✓ Interrupt firing test passed" << std::endl;
}

// DISABLE stops the countdown where it is; ENABLE resumes it
void testEnableDisable() {
    fresh();

    int fireCount = 0;
    TimerSystem::interrupt("test-timer", [&fireCount]() { fireCount++; return true; });
    TimerSystem::queue("test-timer", 2);
    TimerSystem::enable("test-timer");

    TimerSystem::clocker(); // 2 -> 1

    TimerSystem::disable("test-timer");
    TEST_ASSERT(!TimerSystem::isEnabled("test-timer"), "Should be disabled");

    bool fired = TimerSystem::clocker();
    TEST_ASSERT(!fired, "Disabled interrupt should not fire");
    TEST_ASSERT(fireCount == 0, "Fire count should be 0 while disabled");

    TimerSystem::enable("test-timer");
    fired = TimerSystem::clocker(); // tick was 1 -> fires
    TEST_ASSERT(fired, "Re-enabled interrupt should fire");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1 after re-enabling");

    std::cout << "✓ Enable/disable test passed" << std::endl;
}

// A negative tick runs the routine every turn
void testEveryTurnInterrupt() {
    fresh();

    int fireCount = 0;
    TimerSystem::interrupt("test-timer", [&fireCount]() { fireCount++; return false; });
    TimerSystem::queue("test-timer", -1);
    TimerSystem::enable("test-timer");

    for (int i = 0; i < 10; i++) {
        TimerSystem::clocker();
    }
    TEST_ASSERT(fireCount == 10, "Every-turn interrupt should fire 10 times in 10 passes");

    std::cout << "✓ Every-turn interrupt test passed" << std::endl;
}

// A routine may re-QUEUE itself to run periodically
void testSelfRequeue() {
    fresh();

    int fireCount = 0;
    TimerSystem::interrupt("test-timer", [&fireCount]() {
        fireCount++;
        TimerSystem::queue("test-timer", 2);
        return true;
    });
    TimerSystem::queue("test-timer", 2);
    TimerSystem::enable("test-timer");

    for (int i = 0; i < 10; i++) {
        TimerSystem::clocker();
    }
    TEST_ASSERT(fireCount == 5, "Self re-queueing routine fires every 2 passes");

    std::cout << "✓ Self re-queue test passed" << std::endl;
}

// QUEUE overrides an existing countdown
void testRequeueOverrides() {
    fresh();

    int fireCount = 0;
    TimerSystem::interrupt("test-timer", [&fireCount]() { fireCount++; return true; });
    TimerSystem::queue("test-timer", 10);
    TimerSystem::enable("test-timer");

    TimerSystem::queue("test-timer", 2);

    TimerSystem::clocker();
    TEST_ASSERT(fireCount == 0, "Should not fire on pass 1");

    bool fired = TimerSystem::clocker();
    TEST_ASSERT(fired, "Should fire on pass 2 after re-queue");
    TEST_ASSERT(fireCount == 1, "Fire count should be 1");

    for (int i = 0; i < 10; i++) {
        TimerSystem::clocker();
    }
    TEST_ASSERT(fireCount == 1, "No re-arm without a new QUEUE");

    std::cout << "✓ Re-queue override test passed" << std::endl;
}

// Multiple interrupts count down independently
void testMultipleInterrupts() {
    fresh();

    int fire1 = 0, fire2 = 0, fire3 = 0;
    TimerSystem::interrupt("timer1", [&fire1]() { fire1++; TimerSystem::queue("timer1", 2); return true; });
    TimerSystem::interrupt("timer2", [&fire2]() { fire2++; TimerSystem::queue("timer2", 3); return true; });
    TimerSystem::interrupt("timer3", [&fire3]() { fire3++; TimerSystem::queue("timer3", 5); return true; });
    TimerSystem::queue("timer1", 2);
    TimerSystem::queue("timer2", 3);
    TimerSystem::queue("timer3", 5);
    TimerSystem::enable("timer1");
    TimerSystem::enable("timer2");
    TimerSystem::enable("timer3");

    for (int i = 0; i < 10; i++) {
        TimerSystem::clocker();
    }

    TEST_ASSERT(fire1 == 5, "timer1 should fire 5 times");
    TEST_ASSERT(fire2 == 3, "timer2 should fire 3 times");
    TEST_ASSERT(fire3 == 2, "timer3 should fire 2 times");

    std::cout << "✓ Multiple interrupts test passed" << std::endl;
}

// State round trip used by save/restore
void testStateRoundTrip() {
    fresh();
    auto& mgr = TimerSystem::TimerManager::instance();

    TimerSystem::interrupt("I-LANTERN", []() { return false; });
    TimerSystem::queue("I-LANTERN", 200);
    TEST_ASSERT(mgr.find("I-LANTERN")->tick == 200, "tick stored");

    mgr.setInterruptState("I-LANTERN", true, 7);
    TEST_ASSERT(mgr.find("I-LANTERN")->enabled, "enabled restored");
    TEST_ASSERT(mgr.find("I-LANTERN")->tick == 7, "tick restored");
    TEST_ASSERT(mgr.entries().size() == 1, "one entry");

    std::cout << "✓ State round trip test passed" << std::endl;
}

int main() {
    std::cout << "Running Timer System Tests..." << std::endl;
    std::cout << std::endl;

    try {
        testInterruptRegistration();
        testInterruptFiring();
        testEnableDisable();
        testEveryTurnInterrupt();
        testSelfRequeue();
        testRequeueOverrides();
        testMultipleInterrupts();
        testStateRoundTrip();

        std::cout << std::endl;
        std::cout << "All timer system tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
