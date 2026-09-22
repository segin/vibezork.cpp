/**
 * @file go.cpp
 * @brief Game start-up (mirrors ZIL GO, 1dungeon.zil:2637-2661)
 *
 * Source: zil/1dungeon.zil:2637-2661
 */

#include "go.h"
#include "core/globals.h"
#include "core/gmain.h"
#include "core/io.h"
#include "systems/candle.h"
#include "systems/combat.h"
#include "systems/lamp.h"
#include "systems/melee.h"
#include "world/villains.h"
#include "systems/melee_tables.h"
#include "systems/npc.h"
#include "systems/sword.h"
#include "systems/timer.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"

void initializeGame() {
  initializeWorld();
  initializeAllVerbHandlers();
  NPCSystem::initializeThief();
  NPCSystem::initializeTroll();
  NPCSystem::initializeCyclops();
}

// ZIL: <ROUTINE GO ()
//        <ENABLE <QUEUE I-FIGHT -1>>
//        <QUEUE I-SWORD -1>
//        <ENABLE <QUEUE I-THIEF -1>>
//        <QUEUE I-CANDLES 40>
//        <QUEUE I-LANTERN 200>
//        <PUTP ,INFLATED-BOAT ,P?VTYPE ,NONLANDBIT>
//        <PUT ,DEF1-RES 1 <REST ,DEF1 2>> ... <PUT ,DEF3-RES 3 <REST ,DEF3B 2>>
//        <SETG HERE ,WEST-OF-HOUSE>
//        <THIS-IS-IT ,MAILBOX>
//        <COND (<NOT <FSET? ,HERE ,TOUCHBIT>> <V-VERSION> <CRLF>)>
//        <SETG LIT T>
//        <SETG WINNER ,ADVENTURER>
//        <SETG PLAYER ,WINNER>
//        <MOVE ,WINNER ,HERE>
//        <V-LOOK>
//        <MAIN-LOOP>
//        <AGAIN>>
// Source: zil/1dungeon.zil:2637-2661
// The banner is printed by go(); every other step of GO lives here, in GO's
// order.  V-VERSION sits between THIS-IS-IT and SETG LIT in the source and
// only prints, so hoisting it into go() leaves the output unchanged.
void goSetup() {
  auto &g = Globals::instance();

  // The QUEUE calls allocate the C-TABLE entries in this order (INT,
  // gclock.zil:26-39), which fixes the order CLOCKER runs them in.
  // ZIL: <ENABLE <QUEUE I-FIGHT -1>>
  TimerSystem::interrupt("I-FIGHT", Melee::iFight);
  TimerSystem::queue("I-FIGHT", -1);
  TimerSystem::enable("I-FIGHT");

  // ZIL: <QUEUE I-SWORD -1>  (not enabled until SWORD-FCN, 1actions.zil:2434)
  TimerSystem::interrupt("I-SWORD", SwordSystem::iSword);
  TimerSystem::queue("I-SWORD", -1);

  // ZIL: <ENABLE <QUEUE I-THIEF -1>>
  TimerSystem::interrupt("I-THIEF", Villains::iThief);
  TimerSystem::queue("I-THIEF", -1);
  TimerSystem::enable("I-THIEF");

  // ZIL: <QUEUE I-CANDLES 40>  (enabled by CANDLES-FCN, 1actions.zil:2345)
  TimerSystem::interrupt("I-CANDLES", CandleSystem::iCandles);
  TimerSystem::queue("I-CANDLES", 40);

  // ZIL: <QUEUE I-LANTERN 200>  (enabled by LANTERN, 1actions.zil:2241)
  TimerSystem::interrupt("I-LANTERN", LampSystem::iLantern);
  TimerSystem::queue("I-LANTERN", 200);

  // ZIL: <PUTP ,INFLATED-BOAT ,P?VTYPE ,NONLANDBIT>
  if (auto *boat = g.getObject(ObjectIds::BOAT_INFLATED)) {
    boat->setVehicleType(ObjectFlag::NONLANDBIT);
  }

  // ZIL: the six <PUT ,DEFn-RES i <REST ,DEFx k>> forms
  Melee::patchDefRes();

  // ZIL: <SETG HERE ,WEST-OF-HOUSE>
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);

  // ZIL: <THIS-IS-IT ,MAILBOX>
  g.it = g.getObject(ObjectIds::MAILBOX);

  // ZIL: <SETG LIT T>
  g.lit = true;

  // ZIL: <SETG WINNER ,ADVENTURER> <SETG PLAYER ,WINNER> <MOVE ,WINNER ,HERE>
  g.winner = g.getObject(ObjectIds::ADVENTURER);
  g.player = g.winner;
  if (g.winner && g.here) {
    g.winner->moveTo(g.here);
  }

}

void go() {
  auto &g = Globals::instance();

  goSetup();

  // ZIL: <COND (<NOT <FSET? ,HERE ,TOUCHBIT>> <V-VERSION> <CRLF>)>
  if (g.here && !g.here->hasFlag(ObjectFlag::TOUCHBIT)) {
    Verbs::vVersion();
    crlf();
  }

  // ZIL: <V-LOOK>
  Verbs::vLook();

  // ZIL: <MAIN-LOOP>
  mainLoop();
}
