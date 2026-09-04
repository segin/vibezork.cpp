// Dungeon routines and tables ported from zil/1dungeon.zil
#include "dungeon.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "verbs/verbs.h"
#include "systems/death.h"
#include <format>
#include <iostream>

namespace Dungeon {

// ZIL: <ROUTINE TREASURE-INSIDE ()
// 	 <COND (<VERB? OPEN>
// 		<SCORE-OBJ ,EMERALD>
// 		<RFALSE>)>>
// Source: zil/1dungeon.zil:793-796
bool treasureInsideAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN) {
        if (auto* emerald = g.getObject(ObjectIds::EMERALD)) {
            Verbs::scoreObj(emerald);
        }
        return false; // Return RFALSE to allow standard OPEN action
    }
    return false;
}

// ZIL: <ROUTINE GRATING-EXIT ()
// 	 <COND (,GRATE-REVEALED
// 		<COND (<FSET? ,GRATE ,OPENBIT>
// 		       ,GRATING-ROOM)
// 		      (T
// 		       <TELL "The grating is closed!" CR>
// 		       <THIS-IS-IT ,GRATE>
// 		       <RFALSE>)>)
// 	       (T <TELL "You can't go that way." CR> <RFALSE>)>>
// Source: zil/1dungeon.zil:1400-1408
ObjectId gratingExit() {
    auto& g = Globals::instance();
    if (g.grateRevealed) {
        auto* grate = g.getObject(ObjectIds::GRATE);
        if (grate && grate->hasFlag(ObjectFlag::OPENBIT)) {
            return RoomIds::GRATING_ROOM;
        } else {
            printLine("The grating is closed!");
            if (grate) {
                g.pItObject = grate;
            }
            return 0;
        }
    } else {
        printLine("You can't go that way.");
        return 0;
    }
}

// ZIL: <ROUTINE TRAP-DOOR-EXIT ()
// 	 <COND (,RUG-MOVED
// 		<COND (<FSET? ,TRAP-DOOR ,OPENBIT>
// 		       <RETURN ,CELLAR>)
// 		      (T
// 		       <TELL "The trap door is closed." CR>
// 		       <THIS-IS-IT ,TRAP-DOOR>
// 		       <RFALSE>)>)
// 	       (T
// 		<TELL "You can't go that way." CR>
// 		<RFALSE>)>>
// Source: zil/1actions.zil:567-577, zil/1dungeon.zil:1456
ObjectId trapDoorExit() {
    auto& g = Globals::instance();
    if (g.rugMoved) {
        auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
        if (trapDoor && trapDoor->hasFlag(ObjectFlag::OPENBIT)) {
            return RoomIds::CELLAR;
        } else {
            printLine("The trap door is closed.");
            if (trapDoor) {
                g.pItObject = trapDoor;
            }
            return 0;
        }
    } else {
        printLine("You can't go that way.");
        return 0;
    }
}

// ZIL: <ROUTINE UP-CHIMNEY-FUNCTION ("AUX" F)
//   <COND (<NOT <SET F <FIRST? ,WINNER>>>
// 	 <TELL "Going up empty-handed is a bad idea." CR>
// 	 <RFALSE>)
// 	(<AND <OR <NOT <SET F <NEXT? .F>>>
// 		  <NOT <NEXT? .F>>>
// 	      <IN? ,LAMP ,WINNER>>
// 	 <COND (<NOT <FSET? ,TRAP-DOOR ,OPENBIT>>
// 		<FCLEAR ,TRAP-DOOR ,TOUCHBIT>)>
// 	 <RETURN ,KITCHEN>)
// 	(T
// 	 <TELL "You can't get up there with what you're carrying." CR>
// 	 <RFALSE>)>>
// Source: zil/1actions.zil:553-565, zil/1dungeon.zil:1529
ObjectId upChimneyFunction() {
    auto& g = Globals::instance();
    if (!g.winner) return 0;
    const auto& contents = g.winner->getContents();
    if (contents.empty()) {
        printLine("Going up empty-handed is a bad idea.");
        return 0;
    }
    // In ZIL: exactly 1 item and it must be the lamp
    bool hasLamp = false;
    for (auto* item : contents) {
        if (item && item->getId() == ObjectIds::LAMP) {
            hasLamp = true;
            break;
        }
    }
    if (contents.size() == 1 && hasLamp) {
        auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
        if (trapDoor && !trapDoor->hasFlag(ObjectFlag::OPENBIT)) {
            trapDoor->clearFlag(ObjectFlag::TOUCHBIT);
        }
        return RoomIds::KITCHEN;
    }
    printLine("You can't get up there with what you're carrying.");
    return 0;
}

// ZIL: <ROUTINE MAZE-DIODES ()
// 	 <TELL
// "You won't be able to get back up to the tunnel you are going through
// when it gets to the next room." CR CR>
// 	 <COND (<EQUAL? ,HERE ,MAZE-2> ,MAZE-4)
// 	       (<EQUAL? ,HERE ,MAZE-7> ,DEAD-END-1)
// 	       (<EQUAL? ,HERE ,MAZE-9> ,MAZE-11)
// 	       (<EQUAL? ,HERE ,MAZE-12> ,MAZE-5)>>
// Source: zil/1actions.zil:898-905, zil/1dungeon.zil:1553, 1615, 1641, 1681
ObjectId mazeDiodes() {
    auto& g = Globals::instance();
    printLine("You won't be able to get back up to the tunnel you are going through when it gets to the next room.\n");
    if (!g.here) return 0;
    switch (g.here->getId()) {
        case RoomIds::MAZE_2: return RoomIds::MAZE_4;
        case RoomIds::MAZE_7: return RoomIds::DEAD_END_1;
        case RoomIds::MAZE_9: return RoomIds::MAZE_11;
        case RoomIds::MAZE_12: return RoomIds::MAZE_5;
        default: return 0;
    }
}

// ZIL: <ROUTINE CANYON-VIEW-F (RARG)
// 	 <COND (<AND <==? .RARG ,M-BEG>
// 		     <VERB? LEAP>
// 		     <NOT ,PRSO>>
// 		<JIGS-UP "Nice view, lousy place to jump.">
// 		<RTRUE>)>>
// Source: zil/1dungeon.zil:2406-2411
void canyonViewRoomAction(int rarg) {
    auto& g = Globals::instance();
    if (rarg == M_BEG && g.prsa == V_LEAP && !g.prso) {
        DeathSystem::jigsUp("Nice view, lousy place to jump.");
    }
}

// ZIL: RANDOM TABLES FOR WALK-AROUND (1dungeon.zil:2620-2633)
// Source: zil/1dungeon.zil:2620-2633
const ObjectId HOUSE_AROUND[5] = {
    ROOM_WEST_OF_HOUSE, ROOM_NORTH_OF_HOUSE,
    ROOM_EAST_OF_HOUSE, ROOM_SOUTH_OF_HOUSE,
    ROOM_WEST_OF_HOUSE
};

const ObjectId FOREST_AROUND[6] = {
    RoomIds::FOREST_1, RoomIds::FOREST_2, RoomIds::FOREST_3,
    RoomIds::FOREST_PATH, RoomIds::CLEARING, RoomIds::FOREST_1
};

const ObjectId IN_HOUSE_AROUND[4] = {
    RoomIds::LIVING_ROOM, RoomIds::KITCHEN, RoomIds::ATTIC, RoomIds::KITCHEN
};

const ObjectId ABOVE_GROUND[11] = {
    ROOM_WEST_OF_HOUSE, ROOM_NORTH_OF_HOUSE,
    ROOM_EAST_OF_HOUSE, ROOM_SOUTH_OF_HOUSE,
    RoomIds::FOREST_1, RoomIds::FOREST_2, RoomIds::FOREST_3,
    RoomIds::FOREST_PATH, RoomIds::CLEARING, RoomIds::GRATING_CLEARING,
    RoomIds::CANYON_VIEW
};

} // namespace Dungeon
