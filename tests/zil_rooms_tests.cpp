// Room data built from the generated ZIL model (docs/ZIL_FIDELITY_TODO.md D1).
//
// The cases below are the room-level findings of docs/audit/world_data_audit.md:
// exits the port had invented or scrambled, missing room VALUEs, missing
// (GLOBAL ...) entries and missing (PSEUDO ...) clauses.  Every expectation
// cites the line of zil/1dungeon.zil it comes from.
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/world/generated/world_data.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/world/zil_registry.h"
#include "test_framework.h"

namespace {

ZRoom *room(ObjectId id) {
  return dynamic_cast<ZRoom *>(Globals::instance().getObject(id));
}

void setup() {
  Globals::instance().reset();
  initializeWorld();
}

} // namespace

// =============================================================================
// Coverage: every ROOM of 1dungeon.zil exists and carries its data
// =============================================================================

TEST(ZilRooms_AllRoomsLoaded) {
  setup();
  ASSERT_EQ(zork::zil::kRooms.size(), static_cast<std::size_t>(110));
  for (const auto &def : zork::zil::kRooms) {
    ObjectId id = ZilRegistry::idFor(def.name);
    ASSERT_TRUE(id != 0);
    ZRoom *r = room(id);
    ASSERT_TRUE(r != nullptr);
    ASSERT_EQ(r->getDesc(), std::string(def.desc));
    ASSERT_EQ(r->getLongDesc(), std::string(def.ldesc));
  }
}

TEST(ZilRooms_FlagsAndValues) {
  setup();
  // ZIL: (FLAGS RLANDBIT ONBIT SACREDBIT) (1dungeon.zil:1250)
  ZRoom *west = room(RoomIds::WEST_OF_HOUSE);
  ASSERT_TRUE(west->hasFlag(ObjectFlag::RLANDBIT));
  ASSERT_TRUE(west->hasFlag(ObjectFlag::ONBIT));
  ASSERT_TRUE(west->hasFlag(ObjectFlag::SACREDBIT));

  // ZIL: the four rooms that award points on entry.
  // Source: 1dungeon.zil:1437 (KITCHEN 10), 1474 (CELLAR 25),
  // 1897 (EW-PASSAGE 5), 1756 (TREASURE-ROOM 25)
  ASSERT_EQ(room(RoomIds::KITCHEN)->getProperty(P_VALUE), 10);
  ASSERT_EQ(room(RoomIds::CELLAR)->getProperty(P_VALUE), 25);
  ASSERT_EQ(room(RoomIds::EW_PASSAGE)->getProperty(P_VALUE), 5);
  ASSERT_EQ(room(RoomIds::TREASURE_ROOM)->getProperty(P_VALUE), 25);
  // A room with no (VALUE n) scores nothing.
  ASSERT_EQ(room(RoomIds::WEST_OF_HOUSE)->getProperty(P_VALUE), 0);

  // ZIL: (FLAGS RLANDBIT MAZEBIT) on every maze room, dead ends included.
  // Source: 1dungeon.zil:1546, 1723
  ASSERT_TRUE(room(RoomIds::MAZE_1)->hasFlag(ObjectFlag::MAZEBIT));
  ASSERT_TRUE(room(RoomIds::DEAD_END_1)->hasFlag(ObjectFlag::MAZEBIT));
  // ZIL: (FLAGS NONLANDBIT) - the reservoir and the stream are water.
  // Source: 1dungeon.zil:1958, 2004
  ASSERT_TRUE(room(RoomIds::RESERVOIR)->hasFlag(ObjectFlag::NONLANDBIT));
  ASSERT_TRUE(room(RoomIds::IN_STREAM)->hasFlag(ObjectFlag::NONLANDBIT));
}

// =============================================================================
// Exits the port had invented, dropped or scrambled
// =============================================================================

TEST(ZilRooms_KitchenHasNoNorthExit) {
  setup();
  ZRoom *k = room(RoomIds::KITCHEN);
  // ZIL: KITCHEN has EAST, WEST, OUT, UP and DOWN only (1dungeon.zil:1432-1436).
  ASSERT_TRUE(k->getExit(Direction::NORTH) == nullptr);
  ASSERT_EQ(k->getExit(Direction::WEST)->targetRoom, RoomIds::LIVING_ROOM);
  ASSERT_EQ(k->getExit(Direction::UP)->targetRoom, RoomIds::ATTIC);

  // ZIL: (DOWN TO STUDIO IF FALSE-FLAG ELSE "Only Santa Claus climbs down
  // chimneys.") - a flag that is never set, so the message always prints.
  // Source: 1dungeon.zil:1436
  const RoomExit *down = k->getExit(Direction::DOWN);
  ASSERT_TRUE(down != nullptr);
  ASSERT_TRUE(down->condition != nullptr);
  ASSERT_FALSE(down->condition());
  ASSERT_EQ(down->message, std::string("Only Santa Claus climbs down chimneys."));
}

TEST(ZilRooms_ReservoirSouthExits) {
  setup();
  ZRoom *rs = room(RoomIds::RESERVOIR_SOUTH);
  // ZIL: (SE TO DEEP-CANYON) (EAST TO DAM-ROOM) (SW TO CHASM-ROOM)
  // (WEST TO STREAM-VIEW) plus the NORTH crossing.
  // Source: 1dungeon.zil:1934-1941
  ASSERT_EQ(rs->getExit(Direction::SE)->targetRoom, RoomIds::DEEP_CANYON);
  ASSERT_EQ(rs->getExit(Direction::EAST)->targetRoom, RoomIds::DAM_ROOM);
  ASSERT_EQ(rs->getExit(Direction::SW)->targetRoom, RoomIds::CHASM_ROOM);
  ASSERT_EQ(rs->getExit(Direction::WEST)->targetRoom, RoomIds::STREAM_VIEW);
  ASSERT_TRUE(rs->getExit(Direction::SOUTH) == nullptr);

  // ZIL: (NORTH TO RESERVOIR IF LOW-TIDE ELSE "You would drown.")
  // Source: 1dungeon.zil:1936-1937
  const RoomExit *north = rs->getExit(Direction::NORTH);
  ASSERT_TRUE(north->condition != nullptr);
  Globals::instance().lowTide = false;
  ASSERT_FALSE(north->condition());
  Globals::instance().lowTide = true;
  ASSERT_TRUE(north->condition());
}

TEST(ZilRooms_CanyonViewAndGratingClearing) {
  setup();
  // ZIL: (EAST TO CLIFF-MIDDLE) (DOWN TO CLIFF-MIDDLE) (1dungeon.zil:2458-2459)
  ZRoom *cv = room(RoomIds::CANYON_VIEW);
  ASSERT_EQ(cv->getExit(Direction::EAST)->targetRoom, RoomIds::CLIFF_MIDDLE);
  ASSERT_EQ(cv->getExit(Direction::DOWN)->targetRoom, RoomIds::CLIFF_MIDDLE);

  // ZIL: GRATING-CLEARING goes EAST to FOREST-2, not to the canyon, and has
  // no SW exit.  Source: 1dungeon.zil:1391-1395
  ZRoom *gc = room(RoomIds::GRATING_CLEARING);
  ASSERT_EQ(gc->getExit(Direction::EAST)->targetRoom, RoomIds::FOREST_2);
  ASSERT_TRUE(gc->getExit(Direction::SW) == nullptr);
  // (DOWN PER GRATING-EXIT)
  ASSERT_TRUE(gc->getExit(Direction::DOWN)->type == ExitType::PROCEDURAL);
}

TEST(ZilRooms_NoInventedExits) {
  setup();
  // ZIL: EAST-OF-HOUSE has no NE/SE exits (1dungeon.zil:1298-1304).
  ZRoom *east = room(RoomIds::EAST_OF_HOUSE);
  ASSERT_TRUE(east->getExit(Direction::NE) == nullptr);
  ASSERT_TRUE(east->getExit(Direction::SE) == nullptr);

  // ZIL: STRANGE-PASSAGE has EAST and WEST only (1dungeon.zil:1746-1748).
  ZRoom *sp = room(RoomIds::STRANGE_PASSAGE);
  ASSERT_TRUE(sp->getExit(Direction::NORTH) == nullptr);
  ASSERT_TRUE(sp->getExit(Direction::SOUTH) == nullptr);
  ASSERT_TRUE(sp->getExit(Direction::OUT) == nullptr);

  // ZIL: RIVER-5 has no DOWN exit; the falls are reached by drifting
  // (1dungeon.zil:2233-2238).
  ASSERT_TRUE(room(RoomIds::RIVER_5)->getExit(Direction::DOWN) == nullptr);

  // ZIL: (LAND TO STREAM-VIEW) is how you leave the stream
  // (1dungeon.zil:2010).
  ASSERT_EQ(room(RoomIds::IN_STREAM)->getExit(Direction::LAND)->targetRoom,
            RoomIds::STREAM_VIEW);
}

TEST(ZilRooms_CellarRefusalText) {
  setup();
  // ZIL: (WEST "You try to ascend the ramp, but it is impossible, and you
  // slide back down.")  Source: 1dungeon.zil:1472
  const RoomExit *west = room(RoomIds::CELLAR)->getExit(Direction::WEST);
  ASSERT_TRUE(west != nullptr);
  ASSERT_EQ(west->message,
            std::string("You try to ascend the ramp, but it is impossible, and "
                        "you slide back down."));
}

TEST(ZilRooms_MazeSelfLoopsAndDiodes) {
  setup();
  // ZIL: the maze rooms that lead back to themselves (1dungeon.zil:1543, 1605,
  // 1625, 1645, 1710) and the mine's four (2570, 2578, 2587, 2597).
  ASSERT_EQ(room(RoomIds::MAZE_1)->getExit(Direction::NORTH)->targetRoom,
            RoomIds::MAZE_1);
  ASSERT_EQ(room(RoomIds::MINE_1)->getExit(Direction::EAST)->targetRoom,
            RoomIds::MINE_1);

  // ZIL: (DOWN PER MAZE-DIODES) in MAZE-2, MAZE-7, MAZE-9 and MAZE-12; the
  // destination lives in the routine, not in the room data.
  // Source: 1dungeon.zil:1553, 1614, 1642, 1682 and 1actions.zil:898-905
  for (ObjectId id : {RoomIds::MAZE_2, RoomIds::MAZE_7, RoomIds::MAZE_9,
                      RoomIds::MAZE_12}) {
    const RoomExit *down = room(id)->getExit(Direction::DOWN);
    ASSERT_TRUE(down != nullptr);
    ASSERT_TRUE(down->type == ExitType::PROCEDURAL);
    ASSERT_TRUE(down->procedural != nullptr);
  }
}

TEST(ZilRooms_DoorExits) {
  setup();
  // ZIL: (WEST TO KITCHEN IF KITCHEN-WINDOW IS OPEN) (1dungeon.zil:1303)
  const RoomExit *w = room(RoomIds::EAST_OF_HOUSE)->getExit(Direction::WEST);
  ASSERT_TRUE(w->type == ExitType::DOOR);
  ASSERT_EQ(w->doorObject, ObjectIds::KITCHEN_WINDOW);
  ASSERT_EQ(w->targetRoom, RoomIds::KITCHEN);

  // ZIL: (UP TO LIVING-ROOM IF TRAP-DOOR IS OPEN ELSE "The trap door is
  // closed.")  Source: 1dungeon.zil:1471
  const RoomExit *up = room(RoomIds::CELLAR)->getExit(Direction::UP);
  ASSERT_TRUE(up->type == ExitType::DOOR);
  ASSERT_EQ(up->doorObject, ObjectIds::TRAP_DOOR);
}

// =============================================================================
// (GLOBAL ...) and (PSEUDO ...) clauses
// =============================================================================

TEST(ZilRooms_GlobalLists) {
  setup();
  // ZIL: (GLOBAL WHITE-HOUSE BOARD FOREST) (1dungeon.zil:1252)
  ZRoom *west = room(RoomIds::WEST_OF_HOUSE);
  ASSERT_TRUE(west->hasGlobal(ObjectIds::WHITE_HOUSE));
  ASSERT_TRUE(west->hasGlobal(ObjectIds::BOARD));
  ASSERT_TRUE(west->hasGlobal(ObjectIds::FOREST));

  // ZIL: STAIRS is reachable in the rooms that list it (1dungeon.zil:1438,
  // 1448, 1457, 1475).
  ASSERT_TRUE(room(RoomIds::KITCHEN)->hasGlobal(ObjectIds::STAIRS));
  ASSERT_TRUE(room(RoomIds::ATTIC)->hasGlobal(ObjectIds::STAIRS));
  ASSERT_TRUE(room(RoomIds::LIVING_ROOM)->hasGlobal(ObjectIds::STAIRS));
  ASSERT_TRUE(room(RoomIds::CELLAR)->hasGlobal(ObjectIds::STAIRS));

  // ZIL: (GLOBAL TREE SONGBIRD WHITE-HOUSE FOREST) (1dungeon.zil:1320)
  ZRoom *f1 = room(RoomIds::FOREST_1);
  ASSERT_TRUE(f1->hasGlobal(ObjectIds::TREE));
  ASSERT_TRUE(f1->hasGlobal(ObjectIds::SONGBIRD));
  ASSERT_TRUE(f1->hasGlobal(ObjectIds::FOREST));

  // ZIL: the water rooms reach GLOBAL-WATER and RIVER (1dungeon.zil:1943, 2195).
  ASSERT_TRUE(room(RoomIds::RESERVOIR_SOUTH)->hasGlobal(ObjectIds::GLOBAL_WATER));
  ASSERT_TRUE(room(RoomIds::RIVER_1)->hasGlobal(ObjectIds::RIVER));
}

TEST(ZilRooms_PseudoClauses) {
  setup();
  // ZIL: (PSEUDO "NAILS" NAILS-PSEUDO "NAIL" NAILS-PSEUDO) (1dungeon.zil:1458)
  auto pseudos = room(RoomIds::LIVING_ROOM)->getPseudos();
  ASSERT_EQ(pseudos.size(), static_cast<std::size_t>(2));
  ASSERT_EQ(pseudos[0].word, std::string("NAILS"));
  ASSERT_EQ(pseudos[1].word, std::string("NAIL"));
  ASSERT_TRUE(pseudos[0].action != nullptr);

  // ZIL: (PSEUDO "CHASM" CHASM-PSEUDO) (1dungeon.zil:1504)
  auto chasm = room(RoomIds::EAST_OF_CHASM)->getPseudos();
  ASSERT_EQ(chasm.size(), static_cast<std::size_t>(1));
  ASSERT_EQ(chasm[0].word, std::string("CHASM"));

  // ZIL: (PSEUDO "DOOR" DOOR-PSEUDO "PAINT" PAINT-PSEUDO) (1dungeon.zil:1530)
  ASSERT_EQ(room(RoomIds::STUDIO)->getPseudos().size(),
            static_cast<std::size_t>(2));
}

TEST(ZilRooms_ActionsBound) {
  setup();
  // A sample of the (ACTION ...) clauses: the routine must actually be bound.
  // Source: 1dungeon.zil:1251 (WEST-HOUSE), 1492 (TROLL-ROOM-F),
  // 1737 (CYCLOPS-ROOM-FCN), 2411 (CANYON-VIEW-F)
  ASSERT_TRUE(room(RoomIds::WEST_OF_HOUSE)->hasRoomAction());
  ASSERT_TRUE(room(RoomIds::TROLL_ROOM)->hasRoomAction());
  ASSERT_TRUE(room(RoomIds::CYCLOPS_ROOM)->hasRoomAction());
  ASSERT_TRUE(room(RoomIds::CANYON_VIEW)->hasRoomAction());
  // A room with no (ACTION ...) has none.  Source: 1dungeon.zil:1267 (MOUNTAINS)
  ASSERT_FALSE(room(RoomIds::MOUNTAINS)->hasRoomAction());
}

int main() {
  std::cout << "Running ZIL Room Data Tests" << std::endl;
  std::cout << "===========================" << std::endl;

  auto results = TestFramework::instance().runAll();

  int passed = 0, failed = 0;
  for (const auto &r : results) {
    if (r.passed)
      passed++;
    else
      failed++;
  }

  std::cout << "\nResults: " << passed << " passed, " << failed << " failed"
            << std::endl;
  return failed > 0 ? 1 : 0;
}
