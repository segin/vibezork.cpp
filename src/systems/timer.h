#pragma once
#include "core/types.h"
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/**
 * @file timer.h
 * @brief GCLOCK Interrupt & Demon System (mirrors ZIL gclock.zil)
 *
 * Source: zil/gclock.zil:1-61
 *
 * Infocom ZIL Clock architecture:
 * - C-TABLE / C-INTS / C-DEMONS: Master table managing scheduled interrupts and demons.
 * - INT (RTN, DEMON): Finds or registers an interrupt routine entry.
 * - QUEUE (RTN, TICK): Sets ticks on an interrupt routine.
 * - CLOCKER (): Main per-turn clock routine, handles demons and normal interrupts.
 * - CLOCK-WAIT: Skip-turn flag.
 */

namespace TimerSystem {

// ZIL: Constants from gclock.zil:5-19
constexpr int C_TABLELEN = 180;
constexpr int C_INTLEN = 6;
constexpr int C_ENABLED_FLAG = 0; // ZIL: C-ENABLED?
constexpr int C_TICK_OFFSET = 1;  // ZIL: C-TICK
constexpr int C_RTN_OFFSET = 2;   // ZIL: C-RTN

// Timer callback function type
using TimerCallback = std::function<void()>;

// Structure representing an interrupt entry in C-TABLE (gclock.zil:26-40)
struct Timer {
  std::string name;       ///< Identifier/Routine name (e.g. "I-LANTERN", "I-FIGHT")
  int interval = 0;       ///< Default interval
  int counter = 0;        ///< Current countdown counter (ZIL: C-TICK)
  TimerCallback callback; ///< Function to execute (ZIL: C-RTN)
  bool enabled = false;   ///< Enabled status (ZIL: C-ENABLED?)
  bool repeating = true;  ///< Does timer reset or fire once?
  bool isDemon = false;   ///< Is this a demon (runs every turn)?

  Timer() = default;
  Timer(std::string_view name_, int interval_, TimerCallback callback_,
        bool repeating_ = true, bool isDemon_ = false)
      : name(name_), interval(interval_), counter(interval_),
        callback(std::move(callback_)), enabled(true), repeating(repeating_),
        isDemon(isDemon_) {}
};

class TimerManager {
public:
  static TimerManager &instance();

  // ZIL: <ROUTINE INT (RTN "OPTIONAL" (DEMON <>) E C INT) ...> (gclock.zil:26-40)
  Timer *interrupt(std::string_view name, bool demon = false,
                   TimerCallback callback = nullptr);

  // ZIL: <ROUTINE QUEUE (RTN TICK "AUX" CINT) ...> (gclock.zil:21-24)
  Timer *queue(std::string_view name, int tick);

  // ZIL: <ROUTINE CLOCKER ("AUX" C E TICK (FLG <>)) ...> (gclock.zil:43-61)
  bool clocker();

  // Timer configuration and state control
  void registerTimer(std::string_view name, int interval,
                     TimerCallback callback, bool repeating = true,
                     bool isDemon = false);
  void enableTimer(std::string_view name);
  void disableTimer(std::string_view name);
  bool isTimerEnabled(std::string_view name) const;
  void resetTimer(std::string_view name);
  void queueTimer(std::string_view name, int ticks);
  void clear();

  // Serialization helpers
  size_t getTimerCount() const { return timers_.size(); }
  const std::unordered_map<std::string, Timer> &getAllTimers() const {
    return timers_;
  }
  void setTimerState(std::string_view name, bool enabled, int counter);

private:
  TimerManager() = default;
  TimerManager(const TimerManager &) = delete;
  TimerManager &operator=(const TimerManager &) = delete;

  std::unordered_map<std::string, Timer> timers_;
  std::vector<std::string> timerOrder_; // Preserves registration order in C-TABLE
};

// Convenience free functions mirroring ZIL routines and existing APIs

// ZIL: <ROUTINE INT ...>
inline Timer *interrupt(std::string_view name, bool demon = false,
                        TimerCallback callback = nullptr) {
  return TimerManager::instance().interrupt(name, demon, std::move(callback));
}

// ZIL: <ROUTINE QUEUE ...>
inline Timer *queue(std::string_view name, int tick) {
  return TimerManager::instance().queue(name, tick);
}

// ZIL: <ROUTINE CLOCKER ...>
inline bool clocker() { return TimerManager::instance().clocker(); }

// Aliased to clocker() for turn execution
inline bool tick() { return clocker(); }

inline void registerTimer(std::string_view name, int interval,
                          TimerCallback callback, bool repeating = true,
                          bool isDemon = false) {
  TimerManager::instance().registerTimer(name, interval, std::move(callback),
                                         repeating, isDemon);
}

inline void enableTimer(std::string_view name) {
  TimerManager::instance().enableTimer(name);
}

inline void disableTimer(std::string_view name) {
  TimerManager::instance().disableTimer(name);
}

inline bool isTimerEnabled(std::string_view name) {
  return TimerManager::instance().isTimerEnabled(name);
}

inline void resetTimer(std::string_view name) {
  TimerManager::instance().resetTimer(name);
}

inline void queueTimer(std::string_view name, int ticks) {
  TimerManager::instance().queueTimer(name, ticks);
}

inline void clear() { TimerManager::instance().clear(); }

} // namespace TimerSystem
