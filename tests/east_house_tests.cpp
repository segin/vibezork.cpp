// Unit tests for ZIL EAST-HOUSE (zil/1actions.zil:17-26)
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
static void setupTestWorld() { initializeWorld(); }

// =============================================================================
// EastHouseFcn Tests (1actions.zil lines 17-26)
// ZIL Logic:
// - M-LOOK: Outputs Behind House description
//   - If KITCHEN-WINDOW has OPENBIT: "open."
//   - Else: "slightly ajar."
// - Other RARG: does nothing
// =============================================================================

TEST(EastHouseFcn_LookWhenClosedShowsAjar) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *window = g.getObject(ObjectIds::KITCHEN_WINDOW);
  ASSERT_TRUE(window != nullptr);
  window->clearFlag(ObjectFlag::OPENBIT);

  OutputCapture cap;
  eastHouse(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You are behind the white house.") != std::string::npos);
  ASSERT_TRUE(output.find("slightly ajar.") != std::string::npos);
  ASSERT_TRUE(output.find("which is open.") == std::string::npos);
}

TEST(EastHouseFcn_LookWhenOpenShowsOpen) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *window = g.getObject(ObjectIds::KITCHEN_WINDOW);
  ASSERT_TRUE(window != nullptr);
  window->setFlag(ObjectFlag::OPENBIT);

  OutputCapture cap;
  eastHouse(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You are behind the white house.") != std::string::npos);
  ASSERT_TRUE(output.find("which is open.") != std::string::npos);
  ASSERT_TRUE(output.find("slightly ajar.") == std::string::npos);
}

TEST(EastHouseFcn_OtherRargOutputsNothing) {
  setupTestWorld();

  for (int rarg : {M_ENTER, M_END, M_BEG, 0, 99}) {
    OutputCapture cap;
    eastHouse(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(EastHouseFcn_BehindHouseActionEquivalent) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *window = g.getObject(ObjectIds::KITCHEN_WINDOW);
  ASSERT_TRUE(window != nullptr);
  window->setFlag(ObjectFlag::OPENBIT);

  OutputCapture cap1;
  eastHouse(M_LOOK);

  OutputCapture cap2;
  behindHouseAction(M_LOOK);

  ASSERT_EQ(cap1.getOutput(), cap2.getOutput());
}

int main(int argc, char *argv[]) {
  std::cout << "Running EastHouse Action Tests" << std::endl;
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
