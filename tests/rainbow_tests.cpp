// Rainbow action unit tests for ZIL RAINBOW-FCN
#include "../src/core/globals.h"
#include "../src/core/object.h"
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

// Forward declaration
extern bool rainbowAction();

// Initialize world for testing
static void setupTestWorld() { initializeWorld(); }

// =============================================================================
// RAINBOW-FCN Tests (1actions.zil lines 2634-2650)
// ZIL Logic: CROSS/THROUGH with location checks, LOOK-UNDER
// =============================================================================

TEST(RainbowFcn_CrossFromCanyonView) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *canyonView = g.getObject(RoomIds::CANYON_VIEW);
  if (!canyonView) {
    return; // Skip test
  }

  g.here = canyonView;
  g.prsa = V_CROSS;

  OutputCapture cap;
  bool result = rainbowAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("From here?!?") != std::string::npos);
}

TEST(RainbowFcn_CrossWithoutFlagPrintsVapor) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *falls = g.getObject(RoomIds::ARAGAIN_FALLS);
  if (!falls) {
    return; // Skip test
  }

  g.here = falls;
  g.rainbowFlag = false; // Rainbow not solid
  g.prsa = V_CROSS;

  OutputCapture cap;
  bool result = rainbowAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("water vapor") != std::string::npos);
}

TEST(RainbowFcn_CrossWithFlagFromFalls) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *falls = g.getObject(RoomIds::ARAGAIN_FALLS);
  ZObject *endOfRainbow = g.getObject(RoomIds::END_OF_RAINBOW);
  if (!falls || !endOfRainbow) {
    return; // Skip test
  }

  g.here = falls;
  g.player->moveTo(falls);
  g.rainbowFlag = true; // Rainbow is solid
  g.prsa = V_CROSS;

  bool result = rainbowAction();

  ASSERT_TRUE(result);
  // Player should be at END_OF_RAINBOW
  ASSERT_EQ(g.here, endOfRainbow);
}

TEST(RainbowFcn_CrossWithFlagFromEndOfRainbow) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *falls = g.getObject(RoomIds::ARAGAIN_FALLS);
  ZObject *endOfRainbow = g.getObject(RoomIds::END_OF_RAINBOW);
  if (!falls || !endOfRainbow) {
    return; // Skip test
  }

  g.here = endOfRainbow;
  g.player->moveTo(endOfRainbow);
  g.rainbowFlag = true; // Rainbow is solid
  g.prsa = V_THROUGH;

  bool result = rainbowAction();

  ASSERT_TRUE(result);
  // Player should be at ARAGAIN_FALLS
  ASSERT_EQ(g.here, falls);
}

TEST(RainbowFcn_CrossWithFlagFromWrongLocation) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *livingRoom = g.getObject(RoomIds::LIVING_ROOM);
  if (!livingRoom) {
    return; // Skip test
  }

  g.here = livingRoom;
  g.rainbowFlag = true; // Rainbow is solid but we're not at the right location
  g.prsa = V_CROSS;

  OutputCapture cap;
  bool result = rainbowAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("which way") != std::string::npos);
}

TEST(RainbowFcn_LookUnder) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.prsa = V_LOOK_UNDER;

  OutputCapture cap;
  bool result = rainbowAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("Frigid River") != std::string::npos);
  ASSERT_TRUE(output.find("under the rainbow") != std::string::npos);
}

TEST(RainbowFcn_OtherVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // TAKE should return false (not handled)
  g.prsa = V_TAKE;

  bool result = rainbowAction();

  ASSERT_FALSE(result);
}

// Main test runner
int main(int argc, char *argv[]) {
  std::cout << "Running Rainbow Action Tests" << std::endl;
  std::cout << "============================" << std::endl;
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
