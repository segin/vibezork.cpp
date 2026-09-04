// Unit tests for ZIL CHAIN-PSEUDO (zil/1actions.zil:4167-4174)
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
// ChainPseudoFcn Tests (1actions.zil lines 4167-4174)
// ZIL Logic:
// - TAKE / MOVE: "The chain is secure."
// - RAISE / LOWER: "Perhaps you should do that to the basket."
// - EXAMINE: "The chain secures a basket within the shaft."
// - Other verbs return false
// =============================================================================

TEST(ChainPseudoFcn_TakeReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The chain is secure.\n");
}

TEST(ChainPseudoFcn_MoveReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_MOVE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The chain is secure.\n");
}

TEST(ChainPseudoFcn_RaiseReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_RAISE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "Perhaps you should do that to the basket.\n");
}

TEST(ChainPseudoFcn_LowerReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_LOWER;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "Perhaps you should do that to the basket.\n");
}

TEST(ChainPseudoFcn_ExamineReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The chain secures a basket within the shaft.\n");
}

TEST(ChainPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_OPEN, V_CLOSE, V_MUNG, V_DROP, V_LOOK
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = chainPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(ChainPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = chainPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "The chain secures a basket within the shaft.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running ChainPseudo Action Tests" << std::endl;
  std::cout << "================================" << std::endl;
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
