#include "core/globals.h"
#include "core/gmacros.h"
#include "systems/timer.h"
#include <cassert>
#include <iostream>
#include <print>
#include <string>
#include <vector>

// ZIL: Test suite for gclock.zil routines and constants
// Source: zil/gclock.zil:1-60

void testGClockConstants() {
  std::println("Testing GCLOCK constants...");

  assert(TimerSystem::C_TABLELEN == 180);
  assert(TimerSystem::C_INTLEN == 6);
  assert(TimerSystem::C_MAX_INTERRUPTS == 30);
  assert(TimerSystem::C_ENABLED_FLAG == 0);
  assert(TimerSystem::C_TICK_OFFSET == 1);
  assert(TimerSystem::C_RTN_OFFSET == 2);

  std::println("✓ Constants verified against gclock.zil:5-19");
}

// ZIL: INT finds an existing entry or allocates a new one downward from the
// end of C-TABLE (gclock.zil:26-39); QUEUE stores the tick (21-24)
void testIntAndQueue() {
  std::println("Testing INT and QUEUE routines...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  auto &mgr = TimerSystem::TimerManager::instance();

  int calls = 0;
  TimerSystem::Interrupt *t1 = TimerSystem::interrupt("I-LANTERN", [&]() { calls++; });
  assert(t1 != nullptr);
  assert(t1->name == "I-LANTERN");
  assert(!t1->enabled); // fresh entries are disabled (table words are 0)
  assert(t1->tick == 0);
  assert(mgr.count() == 1);

  // INT finds the existing entry
  TimerSystem::Interrupt *t1Again = TimerSystem::interrupt("I-LANTERN");
  assert(t1 == t1Again);
  assert(mgr.count() == 1);

  // QUEUE sets ticks and returns the entry
  TimerSystem::Interrupt *q = TimerSystem::queue("I-LANTERN", 40);
  assert(q == t1);
  assert(t1->tick == 40);

  // Allocation grows downward: a second interrupt sits before the first
  TimerSystem::Interrupt *t2 = TimerSystem::interrupt("I-FIGHT");
  assert(t2 < t1);
  assert(mgr.count() == 2);
  auto entries = mgr.entries();
  assert(entries.size() == 2);
  assert(entries[0]->name == "I-FIGHT");
  assert(entries[1]->name == "I-LANTERN");

  // ENABLE/DISABLE flip C-ENABLED?
  TimerSystem::enable("I-FIGHT");
  assert(t2->enabled);
  assert(TimerSystem::isEnabled("I-FIGHT"));
  TimerSystem::disable("I-FIGHT");
  assert(!t2->enabled);
  GMacros::enable(t2);
  assert(t2->enabled);
  GMacros::disable(t2);
  assert(!t2->enabled);

  std::println("✓ INT and QUEUE verified against gclock.zil:21-39");
}

// ZIL: an entry fires when its tick was 1 before the decrement and then
// stays at 0 (no re-arm); FLG is the routine's return value (gclock.zil:52-59)
void testClockerCountdown() {
  std::println("Testing CLOCKER countdown...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;

  int candleFired = 0;
  TimerSystem::Interrupt *candle = TimerSystem::interrupt("I-CANDLES", [&]() {
    candleFired++;
    return true;
  });
  TimerSystem::queue("I-CANDLES", 3);
  TimerSystem::enable("I-CANDLES");

  // Turn 1: 3 -> 2
  bool fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 0);
  assert(candle->tick == 2);

  // Turn 2: 2 -> 1
  fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 0);
  assert(candle->tick == 1);

  // Turn 3: tick was 1 -> fires, tick becomes 0
  fired = TimerSystem::clocker();
  assert(fired);
  assert(candleFired == 1);
  assert(candle->tick == 0);
  assert(candle->enabled); // ZIL never disables it; a 0 tick is inert

  // Turn 4: tick 0 -> nothing
  fired = TimerSystem::clocker();
  assert(!fired);
  assert(candleFired == 1);

  std::println("✓ CLOCKER countdown verified against gclock.zil:52-59");
}

// ZIL: a negative tick is "not greater than 1" every turn, so the routine
// runs every turn and the tick keeps decreasing (gclock.zil:54-58)
void testNegativeTickRunsEveryTurn() {
  std::println("Testing negative tick (every turn)...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;

  int fired = 0;
  TimerSystem::Interrupt *t = TimerSystem::interrupt("I-THIEF", [&]() {
    fired++;
    return false;
  });
  TimerSystem::queue("I-THIEF", -1);
  TimerSystem::enable("I-THIEF");

  for (int i = 1; i <= 5; ++i) {
    bool flg = TimerSystem::clocker();
    assert(!flg); // routine returns false -> FLG stays false
    assert(fired == i);
    assert(t->tick == -1 - i);
  }

  // A disabled entry does not run
  TimerSystem::disable("I-THIEF");
  TimerSystem::clocker();
  assert(fired == 5);

  std::println("✓ Negative tick verified against gclock.zil:54-58");
}

// ZIL: interrupts do not re-arm; a routine that wants to run again must
// QUEUE itself (as I-LANTERN/I-CANDLES do via LIGHT-INT)
void testNoAutoRepeatButSelfQueue() {
  std::println("Testing self re-queue...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;

  int fired = 0;
  TimerSystem::interrupt("I-SELF", [&]() {
    fired++;
    TimerSystem::queue("I-SELF", 2);
    return true;
  });
  TimerSystem::queue("I-SELF", 2);
  TimerSystem::enable("I-SELF");

  for (int i = 0; i < 6; ++i) {
    TimerSystem::clocker();
  }
  assert(fired == 3); // turns 2, 4, 6

  int once = 0;
  TimerSystem::interrupt("I-ONCE", [&]() {
    once++;
    return true;
  });
  TimerSystem::queue("I-ONCE", 1);
  TimerSystem::enable("I-ONCE");
  for (int i = 0; i < 4; ++i) {
    TimerSystem::clocker();
  }
  assert(once == 1);

  std::println("✓ No auto-repeat verified against gclock.zil:56");
}

// ZIL: C-INTS grows downward and CLOCKER scans upward, so the most recently
// registered interrupt runs first (gclock.zil:29-39, 46-60)
void testClockerOrderNewestFirst() {
  std::println("Testing CLOCKER traversal order...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;

  std::vector<std::string> order;
  TimerSystem::interrupt("I-FIRST", [&]() {
    order.push_back("I-FIRST");
    return false;
  });
  TimerSystem::interrupt("I-SECOND", [&]() {
    order.push_back("I-SECOND");
    return false;
  });
  TimerSystem::interrupt("I-THIRD", [&]() {
    order.push_back("I-THIRD");
    return false;
  });
  for (const char *name : {"I-FIRST", "I-SECOND", "I-THIRD"}) {
    TimerSystem::queue(name, -1);
    TimerSystem::enable(name);
  }

  TimerSystem::clocker();
  assert(order.size() == 3);
  assert(order[0] == "I-THIRD");
  assert(order[1] == "I-SECOND");
  assert(order[2] == "I-FIRST");

  std::println("✓ Traversal order verified against gclock.zil:46-60");
}

// ZIL: <COND (,CLOCK-WAIT <SETG CLOCK-WAIT <>> <RFALSE>)> (gclock.zil:45)
void testClockWait() {
  std::println("Testing CLOCK-WAIT behavior...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;
  g.clockWait = true;

  int firedCount = 0;
  TimerSystem::Interrupt *t = TimerSystem::interrupt("I-TEST", [&]() {
    firedCount++;
    return true;
  });
  TimerSystem::queue("I-TEST", 1);
  TimerSystem::enable("I-TEST");

  bool result = TimerSystem::clocker();
  assert(!result);
  assert(!g.clockWait);
  assert(firedCount == 0);
  assert(t->tick == 1); // untouched
  assert(g.moves == 0); // MOVES not counted on a skipped pass

  result = TimerSystem::clocker();
  assert(result);
  assert(firedCount == 1);
  assert(g.moves == 1);

  std::println("✓ CLOCK-WAIT verified against gclock.zil:45");
}

// ZIL: FLG is set only when the routine returns true; CLOCKER returns FLG
void testReturnValuePropagation() {
  std::println("Testing return value propagation...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;

  TimerSystem::interrupt("I-FALSE", []() { return false; });
  TimerSystem::queue("I-FALSE", -1);
  TimerSystem::enable("I-FALSE");
  assert(!TimerSystem::clocker());

  TimerSystem::interrupt("I-TRUE", []() { return true; });
  TimerSystem::queue("I-TRUE", -1);
  TimerSystem::enable("I-TRUE");
  assert(TimerSystem::clocker());

  // A void routine counts as returning false
  TimerSystem::disable("I-TRUE");
  int ran = 0;
  TimerSystem::interrupt("I-VOID", [&]() { ran++; });
  TimerSystem::queue("I-VOID", -1);
  TimerSystem::enable("I-VOID");
  assert(!TimerSystem::clocker());
  assert(ran == 1);

  std::println("✓ Return value propagation verified against gclock.zil:57-59");
}

// The table holds exactly C-TABLELEN / C-INTLEN = 30 interrupts
void testTableCapacity() {
  std::println("Testing C-TABLE capacity...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  auto &mgr = TimerSystem::TimerManager::instance();

  for (int i = 0; i < TimerSystem::C_MAX_INTERRUPTS; ++i) {
    TimerSystem::interrupt("I-" + std::to_string(i));
  }
  assert(mgr.count() == TimerSystem::C_MAX_INTERRUPTS);
  bool overflow = false;
  try {
    TimerSystem::interrupt("I-OVERFLOW");
  } catch (const std::exception &) {
    overflow = true;
  }
  assert(overflow);
  TimerSystem::clear();
  assert(mgr.count() == 0);

  std::println("✓ Capacity verified against gclock.zil:5-13");
}

int main() {
  std::println("========================================");
  std::println("Running GCLOCK Tests (gclock.zil)");
  std::println("========================================");

  testGClockConstants();
  testIntAndQueue();
  testClockerCountdown();
  testNegativeTickRunsEveryTurn();
  testNoAutoRepeatButSelfQueue();
  testClockerOrderNewestFirst();
  testClockWait();
  testReturnValuePropagation();
  testTableCapacity();

  std::println("========================================");
  std::println("All GCLOCK Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
