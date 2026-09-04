/**
 * @file gglobals.cpp
 * @brief Canonical global routines, variables, and objects ported from GGLOBALS.ZIL
 *
 * Source: zil/gglobals.zil:1-309
 */

#include "core/gglobals.h"
#include "core/flags.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "parser/gparser.h"
#include "systems/death.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <format>
#include <memory>

bool sandAction();

namespace GGlobals {

// ZIL: <ROUTINE NOT-HERE-OBJECT-F ("AUX" TBL (PRSO? T) OBJ) ...> (gglobals.zil:52-74)
bool notHereObjectF() {
  auto &g = Globals::instance();
  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);

  if (g.prso == notHere && g.prsi == notHere) {
    printLine("Those things aren't here!");
    return true;
  }

  bool prsoTarget = (g.prso == notHere);

  g.pCont = false;
  g.quoteFlag = false;

  if (g.winner == g.player || !g.winner) {
    print("You can't see any ");
    notHerePrint(prsoTarget);
    printLine(" here!");
  } else {
    print(std::format("The {} seems confused. \"I don't see any ", g.winner->getDesc()));
    notHerePrint(prsoTarget);
    printLine(" here!\"");
  }
  return true;
}

// ZIL: <ROUTINE NOT-HERE-PRINT (PRSO?) ...> (gglobals.zil:76-84)
void notHerePrint(bool prso) {
  auto &g = Globals::instance();
  if (g.pOflag) {
    if (!g.pXadjn.empty()) {
      print(g.pXadjn);
      print(" ");
    }
    if (!g.pXnam.empty()) {
      print(g.pXnam);
    }
  } else if (prso) {
    if (!g.pNc1.empty()) {
      GParser::bufferPrint(g.pNc1, false);
    } else {
      print("such thing");
    }
  } else {
    if (!g.pNc2.empty()) {
      GParser::bufferPrint(g.pNc2, false);
    } else {
      print("such thing");
    }
  }
}

// ZIL: <ROUTINE NULL-F ("OPTIONAL" A1 A2) ...> (gglobals.zil:85-87)
bool nullF(void * /*a1*/, void * /*a2*/) {
  return false;
}

// ZIL: <ROUTINE STAIRS-F () ...> (gglobals.zil:110-113)
bool stairsF() {
  auto &g = Globals::instance();
  if (g.prsa == V_THROUGH) {
    printLine("You should say whether you want to go up or down.");
    return true;
  }
  return false;
}

// ZIL: <ROUTINE SAILOR-FCN () ...> (gglobals.zil:122-162)
bool sailorFcn() {
  auto &g = Globals::instance();
  if (g.prsa == V_TELL) {
    g.pCont = false;
    g.quoteFlag = false;
    printLine("You can't talk to the sailor that way.");
    return true;
  }
  if (g.prsa == V_EXAMINE) {
    printLine("There is no sailor to be seen.");
    return true;
  }
  if (g.prsa == V_HELLO) {
    g.hs += 1;
    if (g.hs % 20 == 0) {
      printLine("You seem to be repeating yourself.");
    } else if (g.hs % 10 == 0) {
      printLine("I think that phrase is getting a bit worn out.");
    } else {
      printLine("Nothing happens here.");
    }
    return true;
  }
  return false;
}

// ZIL: <ROUTINE GROUND-FUNCTION () ...> (gglobals.zil:170-183)
bool groundFunction() {
  auto &g = Globals::instance();
  ZObject *ground = g.getObject(ObjectIds::GROUND);
  if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && g.prsi &&
      (g.prsi == ground || g.prsi->getId() == ObjectIds::GROUND)) {
    g.prsa = V_DROP;
    g.prsi = nullptr;
    return Verbs::vDrop();
  }
  if (g.here && g.here->getId() == RoomIds::SANDY_CAVE) {
    return ::sandAction();
  }
  if (g.prsa == V_DIG) {
    printLine("The ground is too hard for digging here.");
    return true;
  }
  return false;
}

// ZIL: <ROUTINE GRUE-FUNCTION () ...> (gglobals.zil:191-206)
bool grueFunction() {
  auto &g = Globals::instance();
  if (g.prsa == V_EXAMINE) {
    printLine(
        "The grue is a sinister, lurking presence in the dark places of the\n"
        "earth. Its favorite diet is adventurers, but its insatiable\n"
        "appetite is tempered by its fear of light. No grue has ever been\n"
        "seen by the light of day, and few have survived its fearsome jaws\n"
        "to tell the tale.");
    return true;
  }
  if (g.prsa == V_FIND) {
    printLine(
        "There is no grue here, but I'm sure there is at least one lurking\n"
        "in the darkness nearby. I wouldn't let my light go out if I were\n"
        "you!");
    return true;
  }
  if (g.prsa == V_LISTEN) {
    printLine("It makes no sound but is always lurking in the darkness nearby.");
    return true;
  }
  return false;
}

// ZIL: <ROUTINE CRETIN-FCN () ...> (gglobals.zil:221-265)
bool cretinFcn() {
  auto &g = Globals::instance();
  if (g.prsa == V_TELL) {
    g.pCont = false;
    g.quoteFlag = false;
    printLine("Talking to yourself is said to be a sign of impending mental collapse.");
    return true;
  }
  ZObject *meObj = g.getObject(ObjectIds::ME);
  if (g.prsa == V_GIVE && g.prsi &&
      (g.prsi == meObj || g.prsi == g.player ||
       g.prsi->getId() == ObjectIds::ME || g.prsi->getId() == ObjectIds::ADVENTURER)) {
    g.prsa = V_TAKE;
    g.prsi = nullptr;
    return Verbs::vTake();
  }
  if (g.prsa == V_MAKE) {
    printLine("Only you can do that.");
    return true;
  }
  if (g.prsa == V_DISEMBARK) {
    printLine("You'll have to do that on your own.");
    return true;
  }
  if (g.prsa == V_EAT) {
    printLine("Auto-cannibalism is not the answer.");
    return true;
  }
  if (g.prsa == V_ATTACK || g.prsa == V_MUNG || g.prsa == V_KILL) {
    if (g.prsi && g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
      DeathSystem::jigsUp("If you insist.... Poof, you're dead!");
    } else {
      printLine("Suicide is not the answer.");
    }
    return true;
  }
  if (g.prsa == V_THROW) {
    if (g.prso &&
        (g.prso == meObj || g.prso == g.player ||
         g.prso->getId() == ObjectIds::ME || g.prso->getId() == ObjectIds::ADVENTURER)) {
      printLine("Why don't you just walk like normal people?");
      return true;
    }
    return false;
  }
  if (g.prsa == V_TAKE) {
    printLine("How romantic!");
    return true;
  }
  if (g.prsa == V_EXAMINE) {
    ZObject *m1 = g.getObject(ObjectIds::MIRROR_1);
    ZObject *m2 = g.getObject(ObjectIds::MIRROR_2);
    if (g.here && ((m1 && m1->getLocation() == g.here) || (m2 && m2->getLocation() == g.here))) {
      printLine("Your image in the mirror looks tired.");
    } else {
      printLine("That's difficult unless your eyes are prehensile.");
    }
    return true;
  }
  return false;
}

// ZIL: <ROUTINE PATH-OBJECT () ...> (gglobals.zil:282-288)
bool pathObject() {
  auto &g = Globals::instance();
  if (g.prsa == V_TAKE || g.prsa == V_FOLLOW) {
    printLine("You must specify a direction to go.");
    return true;
  }
  if (g.prsa == V_FIND) {
    printLine("I can't help you there....");
    return true;
  }
  if (g.prsa == V_DIG) {
    printLine("Not a chance.");
    return true;
  }
  return false;
}

// ZIL: <ROUTINE ZORKMID-FUNCTION () ...> (gglobals.zil:296-302)
bool zorkmidFunction() {
  auto &g = Globals::instance();
  if (g.prsa == V_EXAMINE) {
    printLine("The zorkmid is the unit of currency of the Great Underground Empire.");
    return true;
  }
  if (g.prsa == V_FIND) {
    printLine("The best way to find zorkmids is to go out and look for them.");
    return true;
  }
  return false;
}

// ============================================================================
// Object Initialization (gglobals.zil:7-309)
// ============================================================================

void initGlobalObjects(Globals &g) {
  auto getOrCreate = [&g](ObjectId id, std::string_view desc) -> ZObject * {
    ZObject *obj = g.getObject(id);
    if (!obj) {
      auto uobj = std::make_unique<ZObject>(id, desc);
      obj = uobj.get();
      g.registerObject(id, std::move(uobj));
    }
    return obj;
  };

  // 1. GLOBAL-OBJECTS (gglobals.zil:7-10)
  auto *glob = getOrCreate(ObjectIds::GLOBAL_OBJECTS, "global objects");
  glob->setFlag(ObjectFlag::RMUNGBIT);
  glob->setFlag(ObjectFlag::INVISIBLE);
  glob->setFlag(ObjectFlag::TOUCHBIT);
  glob->setFlag(ObjectFlag::SURFACEBIT);
  glob->setFlag(ObjectFlag::TRYTAKEBIT);
  glob->setFlag(ObjectFlag::OPENBIT);
  glob->setFlag(ObjectFlag::SEARCHBIT);
  glob->setFlag(ObjectFlag::TRANSBIT);
  glob->setFlag(ObjectFlag::ONBIT);
  glob->setFlag(ObjectFlag::RLANDBIT);
  glob->setFlag(ObjectFlag::FIGHTBIT);
  glob->setFlag(ObjectFlag::STAGGERED);
  glob->setFlag(ObjectFlag::WEARBIT);

  // 2. LOCAL-GLOBALS (gglobals.zil:12-24)
  auto *lg = getOrCreate(ObjectIds::LOCAL_GLOBALS, "local globals");
  lg->moveTo(glob);
  lg->addSynonym("zzmgck");
  lg->setAction(pathObject);

  // 3. ROOMS (gglobals.zil:28-29)
  getOrCreate(ObjectIds::ROOMS, "rooms");

  // 4. INTNUM (gglobals.zil:31-35)
  auto *intnum = getOrCreate(ObjectIds::INTNUM, "number");
  intnum->moveTo(glob);
  intnum->addSynonym("intnum");
  intnum->addSynonym("number");
  intnum->setFlag(ObjectFlag::TOOLBIT);

  // 5. PSEUDO-OBJECT (gglobals.zil:37-40)
  auto *pseudo = getOrCreate(ObjectIds::PSEUDO_OBJECT, "pseudo");
  pseudo->moveTo(lg);
  pseudo->setAction(cretinFcn);

  // 6. IT (gglobals.zil:42-46)
  auto *itObj = getOrCreate(ObjectIds::IT, "random object");
  itObj->moveTo(glob);
  itObj->addSynonym("it");
  itObj->addSynonym("them");
  itObj->addSynonym("her");
  itObj->addSynonym("him");
  itObj->setFlag(ObjectFlag::NDESCBIT);
  itObj->setFlag(ObjectFlag::TOUCHBIT);

  // 7. NOT-HERE-OBJECT (gglobals.zil:48-50)
  auto *notHere = getOrCreate(ObjectIds::NOT_HERE_OBJECT, "such thing");
  notHere->setAction(notHereObjectF);

  // 8. BLESSINGS (gglobals.zil:96-100)
  auto *blessings = getOrCreate(ObjectIds::BLESSINGS, "blessings");
  blessings->moveTo(glob);
  blessings->addSynonym("blessings");
  blessings->addSynonym("graces");
  blessings->setFlag(ObjectFlag::NDESCBIT);

  // 9. STAIRS (gglobals.zil:102-108)
  auto *stairs = getOrCreate(ObjectIds::STAIRS, "stairs");
  stairs->moveTo(lg);
  stairs->addSynonym("stairs");
  stairs->addSynonym("steps");
  stairs->addSynonym("staircase");
  stairs->addSynonym("stairway");
  stairs->addAdjective("stone");
  stairs->addAdjective("dark");
  stairs->addAdjective("marble");
  stairs->addAdjective("forbidding");
  stairs->addAdjective("steep");
  stairs->setFlag(ObjectFlag::NDESCBIT);
  stairs->setFlag(ObjectFlag::CLIMBBIT);
  stairs->setAction(stairsF);

  // 10. SAILOR (gglobals.zil:115-120)
  auto *sailor = getOrCreate(ObjectIds::SAILOR, "sailor");
  sailor->moveTo(glob);
  sailor->addSynonym("sailor");
  sailor->addSynonym("footpad");
  sailor->addSynonym("aviator");
  sailor->setFlag(ObjectFlag::NDESCBIT);
  sailor->setAction(sailorFcn);

  // 11. GROUND (gglobals.zil:164-168)
  auto *ground = getOrCreate(ObjectIds::GROUND, "ground");
  ground->moveTo(glob);
  ground->addSynonym("ground");
  ground->addSynonym("sand");
  ground->addSynonym("dirt");
  ground->addSynonym("floor");
  ground->setFlag(ObjectFlag::NDESCBIT);
  ground->setFlag(ObjectFlag::INVISIBLE);
  ground->setAction(groundFunction);

  // 12. GRUE (gglobals.zil:184-189)
  auto *grue = getOrCreate(ObjectIds::GRUE, "grue");
  grue->addSynonym("grue");
  grue->addAdjective("lurking");
  grue->addAdjective("sinister");
  grue->addAdjective("hungry");
  grue->addAdjective("silent");
  grue->setFlag(ObjectFlag::INVISIBLE);
  grue->setAction(grueFunction);

  // 13. LUNGS (gglobals.zil:208-212)
  auto *lungs = getOrCreate(ObjectIds::LUNGS, "blast of air");
  lungs->moveTo(glob);
  lungs->addSynonym("lungs");
  lungs->addSynonym("air");
  lungs->addSynonym("mouth");
  lungs->addSynonym("breath");
  lungs->setFlag(ObjectFlag::NDESCBIT);

  // 14. ME (gglobals.zil:214-219)
  auto *me = getOrCreate(ObjectIds::ME, "you");
  me->moveTo(glob);
  me->addSynonym("me");
  me->addSynonym("myself");
  me->addSynonym("self");
  me->addSynonym("cretin");
  me->setFlag(ObjectFlag::ACTORBIT);
  me->setAction(cretinFcn);

  // 15. ADVENTURER (gglobals.zil:267-272)
  auto *adv = getOrCreate(ObjectIds::ADVENTURER, "cretin");
  adv->addSynonym("adventurer");
  adv->setFlag(ObjectFlag::NDESCBIT);
  adv->setFlag(ObjectFlag::INVISIBLE);
  adv->setFlag(ObjectFlag::SACREDBIT);
  adv->setFlag(ObjectFlag::ACTORBIT);
  if (!g.player) g.player = adv;
  if (!g.winner) g.winner = adv;

  // 16. PATHOBJ (gglobals.zil:274-280)
  auto *path = getOrCreate(ObjectIds::PATHOBJ, "passage");
  path->moveTo(glob);
  path->addSynonym("trail");
  path->addSynonym("path");
  path->addAdjective("forest");
  path->addAdjective("narrow");
  path->addAdjective("long");
  path->addAdjective("winding");
  path->setFlag(ObjectFlag::NDESCBIT);
  path->setAction(pathObject);

  // 17. ZORKMID (gglobals.zil:290-294)
  auto *zorkmid = getOrCreate(ObjectIds::ZORKMID, "zorkmid");
  zorkmid->moveTo(glob);
  zorkmid->addSynonym("zorkmid");
  zorkmid->setAction(zorkmidFunction);

  // 18. HANDS (gglobals.zil:304-309)
  auto *hands = getOrCreate(ObjectIds::HANDS, "pair of hands");
  hands->moveTo(glob);
  hands->addSynonym("pair");
  hands->addSynonym("hands");
  hands->addSynonym("hand");
  hands->addAdjective("bare");
  hands->setFlag(ObjectFlag::NDESCBIT);
  hands->setFlag(ObjectFlag::TOOLBIT);
}

} // namespace GGlobals
