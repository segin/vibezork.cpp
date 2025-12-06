// Special area rooms - temple, cyclops, hades, treasure room, etc.
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"
#include "systems/npc_system.h"

void initializeSpecialRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // CYCLOPS_ROOM - Cyclops guards treasure room
    createRoom({
        .id = CYCLOPS_ROOM,
        .name = "Cyclops Room",
        .longDesc = "This is a large room with a ceiling which cannot be detected from the "
                    "ground. There is a narrow passage from east to west and a stone stairway "
                    "leading upward. The room is eerie in its quietness.",
        .exits = {
            {EAST, STRANGE_PASSAGE}
        },
        .conditionalExits = {
            {
                .dir = UP,
                .target = TREASURE_ROOM,
                .condition = []() {
                    auto& cyclopsState = NPCSystem::getCyclopsState();
                    return cyclopsState.isAsleep || cyclopsState.hasFled;
                },
                .blockedMsg = "There is no exit in that direction."
            }
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // STRANGE_PASSAGE - Passage to cyclops room
    createRoom({
        .id = STRANGE_PASSAGE,
        .name = "Strange Passage",
        .longDesc = "This is a long passage. To the west is one entrance. On the east there "
                    "is an old wooden door, with a large opening in it (about cyclops sized).",
        .exits = {
            {WEST, CYCLOPS_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // TREASURE_ROOM - Thief's lair
    createRoom({
        .id = TREASURE_ROOM,
        .name = "Treasure Room",
        .longDesc = "This is a large room, whose east wall is solid granite. A number of "
                    "discarded bags, which crumble at your touch, are scattered about on the "
                    "floor. There is an exit down a staircase.",
        .exits = {
            {DOWN, CYCLOPS_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // ENTRANCE_TO_HADES - Guarded by spirits
    createRoom({
        .id = ENTRANCE_TO_HADES,
        .name = "Entrance to Hades",
        .longDesc = "You are outside a large gateway, on which is inscribed:\n\n"
                    "    Abandon every hope all ye who enter here\n\n"
                    "The gate is open; through it you can see a desolation, with a pile of "
                    "mangled corpses in one corner. Thousands of voices, lamenting some hideous "
                    "fate, can be heard.",
        .exits = {
            {SOUTH, LAND_OF_LIVING_DEAD},
            {DOWN, LAND_OF_LIVING_DEAD},
            {UP, TINY_CAVE}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // LAND_OF_LIVING_DEAD - End game area
    createRoom({
        .id = LAND_OF_LIVING_DEAD,
        .name = "Land of the Living Dead",
        .longDesc = "You have entered the Land of the Living Dead. Thousands of lost souls "
                    "can be heard weeping and moaning. In the corner are stacked the remains "
                    "of dozens of previous adventurers less fortunate than yourself. A passage "
                    "exits to the north.",
        .exits = {
            {NORTH, ENTRANCE_TO_HADES},
            {UP, ENTRANCE_TO_HADES}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // ENGRAVINGS_CAVE - Cave with ancient engravings
    createRoom({
        .id = ENGRAVINGS_CAVE,
        .name = "Engravings Cave",
        .longDesc = "You have entered a low cave with passages leading northwest and east.",
        .exits = {
            {NW, ROUND_ROOM},
            {EAST, DOME_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // DOME_ROOM - Dome with railing
    createRoom({
        .id = DOME_ROOM,
        .name = "Dome Room",
        .longDesc = "You are at the periphery of a large dome, which forms the ceiling of "
                    "another room below. Protecting you from a precipitous drop is a wooden "
                    "railing which circles the dome.",
        .exits = {
            {WEST, ENGRAVINGS_CAVE},
            {DOWN, TORCH_ROOM}  // Requires rope tied
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // TORCH_ROOM - Room with pedestal and torch
    createRoom({
        .id = TORCH_ROOM,
        .name = "Torch Room",
        .longDesc = "This is a large room with a prominent doorway leading to a down staircase. "
                    "To the west is a narrow twisting tunnel, through which is coming a horrible "
                    "stench. Above you is a large dome painted with scenes depicting elvish "
                    "hacking rites. Up around the edge of the dome (20 feet up) is a wooden "
                    "railing. In the center of the room there is a white marble pedestal.",
        .exits = {
            {SOUTH, NORTH_TEMPLE},
            {DOWN, NORTH_TEMPLE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // NORTH_TEMPLE - North end of temple
    createRoom({
        .id = NORTH_TEMPLE,
        .name = "Temple",
        .longDesc = "This is the north end of a large temple. In front of you is what appears "
                    "to be an altar. In one corner is a small hole in the floor which leads "
                    "into darkness. You probably could not get back up it.",
        .exits = {
            {DOWN, EGYPT_ROOM},
            {EAST, EGYPT_ROOM},
            {NORTH, TORCH_ROOM},
            {OUT, TORCH_ROOM},
            {UP, TORCH_ROOM},
            {SOUTH, SOUTH_TEMPLE}
        },
        .flags = {RLANDBIT, SACREDBIT}  // Dark, safe
    });
    
    // SOUTH_TEMPLE - South end of temple (altar)
    createRoom({
        .id = SOUTH_TEMPLE,
        .name = "Altar",
        .longDesc = "This is the south end of a large temple. In front of you is what appears "
                    "to be an altar. In one corner is a small hole in the floor which leads "
                    "into darkness. You probably could not get back up it.",
        .exits = {
            {NORTH, NORTH_TEMPLE},
            {DOWN, ENTRANCE_TO_HADES}
        },
        .flags = {RLANDBIT, SACREDBIT}  // Dark, safe
    });

    // EGYPT_ROOM - Egyptian tomb
    createRoom({
        .id = EGYPT_ROOM,
        .name = "Egyptian Room",
        .longDesc = "This is a room which looks like an Egyptian tomb. There is an ascending "
                    "staircase to the west.",
        .exits = {
            {WEST, NORTH_TEMPLE},
            {UP, NORTH_TEMPLE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MIRROR_ROOM_1 - First mirror room
    createRoom({
        .id = MIRROR_ROOM_1,
        .name = "Mirror Room",
        .longDesc = "You are in a large square room with tall ceilings. On the south wall is "
                    "an enormous mirror which fills the entire wall. There are exits on the "
                    "other three sides of the room.",
        .exits = {
            {NORTH, COLD_PASSAGE},
            {WEST, TWISTING_PASSAGE},
            {EAST, SMALL_CAVE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MIRROR_ROOM_2 - Second mirror room (on the other side)
    createRoom({
        .id = MIRROR_ROOM_2,
        .name = "Mirror Room",
        .longDesc = "You are in a large square room with tall ceilings. On the south wall is "
                    "an enormous mirror which fills the entire wall. There are exits on the "
                    "other three sides of the room.",
        .exits = {
            {WEST, WINDING_PASSAGE},
            {NORTH, NARROW_PASSAGE},
            {EAST, TINY_CAVE}
        },
        .flags = {RLANDBIT, ONBIT}  // Dark, on (mirror world)
    });

    // SMALL_CAVE - Cave connecting to mirror room 1
    createRoom({
        .id = SMALL_CAVE,
        .name = "Cave",
        .longDesc = "This is a tiny cave with entrances west and north, and a staircase "
                    "leading down.",
        .exits = {
            {NORTH, MIRROR_ROOM_1},
            {WEST, TWISTING_PASSAGE},
            {DOWN, ATLANTIS_ROOM},
            {SOUTH, ATLANTIS_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // TINY_CAVE - Cave connecting to mirror room 2
    createRoom({
        .id = TINY_CAVE,
        .name = "Cave",
        .longDesc = "This is a tiny cave with entrances west and north, and a dark, forbidding "
                    "staircase leading down.",
        .exits = {
            {NORTH, MIRROR_ROOM_2},
            {WEST, WINDING_PASSAGE},
            {DOWN, ENTRANCE_TO_HADES}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // WINDING_PASSAGE - Passage in mirror world
    createRoom({
        .id = WINDING_PASSAGE,
        .name = "Winding Passage",
        .longDesc = "This is a winding passage. It seems that there are only exits on the "
                    "east and north.",
        .exits = {
            {NORTH, MIRROR_ROOM_2},
            {EAST, TINY_CAVE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // TWISTING_PASSAGE - Passage in normal world
    createRoom({
        .id = TWISTING_PASSAGE,
        .name = "Twisting Passage",
        .longDesc = "This is a winding passage. It seems that there are only exits on the "
                    "east and north.",
        .exits = {
            {NORTH, MIRROR_ROOM_1},
            {EAST, SMALL_CAVE}
        },
        .flags = {RLANDBIT}  // Dark
    });
}
