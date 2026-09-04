// Unit tests for ZIL CAVE2-ROOM (zil/1actions.zil:2416-2430)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/systems/candle.h"
#include "../src/systems/lamp.h"
#include "../src/parser/gparser.h"
#include "test_framework.h"
#include <cstdlib>
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
  CandleSystem::initialize();
  LampSystem::initialize();
}

// =============================================================================
// Cave2RoomFcn Tests (1actions.zil lines 2416-2430)
// ZIL Logic:
// - M-LOOK: Outputs Cave description
// - M-END:
//   - If CANDLES in WINNER, PROB 50 80 (zprob 50), and CANDLES has ONBIT:
//     - DISABLE I-CANDLES
//     - FCLEAR CANDLES ONBIT
//     - TELL "A gust of wind blows out your candles!" CR
//     - If NOT LIT? HERE:
//       - TELL "It is now completely dark." CR
// - Other RARG: does nothing
// =============================================================================

TEST(Cave2RoomFcn_LookPrintsDescription) {
  setupTestWorld();
  OutputCapture cap;
  cave2Room(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("This is a tiny cave with entrances west and north") !=
              std::string::npos);
  ASSERT_TRUE(output.find("dark, forbidding") != std::string::npos);
  ASSERT_TRUE(output.find("staircase leading down.") != std::string::npos);
}

TEST(Cave2RoomFcn_OtherRargOutputsNothing) {
  setupTestWorld();
  for (int rarg : {M_ENTER, M_BEG, 0, 99}) {
    OutputCapture cap;
    cave2Room(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(Cave2RoomFcn_CandlesNotInWinnerDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *candles = g.getObject(ObjectIds::CANDLES);
  ASSERT_TRUE(candles != nullptr);

  // Place candles on the ground, not in winner
  auto *tinyCave = g.getObject(RoomIds::TINY_CAVE);
  ASSERT_TRUE(tinyCave != nullptr);
  candles->moveTo(tinyCave);
  candles->setFlag(ObjectFlag::ONBIT);

  for (int i = 0; i < 20; ++i) {
    OutputCapture cap;
    cave2Room(M_END);
    ASSERT_TRUE(cap.getOutput().empty());
    ASSERT_TRUE(candles->hasFlag(ObjectFlag::ONBIT));
  }
}

TEST(Cave2RoomFcn_CandlesNotLitDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *candles = g.getObject(ObjectIds::CANDLES);
  ASSERT_TRUE(candles != nullptr);

  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(winner != nullptr);
  candles->moveTo(winner);
  candles->clearFlag(ObjectFlag::ONBIT);

  for (int i = 0; i < 20; ++i) {
    OutputCapture cap;
    cave2Room(M_END);
    ASSERT_TRUE(cap.getOutput().empty());
    ASSERT_FALSE(candles->hasFlag(ObjectFlag::ONBIT));
  }
}

TEST(Cave2RoomFcn_CandlesBlownOutAndDarkMessage) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *candles = g.getObject(ObjectIds::CANDLES);
  auto *tinyCave = g.getObject(RoomIds::TINY_CAVE);
  ASSERT_TRUE(candles != nullptr && tinyCave != nullptr);

  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(winner != nullptr);
  g.here = tinyCave;

  // Turn off any other light in inventory or room
  auto *lantern = g.getObject(ObjectIds::LAMP);
  if (lantern) {
    lantern->clearFlag(ObjectFlag::ONBIT);
    lantern->moveTo(tinyCave); // Put lantern on ground unlit
    lantern->clearFlag(ObjectFlag::ONBIT);
  }
  auto *sword = g.getObject(ObjectIds::SWORD);
  if (sword) {
    sword->clearFlag(ObjectFlag::ONBIT);
  }

  candles->moveTo(winner);
  candles->setFlag(ObjectFlag::ONBIT);
  CandleSystem::enableCandleTimer();

  // Try until probability roll succeeds
  bool extinguished = false;
  for (int attempt = 0; attempt < 500 && !extinguished; ++attempt) {
    candles->setFlag(ObjectFlag::ONBIT);
    OutputCapture cap;
    cave2Room(M_END);
    std::string out = cap.getOutput();
    if (!out.empty()) {
      ASSERT_TRUE(out.find("A gust of wind blows out your candles!") !=
                  std::string::npos);
      ASSERT_TRUE(out.find("It is now completely dark.") != std::string::npos);
      ASSERT_FALSE(candles->hasFlag(ObjectFlag::ONBIT));
      ASSERT_FALSE(g.lit);
      extinguished = true;
    }
  }

  ASSERT_TRUE(extinguished);
}

TEST(Cave2RoomFcn_CandlesBlownOutWithOtherLightSource) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *candles = g.getObject(ObjectIds::CANDLES);
  auto *lantern = g.getObject(ObjectIds::LAMP);
  auto *tinyCave = g.getObject(RoomIds::TINY_CAVE);
  ASSERT_TRUE(candles != nullptr && lantern != nullptr && tinyCave != nullptr);

  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(winner != nullptr);
  g.here = tinyCave;

  candles->moveTo(winner);
  candles->setFlag(ObjectFlag::ONBIT);

  // Keep lantern on in winner's inventory
  lantern->moveTo(winner);
  lantern->setFlag(ObjectFlag::ONBIT);

  bool extinguished = false;
  for (int attempt = 0; attempt < 500 && !extinguished; ++attempt) {
    candles->setFlag(ObjectFlag::ONBIT);
    OutputCapture cap;
    cave2Room(M_END);
    std::string out = cap.getOutput();
    if (!out.empty()) {
      ASSERT_TRUE(out.find("A gust of wind blows out your candles!") !=
                  std::string::npos);
      // Because lantern is lit, it should NOT be completely dark
      ASSERT_TRUE(out.find("It is now completely dark.") == std::string::npos);
      ASSERT_FALSE(candles->hasFlag(ObjectFlag::ONBIT));
      ASSERT_TRUE(g.lit);
      extinguished = true;
    }
  }

  ASSERT_TRUE(extinguished);
}

int main(int argc, char *argv[]) {
  std::cout << "Running Cave2Room Action Tests" << std::endl;
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
