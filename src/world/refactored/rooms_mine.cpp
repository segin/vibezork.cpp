// Coal mine rooms - confusing maze-like area
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"

void initializeMineRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // MINE_1 - First mine room
    createRoom({
        .id = MINE_1,
        .name = "Coal Mine",
        .longDesc = "This is a nondescript part of a coal mine.",
        .exits = {
            {NORTH, GAS_ROOM},
            {EAST, MINE_1},   // Loops to itself
            {NE, MINE_2}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MINE_2 - Second mine room
    createRoom({
        .id = MINE_2,
        .name = "Coal Mine",
        .longDesc = "This is a nondescript part of a coal mine.",
        .exits = {
            {NORTH, MINE_2},  // Loops to itself
            {SOUTH, MINE_1},
            {SE, MINE_3}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MINE_3 - Third mine room
    createRoom({
        .id = MINE_3,
        .name = "Coal Mine",
        .longDesc = "This is a nondescript part of a coal mine.",
        .exits = {
            {SOUTH, MINE_3},  // Loops to itself
            {SW, MINE_4},
            {EAST, MINE_2}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MINE_4 - Fourth mine room (exit to ladder)
    createRoom({
        .id = MINE_4,
        .name = "Coal Mine",
        .longDesc = "This is a nondescript part of a coal mine.",
        .exits = {
            {NORTH, MINE_3},
            {WEST, MINE_4},   // Loops to itself
            {DOWN, LADDER_TOP}
        },
        .flags = {RLANDBIT}  // Dark
    });
}
