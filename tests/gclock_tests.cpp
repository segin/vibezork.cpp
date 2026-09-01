#include "core/globals.h"
#include "systems/timer.h"
#include <cassert>
#include <iostream>
#include <print>

// ZIL: Test suite for gclock.zil routines and constants
// Source: zil/gclock.zil:1-61

void testGClockConstants() {
  std::println("Testing GCLOCK constants...");

  assert(TimerSystem::C_TABLELEN == 180);
  assert(TimerSystem::C_INTLEN == 6);
  assert(TimerSystem::C_ENABLED_FLAG == 0);
  assert(TimerSystem::C_TICK_OFFSET == 1);
  assert(TimerSystem::C_RTN_OFFSET == 2);

  std::println("✓ Constants verified against gclock.zil:5-19");
}

void testIntAndQueue() {
  std::println("Testing INT and QUEUE routines...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();

  // Test INT routine allocation (gclock.zil:26-40)
  int calls = 0;
  TimerSystem::Timer *t1 = TimerSystem::interrupt("I-LANTERN", false, [&]() { calls++; });
  assert(t1 != nullptr);
  assert(t1->name == "I-LANTERN");
  assert(!t1->isDemon);

  // Test INT finds existing interrupt
  TimerSystem::Timer *t1_again = TimerSystem::interrupt("I-LANTERN");
  assert(t1 == t1_again);

  // Test QUEUE sets ticks (gclock.zil:21-24)
  TimerSystem::queue("I-LANTERN", 40);
  assert(t1->counter == 40);
  assert(t1->interval == 40);

  // Test Demon registration
  TimerSystem::Timer *demon = TimerSystem::interrupt("I-FIGHT", true);
  assert(demon != nullptr);
  assert(demon->isDemon);

  TimerSystem::queue("I-FIGHT", -1);
  assert(demon->counter == -1);
  assert(demon->isDemon);

  std::println("✓ INT and QUEUE verified against gclock.zil:21-40");
}

void testClockerExecution() {
  std::println("Testing CLOCKER execution...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();

  g.pWon = true;

  int candleFired = 0;
  TimerSystem::Timer *candle =
      TimerSystem::interrupt("I-CANDLES", false, [&]() { candleFired++; });
  TimerSystem::queue("I-CANDLES", 3);
  candle->enabled = true;
  candle->repeating = false;

  // Turn 1: 3 -> 2
  bool fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 0);
  assert(candle->counter == 2);

  // Turn 2: 2 -> 1
  fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 0);
  assert(candle->counter == 1);

  // Turn 3: 1 -> 0 (Fires!)
  fired = TimerSystem::clocker();
  assert(fired);
  assert(candleFired == 1);
  assert(!candle->enabled); // Disabled after one-shot fire

  // Turn 4: Should not fire again
  fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 1);

  std::println("✓ CLOCKER countdown verified against gclock.zil:43-61");
}

void testClockWait() {
  std::println("Testing CLOCK-WAIT behavior...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();

  g.pWon = true;
  g.clockWait = true;

  int firedCount = 0;
  TimerSystem::Timer *t =
      TimerSystem::interrupt("I-TEST", false, [&]() { firedCount++; });
  TimerSystem::queue("I-TEST", 1);
  t->enabled = true;

  // When CLOCK-WAIT is true, CLOCKER skips turn and resets CLOCK-WAIT (gclock.zil:45)
  bool result = TimerSystem::clocker();
  assert(!result);
  assert(!g.clockWait);
  assert(firedCount == 0);
  assert(t->counter == 1); // Unchanged

  // Next turn ticks normally
  result = TimerSystem::clocker();
  assert(result);
  assert(firedCount == 1);

  std::println("✓ CLOCK-WAIT verified against gclock.zil:45");
}

void testDemonExecutionWhenPWonFalse() {
  std::println("Testing Demon execution when P-WON is false...");
  auto &g = Globals::instance();
  g.reset();
  auto adv = std::make_unique<ZObject>(6001, "adventurer");
  g.winner = adv.get();

  int normalFired = 0;
  int demonFired = 0;

  TimerSystem::Timer *norm =
      TimerSystem::interrupt("I-NORMAL", false, [&]() { normalFired++; });
  TimerSystem::queue("I-NORMAL", 1);
  norm->enabled = true;

  TimerSystem::Timer *dem =
      TimerSystem::interrupt("I-DEMON", true, [&]() { demonFired++; });
  TimerSystem::queue("I-DEMON", -1);
  dem->enabled = true;

  // When P-WON is false, only demons run (gclock.zil:46)
  g.pWon = false;
  TimerSystem::clocker();
  assert(normalFired == 0);
  assert(demonFired == 1);

  // When P-WON is true, both run
  g.pWon = true;
  TimerSystem::clocker();
  assert(normalFired == 1);
  assert(demonFired == 2);

  std::println("✓ Demon isolation verified against gclock.zil:46");
}

int main() {
  std::println("========================================");
  std::println("Running GCLOCK Tests (gclock.zil)");
  std::println("========================================");

  testGClockConstants();
  testIntAndQueue();
  testClockerExecution();
  testClockWait();
  testDemonExecutionWhenPWonFalse();

  std::println("========================================");
  std::println("All GCLOCK Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
