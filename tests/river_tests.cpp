// River action unit tests for ZIL RIVER-FUNCTION
// Source: zil/1actions.zil:2669-2690
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/systems/death.h"
#include "test_framework.h"
#include <iostream>
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
extern bool riverAction();

// Initialize world for testing
static void setupTestWorld() {
  Globals::instance().reset();
  initializeWorld();
  DeathSystem::initialize();
  DeathSystem::setTestMode(true);
}

// =============================================================================
// RIVER-FUNCTION Tests (1actions.zil lines 2669-2690)
// =============================================================================

TEST(RiverActionFcn_UnhandledVerbReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.prsa = V_EXAMINE;
  g.prso = g.getObject(ObjectIds::RIVER);
  g.prsi = nullptr;

  bool result = riverAction();
  ASSERT_FALSE(result);
}

TEST(RiverActionFcn_PutSelfInRiverDrowns) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.prsa = V_PUT;
  g.prso = g.player;
  g.prsi = g.getObject(ObjectIds::RIVER);

  OutputCapture cap;
  bool result = riverAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You splash around for a while, fighting the current, then you drown.") != std::string::npos);
}

TEST(RiverActionFcn_PutInflatedBoatInRiverTellsLaunch) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.prsa = V_PUT;
  g.prso = g.getObject(ObjectIds::BOAT_INFLATED);
  g.prsi = g.getObject(ObjectIds::RIVER);

  OutputCapture cap;
  bool result = riverAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You should get in the boat then launch it.") != std::string::npos);
}

TEST(RiverActionFcn_PutBurnableObjectFloatsAndSinks) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Create a burnable test object
  auto torch = g.getObject(ObjectIds::TORCH);
  if (!torch) {
    torch = g.getObject(ObjectIds::MATCH);
  }
  ASSERT_TRUE(torch != nullptr);
  torch->setFlag(ObjectFlag::BURNBIT);

  g.prsa = V_PUT;
  g.prso = torch;
  g.prsi = g.getObject(ObjectIds::RIVER);

  OutputCapture cap;
  bool result = riverAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("floats for a moment, then sinks.") != std::string::npos);
  ASSERT_TRUE(torch->getLocation() == nullptr);
}

TEST(RiverActionFcn_PutNonBurnableObjectSplashes) {
  setupTestWorld();
  auto &g = Globals::instance();

  auto wrench = g.getObject(ObjectIds::WRENCH);
  ASSERT_TRUE(wrench != nullptr);
  wrench->clearFlag(ObjectFlag::BURNBIT);

  g.prsa = V_PUT;
  g.prso = wrench;
  g.prsi = g.getObject(ObjectIds::RIVER);

  OutputCapture cap;
  bool result = riverAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("splashes into the water and is gone forever.") != std::string::npos);
  ASSERT_TRUE(wrench->getLocation() == nullptr);
}

TEST(RiverActionFcn_JumpOrEnterRiverWarnsAndForgoes) {
  setupTestWorld();
  auto &g = Globals::instance();

  g.prsa = V_JUMP;
  g.prso = g.getObject(ObjectIds::RIVER);
  g.prsi = nullptr;

  OutputCapture cap;
  bool result = riverAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("A look before leaping reveals that the river is wide and dangerous") != std::string::npos);
}

int main(int argc, char *argv[]) {
  std::cout << "Running River Action Tests" << std::endl;
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
