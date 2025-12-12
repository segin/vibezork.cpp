#include "../src/core/globals.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include <gtest/gtest.h>
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

TEST(PaintingFcn, MungDestroysValue) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    GTEST_SKIP() << "PAINTING object not defined";
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

TEST(PaintingFcn, MungChangesDescription) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    GTEST_SKIP() << "PAINTING object not defined";
  }

  g.prsa = V_MUNG;
  g.prso = painting;

  paintingAction();

  // Verify LDESC changed to "worthless piece of canvas"
  std::string ldesc = painting->getLongDesc();
  ASSERT_TRUE(ldesc.find("worthless") != std::string::npos);
  ASSERT_TRUE(ldesc.find("canvas") != std::string::npos);
}

TEST(PaintingFcn, OtherVerbsReturnFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    GTEST_SKIP() << "PAINTING object not defined";
  }

  // EXAMINE should return false (not handled by ZIL spec)
  g.prsa = V_EXAMINE;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn, TakeReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    GTEST_SKIP() << "PAINTING object not defined";
  }

  // TAKE should return false (not handled by ZIL spec)
  g.prsa = V_TAKE;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn, BurnReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  if (!painting) {
    GTEST_SKIP() << "PAINTING object not defined";
  }

  // BURN should return false (ZIL spec only handles MUNG)
  g.prsa = V_BURN;
  g.prso = painting;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn, NullObjectReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // No object set
  g.prsa = V_MUNG;
  g.prso = nullptr;

  bool result = paintingAction();

  ASSERT_FALSE(result);
}

TEST(PaintingFcn, WrongObjectReturnsFalse) {
  setupTestWorld();
  auto &g = Globals::instance();

  // Try with a different object
  ZObject *lamp = g.getObject(ObjectIds::LAMP);
  if (!lamp) {
    GTEST_SKIP() << "LAMP object not defined";
  }

  g.prsa = V_MUNG;
  g.prso = lamp; // Wrong object

  bool result = paintingAction();

  ASSERT_FALSE(result);
}
