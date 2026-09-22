// Unit tests for LANTERN, I-LANTERN, LAMP-TABLE and LIGHT-INT
// (zil/1actions.zil:2216-2254, 2303-2330).
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/light_sources.h"
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
  LightSources::reset();
  auto &g = Globals::instance();
  // Hold the lamp so LIGHT-INT's <HELD? .OBJ> test passes.
  if (auto *lamp = g.getObject(ObjectIds::LAMP))
    lamp->moveTo(g.winner);
}

ZObject *lamp() { return Globals::instance().getObject(ObjectIds::LAMP); }

/// Run I-LANTERN once and capture what it printed.
std::string fire() {
  OutputCapture cap;
  LightSources::iLantern();
  return cap.get();
}

} // namespace

// ZIL: <QUEUE I-LANTERN 200> in GO, so the interrupt is armed for turn 200
// and disabled until LANTERN enables it.
// Source: zil/1dungeon.zil:2642, zil/1actions.zil:2241
TEST(LanternStartsQueuedAtTwoHundredAndDisabled) {
  setup();
  const auto *e = TimerSystem::interrupt("I-LANTERN");
  ASSERT_TRUE(e != nullptr);
  ASSERT_EQ(200, e->tick);
  ASSERT_FALSE(e->enabled);
}

// ZIL: LAMP-TABLE is 100/"a bit dimmer", 70/"definitely dimmer", 15/"nearly
// out", 0, so the lamp lasts 200 + 100 + 70 + 15 turns and each step prints
// its own line before the table advances.
// Source: zil/1actions.zil:2216-2224, 2303-2308
TEST(LanternStepsThroughLampTable) {
  setup();

  ASSERT_CONTAINS(fire(), "The lamp appears a bit dimmer.");
  ASSERT_EQ(100, TimerSystem::interrupt("I-LANTERN")->tick);

  ASSERT_CONTAINS(fire(), "The lamp is definitely dimmer now.");
  ASSERT_EQ(70, TimerSystem::interrupt("I-LANTERN")->tick);

  ASSERT_CONTAINS(fire(), "The lamp is nearly out.");
  ASSERT_EQ(15, TimerSystem::interrupt("I-LANTERN")->tick);
}

// ZIL: LIGHT-INT on a zero tick clears ONBIT, sets RMUNGBIT and prints
// "You'd better have more light than from the <object>."
// Source: zil/1actions.zil:2319-2330
TEST(LanternBurnsOutOnTheZeroEntry) {
  setup();
  lamp()->setFlag(ObjectFlag::ONBIT);
  fire();
  fire();
  fire();

  std::string out = fire();
  ASSERT_CONTAINS(out, "You'd better have more light than from the brass lantern.");
  ASSERT_FALSE(lamp()->hasFlag(ObjectFlag::ONBIT));
  ASSERT_TRUE(lamp()->hasFlag(ObjectFlag::RMUNGBIT));
}

// ZIL: LIGHT-INT only speaks when the object is held or in the room.
// Source: zil/1actions.zil:2323
TEST(LightIntIsSilentWhenTheLampIsElsewhere) {
  setup();
  auto &g = Globals::instance();
  lamp()->moveTo(g.getObject(RoomIds::ATTIC));
  ASSERT_EQ(std::string(), fire());
}

// ZIL: <COND (<FSET? ,LAMP ,RMUNGBIT> <TELL "A burned-out lamp won't light.">)>
// Source: zil/1actions.zil:2233-2235
TEST(LanternRefusesToRelightWhenBurnedOut) {
  setup();
  lamp()->setFlag(ObjectFlag::RMUNGBIT);

  OutputCapture cap;
  auto &g = Globals::instance();
  g.prsa = V_LAMP_ON;
  g.prso = lamp();
  int res = lamp()->performAction(0);
  ASSERT_EQ(M_HANDLED, res);
  ASSERT_CONTAINS(cap.get(), "A burned-out lamp won't light.");
}

// ZIL: LAMP-OFF on a burned-out lamp says so rather than toggling.
// Source: zil/1actions.zil:2239-2241
TEST(LanternSaysItAlreadyBurnedOutWhenTurnedOff) {
  setup();
  lamp()->setFlag(ObjectFlag::RMUNGBIT);

  OutputCapture cap;
  auto &g = Globals::instance();
  g.prsa = V_LAMP_OFF;
  g.prso = lamp();
  ASSERT_EQ(M_HANDLED, lamp()->performAction(0));
  ASSERT_CONTAINS(cap.get(), "The lamp has already burned out.");
}

// ZIL: LAMP-ON enables I-LANTERN and returns <> so V-LAMP-ON speaks.
// Source: zil/1actions.zil:2236-2238
TEST(LanternEnablesTheInterruptAndFallsThrough) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_LAMP_ON;
  g.prso = lamp();

  OutputCapture cap;
  ASSERT_EQ(M_NOT_HANDLED, lamp()->performAction(0));
  ASSERT_TRUE(TimerSystem::isEnabled("I-LANTERN"));
}

// ZIL: THROW smashes the lamp, disables the interrupt and leaves BROKEN-LAMP.
// Source: zil/1actions.zil:2227-2232
TEST(LanternThrowLeavesTheBrokenLamp) {
  setup();
  auto &g = Globals::instance();
  TimerSystem::enable("I-LANTERN");
  g.prsa = V_THROW;
  g.prso = lamp();

  std::string out;
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, lamp()->performAction(0));
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "The lamp has smashed into the floor, and the light has gone out.");
  ASSERT_FALSE(TimerSystem::isEnabled("I-LANTERN"));
  ASSERT_TRUE(g.getObject(ObjectIds::BROKEN_LAMP)->getLocation() == g.here);
}

// ZIL: EXAMINE reports burned out, on, or turned off.
// Source: zil/1actions.zil:2243-2253
TEST(LanternExamineReportsState) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  g.prso = lamp();

  {
    OutputCapture cap;
    lamp()->performAction(0);
    ASSERT_CONTAINS(cap.get(), "The lamp is turned off.");
  }
  lamp()->setFlag(ObjectFlag::ONBIT);
  {
    OutputCapture cap;
    lamp()->performAction(0);
    ASSERT_CONTAINS(cap.get(), "The lamp is on.");
  }
  lamp()->setFlag(ObjectFlag::RMUNGBIT);
  {
    OutputCapture cap;
    lamp()->performAction(0);
    ASSERT_CONTAINS(cap.get(), "The lamp has burned out.");
  }
}

// ZIL: TORCH-OBJECT. The torch always burns: EXAMINE says so, water
// evaporates before it arrives, and trying to put it out burns your hand.
// It has no fuel and cannot be lit.
// Source: zil/1actions.zil:944-953
TEST(TorchIsAlwaysBurning) {
  setup();
  auto &g = Globals::instance();
  ZObject *torch = g.getObject(ObjectIds::TORCH);
  ASSERT_TRUE(torch != nullptr);
  ASSERT_TRUE(torch->hasFlag(ObjectFlag::ONBIT));
  ASSERT_TRUE(torch->hasFlag(ObjectFlag::FLAMEBIT));

  g.prso = torch;
  g.prsi = nullptr;
  g.prsa = V_EXAMINE;
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, torch->performAction(0));
    ASSERT_CONTAINS(cap.get(), "The torch is burning.");
  }

  g.prsa = V_LAMP_OFF;
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, torch->performAction(0));
    ASSERT_CONTAINS(cap.get(),
                    "You nearly burn your hand trying to extinguish the flame.");
  }
  ASSERT_TRUE(torch->hasFlag(ObjectFlag::ONBIT));

  // POUR water ON the torch: the torch is the indirect object.
  g.prsa = V_POUR_ON;
  g.prso = g.getObject(ObjectIds::WATER);
  g.prsi = torch;
  {
    OutputCapture cap;
    ASSERT_EQ(M_HANDLED, torch->performAction(0));
    ASSERT_CONTAINS(cap.get(), "The water evaporates before it gets close.");
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
