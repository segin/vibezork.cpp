#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"

// CANYON-VIEW-F - Canyon view scenery
// ZIL: Handles CROSS to prevent crossing rainbow from here
// Source: 1actions.zil lines 2652-2659
bool canyonViewAction() {
  auto &g = Globals::instance();

  // CROSS - can't cross rainbow from canyon view
  if (g.prsa == V_CROSS) {
    printLine("From here?!?");
    return RTRUE;
  }

  return RFALSE;
}

// NULL-F - No-op action
// ZIL: Always returns RFALSE
// Source: gglobals.zil lines 85-86
bool nullAction() { return RFALSE; }

// STAIRS-F - Stairs navigation
// ZIL: Handles THROUGH to prompt for direction
// Source: gglobals.zil lines 110-113
bool stairsAction() {
  auto &g = Globals::instance();

  // THROUGH - need to specify up or down
  if (g.prsa == V_THROUGH) {
    printLine("You should say whether you want to go up or down.");
    return RTRUE;
  }

  return RFALSE;
}
