/**
 * @file gglobals_tests.cpp
 * @brief Unit tests for routines, variables, and objects from gglobals.zil
 *
 * Source: zil/gglobals.zil:1-309
 */

#include "core/flags.h"
#include "core/gglobals.h"
#include "core/globals.h"
#include "core/object.h"
#include "systems/death.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"
#include "test_framework.h"
#include <cassert>
#include <iostream>
#include <sstream>

class OutputCapture {
public:
  OutputCapture() : old_cout(std::cout.rdbuf(buffer.rdbuf())) {}
  ~OutputCapture() { std::cout.rdbuf(old_cout); }
  std::string getOutput() const { return buffer.str(); }

private:
  std::stringstream buffer;
  std::streambuf *old_cout;
};

// ============================================================================
// Routine Tests: NULL-F (gglobals.zil:85-87)
// ============================================================================

TEST(NullF_ReturnsFalse) {
  assert(!GGlobals::nullF());
  int dummy = 42;
  assert(!GGlobals::nullF(&dummy, &dummy));
}

// ============================================================================
// Routine Tests: NOT-HERE-OBJECT-F (gglobals.zil:52-74)
// ============================================================================

TEST(NotHereObjectF_BothNotHere) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  assert(notHere != nullptr);

  g.prso = notHere;
  g.prsi = notHere;

  OutputCapture cap;
  bool res = GGlobals::notHereObjectF();
  assert(res);
  assert(cap.getOutput().find("Those things aren't here!") != std::string::npos);
}

TEST(NotHereObjectF_PlayerWinner_DirectObject) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  g.prso = notHere;
  g.prsi = nullptr;
  g.winner = g.player;
  g.pOflag = false;
  g.pNc1 = {"brass", "lantern"};

  OutputCapture cap;
  bool res = GGlobals::notHereObjectF();
  assert(res);
  assert(cap.getOutput().find("You can't see any brass lantern here!") != std::string::npos);
  assert(!g.pCont);
  assert(!g.quoteFlag);
}

TEST(NotHereObjectF_PlayerWinner_IndirectObject) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  g.prso = g.getObject(ObjectIds::SWORD);
  g.prsi = notHere;
  g.winner = g.player;
  g.pOflag = false;
  g.pNc2 = {"rusty", "knife"};

  OutputCapture cap;
  bool res = GGlobals::notHereObjectF();
  assert(res);
  assert(cap.getOutput().find("You can't see any rusty knife here!") != std::string::npos);
}

TEST(NotHereObjectF_NPCWinner) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  ZObject *thief = g.getObject(ObjectIds::THIEF);
  assert(thief != nullptr);

  g.prso = notHere;
  g.prsi = nullptr;
  g.winner = thief;
  g.pOflag = false;
  g.pNc1 = {"gold", "coin"};

  OutputCapture cap;
  bool res = GGlobals::notHereObjectF();
  assert(res);
  assert(cap.getOutput().find("The thief seems confused. \"I don't see any gold coin here!\"") != std::string::npos);
}

TEST(NotHerePrint_OrphanMode) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  g.prso = notHere;
  g.winner = g.player;
  g.pOflag = true;
  g.pXadjn = "shiny";
  g.pXnam = "key";

  OutputCapture cap;
  bool res = GGlobals::notHereObjectF();
  assert(res);
  assert(cap.getOutput().find("You can't see any shiny key here!") != std::string::npos);
}

// ============================================================================
// Routine Tests: STAIRS-F (gglobals.zil:110-113)
// ============================================================================

TEST(StairsF_Through) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_THROUGH;
  OutputCapture cap;
  bool res = GGlobals::stairsF();
  assert(res);
  assert(cap.getOutput().find("You should say whether you want to go up or down.") != std::string::npos);
}

TEST(StairsF_Unhandled) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_EXAMINE;
  assert(!GGlobals::stairsF());
}

// ============================================================================
// Routine Tests: SAILOR-FCN (gglobals.zil:122-162)
// ============================================================================

TEST(SailorFcn_Tell) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_TELL;
  g.pCont = true;
  g.quoteFlag = true;

  OutputCapture cap;
  bool res = GGlobals::sailorFcn();
  assert(res);
  assert(cap.getOutput().find("You can't talk to the sailor that way.") != std::string::npos);
  assert(!g.pCont);
  assert(!g.quoteFlag);
}

TEST(SailorFcn_Examine) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_EXAMINE;
  OutputCapture cap;
  bool res = GGlobals::sailorFcn();
  assert(res);
  assert(cap.getOutput().find("There is no sailor to be seen.") != std::string::npos);
}

TEST(SailorFcn_HelloProgression) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_HELLO;
  g.hs = 0;

  // Turns 1-9: "Nothing happens here."
  for (int i = 1; i <= 9; ++i) {
    OutputCapture cap;
    bool res = GGlobals::sailorFcn();
    assert(res);
    assert(g.hs == i);
    assert(cap.getOutput().find("Nothing happens here.") != std::string::npos);
  }

  // Turn 10: "I think that phrase is getting a bit worn out."
  {
    OutputCapture cap;
    bool res = GGlobals::sailorFcn();
    assert(res);
    assert(g.hs == 10);
    assert(cap.getOutput().find("I think that phrase is getting a bit worn out.") != std::string::npos);
  }

  // Turns 11-19: "Nothing happens here."
  for (int i = 11; i <= 19; ++i) {
    OutputCapture cap;
    bool res = GGlobals::sailorFcn();
    assert(res);
    assert(g.hs == i);
    assert(cap.getOutput().find("Nothing happens here.") != std::string::npos);
  }

  // Turn 20: "You seem to be repeating yourself."
  {
    OutputCapture cap;
    bool res = GGlobals::sailorFcn();
    assert(res);
    assert(g.hs == 20);
    assert(cap.getOutput().find("You seem to be repeating yourself.") != std::string::npos);
  }
}

TEST(SailorFcn_Unhandled) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_TAKE;
  assert(!GGlobals::sailorFcn());
}

// ============================================================================
// Routine Tests: GROUND-FUNCTION (gglobals.zil:170-183)
// ============================================================================

TEST(GroundFunction_PutRedirectsToDrop) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *ground = g.getObject(ObjectIds::GROUND);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  assert(ground != nullptr);
  assert(sword != nullptr);

  // Player holds sword
  sword->moveTo(g.player);
  assert(sword->getLocation() == g.player);

  g.prsa = V_PUT;
  g.prso = sword;
  g.prsi = ground;

  OutputCapture cap;
  bool res = GGlobals::groundFunction();
  assert(res);
  assert(sword->getLocation() == g.here);
}

TEST(GroundFunction_Dig) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_DIG;
  OutputCapture cap;
  bool res = GGlobals::groundFunction();
  assert(res);
  assert(cap.getOutput().find("The ground is too hard for digging here.") != std::string::npos);
}

TEST(GroundFunction_Unhandled) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_LOOK;
  assert(!GGlobals::groundFunction());
}

// ============================================================================
// Routine Tests: GRUE-FUNCTION (gglobals.zil:191-206)
// ============================================================================

TEST(GrueFunction_Examine) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_EXAMINE;
  OutputCapture cap;
  bool res = GGlobals::grueFunction();
  assert(res);
  assert(cap.getOutput().find("The grue is a sinister, lurking presence") != std::string::npos);
  assert(cap.getOutput().find("few have survived its fearsome jaws") != std::string::npos);
}

TEST(GrueFunction_Find) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_FIND;
  OutputCapture cap;
  bool res = GGlobals::grueFunction();
  assert(res);
  assert(cap.getOutput().find("There is no grue here, but I'm sure there is at least one lurking") != std::string::npos);
}

TEST(GrueFunction_Listen) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_LISTEN;
  OutputCapture cap;
  bool res = GGlobals::grueFunction();
  assert(res);
  assert(cap.getOutput().find("It makes no sound but is always lurking in the darkness nearby.") != std::string::npos);
}

TEST(GrueFunction_Unhandled) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_TAKE;
  assert(!GGlobals::grueFunction());
}

// ============================================================================
// Routine Tests: CRETIN-FCN (gglobals.zil:221-265)
// ============================================================================

TEST(CretinFcn_Tell) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_TELL;
  g.pCont = true;
  g.quoteFlag = true;

  OutputCapture cap;
  bool res = GGlobals::cretinFcn();
  assert(res);
  assert(cap.getOutput().find("Talking to yourself is said to be a sign of impending mental collapse.") != std::string::npos);
  assert(!g.pCont);
  assert(!g.quoteFlag);
}

TEST(CretinFcn_GiveRedirectsToTake) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *meObj = g.getObject(ObjectIds::ME);
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  assert(meObj != nullptr);
  assert(sword != nullptr);

  // Sword is in room
  sword->moveTo(g.here);

  g.prsa = V_GIVE;
  g.prso = sword;
  g.prsi = meObj;

  OutputCapture cap;
  bool res = GGlobals::cretinFcn();
  assert(res);
  assert(sword->getLocation() == g.player);
}

TEST(CretinFcn_MakeDisembarkEat) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  // MAKE
  g.prsa = V_MAKE;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Only you can do that.") != std::string::npos);
  }

  // DISEMBARK
  g.prsa = V_DISEMBARK;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("You'll have to do that on your own.") != std::string::npos);
  }

  // EAT
  g.prsa = V_EAT;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Auto-cannibalism is not the answer.") != std::string::npos);
  }
}

TEST(CretinFcn_AttackSuicideAndWeapon) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  // Without weapon: "Suicide is not the answer."
  g.prsa = V_ATTACK;
  g.prsi = nullptr;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Suicide is not the answer.") != std::string::npos);
  }

  // With weapon: "If you insist.... Poof, you're dead!"
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  sword->setFlag(ObjectFlag::WEAPONBIT);
  g.prsi = sword;
  {
    InputRedirect in("no\n");
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Poof, you're dead!") != std::string::npos);
  }
}

TEST(CretinFcn_ThrowMeAndTake) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  ZObject *meObj = g.getObject(ObjectIds::ME);
  g.prsa = V_THROW;
  g.prso = meObj;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Why don't you just walk like normal people?") != std::string::npos);
  }

  // TAKE
  g.prsa = V_TAKE;
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("How romantic!") != std::string::npos);
  }
}

TEST(CretinFcn_ExamineMirrorVsNoMirror) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_EXAMINE;

  // Outside mirror room: "That's difficult unless your eyes are prehensile."
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("That's difficult unless your eyes are prehensile.") != std::string::npos);
  }

  // In mirror room: "Your image in the mirror looks tired."
  g.here = g.getObject(RoomIds::MIRROR_ROOM_1);
  ZObject *m1 = g.getObject(ObjectIds::MIRROR_1);
  if (m1) m1->moveTo(g.here);
  {
    OutputCapture cap;
    assert(GGlobals::cretinFcn());
    assert(cap.getOutput().find("Your image in the mirror looks tired.") != std::string::npos);
  }
}

TEST(CretinFcn_Unhandled) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  g.prsa = V_READ;
  assert(!GGlobals::cretinFcn());
}

// ============================================================================
// Routine Tests: PATH-OBJECT (gglobals.zil:282-288)
// ============================================================================

TEST(PathObject_Verbs) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  // TAKE / FOLLOW
  g.prsa = V_TAKE;
  {
    OutputCapture cap;
    assert(GGlobals::pathObject());
    assert(cap.getOutput().find("You must specify a direction to go.") != std::string::npos);
  }
  g.prsa = V_FOLLOW;
  {
    OutputCapture cap;
    assert(GGlobals::pathObject());
    assert(cap.getOutput().find("You must specify a direction to go.") != std::string::npos);
  }

  // FIND
  g.prsa = V_FIND;
  {
    OutputCapture cap;
    assert(GGlobals::pathObject());
    assert(cap.getOutput().find("I can't help you there....") != std::string::npos);
  }

  // DIG
  g.prsa = V_DIG;
  {
    OutputCapture cap;
    assert(GGlobals::pathObject());
    assert(cap.getOutput().find("Not a chance.") != std::string::npos);
  }

  // Unhandled
  g.prsa = V_BURN;
  assert(!GGlobals::pathObject());
}

// ============================================================================
// Routine Tests: ZORKMID-FUNCTION (gglobals.zil:296-302)
// ============================================================================

TEST(ZorkmidFunction_Verbs) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  // EXAMINE
  g.prsa = V_EXAMINE;
  {
    OutputCapture cap;
    assert(GGlobals::zorkmidFunction());
    assert(cap.getOutput().find("The zorkmid is the unit of currency of the Great Underground Empire.") != std::string::npos);
  }

  // FIND
  g.prsa = V_FIND;
  {
    OutputCapture cap;
    assert(GGlobals::zorkmidFunction());
    assert(cap.getOutput().find("The best way to find zorkmids is to go out and look for them.") != std::string::npos);
  }

  // Unhandled
  g.prsa = V_TAKE;
  assert(!GGlobals::zorkmidFunction());
}

// ============================================================================
// Object Initialization Tests: All 18 Objects (gglobals.zil:7-309)
// ============================================================================

TEST(All18ObjectsInitialization) {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();

  // 1. GLOBAL-OBJECTS
  ZObject *glob = g.getObject(ObjectIds::GLOBAL_OBJECTS);
  assert(glob != nullptr);
  assert(glob->hasFlag(ObjectFlag::RMUNGBIT));
  assert(glob->hasFlag(ObjectFlag::INVISIBLE));
  assert(glob->hasFlag(ObjectFlag::TOUCHBIT));
  assert(glob->hasFlag(ObjectFlag::SURFACEBIT));
  assert(glob->hasFlag(ObjectFlag::TRYTAKEBIT));
  assert(glob->hasFlag(ObjectFlag::OPENBIT));
  assert(glob->hasFlag(ObjectFlag::SEARCHBIT));
  assert(glob->hasFlag(ObjectFlag::TRANSBIT));
  assert(glob->hasFlag(ObjectFlag::ONBIT));
  assert(glob->hasFlag(ObjectFlag::RLANDBIT));
  assert(glob->hasFlag(ObjectFlag::FIGHTBIT));
  assert(glob->hasFlag(ObjectFlag::STAGGERED));
  assert(glob->hasFlag(ObjectFlag::WEARBIT));

  // 2. LOCAL-GLOBALS
  ZObject *lg = g.getObject(ObjectIds::LOCAL_GLOBALS);
  assert(lg != nullptr);
  assert(lg->getLocation() == glob);
  assert(lg->hasSynonym("zzmgck"));

  // 3. ROOMS
  ZObject *rooms = g.getObject(ObjectIds::ROOMS);
  assert(rooms != nullptr);

  // 4. INTNUM
  ZObject *intnum = g.getObject(ObjectIds::INTNUM);
  assert(intnum != nullptr);
  assert(intnum->getLocation() == glob);
  assert(intnum->hasSynonym("intnum"));
  assert(intnum->hasFlag(ObjectFlag::TOOLBIT));

  // 5. PSEUDO-OBJECT
  ZObject *pseudo = g.getObject(ObjectIds::PSEUDO_OBJECT);
  assert(pseudo != nullptr);
  assert(pseudo->getLocation() == lg);

  // 6. IT
  ZObject *itObj = g.getObject(ObjectIds::IT);
  assert(itObj != nullptr);
  assert(itObj->getLocation() == glob);
  assert(itObj->hasSynonym("it"));
  assert(itObj->hasSynonym("them"));
  assert(itObj->hasFlag(ObjectFlag::NDESCBIT));
  assert(itObj->hasFlag(ObjectFlag::TOUCHBIT));

  // 7. NOT-HERE-OBJECT
  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  assert(notHere != nullptr);

  // 8. BLESSINGS
  ZObject *blessings = g.getObject(ObjectIds::BLESSINGS);
  assert(blessings != nullptr);
  assert(blessings->getLocation() == glob);
  assert(blessings->hasSynonym("blessings"));
  assert(blessings->hasSynonym("graces"));
  assert(blessings->hasFlag(ObjectFlag::NDESCBIT));

  // 9. STAIRS
  ZObject *stairs = g.getObject(ObjectIds::STAIRS);
  assert(stairs != nullptr);
  assert(stairs->getLocation() == lg);
  assert(stairs->hasSynonym("stairs"));
  assert(stairs->hasSynonym("steps"));
  assert(stairs->hasSynonym("staircase"));
  assert(stairs->hasSynonym("stairway"));
  assert(stairs->hasAdjective("stone"));
  assert(stairs->hasAdjective("dark"));
  assert(stairs->hasAdjective("marble"));
  assert(stairs->hasAdjective("forbidding"));
  assert(stairs->hasAdjective("steep"));
  assert(stairs->hasFlag(ObjectFlag::NDESCBIT));
  assert(stairs->hasFlag(ObjectFlag::CLIMBBIT));

  // 10. SAILOR
  ZObject *sailor = g.getObject(ObjectIds::SAILOR);
  assert(sailor != nullptr);
  assert(sailor->getLocation() == glob);
  assert(sailor->hasSynonym("sailor"));
  assert(sailor->hasSynonym("footpad"));
  assert(sailor->hasSynonym("aviator"));
  assert(sailor->hasFlag(ObjectFlag::NDESCBIT));

  // 11. GROUND
  ZObject *ground = g.getObject(ObjectIds::GROUND);
  assert(ground != nullptr);
  assert(ground->getLocation() == glob);
  assert(ground->hasSynonym("ground"));
  assert(ground->hasSynonym("floor"));
  assert(ground->hasSynonym("dirt"));
  assert(ground->hasSynonym("sand"));
  assert(ground->hasFlag(ObjectFlag::NDESCBIT));
  assert(ground->hasFlag(ObjectFlag::INVISIBLE));

  // 12. GRUE
  ZObject *grue = g.getObject(ObjectIds::GRUE);
  assert(grue != nullptr);
  assert(grue->getLocation() == nullptr);
  assert(grue->hasSynonym("grue"));
  assert(grue->hasAdjective("lurking"));
  assert(grue->hasAdjective("sinister"));
  assert(grue->hasAdjective("hungry"));
  assert(grue->hasAdjective("silent"));

  // 13. LUNGS
  ZObject *lungs = g.getObject(ObjectIds::LUNGS);
  assert(lungs != nullptr);
  assert(lungs->getLocation() == glob);
  assert(lungs->hasSynonym("lungs"));
  assert(lungs->hasSynonym("air"));
  assert(lungs->hasSynonym("mouth"));
  assert(lungs->hasSynonym("breath"));
  assert(lungs->hasFlag(ObjectFlag::NDESCBIT));

  // 14. ME
  ZObject *meObj = g.getObject(ObjectIds::ME);
  assert(meObj != nullptr);
  assert(meObj->getLocation() == glob);
  assert(meObj->hasSynonym("me"));
  assert(meObj->hasSynonym("myself"));
  assert(meObj->hasSynonym("self"));
  assert(meObj->hasSynonym("cretin"));
  assert(meObj->hasFlag(ObjectFlag::ACTORBIT));

  // 15. ADVENTURER
  ZObject *adv = g.getObject(ObjectIds::ADVENTURER);
  assert(adv != nullptr);
  assert(adv->hasSynonym("adventurer"));
  assert(adv->hasFlag(ObjectFlag::NDESCBIT));
  assert(adv->hasFlag(ObjectFlag::INVISIBLE));
  assert(adv->hasFlag(ObjectFlag::SACREDBIT));
  assert(adv->hasFlag(ObjectFlag::ACTORBIT));

  // 16. PATHOBJ
  ZObject *path = g.getObject(ObjectIds::PATHOBJ);
  assert(path != nullptr);
  assert(path->getLocation() == glob);
  assert(path->hasSynonym("trail"));
  assert(path->hasSynonym("path"));
  assert(path->hasAdjective("forest"));
  assert(path->hasAdjective("narrow"));
  assert(path->hasAdjective("long"));
  assert(path->hasAdjective("winding"));
  assert(path->hasFlag(ObjectFlag::NDESCBIT));

  // 17. ZORKMID
  ZObject *zorkmid = g.getObject(ObjectIds::ZORKMID);
  assert(zorkmid != nullptr);
  assert(zorkmid->getLocation() == glob);
  assert(zorkmid->hasSynonym("zorkmid"));

  // 18. HANDS
  ZObject *hands = g.getObject(ObjectIds::HANDS);
  assert(hands != nullptr);
  assert(hands->getLocation() == glob);
  assert(hands->hasSynonym("pair"));
  assert(hands->hasSynonym("hands"));
  assert(hands->hasSynonym("hand"));
  assert(hands->hasAdjective("bare"));
  assert(hands->hasFlag(ObjectFlag::NDESCBIT));
  assert(hands->hasFlag(ObjectFlag::TOOLBIT));
}

// ============================================================================
// Global Variables & Reset Tests (gglobals.zil:92-94, 137)
// ============================================================================

TEST(GlobalVariables_DefaultsAndReset) {
  auto &g = Globals::instance();
  g.reset();

  assert(g.loadMax == 100);
  assert(g.loadAllowed == 100);
  assert(g.hs == 0);

  g.loadMax = 150;
  g.loadAllowed = 80;
  g.hs = 42;
  g.pXadjn = "adj";
  g.pXnam = "nam";
  g.pNc1 = {"token1"};
  g.pNc2 = {"token2"};

  g.reset();
  assert(g.loadMax == 100);
  assert(g.loadAllowed == 100);
  assert(g.hs == 0);
  assert(g.pXadjn.empty());
  assert(g.pXnam.empty());
  assert(g.pNc1.empty());
  assert(g.pNc2.empty());
}

int main() {
  std::println("Running GGlobalsTests (gglobals.zil)...");
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  std::println("\nTotal tests: {}, Failed: {}", results.size(), failed);
  return failed == 0 ? 0 : 1;
}
