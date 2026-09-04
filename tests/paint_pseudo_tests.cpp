// Unit tests for ZIL PAINT-PSEUDO (zil/1actions.zil:3222-3225)
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
// PaintPseudoFcn Tests (1actions.zil lines 3222-3225)
// ZIL Logic:
// - MUNG: "Some paint chips away, revealing more paint."
// - Other verbs return false
// =============================================================================

TEST(PaintPseudoFcn_MungReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_MUNG;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = paintPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "Some paint chips away, revealing more paint.\n");
}

TEST(PaintPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_TAKE, V_OPEN, V_CLOSE, V_MOVE, V_DROP, V_LOOK
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = paintPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(PaintPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_MUNG;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = paintPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "Some paint chips away, revealing more paint.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running PaintPseudo Action Tests" << std::endl;
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
