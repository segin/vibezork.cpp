/**
 * @file villains.cpp
 * @brief The villain ACTION routines of zil/1actions.zil, ported verbatim.
 *
 * Source: zil/1actions.zil:622-764 (WEAPON-FUNCTION, AXE-F,
 *         STILETTO-FUNCTION, TROLL-FCN).
 */

#include "villains.h"

#include "core/globals.h"
#include "core/gmacros.h"
#include "core/io.h"
#include "core/object.h"
#include "systems/melee.h"
#include "systems/melee_tables.h"
#include "verbs/verbs.h"
#include "world/objects.h"

namespace Villains {

namespace {

ZObject *obj(ObjectId id) { return Globals::instance().getObject(id); }

/// ZIL LDESC strings the troll switches between as he is armed, disarmed,
/// knocked out or killed. Source: zil/1actions.zil:648-697
constexpr const char *kTrollArmed =
    "A nasty-looking troll, brandishing a bloody axe, blocks all passages out "
    "of the room.";
constexpr const char *kTrollBabbling = "A pathetically babbling troll is here.";
constexpr const char *kTrollUnconscious =
    "An unconscious troll is sprawled on the floor. All passages out of the "
    "room are open.";
constexpr const char *kTrollPlain = "A troll is here.";

} // namespace

// ZIL: <ROUTINE WEAPON-FUNCTION (W V) ...>
// Source: zil/1actions.zil:629-638
bool weaponFunction(ZObject *weapon, ZObject *villain) {
  auto &g = Globals::instance();
  if (!villain || villain->getLocation() != g.here) return false;
  if (g.prsa != V_TAKE) return false;

  if (weapon && weapon->getLocation() == villain) {
    tell("The ", villain, " swings it out of your reach.", CR);
  } else {
    tell("The ", weapon, " seems white-hot. You can't hold on to it.", CR);
  }
  return true;
}

// ZIL: <ROUTINE AXE-F () <COND (,TROLL-FLAG <>) (T <WEAPON-FUNCTION ,AXE ,TROLL>)>>
// Source: zil/1actions.zil:622-624
bool axeF() {
  auto &g = Globals::instance();
  if (g.trollFlag) return false;
  return weaponFunction(obj(ObjectIds::AXE), obj(ObjectIds::TROLL));
}

// ZIL: <ROUTINE STILETTO-FUNCTION () <WEAPON-FUNCTION ,STILETTO ,THIEF>>
// Source: zil/1actions.zil:626-627
bool stilettoFunction() {
  return weaponFunction(obj(ObjectIds::STILETTO), obj(ObjectIds::THIEF));
}

// ZIL: <ROUTINE TROLL-FCN ("OPTIONAL" (MODE <>)) ...>
// Source: zil/1actions.zil:640-764
int trollFcn(int mode) {
  auto &g = Globals::instance();
  ZObject *troll = obj(ObjectIds::TROLL);
  ZObject *axe = obj(ObjectIds::AXE);
  if (!troll || !axe) return M_NOT_HANDLED;

  const bool axeOnTroll = axe->getLocation() == troll;
  const bool trollHere = troll->getLocation() == g.here;

  if (g.prsa == V_TELL) {
    g.pCont = 0;
    tell("The troll isn't much of a conversationalist.", CR);
    return M_HANDLED;
  }

  if (mode == Melee::F_BUSY) {
    // ZIL: with his axe in hand he is not busy at all.
    if (axeOnTroll) return M_NOT_HANDLED;
    // ZIL: <PROB 75 90>. The second argument is discarded by the macro, so
    // this is 75 percent, or 75 against RANDOM 300 when unlucky.
    if (axe->getLocation() == g.here && GMacros::prob(75, true)) {
      axe->setFlag(ObjectFlag::NDESCBIT);
      axe->clearFlag(ObjectFlag::WEAPONBIT);
      axe->moveTo(troll);
      troll->setLongDesc(kTrollArmed);
      if (trollHere) {
        tell("The troll, angered and humiliated, recovers his weapon. He "
             "appears to have an axe to grind with you.",
             CR);
      }
      return M_HANDLED;
    }
    if (trollHere) {
      troll->setLongDesc(kTrollBabbling);
      tell("The troll, disarmed, cowers in terror, pleading for his life in "
           "the guttural tongue of the trolls.",
           CR);
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }

  if (mode == Melee::F_DEAD) {
    if (axeOnTroll) {
      axe->moveTo(g.here);
      axe->clearFlag(ObjectFlag::NDESCBIT);
      axe->setFlag(ObjectFlag::WEAPONBIT);
    }
    g.trollFlag = true;
    return M_HANDLED;
  }

  if (mode == Melee::F_UNCONSCIOUS) {
    troll->clearFlag(ObjectFlag::FIGHTBIT);
    if (axeOnTroll) {
      axe->moveTo(g.here);
      axe->clearFlag(ObjectFlag::NDESCBIT);
      axe->setFlag(ObjectFlag::WEAPONBIT);
    }
    troll->setLongDesc(kTrollUnconscious);
    g.trollFlag = true;
    return M_HANDLED;
  }

  if (mode == Melee::F_CONSCIOUS) {
    if (trollHere) {
      troll->setFlag(ObjectFlag::FIGHTBIT);
      tell("The troll stirs, quickly resuming a fighting stance.", CR);
    }
    if (axeOnTroll) {
      troll->setLongDesc(kTrollArmed);
    } else if (axe->getLocation() == obj(RoomIds::TROLL_ROOM)) {
      axe->setFlag(ObjectFlag::NDESCBIT);
      axe->clearFlag(ObjectFlag::WEAPONBIT);
      axe->moveTo(troll);
      troll->setLongDesc(kTrollArmed);
    } else {
      troll->setLongDesc(kTrollPlain);
    }
    g.trollFlag = false;
    return M_HANDLED;
  }

  if (mode == Melee::F_FIRST) {
    // ZIL: one turn in three the troll gets the first blow.
    if (GMacros::prob(33)) {
      troll->setFlag(ObjectFlag::FIGHTBIT);
      g.pCont = 0;
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }

  // ZIL: <NOT .MODE> - ordinary verb dispatch
  if (mode != 0) return M_NOT_HANDLED;

  if (g.prsa == V_EXAMINE) {
    // ZIL prints the LDESC, which changes as the fight goes on.
    tell(troll->getLongDesc(), CR);
    return M_HANDLED;
  }

  const bool throwOrGive = (g.prsa == V_THROW || g.prsa == V_GIVE);
  const bool atTroll = g.prso && g.prsi == troll;
  if ((throwOrGive && atTroll) || g.prsa == V_TAKE || g.prsa == V_MOVE ||
      g.prsa == V_MUNG) {
    Melee::awaken(troll);

    if (throwOrGive) {
      if (g.prso == axe && axe->getLocation() == g.winner) {
        tell("The troll scratches his head in confusion, then takes the axe.",
             CR);
        troll->setFlag(ObjectFlag::FIGHTBIT);
        axe->moveTo(troll);
        return M_HANDLED;
      }
      if (g.prso == troll || g.prso == axe) {
        tell("You would have to get the ", g.prso,
             " first, and that seems unlikely.", CR);
        return M_HANDLED;
      }
      if (g.prsa == V_THROW) {
        tell("The troll, who is remarkably coordinated, catches the ", g.prso);
      } else {
        tell("The troll, who is not overly proud, graciously accepts the gift");
      }

      const bool bladed = g.prso == obj(ObjectIds::KNIFE) ||
                          g.prso == obj(ObjectIds::SWORD) || g.prso == axe;
      if (GMacros::prob(20) && bladed) {
        Verbs::removeCarefully(g.prso);
        tell(" and eats it hungrily. Poor troll, he dies from an internal "
             "hemorrhage and his carcass disappears in a sinister black fog.",
             CR);
        Verbs::removeCarefully(troll);
        troll->performAction(Melee::F_DEAD);
        g.trollFlag = true;
      } else if (bladed) {
        g.prso->moveTo(g.here);
        tell(" and, being for the moment sated, throws it back. Fortunately, "
             "the troll has poor control, and the ",
             g.prso, " falls to the floor. He does not look pleased.", CR);
        troll->setFlag(ObjectFlag::FIGHTBIT);
      } else {
        tell(" and not having the most discriminating tastes, gleefully eats "
             "it.",
             CR);
        Verbs::removeCarefully(g.prso);
      }
      return M_HANDLED;
    }

    if (g.prsa == V_TAKE || g.prsa == V_MOVE) {
      tell("The troll spits in your face, grunting \"Better luck next time\" "
           "in a rather barbarous accent.",
           CR);
      return M_HANDLED;
    }
    if (g.prsa == V_MUNG) {
      tell("The troll laughs at your puny gesture.", CR);
      return M_HANDLED;
    }
  }

  if (g.prsa == V_LISTEN) {
    tell("Every so often the troll says something, probably uncomplimentary, "
         "in his guttural tongue.",
         CR);
    return M_HANDLED;
  }
  if (g.trollFlag && g.prsa == V_HELLO) {
    tell("Unfortunately, the troll can't hear you.", CR);
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

} // namespace Villains
