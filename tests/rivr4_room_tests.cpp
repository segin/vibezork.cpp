// Unit tests for ZIL RIVR4-ROOM (zil/1actions.zil:2844-2853)
#include "../src/core/globals.h"
#include "../src/core/object.h"
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
// Rivr4RoomFcn Tests (1actions.zil lines 2844-2853)
// ZIL Logic:
// - M-LOOK: Outputs Frigid River (River 4) description
// - M-END:
//   - If IN? BUOY WINNER and BUOY-FLAG:
//     - TELL "You notice something funny about the feel of the buoy." CR
//     - SETG BUOY-FLAG <>
// - Other RARG: does nothing
// =============================================================================

TEST(Rivr4RoomFcn_LookPrintsDescription) {
  setupTestWorld();
  OutputCapture cap;
  rivr4Room(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The river is running faster here") != std::string::npos);
  ASSERT_TRUE(output.find("rushing water") != std::string::npos);
  ASSERT_TRUE(output.find("sandy beach") != std::string::npos);
}

TEST(Rivr4RoomFcn_OtherRargOutputsNothing) {
  setupTestWorld();
  for (int rarg : {M_ENTER, M_BEG, 0, 99}) {
    OutputCapture cap;
    rivr4Room(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(Rivr4RoomFcn_EndWhenHoldingBuoyAndFlagTrueNoticesFeel) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *buoy = g.getObject(ObjectIds::BUOY);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(buoy != nullptr && winner != nullptr);

  buoy->moveTo(winner);
  g.buoyFlag = true;

  OutputCapture cap;
  rivr4Room(M_END);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You notice something funny about the feel of the buoy.") !=
              std::string::npos);
  ASSERT_FALSE(g.buoyFlag);
}

TEST(Rivr4RoomFcn_EndWhenHoldingBuoyAndFlagFalseDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *buoy = g.getObject(ObjectIds::BUOY);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(buoy != nullptr && winner != nullptr);

  buoy->moveTo(winner);
  g.buoyFlag = false;

  OutputCapture cap;
  rivr4Room(M_END);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.empty());
  ASSERT_FALSE(g.buoyFlag);
}

TEST(Rivr4RoomFcn_EndWhenBuoyOnGroundDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *buoy = g.getObject(ObjectIds::BUOY);
  auto *river4 = g.getObject(RoomIds::RIVER_4);
  ASSERT_TRUE(buoy != nullptr && river4 != nullptr);

  buoy->moveTo(river4); // in room, not in winner
  g.buoyFlag = true;

  OutputCapture cap;
  rivr4Room(M_END);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.empty());
  ASSERT_TRUE(g.buoyFlag);
}

int main(int argc, char *argv[]) {
  std::cout << "Running Rivr4Room Action Tests" << std::endl;
  std::cout << "==============================" << std::endl;
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
