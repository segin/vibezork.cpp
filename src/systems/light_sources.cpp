#include "light_sources.h"

#include "core/globals.h"
#include "core/io.h"
#include "systems/timer.h"
#include "parser/gparser.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"

namespace LightSources {

namespace {

// ZIL: <GLOBAL LAMP-TABLE <TABLE (PURE) 100 "..." 70 "..." 15 "..." 0>>
// (1actions.zil:2216-2224).  The global is a table pointer; I-LANTERN advances
// it with <REST .TBL 4> after each step, so model it as an index.
std::size_t lampIndex = 0;

// ZIL: <GLOBAL CANDLE-TABLE <TABLE (PURE) 20 "..." 10 "..." 5 "..." 0>>
// (1actions.zil:2406-2413), advanced the same way by I-CANDLES.
std::size_t candleIndex = 0;

} // namespace

std::span<const TimerStep> lampTable() {
  std::span<const TimerStep> all{zork::zil::kLAMP_TABLE};
  if (lampIndex >= all.size())
    return all.last(1); // the terminating zero entry
  return all.subspan(lampIndex);
}

std::span<const TimerStep> candleTable() {
  std::span<const TimerStep> all{zork::zil::kCANDLE_TABLE};
  if (candleIndex >= all.size())
    return all.last(1);
  return all.subspan(candleIndex);
}

void reset() {
  lampIndex = 0;
  candleIndex = 0;
}

// ZIL: <ROUTINE LIGHT-INT (OBJ TBL TICK)
//        <COND (<0? .TICK> <FCLEAR .OBJ ,ONBIT> <FSET .OBJ ,RMUNGBIT>)>
//        <COND (<OR <HELD? .OBJ> <IN? .OBJ ,HERE>>
//               <COND (<0? .TICK>
//                      <TELL "You'd better have more light than from the "
//                            D .OBJ "." CR>)
//                     (T <TELL <GET .TBL 1> CR>)>)>>
// Source: zil/1actions.zil:2319-2330
void lightInt(ZObject *obj, std::span<const TimerStep> tbl, int tick) {
  auto &g = Globals::instance();
  if (!obj)
    return;

  if (tick == 0) {
    obj->clearFlag(ObjectFlag::ONBIT);
    obj->setFlag(ObjectFlag::RMUNGBIT);
  }

  // ZIL: <OR <HELD? .OBJ> <IN? .OBJ ,HERE>>
  if (Verbs::isHeld(obj) || obj->getLocation() == g.here) {
    if (tick == 0) {
      tell("You'd better have more light than from the ", obj->getDesc(), ".",
           CR);
    } else if (!tbl.empty()) {
      tell(tbl.front().text, CR);
    }
  }

  // The light going out can darken the room.
  g.lit = GParser::isLit(g.here);
}

// ZIL: <ROUTINE I-LANTERN ("AUX" TICK (TBL <VALUE LAMP-TABLE>))
//        <ENABLE <QUEUE I-LANTERN <SET TICK <GET .TBL 0>>>>
//        <LIGHT-INT ,LAMP .TBL .TICK>
//        <COND (<NOT <0? .TICK>> <SETG LAMP-TABLE <REST .TBL 4>>)>>
// Source: zil/1actions.zil:2303-2308
bool iLantern() {
  auto &g = Globals::instance();
  std::span<const TimerStep> tbl = lampTable();
  int tick = tbl.empty() ? 0 : tbl.front().turns;

  TimerSystem::queue("I-LANTERN", tick);
  TimerSystem::enable("I-LANTERN");

  lightInt(g.getObject(ObjectIds::LAMP), tbl, tick);

  if (tick != 0)
    ++lampIndex;

  // ZIL: the routine's value is whatever the final COND yields; a non-zero
  // tick leaves the SETG's value (true), a zero tick falls out false.  Only
  // CLOCKER's FLG depends on it, which V-WAIT uses to stop waiting early.
  return tick != 0;
}

} // namespace LightSources

// ZIL: <ROUTINE I-CANDLES ("AUX" TICK (TBL <VALUE CANDLE-TABLE>))
//        <FSET ,CANDLES ,TOUCHBIT>
//        <ENABLE <QUEUE I-CANDLES <SET TICK <GET .TBL 0>>>>
//        <LIGHT-INT ,CANDLES .TBL .TICK>
//        <COND (<NOT <0? .TICK>> <SETG CANDLE-TABLE <REST .TBL 4>>)>>
// Source: zil/1actions.zil:2310-2317
namespace LightSources {

bool iCandles() {
  auto &g = Globals::instance();
  ZObject *candles = g.getObject(ObjectIds::CANDLES);
  if (candles)
    candles->setFlag(ObjectFlag::TOUCHBIT);

  std::span<const TimerStep> tbl = candleTable();
  int tick = tbl.empty() ? 0 : tbl.front().turns;

  TimerSystem::queue("I-CANDLES", tick);
  TimerSystem::enable("I-CANDLES");

  lightInt(candles, tbl, tick);

  if (tick != 0)
    ++candleIndex;

  return tick != 0;
}

} // namespace LightSources

namespace LightSources {

// ZIL: <ROUTINE INFESTED? (R "AUX" (F <FIRST? .R>))
//        <REPEAT () <COND (<NOT .F> <RFALSE>)
//                         (<AND <FSET? .F ,ACTORBIT>
//                               <NOT <FSET? .F ,INVISIBLE>>> <RTRUE>)
//                         (<NOT <SET F <NEXT? .F>>> <RFALSE>)>>>
// Source: zil/1actions.zil:3880-3886
bool infested(ZObject *room) {
  if (!room)
    return false;
  for (const ZObject *o : room->getContents()) {
    if (o->hasFlag(ObjectFlag::ACTORBIT) && !o->hasFlag(ObjectFlag::INVISIBLE))
      return true;
  }
  return false;
}

// ZIL: <ROUTINE I-SWORD ...>
// Source: zil/1actions.zil:3851-3878
//
// Two glows: brightly (TVALUE 2) when a villain shares the room, faintly
// (TVALUE 1) when one stands through a UEXIT, CEXIT or DEXIT exit.  The
// interrupt disables itself as soon as the sword is not carried, and it
// speaks only when the glow actually changes.
bool iSword() {
  auto &g = Globals::instance();
  ZObject *sword = g.getObject(ObjectIds::SWORD);
  ZObject *adventurer = g.getObject(ObjectIds::ADVENTURER);

  if (!sword || !adventurer || sword->getLocation() != adventurer) {
    // ZIL: <PUT .DEM ,C-ENABLED? 0> <RFALSE>
    TimerSystem::disable("I-SWORD");
    return false;
  }

  int glow = sword->getProperty(P_TVALUE);
  int next = 0;

  if (infested(g.here)) {
    next = 2;
  } else if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
    // ZIL walks HERE's property table and only considers the exit kinds that
    // name a destination room: UEXIT, CEXIT and DEXIT.
    for (const auto &[dir, exit] : room->getExits()) {
      (void)dir;
      if (exit.type == ExitType::PROCEDURAL || exit.type == ExitType::SPECIAL)
        continue;
      if (exit.targetRoom == 0)
        continue; // NEXIT: a refusal string, no room behind it
      if (infested(g.getObject(exit.targetRoom))) {
        next = 1;
        break;
      }
    }
  }

  if (next == glow)
    return false;

  if (next == 2)
    printLine("Your sword has begun to glow very brightly.");
  else if (next == 1)
    printLine("Your sword is glowing with a faint blue glow.");
  else
    printLine("Your sword is no longer glowing.");

  sword->setProperty(P_TVALUE, next);
  return true;
}

} // namespace LightSources
