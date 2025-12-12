// Putty action unit tests for ZIL PUTTY-FCN
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/verbs/verbs.h"
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

// Forward declaration
extern bool puttyAction();

// Initialize world for testing
static void setupTestWorld() { initializeWorld(); }

// =============================================================================
// PUTTY-FCN Tests (1actions.zil lines 1379-1384)
// ZIL Logic: V_OIL and V_PUT with putty prevented (not a lubricant)
// =============================================================================

TEST(PuttyFcn_OilWithPuttyAsIndirect) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *putty = g.getObject(ObjectIds::PUTTY);
  if (!putty) {
    return; // Skip test
  }

  g.prsa = V_OIL;
  g.prsi = putty; // Putty as indirect object

  OutputCapture cap;
  bool result = puttyAction();

  ASSERT_TRUE(result);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("all-purpose gunk") != std::string::npos);
  ASSERT_TRUE(output.find("isn't a lubricant") != std::string::npos);
}

TEST(PuttyFcn_PutWithPuttyAsDirect) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *putty = g.getObject(ObjectIds::PUTTY);
  if (!putty) {
    return; // Skip test
  }

  g.prsa = V_PUT;
  g.prso = putty; // Putty as direct object

  OutputCapture cap;
  bool result = puttyAction();

  ASSERT_TRUE(result);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("all-purpose gunk") != std::string::npos);
  ASSERT_TRUE(output.find("isn't a lubricant") != std::string::npos);
}

TEST(PuttyFcn_OtherVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *putty = g.getObject(ObjectIds::PUTTY);
  if (!putty) {
    return; // Skip test
  }

  // TAKE should return false (not handled)
  g.prsa = V_TAKE;
  g.prso = putty;

  bool result = puttyAction();

  ASSERT_FALSE(result);
}

TEST(PuttyFcn_PlugReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *putty = g.getObject(ObjectIds::PUTTY);
  if (!putty) {
    return; // Skip test
  }

  // PLUG should return false (not in ZIL spec)
  g.prsa = V_PLUG;
  g.prso = putty;

  bool result = puttyAction();

  ASSERT_FALSE(result);
}

TEST(PuttyFcn_WrongObjectReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Try with a different object
  ZObject *lamp = g.getObject(ObjectIds::LAMP);
  if (!lamp) {
    return; // Skip test
  }

  g.prsa = V_OIL;
  g.prsi = lamp; // Wrong object

  bool result = puttyAction();

  ASSERT_FALSE(result);
}

// Main test runner
int main(int argc, char *argv[]) {
  std::cout << "Running Putty Action Tests" << std::endl;
  std::cout << "==========================" << std::endl;
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
