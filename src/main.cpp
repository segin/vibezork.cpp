#include "core/globals.h"
#include "core/gmain.h"
#include "core/io.h"
#include "parser/parser.h"
#include "systems/candle.h"
#include "systems/lamp.h"
#include "systems/npc.h"
#include "systems/score.h"
#include "systems/sword.h"
#include "systems/timer.h"
#include "verbs/verbs.h"
#include "world/world.h"
#include <cstdlib>
#include <iostream>
#include <print>

void initializeGame() {
  initializeWorld();
  initializeAllVerbHandlers();
  NPCSystem::initializeThief();
  NPCSystem::initializeTroll();
  NPCSystem::initializeCyclops();
  LampSystem::initialize();   // Initialize lamp timer (Requirement 47)
  CandleSystem::initialize(); // Initialize candle timer (Requirement 48)
  SwordSystem::initialize();  // Initialize sword glow timer (Requirement 49)
}

// ZIL: <ROUTINE GO () ...> (1dungeon.zil:2637-2661)
void go() {
  auto &g = Globals::instance();

  // ZIL: <PUTP ,INFLATED-BOAT ,P?VTYPE ,NONLANDBIT>
  if (auto *boat = g.getObject(ObjectIds::BOAT_INFLATED)) {
    boat->setFlag(ObjectFlag::NONLANDBIT);
  }

  // ZIL: <SETG HERE ,WEST-OF-HOUSE>
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);

  // ZIL: <THIS-IS-IT ,MAILBOX>
  g.it = g.getObject(ObjectIds::MAILBOX);

  // ZIL: <COND (<NOT <FSET? ,HERE ,TOUCHBIT>> <V-VERSION> <CRLF>)>
  if (g.here && !g.here->hasFlag(ObjectFlag::TOUCHBIT)) {
    Verbs::vVersion();
    std::println();
  }

  // ZIL: <SETG LIT T>
  g.lit = true;

  // ZIL: <SETG WINNER ,ADVENTURER>
  // ZIL: <SETG PLAYER ,WINNER>
  // ZIL: <MOVE ,WINNER ,HERE>
  if (!g.player) {
    g.player = g.getObject(ObjectIds::ADVENTURER);
  }
  g.winner = g.player;
  if (g.winner && g.here) {
    g.winner->moveTo(g.here);
  }

  // ZIL: <V-LOOK>
  Verbs::vLook();

  // ZIL: <MAIN-LOOP>
  mainLoop();
}

int main() {
  initializeGame();
  go();
  return 0;
}
