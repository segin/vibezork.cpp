// Unit tests for ZIL BATS-ROOM (zil/1actions.zil:2478-2486)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/systems/death.h"
#include "../src/verbs/verbs.h"
#include "test_framework.h"
#include <algorithm>
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

// Initialize world and death system for testing
static void setupTestWorld() {
  initializeWorld();
  DeathSystem::initialize();
  DeathSystem::setTestMode(true);
}

// =============================================================================
// BatsRoomFcn Tests (1actions.zil lines 2478-2486)
// ZIL Logic:
// - M-LOOK: Outputs Bat Room description:
//   "You are in a small room which has doors only to the east and south."
// - M-ENTER (when NOT DEAD):
//   - If GARLIC is NOT with WINNER and NOT in HERE:
//     - V-LOOK
//     - CRLF
//     - FLY-ME (fweep 4, bat grabs you, moves to random BAT-DROPS room)
// - Other RARG: does nothing
// =============================================================================

TEST(BatsRoomFcn_LookPrintsDescription) {
  setupTestWorld();
  OutputCapture cap;
  batsRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You are in a small room which has doors only to the east and south.") !=
              std::string::npos);
}

TEST(BatsRoomFcn_OtherRargOutputsNothing) {
  setupTestWorld();
  for (int rarg : {M_BEG, M_END, 0, 99}) {
    OutputCapture cap;
    batsRoom(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(BatsRoomFcn_EnterWhenDeadDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *batRoom = g.getObject(RoomIds::BAT_ROOM);
  ASSERT_TRUE(batRoom != nullptr);
  g.here = batRoom;

  DeathSystem::setDead(true);

  OutputCapture cap;
  batsRoom(M_ENTER);
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(g.here, batRoom);
}

TEST(BatsRoomFcn_EnterWithGarlicInWinnerProtectsPlayer) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *batRoom = g.getObject(RoomIds::BAT_ROOM);
  auto *garlic = g.getObject(ObjectIds::GARLIC);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(batRoom != nullptr && garlic != nullptr && winner != nullptr);

  g.here = batRoom;
  winner->moveTo(batRoom);
  garlic->moveTo(winner);

  OutputCapture cap;
  batsRoom(M_ENTER);

  // When garlic is present, FLY-ME is not called
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(g.here, batRoom);
}

TEST(BatsRoomFcn_EnterWithGarlicInRoomProtectsPlayer) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *batRoom = g.getObject(RoomIds::BAT_ROOM);
  auto *garlic = g.getObject(ObjectIds::GARLIC);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(batRoom != nullptr && garlic != nullptr && winner != nullptr);

  g.here = batRoom;
  winner->moveTo(batRoom);
  garlic->moveTo(batRoom); // on floor of bat room

  OutputCapture cap;
  batsRoom(M_ENTER);

  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(g.here, batRoom);
}

TEST(BatsRoomFcn_EnterWithoutGarlicTriggersFlyMe) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *batRoom = g.getObject(RoomIds::BAT_ROOM);
  auto *garlic = g.getObject(ObjectIds::GARLIC);
  auto *winner = g.winner ? g.winner : g.player;
  auto *kitchen = g.getObject(RoomIds::KITCHEN);
  ASSERT_TRUE(batRoom != nullptr && garlic != nullptr && winner != nullptr);

  g.here = batRoom;
  winner->moveTo(batRoom);
  if (kitchen) {
    garlic->moveTo(kitchen); // Garlic is elsewhere
  }

  OutputCapture cap;
  batsRoom(M_ENTER);
  std::string output = cap.getOutput();

  // Should have fwept and grabbed player
  ASSERT_TRUE(output.find("Fweep!") != std::string::npos);
  ASSERT_TRUE(output.find("The bat grabs you by the scruff of your neck and lifts you away....") !=
              std::string::npos);

  // Player should have been transported out of Bat Room
  ASSERT_NE(g.here, batRoom);

  // Target must be one of the BAT_DROPS rooms
  static const std::vector<ObjectId> expectedDrops = {
      RoomIds::MINE_1,       RoomIds::MINE_2,     RoomIds::MINE_3,
      RoomIds::MINE_4,       RoomIds::LADDER_TOP, RoomIds::LADDER_BOTTOM,
      RoomIds::SQUEEKY_ROOM, RoomIds::MINE_ENTRANCE};

  bool found = std::find(expectedDrops.begin(), expectedDrops.end(),
                         g.here->getId()) != expectedDrops.end();
  ASSERT_TRUE(found);
}

TEST(BatsRoomFcn_FweepOutputsExpectedLines) {
  OutputCapture cap;
  fweep(4);
  std::string output = cap.getOutput();

  int count = 0;
  size_t pos = 0;
  while ((pos = output.find("Fweep!", pos)) != std::string::npos) {
    count++;
    pos += 6;
  }
  ASSERT_EQ(count, 4);
}

int main(int argc, char *argv[]) {
  std::cout << "Running BatsRoom Action Tests" << std::endl;
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
