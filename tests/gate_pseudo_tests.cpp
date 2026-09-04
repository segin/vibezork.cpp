// Unit tests for ZIL GATE-PSEUDO (zil/1actions.zil:3207-3215)
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
// GatePseudoFcn Tests (1actions.zil lines 3207-3215)
// ZIL Logic:
// - THROUGH: <DO-WALK ,P?IN>, returns true
// - T: "The gate is protected by an invisible force. It makes your teeth ache to touch it."
// =============================================================================

TEST(GatePseudoFcn_ThroughReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_THROUGH;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = gatePseudo();

  ASSERT_TRUE(result);
}

TEST(GatePseudoFcn_OtherVerbsTriggerProtectionMessage) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> testVerbs = {
      V_OPEN, V_CLOSE, V_EXAMINE, V_TAKE, V_MUNG, V_MOVE
  };

  for (int verb : testVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = gatePseudo();
    ASSERT_TRUE(result);
    ASSERT_TRUE(cap.getOutput().find("The gate is protected by an invisible force.") != std::string::npos);
    ASSERT_TRUE(cap.getOutput().find("teeth ache") != std::string::npos);
  }
}

TEST(GatePseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = gatePseudo();

  ASSERT_TRUE(result);
  ASSERT_TRUE(cap.getOutput().find("invisible force") != std::string::npos);
}

int main(int argc, char *argv[]) {
  std::cout << "Running GatePseudo Action Tests" << std::endl;
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
