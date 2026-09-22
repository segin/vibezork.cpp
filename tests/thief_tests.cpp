// Unit tests for the thief of zil/1actions.zil:1764-2163, 3890-3990.
#include "../src/core/globals.h"
#include "../src/core/gmacros.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/melee.h"
#include "../src/systems/melee_tables.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
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
  ZObject *thief = nullptr;
  ZObject *stiletto = nullptr;
  ZObject *bag = nullptr;
  ZObject *room = nullptr;
};

/// Player and a visible thief in the Round Room.
Scene setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  Melee::resetVillains();
  auto &g = Globals::instance();
  g.thiefHere = false;
  g.thiefEngrossed = false;
  g.eggSolve = false;

  Scene s;
  s.room = g.getObject(RoomIds::ROUND_ROOM);
  s.thief = g.getObject(ObjectIds::THIEF);
  s.stiletto = g.getObject(ObjectIds::STILETTO);
  s.bag = g.getObject(ObjectIds::LARGE_BAG);
  g.here = s.room;
  g.winner->moveTo(s.room);
  s.thief->moveTo(s.room);
  s.thief->clearFlag(ObjectFlag::INVISIBLE);
  s.thief->setLongDesc(Villains::kRobberCDesc);
  s.stiletto->moveTo(s.thief);
  g.prso = s.thief;
  g.prsi = nullptr;
  return s;
}

std::string run(int mode) {
  OutputCapture cap;
  Villains::robberFunction(mode);
  return cap.get();
}

} // namespace

// ZIL: the EXAMINE description, verbatim
// Source: zil/1actions.zil:2013-2019
TEST(ThiefExamineDescription) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  const std::string out = run(0);
  ASSERT_CONTAINS(out, "The thief is a slippery character with beady eyes that "
                       "flit back and forth.");
  ASSERT_CONTAINS(out, "I'd watch out if I were you.");
}

// ZIL: TELL, TAKE and LISTEN each have their own line
// Source: zil/1actions.zil:1948-1950, 2011-2012, 2020-2022
TEST(ThiefTellTakeAndListen) {
  Scene s = setup();
  auto &g = Globals::instance();

  g.prsa = V_TELL;
  g.pCont = 3;
  ASSERT_CONTAINS(run(0), "The thief is a strong, silent type.");
  ASSERT_EQ(g.pCont, 0);

  g.prsa = V_TAKE;
  ASSERT_CONTAINS(run(0), "Once you got him, what would you do with him?");

  g.prsa = V_LISTEN;
  ASSERT_CONTAINS(run(0),
                  "The thief says nothing, as you have not been formally "
                  "introduced.");
}

// ZIL: giving a treasure engrosses him, anything else just goes in the bag
// Source: zil/1actions.zil:1999-2010
TEST(ThiefGiveTreasureEngrossesHim) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  coffin->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = coffin;
  g.prsi = s.thief;

  const std::string out = run(0);
  ASSERT_CONTAINS(out, "taken aback by your unexpected generosity");
  ASSERT_CONTAINS(out, "stops to admire its beauty.");
  ASSERT_TRUE(g.thiefEngrossed);
  ASSERT_EQ(coffin->getLocation(), s.thief);

  // A worthless item gets the other line
  Scene s2 = setup();
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  leaflet->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = leaflet;
  g.prsi = s2.thief;
  ASSERT_CONTAINS(run(0), "in his bag and thanks you politely.");
  ASSERT_TRUE(!g.thiefEngrossed);
}

// ZIL: giving to an unconscious thief wakes him first
// Source: zil/1actions.zil:1989-1998
TEST(ThiefGiveWakesTheUnconscious) {
  Scene s = setup();
  auto &g = Globals::instance();
  s.thief->setProperty(P_STRENGTH, -3);
  s.thief->setLongDesc(Villains::kRobberUDesc);
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  coffin->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = coffin;
  g.prsi = s.thief;

  const std::string out = run(0);
  ASSERT_CONTAINS(out, "Your proposed victim suddenly recovers consciousness.");
  ASSERT_EQ(s.thief->getProperty(P_STRENGTH), 3);
  ASSERT_EQ(s.thief->getLongDesc(), std::string(Villains::kRobberCDesc));
}

// ZIL: HELLO to a knocked-out thief
// Source: zil/1actions.zil:1952-1956
TEST(ThiefHelloWhenUnconscious) {
  Scene s = setup();
  auto &g = Globals::instance();
  s.thief->setLongDesc(Villains::kRobberUDesc);
  g.prsa = V_HELLO;
  ASSERT_CONTAINS(run(0), "being temporarily incapacitated, is unable to "
                          "acknowledge your greeting");
}

// ZIL: F-UNCONSCIOUS drops the stiletto and switches to the U description
// Source: zil/1actions.zil:2070-2075
TEST(ThiefUnconsciousDropsStiletto) {
  Scene s = setup();
  s.thief->setFlag(ObjectFlag::FIGHTBIT);
  run(Melee::F_UNCONSCIOUS);
  ASSERT_TRUE(!s.thief->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_EQ(s.stiletto->getLocation(), s.room);
  ASSERT_TRUE(!s.stiletto->hasFlag(ObjectFlag::NDESCBIT));
  ASSERT_EQ(s.thief->getLongDesc(), std::string(Villains::kRobberUDesc));
  ASSERT_TRUE(!TimerSystem::isEnabled("I-THIEF"));
}

// ZIL: F-CONSCIOUS revives him and re-enables the demon
// Source: zil/1actions.zil:2076-2085
TEST(ThiefConsciousRevives) {
  Scene s = setup();
  run(Melee::F_UNCONSCIOUS);
  const std::string out = run(Melee::F_CONSCIOUS);
  ASSERT_CONTAINS(out, "The robber revives, briefly feigning continued "
                       "unconsciousness");
  ASSERT_TRUE(s.thief->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(TimerSystem::isEnabled("I-THIEF"));
  ASSERT_EQ(s.thief->getLongDesc(), std::string(Villains::kRobberCDesc));
  ASSERT_EQ(s.stiletto->getLocation(), s.thief);
}

// ZIL: F-BUSY? picks up a dropped stiletto
// Source: zil/1actions.zil:2058-2069
TEST(ThiefBusyRetrievesStiletto) {
  Scene s = setup();
  s.stiletto->moveTo(s.room);
  const std::string out = run(Melee::F_BUSY);
  ASSERT_CONTAINS(out, "nimbly retrieves his stiletto.");
  ASSERT_EQ(s.stiletto->getLocation(), s.thief);
  ASSERT_TRUE(s.stiletto->hasFlag(ObjectFlag::NDESCBIT));

  // With it in hand he is not busy
  ASSERT_EQ(Villains::robberFunction(Melee::F_BUSY), M_NOT_HANDLED);
}

// ZIL: F-DEAD scatters his booty and stops the demon
// Source: zil/1actions.zil:2036-2057
TEST(ThiefDeadLeavesBooty) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  coffin->moveTo(s.thief);
  TimerSystem::enable("I-THIEF");

  const std::string out = run(Melee::F_DEAD);
  ASSERT_CONTAINS(out, "His booty remains.");
  ASSERT_EQ(coffin->getLocation(), s.room);
  ASSERT_EQ(s.stiletto->getLocation(), s.room);
  ASSERT_TRUE(!TimerSystem::isEnabled("I-THIEF"));
}

// ZIL: dying in the treasure room gives the treasures back
// Source: zil/1actions.zil:2039-2053
TEST(ThiefDeadInTreasureRoomRevealsHoard) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  g.here = treasureRoom;
  g.winner->moveTo(treasureRoom);
  s.thief->moveTo(treasureRoom);
  coffin->moveTo(treasureRoom);
  coffin->setFlag(ObjectFlag::INVISIBLE);

  const std::string out = run(Melee::F_DEAD);
  ASSERT_CONTAINS(out, "As the thief dies, the power of his magic decreases, "
                       "and his treasures reappear:");
  ASSERT_CONTAINS(out, "The chalice is now safe to take.");
  ASSERT_TRUE(!coffin->hasFlag(ObjectFlag::INVISIBLE));
}

// ZIL: DEPOSIT-BOOTY opens the egg, which only the thief can do
// Source: zil/1actions.zil:1898-1912
TEST(DepositBootyOpensTheEgg) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *egg = g.getObject(ObjectIds::EGG);
  ZObject *treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
  egg->moveTo(s.thief);

  ASSERT_TRUE(Villains::depositBooty(treasureRoom));
  ASSERT_EQ(egg->getLocation(), treasureRoom);
  ASSERT_TRUE(g.eggSolve);
  ASSERT_TRUE(egg->hasFlag(ObjectFlag::OPENBIT));
}

// ZIL: ROB takes only visible, unsacred treasure
// Source: zil/1actions.zil:3978-3990
TEST(RobTakesOnlyTreasure) {
  Scene s = setup();
  auto &g = Globals::instance();
  // The painting is a treasure with no SACREDBIT, so it is fair game.
  ZObject *painting = g.getObject(ObjectIds::PAINTING);
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  painting->moveTo(s.room);
  leaflet->moveTo(s.room);

  ASSERT_TRUE(Villains::rob(s.room, s.thief, 100));
  ASSERT_EQ(painting->getLocation(), s.thief);
  ASSERT_TRUE(painting->hasFlag(ObjectFlag::INVISIBLE)); // hidden in his bag
  ASSERT_TRUE(painting->hasFlag(ObjectFlag::TOUCHBIT));
  // No TVALUE, so the leaflet stays put
  ASSERT_EQ(leaflet->getLocation(), s.room);
}

// ZIL: a sacred treasure is safe from him
// Source: zil/1actions.zil:3984
TEST(RobSkipsSacredObjects) {
  Scene s = setup();
  auto &g = Globals::instance();
  // 1dungeon.zil:466 gives the gold coffin SACREDBIT, which is exactly what
  // keeps the thief off it.
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  ASSERT_TRUE(coffin->hasFlag(ObjectFlag::SACREDBIT));
  coffin->moveTo(s.room);
  ASSERT_TRUE(!Villains::rob(s.room, s.thief, 100));
  ASSERT_EQ(coffin->getLocation(), s.room);
}

// ZIL: RECOVER-STILETTO only picks it up from his own room
// Source: zil/1actions.zil:3951-3955
TEST(RecoverStilettoFromHisRoom) {
  Scene s = setup();
  s.stiletto->moveTo(s.room);
  Villains::recoverStiletto();
  ASSERT_EQ(s.stiletto->getLocation(), s.thief);
  ASSERT_TRUE(s.stiletto->hasFlag(ObjectFlag::NDESCBIT));
}

// ZIL: THIEF-IN-TREASURE hides everything but the chalice
// Source: zil/1actions.zil:2152-2163
TEST(ThiefInTreasureHidesTheHoard) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
  ZObject *chalice = g.getObject(ObjectIds::CHALICE);
  ZObject *coffin = g.getObject(ObjectIds::COFFIN);
  g.here = treasureRoom;
  s.thief->moveTo(treasureRoom);
  coffin->moveTo(treasureRoom);
  chalice->moveTo(treasureRoom);

  std::string out;
  {
    OutputCapture cap;
    Villains::thiefInTreasure();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "The thief gestures mysteriously, and the treasures in "
                       "the room suddenly vanish.");
  ASSERT_TRUE(coffin->hasFlag(ObjectFlag::INVISIBLE));
  ASSERT_TRUE(!chalice->hasFlag(ObjectFlag::INVISIBLE));
  ASSERT_TRUE(!s.thief->hasFlag(ObjectFlag::INVISIBLE));
}

// ZIL: entering the treasure room summons him to defend it
// Source: zil/1actions.zil:2138-2150
TEST(TreasureRoomEntryBringsTheThief) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
  g.here = treasureRoom;
  g.winner->moveTo(treasureRoom);
  TimerSystem::enable("I-THIEF");

  std::string out;
  {
    OutputCapture cap;
    Villains::treasureRoomFcn(M_ENTER);
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "You hear a scream of anguish as you violate the "
                       "robber's hideaway.");
  ASSERT_EQ(s.thief->getLocation(), treasureRoom);
  ASSERT_TRUE(s.thief->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(!s.thief->hasFlag(ObjectFlag::INVISIBLE));
}

// ZIL: the bag cannot be had while he lives
// Source: zil/1actions.zil:2088-2112
TEST(LargeBagRefusals) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.prso = s.bag;

  g.prsa = V_TAKE;
  {
    OutputCapture cap;
    ASSERT_TRUE(Villains::largeBagF());
    ASSERT_CONTAINS(cap.get(), "The bag will be taken over his dead body.");
  }

  s.thief->setLongDesc(Villains::kRobberUDesc);
  {
    OutputCapture cap;
    ASSERT_TRUE(Villains::largeBagF());
    ASSERT_CONTAINS(cap.get(), "the robber collapsed on top of the bag");
  }

  g.prsa = V_EXAMINE;
  {
    OutputCapture cap;
    ASSERT_TRUE(Villains::largeBagF());
    ASSERT_CONTAINS(cap.get(), "The bag is underneath the thief");
  }
}

// ZIL: the chalice cannot be taken while he is up and fighting
// Source: zil/1actions.zil:2124-2136
TEST(ChaliceGuardedWhileThiefFights) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *treasureRoom = g.getObject(RoomIds::TREASURE_ROOM);
  ZObject *chalice = g.getObject(ObjectIds::CHALICE);
  g.here = treasureRoom;
  s.thief->moveTo(treasureRoom);
  chalice->moveTo(treasureRoom);
  s.thief->setFlag(ObjectFlag::FIGHTBIT);
  s.thief->clearFlag(ObjectFlag::INVISIBLE);
  g.prsa = V_TAKE;
  g.prso = chalice;

  OutputCapture cap;
  ASSERT_TRUE(Villains::chaliceFcn());
  ASSERT_CONTAINS(cap.get(), "You'd be stabbed in the back first.");
}

// ZIL: THROW KNIFE either frightens him off or angers him
// Source: zil/1actions.zil:1957-1988
TEST(ThrowKnifeAtThief) {
  auto &g = Globals::instance();
  bool sawFlee = false;
  bool sawMiss = false;

  for (int seed = 1; seed <= 60 && !(sawFlee && sawMiss); ++seed) {
    Scene s = setup();
    ZObject *knife = g.getObject(ObjectIds::KNIFE);
    knife->moveTo(g.winner);
    g.prsa = V_THROW;
    g.prso = knife;
    g.prsi = s.thief;
    s.thief->clearFlag(ObjectFlag::FIGHTBIT);
    GMacros::seedRandom(static_cast<uint32_t>(seed));

    const std::string out = run(0);
    if (out.find("You evidently frightened the robber") != std::string::npos) {
      sawFlee = true;
      ASSERT_TRUE(s.thief->hasFlag(ObjectFlag::INVISIBLE));
      // ZIL puts the bag and stiletto back after emptying him out
      ASSERT_EQ(s.bag->getLocation(), s.thief);
    }
    if (out.find("You missed.") != std::string::npos) {
      sawMiss = true;
      ASSERT_TRUE(s.thief->hasFlag(ObjectFlag::FIGHTBIT));
      ASSERT_CONTAINS(out, "He does seem angered by your attempt.");
    }
  }
  ASSERT_TRUE(sawFlee);
  ASSERT_TRUE(sawMiss);
}

// ZIL: STOLE-LIGHT? notices when he took the only light source
// Source: zil/1actions.zil:1876-1882
TEST(StoleLightReportsDarkness) {
  Scene s = setup();
  auto &g = Globals::instance();
  ZObject *cellar = g.getObject(RoomIds::CELLAR);
  g.here = cellar;
  g.winner->moveTo(cellar);
  g.lit = true; // as if the lamp had been lighting the room

  std::string out;
  {
    OutputCapture cap;
    Villains::stoleLight();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "The thief seems to have left you in the dark.");
  ASSERT_TRUE(!g.lit);
}

// ZIL: F-FIRST? lets him strike first one turn in five, once announced
// Source: zil/1actions.zil:2063-2069
TEST(ThiefStrikesFirstSometimes) {
  auto &g = Globals::instance();
  int fought = 0;
  for (int seed = 1; seed <= 80; ++seed) {
    Scene s = setup();
    g.thiefHere = true;
    GMacros::seedRandom(static_cast<uint32_t>(seed));
    if (Villains::robberFunction(Melee::F_FIRST) == M_HANDLED) ++fought;
  }
  ASSERT_TRUE(fought > 2);
  ASSERT_TRUE(fought < 60);

  // Unannounced, he never starts the fight
  Scene s = setup();
  g.thiefHere = false;
  ASSERT_EQ(Villains::robberFunction(Melee::F_FIRST), M_NOT_HANDLED);
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
