// Unit tests for MATCH-FUNCTION and I-MATCH (zil/1actions.zil:2260-2300).
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
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

ZObject *match() { return Globals::instance().getObject(ObjectIds::MATCH); }

void setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  auto &g = Globals::instance();
  // initializeWorld() rebuilds the objects but leaves the game globals alone,
  // so restore MATCH-COUNT's initial value between cases.
  g.matchCount = 6;
  // A lit room so striking a match does not trigger the V-LOOK branch.
  if (auto *lr = g.getObject(RoomIds::LIVING_ROOM)) {
    g.here = lr;
    lr->setFlag(ObjectFlag::ONBIT);
    g.winner->moveTo(lr);
    g.lit = true;
  }
  match()->moveTo(g.winner);
}

std::string act(VerbId verb, ZObject *direct = nullptr,
                ZObject *indirect = nullptr) {
  auto &g = Globals::instance();
  g.prsa = verb;
  g.prso = direct ? direct : match();
  g.prsi = indirect;
  OutputCapture cap;
  match()->performAction(0);
  return cap.get();
}

} // namespace

// ZIL: <GLOBAL MATCH-COUNT 6>, and COUNT reports one less, so a fresh
// matchbook says five.
// Source: zil/1actions.zil:2260, 2288-2295
TEST(MatchCountStartsAtSixAndReportsFive) {
  setup();
  ASSERT_EQ(6, Globals::instance().matchCount);
  ASSERT_CONTAINS(act(V_COUNT), "You have 5 matches.");
}

// ZIL: each strike decrements first, so the reported count follows.
// Source: zil/1actions.zil:2263-2265, 2288-2295
TEST(MatchCountFallsWithEachStrike) {
  setup();
  act(V_LAMP_ON);
  ASSERT_CONTAINS(act(V_COUNT), "You have 4 matches.");
  act(V_LAMP_ON);
  act(V_LAMP_ON);
  act(V_LAMP_ON);
  ASSERT_CONTAINS(act(V_COUNT), "You have 1 match.");
}

// ZIL: the count runs out at zero, and the singular/plural and "no" forms
// come from the same branch.
// Source: zil/1actions.zil:2266-2268, 2288-2295
TEST(MatchesRunOut) {
  setup();
  for (int i = 0; i < 5; ++i)
    act(V_LAMP_ON);
  ASSERT_CONTAINS(act(V_COUNT), "You have no matches.");
  ASSERT_CONTAINS(act(V_LAMP_ON), "I'm afraid that you have run out of matches.");
}

// ZIL: OPEN shares the COUNT branch.
// Source: zil/1actions.zil:2288
TEST(MatchOpenReportsTheCount) {
  setup();
  ASSERT_CONTAINS(act(V_OPEN), "You have 5 matches.");
}

// ZIL: a successful strike sets FLAMEBIT and ONBIT, queues I-MATCH for two
// turns and says so.
// Source: zil/1actions.zil:2272-2281
TEST(MatchStrikeLightsAndQueuesTheInterrupt) {
  setup();
  ASSERT_CONTAINS(act(V_LAMP_ON), "One of the matches starts to burn.");
  ASSERT_TRUE(match()->hasFlag(ObjectFlag::FLAMEBIT));
  ASSERT_TRUE(match()->hasFlag(ObjectFlag::ONBIT));
  const auto *e = TimerSystem::interrupt("I-MATCH");
  ASSERT_TRUE(e != nullptr);
  ASSERT_EQ(2, e->tick);
  ASSERT_TRUE(e->enabled);
}

// ZIL: two drafty rooms blow the match out at once, and the count is already
// spent by then.
// Source: zil/1actions.zil:2269-2271
TEST(MatchGoesOutInstantlyInDraftyRooms) {
  for (ObjectId room : {RoomIds::LOWER_SHAFT, RoomIds::TIMBER_ROOM}) {
    setup();
    auto &g = Globals::instance();
    g.here = g.getObject(room);
    ASSERT_CONTAINS(act(V_LAMP_ON),
                    "This room is drafty, and the match goes out instantly.");
    ASSERT_FALSE(match()->hasFlag(ObjectFlag::ONBIT));
    ASSERT_EQ(5, g.matchCount);
  }
}

// ZIL: blowing out a burning match, and the darkness note when nothing else
// is lit.
// Source: zil/1actions.zil:2282-2287
TEST(MatchLampOff) {
  setup();
  act(V_LAMP_ON);
  std::string out = act(V_LAMP_OFF);
  ASSERT_CONTAINS(out, "The match is out.");
  ASSERT_NOT_CONTAINS(out, "It's pitch black in here!");
  ASSERT_FALSE(match()->hasFlag(ObjectFlag::FLAMEBIT));
  ASSERT_EQ(0, TimerSystem::interrupt("I-MATCH")->tick);

  setup();
  auto &g = Globals::instance();
  g.here = g.getObject(RoomIds::CELLAR);
  g.here->clearFlag(ObjectFlag::ONBIT);
  g.winner->moveTo(g.here);
  act(V_LAMP_ON);
  ASSERT_CONTAINS(act(V_LAMP_OFF), "It's pitch black in here!");
}

// ZIL: EXAMINE distinguishes a burning match from the matchbook.
// Source: zil/1actions.zil:2296-2302
TEST(MatchExamine) {
  setup();
  ASSERT_CONTAINS(act(V_EXAMINE), "The matchbook isn't very interesting, "
                                  "except for what's written on it.");
  act(V_LAMP_ON);
  ASSERT_CONTAINS(act(V_EXAMINE), "The match is burning.");
}

// ZIL: the LAMP-ON branch is guarded by <EQUAL? ,PRSO ,MATCH>, so lighting
// something else with the match leaves the matchbook's routine alone.  This
// is what lets CANDLES-FCN's "(with the match)" re-dispatch reach the
// candles.
// Source: zil/1actions.zil:2262
TEST(MatchDeclinesWhenItIsNotTheDirectObject) {
  setup();
  auto &g = Globals::instance();
  ZObject *candles = g.getObject(ObjectIds::CANDLES);
  candles->clearFlag(ObjectFlag::ONBIT);
  match()->setFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  g.prsa = V_LAMP_ON;
  g.prso = candles;
  g.prsi = match();
  ASSERT_EQ(M_NOT_HANDLED, match()->performAction(0));
  ASSERT_EQ(std::string(), cap.get());
}

// ZIL: CANDLES-FCN's implicit-match path now reaches the candles, because
// MATCH-FUNCTION no longer swallows the verb.
// Source: zil/1actions.zil:2355-2357, 2363-2371
TEST(CandlesLitByTheImplicitMatchThroughPerform) {
  setup();
  auto &g = Globals::instance();
  ZObject *candles = g.getObject(ObjectIds::CANDLES);
  candles->clearFlag(ObjectFlag::ONBIT);
  candles->moveTo(g.winner);
  match()->setFlag(ObjectFlag::FLAMEBIT);
  match()->setFlag(ObjectFlag::ONBIT);

  g.prsa = V_LAMP_ON;
  g.prso = candles;
  g.prsi = nullptr;
  std::string out;
  {
    OutputCapture cap;
    candles->performAction(0);
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "(with the match)");
  ASSERT_CONTAINS(out, "The candles are lit.");
  ASSERT_TRUE(candles->hasFlag(ObjectFlag::ONBIT));
}

// ZIL: I-MATCH announces the match going out and clears both flags.
// Source: zil/1actions.zil:2296-2300
TEST(IMatchPutsTheMatchOut) {
  setup();
  act(V_LAMP_ON);

  std::string out;
  {
    OutputCapture cap;
    bool flg = TimerSystem::interrupt("I-MATCH")->routine();
    ASSERT_TRUE(flg);
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "The match has gone out.");
  ASSERT_FALSE(match()->hasFlag(ObjectFlag::FLAMEBIT));
  ASSERT_FALSE(match()->hasFlag(ObjectFlag::ONBIT));
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
