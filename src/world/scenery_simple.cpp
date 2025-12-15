#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include "world/objects.h"

// WHITE-HOUSE-F - White house exterior scenery
// ZIL: Handles KNOCK, OPEN, READ (prevents actions on house)
// Source: 1actions.zil lines 92-99
bool whiteHouseAction() {
  auto &g = Globals::instance();

  // KNOCK on house
  if (g.prsa == V_KNOCK) {
    printLine("You hear no answer.");
    return RTRUE;
  }

  // OPEN house (can't)
  if (g.prsa == V_OPEN) {
    printLine("The door is boarded and you can't remove the boards.");
    return RTRUE;
  }

  // READ house (silly)
  if (g.prsa == V_READ) {
    printLine("How can you read a house?");
    return RTRUE;
  }

  return RFALSE;
}
