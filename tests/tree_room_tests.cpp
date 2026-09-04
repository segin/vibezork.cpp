// Unit tests for ZIL TREE-ROOM (zil/1actions.zil:2880-2917)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/systems/death.h"
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
  DeathSystem::setTestMode(true);
}

// =============================================================================
// TreeRoomFcn Tests (1actions.zil lines 2880-2917)
// ZIL Logic:
// - M-LOOK:
//   - "You are about 10 feet above the ground nestled among some large branches..."
//   - If items on ground in PATH, "On the ground below you can see:  a ..."
// - M-BEG:
//   - CLIMB-DOWN: DO-WALK P?DOWN
//   - CLIMB-UP / CLIMB-FOO TREE: DO-WALK P?UP
//   - LEAP: JIGS-UP "That was just a bit too far down."
//   - DROP:
//     - !IDROP: return
//     - NEST with EGG: spills egg, broken egg and nest to PATH
//     - EGG: BAD-EGG, egg to PATH, broken egg
//     - other object: falls to ground (moves to PATH)
// - M-ENTER:
//   - ENABLE QUEUE I-FOREST-ROOM -1
// =============================================================================

TEST(TreeRoomFcn_LookPrintsDescription) {
  setupTestWorld();
  auto &g = Globals::instance();
  // Clear any items from PATH
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  if (path) {
    for (auto *c : path->getContents()) {
      if (c) c->moveTo(nullptr);
    }
  }

  OutputCapture cap;
  treeRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("10 feet above the ground nestled among some large") != std::string::npos);
  ASSERT_TRUE(output.find("nearest branch above you is above your reach") != std::string::npos);
  ASSERT_TRUE(output.find("On the ground below you can see") == std::string::npos);
}

TEST(TreeRoomFcn_LookShowsItemsOnGround) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *sword = g.getObject(ObjectIds::SWORD);
  auto *lantern = g.getObject(ObjectIds::LAMP);

  ASSERT_TRUE(path != nullptr);
  ASSERT_TRUE(sword != nullptr);
  ASSERT_TRUE(lantern != nullptr);

  sword->moveTo(path);
  lantern->moveTo(path);

  OutputCapture cap;
  treeRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("On the ground below you can see:") != std::string::npos);
  ASSERT_TRUE(output.find("sword") != std::string::npos);
  ASSERT_TRUE(output.find("lamp") != std::string::npos || output.find("lantern") != std::string::npos);
}

TEST(TreeRoomFcn_EnterQueuesInterrupt) {
  setupTestWorld();
  TimerSystem::TimerManager::instance().disableTimer("I-FOREST-ROOM");
  treeRoom(M_ENTER);

  ASSERT_TRUE(TimerSystem::TimerManager::instance().isTimerEnabled("I-FOREST-ROOM"));
}

TEST(TreeRoomFcn_ClimbDownMovesPlayer) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);

  g.prsa = V_CLIMB_DOWN;
  g.prso = treeObj;

  OutputCapture cap;
  treeRoom(M_BEG);

  ASSERT_EQ(g.here->getId(), RoomIds::FOREST_PATH);
}

TEST(TreeRoomFcn_ClimbUpBlocked) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *treeObj = g.getObject(ObjectIds::TREE);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);

  g.prsa = V_CLIMB_UP;
  g.prso = treeObj;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You cannot climb any higher.") != std::string::npos);
  ASSERT_EQ(g.here->getId(), RoomIds::UP_A_TREE);
}

TEST(TreeRoomFcn_DropEggShatters) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *egg = g.getObject(ObjectIds::EGG);
  auto *brokenEgg = g.getObject(ObjectIds::BROKEN_EGG);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);
  egg->moveTo(g.player);

  g.prsa = V_DROP;
  g.prso = egg;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The egg falls to the ground and springs open, seriously damaged.") != std::string::npos);
  ASSERT_EQ(egg->getLocation(), nullptr);
  ASSERT_EQ(brokenEgg->getLocation(), path);
}

TEST(TreeRoomFcn_DropNestWithEgg) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *nest = g.getObject(ObjectIds::NEST);
  auto *egg = g.getObject(ObjectIds::EGG);
  auto *brokenEgg = g.getObject(ObjectIds::BROKEN_EGG);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);
  nest->moveTo(g.player);
  egg->moveTo(nest);

  g.prsa = V_DROP;
  g.prso = nest;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The nest falls to the ground, and the egg spills out of it, seriously") != std::string::npos);
  ASSERT_TRUE(output.find("damaged.") != std::string::npos);
  ASSERT_EQ(nest->getLocation(), path);
  ASSERT_EQ(egg->getLocation(), nullptr);
  ASSERT_EQ(brokenEgg->getLocation(), path);
}

TEST(TreeRoomFcn_DropOtherObject) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *sword = g.getObject(ObjectIds::SWORD);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);
  sword->moveTo(g.player);

  g.prsa = V_DROP;
  g.prso = sword;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("falls to the ground.") != std::string::npos);
  ASSERT_EQ(sword->getLocation(), path);
}

TEST(TreeRoomFcn_DropNotHeldFails) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);
  auto *path = g.getObject(RoomIds::FOREST_PATH);
  auto *sword = g.getObject(ObjectIds::SWORD);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);
  sword->moveTo(nullptr); // Not in inventory

  g.prsa = V_DROP;
  g.prso = sword;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("not carrying") != std::string::npos);
  ASSERT_NE(sword->getLocation(), path);
}

TEST(TreeRoomFcn_LeapIsFatal) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *upATree = g.getObject(RoomIds::UP_A_TREE);

  g.here = upATree;
  g.winner = g.player;
  g.player->moveTo(upATree);

  g.prsa = V_LEAP;
  g.prso = nullptr;

  OutputCapture cap;
  treeRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("That was just a bit too far down.") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(TreeRoomFcn_UnhandledRarg) {
  setupTestWorld();
  OutputCapture cap;
  treeRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(TreeRoomFcn_ForestInterrupt) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.here = g.getObject(RoomIds::UP_A_TREE);
  ASSERT_TRUE(isForestRoom());

  g.here = g.getObject(RoomIds::FOREST_1);
  ASSERT_TRUE(isForestRoom());

  g.here = g.getObject(RoomIds::LIVING_ROOM);
  ASSERT_FALSE(isForestRoom());

  TimerSystem::TimerManager::instance().enableTimer("I-FOREST-ROOM");
  ASSERT_FALSE(iForestRoom());
  ASSERT_FALSE(TimerSystem::TimerManager::instance().isTimerEnabled("I-FOREST-ROOM"));
}

int main(int argc, char *argv[]) {
  std::cout << "Running TreeRoom Action Tests" << std::endl;
  std::cout << "=============================" << std::endl;
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
