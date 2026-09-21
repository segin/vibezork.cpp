#pragma once
#include "core/types.h"
#include "world/generated/zil_tables.h"

#include <span>

/**
 * @file melee_tables.h
 * @brief The melee data of 1actions.zil, as the engine sees it.
 *
 * Source: zil/1actions.zil:3236-3330 (constants, DEF tables, DEF*-RES),
 *         zil/1actions.zil:3606-3803 (message tables, VILLAINS),
 *         zil/1dungeon.zil:2644-2649 (the GO fix-ups).
 *
 * The tables themselves are generated from the ZIL by
 * tools/worldgen/melee_tables.py into src/world/generated/zil_tables.*; this
 * header adds only what the engine needs on top of them: the DEF*-RES windows
 * that GO patches at start-up, and VILLAINS resolved to live objects.
 *
 * The DEF*-RES tables are compiled with literal 0 slots because the
 * <REST ,DEFn k> forms are commented out in the source; GO patches them at
 * startup.  Each slot is a window onto a DEF table starting at word 0, 1 or 2;
 * VILLAIN-BLOW/HERO-BLOW read nine entries (<RANDOM 9>) from the window.
 */
namespace Melee {

using zork::zil::BlowResult;
using zork::zil::MeleeMsg;
using zork::zil::MeleePart;
using zork::zil::MeleeResult;

// ZIL: villain action messages (1actions.zil:3236-3240)
inline constexpr int F_BUSY = zork::zil::kF_BUSY_Q;   // ZIL: F-BUSY? "busy recovering weapon?"
inline constexpr int F_DEAD = zork::zil::kF_DEAD;     // ZIL: F-DEAD "mistah kurtz, he dead."
inline constexpr int F_UNCONSCIOUS = zork::zil::kF_UNCONSCIOUS; // ZIL: "into dreamland"
inline constexpr int F_CONSCIOUS = zork::zil::kF_CONSCIOUS;     // ZIL: "rise and shine"
inline constexpr int F_FIRST = zork::zil::kF_FIRST_Q; // ZIL: F-FIRST? "strike first?"

// ZIL: blow results (1actions.zil:3246-3254)
inline constexpr BlowResult MISSED = BlowResult::MISSED;
inline constexpr BlowResult UNCONSCIOUS = BlowResult::UNCONSCIOUS;
inline constexpr BlowResult KILLED = BlowResult::KILLED;
inline constexpr BlowResult LIGHT_WOUND = BlowResult::LIGHT_WOUND;
inline constexpr BlowResult SERIOUS_WOUND = BlowResult::SERIOUS_WOUND;
inline constexpr BlowResult STAGGER = BlowResult::STAGGER;
inline constexpr BlowResult LOSE_WEAPON = BlowResult::LOSE_WEAPON;
inline constexpr BlowResult HESITATE = BlowResult::HESITATE;
inline constexpr BlowResult SITTING_DUCK = BlowResult::SITTING_DUCK;

// ZIL: useful constants (1actions.zil:3322-3326)
inline constexpr int STRENGTH_MAX = zork::zil::kSTRENGTH_MAX;
inline constexpr int STRENGTH_MIN = zork::zil::kSTRENGTH_MIN;
inline constexpr int CURE_WAIT = zork::zil::kCURE_WAIT;

// ZIL: tables of melee results (1actions.zil:3258-3298), verbatim
inline constexpr std::span<const BlowResult> DEF1{zork::zil::kDEF1};
inline constexpr std::span<const BlowResult> DEF2A{zork::zil::kDEF2A};
inline constexpr std::span<const BlowResult> DEF2B{zork::zil::kDEF2B};
inline constexpr std::span<const BlowResult> DEF3A{zork::zil::kDEF3A};
inline constexpr std::span<const BlowResult> DEF3B{zork::zil::kDEF3B};
inline constexpr std::span<const BlowResult> DEF3C{zork::zil::kDEF3C};

// ZIL: DEF1-RES, DEF2-RES, DEF3-RES (1actions.zil:3301-3317). A slot is a
// window onto a DEF table (<REST ,DEFn k> = k/2 words in). Slots that the
// source leaves as 0 are empty until GO patches them.
using DefWindow = std::span<const BlowResult>;
extern std::array<DefWindow, 3> DEF1_RES;
extern std::array<DefWindow, 4> DEF2_RES;
extern std::array<DefWindow, 5> DEF3_RES;

/// Reset the DEF*-RES tables to their compiled (unpatched) contents.
void resetDefRes();

/// ZIL: the six <PUT ,DEFn-RES i <REST ,DEFx k>> forms in GO
/// (1dungeon.zil:2644-2649).
void patchDefRes();

/// True once patchDefRes() has filled every slot.
bool defResPatched();

// ZIL: message tables (1actions.zil:3611-3800). Row i holds the messages for
// blow result i+1; each message is a list of parts printed in order by REMARK.
inline const std::span<const MeleeResult> &HERO_MELEE = zork::zil::HERO_MELEE;
inline const std::span<const MeleeResult> &CYCLOPS_MELEE = zork::zil::CYCLOPS_MELEE;
inline const std::span<const MeleeResult> &TROLL_MELEE = zork::zil::TROLL_MELEE;
inline const std::span<const MeleeResult> &THIEF_MELEE = zork::zil::THIEF_MELEE;

/**
 * @brief One row of VILLAINS (1actions.zil:3795-3800).
 *
 * ZIL: <TABLE villain best-weapon best-adv prob msgs>, addressed by the
 * V-VILLAIN, V-BEST, V-BEST-ADV, V-PROB and V-MSGS offsets.  V-PROB is written
 * at run time by I-FIGHT, so the table is mutable state, not a constant.
 */
struct Villain {
  ObjectId villain = 0;                 ///< ZIL: V-VILLAIN
  ObjectId best = 0;                    ///< ZIL: V-BEST, 0 when the ZIL has <>
  int bestAdv = 0;                      ///< ZIL: V-BEST-ADV
  int prob = 0;                         ///< ZIL: V-PROB, mutated by I-FIGHT
  std::span<const MeleeResult> msgs{};  ///< ZIL: V-MSGS
};

/// ZIL: <GLOBAL VILLAINS <LTABLE ...>> (1actions.zil:3801). Resolved from the
/// generated names on first use; the port's ids are not known at compile time.
std::span<Villain> villains();

/// Restores VILLAINS to its compiled contents (V-PROB back to 0). Called by GO.
void resetVillains();

/// The VILLAINS row for an object, or nullptr when it is not a villain.
Villain *villainFor(ObjectId id);

} // namespace Melee
