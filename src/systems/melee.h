#pragma once
#include "core/object.h"
#include "core/types.h"
#include "systems/melee_tables.h"

/**
 * @file melee.h
 * @brief The melee engine of zil/1actions.zil.
 *
 * Source: zil/1actions.zil:3331-3606 (DO-FIGHT, REMARK, FIGHT-STRENGTH,
 *         VILLAIN-STRENGTH, VILLAIN-BLOW, HERO-BLOW, WINNER-RESULT,
 *         VILLAIN-RESULT, WINNING?, I-CURE) and 3810-3849 (I-FIGHT, AWAKEN).
 *
 * A blow is not a damage roll. The defender's strength picks one of the DEF
 * tables, the attacker-minus-defender difference picks a nine-entry window
 * into it, and <RANDOM 9> picks the result: one of MISSED, UNCONSCIOUS,
 * KILLED, LIGHT-WOUND, SERIOUS-WOUND, STAGGER, LOSE-WEAPON, HESITATE or
 * SITTING-DUCK. Wounds are stored as a negative STRENGTH property on the
 * wounded party and heal a point at a time through I-CURE.
 */
namespace Melee {

/// ZIL: <ROUTINE REMARK (REMARK D W)> (1actions.zil:3361-3369). Prints one
/// melee message, substituting the defender for F-DEF and the weapon for
/// F-WEP, then a newline.
void remark(const MeleeMsg &msg, ZObject *defender, ZObject *weapon);

/// ZIL: <ROUTINE FIGHT-STRENGTH ("OPTIONAL" (ADJUST? T))>
/// (1actions.zil:3374-3381). STRENGTH-MIN plus a fifth of the score per
/// step, optionally adjusted by the winner's STRENGTH property (its wounds).
int fightStrength(bool adjust = true);

/// ZIL: <ROUTINE VILLAIN-STRENGTH (OO)> (1actions.zil:3383-3396). The
/// villain's STRENGTH, reduced by the advantage of the best weapon against
/// him and capped at 2 while the thief is engrossed.
int villainStrength(const Villain &row);

/// ZIL: <ROUTINE VILLAIN-BLOW (OO OUT?)> (1actions.zil:3412-3474). One blow
/// by a villain against the player. Returns the blow result, or 0 when the
/// player died or nothing happened.
int villainBlow(Villain &row, int out);

/// ZIL: <ROUTINE HERO-BLOW ()> (1actions.zil:3476-3556). One blow by the
/// player against PRSO with PRSI.
int heroBlow();

/// ZIL: <ROUTINE WINNER-RESULT (DEF RES OD)> (1actions.zil:3560-3574).
int winnerResult(int def, int res, int od);

/// ZIL: <ROUTINE VILLAIN-RESULT (VILLAIN DEF RES)> (1actions.zil:3576-3592).
int villainResult(ZObject *villain, int def, int res);

/// ZIL: <ROUTINE WINNING? (V)> (1actions.zil:3596-3603). True when the
/// villain is likely to be winning the fight.
bool winning(ZObject *villain);

/// ZIL: <ROUTINE DO-FIGHT (LEN)> (1actions.zil:3331-3357). Every fighting
/// villain strikes; a knocked-out player keeps taking free blows.
void doFight();

/// ZIL: <ROUTINE I-FIGHT ()> (1actions.zil:3810-3840). The fight demon,
/// queued every turn by GO.
bool iFight();

/// ZIL: <ROUTINE AWAKEN (O)> (1actions.zil:3842-3846). Brings a knocked-out
/// villain round.
bool awaken(ZObject *villain);

/// ZIL: <ROUTINE I-CURE ()> (1actions.zil:3605-3617). Heals one point of
/// wound every CURE-WAIT turns and restores the carrying capacity.
bool iCure();

/// ZIL: <ROUTINE FIND-WEAPON (O)> (1actions.zil:3398-3408).
ZObject *findWeapon(const ZObject *holder);

} // namespace Melee
