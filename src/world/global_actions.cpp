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
