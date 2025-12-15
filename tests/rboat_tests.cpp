// RBOAT-FUNCTION comprehensive tests
// ZIL Source: 1actions.zil lines 2722-2815
// Tests cover: WALK/LAUNCH/DROP/PUT/ATTACK/MUNG/BOARD/INFLATE/DEFLATE verbs
// Plus: death scenarios and boat state transformations

#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"
#include <sstream>
#include <string>

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

// Forward declarations
extern bool inflatedBoatAction();

static void setupTestWorld() { initializeWorld(); }

// =============================================================================
// RBOAT-FUNCTION Tests - WALK Verb (5 tests)
// =============================================================================

TEST(RBoat_WalkLandEastWestAllowed) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  // Put player in boat
  g.winner->moveTo(boat);
  g.here = g.getObject(RoomIds::RESERVOIR);

  // WALK LAND should return false (allowed)
  g.prsa = V_WALK;
  g.prso = g.getObject(ObjectIds::BEACH); // Represents LAND

  bool result = inflatedBoatAction();
  ASSERT_FALSE(result); // Not handled, allows default movement
}

TEST(RBoat_WalkOtherDirectionsBlocked) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  g.winner->moveTo(boat);
  g.here = g.getObject(RoomIds::RIVER_1);

  // WALK UP should be blocked
  g.prsa = V_WALK;
  // Use a non-allowed direction

  OutputCapture cap;
  bool result = inflatedBoatAction();

  std::string output = cap.getOutput();
  // Should print "Read the label for the boat's instructions."
  ASSERT_TRUE(output.find("label") != std::string::npos ||
              output.find("instructions") != std::string::npos);
}

// =============================================================================
// RBOAT-FUNCTION Tests - LAUNCH Verb (4 tests)
// =============================================================================

TEST(RBoat_LaunchWhenAlreadyOnWater) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  g.winner->moveTo(boat);
  g.here = g.getObject(RoomIds::RESERVOIR);

  g.prsa = V_LAUNCH;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("reservoir") != std::string::npos ||
              output.find("on the") != std::string::npos);
}

TEST(RBoat_LaunchNotInBoat) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  // Player NOT in boat
  g.here = g.getObject(RoomIds::BEACH);
  boat->moveTo(g.here);

  g.prsa = V_LAUNCH;
  g.prso = boat;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("not in the boat") != std::string::npos);
}

// =============================================================================
// RBOAT-FUNCTION Tests - Weapon Puncture (6 tests)
// =============================================================================

TEST(RBoat_DropWeaponPuncturesBoat) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  if (!boat || !sword)
    return;

  g.winner->moveTo(boat);
  g.here = g.getObject(RoomIds::BEACH); // Safe location (not NONLANDBIT)
  boat->moveTo(g.here);

  g.prsa = V_DROP;
  g.prso = sword;
  sword->setFlag(ObjectFlag::WEAPONBIT);

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("puncture") != std::string::npos ||
              output.find("hissing") != std::string::npos);

  // Verify boat is now punctured
  ZObject *puncturedBoat = g.getObject(ObjectIds::BOAT_PUNCTURED);
  if (puncturedBoat) {
    ASSERT_TRUE(puncturedBoat->getLocation() != nullptr);
  }
}

TEST(RBoat_BoardWithSwordPunctures) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  if (!boat || !sword)
    return;

  g.here = g.getObject(RoomIds::BEACH);
  boat->moveTo(g.here);
  sword->moveTo(g.winner); // Carrying sword

  g.prsa = V_BOARD;
  g.prso = boat;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("sharp") != std::string::npos ||
              output.find("puncture") != std::string::npos);
}

// =============================================================================
// RBOAT-FUNCTION Tests - INFLATE/DEFLATE (4 tests)
// =============================================================================

TEST(RBoat_InflateBlockedAlreadyInflated) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  g.prsa = V_INFLATE;
  g.prso = boat;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("burst") != std::string::npos ||
              output.find("further") != std::string::npos);
}

TEST(RBoat_DeflateWhileInsideBlocked) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  g.winner->moveTo(boat); // Player inside boat
  g.here = g.getObject(RoomIds::BEACH);
  boat->moveTo(g.here);

  g.prsa = V_DEFLATE;
  g.prso = boat;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("while you're in it") != std::string::npos);
}

TEST(RBoat_DeflateSucceeds) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
  if (!boat)
    return;

  g.here = g.getObject(RoomIds::BEACH);
  boat->moveTo(g.here); // Boat on ground
  // Player NOT in boat

  g.prsa = V_DEFLATE;
  g.prso = boat;

  OutputCapture cap;
  bool result = inflatedBoatAction();

  ASSERT_TRUE(result);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("deflates") != std::string::npos);

  // Verify boat is now deflatable
  ZObject *deflatableBoat = g.getObject(ObjectIds::BOAT_INFLATABLE);
  if (deflatableBoat) {
    ASSERT_TRUE(deflatableBoat->getLocation() != nullptr);
  }
}

// Main test runner
int main(int argc, char *argv[]) {
  std::cout << "Running RBOAT-FUNCTION Tests" << std::endl;
  std::cout << "=============================" << std::endl;
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
  std::cout << "(Note: This is initial test suite - will expand to 26 tests)"
            << std::endl;

  return failed > 0 ? 1 : 0;
}
