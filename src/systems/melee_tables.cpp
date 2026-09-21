/**
 * @file melee_tables.cpp
 * @brief The DEF*-RES windows and VILLAINS, built on the generated ZIL tables.
 *
 * Source: zil/1actions.zil:3301-3317 (DEF*-RES), 3795-3801 (VILLAINS),
 *         zil/1dungeon.zil:2644-2649 (the GO fix-ups).
 *
 * The DEF and message tables themselves live in
 * src/world/generated/zil_tables.* and are produced from the ZIL by
 * tools/worldgen/melee_tables.py.
 */

#include "melee_tables.h"

#include "world/zil_registry.h"

#include <array>

namespace Melee {

namespace {
// ZIL: <REST ,TBL k> on a word table skips k/2 entries.
DefWindow rest(DefWindow tbl, int bytes) {
  return tbl.subspan(static_cast<std::size_t>(bytes / 2));
}
} // namespace

// ZIL: <GLOBAL DEF1-RES <TABLE DEF1 0 ;<REST ,DEF1 2> 0 ;<REST ,DEF1 4>>>
// Source: zil/1actions.zil:3301-3304
std::array<DefWindow, 3> DEF1_RES = {DEF1, DefWindow{}, DefWindow{}};

// ZIL: <GLOBAL DEF2-RES <TABLE DEF2A DEF2B 0; <REST ,DEF2B 2> 0; <REST ,DEF2B 4>>>
// Source: zil/1actions.zil:3306-3310
std::array<DefWindow, 4> DEF2_RES = {DEF2A, DEF2B, DefWindow{}, DefWindow{}};

// ZIL: <GLOBAL DEF3-RES <TABLE DEF3A 0 ;<REST ,DEF3A 2> DEF3B 0 ;<REST ,DEF3B 2> DEF3C>>
// Source: zil/1actions.zil:3312-3317
std::array<DefWindow, 5> DEF3_RES = {DEF3A, DefWindow{}, DEF3B, DefWindow{}, DEF3C};

void resetDefRes() {
  DEF1_RES = {DEF1, DefWindow{}, DefWindow{}};
  DEF2_RES = {DEF2A, DEF2B, DefWindow{}, DefWindow{}};
  DEF3_RES = {DEF3A, DefWindow{}, DEF3B, DefWindow{}, DEF3C};
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

namespace {

std::span<const MeleeResult> messagesNamed(std::string_view name) {
  if (name == "TROLL-MELEE") return TROLL_MELEE;
  if (name == "THIEF-MELEE") return THIEF_MELEE;
  if (name == "CYCLOPS-MELEE") return CYCLOPS_MELEE;
  if (name == "HERO-MELEE") return HERO_MELEE;
  return {};
}

std::array<Villain, std::size(zork::zil::kVILLAINS)> gVillains{};
bool gVillainsBuilt = false;

void build() {
  for (std::size_t i = 0; i < std::size(zork::zil::kVILLAINS); ++i) {
    const auto &def = zork::zil::kVILLAINS[i];
    gVillains[i] = Villain{ZilRegistry::idFor(def.villain),
                           def.best_weapon.empty() ? ObjectId{0}
                                                   : ZilRegistry::idFor(def.best_weapon),
                           def.best_adv, def.prob, messagesNamed(def.msgs)};
  }
  gVillainsBuilt = true;
}

} // namespace

std::span<Villain> villains() {
  if (!gVillainsBuilt) build();
  return gVillains;
}

void resetVillains() { build(); }

Villain *villainFor(ObjectId id) {
  if (id == 0) return nullptr;
  for (auto &v : villains()) {
    if (v.villain == id) return &v;
  }
  return nullptr;
}

} // namespace Melee
