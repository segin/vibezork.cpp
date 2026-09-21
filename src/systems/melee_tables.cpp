/**
 * @file melee_tables.cpp
 * @brief Melee result tables (mirrors 1actions.zil:3258-3317)
 *
 * Source: zil/1actions.zil:3258-3317, zil/1dungeon.zil:2644-2649
 */

#include "melee_tables.h"

namespace Melee {

// ZIL: <GLOBAL DEF1 <TABLE (PURE) MISSED MISSED MISSED MISSED STAGGER STAGGER
//        UNCONSCIOUS UNCONSCIOUS KILLED KILLED KILLED KILLED KILLED>>
// Source: zil/1actions.zil:3258-3263
const std::array<int, 13> DEF1 = {MISSED, MISSED,      MISSED,      MISSED, STAGGER,
                                  STAGGER, UNCONSCIOUS, UNCONSCIOUS, KILLED, KILLED,
                                  KILLED, KILLED,      KILLED};

// ZIL: <GLOBAL DEF2A <TABLE (PURE) MISSED MISSED MISSED MISSED MISSED STAGGER
//        STAGGER LIGHT-WOUND LIGHT-WOUND UNCONSCIOUS>>
// Source: zil/1actions.zil:3265-3270
const std::array<int, 10> DEF2A = {MISSED,  MISSED,      MISSED,      MISSED, MISSED,
                                   STAGGER, STAGGER,     LIGHT_WOUND, LIGHT_WOUND,
                                   UNCONSCIOUS};

// ZIL: <GLOBAL DEF2B <TABLE (PURE) MISSED MISSED MISSED STAGGER STAGGER
//        LIGHT-WOUND LIGHT-WOUND LIGHT-WOUND UNCONSCIOUS KILLED KILLED KILLED>>
// Source: zil/1actions.zil:3272-3278
const std::array<int, 12> DEF2B = {MISSED,      MISSED,      MISSED,      STAGGER,
                                   STAGGER,     LIGHT_WOUND, LIGHT_WOUND, LIGHT_WOUND,
                                   UNCONSCIOUS, KILLED,      KILLED,      KILLED};

// ZIL: <GLOBAL DEF3A <TABLE (PURE) MISSED MISSED MISSED MISSED MISSED STAGGER
//        STAGGER LIGHT-WOUND LIGHT-WOUND SERIOUS-WOUND SERIOUS-WOUND>>
// Source: zil/1actions.zil:3280-3285
const std::array<int, 11> DEF3A = {MISSED,      MISSED,      MISSED,        MISSED,
                                   MISSED,      STAGGER,     STAGGER,       LIGHT_WOUND,
                                   LIGHT_WOUND, SERIOUS_WOUND, SERIOUS_WOUND};

// ZIL: <GLOBAL DEF3B <TABLE (PURE) MISSED MISSED MISSED STAGGER STAGGER
//        LIGHT-WOUND LIGHT-WOUND LIGHT-WOUND SERIOUS-WOUND SERIOUS-WOUND
//        SERIOUS-WOUND>>
// Source: zil/1actions.zil:3287-3292
const std::array<int, 11> DEF3B = {MISSED,      MISSED,        MISSED,        STAGGER,
                                   STAGGER,     LIGHT_WOUND,   LIGHT_WOUND,   LIGHT_WOUND,
                                   SERIOUS_WOUND, SERIOUS_WOUND, SERIOUS_WOUND};

// ZIL: <GLOBAL DEF3C <TABLE (PURE) MISSED STAGGER STAGGER LIGHT-WOUND
//        LIGHT-WOUND LIGHT-WOUND LIGHT-WOUND SERIOUS-WOUND SERIOUS-WOUND
//        SERIOUS-WOUND>>
// Source: zil/1actions.zil:3294-3299
const std::array<int, 11> DEF3C = {MISSED,      STAGGER,     STAGGER,       LIGHT_WOUND,
                                   LIGHT_WOUND, LIGHT_WOUND, LIGHT_WOUND,   SERIOUS_WOUND,
                                   SERIOUS_WOUND, SERIOUS_WOUND, SERIOUS_WOUND};

namespace {
// <REST ,TBL k> on a word table: skip k/2 entries
template <std::size_t N> DefWindow rest(const std::array<int, N> &tbl, int bytes) {
  return DefWindow(tbl).subspan(static_cast<std::size_t>(bytes / 2));
}
} // namespace

// ZIL: <GLOBAL DEF1-RES <TABLE DEF1 0 ;<REST ,DEF1 2> 0 ;<REST ,DEF1 4>>>
// Source: zil/1actions.zil:3301-3304
std::array<DefWindow, 3> DEF1_RES = {DefWindow(DEF1), DefWindow{}, DefWindow{}};

// ZIL: <GLOBAL DEF2-RES <TABLE DEF2A DEF2B 0; <REST ,DEF2B 2> 0; <REST ,DEF2B 4>>>
// Source: zil/1actions.zil:3306-3310
std::array<DefWindow, 4> DEF2_RES = {DefWindow(DEF2A), DefWindow(DEF2B), DefWindow{},
                                     DefWindow{}};

// ZIL: <GLOBAL DEF3-RES <TABLE DEF3A 0 ;<REST ,DEF3A 2> DEF3B 0 ;<REST ,DEF3B 2> DEF3C>>
// Source: zil/1actions.zil:3312-3317
std::array<DefWindow, 5> DEF3_RES = {DefWindow(DEF3A), DefWindow{}, DefWindow(DEF3B),
                                     DefWindow{}, DefWindow(DEF3C)};

void resetDefRes() {
  DEF1_RES = {DefWindow(DEF1), DefWindow{}, DefWindow{}};
  DEF2_RES = {DefWindow(DEF2A), DefWindow(DEF2B), DefWindow{}, DefWindow{}};
  DEF3_RES = {DefWindow(DEF3A), DefWindow{}, DefWindow(DEF3B), DefWindow{},
              DefWindow(DEF3C)};
}

// ZIL: <PUT ,DEF1-RES 1 <REST ,DEF1 2>>
//      <PUT ,DEF1-RES 2 <REST ,DEF1 4>>
//      <PUT ,DEF2-RES 2 <REST ,DEF2B 2>>
//      <PUT ,DEF2-RES 3 <REST ,DEF2B 4>>
//      <PUT ,DEF3-RES 1 <REST ,DEF3A 2>>
//      <PUT ,DEF3-RES 3 <REST ,DEF3B 2>>
// Source: zil/1dungeon.zil:2644-2649
void patchDefRes() {
  DEF1_RES[1] = rest(DEF1, 2);
  DEF1_RES[2] = rest(DEF1, 4);
  DEF2_RES[2] = rest(DEF2B, 2);
  DEF2_RES[3] = rest(DEF2B, 4);
  DEF3_RES[1] = rest(DEF3A, 2);
  DEF3_RES[3] = rest(DEF3B, 2);
}

bool defResPatched() {
  for (const auto &w : DEF1_RES) {
    if (w.empty()) return false;
  }
  for (const auto &w : DEF2_RES) {
    if (w.empty()) return false;
  }
  for (const auto &w : DEF3_RES) {
    if (w.empty()) return false;
  }
  return true;
}

} // namespace Melee
