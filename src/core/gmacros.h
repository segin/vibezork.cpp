#pragma once
#include "core/globals.h"
#include "core/object.h"
#include "core/types.h"
#include "systems/timer.h"
#include <concepts>
#include <cstdlib>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/**
 * @file gmacros.h
 * @brief Canonical macros and utilities ported from GMACROS.ZIL
 *
 * Source: zil/gmacros.zil:1-155
 *
 * Infocom ZIL Macro system:
 * - Predicate matching: VERB?, PRSO?, PRSI?, ROOM?
 * - Multi-bit operations: BSET, BCLEAR, BSET?
 * - Execution control: RFATAL
 * - Probability & RNG: PROB, ZPROB
 * - Randomized tables: RANDOM-ELEMENT, PICK-ONE
 * - Interrupt control: ENABLE, DISABLE
 * - Object queries: FLAMING?, OPENABLE?
 * - Math: ABS
 */

namespace GMacros {

// ZIL: Constants from gmacros.zil:5-7
constexpr int C_ENABLED_FLAG = 0;  // ZIL: C-ENABLED?
constexpr int C_ENABLED = 1;       // ZIL: C-ENABLED
constexpr int C_DISABLED = 0;      // ZIL: C-DISABLED

// ============================================================================
// Multi-comparison Predicate Helpers (gmacros.zil:52-86)
// ============================================================================

// ZIL: <DEFMAC VERB? ("ARGS" ATMS) <MULTIFROB PRSA .ATMS>> (gmacros.zil:52-53)
template <typename... Args>
inline bool isVerb(Args... verbs) {
  auto prsa = Globals::instance().prsa;
  return ((prsa == static_cast<VerbId>(verbs)) || ...);
}

// ZIL: <DEFMAC PRSO? ("ARGS" ATMS) <MULTIFROB PRSO .ATMS>> (gmacros.zil:55-56)
template <typename... Args>
inline bool isPrso(Args... objs) {
  auto *prso = Globals::instance().prso;
  if (!prso) return false;
  auto matches = [prso](auto obj) -> bool {
    if constexpr (std::is_pointer_v<decltype(obj)>) {
      return prso == obj;
    } else {
      return prso->getId() == static_cast<ObjectId>(obj);
    }
  };
  return (matches(objs) || ...);
}

// ZIL: <DEFMAC PRSI? ("ARGS" ATMS) <MULTIFROB PRSI .ATMS>> (gmacros.zil:58-59)
template <typename... Args>
inline bool isPrsi(Args... objs) {
  auto *prsi = Globals::instance().prsi;
  if (!prsi) return false;
  auto matches = [prsi](auto obj) -> bool {
    if constexpr (std::is_pointer_v<decltype(obj)>) {
      return prsi == obj;
    } else {
      return prsi->getId() == static_cast<ObjectId>(obj);
    }
  };
  return (matches(objs) || ...);
}

// ZIL: <DEFMAC ROOM? ("ARGS" ATMS) <MULTIFROB HERE .ATMS>> (gmacros.zil:61-62)
template <typename... Args>
inline bool isRoom(Args... rooms) {
  auto *here = Globals::instance().here;
  if (!here) return false;
  auto matches = [here](auto room) -> bool {
    if constexpr (std::is_pointer_v<decltype(room)>) {
      return here == room;
    } else {
      return here->getId() == static_cast<ObjectId>(room);
    }
  };
  return (matches(rooms) || ...);
}

// ============================================================================
// Multi-bit Flag Operations (gmacros.zil:88-110)
// ============================================================================

// ZIL: <DEFMAC BSET ('OBJ "ARGS" BITS) <MULTIBITS FSET .OBJ .BITS>> (gmacros.zil:88-89)
template <typename... Flags>
inline void bset(ZObject *obj, Flags... flags) {
  if (obj) {
    (obj->setFlag(flags), ...);
  }
}

// ZIL: <DEFMAC BCLEAR ('OBJ "ARGS" BITS) <MULTIBITS FCLEAR .OBJ .BITS>> (gmacros.zil:91-92)
template <typename... Flags>
inline void bclear(ZObject *obj, Flags... flags) {
  if (obj) {
    (obj->clearFlag(flags), ...);
  }
}

// ZIL: <DEFMAC BSET? ('OBJ "ARGS" BITS) <MULTIBITS FSET? .OBJ .BITS>> (gmacros.zil:94-95)
// Returns true if ANY of the specified bits are set on OBJ
template <typename... Flags>
inline bool bsetQ(const ZObject *obj, Flags... flags) {
  return obj && (obj->hasFlag(flags) || ...);
}

// ============================================================================
// Execution Control (gmacros.zil:112-113)
// ============================================================================

// ZIL: <DEFMAC RFATAL () '<PROG () <PUSH 2> <RSTACK>>> (gmacros.zil:112-113)
constexpr int rfatal() {
  return M_FATAL;
}

// ============================================================================
// Probability and Randomization (gmacros.zil:115-123)
// ============================================================================

// ZIL: <ROUTINE ZPROB (BASE) ...> (gmacros.zil:119-123)
inline bool zprob(int base) {
  int limit = Globals::instance().lucky ? 100 : 300;
  int roll = (std::rand() % limit) + 1; // 1 to limit
  return base > roll;
}

// ZIL: <DEFMAC PROB ('BASE? "OPTIONAL" 'LOSER?) ...> (gmacros.zil:115-117)
inline bool prob(int base, bool hasLoser = false) {
  if (hasLoser) {
    return zprob(base);
  }
  int roll = (std::rand() % 100) + 1; // 1 to 100
  return base > roll;
}

// ============================================================================
// Randomized Table Routines (gmacros.zil:124-140)
// ============================================================================

// ZIL: <ROUTINE RANDOM-ELEMENT (FROB) <GET .FROB <RANDOM <GET .FROB 0>>>> (gmacros.zil:124-125)
template <typename T>
inline const T &randomElement(std::span<const T> table) {
  if (table.empty()) {
    static const T defaultVal{};
    return defaultVal;
  }
  size_t idx = std::rand() % table.size();
  return table[idx];
}

template <typename T>
inline const T &randomElement(const std::vector<T> &table) {
  return randomElement(std::span<const T>(table));
}

/**
 * @brief Class representing a ZIL randomized table (LTABLE/P-TABLE)
 * Implements non-repeating permutation cycle matching PICK-ONE (gmacros.zil:127-140)
 */
template <typename T>
class ZilRandomTable {
public:
  ZilRandomTable() = default;
  explicit ZilRandomTable(std::initializer_list<T> items)
      : items_(items), counter_(0) {}
  explicit ZilRandomTable(std::vector<T> items)
      : items_(std::move(items)), counter_(0) {}

  // ZIL: <ROUTINE RANDOM-ELEMENT (FROB) ...> (gmacros.zil:124-125)
  const T &randomElement() const {
    return GMacros::randomElement(std::span<const T>(items_));
  }

  // ZIL: <ROUTINE PICK-ONE (FROB ...)> (gmacros.zil:127-140)
  // Guarantees all elements are returned once in random order before repeating
  const T &pickOne() {
    if (items_.empty()) {
      static const T defaultVal{};
      return defaultVal;
    }
    size_t l = items_.size();
    if (l == 1) {
      return items_[0];
    }

    // Pick from remaining unselected items [counter_, l - 1]
    size_t remaining = l - counter_;
    size_t rndOffset = std::rand() % remaining;
    size_t chosenIdx = counter_ + rndOffset;

    // Swap chosen item into current slot
    std::swap(items_[counter_], items_[chosenIdx]);
    const T &msg = items_[counter_];

    // Advance cycle counter
    counter_++;
    if (counter_ >= l) {
      counter_ = 0;
    }

    return msg;
  }

  size_t size() const { return items_.size(); }
  size_t getCycleCount() const { return counter_; }
  void reset() { counter_ = 0; }
  const std::vector<T> &getItems() const { return items_; }

private:
  std::vector<T> items_;
  size_t counter_ = 0;
};

// Convenience free function wrappers
template <typename T>
inline const T &pickOne(ZilRandomTable<T> &table) {
  return table.pickOne();
}

template <typename T>
inline const T &randomElement(const ZilRandomTable<T> &table) {
  return table.randomElement();
}

// ============================================================================
// Interrupt Enable / Disable (gmacros.zil:141-144)
// ============================================================================

// ZIL: <DEFMAC ENABLE ('INT) <FORM PUT .INT ,C-ENABLED? 1>> (gmacros.zil:141)
inline void enable(TimerSystem::Timer *cint) {
  if (cint) {
    cint->enabled = true;
  }
}

// ZIL: <DEFMAC DISABLE ('INT) <FORM PUT .INT ,C-ENABLED? 0>> (gmacros.zil:143)
inline void disable(TimerSystem::Timer *cint) {
  if (cint) {
    cint->enabled = false;
  }
}

// ============================================================================
// Object Queries (gmacros.zil:145-152)
// ============================================================================

// ZIL: <DEFMAC FLAMING? ('OBJ) <FORM AND <FORM FSET? .OBJ ',FLAMEBIT> <FORM FSET? .OBJ ',ONBIT>>> (gmacros.zil:145-147)
inline bool isFlaming(const ZObject *obj) {
  return obj && obj->hasFlag(ObjectFlag::FLAMEBIT) && obj->hasFlag(ObjectFlag::ONBIT);
}

// ZIL: <DEFMAC OPENABLE? ('OBJ) <FORM OR <FORM FSET? .OBJ ',DOORBIT> <FORM FSET? .OBJ ',CONTBIT>>> (gmacros.zil:149-151)
inline bool isOpenable(const ZObject *obj) {
  return obj && (obj->hasFlag(ObjectFlag::DOORBIT) || obj->hasFlag(ObjectFlag::CONTBIT));
}

// ============================================================================
// Math Macro (gmacros.zil:153-155)
// ============================================================================

// ZIL: <DEFMAC ABS ('NUM) <FORM COND (<FORM L? .NUM 0> <FORM - 0 .NUM>) (T .NUM)>> (gmacros.zil:153-155)
template <typename T>
constexpr T absVal(T num) {
  return num < 0 ? -num : num;
}

} // namespace GMacros
