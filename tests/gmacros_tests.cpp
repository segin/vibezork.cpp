#include "core/globals.h"
#include "core/gmacros.h"
#include "core/object.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "verbs/verbs.h"
#include <cassert>
#include <iostream>
#include <print>
#include <set>
#include <vector>

// ZIL: Test suite for gmacros.zil macros and routines
// Source: zil/gmacros.zil:1-155

void testGMacrosConstants() {
  std::println("Testing GMACROS constants...");
  assert(GMacros::C_ENABLED_FLAG == 0);
  assert(GMacros::C_ENABLED == 1);
  assert(GMacros::C_DISABLED == 0);
  std::println("✓ Constants verified against gmacros.zil:5-7");
}

void testPredicates() {
  std::println("Testing Predicate matchers (VERB?, PRSO?, PRSI?, ROOM?)...");
  auto &g = Globals::instance();
  g.reset();

  auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "elvish sword");
  auto lamp = std::make_unique<ZObject>(ObjectIds::LAMP, "brass lantern");
  auto room = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "In the living room.");

  g.prsa = V_TAKE;
  g.prso = sword.get();
  g.prsi = lamp.get();
  g.here = room.get();

  // Test isVerb (VERB?)
  assert(GMacros::isVerb(V_TAKE));
  assert(GMacros::isVerb(V_DROP, V_TAKE, V_LOOK));
  assert(!GMacros::isVerb(V_DROP, V_LOOK));

  // Test isPrso (PRSO?)
  assert(GMacros::isPrso(sword.get()));
  assert(GMacros::isPrso(ObjectIds::SWORD));
  assert(GMacros::isPrso(ObjectIds::LAMP, ObjectIds::SWORD));
  assert(!GMacros::isPrso(ObjectIds::LAMP, ObjectIds::TROPHY_CASE));

  // Test isPrsi (PRSI?)
  assert(GMacros::isPrsi(lamp.get()));
  assert(GMacros::isPrsi(ObjectIds::LAMP));
  assert(!GMacros::isPrsi(ObjectIds::SWORD));

  // Test isRoom (ROOM?)
  assert(GMacros::isRoom(room.get()));
  assert(GMacros::isRoom(RoomIds::LIVING_ROOM));
  assert(GMacros::isRoom(RoomIds::KITCHEN, RoomIds::LIVING_ROOM));
  assert(!GMacros::isRoom(RoomIds::KITCHEN, RoomIds::ATTIC));

  std::println("✓ Predicates verified against gmacros.zil:52-86");
}

void testMultiBits() {
  std::println("Testing Multi-bit operations (BSET, BCLEAR, BSET?)...");
  auto obj = std::make_unique<ZObject>(1001, "item");

  // Initial state: no flags
  assert(!GMacros::bsetQ(obj.get(), ObjectFlag::TAKEBIT, ObjectFlag::READBIT));

  // Test BSET
  GMacros::bset(obj.get(), ObjectFlag::TAKEBIT, ObjectFlag::READBIT);
  assert(obj->hasFlag(ObjectFlag::TAKEBIT));
  assert(obj->hasFlag(ObjectFlag::READBIT));

  // Test BSET? (returns true if ANY bit matches)
  assert(GMacros::bsetQ(obj.get(), ObjectFlag::TAKEBIT));
  assert(GMacros::bsetQ(obj.get(), ObjectFlag::DOORBIT, ObjectFlag::READBIT));
  assert(!GMacros::bsetQ(obj.get(), ObjectFlag::DOORBIT, ObjectFlag::OPENBIT));

  // Test BCLEAR
  GMacros::bclear(obj.get(), ObjectFlag::TAKEBIT, ObjectFlag::READBIT);
  assert(!obj->hasFlag(ObjectFlag::TAKEBIT));
  assert(!obj->hasFlag(ObjectFlag::READBIT));
  assert(!GMacros::bsetQ(obj.get(), ObjectFlag::TAKEBIT, ObjectFlag::READBIT));

  std::println("✓ Multi-bit operations verified against gmacros.zil:88-110");
}

void testRFatal() {
  std::println("Testing RFATAL...");
  assert(GMacros::rfatal() == 2);
  assert(GMacros::rfatal() == M_FATAL);
  std::println("✓ RFATAL verified against gmacros.zil:112-113");
}

void testProbability() {
  std::println("Testing PROB and ZPROB...");
  auto &g = Globals::instance();
  g.reset();

  // 100% chance and 0% chance boundary checks
  assert(GMacros::prob(101));
  assert(!GMacros::prob(0));

  // ZPROB lucky test
  g.lucky = true;
  assert(GMacros::zprob(101));
  assert(!GMacros::zprob(0));

  // ZPROB unlucky test
  g.lucky = false;
  assert(GMacros::zprob(301));
  assert(!GMacros::zprob(0));

  std::println("✓ Probability routines verified against gmacros.zil:115-123");
}

void testRandomTables() {
  std::println("Testing RANDOM-ELEMENT and PICK-ONE...");

  std::vector<std::string> options = {"Alpha", "Beta", "Gamma", "Delta"};
  std::string randElem = GMacros::randomElement(options);
  assert(!randElem.empty());

  GMacros::ZilRandomTable<std::string> table({"Message 1", "Message 2", "Message 3", "Message 4"});
  assert(table.size() == 4);

  // Test PICK-ONE non-repeating cycle
  // Every cycle of 4 picks MUST produce all 4 distinct messages
  std::set<std::string> cycle1;
  for (size_t i = 0; i < 4; ++i) {
    cycle1.insert(table.pickOne());
  }
  assert(cycle1.size() == 4); // All 4 unique items picked
  assert(table.getCycleCount() == 0); // Reset for next cycle

  // Second cycle should also return all 4 unique items
  std::set<std::string> cycle2;
  for (size_t i = 0; i < 4; ++i) {
    cycle2.insert(table.pickOne());
  }
  assert(cycle2.size() == 4);

  std::println("✓ Random tables verified against gmacros.zil:124-140");
}

void testInterruptControl() {
  std::println("Testing ENABLE and DISABLE macros...");
  TimerSystem::Timer timer("I-TEST", 5, nullptr, true);
  timer.enabled = false;

  GMacros::enable(&timer);
  assert(timer.enabled);

  GMacros::disable(&timer);
  assert(!timer.enabled);

  std::println("✓ Interrupt macros verified against gmacros.zil:141-144");
}

void testObjectQueriesAndMath() {
  std::println("Testing FLAMING?, OPENABLE?, and ABS...");
  auto obj = std::make_unique<ZObject>(1002, "candle");

  // Test FLAMING?
  assert(!GMacros::isFlaming(obj.get()));
  obj->setFlag(ObjectFlag::FLAMEBIT);
  assert(!GMacros::isFlaming(obj.get())); // Missing ONBIT
  obj->setFlag(ObjectFlag::ONBIT);
  assert(GMacros::isFlaming(obj.get())); // Both set

  // Test OPENABLE?
  auto box = std::make_unique<ZObject>(1003, "box");
  assert(!GMacros::isOpenable(box.get()));
  box->setFlag(ObjectFlag::CONTBIT);
  assert(GMacros::isOpenable(box.get()));

  auto door = std::make_unique<ZObject>(1004, "door");
  door->setFlag(ObjectFlag::DOORBIT);
  assert(GMacros::isOpenable(door.get()));

  // Test ABS
  assert(GMacros::absVal(5) == 5);
  assert(GMacros::absVal(-5) == 5);
  assert(GMacros::absVal(0) == 0);
  assert(GMacros::absVal(-42.5) == 42.5);

  std::println("✓ Object queries and ABS verified against gmacros.zil:145-155");
}

int main() {
  std::println("========================================");
  std::println("Running GMACROS Tests (gmacros.zil)");
  std::println("========================================");

  testGMacrosConstants();
  testPredicates();
  testMultiBits();
  testRFatal();
  testProbability();
  testRandomTables();
  testInterruptControl();
  testObjectQueriesAndMath();

  std::println("========================================");
  std::println("All GMACROS Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
