/**
 * @file timer.cpp
 * @brief GCLOCK Interrupt & Demon System Implementation
 *
 * Source: zil/gclock.zil:1-61
 */

#include "timer.h"
#include "core/globals.h"
#include <algorithm>

namespace TimerSystem {

TimerManager &TimerManager::instance() {
  static TimerManager instance;
  return instance;
}

// ZIL: <ROUTINE INT (RTN "OPTIONAL" (DEMON <>) E C INT) ...> (gclock.zil:26-40)
Timer *TimerManager::interrupt(std::string_view name, bool demon,
                               TimerCallback callback) {
  std::string key(name);
  auto it = timers_.find(key);
  if (it != timers_.end()) {
    if (callback) {
      it->second.callback = std::move(callback);
    }
    if (demon) {
      it->second.isDemon = true;
    }
    return &it->second;
  }

  // Allocate new interrupt entry in C-TABLE
  timers_[key] = Timer(name, 0, std::move(callback), true, demon);
  timerOrder_.push_back(key);
  return &timers_[key];
}

// ZIL: <ROUTINE QUEUE (RTN TICK "AUX" CINT) ...> (gclock.zil:21-24)
Timer *TimerManager::queue(std::string_view name, int tick) {
  Timer *cint = interrupt(name);
  cint->counter = tick;
  if (cint->interval == 0) {
    cint->interval = (tick > 0 ? tick : 1);
  }
  if (tick < 0) {
    cint->isDemon = true;
  }
  return cint;
}

void TimerManager::registerTimer(std::string_view name, int interval,
                                 TimerCallback callback, bool repeating,
                                 bool isDemon) {
  std::string key(name);
  auto it = timers_.find(key);
  if (it != timers_.end()) {
    it->second.interval = interval;
    it->second.counter = interval;
    it->second.callback = std::move(callback);
    it->second.repeating = repeating;
    it->second.isDemon = isDemon;
    it->second.enabled = true;
  } else {
    timers_[key] =
        Timer(name, interval, std::move(callback), repeating, isDemon);
    timerOrder_.push_back(key);
  }
}

void TimerManager::enableTimer(std::string_view name) {
  std::string key(name);
  if (auto it = timers_.find(key); it != timers_.end()) {
    it->second.enabled = true;
  } else {
    Timer *cint = interrupt(name);
    cint->enabled = true;
  }
}

void TimerManager::disableTimer(std::string_view name) {
  if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
    it->second.enabled = false;
  }
}

bool TimerManager::isTimerEnabled(std::string_view name) const {
  if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
    return it->second.enabled;
  }
  return false;
}

void TimerManager::resetTimer(std::string_view name) {
  if (auto it = timers_.find(std::string(name)); it != timers_.end()) {
    it->second.counter = it->second.interval;
  }
}

void TimerManager::queueTimer(std::string_view name, int ticks) {
  queue(name, ticks);
}

// ZIL: <ROUTINE CLOCKER ("AUX" C E TICK (FLG <>)) ...> (gclock.zil:43-61)
bool TimerManager::clocker() {
  auto &g = Globals::instance();

  // ZIL: <COND (,CLOCK-WAIT <SETG CLOCK-WAIT <>> <RFALSE>)>
  if (g.clockWait) {
    g.clockWait = false;
    return false;
  }

  bool flg = false;

  // Process entries in registration order (mirrors C-TABLE traversal)
  for (const auto &name : timerOrder_) {
    auto it = timers_.find(name);
    if (it == timers_.end()) {
      continue;
    }
    Timer &c = it->second;

    // ZIL: <COND (<NOT <0? <GET .C ,C-ENABLED?>>> ...)>
    if (!c.enabled) {
      continue;
    }

    // ZIL: Demons run even when P-WON is false; non-demons only run when P-WON is true
    // ZIL: <SET C <REST ,C-TABLE <COND (,P-WON ,C-INTS) (T ,C-DEMONS)>>>
    if (!g.pWon && g.winner != nullptr && !c.isDemon) {
      continue;
    }

    // ZIL: <COND (<0? .TICK>) (T ...)>
    if (c.counter == 0) {
      continue;
    }

    if (c.counter < 0) {
      // Continuous demon mode (tick = -1)
      if (c.callback) {
        c.callback();
        flg = true;
      }
    } else {
      // Countdown interrupt mode
      c.counter--;
      if (c.counter == 0) {
        if (c.callback) {
          c.callback();
          flg = true;
        }

        if (c.repeating) {
          c.counter = c.interval;
        } else {
          c.enabled = false;
        }
      }
    }
  }

  return flg;
}

void TimerManager::clear() {
  timers_.clear();
  timerOrder_.clear();
}

void TimerManager::setTimerState(std::string_view name, bool enabled,
                                 int counter) {
  std::string key(name);
  if (auto it = timers_.find(key); it != timers_.end()) {
    it->second.enabled = enabled;
    it->second.counter = counter;
  } else {
    Timer *cint = interrupt(name);
    cint->enabled = enabled;
    cint->counter = counter;
  }
}

} // namespace TimerSystem
