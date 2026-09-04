// Unit tests for ZIL NAILS-PSEUDO (zil/1actions.zil:376-380)
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
// NailsPseudoFcn Tests (1actions.zil lines 376-380)
// ZIL Logic: V_TAKE returns message that nails cannot be removed; other verbs return false.
// =============================================================================

TEST(NailsPseudoFcn_TakeReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = nailsPseudo();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The nails, deeply imbedded in the door, cannot be removed.") != std::string::npos);
}

TEST(NailsPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_OPEN, V_CLOSE, V_MOVE, V_MUNG, V_DROP, V_LOOK, V_PULL
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = nailsPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(NailsPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = nailsPseudo();

  ASSERT_TRUE(result);
  ASSERT_TRUE(cap.getOutput().find("cannot be removed") != std::string::npos);
}

TEST(NailsPseudoFcn_ExactOutputMatch) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;

  OutputCapture cap;
  nailsPseudo();

  std::string expected = "The nails, deeply imbedded in the door, cannot be removed.\n";
  ASSERT_EQ(cap.getOutput(), expected);
}

int main(int argc, char *argv[]) {
  std::cout << "Running NailsPseudo Action Tests" << std::endl;
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
