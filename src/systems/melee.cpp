/**
 * @file melee.cpp
 * @brief The melee engine of zil/1actions.zil, ported routine for routine.
 *
 * Source: zil/1actions.zil:3331-3617, 3810-3846.
 */

#include "melee.h"

#include "core/gmacros.h"
#include "core/globals.h"
#include "core/io.h"
#include "systems/death.h"
#include "systems/timer.h"
#include "verbs/verbs.h"
#include "world/objects.h"

namespace Melee {

namespace {

ZObject *obj(ObjectId id) { return Globals::instance().getObject(id); }

/// ZIL: <GET ,VILLAINS 0> is the LTABLE length, and both DO-FIGHT and
/// I-FIGHT stop as soon as CNT reaches it, so the last row is never used.
/// The cyclops is that row: he never trades blows, which is why
/// CYCLOPS-MELEE is among the unused symbols in zil/zork1.errors. He fights
/// through I-CYCLOPS and CYCLOPS-FCN instead.
/// Source: zil/1actions.zil:3334-3338, 3814-3816
std::size_t villainLoopLimit() { return villains().size(); }

} // namespace

// ZIL: <ROUTINE FIND-WEAPON (O "AUX" W) ...>
// Source: zil/1actions.zil:3398-3408
ZObject *findWeapon(const ZObject *holder) {
  if (!holder) return nullptr;
  for (ZObject *w : holder->getContents()) {
    const ObjectId id = w->getId();
    if (id == ObjectIds::STILETTO || id == ObjectIds::AXE ||
        id == ObjectIds::SWORD || id == ObjectIds::KNIFE ||
        id == ObjectIds::RUSTY_KNIFE) {
      return w;
    }
  }
  return nullptr;
}

// ZIL: <ROUTINE REMARK (REMARK D W "AUX" ...) ...>
// Source: zil/1actions.zil:3361-3369
void remark(const MeleeMsg &msg, ZObject *defender, ZObject *weapon) {
  for (const auto &part : msg) {
    switch (part.kind) {
    case MeleePart::Weapon:
      // ZIL <PRINTD .W> with no weapon prints nothing.
      if (weapon) tell(weapon);
      break;
    case MeleePart::Defender:
      if (defender) tell(defender);
      break;
    case MeleePart::Text:
      tell(part.text);
      break;
    }
  }
  crlf();
}

// ZIL: <ROUTINE FIGHT-STRENGTH ("OPTIONAL" (ADJUST? T) "AUX" S) ...>
// Source: zil/1actions.zil:3374-3381
int fightStrength(bool adjust) {
  auto &g = Globals::instance();
  const int s =
      STRENGTH_MIN + g.score / (Globals::SCORE_MAX / (STRENGTH_MAX - STRENGTH_MIN));
  if (!adjust) return s;
  return s + (g.winner ? g.winner->getProperty(P_STRENGTH) : 0);
}

// ZIL: <ROUTINE VILLAIN-STRENGTH (OO "AUX" (VILLAIN ...) OD TMP) ...>
// Source: zil/1actions.zil:3383-3396
int villainStrength(const Villain &row) {
  auto &g = Globals::instance();
  ZObject *villain = obj(row.villain);
  if (!villain) return 0;
  int od = villain->getProperty(P_STRENGTH);
  if (od >= 0) {
    // ZIL: the engrossed thief fights at no more than 2, once.
    if (row.villain == ObjectIds::THIEF && g.thiefEngrossed) {
      if (od > 2) od = 2;
      g.thiefEngrossed = false;
    }
    // ZIL: the weapon he fears most takes V-BEST-ADV off his strength.
    if (g.prsi && g.prsi->hasFlag(ObjectFlag::WEAPONBIT) &&
        row.best != 0 && g.prsi->getId() == row.best) {
      int tmp = od - row.bestAdv;
      if (tmp < 1) tmp = 1;
      od = tmp;
    }
  }
  return od;
}

namespace {

/// The shared table selection of VILLAIN-BLOW and HERO-BLOW: the defender's
/// strength picks the DEF table, the attacker's the window into it.
/// Source: zil/1actions.zil:3444-3456, 3524-3536
DefWindow selectWindow(int att, int def) {
  if (def == 1) {
    if (att > 2) att = 3;
    return DEF1_RES[static_cast<std::size_t>(att - 1)];
  }
  if (def == 2) {
    if (att > 3) att = 4;
    return DEF2_RES[static_cast<std::size_t>(att - 1)];
  }
  // def > 2
  att = att - def;
  if (att < -1) att = -2;
  else if (att > 1) att = 2;
  return DEF3_RES[static_cast<std::size_t>(att + 2)];
}

int rollResult(const DefWindow &window) {
  if (window.empty()) return static_cast<int>(MISSED);
  const std::size_t idx = static_cast<std::size_t>(GMacros::random(9) - 1);
  return static_cast<int>(window[idx < window.size() ? idx : 0]);
}

} // namespace

// ZIL: <ROUTINE VILLAIN-BLOW (OO OUT? "AUX" ...) ...>
// Source: zil/1actions.zil:3412-3474
int villainBlow(Villain &row, int out) {
  auto &g = Globals::instance();
  ZObject *villain = obj(row.villain);
  if (!villain || !g.winner) return 0;

  g.winner->clearFlag(ObjectFlag::STAGGERED);

  // ZIL: a staggered villain spends his blow getting up.
  if (villain->hasFlag(ObjectFlag::STAGGERED)) {
    tell("The ", villain, " slowly regains his feet.", CR);
    villain->clearFlag(ObjectFlag::STAGGERED);
    return static_cast<int>(MISSED);
  }

  int att = villainStrength(row);
  int def = fightStrength();
  if (def <= 0) return static_cast<int>(MISSED);
  const int od = fightStrength(false);
  ZObject *dweapon = findWeapon(g.winner);

  int res = 0;
  if (def < 0) {
    res = static_cast<int>(KILLED);
  } else {
    res = rollResult(selectWindow(att, def));
    // ZIL: a knocked-out defender cannot dodge.
    if (out) {
      res = (res == static_cast<int>(STAGGER)) ? static_cast<int>(HESITATE)
                                               : static_cast<int>(SITTING_DUCK);
    }
    // ZIL: <PROB 25 <COND (.HERO? 10)(T 50)>>. HERO? is an undeclared local
    // inside the discarded second argument of the PROB macro, so the second
    // value never reaches ZPROB; only its presence matters.
    // Source: zil/1actions.zil:3459-3461, zil/gmacros.zil:115-122
    if (res == static_cast<int>(STAGGER) && dweapon && GMacros::prob(25, true)) {
      res = static_cast<int>(LOSE_WEAPON);
    }
    const auto &rows = row.msgs;
    const std::size_t rowIdx = static_cast<std::size_t>(res - 1);
    if (rowIdx < rows.size() && !rows[rowIdx].empty()) {
      remark(GMacros::randomElement(rows[rowIdx]), g.winner, dweapon);
    }
  }

  if (res == static_cast<int>(MISSED) || res == static_cast<int>(HESITATE)) {
    // no effect
  } else if (res == static_cast<int>(UNCONSCIOUS)) {
    // no effect on strength; DO-FIGHT starts the free-blow countdown
  } else if (res == static_cast<int>(KILLED) || res == static_cast<int>(SITTING_DUCK)) {
    def = 0;
  } else if (res == static_cast<int>(LIGHT_WOUND)) {
    def -= 1;
    if (def < 0) def = 0;
    if (g.loadAllowed > 50) g.loadAllowed -= 10;
  } else if (res == static_cast<int>(SERIOUS_WOUND)) {
    def -= 2;
    if (def < 0) def = 0;
    if (g.loadAllowed > 50) g.loadAllowed -= 20;
  } else if (res == static_cast<int>(STAGGER)) {
    g.winner->setFlag(ObjectFlag::STAGGERED);
  } else {
    // ZIL: LOSE-WEAPON
    if (dweapon) {
      dweapon->moveTo(g.here);
      if (ZObject *nweapon = findWeapon(g.winner)) {
        tell("Fortunately, you still have a ", nweapon, ".", CR);
      }
    }
  }

  return winnerResult(def, res, od);
}

// ZIL: <ROUTINE HERO-BLOW ("AUX" ...) ...>
// Source: zil/1actions.zil:3476-3556
int heroBlow() {
  auto &g = Globals::instance();
  if (!g.prso) return 0;

  // ZIL: walk VILLAINS looking for PRSO. The loop stops at the table length,
  // so a villain in the last row is never matched and OO keeps the previous
  // row; only the troll and thief can be found.
  // Source: zil/1actions.zil:3478-3484
  auto rows = villains();
  Villain *row = nullptr;
  for (std::size_t cnt = 1; cnt < villainLoopLimit(); ++cnt) {
    row = &rows[cnt - 1];
    if (row->villain == g.prso->getId()) break;
  }
  if (!row) return 0;

  g.prso->setFlag(ObjectFlag::FIGHTBIT);

  if (g.winner->hasFlag(ObjectFlag::STAGGERED)) {
    tell("You are still recovering from that last blow, so your attack is "
         "ineffective.",
         CR);
    g.winner->clearFlag(ObjectFlag::STAGGERED);
    return static_cast<int>(MISSED);
  }

  int att = fightStrength();
  if (att < 1) att = 1;
  ZObject *villain = obj(row->villain);
  if (!villain) return 0;

  int def = villainStrength(*row);
  const int od = def;
  (void)od;
  if (def == 0) {
    if (g.prso == g.winner) {
      DeathSystem::jigsUp(
          "Well, you really did it that time. Is suicide painless?");
      return GMacros::rfatal();
    }
    tell("Attacking the ", villain, " is pointless.", CR);
    return static_cast<int>(MISSED);
  }

  ZObject *dweapon = findWeapon(villain);
  int res = 0;
  if (!dweapon || def < 0) {
    tell("The ");
    tell(def < 0 ? "unconscious" : "unarmed");
    tell(" ", villain, " cannot defend himself: He dies.", CR);
    res = static_cast<int>(KILLED);
  } else {
    res = rollResult(selectWindow(att, def));
    // HERO-BLOW's OUT? is always false, so no HESITATE/SITTING-DUCK here.
    if (res == static_cast<int>(STAGGER) && dweapon && GMacros::prob(25)) {
      res = static_cast<int>(LOSE_WEAPON);
    }
    const std::size_t rowIdx = static_cast<std::size_t>(res - 1);
    if (rowIdx < HERO_MELEE.size() && !HERO_MELEE[rowIdx].empty()) {
      remark(GMacros::randomElement(HERO_MELEE[rowIdx]), g.prso, g.prsi);
    }
  }

  if (res == static_cast<int>(MISSED) || res == static_cast<int>(HESITATE)) {
    // no effect
  } else if (res == static_cast<int>(UNCONSCIOUS)) {
    def = -def;
  } else if (res == static_cast<int>(KILLED) || res == static_cast<int>(SITTING_DUCK)) {
    def = 0;
  } else if (res == static_cast<int>(LIGHT_WOUND)) {
    def -= 1;
    if (def < 0) def = 0;
  } else if (res == static_cast<int>(SERIOUS_WOUND)) {
    def -= 2;
    if (def < 0) def = 0;
  } else if (res == static_cast<int>(STAGGER)) {
    g.prso->setFlag(ObjectFlag::STAGGERED);
  } else {
    // ZIL: LOSE-WEAPON - the villain's weapon lands on the floor, takeable.
    if (dweapon) {
      dweapon->clearFlag(ObjectFlag::NDESCBIT);
      dweapon->setFlag(ObjectFlag::WEAPONBIT);
      dweapon->moveTo(g.here);
      Verbs::thisIsIt(dweapon);
    }
  }

  return villainResult(g.prso, def, res);
}

// ZIL: <ROUTINE WINNER-RESULT (DEF RES OD) ...>
// Source: zil/1actions.zil:3560-3574
int winnerResult(int def, int res, int od) {
  auto &g = Globals::instance();
  if (!g.winner) return res;

  g.winner->setProperty(P_STRENGTH, def == 0 ? -10000 : def - od);
  if (def - od < 0) {
    // ZIL: <ENABLE <QUEUE I-CURE ,CURE-WAIT>>. QUEUE calls INT, which
    // allocates the C-TABLE entry on first use, so I-CURE is not one of the
    // five interrupts GO queues.
    TimerSystem::interrupt("I-CURE", iCure);
    TimerSystem::queue("I-CURE", CURE_WAIT);
    TimerSystem::enable("I-CURE");
  }
  if (fightStrength() <= 0) {
    g.winner->setProperty(P_STRENGTH, 1 + (-fightStrength(false)));
    DeathSystem::jigsUp("It appears that that last blow was too much for you. "
                        "I'm afraid you are dead.");
    return 0;
  }
  return res;
}

// ZIL: <ROUTINE VILLAIN-RESULT (VILLAIN DEF RES) ...>
// Source: zil/1actions.zil:3576-3592
int villainResult(ZObject *villain, int def, int res) {
  if (!villain) return res;
  villain->setProperty(P_STRENGTH, def);
  if (def == 0) {
    villain->clearFlag(ObjectFlag::FIGHTBIT);
    tell("Almost as soon as the ", villain,
         " breathes his last breath, a cloud of sinister black fog envelops "
         "him, and when the fog lifts, the carcass has disappeared.",
         CR);
    Verbs::removeCarefully(villain);
    villain->performAction(F_DEAD);
    return res;
  }
  if (res == static_cast<int>(UNCONSCIOUS)) {
    villain->performAction(F_UNCONSCIOUS);
  }
  return res;
}

// ZIL: <ROUTINE WINNING? (V "AUX" VS PS) ...>
// Source: zil/1actions.zil:3596-3603
bool winning(ZObject *villain) {
  if (!villain) return false;
  const int vs = villain->getProperty(P_STRENGTH);
  const int ps = vs - fightStrength();
  if (ps > 3) return GMacros::prob(90);
  if (ps > 0) return GMacros::prob(75);
  if (ps == 0) return GMacros::prob(50);
  if (vs > 1) return GMacros::prob(25);
  return GMacros::prob(10);
}

// ZIL: <ROUTINE AWAKEN (O "AUX" (S ...)) ...>
// Source: zil/1actions.zil:3842-3846
bool awaken(ZObject *villain) {
  if (!villain) return true;
  const int s = villain->getProperty(P_STRENGTH);
  if (s < 0) {
    villain->setProperty(P_STRENGTH, -s);
    villain->performAction(F_CONSCIOUS);
  }
  return true;
}

// ZIL: <ROUTINE DO-FIGHT (LEN "AUX" CNT RES O OO (OUT <>)) ...>
// Source: zil/1actions.zil:3331-3357
void doFight() {
  auto rows = villains();
  int out = 0;
  bool res = false;

  while (true) {
    res = false;
    bool broke = false;
    for (std::size_t cnt = 1; cnt < villainLoopLimit(); ++cnt) {
      Villain &row = rows[cnt - 1];
      ZObject *o = obj(row.villain);
      if (!o || !o->hasFlag(ObjectFlag::FIGHTBIT)) continue;
      // ZIL: a villain busy recovering his weapon does not strike.
      if (o->performAction(F_BUSY) != M_NOT_HANDLED) continue;
      const int blow = villainBlow(row, out);
      if (blow == 0) {
        res = false;
        broke = true;
        break;
      }
      res = true;
      if (blow == static_cast<int>(UNCONSCIOUS)) {
        out = 1 + GMacros::random(3);
      }
    }
    if (broke) return;
    if (!res) return;
    // ZIL: while the player is out, each pass costs one of his free blows.
    if (out == 0) return;
    out -= 1;
    if (out == 0) return;
  }
}

// ZIL: <ROUTINE I-FIGHT ("AUX" (FIGHT? <>) ...) ...>
// Source: zil/1actions.zil:3810-3840
bool iFight() {
  auto &g = Globals::instance();
  if (DeathSystem::isDead()) return false;

  bool fight = false;
  auto rows = villains();
  for (std::size_t cnt = 1; cnt < villainLoopLimit(); ++cnt) {
    Villain &row = rows[cnt - 1];
    ZObject *o = obj(row.villain);
    if (!o) continue;

    if (o->getLocation() == g.here && !o->hasFlag(ObjectFlag::INVISIBLE)) {
      if (row.villain == ObjectIds::THIEF && g.thiefEngrossed) {
        g.thiefEngrossed = false;
      } else if (o->getProperty(P_STRENGTH) < 0) {
        // ZIL: a knocked-out villain comes round on a rising chance.
        const int p = row.prob;
        if (p != 0 && GMacros::prob(p)) {
          row.prob = 0;
          awaken(o);
        } else {
          row.prob = p + 25;
        }
      } else if (o->hasFlag(ObjectFlag::FIGHTBIT) ||
                 o->performAction(F_FIRST) != M_NOT_HANDLED) {
        fight = true;
      }
    } else {
      // ZIL: out of the room, the fight is off and he recovers.
      if (o->hasFlag(ObjectFlag::FIGHTBIT)) o->performAction(F_BUSY);
      if (row.villain == ObjectIds::THIEF) g.thiefEngrossed = false;
      if (g.winner) g.winner->clearFlag(ObjectFlag::STAGGERED);
      o->clearFlag(ObjectFlag::STAGGERED);
      o->clearFlag(ObjectFlag::FIGHTBIT);
      awaken(o);
    }
  }
  if (!fight) return false;
  doFight();
  return true;
}

// ZIL: <ROUTINE I-CURE ("AUX" (S ...)) ...>
// Source: zil/1actions.zil:3605-3617
bool iCure() {
  auto &g = Globals::instance();
  if (!g.winner) return false;
  int s = g.winner->getProperty(P_STRENGTH);
  if (s > 0) {
    s = 0;
    g.winner->setProperty(P_STRENGTH, s);
  } else if (s < 0) {
    s = s + 1;
    g.winner->setProperty(P_STRENGTH, s);
  }
  if (s < 0) {
    if (g.loadAllowed < g.loadMax) g.loadAllowed += 10;
    TimerSystem::interrupt("I-CURE", iCure);
    TimerSystem::queue("I-CURE", CURE_WAIT);
    TimerSystem::enable("I-CURE");
  } else {
    g.loadAllowed = g.loadMax;
    TimerSystem::disable("I-CURE");
  }
  return false;
}

} // namespace Melee
