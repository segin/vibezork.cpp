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
#include "world/rooms.h"
#include "parser/gparser.h"
#include "systems/death.h"
#include "systems/timer.h"

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

// ===========================================================================
// The thief. Source: zil/1actions.zil:1764-2163, 3890-3990.
// ===========================================================================

namespace Villains {

// ZIL: <GLOBAL ROBBER-C-DESC ...> / <GLOBAL ROBBER-U-DESC ...>
// Source: zil/1actions.zil:2087-2094
const char *const kRobberCDesc =
    "There is a suspicious-looking individual, holding a bag, leaning against "
    "one wall. He is armed with a vicious-looking stiletto.";
const char *const kRobberUDesc =
    "There is a suspicious-looking individual lying unconscious on the ground.";

// ZIL: <ROUTINE RECOVER-STILETTO () ...>
// Source: zil/1actions.zil:3951-3955
void recoverStiletto() {
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *stiletto = obj(ObjectIds::STILETTO);
  if (!thief || !stiletto) return;
  if (stiletto->getLocation() == thief->getLocation()) {
    stiletto->setFlag(ObjectFlag::NDESCBIT);
    stiletto->moveTo(thief);
  }
}

// ZIL: <ROUTINE MOVE-ALL (FROM TO "AUX" X N) ...>
// Source: zil/1actions.zil:2114-2122
void moveAll(ZObject *from, ZObject *to) {
  if (!from || !to) return;
  auto contents = from->getContents();
  for (ZObject *x : contents) {
    x->clearFlag(ObjectFlag::INVISIBLE);
    x->moveTo(to);
  }
}

// ZIL: <ROUTINE ROB (WHAT WHERE "OPTIONAL" (PROB <>) ...) ...>
// Source: zil/1actions.zil:3978-3990
bool rob(ZObject *what, ZObject *where, int probability) {
  if (!what || !where) return false;
  bool robbed = false;
  auto contents = what->getContents();
  for (ZObject *x : contents) {
    if (!x->hasFlag(ObjectFlag::INVISIBLE) && !x->hasFlag(ObjectFlag::SACREDBIT) &&
        x->getProperty(P_TVALUE) > 0 &&
        (probability == 0 || GMacros::prob(probability))) {
      x->moveTo(where);
      x->setFlag(ObjectFlag::TOUCHBIT);
      if (where->getId() == ObjectIds::THIEF) x->setFlag(ObjectFlag::INVISIBLE);
      robbed = true;
    }
  }
  return robbed;
}

// ZIL: <ROUTINE STEAL-JUNK (RM "AUX" X N) ...>
// Source: zil/1actions.zil:3957-3976
bool stealJunk(ZObject *room) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  if (!room || !thief) return false;
  auto contents = room->getContents();
  for (ZObject *x : contents) {
    // ZIL: <PROB 10 T> - the second argument is discarded by the macro.
    if (x->getProperty(P_TVALUE) == 0 && x->hasFlag(ObjectFlag::TAKEBIT) &&
        !x->hasFlag(ObjectFlag::SACREDBIT) && !x->hasFlag(ObjectFlag::INVISIBLE) &&
        (x->getId() == ObjectIds::STILETTO || GMacros::prob(10, true))) {
      x->moveTo(thief);
      x->setFlag(ObjectFlag::TOUCHBIT);
      x->setFlag(ObjectFlag::INVISIBLE);
      if (x->getId() == ObjectIds::ROPE) g.domeFlag = false;
      if (room == g.here) {
        tell("You suddenly notice that the ", x, " vanished.", CR);
        return true;
      }
      return false;
    }
  }
  return false;
}

// ZIL: <ROUTINE DROP-JUNK (RM "AUX" X N (FLG <>)) ...>
// Source: zil/1actions.zil:3932-3949
bool dropJunk(ZObject *room) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  if (!room || !thief) return false;
  bool flg = false;
  auto contents = thief->getContents();
  for (ZObject *x : contents) {
    if (x->getId() == ObjectIds::STILETTO || x->getId() == ObjectIds::LARGE_BAG) {
      continue;
    }
    // ZIL: <PROB 30 T> - second argument discarded.
    if (x->getProperty(P_TVALUE) == 0 && GMacros::prob(30, true)) {
      x->clearFlag(ObjectFlag::INVISIBLE);
      x->moveTo(room);
      if (!flg && room == g.here) {
        tell("The robber, rummaging through his bag, dropped a few items he "
             "found valueless.",
             CR);
        flg = true;
      }
    }
  }
  return flg;
}

// ZIL: <ROUTINE ROB-MAZE (RM "AUX" X N) ...>
// Source: zil/1actions.zil:1917-1933
bool robMaze(ZObject *room) {
  ZObject *thief = obj(ObjectIds::THIEF);
  if (!room || !thief) return false;
  auto contents = room->getContents();
  for (ZObject *x : contents) {
    if (x->hasFlag(ObjectFlag::TAKEBIT) && !x->hasFlag(ObjectFlag::INVISIBLE) &&
        GMacros::prob(40)) {
      tell("You hear, off in the distance, someone saying \"My, I wonder what "
           "this fine ",
           x, " is doing here.\"", CR);
      // ZIL: <PROB 60 80> - second argument discarded.
      if (GMacros::prob(60, true)) {
        x->moveTo(thief);
        x->setFlag(ObjectFlag::TOUCHBIT);
        x->setFlag(ObjectFlag::INVISIBLE);
      }
      return false;
    }
  }
  return false;
}

// ZIL: <ROUTINE DEPOSIT-BOOTY (RM "AUX" X N (FLG <>)) ...>
// Source: zil/1actions.zil:1898-1912
bool depositBooty(ZObject *room) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  if (!room || !thief) return false;
  bool flg = false;
  auto contents = thief->getContents();
  for (ZObject *x : contents) {
    if (x->getId() == ObjectIds::STILETTO || x->getId() == ObjectIds::LARGE_BAG) {
      continue;
    }
    if (x->getProperty(P_TVALUE) > 0) {
      x->moveTo(room);
      flg = true;
      // ZIL: the thief is the only one who can open the egg.
      if (x->getId() == ObjectIds::EGG) {
        g.eggSolve = true;
        x->setFlag(ObjectFlag::OPENBIT);
      }
    }
  }
  return flg;
}

// ZIL: <ROUTINE HACK-TREASURES ("AUX" X) ...>
// Source: zil/1actions.zil:1888-1896
void hackTreasures() {
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *treasureRoom = obj(RoomIds::TREASURE_ROOM);
  recoverStiletto();
  if (thief) thief->setFlag(ObjectFlag::INVISIBLE);
  if (!treasureRoom) return;
  for (ZObject *x : treasureRoom->getContents()) {
    x->clearFlag(ObjectFlag::INVISIBLE);
  }
}

// ZIL: <ROUTINE STOLE-LIGHT? ("AUX" OLD-LIT) ...>
// Source: zil/1actions.zil:1876-1882
bool stoleLight() {
  auto &g = Globals::instance();
  const bool oldLit = g.lit;
  g.lit = GParser::isLit(g.here);
  if (!g.lit && oldLit) {
    tell("The thief seems to have left you in the dark.", CR);
  }
  return true;
}

// ZIL: <ROUTINE THIEF-IN-TREASURE ("AUX" F N) ...>
// Source: zil/1actions.zil:2152-2163
void thiefInTreasure() {
  auto &g = Globals::instance();
  if (!g.here) return;
  auto contents = g.here->getContents();
  if (contents.size() > 1) {
    tell("The thief gestures mysteriously, and the treasures in the room "
         "suddenly vanish.",
         CR, CR);
  }
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *chalice = obj(ObjectIds::CHALICE);
  for (ZObject *f : contents) {
    if (f != chalice && f != thief) f->setFlag(ObjectFlag::INVISIBLE);
  }
}

// ZIL: <ROUTINE THIEF-VS-ADVENTURER (HERE? "AUX" ROBBED? ...) ...>
// Source: zil/1actions.zil:1764-1873
bool thiefVsAdventurer(bool here) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *stiletto = obj(ObjectIds::STILETTO);
  if (!thief) return false;
  const bool dead = DeathSystem::isDead();

  // ZIL: in the treasure room the thief has other business.
  if (!dead && g.here == obj(RoomIds::TREASURE_ROOM)) return false;

  ZObject *robbed = nullptr;

  if (!g.thiefHere) {
    if (!dead && !here && GMacros::prob(30)) {
      if (stiletto && stiletto->getLocation() == thief) {
        thief->clearFlag(ObjectFlag::INVISIBLE);
        tell("Someone carrying a large bag is casually leaning against one of "
             "the walls here. He does not speak, but it is clear from his "
             "aspect that the bag will be taken only over his dead body.",
             CR);
        g.thiefHere = true;
        return true;
      }
    }
    if (here && thief->hasFlag(ObjectFlag::FIGHTBIT) && !Melee::winning(thief)) {
      tell("Your opponent, determining discretion to be the better part of "
           "valor, decides to terminate this little contretemps. With a rueful "
           "nod of his head, he steps backward into the gloom and disappears.",
           CR);
      thief->setFlag(ObjectFlag::INVISIBLE);
      thief->clearFlag(ObjectFlag::FIGHTBIT);
      recoverStiletto();
      return true;
    }
    if (here && thief->hasFlag(ObjectFlag::FIGHTBIT) && GMacros::prob(90)) {
      return false;
    }
    if (here && GMacros::prob(30)) {
      tell("The holder of the large bag just left, looking disgusted. "
           "Fortunately, he took nothing.",
           CR);
      thief->setFlag(ObjectFlag::INVISIBLE);
      recoverStiletto();
      return true;
    }
    if (GMacros::prob(70)) return false;
    if (!dead) {
      if (rob(g.here, thief, 100)) {
        robbed = g.here;
      } else if (rob(g.winner, thief)) {
        robbed = g.player;
      }
      g.thiefHere = true;
      if (robbed && !here) {
        tell("A seedy-looking individual with a large bag just wandered "
             "through the room. On the way through, he quietly abstracted some "
             "valuables from ");
        tell(robbed == g.here ? "the room" : "your possession");
        tell(", mumbling something about \"Doing unto others before...\"", CR);
        stoleLight();
        return false;
      }
      if (here) {
        recoverStiletto();
        if (robbed) {
          tell("The thief just left, still carrying his large bag. You may not "
               "have noticed that he ");
          tell(robbed == g.player ? "robbed you blind first."
                                  : "appropriated the valuables in the room.");
          crlf();
          stoleLight();
        } else {
          tell("The thief, finding nothing of value, left disgusted.", CR);
        }
        thief->setFlag(ObjectFlag::INVISIBLE);
        return true;
      }
      tell("A \"lean and hungry\" gentleman just wandered through, carrying a "
           "large bag. Finding nothing of value, he left disgruntled.",
           CR);
      return true;
    }
    return false;
  }

  // ZIL: already announced and still here.
  if (here && GMacros::prob(30)) {
    if (rob(g.here, thief, 100)) {
      robbed = g.here;
    } else if (rob(g.winner, thief)) {
      robbed = g.player;
    }
    if (robbed) {
      tell("The thief just left, still carrying his large bag. You may not "
           "have noticed that he ");
      tell(robbed == g.player ? "robbed you blind first."
                              : "appropriated the valuables in the room.");
      crlf();
      stoleLight();
    } else {
      tell("The thief, finding nothing of value, left disgusted.", CR);
    }
    thief->setFlag(ObjectFlag::INVISIBLE);
    recoverStiletto();
  }
  return false;
}

// ZIL: <ROUTINE I-THIEF ("AUX" (RM <LOC ,THIEF>) ROBJ HERE? ...) ...>
// Source: zil/1actions.zil:3890-3930
bool iThief() {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *treasureRoom = obj(RoomIds::TREASURE_ROOM);
  if (!thief) return false;

  ZObject *rm = thief->getLocation();
  bool here = false;
  bool flg = false;
  bool once = false;

  // ZIL: <PROG ()> with <AGAIN> at the end of the move branch, so the body
  // runs a second time after the thief has walked to the next room.
  while (true) {
    here = !thief->hasFlag(ObjectFlag::INVISIBLE);
    if (here) rm = thief->getLocation();

    if (rm == treasureRoom && rm != g.here) {
      if (here) {
        hackTreasures();
        here = false;
      }
      depositBooty(treasureRoom); // silent
    } else if (rm == g.here && rm && !rm->hasFlag(ObjectFlag::ONBIT) &&
               obj(ObjectIds::TROLL) &&
               obj(ObjectIds::TROLL)->getLocation() != g.here) {
      if (thiefVsAdventurer(here)) return true;
      if (thief->hasFlag(ObjectFlag::INVISIBLE)) here = false;
    } else {
      // ZIL: leave if the victim left.
      if (rm && thief->getLocation() == rm && !thief->hasFlag(ObjectFlag::INVISIBLE)) {
        thief->setFlag(ObjectFlag::INVISIBLE);
        here = false;
      }
      if (rm && rm->hasFlag(ObjectFlag::TOUCHBIT)) {
        // ZIL: hack the adventurer's belongings.
        rob(rm, thief, 75);
        if (rm->hasFlag(ObjectFlag::MAZEBIT) && g.here &&
            g.here->hasFlag(ObjectFlag::MAZEBIT)) {
          flg = robMaze(rm);
        } else {
          flg = stealJunk(rm);
        }
      }
    }

    once = !once;
    if (once && !here) {
      // ZIL: move to the next room, and hack.
      recoverStiletto();
      // ZIL walks the child chain of the ROOMS object, wrapping at the end.
      ZObject *rooms = obj(ObjectIds::ROOMS);
      const auto &roomList = rooms ? rooms->getContents()
                                   : std::vector<ZObject *>{};
      std::size_t idx = 0;
      if (rm) {
        for (std::size_t i = 0; i < roomList.size(); ++i) {
          if (roomList[i] == rm) { idx = i + 1; break; }
        }
      }
      bool moved = false;
      for (std::size_t step = 0; step < roomList.size(); ++step) {
        rm = roomList[(idx + step) % roomList.size()];
        if (!rm) break;
        if (!rm->hasFlag(ObjectFlag::SACREDBIT) &&
            rm->hasFlag(ObjectFlag::RLANDBIT)) {
          thief->moveTo(rm);
          thief->clearFlag(ObjectFlag::FIGHTBIT);
          thief->setFlag(ObjectFlag::INVISIBLE);
          g.thiefHere = false;
          moved = true;
          break;
        }
      }
      if (!moved) break;
      continue; // ZIL <AGAIN>
    }
    break;
  }

  if (rm != treasureRoom) dropJunk(rm);
  return flg;
}

// ZIL: <ROUTINE ROBBER-FUNCTION ("OPTIONAL" (MODE <>) ...) ...>
// Source: zil/1actions.zil:1947-2085
int robberFunction(int mode) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *stiletto = obj(ObjectIds::STILETTO);
  ZObject *bag = obj(ObjectIds::LARGE_BAG);
  if (!thief) return M_NOT_HANDLED;

  if (g.prsa == V_TELL) {
    tell("The thief is a strong, silent type.", CR);
    g.pCont = 0;
    return M_HANDLED;
  }

  if (mode == 0) {
    const bool unconscious = thief->getLongDesc() == kRobberUDesc;

    if (g.prsa == V_HELLO && unconscious) {
      tell("The thief, being temporarily incapacitated, is unable to "
           "acknowledge your greeting with his usual graciousness.",
           CR);
      return M_HANDLED;
    }
    if (g.prso == obj(ObjectIds::KNIFE) && g.prsa == V_THROW &&
        !thief->hasFlag(ObjectFlag::FIGHTBIT)) {
      g.prso->moveTo(g.here);
      // ZIL: <PROB 10 0> - second argument discarded.
      if (GMacros::prob(10, true)) {
        tell("You evidently frightened the robber, though you didn't hit him. "
             "He flees");
        if (bag) bag->moveTo(nullptr);
        bool hadStiletto = false;
        if (stiletto && stiletto->getLocation() == thief) {
          stiletto->moveTo(nullptr);
          hadStiletto = true;
        }
        if (!thief->getContents().empty()) {
          moveAll(thief, g.here);
          tell(", but the contents of his bag fall on the floor.");
        } else {
          tell(".");
        }
        if (bag) bag->moveTo(thief);
        if (hadStiletto && stiletto) stiletto->moveTo(thief);
        crlf();
        thief->setFlag(ObjectFlag::INVISIBLE);
      } else {
        tell("You missed. The thief makes no attempt to take the knife, though "
             "it would be a fine addition to the collection in his bag. He "
             "does seem angered by your attempt.",
             CR);
        thief->setFlag(ObjectFlag::FIGHTBIT);
      }
      return M_HANDLED;
    }
    if ((g.prsa == V_THROW || g.prsa == V_GIVE) && g.prso && g.prso != thief &&
        g.prsi == thief) {
      if (thief->getProperty(P_STRENGTH) < 0) {
        thief->setProperty(P_STRENGTH, -thief->getProperty(P_STRENGTH));
        TimerSystem::enable("I-THIEF");
        recoverStiletto();
        thief->setLongDesc(kRobberCDesc);
        tell("Your proposed victim suddenly recovers consciousness.", CR);
      }
      g.prso->moveTo(thief);
      if (g.prso->getProperty(P_TVALUE) > 0) {
        g.thiefEngrossed = true;
        tell("The thief is taken aback by your unexpected generosity, but "
             "accepts the ",
             g.prso, " and stops to admire its beauty.", CR);
      } else {
        tell("The thief places the ", g.prso, " in his bag and thanks you "
             "politely.",
             CR);
      }
      return M_HANDLED;
    }
    if (g.prsa == V_TAKE) {
      tell("Once you got him, what would you do with him?", CR);
      return M_HANDLED;
    }
    if (g.prsa == V_EXAMINE || g.prsa == V_LOOK_INSIDE) {
      tell("The thief is a slippery character with beady eyes that flit back "
           "and forth. He carries, along with an unmistakable arrogance, a "
           "large bag over his shoulder and a vicious stiletto, whose blade is "
           "aimed menacingly in your direction. I'd watch out if I were you.",
           CR);
      return M_HANDLED;
    }
    if (g.prsa == V_LISTEN) {
      tell("The thief says nothing, as you have not been formally introduced.",
           CR);
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }

  if (mode == Melee::F_BUSY) {
    if (stiletto && stiletto->getLocation() == thief) return M_NOT_HANDLED;
    if (stiletto && stiletto->getLocation() == thief->getLocation()) {
      stiletto->moveTo(thief);
      stiletto->setFlag(ObjectFlag::NDESCBIT);
      if (thief->getLocation() == g.here) {
        tell("The robber, somewhat surprised at this turn of events, nimbly "
             "retrieves his stiletto.",
             CR);
      }
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }

  if (mode == Melee::F_DEAD) {
    if (stiletto) {
      stiletto->moveTo(g.here);
      stiletto->clearFlag(ObjectFlag::NDESCBIT);
    }
    const bool booty = depositBooty(g.here);
    if (g.here == obj(RoomIds::TREASURE_ROOM)) {
      bool flg = false;
      ZObject *chalice = obj(ObjectIds::CHALICE);
      auto contents = g.here->getContents();
      for (ZObject *x : contents) {
        if (x == chalice || x == thief || x == g.player) continue;
        x->clearFlag(ObjectFlag::INVISIBLE);
        if (!flg) {
          flg = true;
          tell("As the thief dies, the power of his magic decreases, and his "
               "treasures reappear:",
               CR);
        }
        tell("  A ", x);
        if (!x->getContents().empty() && Verbs::seeInside(x)) {
          tell(", with ");
          Verbs::printContents(x);
        }
        crlf();
      }
      tell("The chalice is now safe to take.", CR);
    } else if (booty) {
      tell("His booty remains.", CR);
    }
    TimerSystem::disable("I-THIEF");
    return M_HANDLED;
  }

  if (mode == Melee::F_FIRST) {
    if (g.thiefHere && !thief->hasFlag(ObjectFlag::INVISIBLE) &&
        GMacros::prob(20)) {
      thief->setFlag(ObjectFlag::FIGHTBIT);
      g.pCont = 0;
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }

  if (mode == Melee::F_UNCONSCIOUS) {
    TimerSystem::disable("I-THIEF");
    thief->clearFlag(ObjectFlag::FIGHTBIT);
    if (stiletto) {
      stiletto->moveTo(g.here);
      stiletto->clearFlag(ObjectFlag::NDESCBIT);
    }
    thief->setLongDesc(kRobberUDesc);
    return M_HANDLED;
  }

  if (mode == Melee::F_CONSCIOUS) {
    if (thief->getLocation() == g.here) {
      thief->setFlag(ObjectFlag::FIGHTBIT);
      tell("The robber revives, briefly feigning continued unconsciousness, "
           "and, when he sees his moment, scrambles away from you.",
           CR);
    }
    TimerSystem::enable("I-THIEF");
    thief->setLongDesc(kRobberCDesc);
    recoverStiletto();
    return M_HANDLED;
  }

  return M_NOT_HANDLED;
}

// ZIL: <ROUTINE LARGE-BAG-F () ...>
// Source: zil/1actions.zil:2088-2112
bool largeBagF() {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  if (g.prsa == V_TAKE) {
    if (thief && thief->getLongDesc() == kRobberUDesc) {
      tell("Sadly for you, the robber collapsed on top of the bag. Trying to "
           "take it would wake him.",
           CR);
    } else {
      tell("The bag will be taken over his dead body.", CR);
    }
    return true;
  }
  if (g.prsa == V_PUT && g.prsi == obj(ObjectIds::LARGE_BAG)) {
    tell("It would be a good trick.", CR);
    return true;
  }
  if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
    tell("Getting close enough would be a good trick.", CR);
    return true;
  }
  if (g.prsa == V_EXAMINE || g.prsa == V_LOOK_INSIDE) {
    tell("The bag is underneath the thief, so one can't say what, if anything, "
         "is inside.",
         CR);
    return true;
  }
  return false;
}

// ZIL: <ROUTINE CHALICE-FCN () ...>
// Source: zil/1actions.zil:2124-2136
bool chaliceFcn() {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  ZObject *treasureRoom = obj(RoomIds::TREASURE_ROOM);
  if (g.prsa == V_TAKE) {
    if (g.prso && g.prso->getLocation() == treasureRoom && thief &&
        thief->getLocation() == treasureRoom &&
        thief->hasFlag(ObjectFlag::FIGHTBIT) &&
        !thief->hasFlag(ObjectFlag::INVISIBLE) &&
        thief->getLongDesc() != kRobberUDesc) {
      tell("You'd be stabbed in the back first.", CR);
      return true;
    }
    return false;
  }
  if (g.prsa == V_PUT && g.prsi == obj(ObjectIds::CHALICE)) {
    tell("You can't. It's not a very good chalice, is it?", CR);
    return true;
  }
  // TODO(G10): DUMB-CONTAINER (1actions.zil) is not ported yet, so the
  // chalice falls through to the generic container verbs meanwhile.
  return false;
}

// ZIL: <ROUTINE TREASURE-ROOM-FCN (RARG "AUX" TL) ...>
// Source: zil/1actions.zil:2138-2150
int treasureRoomFcn(int rarg) {
  auto &g = Globals::instance();
  ZObject *thief = obj(ObjectIds::THIEF);
  if (rarg == M_ENTER && thief && TimerSystem::isEnabled("I-THIEF") &&
      !DeathSystem::isDead()) {
    if (thief->getLocation() != g.here) {
      tell("You hear a scream of anguish as you violate the robber's hideaway. "
           "Using passages unknown to you, he rushes to its defense.",
           CR);
      thief->moveTo(g.here);
    }
    thief->setFlag(ObjectFlag::FIGHTBIT);
    thief->clearFlag(ObjectFlag::INVISIBLE);
    thiefInTreasure();
  }
  return M_NOT_HANDLED;
}

} // namespace Villains
