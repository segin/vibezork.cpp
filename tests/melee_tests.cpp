// Unit tests for the melee engine of zil/1actions.zil:3331-3617, 3810-3846.
#include "../src/core/globals.h"
#include "../src/core/gmacros.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/death.h"
#include "../src/systems/melee.h"
#include "../src/systems/timer.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"

#include <sstream>

using namespace Melee;

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
  // No wrapping, so assertions can match whole sentences.
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  Melee::resetVillains();
  DeathSystem::setTestMode(true);
  DeathSystem::reset();
  auto &g = Globals::instance();
  g.thiefEngrossed = false;
  g.loadAllowed = g.loadMax;
  if (g.winner) g.winner->setProperty(P_STRENGTH, 0);
}

/// Puts the player and a villain in the troll room, armed.
ZObject *armAgainst(ObjectId villainId) {
  auto &g = Globals::instance();
  ZObject *room = g.getObject(RoomIds::TROLL_ROOM);
  ZObject *villain = g.getObject(villainId);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  g.here = room;
  g.winner->moveTo(room);
  villain->moveTo(room);
  villain->clearFlag(ObjectFlag::INVISIBLE);
  sword->moveTo(g.winner);
  g.prso = villain;
  g.prsi = sword;
  return villain;
}

} // namespace

// ZIL: S = STRENGTH-MIN + SCORE / (SCORE-MAX / (STRENGTH-MAX - STRENGTH-MIN))
// Source: zil/1actions.zil:3374-3381
TEST(FightStrengthScalesWithScore) {
  setup();
  auto &g = Globals::instance();

  g.score = 0;
  ASSERT_EQ(fightStrength(), STRENGTH_MIN);

  // 350/5 = 70 points per step
  g.score = 69;
  ASSERT_EQ(fightStrength(), 2);
  g.score = 70;
  ASSERT_EQ(fightStrength(), 3);
  g.score = 350;
  ASSERT_EQ(fightStrength(), STRENGTH_MAX);

  // ZIL: wounds are a negative STRENGTH on the winner, and ADJUST? <> omits them
  g.score = 140;
  g.winner->setProperty(P_STRENGTH, -2);
  ASSERT_EQ(fightStrength(), 2);
  ASSERT_EQ(fightStrength(false), 4);
}

// ZIL: VILLAIN-STRENGTH subtracts V-BEST-ADV for the weapon the villain fears
// Source: zil/1actions.zil:3383-3396
TEST(VillainStrengthWeaponAdvantage) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, 2);

  Villain *row = villainFor(ObjectIds::TROLL);
  ASSERT_TRUE(row != nullptr);

  // The sword is the troll's V-BEST, worth one point
  ASSERT_EQ(villainStrength(*row), 1);

  // A weapon that is not his V-BEST leaves him at full strength
  g.prsi = g.getObject(ObjectIds::AXE);
  g.prsi->setFlag(ObjectFlag::WEAPONBIT);
  ASSERT_EQ(villainStrength(*row), 2);

  // ZIL: the advantage never drops him below 1
  troll->setProperty(P_STRENGTH, 1);
  g.prsi = g.getObject(ObjectIds::SWORD);
  ASSERT_EQ(villainStrength(*row), 1);
}

// ZIL: an engrossed thief fights at no more than 2, and only once
// Source: zil/1actions.zil:3387-3390
TEST(VillainStrengthThiefEngrossed) {
  setup();
  auto &g = Globals::instance();
  ZObject *thief = g.getObject(ObjectIds::THIEF);
  thief->setProperty(P_STRENGTH, 5);
  g.prsi = nullptr;

  Villain *row = villainFor(ObjectIds::THIEF);
  ASSERT_TRUE(row != nullptr);

  g.thiefEngrossed = true;
  ASSERT_EQ(villainStrength(*row), 2);
  ASSERT_TRUE(!g.thiefEngrossed);

  // The cap is gone on the next blow
  ASSERT_EQ(villainStrength(*row), 5);
}

// ZIL: WINNER-RESULT stores the wound, queues I-CURE and can kill
// Source: zil/1actions.zil:3560-3574
TEST(WinnerResultWoundsAndCure) {
  setup();
  auto &g = Globals::instance();
  g.score = 350; // fight strength 7, room to be wounded

  // A wound of two points: DEF - OD is negative, so I-CURE is queued
  const int res = winnerResult(5, static_cast<int>(SERIOUS_WOUND), 7);
  ASSERT_EQ(res, static_cast<int>(SERIOUS_WOUND));
  ASSERT_EQ(g.winner->getProperty(P_STRENGTH), -2);
  ASSERT_TRUE(TimerSystem::isEnabled("I-CURE"));

  // ZIL: DEF of 0 is death; JIGS-UP runs and the result is dropped
  DeathSystem::reset();
  g.winner->setProperty(P_STRENGTH, 0);
  OutputCapture out;
  const int dead = winnerResult(0, static_cast<int>(KILLED), 7);
  ASSERT_EQ(dead, 0);
}

// ZIL: I-CURE heals one point per CURE-WAIT turns and restores LOAD-ALLOWED
// Source: zil/1actions.zil:3605-3617
TEST(ICureHealsOnePointAtATime) {
  setup();
  auto &g = Globals::instance();
  g.winner->setProperty(P_STRENGTH, -2);
  g.loadAllowed = 60;

  iCure();
  ASSERT_EQ(g.winner->getProperty(P_STRENGTH), -1);
  ASSERT_EQ(g.loadAllowed, 70);
  ASSERT_TRUE(TimerSystem::isEnabled("I-CURE"));

  iCure();
  ASSERT_EQ(g.winner->getProperty(P_STRENGTH), 0);
  ASSERT_EQ(g.loadAllowed, g.loadMax);
  ASSERT_TRUE(!TimerSystem::isEnabled("I-CURE"));
}

// ZIL: AWAKEN turns a negative strength positive and tells the villain
// Source: zil/1actions.zil:3842-3846
TEST(AwakenRestoresStrength) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = g.getObject(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, -2);
  awaken(troll);
  ASSERT_EQ(troll->getProperty(P_STRENGTH), 2);

  // A conscious villain is left alone
  awaken(troll);
  ASSERT_EQ(troll->getProperty(P_STRENGTH), 2);
}

// ZIL: VILLAIN-RESULT prints the fog and removes the body at zero strength
// Source: zil/1actions.zil:3576-3592
TEST(VillainResultDeathRemovesBody) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setFlag(ObjectFlag::FIGHTBIT);

  std::string out;
  {
    OutputCapture cap;
    villainResult(troll, 0, static_cast<int>(KILLED));
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "breathes his last breath, a cloud of sinister black fog");
  ASSERT_CONTAINS(out, "the carcass has disappeared.");
  ASSERT_TRUE(!troll->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(troll->getLocation() == nullptr);
}

// ZIL: HERO-BLOW on an unarmed or unconscious villain kills outright
// Source: zil/1actions.zil:3510-3517
TEST(HeroBlowKillsDefencelessVillain) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, 2);
  // Take his axe away so FIND-WEAPON finds nothing
  if (ZObject *axe = g.getObject(ObjectIds::AXE)) axe->moveTo(g.here);

  std::string out;
  {
    OutputCapture cap;
    heroBlow();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "cannot defend himself: He dies.");
  ASSERT_CONTAINS(out, "unarmed");
}

// ZIL: attacking something that is not a villain at all
// Source: zil/1actions.zil:3495-3502
TEST(HeroBlowPointlessAgainstZeroStrength) {
  setup();
  auto &g = Globals::instance();
  ZObject *thief = armAgainst(ObjectIds::THIEF);
  thief->setProperty(P_STRENGTH, 0);

  std::string out;
  {
    OutputCapture cap;
    heroBlow();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "is pointless.");
}

// ZIL: a staggered attacker loses the blow
// Source: zil/1actions.zil:3486-3492
TEST(HeroBlowStaggeredLosesTurn) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, 2);
  g.winner->setFlag(ObjectFlag::STAGGERED);

  std::string out;
  {
    OutputCapture cap;
    heroBlow();
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "You are still recovering from that last blow");
  ASSERT_TRUE(!g.winner->hasFlag(ObjectFlag::STAGGERED));
}

// ZIL: VILLAIN-BLOW spends a staggered villain's turn getting up
// Source: zil/1actions.zil:3416-3421
TEST(VillainBlowStaggeredRegainsFeet) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, 2);
  troll->setFlag(ObjectFlag::STAGGERED);

  Villain *row = villainFor(ObjectIds::TROLL);
  std::string out;
  {
    OutputCapture cap;
    villainBlow(*row, 0);
    out = cap.get();
  }
  ASSERT_CONTAINS(out, "slowly regains his feet.");
  ASSERT_TRUE(!troll->hasFlag(ObjectFlag::STAGGERED));
}

// ZIL: every blow the troll lands comes out of TROLL-MELEE
// Source: zil/1actions.zil:3462-3466
TEST(VillainBlowPrintsATableMessage) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = armAgainst(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, 2);
  g.score = 350; // strong enough to survive the blows

  Villain *row = villainFor(ObjectIds::TROLL);

  // Every message the engine can print for the troll, rendered for matching.
  bool sawKnown = false;
  for (int seed = 1; seed <= 25 && !sawKnown; ++seed) {
    setup();
    troll = armAgainst(ObjectIds::TROLL);
    troll->setProperty(P_STRENGTH, 2);
    g.score = 350;
    row = villainFor(ObjectIds::TROLL);
    GMacros::seedRandom(static_cast<uint32_t>(seed));

    std::string out;
    {
      OutputCapture cap;
      villainBlow(*row, 0);
      out = cap.get();
    }
    if (out.find("troll") != std::string::npos ||
        out.find("axe") != std::string::npos) {
      sawKnown = true;
    }
  }
  ASSERT_TRUE(sawKnown);
}

// ZIL: both loops stop at <GET ,VILLAINS 0>, so the last row never fights.
// That row is the cyclops, which is why CYCLOPS-MELEE is unused.
// Source: zil/1actions.zil:3334-3338, 3814-3816, zil/zork1.errors
TEST(CyclopsNeverEntersTheMeleeLoop) {
  setup();
  auto &g = Globals::instance();
  ZObject *cyclops = g.getObject(ObjectIds::CYCLOPS);
  ZObject *room = g.getObject(RoomIds::CYCLOPS_ROOM);
  g.here = room;
  g.winner->moveTo(room);
  cyclops->moveTo(room);
  cyclops->clearFlag(ObjectFlag::INVISIBLE);
  cyclops->setFlag(ObjectFlag::FIGHTBIT);

  std::string out;
  {
    OutputCapture cap;
    iFight();
    out = cap.get();
  }
  // No CYCLOPS-MELEE line can appear, because the loop never reaches his row.
  ASSERT_NOT_CONTAINS(out, "Cyclops");
  ASSERT_NOT_CONTAINS(out, "cyclops");
}

// ZIL: leaving the room calls off the fight and wakes the villain
// Source: zil/1actions.zil:3833-3839
TEST(IFightEndsWhenVillainIsElsewhere) {
  setup();
  auto &g = Globals::instance();
  ZObject *troll = g.getObject(ObjectIds::TROLL);
  troll->setProperty(P_STRENGTH, -2);
  troll->setFlag(ObjectFlag::FIGHTBIT);
  troll->setFlag(ObjectFlag::STAGGERED);
  // Player is somewhere else entirely
  ZObject *kitchen = g.getObject(RoomIds::KITCHEN);
  g.here = kitchen;
  g.winner->moveTo(kitchen);

  iFight();
  ASSERT_TRUE(!troll->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(!troll->hasFlag(ObjectFlag::STAGGERED));
  ASSERT_EQ(troll->getProperty(P_STRENGTH), 2); // AWAKEN
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
