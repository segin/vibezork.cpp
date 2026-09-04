// Unit test suite for ZIL 1dungeon.zil routines, tables, exits, and objects
#include "test_framework.h"
#include "core/globals.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"
#include "world/dungeon.h"
#include "systems/death.h"
#include <sstream>
#include <iostream>

class OutputCapture {
public:
  OutputCapture() : old_cout(std::cout.rdbuf()) {
    std::cout.rdbuf(buffer.rdbuf());
  }
  ~OutputCapture() { std::cout.rdbuf(old_cout); }
  std::string getOutput() const { return buffer.str(); }

private:
  std::stringstream buffer;
  std::streambuf *old_cout;
};

// =============================================================================
// Constants & Random Walk Tables (1dungeon.zil:7, 9, 2620-2633)
// =============================================================================

TEST(DungeonConstants_ScoreMaxAndFalseFlag) {
  ASSERT_EQ(Dungeon::SCORE_MAX, 350);
  ASSERT_FALSE(Dungeon::FALSE_FLAG);
}

TEST(DungeonTables_WalkAroundTables) {
  // HOUSE_AROUND: WEST, NORTH, EAST, SOUTH, WEST (1dungeon.zil:2620)
  ASSERT_EQ(Dungeon::HOUSE_AROUND[0], ROOM_WEST_OF_HOUSE);
  ASSERT_EQ(Dungeon::HOUSE_AROUND[1], ROOM_NORTH_OF_HOUSE);
  ASSERT_EQ(Dungeon::HOUSE_AROUND[2], ROOM_EAST_OF_HOUSE);
  ASSERT_EQ(Dungeon::HOUSE_AROUND[3], ROOM_SOUTH_OF_HOUSE);
  ASSERT_EQ(Dungeon::HOUSE_AROUND[4], ROOM_WEST_OF_HOUSE);

  // FOREST_AROUND: FOREST_1, FOREST_2, FOREST_3, FOREST_PATH, CLEARING, FOREST_1 (1dungeon.zil:2625)
  ASSERT_EQ(Dungeon::FOREST_AROUND[0], RoomIds::FOREST_1);
  ASSERT_EQ(Dungeon::FOREST_AROUND[1], RoomIds::FOREST_2);
  ASSERT_EQ(Dungeon::FOREST_AROUND[2], RoomIds::FOREST_3);
  ASSERT_EQ(Dungeon::FOREST_AROUND[3], RoomIds::FOREST_PATH);
  ASSERT_EQ(Dungeon::FOREST_AROUND[4], RoomIds::CLEARING);
  ASSERT_EQ(Dungeon::FOREST_AROUND[5], RoomIds::FOREST_1);

  // IN_HOUSE_AROUND: LIVING_ROOM, KITCHEN, ATTIC, KITCHEN (1dungeon.zil:2629)
  ASSERT_EQ(Dungeon::IN_HOUSE_AROUND[0], RoomIds::LIVING_ROOM);
  ASSERT_EQ(Dungeon::IN_HOUSE_AROUND[1], RoomIds::KITCHEN);
  ASSERT_EQ(Dungeon::IN_HOUSE_AROUND[2], RoomIds::ATTIC);
  ASSERT_EQ(Dungeon::IN_HOUSE_AROUND[3], RoomIds::KITCHEN);

  // ABOVE_GROUND: 11 rooms (1dungeon.zil:2631)
  ASSERT_EQ(Dungeon::ABOVE_GROUND[0], ROOM_WEST_OF_HOUSE);
  ASSERT_EQ(Dungeon::ABOVE_GROUND[10], RoomIds::CANYON_VIEW);
}

// =============================================================================
// Procedural Exit Routines (1dungeon.zil:1400-1408, 1actions.zil:553, 567, 898)
// =============================================================================

TEST(DungeonProceduralExits_GratingExit) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  auto* grate = g.getObject(ObjectIds::GRATE);
  ASSERT_TRUE(grate != nullptr);

  // Case 1: Grate not revealed
  g.grateRevealed = false;
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::gratingExit();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("You can't go that way.") != std::string::npos);
  }

  // Case 2: Grate revealed but closed
  g.grateRevealed = true;
  grate->clearFlag(ObjectFlag::OPENBIT);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::gratingExit();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("The grating is closed!") != std::string::npos);
    ASSERT_EQ(g.pItObject, grate);
  }

  // Case 3: Grate revealed and open
  grate->setFlag(ObjectFlag::OPENBIT);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::gratingExit();
    ASSERT_EQ(dest, RoomIds::GRATING_ROOM);
  }
}

TEST(DungeonProceduralExits_TrapDoorExit) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
  ASSERT_TRUE(trapDoor != nullptr);

  // Case 1: Rug not moved
  g.rugMoved = false;
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::trapDoorExit();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("You can't go that way.") != std::string::npos);
  }

  // Case 2: Rug moved but trap door closed
  g.rugMoved = true;
  trapDoor->clearFlag(ObjectFlag::OPENBIT);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::trapDoorExit();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("The trap door is closed.") != std::string::npos);
    ASSERT_EQ(g.pItObject, trapDoor);
  }

  // Case 3: Rug moved and trap door open
  trapDoor->setFlag(ObjectFlag::OPENBIT);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::trapDoorExit();
    ASSERT_EQ(dest, RoomIds::CELLAR);
  }
}

TEST(DungeonProceduralExits_UpChimneyFunction) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  auto* adventurer = g.getObject(ObjectIds::ADVENTURER);
  auto* lamp = g.getObject(ObjectIds::LAMP);
  auto* sword = g.getObject(ObjectIds::SWORD);
  auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
  ASSERT_TRUE(adventurer != nullptr);
  ASSERT_TRUE(lamp != nullptr);
  ASSERT_TRUE(sword != nullptr);
  ASSERT_TRUE(trapDoor != nullptr);

  g.winner = adventurer;

  // Case 1: Empty-handed
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::upChimneyFunction();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("Going up empty-handed is a bad idea.") != std::string::npos);
  }

  // Case 2: Carrying single non-lamp item (sword)
  sword->moveTo(adventurer);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::upChimneyFunction();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("You can't get up there with what you're carrying.") != std::string::npos);
  }
  sword->moveTo(nullptr);

  // Case 3: Carrying both lamp and sword
  lamp->moveTo(adventurer);
  sword->moveTo(adventurer);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::upChimneyFunction();
    ASSERT_EQ(dest, 0);
    ASSERT_TRUE(cap.getOutput().find("You can't get up there with what you're carrying.") != std::string::npos);
  }
  sword->moveTo(nullptr);

  // Case 4: Carrying ONLY the lamp
  trapDoor->clearFlag(ObjectFlag::OPENBIT);
  trapDoor->setFlag(ObjectFlag::TOUCHBIT);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::upChimneyFunction();
    ASSERT_EQ(dest, RoomIds::KITCHEN);
    ASSERT_FALSE(trapDoor->hasFlag(ObjectFlag::TOUCHBIT));
  }
}

TEST(DungeonProceduralExits_MazeDiodes) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  // Test maze diode transitions
  g.here = g.getObject(RoomIds::MAZE_2);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::mazeDiodes();
    ASSERT_EQ(dest, RoomIds::MAZE_4);
    ASSERT_TRUE(cap.getOutput().find("won't be able to get back up") != std::string::npos);
  }

  g.here = g.getObject(RoomIds::MAZE_7);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::mazeDiodes();
    ASSERT_EQ(dest, RoomIds::DEAD_END_1);
  }

  g.here = g.getObject(RoomIds::MAZE_9);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::mazeDiodes();
    ASSERT_EQ(dest, RoomIds::MAZE_11);
  }

  g.here = g.getObject(RoomIds::MAZE_12);
  {
    OutputCapture cap;
    ObjectId dest = Dungeon::mazeDiodes();
    ASSERT_EQ(dest, RoomIds::MAZE_5);
  }
}

// =============================================================================
// Room Actions: CANYON-VIEW-F (1dungeon.zil:2406-2411)
// =============================================================================

TEST(DungeonRoomActions_CanyonViewLeapIsFatal) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  DeathSystem::setTestMode(true);
  DeathSystem::reset();
  g.prsa = V_LEAP;
  g.prso = nullptr;

  OutputCapture cap;
  Dungeon::canyonViewRoomAction(M_BEG);

  ASSERT_EQ(DeathSystem::getDeathCount(), 1);
  ASSERT_TRUE(cap.getOutput().find("Nice view, lousy place to jump.") != std::string::npos);
  ASSERT_TRUE(cap.getOutput().find("You have died") != std::string::npos);
  DeathSystem::reset();
}

// =============================================================================
// Object Actions: TREASURE-INSIDE for BUOY (1dungeon.zil:793-796)
// =============================================================================

TEST(DungeonObjectActions_BuoyTreasureInsideScoresEmerald) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  auto* buoy = g.getObject(ObjectIds::BUOY);
  auto* emerald = g.getObject(ObjectIds::EMERALD);
  ASSERT_TRUE(buoy != nullptr);
  ASSERT_TRUE(emerald != nullptr);

  // Buoy properties per ZIL
  ASSERT_EQ(buoy->getLocation()->getId(), RoomIds::RIVER_4);
  ASSERT_EQ(emerald->getLocation(), buoy);
  ASSERT_FALSE(buoy->hasFlag(ObjectFlag::OPENBIT));
  ASSERT_TRUE(buoy->hasFlag(ObjectFlag::SEARCHBIT));
  ASSERT_TRUE(buoy->hasFlag(ObjectFlag::CONTBIT));
  ASSERT_TRUE(buoy->hasFlag(ObjectFlag::TAKEBIT));

  int initialScore = g.score;
  g.prsa = V_OPEN;
  g.prso = buoy;

  bool actionResult = Dungeon::treasureInsideAction();
  ASSERT_FALSE(actionResult); // Returns false to permit normal opening
  ASSERT_TRUE(g.score > initialScore);
}

// =============================================================================
// Maze & Room Exits Integration (1dungeon.zil:1546-1720, 1776-1809)
// =============================================================================

TEST(DungeonExits_MazeNavigationVerification) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  // Verify Maze 1 exits
  auto* m1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_1));
  ASSERT_TRUE(m1 != nullptr);
  ASSERT_EQ(m1->getExit(Direction::NORTH)->targetRoom, RoomIds::MAZE_1);
  ASSERT_EQ(m1->getExit(Direction::EAST)->targetRoom, RoomIds::TROLL_ROOM);
  ASSERT_EQ(m1->getExit(Direction::WEST)->targetRoom, RoomIds::MAZE_4);
  ASSERT_EQ(m1->getExit(Direction::SOUTH)->targetRoom, RoomIds::MAZE_2);

  // Verify Maze 2 procedural exit
  auto* m2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_2));
  ASSERT_TRUE(m2 != nullptr);
  ASSERT_EQ(m2->getExit(Direction::EAST)->targetRoom, RoomIds::MAZE_3);
  ASSERT_EQ(m2->getExit(Direction::SOUTH)->targetRoom, RoomIds::MAZE_1);
  ASSERT_EQ(m2->getExit(Direction::DOWN)->type, ExitType::PROCEDURAL);

  // Verify Grating Room
  auto* gr = dynamic_cast<ZRoom*>(g.getObject(RoomIds::GRATING_ROOM));
  ASSERT_TRUE(gr != nullptr);
  ASSERT_EQ(gr->getExit(Direction::SW)->targetRoom, RoomIds::MAZE_11);
  ASSERT_EQ(gr->getExit(Direction::UP)->targetRoom, RoomIds::GRATING_CLEARING);
  ASSERT_TRUE(gr->hasGlobal(ObjectIds::GRATE));

  // Verify Dead Ends
  auto* de1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_1));
  ASSERT_TRUE(de1 != nullptr);
  ASSERT_EQ(de1->getExit(Direction::SOUTH)->targetRoom, RoomIds::MAZE_4);

  auto* de2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_2));
  ASSERT_TRUE(de2 != nullptr);
  ASSERT_EQ(de2->getExit(Direction::WEST)->targetRoom, RoomIds::MAZE_5);

  auto* de3 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_3));
  ASSERT_TRUE(de3 != nullptr);
  ASSERT_EQ(de3->getExit(Direction::NORTH)->targetRoom, RoomIds::MAZE_8);

  auto* de4 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_4));
  ASSERT_TRUE(de4 != nullptr);
  ASSERT_EQ(de4->getExit(Direction::SOUTH)->targetRoom, RoomIds::MAZE_12);
}

TEST(DungeonExits_ConditionalExitsVerification) {
  auto& g = Globals::instance();
  g.reset();
  initializeWorld();

  // 1. Troll room east/west conditional on trollFlag
  auto* trollRoom = dynamic_cast<ZRoom*>(g.getObject(RoomIds::TROLL_ROOM));
  ASSERT_TRUE(trollRoom != nullptr);
  ASSERT_TRUE(trollRoom->hasGlobal(ObjectIds::TROLL));
  g.trollFlag = false;
  auto* troll = g.getObject(ObjectIds::TROLL);
  if (troll) troll->clearFlag(ObjectFlag::DEADBIT);
  ASSERT_TRUE(trollRoom->getExit(Direction::EAST)->condition != nullptr);
  ASSERT_FALSE(trollRoom->getExit(Direction::EAST)->condition());
  g.trollFlag = true;
  ASSERT_TRUE(trollRoom->getExit(Direction::EAST)->condition());

  // 2. Cyclops room UP and EAST conditional
  auto* cyclopsRoom = dynamic_cast<ZRoom*>(g.getObject(RoomIds::CYCLOPS_ROOM));
  ASSERT_TRUE(cyclopsRoom != nullptr);
  ASSERT_TRUE(cyclopsRoom->hasGlobal(ObjectIds::CYCLOPS));
  ASSERT_EQ(cyclopsRoom->getExit(Direction::WEST)->targetRoom, RoomIds::MAZE_15);
  ASSERT_EQ(cyclopsRoom->getExit(Direction::NW)->targetRoom, RoomIds::MAZE_15);
  g.magicFlag = false;
  ASSERT_FALSE(cyclopsRoom->getExit(Direction::EAST)->condition());
  g.magicFlag = true;
  ASSERT_TRUE(cyclopsRoom->getExit(Direction::EAST)->condition());

  // 3. Reservoir low tide
  auto* resSouth = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RESERVOIR_SOUTH));
  ASSERT_TRUE(resSouth != nullptr);
  ASSERT_TRUE(resSouth->hasGlobal(ObjectIds::GLOBAL_WATER));
  g.lowTide = false;
  ASSERT_FALSE(resSouth->getExit(Direction::NORTH)->condition());
  g.lowTide = true;
  ASSERT_TRUE(resSouth->getExit(Direction::NORTH)->condition());

  // 4. Hades gate LLD-FLAG
  auto* hades = dynamic_cast<ZRoom*>(g.getObject(RoomIds::ENTRANCE_TO_HADES));
  ASSERT_TRUE(hades != nullptr);
  ASSERT_TRUE(hades->hasGlobal(ObjectIds::BODIES));
  g.lldFlag = false;
  ASSERT_FALSE(hades->getExit(Direction::SOUTH)->condition());
  g.lldFlag = true;
  ASSERT_TRUE(hades->getExit(Direction::SOUTH)->condition());

  // 5. White Cliffs deflate
  auto* wcNorth = dynamic_cast<ZRoom*>(g.getObject(RoomIds::WHITE_CLIFFS_NORTH));
  ASSERT_TRUE(wcNorth != nullptr);
  ASSERT_TRUE(wcNorth->hasGlobal(ObjectIds::WHITE_CLIFF));
  ASSERT_TRUE(wcNorth->hasGlobal(ObjectIds::RIVER));
  g.deflate = false;
  ASSERT_FALSE(wcNorth->getExit(Direction::SOUTH)->condition());
  g.deflate = true;
  ASSERT_TRUE(wcNorth->getExit(Direction::SOUTH)->condition());

  // 6. Aragain Falls rainbow
  auto* falls = dynamic_cast<ZRoom*>(g.getObject(RoomIds::ARAGAIN_FALLS));
  ASSERT_TRUE(falls != nullptr);
  ASSERT_TRUE(falls->hasGlobal(ObjectIds::RAINBOW));
  g.rainbowFlag = false;
  ASSERT_FALSE(falls->getExit(Direction::WEST)->condition());
  g.rainbowFlag = true;
  ASSERT_TRUE(falls->getExit(Direction::WEST)->condition());
}

int main(int argc, char* argv[]) {
  std::cout << "Running Dungeon (1dungeon.zil) Tests" << std::endl;
  std::cout << "====================================" << std::endl;

  auto results = TestFramework::instance().runAll();

  int passed = 0, failed = 0;
  for (const auto& r : results) {
    if (r.passed) passed++;
    else failed++;
  }

  std::cout << "\nResults: " << passed << " passed, " << failed << " failed" << std::endl;
  return failed > 0 ? 1 : 0;
}
