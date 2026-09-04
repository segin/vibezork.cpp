// Unit tests for ZIL DOOR-PSEUDO (zil/1actions.zil:3216-3221)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/pseudo_actions.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"
#include <sstream>
#include <vector>

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
// DoorPseudoFcn Tests (1actions.zil lines 3216-3221)
// ZIL Logic:
// - OPEN / CLOSE: "The door won't budge."
// - THROUGH: <DO-WALK ,P?SOUTH>, returns true
// - Other verbs return false
// =============================================================================

TEST(DoorPseudoFcn_OpenReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_OPEN;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = doorPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The door won't budge.\n");
}

TEST(DoorPseudoFcn_CloseReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_CLOSE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = doorPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The door won't budge.\n");
}

TEST(DoorPseudoFcn_ThroughReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_THROUGH;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = doorPseudo();

  ASSERT_TRUE(result);
}

TEST(DoorPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_TAKE, V_MUNG, V_DROP, V_LOOK, V_MOVE
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = doorPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(DoorPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_OPEN;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = doorPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The door won't budge.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running DoorPseudo Action Tests" << std::endl;
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
