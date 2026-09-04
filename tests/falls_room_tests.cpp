// Unit tests for ZIL FALLS-ROOM (zil/1actions.zil:2621-2632)
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
// FallsRoomFcn Tests (1actions.zil lines 2621-2632)
// ZIL Logic:
// - M-LOOK: Outputs Aragain Falls description:
//   "You are at the top of Aragain Falls, an enormous waterfall with a
//    drop of about 450 feet. The only path here is on the north end."
//   - If RAINBOW-FLAG is true:
//     "A solid rainbow spans the falls."
//   - Else:
//     "A beautiful rainbow can be seen over the falls and to the west."
// - Other RARG: does nothing
// =============================================================================

TEST(FallsRoomFcn_LookWhenRainbowNotSolid) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.rainbowFlag = false;

  OutputCapture cap;
  fallsRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("top of Aragain Falls") != std::string::npos);
  ASSERT_TRUE(output.find("450 feet") != std::string::npos);
  ASSERT_TRUE(output.find("A beautiful rainbow can be seen over the falls and to the west.") !=
              std::string::npos);
  ASSERT_TRUE(output.find("A solid rainbow spans the falls.") == std::string::npos);
}

TEST(FallsRoomFcn_LookWhenRainbowIsSolid) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.rainbowFlag = true;

  OutputCapture cap;
  fallsRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("top of Aragain Falls") != std::string::npos);
  ASSERT_TRUE(output.find("450 feet") != std::string::npos);
  ASSERT_TRUE(output.find("A solid rainbow spans the falls.") != std::string::npos);
  ASSERT_TRUE(output.find("A beautiful rainbow can be seen over the falls and to the west.") ==
              std::string::npos);
}

TEST(FallsRoomFcn_OtherRargOutputsNothing) {
  setupTestWorld();
  for (int rarg : {M_ENTER, M_BEG, M_END, 0, 99}) {
    OutputCapture cap;
    fallsRoom(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(FallsRoomFcn_RainbowFlagToggle) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.rainbowFlag = false;
  {
    OutputCapture cap;
    fallsRoom(M_LOOK);
    ASSERT_TRUE(cap.getOutput().find("A beautiful rainbow can be seen") != std::string::npos);
  }

  g.rainbowFlag = true;
  {
    OutputCapture cap;
    fallsRoom(M_LOOK);
    ASSERT_TRUE(cap.getOutput().find("A solid rainbow spans the falls.") != std::string::npos);
  }
}

int main(int argc, char *argv[]) {
  std::cout << "Running FallsRoom Action Tests" << std::endl;
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
