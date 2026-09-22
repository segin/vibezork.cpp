// Unit tests for V-DIAGNOSE (zil/1actions.zil:3993-4025).
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/death.h"
#include "../src/systems/melee.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "test_framework.h"

#include <sstream>

namespace {

class OutputCapture {
public:
  OutputCapture() : old_(std::cout.rdbuf(buffer_.rdbuf())) {}
  ~OutputCapture() { std::cout.rdbuf(old_); }
  std::string get() const { return buffer_.str(); }

private:
  std::stringstream buffer_;
  std::streambuf *old_;
};

void setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  DeathSystem::setTestMode(true);
  DeathSystem::reset();
  auto &g = Globals::instance();
  g.score = 0;
  if (g.winner) g.winner->setProperty(P_STRENGTH, 0);
  TimerSystem::disable("I-CURE");
}

std::string diagnose() {
  OutputCapture cap;
  Verbs::vDiagnose();
  return cap.get();
}

/// Puts a wound of the given size on the player with I-CURE running.
void wound(int points, int tick) {
  auto &g = Globals::instance();
  g.winner->setProperty(P_STRENGTH, -points);
  TimerSystem::interrupt("I-CURE", Melee::iCure);
  TimerSystem::queue("I-CURE", tick);
  TimerSystem::enable("I-CURE");
}

} // namespace

// ZIL: with I-CURE idle the wound count reads zero
// Source: zil/1actions.zil:3996-3998
TEST(DiagnosePerfectHealth) {
  setup();
  const std::string out = diagnose();
  ASSERT_CONTAINS(out, "You are in perfect health.");
  ASSERT_CONTAINS(out, "You can be killed by a serious wound.");
  ASSERT_NOT_CONTAINS(out, "cured after");
}

// ZIL: each wound size has its own wording
// Source: zil/1actions.zil:3999-4005
TEST(DiagnoseWoundWording) {
  setup();
  wound(1, 17);
  ASSERT_CONTAINS(diagnose(), "You have a light wound,");

  setup();
  wound(2, 5);
  ASSERT_CONTAINS(diagnose(), "You have a serious wound,");

  setup();
  wound(3, 5);
  ASSERT_CONTAINS(diagnose(), "You have several wounds,");

  setup();
  wound(4, 5);
  ASSERT_CONTAINS(diagnose(), "You have serious wounds,");
}

// ZIL: the cure countdown is CURE-WAIT*(WD-1) plus the interrupt's tick
// Source: zil/1actions.zil:4006-4011
TEST(DiagnoseCureCountdown) {
  setup();
  wound(1, 17);
  // 30*(1-1) + 17
  ASSERT_CONTAINS(diagnose(), "which will be cured after 17 moves.");

  setup();
  wound(2, 4);
  // 30*(2-1) + 4
  ASSERT_CONTAINS(diagnose(), "which will be cured after 34 moves.");
}

// ZIL: the survival line follows FIGHT-STRENGTH plus the wound
// Source: zil/1actions.zil:4013-4020
TEST(DiagnoseSurvivalLine) {
  setup();
  auto &g = Globals::instance();

  // Score 0 gives strength 2, unwounded
  ASSERT_CONTAINS(diagnose(), "You can be killed by a serious wound.");

  // Strength 7 at full score
  setup();
  g.score = 350;
  ASSERT_CONTAINS(diagnose(), "You can survive several wounds.");

  // Strength 3, wounded by two, leaves one
  setup();
  g.score = 70;
  wound(2, 3);
  ASSERT_CONTAINS(diagnose(), "You can be killed by one more light wound.");

  // Wounded to nothing
  setup();
  g.score = 0;
  wound(2, 3);
  ASSERT_CONTAINS(diagnose(), "You can expect death soon.");
}

// ZIL: the death count, once or twice
// Source: zil/1actions.zil:4021-4025
TEST(DiagnoseReportsDeaths) {
  setup();
  ASSERT_NOT_CONTAINS(diagnose(), "You have been killed");

  {
    OutputCapture cap;
    DeathSystem::jigsUp("Once.");
  }
  ASSERT_CONTAINS(diagnose(), "You have been killed once.");

  {
    OutputCapture cap;
    DeathSystem::jigsUp("Twice.");
  }
  ASSERT_CONTAINS(diagnose(), "You have been killed twice.");
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
