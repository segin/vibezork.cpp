// Unit tests for CANDLES-FCN, I-CANDLES and CANDLE-TABLE
// (zil/1actions.zil:2310-2317, 2343-2413).
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

ZObject *candles() { return Globals::instance().getObject(ObjectIds::CANDLES); }
ZObject *match() { return Globals::instance().getObject(ObjectIds::MATCH); }

void setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  LightSources::reset();
  auto &g = Globals::instance();
  // A lit room, and the candles in hand so LIGHT-INT speaks.
  if (auto *lr = g.getObject(RoomIds::LIVING_ROOM)) {
    g.here = lr;
    lr->setFlag(ObjectFlag::ONBIT);
    if (g.winner)
      g.winner->moveTo(lr);
  }
  if (candles())
    candles()->moveTo(g.winner);
}

/// The candles start burning on the altar (1dungeon.zil:675), so a test that
/// wants them out has to put them out first.
void douse() {
  candles()->clearFlag(ObjectFlag::ONBIT);
}

std::string act(VerbId verb, ZObject *indirect = nullptr) {
  auto &g = Globals::instance();
  g.prsa = verb;
  g.prso = candles();
  g.prsi = indirect;
  OutputCapture cap;
  candles()->performAction(0);
  return cap.get();
}

std::string fire() {
  OutputCapture cap;
  LightSources::iCandles();
  return cap.get();
}

} // namespace

// ZIL: <QUEUE I-CANDLES 40> in GO, disabled until the candles are touched.
// Source: zil/1dungeon.zil:2641, zil/1actions.zil:2344-2345
TEST(CandlesStartQueuedAtFortyAndDisabled) {
  setup();
  const auto *e = TimerSystem::interrupt("I-CANDLES");
  ASSERT_TRUE(e != nullptr);
  ASSERT_EQ(40, e->tick);
  ASSERT_FALSE(e->enabled);
}

// ZIL: the candles ship burning on the altar.
// Source: zil/1dungeon.zil:670-678
TEST(CandlesShipBurning) {
  setup();
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::ONBIT));
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::FLAMEBIT));
  ASSERT_CONTAINS(act(V_EXAMINE), "The candles are burning.");
}

// ZIL: the routine's first COND enables I-CANDLES whenever the candles lack
// TOUCHBIT, whatever the verb is.
// Source: zil/1actions.zil:2344-2345
TEST(CandlesEnableTheInterruptOnFirstTouch) {
  setup();
  candles()->clearFlag(ObjectFlag::TOUCHBIT);
  act(V_EXAMINE);
  ASSERT_TRUE(TimerSystem::isEnabled("I-CANDLES"));
}

// ZIL: CANDLE-TABLE is 20/"grow shorter", 10/"quite short", 5/"won't last
// long", 0, so with GO's 40 the candles last 40 + 20 + 10 + 5 turns.
// Source: zil/1actions.zil:2406-2413, 2310-2317
TEST(CandlesStepThroughCandleTable) {
  setup();
  ASSERT_CONTAINS(fire(), "The candles grow shorter.");
  ASSERT_EQ(20, TimerSystem::interrupt("I-CANDLES")->tick);

  ASSERT_CONTAINS(fire(), "The candles are becoming quite short.");
  ASSERT_EQ(10, TimerSystem::interrupt("I-CANDLES")->tick);

  ASSERT_CONTAINS(fire(), "The candles won't last long now.");
  ASSERT_EQ(5, TimerSystem::interrupt("I-CANDLES")->tick);
}

// ZIL: I-CANDLES sets TOUCHBIT every time it runs, and the zero entry leaves
// the candles in the world with RMUNGBIT rather than removing them.
// Source: zil/1actions.zil:2311, 2319-2322
TEST(CandlesBurnDownToRmungbitAndStay) {
  setup();
  candles()->setFlag(ObjectFlag::ONBIT);
  fire();
  fire();
  fire();
  std::string out = fire();

  ASSERT_CONTAINS(out, "You'd better have more light than from the pair of candles.");
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::RMUNGBIT));
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::TOUCHBIT));
  ASSERT_FALSE(candles()->hasFlag(ObjectFlag::ONBIT));
  // Still held, not removed from play.
  ASSERT_TRUE(candles()->getLocation() == Globals::instance().winner);
}

// ZIL: a burned-out pair refuses to light.
// Source: zil/1actions.zil:2350-2354
TEST(CandlesRefuseToBurnOnceRmunged) {
  setup();
  candles()->setFlag(ObjectFlag::RMUNGBIT);
  ASSERT_CONTAINS(act(V_LAMP_ON),
                  "Alas, there's not much left of the candles. Certainly not "
                  "enough to burn.");
}

// ZIL: with no PRSI and no burning match the routine says so and RFATALs.
// Source: zil/1actions.zil:2358-2362
TEST(CandlesWithoutSomethingToLightThemIsFatal) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_LAMP_ON;
  g.prso = candles();
  g.prsi = nullptr;

  OutputCapture cap;
  ASSERT_EQ(M_FATAL, candles()->performAction(0));
  ASSERT_CONTAINS(cap.get(), "You should say what to light them with.");
}

// ZIL: a burning match in play is used implicitly, announced as
// "(with the match)" and re-dispatched through PERFORM.
// Source: zil/1actions.zil:2355-2357
TEST(CandlesUseABurningMatchImplicitly) {
  setup();
  douse();
  match()->setFlag(ObjectFlag::FLAMEBIT);
  match()->setFlag(ObjectFlag::ONBIT);
  match()->moveTo(Globals::instance().winner);

  // This branch's own work is the announcement and the re-dispatch; what the
  // re-dispatch then prints belongs to MATCH-FUNCTION, which is still the
  // invented handler and swallows LAMP-ON regardless of PRSO.
  // TODO(F3): assert "The candles are lit." here once MATCH-FUNCTION is ported.
  ASSERT_CONTAINS(act(V_LAMP_ON), "(with the match)");
}

// ZIL: lighting them with a burning match held as the indirect object.
// Source: zil/1actions.zil:2363-2371
TEST(CandlesLitByAnExplicitMatch) {
  setup();
  douse();
  match()->setFlag(ObjectFlag::ONBIT);

  ASSERT_CONTAINS(act(V_LAMP_ON, match()), "The candles are lit.");
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::ONBIT));
  ASSERT_TRUE(TimerSystem::isEnabled("I-CANDLES"));
}

// ZIL: lighting them twice says "already lit."
// Source: zil/1actions.zil:2363-2371
TEST(CandlesAlreadyLit) {
  setup();
  match()->setFlag(ObjectFlag::ONBIT);
  candles()->setFlag(ObjectFlag::ONBIT);
  ASSERT_CONTAINS(act(V_LAMP_ON, match()), "The candles are already lit.");
}

// ZIL: the torch vaporizes them, or warns you in time if they are lit.
// Source: zil/1actions.zil:2372-2381
TEST(CandlesAndTheTorch) {
  setup();
  douse();
  ASSERT_CONTAINS(act(V_LAMP_ON, Globals::instance().getObject(ObjectIds::TORCH)),
                  "The heat from the torch is so intense that the candles are "
                  "vaporized.");

  // Re-initialising the world invalidates every object pointer, so the torch
  // has to be fetched again on the far side of setup().
  setup();
  ASSERT_CONTAINS(act(V_LAMP_ON, Globals::instance().getObject(ObjectIds::TORCH)),
                  "You realize, just in time, that the candles are already "
                  "lighted.");
}

// ZIL: anything else that is not burning gets the generic refusal.
// Source: zil/1actions.zil:2382-2384
TEST(CandlesNeedSomethingBurning) {
  setup();
  douse();
  ZObject *sword = Globals::instance().getObject(ObjectIds::SWORD);
  ASSERT_CONTAINS(
      act(V_LAMP_ON, sword),
      "You have to light them with something that's burning, you know.");
}

// ZIL: LAMP-OFF disables the interrupt, extinguishes, sets TOUCHBIT and adds
// " It's really dark in here...." when the room goes dark.
// Source: zil/1actions.zil:2388-2400
TEST(CandlesLampOff) {
  setup();
  auto &g = Globals::instance();
  candles()->setFlag(ObjectFlag::ONBIT);
  TimerSystem::enable("I-CANDLES");

  std::string out = act(V_LAMP_OFF);
  ASSERT_CONTAINS(out, "The flame is extinguished.");
  ASSERT_NOT_CONTAINS(out, "It's really dark in here");
  ASSERT_FALSE(TimerSystem::isEnabled("I-CANDLES"));
  ASSERT_TRUE(candles()->hasFlag(ObjectFlag::TOUCHBIT));

  // In a dark room the same branch appends the darkness clause.
  setup();
  if (auto *cellar = g.getObject(RoomIds::CELLAR)) {
    g.here = cellar;
    cellar->clearFlag(ObjectFlag::ONBIT);
    g.winner->moveTo(cellar);
    candles()->moveTo(g.winner);
  }
  candles()->setFlag(ObjectFlag::ONBIT);
  ASSERT_CONTAINS(act(V_LAMP_OFF), " It's really dark in here....");
}

// ZIL: extinguishing unlit candles.
// Source: zil/1actions.zil:2401
TEST(CandlesNotLighted) {
  setup();
  douse();
  ASSERT_CONTAINS(act(V_LAMP_OFF), "The candles are not lighted.");
}

// ZIL: <AND <VERB? PUT> <FSET? ,PRSI ,BURNBIT>> -> "That wouldn't be smart."
// Source: zil/1actions.zil:2402-2403
TEST(CandlesIntoSomethingBurnable) {
  setup();
  ZObject *sack = Globals::instance().getObject(ObjectIds::SANDWICH_BAG);
  ASSERT_TRUE(sack != nullptr);
  ASSERT_TRUE(sack->hasFlag(ObjectFlag::BURNBIT));
  ASSERT_CONTAINS(act(V_PUT, sack), "That wouldn't be smart.");
}

// ZIL: COUNT and EXAMINE.
// Source: zil/1actions.zil:2385-2387, 2404-2409
TEST(CandlesCountAndExamine) {
  setup();
  ASSERT_CONTAINS(act(V_COUNT), "Let's see, how many objects in a pair? Don't "
                                "tell me, I'll get it.");
  // They ship lit, so EXAMINE reports burning until they are put out.
  ASSERT_CONTAINS(act(V_EXAMINE), "The candles are burning.");
  douse();
  ASSERT_CONTAINS(act(V_EXAMINE), "The candles are out.");
}

// ZIL: <COND (<EQUAL? ,CANDLES ,PRSI> <RFALSE>)> — when the candles are what
// you are lighting something else *with*, the routine declines entirely.
// Source: zil/1actions.zil:2346
TEST(CandlesAsTheIndirectObjectDecline) {
  setup();
  auto &g = Globals::instance();
  g.prsa = V_LAMP_ON;
  g.prso = g.getObject(ObjectIds::MATCH);
  g.prsi = candles();

  OutputCapture cap;
  ASSERT_EQ(M_NOT_HANDLED, candles()->performAction(0));
  ASSERT_EQ(std::string(), cap.get());
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
