#include "core/globals.h"
#include "core/gmain.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <cassert>
#include <iostream>
#include <print>

// ZIL: Test suite for gverbs.zil preactions and system routines
// Source: zil/gverbs.zil:1-1530

void testGVerbsPreactions() {
  std::println("Testing GVERBS preaction routines...");
  auto &g = Globals::instance();
  g.reset();

  auto adv = std::make_unique<ZObject>(6001, "adventurer");
  auto room = std::make_unique<ZRoom>(RoomIds::WEST_OF_HOUSE, "West of House",
                                      "You are standing in an open field west of a white house.");
  auto boat = std::make_unique<ZObject>(ObjectIds::INFLATED_BOAT, "magic boat");
  boat->setFlag(ObjectFlag::VEHBIT);
  boat->moveTo(room.get());

  g.winner = adv.get();
  g.here = room.get();
  adv->moveTo(room.get());

  // Test PRE-BOARD
  g.prso = boat.get();
  assert(!Verbs::preBoard()); // Can board

  adv->moveTo(boat.get());
  assert(Verbs::preBoard()); // Already in that

  // Test PRE-DROP inside vehicle
  g.prso = boat.get();
  // Player is in boat, preDrop triggers disembark
  assert(Verbs::preDrop());

  // Test PRE-GIVE
  auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "elvish sword");
  sword->setFlag(ObjectFlag::TAKEBIT);
  sword->moveTo(room.get());
  g.prso = sword.get();
  assert(Verbs::preGive()); // Not held -> blocked

  sword->moveTo(adv.get());
  assert(!Verbs::preGive()); // Held -> allowed

  // Test PRE-MOVE
  assert(Verbs::preMove()); // Held item move is blocked (juggler)

  // Test PRE-MUNG bare hands
  g.prso = sword.get();
  g.prsi = nullptr;
  assert(Verbs::preMung()); // Bare hands warning

  // Test PRE-TAKE
  assert(Verbs::preTake()); // Already have it

  // Test PRE-TURN
  auto bolt = std::make_unique<ZObject>(7001, "bolt");
  g.prso = bolt.get();
  assert(Verbs::preTurn()); // No TURNBIT

  bolt->setFlag(ObjectFlag::TURNBIT);
  auto wrench = std::make_unique<ZObject>(7002, "wrench");
  g.prsi = wrench.get();
  assert(Verbs::preTurn()); // Wrench lacks TOOLBIT

  wrench->setFlag(ObjectFlag::TOOLBIT);
  assert(!Verbs::preTurn()); // Valid turn

  std::println("✓ Preaction routines verified against gverbs.zil");
}

void testGVerbsSystemRoutines() {
  std::println("Testing GVERBS system routines...");
  auto &g = Globals::instance();
  g.reset();

  auto adv = std::make_unique<ZObject>(6001, "adventurer");
  auto room = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room",
                                      "You are in the living room.");
  auto chest = std::make_unique<ZObject>(7100, "wooden chest");
  chest->setFlag(ObjectFlag::CONTBIT);
  chest->setProperty(P_SIZE, 10);
  chest->moveTo(room.get());

  auto ruby = std::make_unique<ZObject>(7101, "ruby");
  ruby->setFlag(ObjectFlag::TAKEBIT);
  ruby->setProperty(P_SIZE, 3);
  ruby->setProperty(P_VALUE, 15);
  ruby->moveTo(chest.get());

  auto hiddenObj = std::make_unique<ZObject>(7102, "scenery detail");
  hiddenObj->setFlag(ObjectFlag::NDESCBIT);
  hiddenObj->moveTo(chest.get());

  g.winner = adv.get();
  g.here = room.get();
  adv->moveTo(room.get());

  // Test CCOUNT & FIRSTER
  assert(Verbs::ccount(chest.get()) == 1); // Only ruby (not hiddenObj)
  assert(Verbs::firster(chest.get()) == ruby.get());

  // Test SEE-INSIDE?
  assert(!Verbs::seeInside(chest.get())); // Closed container
  chest->setFlag(ObjectFlag::OPENBIT);
  assert(Verbs::seeInside(chest.get())); // Open container

  // Test WEIGHT
  assert(Verbs::weight(chest.get()) == 13); // 10 + 3

  // Test HELD?
  assert(!Verbs::isHeld(ruby.get()));
  ruby->moveTo(adv.get());
  assert(Verbs::isHeld(ruby.get()));

  // Test SCORE-OBJ and SCORE-UPD
  int initialScore = g.score;
  Verbs::scoreObj(ruby.get());
  assert(g.score == initialScore + 15);
  assert(ruby->getProperty(P_VALUE) == 0); // Reset to 0

  // Test THIS-IS-IT
  Verbs::thisIsIt(ruby.get());
  assert(g.it == ruby.get());

  // Test FIND-IN
  assert(Verbs::findIn(adv.get(), ObjectFlag::TAKEBIT) == ruby.get());

  // Test MUNG-ROOM
  Verbs::mungRoom(room.get(), "The living room is completely ruined.");
  assert(room->getLongDesc() == "The living room is completely ruined.");

  std::println("✓ System routines verified against gverbs.zil");
}

int main() {
  std::println("========================================");
  std::println("Running GVERBS Tests (gverbs.zil)");
  std::println("========================================");

  testGVerbsPreactions();
  testGVerbsSystemRoutines();

  std::println("========================================");
  std::println("All GVERBS Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
