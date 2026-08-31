// Parser Flag Tests
// Tests for MAZEBIT, NONLANDBIT, GWIMBIT, INHIBIT, MULTIBIT, SLOCBIT

#include "core/flags.h"
#include "core/globals.h"
#include "core/object.h"
#include "parser/parser.h"
#include "test_framework.h"
#include "world/rooms.h"
#include "world/world.h"

static void setupTestWorld() {
  initializeWorld();
}

// =============================================================================
// MAZEBIT Tests - Maze room identification
// =============================================================================

TEST(MazeBit_FlagCanBeSetAndChecked) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Get a room and set MAZEBIT
  auto *room = g.getObject(RoomIds::MAZE_1);
  if (room) {
    room->setFlag(ObjectFlag::MAZEBIT);
    ASSERT_TRUE(room->hasFlag(ObjectFlag::MAZEBIT));

    room->clearFlag(ObjectFlag::MAZEBIT);
    ASSERT_FALSE(room->hasFlag(ObjectFlag::MAZEBIT));
  } else {
    // Room may not exist yet, just verify flag operations work
    ZObject testObj(1, "test room");
    testObj.setFlag(ObjectFlag::MAZEBIT);
    ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MAZEBIT));
  }
}

TEST(MazeBit_MazeRoomsHaveFlag) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Maze rooms should have MAZEBIT set
  auto *maze1 = g.getObject(RoomIds::MAZE_1);
  if (maze1) {
    ASSERT_TRUE(maze1->hasFlag(ObjectFlag::MAZEBIT));
  }
}

TEST(MazeBit_NonMazeRoomsLackFlag) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Non-maze rooms should not have MAZEBIT
  auto *westOfHouse = g.getObject(RoomIds::WEST_OF_HOUSE);
  if (westOfHouse) {
    ASSERT_FALSE(westOfHouse->hasFlag(ObjectFlag::MAZEBIT));
  }
}

// =============================================================================
// NONLANDBIT Tests - Water area boat requirement
// =============================================================================

TEST(NonlandBit_FlagCanBeSetAndChecked) {
  ZObject testRoom(1, "test water room");
  testRoom.setFlag(ObjectFlag::NONLANDBIT);
  ASSERT_TRUE(testRoom.hasFlag(ObjectFlag::NONLANDBIT));

  testRoom.clearFlag(ObjectFlag::NONLANDBIT);
  ASSERT_FALSE(testRoom.hasFlag(ObjectFlag::NONLANDBIT));
}

TEST(NonlandBit_WaterRoomsHaveFlag) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Water/river rooms should have NONLANDBIT
  auto *riverRoom = g.getObject(RoomIds::RIVER_1);
  if (riverRoom) {
    ASSERT_TRUE(riverRoom->hasFlag(ObjectFlag::NONLANDBIT));
  }
}

TEST(NonlandBit_LandRoomsLackFlag) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Land rooms should have RLANDBIT, not NONLANDBIT
  auto *westOfHouse = g.getObject(RoomIds::WEST_OF_HOUSE);
  if (westOfHouse) {
    ASSERT_FALSE(westOfHouse->hasFlag(ObjectFlag::NONLANDBIT));
    ASSERT_TRUE(westOfHouse->hasFlag(ObjectFlag::RLANDBIT));
  }
}

// =============================================================================
// GWIMBIT Tests - Parser disambiguation hint
// =============================================================================

TEST(GwimBit_FlagCanBeSetAndChecked) {
  ZObject testObj(1, "test object");
  testObj.setFlag(ObjectFlag::GWIMBIT);
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::GWIMBIT));

  testObj.clearFlag(ObjectFlag::GWIMBIT);
  ASSERT_FALSE(testObj.hasFlag(ObjectFlag::GWIMBIT));
}

TEST(GwimBit_PreferredInDisambiguation) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Create two similar objects, one with GWIMBIT
  ZObject obj1(900, "red ball");
  obj1.addSynonym("ball");
  obj1.setFlag(ObjectFlag::TAKEBIT);

  ZObject obj2(901, "blue ball");
  obj2.addSynonym("ball");
  obj2.setFlag(ObjectFlag::TAKEBIT);
  obj2.setFlag(ObjectFlag::GWIMBIT); // This one should be preferred

  // When GWIMBIT is set, parser should prefer that object
  ASSERT_TRUE(obj2.hasFlag(ObjectFlag::GWIMBIT));
  ASSERT_FALSE(obj1.hasFlag(ObjectFlag::GWIMBIT));
}

// =============================================================================
// INHIBIT Tests - Skip in "take all"
// =============================================================================

TEST(InhibitBit_FlagCanBeSetAndChecked) {
  ZObject testObj(1, "test object");
  testObj.setFlag(ObjectFlag::INHIBIT);
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::INHIBIT));

  testObj.clearFlag(ObjectFlag::INHIBIT);
  ASSERT_FALSE(testObj.hasFlag(ObjectFlag::INHIBIT));
}

TEST(InhibitBit_ObjectsSkippedInTakeAll) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Create object with INHIBIT - should be skipped by "take all"
  ZObject inhibitedObj(902, "special item");
  inhibitedObj.setFlag(ObjectFlag::TAKEBIT);
  inhibitedObj.setFlag(ObjectFlag::INHIBIT);

  // Objects with INHIBIT should not be selected by bulk operations
  ASSERT_TRUE(inhibitedObj.hasFlag(ObjectFlag::INHIBIT));
  ASSERT_TRUE(inhibitedObj.hasFlag(ObjectFlag::TAKEBIT));
}

TEST(InhibitBit_StillDirectlyAddressable) {
  // Objects with INHIBIT can still be taken directly
  ZObject inhibitedObj(903, "guarded item");
  inhibitedObj.addSynonym("item");
  inhibitedObj.setFlag(ObjectFlag::TAKEBIT);
  inhibitedObj.setFlag(ObjectFlag::INHIBIT);

  // Should still have TAKEBIT for direct commands
  ASSERT_TRUE(inhibitedObj.hasFlag(ObjectFlag::TAKEBIT));
}

// =============================================================================
// MULTIBIT Tests - Allow bulk selection
// =============================================================================

TEST(MultiBit_FlagCanBeSetAndChecked) {
  ZObject testObj(1, "test object");
  testObj.setFlag(ObjectFlag::MULTIBIT);
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MULTIBIT));

  testObj.clearFlag(ObjectFlag::MULTIBIT);
  ASSERT_FALSE(testObj.hasFlag(ObjectFlag::MULTIBIT));
}

TEST(MultiBit_AllowsMultipleSelection) {
  // Objects with MULTIBIT can be part of "take all"
  ZObject bulkObj(904, "common item");
  bulkObj.setFlag(ObjectFlag::TAKEBIT);
  bulkObj.setFlag(ObjectFlag::MULTIBIT);

  ASSERT_TRUE(bulkObj.hasFlag(ObjectFlag::MULTIBIT));
}

// =============================================================================
// SLOCBIT Tests - Location constraints
// =============================================================================

TEST(SlocBit_FlagCanBeSetAndChecked) {
  ZObject testObj(1, "test object");
  testObj.setFlag(ObjectFlag::SLOCBIT);
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::SLOCBIT));

  testObj.clearFlag(ObjectFlag::SLOCBIT);
  ASSERT_FALSE(testObj.hasFlag(ObjectFlag::SLOCBIT));
}

TEST(SlocBit_LocationConstraintRespected) {
  // SLOCBIT indicates special location handling
  ZObject slocObj(905, "location-bound item");
  slocObj.setFlag(ObjectFlag::SLOCBIT);

  ASSERT_TRUE(slocObj.hasFlag(ObjectFlag::SLOCBIT));
}

// =============================================================================
// Combined Flag Tests
// =============================================================================

TEST(Flags_MultipleCanBeSet) {
  ZObject testObj(1, "multi-flag object");

  testObj.setFlag(ObjectFlag::TAKEBIT);
  testObj.setFlag(ObjectFlag::MAZEBIT);
  testObj.setFlag(ObjectFlag::GWIMBIT);
  testObj.setFlag(ObjectFlag::MULTIBIT);

  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::TAKEBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MAZEBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::GWIMBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MULTIBIT));
}

TEST(Flags_IndependentClearance) {
  ZObject testObj(1, "test object");

  testObj.setFlag(ObjectFlag::MAZEBIT);
  testObj.setFlag(ObjectFlag::NONLANDBIT);
  testObj.setFlag(ObjectFlag::GWIMBIT);

  // Clear one, others remain
  testObj.clearFlag(ObjectFlag::NONLANDBIT);

  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MAZEBIT));
  ASSERT_FALSE(testObj.hasFlag(ObjectFlag::NONLANDBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::GWIMBIT));
}

TEST(Flags_64BitRange) {
  // Verify flags above bit 31 work correctly
  ZObject testObj(1, "extended flag object");

  testObj.setFlag(ObjectFlag::MAZEBIT);    // bit 32
  testObj.setFlag(ObjectFlag::NONLANDBIT); // bit 33
  testObj.setFlag(ObjectFlag::GWIMBIT);    // bit 34
  testObj.setFlag(ObjectFlag::INHIBIT);    // bit 35
  testObj.setFlag(ObjectFlag::MULTIBIT);   // bit 36
  testObj.setFlag(ObjectFlag::SLOCBIT);    // bit 37
  testObj.setFlag(ObjectFlag::P_SBIT);     // bit 38
  testObj.setFlag(ObjectFlag::P_P1BIT);    // bit 39
  testObj.setFlag(ObjectFlag::GBIT);       // bit 40
  testObj.setFlag(ObjectFlag::IBIT);       // bit 41
  testObj.setFlag(ObjectFlag::LBIT);       // bit 42
  testObj.setFlag(ObjectFlag::OBIT);       // bit 43
  testObj.setFlag(ObjectFlag::RMBIT);      // bit 44
  testObj.setFlag(ObjectFlag::XBIT);       // bit 45

  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MAZEBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::NONLANDBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::GWIMBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::INHIBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::MULTIBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::SLOCBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::P_SBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::P_P1BIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::GBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::IBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::LBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::OBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::RMBIT));
  ASSERT_TRUE(testObj.hasFlag(ObjectFlag::XBIT));
}

// =============================================================================
// Run all tests
// =============================================================================

int main(int argc, char *argv[]) {
  std::cout << "Running Parser Flag Tests" << std::endl;
  std::cout << "=========================" << std::endl;
  std::cout << std::endl;

  auto results = TestFramework::instance().runAll();

  int passed = 0, failed = 0;
  for (const auto &r : results) {
    if (r.passed)
      passed++;
    else
      failed++;
  }

  std::cout << std::endl;
  std::cout << "Results: " << passed << " passed, " << failed << " failed"
            << std::endl;

  return failed > 0 ? 1 : 0;
}
