#include "parser/gparser.h"
#include "core/globals.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <cassert>
#include <iostream>
#include <print>
#include <vector>

// ZIL: Test suite for gparser.zil routines and tables
// Source: zil/gparser.zil:1-1407

void testTableAndUtilityRoutines() {
  std::println("Testing table and utility routines (ZMEMQ, ZMEMQB, STUFF, INBUF)...");

  // ZMEMQ
  std::vector<int> nums = {10, 20, 30, 40};
  assert(GParser::zmemq(20, nums));
  assert(!GParser::zmemq(50, nums));

  // ZMEMQB
  std::vector<uint8_t> bytes = {1, 2, 3, 5};
  assert(GParser::zmemqb(3, bytes));
  assert(!GParser::zmemqb(4, bytes));

  // STUFF
  std::vector<std::string> src = {"take", "sword"};
  std::vector<std::string> dest;
  GParser::stuff(src, dest);
  assert(dest.size() == 2 && dest[0] == "take" && dest[1] == "sword");

  // INBUF-STUFF
  std::string sBuf = "hello zork";
  std::string dBuf;
  GParser::inbufStuff(sBuf, dBuf);
  assert(dBuf == "hello zork");

  // INBUF-ADD
  std::string inBuf = "take";
  GParser::inbufAdd(inBuf, "lamp");
  assert(inBuf == "take lamp");

  // WT?
  assert(GParser::wt("the", GParser::PS_BUZZ_WORD));
  assert(!GParser::wt("", GParser::PS_BUZZ_WORD));

  std::println("✓ Table and utility routines verified");
}

void testNumberParsing() {
  std::println("Testing NUMBER? routine (gparser.zil:512-535)...");

  auto num1 = GParser::parseNumber("42");
  assert(num1.has_value() && *num1 == 42);

  auto num2 = GParser::parseNumber("0");
  assert(num2.has_value() && *num2 == 0);

  // Time format HH:MM (minutes past 00:00)
  // 10:30 -> 10 * 60 + 30 = 630
  auto time1 = GParser::parseNumber("10:30");
  assert(time1.has_value() && *time1 == 630);

  // 2:15 -> (< 8, add 12 -> 14) 14 * 60 + 15 = 855
  auto time2 = GParser::parseNumber("2:15");
  assert(time2.has_value() && *time2 == 855);

  auto invalid = GParser::parseNumber("abc");
  assert(!invalid.has_value());

  std::println("✓ NUMBER? verified against gparser.zil:512-535");
}

void testObjectVisibilityAndAccess() {
  std::println("Testing LIT?, THIS-IT?, META-LOC, ACCESSIBLE?...");
  auto &g = Globals::instance();
  g.reset();

  auto room = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "In the living room.");
  auto darkRoom = std::make_unique<ZRoom>(RoomIds::CELLAR, "Cellar", "In the dark cellar.");
  auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "elvish sword");
  sword->addSynonym("sword");
  sword->addAdjective("elvish");

  auto lamp = std::make_unique<ZObject>(ObjectIds::LAMP, "brass lantern");
  lamp->addSynonym("lantern");
  lamp->addSynonym("lamp");

  auto chest = std::make_unique<ZObject>(1005, "chest");
  chest->setFlag(ObjectFlag::CONTBIT);

  auto adventurer = std::make_unique<ZObject>(ObjectIds::ADVENTURER, "adventurer");
  g.player = adventurer.get();
  g.winner = adventurer.get();
  g.here = room.get();

  // LIT? test
  room->setFlag(ObjectFlag::ONBIT);
  assert(GParser::isLit(room.get()));

  darkRoom->clearFlag(ObjectFlag::ONBIT);
  g.here = darkRoom.get();
  assert(!GParser::isLit(darkRoom.get()));

  // Add lit lamp to winner
  lamp->setFlag(ObjectFlag::ONBIT);
  lamp->moveTo(g.winner);
  assert(GParser::isLit(darkRoom.get()));

  // THIS-IT? test
  assert(GParser::thisIt(sword.get(), "sword", "elvish"));
  assert(GParser::thisIt(sword.get(), "sword"));
  assert(!GParser::thisIt(sword.get(), "lamp"));
  assert(!GParser::thisIt(sword.get(), "sword", "rusty"));

  // META-LOC test
  sword->moveTo(room.get());
  assert(GParser::metaLoc(sword.get()) == room.get());

  sword->moveTo(chest.get());
  chest->moveTo(room.get());
  assert(GParser::metaLoc(sword.get()) == room.get());

  // ACCESSIBLE? test
  g.here = room.get();
  // sword is inside closed chest
  chest->clearFlag(ObjectFlag::OPENBIT);
  assert(!GParser::isAccessible(sword.get()));

  // open chest
  chest->setFlag(ObjectFlag::OPENBIT);
  assert(GParser::isAccessible(sword.get()));

  std::println("✓ Visibility and accessibility verified against gparser.zil:1333-1407");
}

void testSearchAndResolution() {
  std::println("Testing DO-SL, SEARCH-LIST, GET-OBJECT, BUT-MERGE, GWIM...");
  auto &g = Globals::instance();
  g.reset();

  auto room = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "In the living room.");
  auto adventurer = std::make_unique<ZObject>(ObjectIds::ADVENTURER, "adventurer");
  g.player = adventurer.get();
  g.winner = adventurer.get();
  g.here = room.get();
  room->setFlag(ObjectFlag::ONBIT);

  auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "elvish sword");
  sword->addSynonym("sword");
  sword->moveTo(room.get());

  auto knife = std::make_unique<ZObject>(ObjectIds::KNIFE, "nasty knife");
  knife->addSynonym("knife");
  knife->moveTo(room.get());

  // SEARCH-LIST
  std::vector<ZObject *> found;
  GParser::searchList(room.get(), found, GParser::P_SRCALL, "sword");
  assert(found.size() == 1 && found[0] == sword.get());

  // BUT-MERGE
  std::vector<ZObject *> allObjs = {sword.get(), knife.get()};
  std::vector<ZObject *> buts = {knife.get()};
  auto merged = GParser::butMerge(allObjs, buts);
  assert(merged.size() == 1 && merged[0] == sword.get());

  // GWIM test: single weapon
  sword->setFlag(ObjectFlag::WEAPONBIT);
  knife->clearFlag(ObjectFlag::WEAPONBIT);
  auto *gwimObj = GParser::gwim(static_cast<uint64_t>(ObjectFlag::WEAPONBIT), GParser::SIR | GParser::SOG);
  assert(gwimObj == sword.get());

  std::println("✓ Search and resolution verified against gparser.zil:901-1237");
}

void testSyntaxAndOrphaning() {
  std::println("Testing SYNTAX-CHECK, ORPHAN, ORPHAN-MERGE, TAKE-CHECK, MANY-CHECK...");
  auto &g = Globals::instance();
  g.reset();

  auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "elvish sword");
  auto room = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "In the living room.");
  sword->moveTo(room.get());
  g.here = room.get();

  // ORPHAN
  GParser::orphan(1, 0);
  assert(g.pOflag);

  // ORPHAN-MERGE
  std::vector<std::string> tokens = {"sword"};
  assert(GParser::orphanMerge(tokens));
  assert(!g.pOflag);
  assert(g.pMerged);

  // TAKE-CHECK / ITAKE-CHECK
  assert(GParser::iTakeCheck(sword.get(), GParser::STAKE));

  // MANY-CHECK
  assert(GParser::manyCheck(V_TAKE, 1, 0));
  assert(!GParser::manyCheck(V_TAKE, 2, 0));

  std::println("✓ Syntax and orphaning verified against gparser.zil:543-1313");
}

int main() {
  std::println("========================================");
  std::println("Running GPARSER Tests (gparser.zil)");
  std::println("========================================");

  testTableAndUtilityRoutines();
  testNumberParsing();
  testObjectVisibilityAndAccess();
  testSearchAndResolution();
  testSyntaxAndOrphaning();

  std::println("========================================");
  std::println("All GPARSER Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
