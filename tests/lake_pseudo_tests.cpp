// Unit tests for ZIL LAKE-PSEUDO (zil/1actions.zil:3177-3184)
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
static void setupTestWorld() {
  initializeWorld();
  Globals::instance().lowTide = false;
}

// =============================================================================
// LakePseudoFcn Tests (1actions.zil lines 3177-3184)
// ZIL Logic:
// - LOW-TIDE takes precedence: "There's not much lake left...."
// - CROSS: "It's too wide to cross."
// - THROUGH: "You can't swim in this lake."
// - Other verbs return false
// =============================================================================

TEST(LakePseudoFcn_LowTidePrecedence) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lowTide = true;
  g.prsa = V_EXAMINE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = lakePseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "There's not much lake left....\n");

  // Also test with V_CROSS while low tide
  g.prsa = V_CROSS;
  OutputCapture cap2;
  bool result2 = lakePseudo();
  ASSERT_TRUE(result2);
  ASSERT_EQ(cap2.getOutput(), "There's not much lake left....\n");
}

TEST(LakePseudoFcn_CrossReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lowTide = false;
  g.prsa = V_CROSS;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = lakePseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It's too wide to cross.\n");
}

TEST(LakePseudoFcn_ThroughReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lowTide = false;
  g.prsa = V_THROUGH;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = lakePseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "You can't swim in this lake.\n");
}

TEST(LakePseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lowTide = false;

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_TAKE, V_OPEN, V_CLOSE, V_MOVE, V_MUNG, V_DROP, V_LOOK
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = lakePseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(LakePseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lowTide = false;
  g.prsa = V_CROSS;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = lakePseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It's too wide to cross.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running LakePseudo Action Tests" << std::endl;
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
