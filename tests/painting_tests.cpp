// Painting action unit tests for ZIL PAINTING-FCN
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
extern bool paintingAction();

// Initialize world for testing
static void setupTestWorld() { initializeWorld(); }

// =============================================================================
// PAINTING-FCN Tests (1actions.zil lines 2207-2214)
// ZIL Logic: V_MUNG destroys painting value and changes description
// =============================================================================

TEST(PaintingFcn_MungDestroysValue) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    return; // Skip test
  }

  // Set initial value
  painting->setProperty(P_TVALUE, 10); // Valuable painting

  g.prsa = V_MUNG;
  g.prso = painting;

  OutputCapture cap;
  bool result = paintingAction();

  ASSERT_TRUE(result);

  // Verify TVALUE set to 0
  ASSERT_EQ(painting->getProperty(P_TVALUE), 0);

  // Verify output message
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("Congratulations!") != std::string::npos);
  ASSERT_TRUE(output.find("vandals") != std::string::npos);
  ASSERT_TRUE(output.find("masterpieces") != std::string::npos);
}

TEST(PaintingFcn_MungChangesDescription) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    return; // Skip test
  }

  g.prsa = V_MUNG;
  g.prso = painting;

  paintingAction();

  // Verify LDESC changed to "worthless piece of canvas"
  std::string ldesc = painting->getLongDesc();
  ASSERT_TRUE(ldesc.find("worthless") != std::string::npos);
  ASSERT_TRUE(ldesc.find("canvas") != std::string::npos);
}

TEST(PaintingFcn_OtherVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    return; // Skip test
  }

  // EXAMINE should return false (not handled by ZIL spec)
  g.prsa = V_EXAMINE;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn_TakeReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    return; // Skip test
  }

  // TAKE should return false (not handled by ZIL spec)
  g.prsa = V_TAKE;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn_BurnReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    return; // Skip test
  }

  // BURN should return false (ZIL spec only handles MUNG)
  g.prsa = V_BURN;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn_NullObjectReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // No object set
  g.prsa = V_MUNG;
  g.prso = nullptr;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn_WrongObjectReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Try with a different object
  ZObject *lamp = g.getObject(ObjectIds::LAMP);
  if (!lamp) {
    return; // Skip test
  }

  g.prsa = V_MUNG;
  g.prso = lamp; // Wrong object

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

// Main test runner
int main(int argc, char *argv[]) {
  std::cout << "Running Painting Action Tests" << std::endl;
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

  return failed > 0 ? 1 : 0;
}
