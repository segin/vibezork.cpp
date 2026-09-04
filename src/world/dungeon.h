// Dungeon routines, tables, and constants ported from zil/1dungeon.zil
#pragma once

#include "core/types.h"
#include <string_view>
#include <span>

namespace Dungeon {

// ZIL: SCORE-MAX (Maximum possible score in game is 350)
// Source: zil/1dungeon.zil:7
constexpr int SCORE_MAX = 350;

// ZIL: FALSE-FLAG (Always false flag used in impossible exits)
// Source: zil/1dungeon.zil:9
constexpr bool FALSE_FLAG = false;

// ZIL: <ROUTINE TREASURE-INSIDE () ...>
// Action handler for BUOY when opened: awards points for finding the emerald.
// Source: zil/1dungeon.zil:793-796
bool treasureInsideAction();

// ZIL: <ROUTINE GRATING-EXIT () ...>
// Procedural exit handler for GRATING-CLEARING (DOWN PER GRATING-EXIT).
// Source: zil/1dungeon.zil:1400-1408
ObjectId gratingExit();

// ZIL: <ROUTINE TRAP-DOOR-EXIT () ...>
// Procedural exit handler for LIVING-ROOM (DOWN PER TRAP-DOOR-EXIT).
// Source: zil/1actions.zil:567-577, zil/1dungeon.zil:1456
ObjectId trapDoorExit();

// ZIL: <ROUTINE UP-CHIMNEY-FUNCTION () ...>
// Procedural exit handler for STUDIO (UP PER UP-CHIMNEY-FUNCTION).
// Source: zil/1actions.zil:553-565, zil/1dungeon.zil:1529
ObjectId upChimneyFunction();

// ZIL: <ROUTINE MAZE-DIODES () ...>
// Procedural exit handler for MAZE rooms with one-way downward tunnels.
// Source: zil/1actions.zil:898-905, zil/1dungeon.zil:1553, 1615, 1641, 1681
ObjectId mazeDiodes();

// ZIL: <ROUTINE CANYON-VIEW-F (RARG) ...>
// Action handler for CANYON-VIEW room: leaps result in fatal fall.
// Source: zil/1dungeon.zil:2406-2411
void canyonViewRoomAction(int rarg);

// ZIL: Random walk tables for walk-around and navigation
// Source: zil/1dungeon.zil:2620-2633
extern const ObjectId HOUSE_AROUND[5];
extern const ObjectId FOREST_AROUND[6];
extern const ObjectId IN_HOUSE_AROUND[4];
extern const ObjectId ABOVE_GROUND[11];

} // namespace Dungeon
