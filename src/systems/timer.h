#pragma once
#include "core/types.h"
#include <array>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @file timer.h
 * @brief GCLOCK interrupt system (mirrors ZIL gclock.zil)
 *
 * Source: zil/gclock.zil:1-60
 *
 * C-TABLE is 180 words holding 6-byte entries (C-ENABLED?, C-TICK, C-RTN),
 * so at most 30 interrupts exist. INT allocates entries downward from the
 * end of the table (C-INTS starts at 180 and shrinks by C-INTLEN), and
 * CLOCKER scans from C-INTS to the end, so the most recently registered
 * interrupt runs first. An entry fires when its tick is 1 or negative
 * before the decrement (negative = every turn); it never re-arms itself,
 * the routine must QUEUE again. R119 never registers demons (C-DEMONS is
 * dead code), so that concept is not modelled.
 */

namespace TimerSystem {

// ZIL: Constants from gclock.zil:5-19
constexpr int C_TABLELEN = 180;
constexpr int C_INTLEN = 6;
constexpr int C_MAX_INTERRUPTS = C_TABLELEN / C_INTLEN; // 30 entries
constexpr int C_ENABLED_FLAG = 0; // ZIL: C-ENABLED?
constexpr int C_TICK_OFFSET = 1;  // ZIL: C-TICK
constexpr int C_RTN_OFFSET = 2;   // ZIL: C-RTN

/// Interrupt routine (ZIL: C-RTN). Its truth value becomes CLOCKER's FLG.
using InterruptRoutine = std::function<bool()>;

/// One C-TABLE entry (gclock.zil:13-19)
struct Interrupt {
  std::string name;         ///< Routine name, e.g. "I-LANTERN" (identity of C-RTN)
  bool enabled = false;     ///< ZIL: C-ENABLED?
  int tick = 0;             ///< ZIL: C-TICK
  InterruptRoutine routine; ///< ZIL: C-RTN
};

namespace detail {
template <typename F> InterruptRoutine wrapRoutine(F &&f) {
  if constexpr (std::is_same_v<std::decay_t<F>, std::nullptr_t>) {
    return nullptr;
  } else if constexpr (std::is_void_v<std::invoke_result_t<F &>>) {
    return [fn = std::forward<F>(f)]() mutable {
      fn();
      return false;
    };
  } else {
    return [fn = std::forward<F>(f)]() mutable { return static_cast<bool>(fn()); };
  }
}
} // namespace detail

class TimerManager {
public:
  static TimerManager &instance();

  // ZIL: <ROUTINE INT (RTN ...) ...> (gclock.zil:26-39): find the entry for
  // RTN or allocate a new one at the low end of the used table.
  Interrupt *interrupt(std::string_view name, InterruptRoutine routine = nullptr);

  template <typename F>
  Interrupt *interrupt(std::string_view name, F &&routine) {
    return interrupt(name, detail::wrapRoutine(std::forward<F>(routine)));
  }

  // ZIL: <ROUTINE QUEUE (RTN TICK ...) ...> (gclock.zil:21-24)
  Interrupt *queue(std::string_view name, int tick);

  // ZIL: ENABLE / DISABLE macros (gmacros.zil:141-143)
  void enable(std::string_view name);
  void disable(std::string_view name);
  bool isEnabled(std::string_view name) const;

  // ZIL: <ROUTINE CLOCKER ...> (gclock.zil:43-60)
  bool clocker();

  // Lookup without allocation
  Interrupt *find(std::string_view name);
  const Interrupt *find(std::string_view name) const;

  /// Entries in table order (C-INTS upward, i.e. newest first)
  std::vector<const Interrupt *> entries() const;
  int count() const { return C_MAX_INTERRUPTS - cInts_; }

  // Serialization support
  void setInterruptState(std::string_view name, bool enabled, int tick);

  void clear();

private:
  TimerManager() = default;
  TimerManager(const TimerManager &) = delete;
  TimerManager &operator=(const TimerManager &) = delete;

  std::array<Interrupt, C_MAX_INTERRUPTS> table_{}; ///< ZIL: C-TABLE
  int cInts_ = C_MAX_INTERRUPTS;                     ///< ZIL: C-INTS (in entries)
};

// Free functions mirroring the ZIL routines and macros

// ZIL: <INT RTN>
inline Interrupt *interrupt(std::string_view name) {
  return TimerManager::instance().interrupt(name);
}
template <typename F> inline Interrupt *interrupt(std::string_view name, F &&routine) {
  return TimerManager::instance().interrupt(name, std::forward<F>(routine));
}

// ZIL: <QUEUE RTN TICK>
inline Interrupt *queue(std::string_view name, int tick) {
  return TimerManager::instance().queue(name, tick);
}

// ZIL: <ENABLE <INT RTN>> / <DISABLE <INT RTN>>
inline void enable(std::string_view name) { TimerManager::instance().enable(name); }
inline void disable(std::string_view name) { TimerManager::instance().disable(name); }
inline bool isEnabled(std::string_view name) {
  return TimerManager::instance().isEnabled(name);
}

// ZIL: <CLOCKER>
inline bool clocker() { return TimerManager::instance().clocker(); }

inline void clear() { TimerManager::instance().clear(); }

// Compatibility spellings used by older call sites and tests
inline void enableTimer(std::string_view name) { enable(name); }
inline void disableTimer(std::string_view name) { disable(name); }
inline bool isTimerEnabled(std::string_view name) { return isEnabled(name); }
inline void queueTimer(std::string_view name, int ticks) { queue(name, ticks); }
inline bool tick() { return clocker(); }

} // namespace TimerSystem
