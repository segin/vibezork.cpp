// Exterior rooms - house surroundings and forest areas
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"

// Forward declarations for room action handlers
void westHouseAction(int rarg);
void northHouseAction(int rarg);
void southHouseAction(int rarg);
void behindHouseAction(int rarg);
void stoneBarrowAction(int rarg);

void initializeExteriorRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // WEST_OF_HOUSE - Starting location
    createRoom({
        .id = WEST_OF_HOUSE,
        .name = "West of House",
        .longDesc = "You are standing in an open field west of a white house, "
                    "with a boarded front door.",
        .exits = {
            {NORTH, NORTH_OF_HOUSE},
            {SOUTH, SOUTH_OF_HOUSE},
            {NE, NORTH_OF_HOUSE},
            {SE, SOUTH_OF_HOUSE},
            {WEST, FOREST_1}
        },
        .blockedExits = {
            {EAST, "The door is boarded and you can't remove the boards."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT},
        .action = westHouseAction
    });

    
    // NORTH_OF_HOUSE
    createRoom({
        .id = NORTH_OF_HOUSE,
        .name = "North of House",
        .longDesc = "You are facing the north side of a white house. There is no door "
                    "here, and all the windows are boarded up. To the north a narrow "
                    "path winds through the trees.",
        .exits = {
            {SW, WEST_OF_HOUSE},
            {SE, BEHIND_HOUSE},
            {WEST, WEST_OF_HOUSE},
            {EAST, BEHIND_HOUSE},
            {NORTH, FOREST_PATH}
        },
        .blockedExits = {
            {SOUTH, "The windows are all boarded."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT},
        .action = northHouseAction
    });
    
    // SOUTH_OF_HOUSE
    createRoom({
        .id = SOUTH_OF_HOUSE,
        .name = "South of House",
        .longDesc = "You are facing the south side of a white house. There is no door "
                    "here, and all the windows are boarded.",
        .exits = {
            {WEST, WEST_OF_HOUSE},
            {EAST, BEHIND_HOUSE},
            {NE, BEHIND_HOUSE},
            {NW, WEST_OF_HOUSE},
            {SOUTH, FOREST_3}
        },
        .blockedExits = {
            {NORTH, "The windows are all boarded."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT},
        .action = southHouseAction
    });

    
    // BEHIND_HOUSE (EAST_OF_HOUSE in ZIL)
    createRoom({
        .id = BEHIND_HOUSE,
        .name = "Behind House",
        .longDesc = "You are behind the white house. A path leads into the forest to "
                    "the east. In one corner of the house there is a small window "
                    "which is slightly ajar.",
        .exits = {
            {NORTH, NORTH_OF_HOUSE},
            {SOUTH, SOUTH_OF_HOUSE},
            {SW, SOUTH_OF_HOUSE},
            {NW, NORTH_OF_HOUSE},
            {EAST, CLEARING},
            {WEST, KITCHEN},  // Through window
            {IN, KITCHEN}     // Through window
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT},
        .action = behindHouseAction
    });
    
    // STONE_BARROW - End game area entrance
    createRoom({
        .id = STONE_BARROW,
        .name = "Stone Barrow",
        .longDesc = "You are standing in front of a massive barrow of stone. In the "
                    "east face is a huge stone door which is open. You cannot see "
                    "into the dark of the tomb.",
        .exits = {
            {WEST, WEST_OF_HOUSE},
            {SW, WEST_OF_HOUSE}
        },
        .flags = {RLANDBIT, ONBIT},
        .action = stoneBarrowAction
    });

    
    // CLEARING - East of house
    createRoom({
        .id = CLEARING,
        .name = "Clearing",
        .longDesc = "You are in a clearing, with a forest surrounding you on all sides. "
                    "A path leads south.",
        .exits = {
            {WEST, BEHIND_HOUSE},
            {SOUTH, FOREST_PATH},
            {EAST, CANYON_VIEW},
            {NORTH, FOREST_2}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });
    
    // FOREST_PATH - Path through forest
    createRoom({
        .id = FOREST_PATH,
        .name = "Forest Path",
        .longDesc = "This is a path winding through a dimly lit forest. The path heads "
                    "north-south here. One particularly large tree with some low branches "
                    "stands at the edge of the path.",
        .exits = {
            {NORTH, CLEARING},
            {SOUTH, NORTH_OF_HOUSE},
            {EAST, FOREST_1},
            {WEST, FOREST_1},
            {UP, UP_A_TREE}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });

    
    // UP_A_TREE - In the tree
    createRoom({
        .id = UP_A_TREE,
        .name = "Up a Tree",
        .longDesc = "You are about 10 feet above the ground nestled among some large "
                    "branches. The nearest branch above you is above your reach.",
        .exits = {
            {DOWN, FOREST_PATH}
        },
        .blockedExits = {
            {UP, "You cannot climb any higher."}
        },
        .flags = {RLANDBIT, ONBIT}
    });
    
    // FOREST_1 - Forest area
    createRoom({
        .id = FOREST_1,
        .name = "Forest",
        .longDesc = "This is a dimly lit forest, with large trees all around.",
        .exits = {
            {NORTH, GRATING_CLEARING},
            {EAST, FOREST_PATH},
            {SOUTH, FOREST_3},
            {WEST, WEST_OF_HOUSE}
        },
        .flags = {RLANDBIT, ONBIT}
    });

    
    // FOREST_2 - Forest area
    createRoom({
        .id = FOREST_2,
        .name = "Forest",
        .longDesc = "This is a dimly lit forest, with large trees all around.",
        .exits = {
            {EAST, MOUNTAINS},
            {SOUTH, CLEARING},
            {WEST, FOREST_PATH}
        },
        .blockedExits = {
            {NORTH, "The forest becomes impenetrable to the north."}
        },
        .flags = {RLANDBIT, ONBIT}
    });
    
    // FOREST_3 - Forest area
    createRoom({
        .id = FOREST_3,
        .name = "Forest",
        .longDesc = "This is a dimly lit forest, with large trees all around.",
        .exits = {
            {NORTH, CLEARING},
            {WEST, FOREST_1},
            {NW, SOUTH_OF_HOUSE}
        },
        .blockedExits = {
            {SOUTH, "Storm-tossed trees block your way."}
        },
        .flags = {RLANDBIT, ONBIT}
    });

    
    // MOUNTAINS - Dead end at mountains
    createRoom({
        .id = MOUNTAINS,
        .name = "Forest",
        .longDesc = "The forest thins out, revealing impassable mountains.",
        .exits = {
            {NORTH, FOREST_2},
            {SOUTH, FOREST_2},
            {WEST, FOREST_2}
        },
        .blockedExits = {
            {UP, "The mountains are impassable."},
            {EAST, "The mountains are impassable."}
        },
        .flags = {RLANDBIT, ONBIT}
    });
    
    // GRATING_CLEARING - Clearing with grating
    createRoom({
        .id = GRATING_CLEARING,
        .name = "Clearing",
        .longDesc = "You are in a clearing, with a forest surrounding you on all sides. "
                    "A path leads south.",
        .exits = {
            {EAST, FOREST_2},
            {WEST, FOREST_1},
            {SOUTH, FOREST_PATH},
            {DOWN, GRATING_ROOM}  // Through grating when open
        },
        .blockedExits = {
            {NORTH, "The forest becomes impenetrable to the north."}
        },
        .flags = {RLANDBIT, ONBIT}
    });

    
    // CANYON_VIEW - Overlook of Great Canyon
    createRoom({
        .id = CANYON_VIEW,
        .name = "Canyon View",
        .longDesc = "You are at the top of the Great Canyon on its west wall. From here "
                    "there is a marvelous view of the canyon and parts of the Frigid River "
                    "upstream. Across the canyon, the walls of the White Cliffs join the "
                    "mighty ramparts of the Flathead Mountains to the east. Following the "
                    "Canyon upstream to the north, Aragain Falls may be seen, complete with "
                    "rainbow. The mighty Frigid River flows out from a great dark cavern. "
                    "To the west and south can be seen an immense forest, stretching for "
                    "miles around. A path leads northwest.",
        .exits = {
            {WEST, CLEARING},
            {NW, CLEARING}
        },
        .blockedExits = {
            {DOWN, "The canyon is too deep to climb down."}
        },
        .flags = {RLANDBIT, ONBIT}
    });
    
} // end initializeExteriorRooms
