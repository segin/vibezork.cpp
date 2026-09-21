/**
 * @file timer.cpp
 * @brief GCLOCK interrupt system implementation
 *
 * Source: zil/gclock.zil:1-60
 */

#include "timer.h"
#include "core/globals.h"
#include <stdexcept>

namespace TimerSystem {

TimerManager &TimerManager::instance() {
  static TimerManager instance;
  return instance;
}

Interrupt *TimerManager::find(std::string_view name) {
  for (int i = cInts_; i < C_MAX_INTERRUPTS; ++i) {
    if (table_[i].name == name) {
      return &table_[i];
    }
  }
  return nullptr;
}

const Interrupt *TimerManager::find(std::string_view name) const {
  for (int i = cInts_; i < C_MAX_INTERRUPTS; ++i) {
    if (table_[i].name == name) {
      return &table_[i];
    }
  }
  return nullptr;
}

// ZIL: <ROUTINE INT (RTN "OPTIONAL" (DEMON <>) E C INT)
//        <SET E <REST ,C-TABLE ,C-TABLELEN>>
//        <SET C <REST ,C-TABLE ,C-INTS>>
//        <REPEAT ()
//          <COND (<==? .C .E>
//                 <SETG C-INTS <- ,C-INTS ,C-INTLEN>>
//                 <SET INT <REST ,C-TABLE ,C-INTS>>
//                 <PUT .INT ,C-RTN .RTN>
//                 <RETURN .INT>)
//                (<EQUAL? <GET .C ,C-RTN> .RTN> <RETURN .C>)>
//          <SET C <REST .C ,C-INTLEN>>>>
// Source: zil/gclock.zil:26-39
Interrupt *TimerManager::interrupt(std::string_view name, InterruptRoutine routine) {
  if (Interrupt *existing = find(name)) {
    if (routine) {
      existing->routine = std::move(routine);
    }
    return existing;
  }
  if (cInts_ == 0) {
    throw std::runtime_error("C-TABLE overflow: more than 30 interrupts");
  }
  --cInts_;
  Interrupt &entry = table_[cInts_];
  entry.name = std::string(name);
  entry.enabled = false; // ZIL: fresh table words are 0
  entry.tick = 0;
  entry.routine = std::move(routine);
  return &entry;
}

// ZIL: <ROUTINE QUEUE (RTN TICK "AUX" CINT)
//        <PUT <SET CINT <INT .RTN>> ,C-TICK .TICK>
//        .CINT>
// Source: zil/gclock.zil:21-24
Interrupt *TimerManager::queue(std::string_view name, int tick) {
  Interrupt *cint = interrupt(name);
  cint->tick = tick;
  return cint;
}

// ZIL: <DEFMAC ENABLE ('INT) <FORM PUT .INT ,C-ENABLED? 1>> (gmacros.zil:141)
void TimerManager::enable(std::string_view name) {
  interrupt(name)->enabled = true;
}

// ZIL: <DEFMAC DISABLE ('INT) <FORM PUT .INT ,C-ENABLED? 0>> (gmacros.zil:143)
void TimerManager::disable(std::string_view name) {
  interrupt(name)->enabled = false;
}

bool TimerManager::isEnabled(std::string_view name) const {
  const Interrupt *cint = find(name);
  return cint && cint->enabled;
}

// ZIL: <ROUTINE CLOCKER ("AUX" C E TICK (FLG <>))
//        <COND (,CLOCK-WAIT <SETG CLOCK-WAIT <>> <RFALSE>)>
//        <SET C <REST ,C-TABLE <COND (,P-WON ,C-INTS) (T ,C-DEMONS)>>>
//        <SET E <REST ,C-TABLE ,C-TABLELEN>>
//        <REPEAT ()
//          <COND (<==? .C .E>
//                 <SETG MOVES <+ ,MOVES 1>>
//                 <RETURN .FLG>)
//                (<NOT <0? <GET .C ,C-ENABLED?>>>
//                 <SET TICK <GET .C ,C-TICK>>
//                 <COND (<0? .TICK>)
//                       (T
//                        <PUT .C ,C-TICK <- .TICK 1>>
//                        <COND (<AND <NOT <G? .TICK 1>>
//                                    <APPLY <GET .C ,C-RTN>>>
//                               <SET FLG T>)>)>)>
//          <SET C <REST .C ,C-INTLEN>>>>
// Source: zil/gclock.zil:43-60. CLOCKER is only ever called with P-WON
// true (gmain.zil:169-172), so the C-DEMONS branch is unreachable and the
// scan always starts at C-INTS.
bool TimerManager::clocker() {
  auto &g = Globals::instance();

  if (g.clockWait) {
    g.clockWait = false;
    return false;
  }

  bool flg = false;
  for (int i = cInts_; i < C_MAX_INTERRUPTS; ++i) {
    Interrupt &c = table_[i];
    if (!c.enabled) {
      continue;
    }
    int tick = c.tick;
    if (tick == 0) {
      continue;
    }
    c.tick = tick - 1;
    if (!(tick > 1) && c.routine && c.routine()) {
      flg = true;
    }
  }

  // ZIL: <SETG MOVES <+ ,MOVES 1>> once the scan reaches the end of C-TABLE
  g.moves++;
  return flg;
}

std::vector<const Interrupt *> TimerManager::entries() const {
  std::vector<const Interrupt *> result;
  for (int i = cInts_; i < C_MAX_INTERRUPTS; ++i) {
    result.push_back(&table_[i]);
  }
  return result;
}

void TimerManager::setInterruptState(std::string_view name, bool enabled, int tick) {
  Interrupt *cint = interrupt(name);
  cint->enabled = enabled;
  cint->tick = tick;
}

void TimerManager::clear() {
  for (auto &entry : table_) {
    entry = Interrupt{};
  }
  cInts_ = C_MAX_INTERRUPTS;
}

} // namespace TimerSystem
