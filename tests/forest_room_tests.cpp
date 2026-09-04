// Unit tests for ZIL FOREST-ROOM (zil/1actions.zil:3004-3009)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"
#include <sstream>

// Output capture helper
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

// Initialize world for testing
static void setupTestWorld() {
  initializeWorld();
}

// =============================================================================
// ForestRoomFcn Tests (1actions.zil lines 3004-3009)
// ZIL Logic:
// - M-ENTER: <ENABLE <QUEUE I-FOREST-ROOM -1>>
// - M-BEG:
//   - If (VERB? CLIMB-FOO CLIMB-UP) and PRSO == TREE:
//     - DO-WALK P?UP
// - M-LOOK: displays room's long description
// - Other rargs: no-op
// =============================================================================

TEST(ForestRoomFcn_EnterQueuesInterrupt) {
  setupTestWorld();
  TimerSystem::TimerManager::instance().disableTimer("I-FOREST-ROOM");
  forestRoom(M_ENTER);

  ASSERT_TRUE(TimerSystem::TimerManager::instance().isTimerEnabled("I-FOREST-ROOM"));
}

TEST(ForestRoomFcn_LookOutputsRoomDescription) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.here = g.getObject(RoomIds::FOREST_1);
  {
    OutputCapture cap;
    forestRoom(M_LOOK);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("This is a forest, with trees in all directions") != std::string::npos);
  }

  g.here = g.getObject(RoomIds::FOREST_PATH);
  {
    OutputCapture cap;
    forestRoom(M_LOOK);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("This is a path winding through a dimly lit forest") != std::string::npos);
  }

  g.here = g.getObject(RoomIds::CLEARING);
  {
    OutputCapture cap;
    forestRoom(M_LOOK);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("You are in a small clearing in a well marked forest path") != std::string::npos);
  }
}

TEST(ForestRoomFcn_ClimbTreeOnPathWalksUp) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = path;
  g.winner = g.player;
  g.player->moveTo(path);

  g.prsa = V_CLIMB_UP;
  g.prso = treeObj;

  OutputCapture cap;
  forestRoom(M_BEG);

  ASSERT_EQ(g.here->getId(), RoomIds::UP_A_TREE);
}

TEST(ForestRoomFcn_ClimbFooTreeOnPathWalksUp) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = path;
  g.winner = g.player;
  g.player->moveTo(path);

  g.prsa = V_CLIMB_FOO;
  g.prso = treeObj;

  OutputCapture cap;
  forestRoom(M_BEG);

  ASSERT_EQ(g.here->getId(), RoomIds::UP_A_TREE);
}

TEST(ForestRoomFcn_ClimbTreeInForestBlocked) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *forest1 = g.getObject(RoomIds::FOREST_1);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = forest1;
  g.winner = g.player;
  g.player->moveTo(forest1);

  g.prsa = V_CLIMB_UP;
  g.prso = treeObj;

  OutputCapture cap;
  forestRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("There is no tree here suitable for climbing.") != std::string::npos);
  ASSERT_EQ(g.here->getId(), RoomIds::FOREST_1);
}

TEST(ForestRoomFcn_ClimbOtherObjectIgnored) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *sword = g.getObject(ObjectIds::SWORD);

  g.here = path;
  g.winner = g.player;
  g.player->moveTo(path);

  g.prsa = V_CLIMB_UP;
  g.prso = sword;

  OutputCapture cap;
  forestRoom(M_BEG);

  ASSERT_EQ(g.here->getId(), RoomIds::FOREST_PATH);
}

TEST(ForestRoomFcn_OtherVerbsIgnored) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = path;
  g.winner = g.player;
  g.player->moveTo(path);

  g.prsa = V_TAKE;
  g.prso = treeObj;

  OutputCapture cap;
  forestRoom(M_BEG);

  ASSERT_EQ(g.here->getId(), RoomIds::FOREST_PATH);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(ForestRoomFcn_UnhandledRarg) {
  setupTestWorld();
  OutputCapture cap;
  forestRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(ForestRoomFcn_SongbirdInterrupt) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.here = g.getObject(RoomIds::FOREST_1);
  ASSERT_TRUE(isForestRoom());

  g.here = g.getObject(RoomIds::LIVING_ROOM);
  ASSERT_FALSE(isForestRoom());

  TimerSystem::TimerManager::instance().enableTimer("I-FOREST-ROOM");
  ASSERT_FALSE(iForestRoom());
  ASSERT_FALSE(TimerSystem::TimerManager::instance().isTimerEnabled("I-FOREST-ROOM"));
}

int main(int argc, char *argv[]) {
  std::cout << "Running ForestRoom Action Tests" << std::endl;
  std::cout << "===============================" << std::endl;
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
