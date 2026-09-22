// Unit tests for JIGS-UP, RANDOMIZE-OBJECTS, KILL-INTERRUPTS and the PRAY
// resurrection (zil/1actions.zil:3113-3172, 4046-4135).
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/death.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"

#include <sstream>

bool deadFunction();

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
  g.score = 100;
  g.baseScore = 100;
  g.lucky = true;
}

} // namespace

// ZIL: <COND (<NOT <L? ,DEATHS 2>> ... <FINISH>)> is tested before the
// increment, so the player is resurrected twice and the third death ends it.
// Source: zil/1actions.zil:4068-4074
TEST(JigsUpResurrectsTwiceThenEnds) {
  setup();
  ASSERT_EQ(DeathSystem::getDeathCount(), 0);

  {
    OutputCapture cap;
    DeathSystem::jigsUp("First.");
    ASSERT_CONTAINS(cap.get(), "****  You have died  ****");
  }
  ASSERT_EQ(DeathSystem::getDeathCount(), 1);

  {
    OutputCapture cap;
    DeathSystem::jigsUp("Second.");
  }
  ASSERT_EQ(DeathSystem::getDeathCount(), 2);

  // The third death is the last, and never asks anything. FINISH then asks
  // for RESTART, RESTORE or QUIT (gverbs.zil:33-53).
  std::string out;
  {
    InputRedirect input("quit\ny\n");
    OutputCapture cap;
    DeathSystem::jigsUp("Third.");
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "You clearly are a suicidal maniac.");
  ASSERT_NOT_CONTAINS(out, "resurrected");
  ASSERT_NOT_CONTAINS(out, "(Y/N)");
}

// ZIL: SCORE-UPD -10 and the exact banner
// Source: zil/1actions.zil:4059-4066
TEST(JigsUpCostsTenPointsAndPrintsBanner) {
  setup();
  auto &g = Globals::instance();
  const int before = g.score;

  OutputCapture cap;
  DeathSystem::jigsUp("You are dead.");
  const std::string out = cap.get();

  ASSERT_EQ(g.score, before - 10);
  ASSERT_CONTAINS(out, "You are dead.");
  ASSERT_CONTAINS(out, "****  You have died  ****");
  // Without the temple touched, the forest resurrection
  ASSERT_CONTAINS(out, "Now, let's take a look here...");
  ASSERT_CONTAINS(out, "you can't have everything.");
}

// ZIL: "Bad luck, huh?" when LUCKY is false
// Source: zil/1actions.zil:4056-4058
TEST(JigsUpBadLuck) {
  setup();
  Globals::instance().lucky = false;
  OutputCapture cap;
  DeathSystem::jigsUp("Squashed.");
  ASSERT_CONTAINS(cap.get(), "Bad luck, huh?");
}

// ZIL: touching the South Temple sends you to Hades as a spirit
// Source: zil/1actions.zil:4077-4090
TEST(JigsUpTempleTouchedGoesToHades) {
  setup();
  auto &g = Globals::instance();
  g.getObject(RoomIds::SOUTH_TEMPLE)->setFlag(ObjectFlag::TOUCHBIT);

  std::string out;
  {
    OutputCapture cap;
    DeathSystem::jigsUp("Gone.");
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "you find yourself before the gates of Hell");
  ASSERT_TRUE(DeathSystem::isDead());
  ASSERT_TRUE(DeathSystem::alwaysLit());
  ASSERT_TRUE(g.trollFlag);
  ASSERT_EQ(g.here->getId(), RoomIds::ENTRANCE_TO_HADES);
}

// ZIL: dying while dead ends the game
// Source: zil/1actions.zil:4049-4055
TEST(JigsUpWhileDeadFinishes) {
  setup();
  DeathSystem::setDead(true);
  InputRedirect input("quit\ny\n");
  OutputCapture cap;
  DeathSystem::jigsUp("Again.");
  ASSERT_CONTAINS(cap.get(),
                  "It takes a talented person to be killed while already "
                  "dead.");
}

// ZIL: KILL-INTERRUPTS disables exactly eight and puts the match out; the
// thief and the fight demon keep running.
// Source: zil/1actions.zil:4125-4135
TEST(KillInterruptsLeavesTheThiefRunning) {
  setup();
  auto &g = Globals::instance();
  for (const char *name : {"I-XB", "I-XC", "I-CYCLOPS", "I-LANTERN",
                           "I-CANDLES", "I-SWORD", "I-FOREST-ROOM",
                           "I-MATCH"}) {
    TimerSystem::interrupt(name, [] { return false; });
    TimerSystem::queue(name, -1);
    TimerSystem::enable(name);
  }
  TimerSystem::enable("I-THIEF");
  TimerSystem::enable("I-FIGHT");
  if (auto *match = g.getObject(ObjectIds::MATCH)) {
    match->setFlag(ObjectFlag::ONBIT);
  }

  {
    OutputCapture cap;
    DeathSystem::jigsUp("Dead.");
  }

  ASSERT_TRUE(!TimerSystem::isEnabled("I-XB"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-CYCLOPS"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-LANTERN"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-CANDLES"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-SWORD"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-FOREST-ROOM"));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-MATCH"));
  ASSERT_TRUE(!g.getObject(ObjectIds::MATCH)->hasFlag(ObjectFlag::ONBIT));
  // ZIL leaves these alone
  ASSERT_TRUE(TimerSystem::isEnabled("I-THIEF"));
  ASSERT_TRUE(TimerSystem::isEnabled("I-FIGHT"));
}

// ZIL: RANDOMIZE-OBJECTS puts the lamp and coffin back and zeroes the sword
// Source: zil/1actions.zil:4101-4123
TEST(RandomizeObjectsPlacesLampAndCoffin) {
  setup();
  auto &g = Globals::instance();
  ZObject *lamp = g.getObject(ObjectIds::LAMP);
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  lamp->moveTo(g.winner);
  coffin->moveTo(g.winner);
  sword->setProperty(P_TVALUE, 5);

  {
    OutputCapture cap;
    DeathSystem::jigsUp("Dead.");
  }

  ASSERT_EQ(lamp->getLocation()->getId(), RoomIds::LIVING_ROOM);
  ASSERT_EQ(coffin->getLocation()->getId(), RoomIds::EGYPT_ROOM);
  ASSERT_EQ(sword->getProperty(P_TVALUE), 0);
  // Nothing of the player's is still carried
  ASSERT_TRUE(g.winner->getContents().empty());
}

// ZIL: DEAD-FUNCTION's refusals, and PRAY at the temple brings you back
// Source: zil/1actions.zil:3113-3172
TEST(DeadFunctionRefusalsAndPrayer) {
  setup();
  auto &g = Globals::instance();
  DeathSystem::setDead(true);

  g.prsa = V_ATTACK;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(), "All such attacks are vain in your condition.");
  }
  g.prsa = V_EAT;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(),
                    "Even such an action is beyond your capabilities.");
  }
  g.prsa = V_WAIT;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(), "Might as well. You've got an eternity.");
  }
  g.prsa = V_SCORE;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(),
                    "You're dead! How can you think of your score?");
  }
  // ZIL: RUB is listed with the "beyond your capabilities" group first, so it
  // never reaches the TAKE line.
  g.prsa = V_RUB;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(),
                    "Even such an action is beyond your capabilities.");
  }
  // ZIL: anything not listed at all
  g.prsa = V_JUMP;
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(), "You can't even do that.");
  }
  // ZIL: these fall through to the ordinary handlers
  g.prsa = V_VERSION;
  ASSERT_FALSE(deadFunction());

  // PRAY anywhere else
  g.prsa = V_PRAY;
  g.here = g.getObject(RoomIds::CELLAR);
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(), "Your prayers are not heard.");
  }

  // PRAY at the South Temple resurrects you in the forest
  g.here = g.getObject(RoomIds::SOUTH_TEMPLE);
  g.winner->moveTo(g.here);
  {
    OutputCapture cap;
    ASSERT_TRUE(deadFunction());
    ASSERT_CONTAINS(cap.get(), "the sound of a lone trumpet is heard.");
  }
  ASSERT_TRUE(!DeathSystem::isDead());
  ASSERT_TRUE(!DeathSystem::alwaysLit());
  ASSERT_EQ(g.here->getId(), RoomIds::FOREST_1);
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
