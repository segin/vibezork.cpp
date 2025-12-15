#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include "world/objects.h"

// SANDWICH-BAG-FCN - Lunch bag container
// ZIL: Handles SMELL when LUNCH is inside
// Source: 1actions.zil lines 3106-3109
bool sandwichBagAction() {
  auto &g = Globals::instance();

  // Handle SMELL when lunch is in bag
  if (g.prsa == V_SMELL && g.prso) {
    // Check if LUNCH is in the bag
    ZObject *lunch = g.getObject(ObjectIds::LUNCH);
    if (lunch && lunch->getLocation() == g.prso) {
      printLine("It smells of hot peppers.");
      return RTRUE;
    }
  }

  return RFALSE;
}

// TRUNK-F - Trunk stupid container
// ZIL: STUPID-CONTAINER - blocks OPEN, CLOSE, EXAMINE shows contents
// Source: 1actions.zil lines 4140-4152
bool trunkAction() {
  auto &g = Globals::instance();

  // STUPID-CONTAINER pattern: OPEN prints message
  if (g.prsa == V_OPEN) {
    printLine("The jewels are safely inside; there's no need to do that.");
    return RTRUE;
  }

  // CLOSE
  if (g.prsa == V_CLOSE) {
    printLine("The jewels are safely inside; there's no need to do that.");
    return RTRUE;
  }

  // LOOK-INSIDE or EXAMINE shows contents
  if (g.prsa == V_LOOK_INSIDE || g.prsa == V_EXAMINE) {
    printLine("There are lots of jewels in there.");
    return RTRUE;
  }

  // PUT into trunk - silly message
  if (g.prsa == V_PUT && g.prsi && g.prsi->getId() == ObjectIds::TRUNK) {
    printLine("Don't be silly. It wouldn't be a trunk anymore.");
    return RTRUE;
  }

  return RFALSE;
}
