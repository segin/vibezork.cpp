#include "core/globals.h"
#include "core/gmacros.h"
#include "core/gmain.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <cassert>
#include <iostream>
#include <print>
#include <sstream>
#include <vector>

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

  // CCOUNT skips worn objects only (gverbs.zil:1979-1986), so a worn cloak
  // is the thing it must not count.
  auto cloak = std::make_unique<ZObject>(7103, "cloak");
  cloak->setFlag(ObjectFlag::WEARBIT);
  cloak->moveTo(chest.get());

  g.winner = adv.get();
  g.here = room.get();
  adv->moveTo(room.get());

  // Test CCOUNT & FIRSTER
  assert(Verbs::ccount(chest.get()) == 2); // ruby + scenery detail, not the worn cloak
  assert(Verbs::firster(chest.get()) == ruby.get());

  // Test SEE-INSIDE?
  assert(!Verbs::seeInside(chest.get())); // Closed container
  chest->setFlag(ObjectFlag::OPENBIT);
  assert(Verbs::seeInside(chest.get())); // Open container

  // Test WEIGHT
  // 10 + 3 + 5 + 5: the scenery detail and the cloak have no SIZE, so each
  // weighs the <PROPDEF SIZE 5> default (zork1.zil:24). The cloak is only
  // counted as 1 when it is worn by the PLAYER itself (gverbs.zil:1988-1998).
  assert(Verbs::weight(chest.get()) == 23);

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

// ZIL: the debug verbs of gverbs.zil:123-147
void testDebugVerbs() {
  std::println("Testing V-VERIFY, V-RANDOM, V-RECORD, V-UNRECORD, V-COMMAND-FILE...");
  auto &g = Globals::instance();
  g.reset();
  auto intnum = std::make_unique<ZObject>(ObjectIds::INTNUM, "number");
  ZObject *intnumPtr = intnum.get();
  g.registerObject(ObjectIds::INTNUM, std::move(intnum));
  auto other = std::make_unique<ZObject>(7001, "rock");
  ZObject *otherPtr = other.get();
  g.registerObject(7001, std::move(other));
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    assert(Verbs::vVerify());
    std::cout.rdbuf(old);
    assert(buf.str() == "Verifying disk...\nThe disk is correct.\n");
  }
  g.prso = otherPtr;
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    assert(!Verbs::vRandom());
    std::cout.rdbuf(old);
    assert(buf.str() == "Illegal call to #RND.\n");
  }
  // #RANDOM seeds the one generator: the same seed gives the same sequence
  g.prso = intnumPtr;
  g.pNumber = 1984;
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    assert(Verbs::vRandom());
    std::cout.rdbuf(old);
    assert(buf.str().empty());
  }
  std::vector<int> first;
  for (int i = 0; i < 8; ++i) first.push_back(GMacros::random(100));
  bool probFirst = GMacros::prob(50);
  assert(Verbs::vRandom());
  for (int i = 0; i < 8; ++i) assert(GMacros::random(100) == first[i]);
  assert(GMacros::prob(50) == probFirst);
  for (int v : first) assert(v >= 1 && v <= 100);
  // A different seed gives a different sequence (with overwhelming probability)
  g.pNumber = 7;
  assert(Verbs::vRandom());
  std::vector<int> second;
  for (int i = 0; i < 8; ++i) second.push_back(GMacros::random(100));
  assert(first != second);
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    assert(Verbs::vRecord() && Verbs::vUnrecord() && Verbs::vCommandFile());
    std::cout.rdbuf(old);
    assert(buf.str().empty());
  }
  std::println("✓ Debug verbs verified against gverbs.zil:123-147");
}

int main() {
  std::println("========================================");
  std::println("Running GVERBS Tests (gverbs.zil)");
  std::println("========================================");

  testGVerbsPreactions();
  testGVerbsSystemRoutines();
  testDebugVerbs();

  std::println("========================================");
  std::println("All GVERBS Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
