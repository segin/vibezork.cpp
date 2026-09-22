// Unit tests for the cyclops of zil/1actions.zil:1515-1660 and V-ODYSSEUS
// (zil/gverbs.zil:945-960).
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/death.h"
#include "../src/systems/melee.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/generated/zil_tables.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/villains.h"
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

struct Scene {
  ZObject *cyclops = nullptr;
  ZObject *room = nullptr;
};

Scene setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  Melee::resetVillains();
  DeathSystem::setTestMode(true);
  DeathSystem::reset();
  auto &g = Globals::instance();
  g.cyclowrath = 0;
  g.cyclopsFlag = false;
  g.magicFlag = false;

  Scene s;
  s.room = g.getObject(RoomIds::CYCLOPS_ROOM);
  s.cyclops = g.getObject(ObjectIds::CYCLOPS);
  g.here = s.room;
  g.winner->moveTo(s.room);
  s.cyclops->moveTo(s.room);
  s.cyclops->clearFlag(ObjectFlag::INVISIBLE);
  g.prso = s.cyclops;
  g.prsi = nullptr;
  return s;
}

std::string run(int mode = 0) {
  OutputCapture cap;
  Villains::cyclopsFcn(mode);
  return cap.get();
}

} // namespace

// ZIL: EXAMINE, TAKE, TIE and LISTEN each have their own line
// Source: zil/1actions.zil:1541-1542, 1588-1596
TEST(CyclopsSimpleVerbs) {
  Scene s = setup();
  auto &g = Globals::instance();

  g.prsa = V_EXAMINE;
  ASSERT_CONTAINS(run(), "A hungry cyclops is standing at the foot of the "
                         "stairs.");

  g.prsa = V_TAKE;
  ASSERT_CONTAINS(run(), "The cyclops doesn't take kindly to being grabbed.");

  g.prsa = V_TIE;
  ASSERT_CONTAINS(run(), "You cannot tie the cyclops, though he is fit to be "
                         "tied.");

  g.prsa = V_LISTEN;
  ASSERT_CONTAINS(run(), "You can hear his stomach rumbling.");
}

// ZIL: the lunch makes him thirsty and starts the clock
// Source: zil/1actions.zil:1545-1556
TEST(CyclopsEatsTheLunch) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *lunch = g.getObject(ObjectIds::LUNCH);
  lunch->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = lunch;
  g.prsi = s.cyclops;

  const std::string out = run();
  ASSERT_CONTAINS(out, "Mmm Mmm. I love hot peppers!");
  ASSERT_CONTAINS(out, "you are \"that thing\".");
  // ZIL: <SETG CYCLOWRATH <MIN -1 <- .COUNT>>>
  ASSERT_EQ(g.cyclowrath, -1);
  ASSERT_TRUE(lunch->getLocation() == nullptr);
  ASSERT_TRUE(TimerSystem::isEnabled("I-CYCLOPS"));
}

// ZIL: water puts him to sleep, but only after the peppers
// Source: zil/1actions.zil:1557-1573
TEST(CyclopsDrinksWaterAndSleeps) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *water = g.getObject(ObjectIds::WATER);
  ZObject *bottle = g.getObject(ObjectIds::BOTTLE);
  water->moveTo(bottle);
  bottle->moveTo(g.winner);

  // Not thirsty yet
  g.prsa = V_GIVE;
  g.prso = bottle;
  g.prsi = s.cyclops;
  ASSERT_CONTAINS(run(), "apparently is not thirsty and refuses your generous "
                         "offer.");
  ASSERT_TRUE(!g.cyclopsFlag);

  // After the peppers he drinks
  g.cyclowrath = -1;
  const std::string out = run();
  ASSERT_CONTAINS(out, "takes the bottle, checks that it's open, and drinks "
                       "the water.");
  ASSERT_CONTAINS(out, "what did you put in that drink, anyway?");
  ASSERT_TRUE(g.cyclopsFlag);
  ASSERT_TRUE(!s.cyclops->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(bottle->hasFlag(ObjectFlag::OPENBIT));
}

// ZIL: garlic and everything else get their own refusals
// Source: zil/1actions.zil:1574-1580
TEST(CyclopsRefusesOtherFood) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *garlic = g.getObject(ObjectIds::GARLIC);
  garlic->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = garlic;
  g.prsi = s.cyclops;
  ASSERT_CONTAINS(run(), "The cyclops may be hungry, but there is a limit.");

  ZObject *sword = g.getObject(ObjectIds::SWORD);
  sword->moveTo(g.winner);
  g.prso = sword;
  ASSERT_CONTAINS(run(), "The cyclops is not so stupid as to eat THAT!");
}

// ZIL: attacking him only angers him, and MUNG gets its own taunt
// Source: zil/1actions.zil:1581-1587
TEST(CyclopsIgnoresAttacks) {
  Scene s = setup();
  auto &g = Globals::instance();

  g.prsa = V_ATTACK;
  ASSERT_CONTAINS(run(), "The cyclops shrugs but otherwise ignores your "
                         "pitiful attempt.");
  ASSERT_TRUE(TimerSystem::isEnabled("I-CYCLOPS"));

  g.prsa = V_MUNG;
  ASSERT_CONTAINS(run(), "\"Do you think I'm as stupid as my father was?\", he "
                         "says, dodging.");
}

// ZIL: while asleep he ignores everything but a rude awakening
// Source: zil/1actions.zil:1528-1540
TEST(CyclopsAsleepAndWoken) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.cyclopsFlag = true;
  g.cyclowrath = -3;

  g.prsa = V_EXAMINE;
  ASSERT_CONTAINS(run(), "sleeping like a baby, albeit a very ugly one.");

  g.prsa = V_ATTACK;
  const std::string out = run();
  ASSERT_CONTAINS(out, "The cyclops yawns and stares at the thing that woke "
                       "him up.");
  ASSERT_TRUE(!g.cyclopsFlag);
  ASSERT_TRUE(s.cyclops->hasFlag(ObjectFlag::FIGHTBIT));
  // ZIL takes the absolute value of the wrath when he wakes
  ASSERT_EQ(g.cyclowrath, 3);
}

// ZIL: I-CYCLOPS prints CYCLOMAD and finally eats you
// Source: zil/1actions.zil:1599-1614
TEST(ICyclopsEscalatesAndKills) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.cyclowrath = 1;

  // Each tick moves the wrath one further from zero and prints its line
  for (int expected = 2; expected <= 6; ++expected) {
    std::string out;
    {
      OutputCapture cap;
      Villains::iCyclops();
      out = cap.get();
    }
    ASSERT_EQ(g.cyclowrath, expected);
    ASSERT_CONTAINS(out, std::string(zork::zil::kCYCLOMAD[expected - 1]));
  }

  // ZIL: above five he grabs you
  std::string out;
  {
    OutputCapture cap;
    Villains::iCyclops();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "tired of all of your games and trickery, grabs you "
                       "firmly.");
  ASSERT_CONTAINS(out, "Just like Mom used to make 'em.");
}

// ZIL: all six CYCLOMAD lines are present
// Source: zil/1actions.zil:1644-1653
TEST(CyclomadHasSixLines) {
  ASSERT_EQ(std::size(zork::zil::kCYCLOMAD), static_cast<size_t>(6));
  ASSERT_EQ(std::string(zork::zil::kCYCLOMAD[4]),
            std::string("The cyclops is moving toward you in an unfriendly "
                        "manner."));
  ASSERT_EQ(std::string(zork::zil::kCYCLOMAD[5]),
            std::string("You have two choices: 1. Leave  2. Become dinner."));
}

// ZIL: leaving the room switches the demon off
// Source: zil/1actions.zil:1601-1602
TEST(ICyclopsStopsWhenYouLeave) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.cyclowrath = 2;
  TimerSystem::interrupt("I-CYCLOPS", Villains::iCyclops);
  TimerSystem::queue("I-CYCLOPS", -1);
  TimerSystem::enable("I-CYCLOPS");

  g.here = g.getObject(RoomIds::MAZE_15);
  Villains::iCyclops();
  ASSERT_TRUE(!TimerSystem::isEnabled("I-CYCLOPS"));
}

// ZIL: the room description depends on the cyclops' state
// Source: zil/1actions.zil:1615-1641
TEST(CyclopsRoomDescriptions) {
  Scene s = setup();
  auto &g = Globals::instance();

  {
    OutputCapture cap;
    Villains::cyclopsRoomFcn(M_LOOK);
    const std::string out = cap.get();
    ASSERT_CONTAINS(out, "This room has an exit on the northwest, and a "
                         "staircase leading up.");
    ASSERT_CONTAINS(out, "blocks the staircase.");
  }

  g.cyclowrath = 2;
  {
    OutputCapture cap;
    Villains::cyclopsRoomFcn(M_LOOK);
    ASSERT_CONTAINS(cap.get(), "eyeing you closely.");
  }

  g.cyclowrath = -2;
  {
    OutputCapture cap;
    Villains::cyclopsRoomFcn(M_LOOK);
    ASSERT_CONTAINS(cap.get(), "having eaten the hot peppers, appears to be "
                               "gasping.");
  }

  g.cyclowrath = 0;
  g.cyclopsFlag = true;
  {
    OutputCapture cap;
    Villains::cyclopsRoomFcn(M_LOOK);
    ASSERT_CONTAINS(cap.get(), "sleeping blissfully at the foot of the "
                               "stairs.");
  }

  g.magicFlag = true;
  {
    OutputCapture cap;
    Villains::cyclopsRoomFcn(M_LOOK);
    ASSERT_CONTAINS(cap.get(), "now has a cyclops-sized opening in it.");
  }
}

// ZIL: V-ODYSSEUS opens the east wall and takes the cyclops away
// Source: zil/gverbs.zil:945-960
TEST(OdysseusRoutsTheCyclops) {
  Scene s = setup();
  auto &g = Globals::instance();
  TimerSystem::interrupt("I-CYCLOPS", Villains::iCyclops);
  TimerSystem::queue("I-CYCLOPS", -1);
  TimerSystem::enable("I-CYCLOPS");

  std::string out;
  {
    OutputCapture cap;
    Verbs::vOdysseus();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "hearing the name of his father's deadly nemesis, flees "
                       "the room by knocking down the wall on the east of the "
                       "room.");
  ASSERT_TRUE(g.cyclopsFlag);
  ASSERT_TRUE(g.magicFlag);
  ASSERT_TRUE(s.cyclops->getLocation() == nullptr);
  ASSERT_TRUE(!TimerSystem::isEnabled("I-CYCLOPS"));
}

// ZIL: said anywhere else it is just a puzzled question
// Source: zil/gverbs.zil:959-960
TEST(OdysseusElsewhereIsPuzzled) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.here = g.getObject(RoomIds::KITCHEN);

  OutputCapture cap;
  Verbs::vOdysseus();
  ASSERT_CONTAINS(cap.get(), "Wasn't he a sailor?");
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
