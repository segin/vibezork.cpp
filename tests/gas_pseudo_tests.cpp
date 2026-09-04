// Unit tests for ZIL GAS-PSEUDO (zil/1actions.zil:3226-3231)
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
// GasPseudoFcn Tests (1actions.zil lines 3226-3231)
// ZIL Logic:
// - BREATHE: "There is too much gas to blow away."
// - SMELL: "It smells like coal gas in here."
// - Other verbs return false
// =============================================================================

TEST(GasPseudoFcn_BreatheReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_BREATHE;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = gasPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "There is too much gas to blow away.\n");
}

TEST(GasPseudoFcn_SmellReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_SMELL;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = gasPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It smells like coal gas in here.\n");
}

TEST(GasPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_TAKE, V_OPEN, V_CLOSE, V_MOVE, V_DROP, V_LOOK
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = gasPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(GasPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_SMELL;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = gasPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It smells like coal gas in here.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running GasPseudo Action Tests" << std::endl;
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
