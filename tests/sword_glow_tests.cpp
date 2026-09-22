// Unit tests for SWORD-FCN, I-SWORD and INFESTED?
// (zil/1actions.zil:2430-2441, 3851-3886).
//
// Rewritten for the ported routines: the glow lives in the sword's TVALUE
// property with three states, not in an ONBIT flag, and the interrupt scans
// the room and then its UEXIT/CEXIT/DEXIT neighbours.
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/light_sources.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
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

ZObject *sword() { return Globals::instance().getObject(ObjectIds::SWORD); }
ZObject *troll() { return Globals::instance().getObject(ObjectIds::TROLL); }

void setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  auto &g = Globals::instance();
  // Carry the sword: I-SWORD only runs while the adventurer holds it.
  sword()->moveTo(g.getObject(ObjectIds::ADVENTURER));
  sword()->setProperty(P_TVALUE, 0);
}

std::string fire() {
  OutputCapture cap;
  LightSources::iSword();
  return cap.get();
}

int glow() { return sword()->getProperty(P_TVALUE); }

} // namespace

// ZIL: GO queues I-SWORD but leaves it disabled; SWORD-FCN arms it on TAKE.
// Source: zil/1dungeon.zil:2639, zil/1actions.zil:2431-2433
TEST(SwordInterruptStartsQueuedButDisabled) {
  setup();
  const auto *e = TimerSystem::interrupt("I-SWORD");
  ASSERT_TRUE(e != nullptr);
  ASSERT_EQ(-1, e->tick);
  ASSERT_FALSE(e->enabled);
}

// ZIL: <AND <VERB? TAKE> <EQUAL? ,WINNER ,ADVENTURER>> arms the interrupt and
// returns <> so V-TAKE still says "Taken."
// Source: zil/1actions.zil:2431-2433
TEST(TakingTheSwordArmsTheInterrupt) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;
  g.prso = sword();
  g.prsi = nullptr;
  g.winner = g.getObject(ObjectIds::ADVENTURER);

  OutputCapture cap;
  ASSERT_EQ(M_NOT_HANDLED, sword()->performAction(0));
  ASSERT_TRUE(TimerSystem::isEnabled("I-SWORD"));
  ASSERT_EQ(-1, TimerSystem::interrupt("I-SWORD")->tick);
}

// ZIL: INFESTED? is true for a visible ACTORBIT object and false for an
// invisible one, which is how the hidden thief avoids setting off the glow.
// Source: zil/1actions.zil:3880-3886
TEST(InfestedIgnoresInvisibleActors) {
  setup();
  auto &g = Globals::instance();
  ZObject *room = g.getObject(RoomIds::TROLL_ROOM);
  ASSERT_TRUE(LightSources::infested(room));

  troll()->setFlag(ObjectFlag::INVISIBLE);
  ASSERT_FALSE(LightSources::infested(room));

  troll()->clearFlag(ObjectFlag::INVISIBLE);
  ASSERT_TRUE(LightSources::infested(room));

  // A room with no actors at all.
  ASSERT_FALSE(LightSources::infested(g.getObject(RoomIds::ATTIC)));
}

// ZIL: a villain in the room glows brightly, TVALUE 2.
// Source: zil/1actions.zil:3854, 3868-3869
TEST(SwordGlowsBrightlyBesideAVillain) {
  setup();
  auto &g = Globals::instance();
  g.here = g.getObject(RoomIds::TROLL_ROOM);
  g.winner->moveTo(g.here);

  ASSERT_CONTAINS(fire(), "Your sword has begun to glow very brightly.");
  ASSERT_EQ(2, glow());
}

// ZIL: a villain one room away glows faintly, TVALUE 1.  The Cellar's east
// exit leads to the Troll Room.
// Source: zil/1actions.zil:3856-3866, 3870-3872
TEST(SwordGlowsFaintlyThroughAnExit) {
  setup();
  auto &g = Globals::instance();
  g.here = g.getObject(RoomIds::CELLAR);
  g.winner->moveTo(g.here);

  ASSERT_CONTAINS(fire(), "Your sword is glowing with a faint blue glow.");
  ASSERT_EQ(1, glow());
}

// ZIL: the routine is silent while the glow does not change, and announces
// the sword going dark when it does.
// Source: zil/1actions.zil:3867, 3873-3874
TEST(SwordSpeaksOnlyWhenTheGlowChanges) {
  setup();
  auto &g = Globals::instance();
  g.here = g.getObject(RoomIds::TROLL_ROOM);
  g.winner->moveTo(g.here);

  ASSERT_CONTAINS(fire(), "very brightly");
  ASSERT_EQ(std::string(), fire()); // unchanged, so nothing is said

  g.here = g.getObject(RoomIds::ATTIC);
  g.winner->moveTo(g.here);
  ASSERT_CONTAINS(fire(), "Your sword is no longer glowing.");
  ASSERT_EQ(0, glow());
  ASSERT_EQ(std::string(), fire());
}

// ZIL: dropping the sword disables the interrupt from inside itself.
// Source: zil/1actions.zil:3876-3878
TEST(SwordInterruptDisablesItselfWhenNotCarried) {
  setup();
  auto &g = Globals::instance();
  TimerSystem::enable("I-SWORD");
  sword()->moveTo(g.getObject(RoomIds::ATTIC));

  ASSERT_EQ(std::string(), fire());
  ASSERT_FALSE(TimerSystem::isEnabled("I-SWORD"));
}

// ZIL: EXAMINE reports the two glowing states and stays quiet otherwise, so
// V-EXAMINE handles a dark sword.
// Source: zil/1actions.zil:2434-2441
TEST(SwordExamineReportsTheGlow) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  g.prso = sword();
  g.prsi = nullptr;

  {
    OutputCapture cap;
    ASSERT_EQ(M_NOT_HANDLED, sword()->performAction(0));
    ASSERT_EQ(std::string(), cap.get());
  }
  sword()->setProperty(P_TVALUE, 1);
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, sword()->performAction(0));
    ASSERT_CONTAINS(cap.get(), "Your sword is glowing with a faint blue glow.");
  }
  sword()->setProperty(P_TVALUE, 2);
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, sword()->performAction(0));
    ASSERT_CONTAINS(cap.get(), "Your sword is glowing very brightly.");
  }
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
