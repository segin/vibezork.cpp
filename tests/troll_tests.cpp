// Unit tests for TROLL-FCN, WEAPON-FUNCTION, AXE-F and TROLL-ROOM-F
// (zil/1actions.zil:622-764, 4175-4178).
#include "../src/core/globals.h"
#include "../src/core/gmacros.h"
#include "../src/core/go.h"
#include "../src/core/io.h"
#include "../src/core/object.h"
#include "../src/systems/melee.h"
#include "../src/systems/melee_tables.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/verbs/verbs.h"
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
  ZObject *troll = nullptr;
  ZObject *axe = nullptr;
  ZObject *room = nullptr;
};

/// Player and troll in the Troll Room, troll holding his axe.
Scene setup() {
  setScreenWidth(0);
  initializeWorld();
  goSetup();
  Melee::resetVillains();
  auto &g = Globals::instance();
  g.trollFlag = false;
  Scene s;
  s.room = g.getObject(RoomIds::TROLL_ROOM);
  s.troll = g.getObject(ObjectIds::TROLL);
  s.axe = g.getObject(ObjectIds::AXE);
  g.here = s.room;
  g.winner->moveTo(s.room);
  s.troll->moveTo(s.room);
  s.troll->clearFlag(ObjectFlag::INVISIBLE);
  s.axe->moveTo(s.troll);
  g.prso = s.troll;
  g.prsi = nullptr;
  return s;
}

std::string run(int mode) {
  OutputCapture cap;
  Villains::trollFcn(mode);
  return cap.get();
}

} // namespace

// ZIL: EXAMINE prints the LDESC, which changes through the fight
// Source: zil/1actions.zil:707-708
TEST(TrollExaminePrintsLongDesc) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.prsa = V_EXAMINE;
  const std::string out = run(0);
  ASSERT_CONTAINS(out, "A nasty-looking troll, brandishing a bloody axe");
}

// ZIL: TELL clears P-CONT and refuses
// Source: zil/1actions.zil:641-643
TEST(TrollTellRefuses) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.prsa = V_TELL;
  g.pCont = 5;
  const std::string out = run(0);
  ASSERT_CONTAINS(out, "The troll isn't much of a conversationalist.");
  ASSERT_EQ(g.pCont, 0);
}

// ZIL: LISTEN, TAKE and MUNG each have their own line
// Source: zil/1actions.zil:752-762
TEST(TrollListenTakeAndMung) {
  Scene s = setup();
  auto &g = Globals::instance();

  g.prsa = V_LISTEN;
  ASSERT_CONTAINS(run(0), "probably uncomplimentary, in his guttural tongue.");

  g.prsa = V_TAKE;
  ASSERT_CONTAINS(run(0), "The troll spits in your face, grunting");

  g.prsa = V_MUNG;
  ASSERT_CONTAINS(run(0), "The troll laughs at your puny gesture.");
}

// ZIL: F-DEAD drops the axe, clears its NDESCBIT and sets TROLL-FLAG
// Source: zil/1actions.zil:665-672
TEST(TrollDeadDropsAxeAndSetsFlag) {
  Scene s = setup();
  auto &g = Globals::instance();
  run(Melee::F_DEAD);
  ASSERT_TRUE(g.trollFlag);
  ASSERT_EQ(s.axe->getLocation(), s.room);
  ASSERT_TRUE(!s.axe->hasFlag(ObjectFlag::NDESCBIT));
  ASSERT_TRUE(s.axe->hasFlag(ObjectFlag::WEAPONBIT));
}

// ZIL: F-UNCONSCIOUS clears FIGHTBIT, drops the axe and rewrites the LDESC
// Source: zil/1actions.zil:673-684
TEST(TrollUnconsciousChangesDescription) {
  Scene s = setup();
  s.troll->setFlag(ObjectFlag::FIGHTBIT);
  run(Melee::F_UNCONSCIOUS);
  ASSERT_TRUE(!s.troll->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_CONTAINS(s.troll->getLongDesc(),
                  "An unconscious troll is sprawled on the floor.");
  ASSERT_TRUE(Globals::instance().trollFlag);
}

// ZIL: F-CONSCIOUS puts him back on his feet and takes the axe up again
// Source: zil/1actions.zil:685-700
TEST(TrollConsciousResumesFighting) {
  Scene s = setup();
  auto &g = Globals::instance();
  // Knock him out first: axe on the floor, troll down
  run(Melee::F_UNCONSCIOUS);
  ASSERT_EQ(s.axe->getLocation(), s.room);

  const std::string out = run(Melee::F_CONSCIOUS);
  ASSERT_CONTAINS(out, "The troll stirs, quickly resuming a fighting stance.");
  ASSERT_TRUE(s.troll->hasFlag(ObjectFlag::FIGHTBIT));
  // The axe was lying in the Troll Room, so he picks it back up
  ASSERT_EQ(s.axe->getLocation(), s.troll);
  ASSERT_CONTAINS(s.troll->getLongDesc(), "brandishing a bloody axe");
  ASSERT_TRUE(!g.trollFlag);
}

// ZIL: F-BUSY? with the axe in hand means he is not busy
// Source: zil/1actions.zil:644-646
TEST(TrollBusyIsFalseWhenArmed) {
  Scene s = setup();
  ASSERT_EQ(Villains::trollFcn(Melee::F_BUSY), M_NOT_HANDLED);
}

// ZIL: disarmed and with no axe to hand, he cowers
// Source: zil/1actions.zil:661-664
TEST(TrollBusyCowersWhenDisarmed) {
  Scene s = setup();
  auto &g = Globals::instance();
  s.axe->moveTo(g.winner); // not in the room, so he cannot recover it
  const std::string out = run(Melee::F_BUSY);
  ASSERT_CONTAINS(out, "The troll, disarmed, cowers in terror");
  ASSERT_CONTAINS(s.troll->getLongDesc(), "A pathetically babbling troll");
}

// ZIL: with the axe on the floor he recovers it on a 75 percent roll
// Source: zil/1actions.zil:647-660
TEST(TrollBusyRecoversAxeFromFloor) {
  bool sawRecovery = false;
  for (int seed = 1; seed <= 20 && !sawRecovery; ++seed) {
    Scene s = setup();
    s.axe->moveTo(s.room);
    GMacros::seedRandom(static_cast<uint32_t>(seed));
    const std::string out = run(Melee::F_BUSY);
    if (out.find("recovers his weapon") != std::string::npos) {
      sawRecovery = true;
      ASSERT_EQ(s.axe->getLocation(), s.troll);
      ASSERT_TRUE(s.axe->hasFlag(ObjectFlag::NDESCBIT));
      ASSERT_CONTAINS(out, "an axe to grind with you.");
    }
  }
  ASSERT_TRUE(sawRecovery);
}

// ZIL: WEAPON-FUNCTION keeps the axe out of the player's hands
// Source: zil/1actions.zil:622-638
TEST(AxeCannotBeTakenWhileTrollLives) {
  Scene s = setup();
  auto &g = Globals::instance();
  g.prsa = V_TAKE;
  g.prso = s.axe;

  std::string out;
  {
    OutputCapture cap;
    const bool handled = Villains::axeF();
    out = cap.get();
    ASSERT_TRUE(handled);
  }
  ASSERT_CONTAINS(out, "The troll swings it out of your reach.");

  // ZIL: once TROLL-FLAG is set the axe is ordinary loot again
  g.trollFlag = true;
  ASSERT_TRUE(!Villains::axeF());
}

// ZIL: GIVE of a blade is caught, and sometimes eaten fatally
// Source: zil/1actions.zil:726-750
TEST(TrollGiveBladeIsCaught) {
  auto &g = Globals::instance();
  bool sawThrowBack = false;
  bool sawFatalMeal = false;

  for (int seed = 1; seed <= 40 && !(sawThrowBack && sawFatalMeal); ++seed) {
    Scene s = setup();
    ZObject *sword = g.getObject(ObjectIds::SWORD);
    sword->moveTo(g.winner);
    g.prsa = V_GIVE;
    g.prso = sword;
    g.prsi = s.troll;
    GMacros::seedRandom(static_cast<uint32_t>(seed));

    const std::string out = run(0);
    if (out.find("throws it back") != std::string::npos) {
      sawThrowBack = true;
      ASSERT_CONTAINS(out, "graciously accepts the gift");
      ASSERT_CONTAINS(out, "the troll has poor control");
    }
    if (out.find("dies from an internal hemorrhage") != std::string::npos) {
      sawFatalMeal = true;
      ASSERT_CONTAINS(out, "eats it hungrily");
      ASSERT_TRUE(g.trollFlag);
    }
  }
  ASSERT_TRUE(sawThrowBack);
  ASSERT_TRUE(sawFatalMeal);
}

// ZIL: giving him back his own axe arms him again
// Source: zil/1actions.zil:714-721
TEST(TrollGivenAxeTakesIt) {
  Scene s = setup();
  auto &g = Globals::instance();
  s.axe->moveTo(g.winner);
  g.prsa = V_GIVE;
  g.prso = s.axe;
  g.prsi = s.troll;

  const std::string out = run(0);
  ASSERT_CONTAINS(out, "scratches his head in confusion, then takes the axe.");
  ASSERT_EQ(s.axe->getLocation(), s.troll);
  ASSERT_TRUE(s.troll->hasFlag(ObjectFlag::FIGHTBIT));
}

// ZIL: F-FIRST? lets him strike first one turn in three
// Source: zil/1actions.zil:701-706
TEST(TrollStrikesFirstSometimes) {
  int fought = 0;
  for (int seed = 1; seed <= 60; ++seed) {
    Scene s = setup();
    GMacros::seedRandom(static_cast<uint32_t>(seed));
    if (Villains::trollFcn(Melee::F_FIRST) == M_HANDLED) {
      ++fought;
      ASSERT_TRUE(s.troll->hasFlag(ObjectFlag::FIGHTBIT));
    }
  }
  // Around a third of the time, never all or none
  ASSERT_TRUE(fought > 5);
  ASSERT_TRUE(fought < 55);
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
