// Unit tests for ZIL CHASM-PSEUDO (zil/1actions.zil:3191-3202)
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
// ChasmPseudoFcn Tests (1actions.zil lines 3191-3202)
// ZIL Logic:
// - LEAP or PUT ME: "You look before leaping, and realize that you would never survive."
// - CROSS: "It's too far to jump, and there's no bridge."
// - PUT / THROW-OFF PRSO into PSEUDO-OBJECT: drops out of sight, REMOVE-CAREFULLY
// - Other verbs return false
// =============================================================================

TEST(ChasmPseudoFcn_LeapReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_LEAP;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "You look before leaping, and realize that you would never survive.\n");
}

TEST(ChasmPseudoFcn_PutMeReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_PUT;
  g.prso = g.getObject(ObjectIds::ME);
  g.prsi = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "You look before leaping, and realize that you would never survive.\n");
}

TEST(ChasmPseudoFcn_CrossReturnsTrue) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_CROSS;
  g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It's too far to jump, and there's no bridge.\n");
}

TEST(ChasmPseudoFcn_PutItemDropsIntoChasm) {
  setupTestWorld();
  auto &g = Globals::instance();
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  ASSERT_TRUE(sword != nullptr);

  g.prsa = V_PUT;
  g.prso = sword;
  g.prsi = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_TRUE(cap.getOutput().find("drops out of sight into the chasm.") != std::string::npos);
  ASSERT_TRUE(sword->getLocation() == nullptr);
}

TEST(ChasmPseudoFcn_ThrowOffItemDropsIntoChasm) {
  setupTestWorld();
  auto &g = Globals::instance();
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  ASSERT_TRUE(sword != nullptr);

  g.prsa = V_THROW_OFF;
  g.prso = sword;
  g.prsi = g.getObject(ObjectIds::PSEUDO_OBJECT);

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_TRUE(cap.getOutput().find("drops out of sight into the chasm.") != std::string::npos);
  ASSERT_TRUE(sword->getLocation() == nullptr);
}

TEST(ChasmPseudoFcn_UnhandledVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  const std::vector<int> unhandledVerbs = {
      V_EXAMINE, V_TAKE, V_OPEN, V_CLOSE, V_MOVE, V_MUNG, V_LOOK
  };

  for (int verb : unhandledVerbs) {
    g.prsa = verb;
    g.prso = g.getObject(ObjectIds::PSEUDO_OBJECT);
    OutputCapture cap;
    bool result = chasmPseudo();
    ASSERT_FALSE(result);
    ASSERT_TRUE(cap.getOutput().empty());
  }
}

TEST(ChasmPseudoFcn_NullObjectsSafe) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.prsa = V_CROSS;
  g.prso = nullptr;
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = chasmPseudo();

  ASSERT_TRUE(result);
  ASSERT_EQ(cap.getOutput(), "It's too far to jump, and there's no bridge.\n");
}

int main(int argc, char *argv[]) {
  std::cout << "Running ChasmPseudo Action Tests" << std::endl;
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
