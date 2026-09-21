#pragma once
#include <array>
#include <span>

/**
 * @file melee_tables.h
 * @brief Melee constants and result tables (mirrors 1actions.zil:3236-3330)
 *
 * Source: zil/1actions.zil:3236-3330
 *
 * The DEF*-RES tables are compiled with literal 0 slots because the
 * <REST ,DEFn k> forms are commented out in the source; GO patches them at
 * startup (1dungeon.zil:2644-2649). Each slot is a window onto a DEF table
 * starting at word 0, 1 or 2; VILLAIN-BLOW/HERO-BLOW read nine entries
 * (<RANDOM 9>) from the selected window.
 */
namespace Melee {

// ZIL: villain action messages (1actions.zil:3236-3240)
constexpr int F_BUSY = 1;        // ZIL: F-BUSY? "busy recovering weapon?"
constexpr int F_DEAD = 2;        // ZIL: F-DEAD "mistah kurtz, he dead."
constexpr int F_UNCONSCIOUS = 3; // ZIL: F-UNCONSCIOUS "into dreamland"
constexpr int F_CONSCIOUS = 4;   // ZIL: F-CONSCIOUS "rise and shine"
constexpr int F_FIRST = 5;       // ZIL: F-FIRST? "strike first?"

// ZIL: blow results (1actions.zil:3246-3254)
constexpr int MISSED = 1;        // attacker misses
constexpr int UNCONSCIOUS = 2;   // defender unconscious
constexpr int KILLED = 3;        // defender dead
constexpr int LIGHT_WOUND = 4;   // defender lightly wounded
constexpr int SERIOUS_WOUND = 5; // defender seriously wounded
constexpr int STAGGER = 6;       // defender staggered (miss turn)
constexpr int LOSE_WEAPON = 7;   // defender loses weapon
constexpr int HESITATE = 8;      // hesitates (miss on free swing)
constexpr int SITTING_DUCK = 9;  // sitting duck (crunch!)

// ZIL: useful constants (1actions.zil:3322-3326)
constexpr int STRENGTH_MAX = 7;
constexpr int STRENGTH_MIN = 2;
constexpr int CURE_WAIT = 30;

// ZIL: tables of melee results (1actions.zil:3258-3298), verbatim
extern const std::array<int, 13> DEF1;
extern const std::array<int, 10> DEF2A;
extern const std::array<int, 12> DEF2B;
extern const std::array<int, 11> DEF3A;
extern const std::array<int, 11> DEF3B;
extern const std::array<int, 11> DEF3C;

// ZIL: DEF1-RES, DEF2-RES, DEF3-RES (1actions.zil:3301-3317). A slot is a
// window onto a DEF table (<REST ,DEFn k> = k/2 words in). Slots that the
// source leaves as 0 are empty until GO patches them.
using DefWindow = std::span<const int>;
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

} // namespace Melee
