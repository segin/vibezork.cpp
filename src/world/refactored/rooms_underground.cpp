// Underground passage rooms - troll room, round room, passages, etc.
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"
#include "systems/npc_system.h"

void initializeUndergroundRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // TROLL_ROOM - Troll blocks passage
    createRoom({
        .id = TROLL_ROOM,
        .name = "Troll Room",
        .longDesc = "This is a small room with passages to the east and south and a "
                    "forbidding hole leading west. Bloodstains and deep scratches (perhaps "
                    "made by an axe) mar the walls.",
        .exits = {
            {SOUTH, CELLAR}
        },
        .conditionalExits = {
            {
                .dir = EAST,
                .target = EW_PASSAGE,
                .condition = []() {
                    auto* troll = Globals::instance().getObject(ObjectIds::TROLL);
                    return !troll || troll->hasFlag(DEADBIT) || !NPCSystem::isTrollActive();
                },
                .blockedMsg = "The troll blocks your way."
            },
            {
                .dir = WEST,
                .target = MAZE_1,
                .condition = []() {
                    auto* troll = Globals::instance().getObject(ObjectIds::TROLL);
                    return !troll || troll->hasFlag(DEADBIT) || !NPCSystem::isTrollActive();
                },
                .blockedMsg = "The troll blocks your way."
            }
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // EAST_OF_CHASM - Edge of chasm
    createRoom({
        .id = EAST_OF_CHASM,
        .name = "East of Chasm",
        .longDesc = "You are on the east edge of a chasm, the bottom of which cannot be "
                    "seen. A narrow passage goes north, and the path you are on continues "
                    "to the east.",
        .exits = {
            {NORTH, CELLAR},
            {EAST, GALLERY}
        },
        .blockedExits = {
            {DOWN, "The chasm probably leads straight to the infernal regions."}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // EW_PASSAGE - East-West Passage
    createRoom({
        .id = EW_PASSAGE,
        .name = "East-West Passage",
        .longDesc = "This is a narrow east-west passageway. There is a narrow stairway "
                    "leading down at the north end of the room.",
        .exits = {
            {EAST, ROUND_ROOM},
            {WEST, TROLL_ROOM},
            {DOWN, CHASM_ROOM},
            {NORTH, CHASM_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // NS_PASSAGE - North-South Passage
    createRoom({
        .id = NS_PASSAGE,
        .name = "North-South Passage",
        .longDesc = "This is a high north-south passage, which forks to the northeast.",
        .exits = {
            {NORTH, CHASM_ROOM},
            {NE, DEEP_CANYON},
            {SOUTH, ROUND_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // CHASM_ROOM - Chasm
    createRoom({
        .id = CHASM_ROOM,
        .name = "Chasm",
        .longDesc = "A chasm runs southwest to northeast and the path follows it. You are "
                    "on the south side of the chasm, where a crack opens into a passage.",
        .exits = {
            {NE, RESERVOIR_SOUTH},
            {SW, EW_PASSAGE},
            {UP, EW_PASSAGE},
            {SOUTH, NS_PASSAGE}
        },
        .blockedExits = {
            {DOWN, "Are you out of your mind?"}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // ROUND_ROOM - Circular stone room
    createRoom({
        .id = ROUND_ROOM,
        .name = "Round Room",
        .longDesc = "This is a circular stone room with passages in all directions. Several "
                    "of them have unfortunately been blocked by cave-ins.",
        .exits = {
            {NORTH, NS_PASSAGE},
            {WEST, EW_PASSAGE},
            {EAST, LOUD_ROOM},
            {SE, LOUD_ROOM},
            {SOUTH, NARROW_PASSAGE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // LOUD_ROOM - Loud Room with platinum bar
    createRoom({
        .id = LOUD_ROOM,
        .name = "Loud Room",
        .longDesc = "This is a large room with a ceiling which cannot be detected from the "
                    "ground. There is a narrow passage from east to west and a stone stairway "
                    "leading upward. The room is extremely noisy. In fact, it is difficult to "
                    "hear yourself think.",
        .exits = {
            {WEST, ROUND_ROOM},
            {EAST, DAMP_CAVE},
            {UP, DEEP_CANYON}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // DEEP_CANYON - Deep Canyon
    createRoom({
        .id = DEEP_CANYON,
        .name = "Deep Canyon",
        .longDesc = "You are on the south edge of a deep canyon. Passages lead off to the "
                    "east, northwest and southwest. You can hear the sound of flowing water "
                    "from below.",
        .exits = {
            {NW, RESERVOIR_SOUTH},
            {EAST, DAM_ROOM},
            {SW, NS_PASSAGE},
            {DOWN, LOUD_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // DAMP_CAVE - Damp Cave
    createRoom({
        .id = DAMP_CAVE,
        .name = "Damp Cave",
        .longDesc = "This cave has exits to the west and east, and narrows to a crack toward "
                    "the south. The earth is particularly damp here.",
        .exits = {
            {WEST, LOUD_ROOM},
            {EAST, WHITE_CLIFFS_NORTH}
        },
        .blockedExits = {
            {SOUTH, "It is too narrow for most insects."}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // COLD_PASSAGE - Cold Passage
    createRoom({
        .id = COLD_PASSAGE,
        .name = "Cold Passage",
        .longDesc = "This is a cold and damp corridor where a long east-west passageway "
                    "turns into a southward path.",
        .exits = {
            {SOUTH, MIRROR_ROOM_1},
            {WEST, SLIDE_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // NARROW_PASSAGE - Narrow Passage
    createRoom({
        .id = NARROW_PASSAGE,
        .name = "Narrow Passage",
        .longDesc = "This is a long and narrow corridor where a long north-south passageway "
                    "briefly narrows even further.",
        .exits = {
            {NORTH, ROUND_ROOM},
            {SOUTH, MIRROR_ROOM_2}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // SLIDE_ROOM - Slide Room
    createRoom({
        .id = SLIDE_ROOM,
        .name = "Slide Room",
        .longDesc = "This is a small chamber, which appears to have been part of a coal mine. "
                    "On the south wall of the chamber the letters \"Granite Wall\" are etched "
                    "in the rock. To the east is a long passage, and there is a steep metal "
                    "slide twisting downward. To the north is a small opening.",
        .exits = {
            {EAST, COLD_PASSAGE},
            {NORTH, MINE_ENTRANCE},
            {DOWN, CELLAR}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // MINE_ENTRANCE - Mine Entrance
    createRoom({
        .id = MINE_ENTRANCE,
        .name = "Mine Entrance",
        .longDesc = "You are standing at the entrance of what might have been a coal mine. "
                    "The shaft enters the west wall, and there is another exit on the south "
                    "end of the room.",
        .exits = {
            {SOUTH, SLIDE_ROOM},
            {IN, SQUEEKY_ROOM},
            {WEST, SQUEEKY_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // SQUEEKY_ROOM - Squeaky Room
    createRoom({
        .id = SQUEEKY_ROOM,
        .name = "Squeaky Room",
        .longDesc = "You are in a small room. Strange squeaky sounds may be heard coming "
                    "from the passage at the north end. You may also escape to the east.",
        .exits = {
            {NORTH, BAT_ROOM},
            {EAST, MINE_ENTRANCE}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // BAT_ROOM - Bat Room
    createRoom({
        .id = BAT_ROOM,
        .name = "Bat Room",
        .longDesc = "You are in a small room which has doors only to the east and south.",
        .exits = {
            {SOUTH, SQUEEKY_ROOM},
            {EAST, SHAFT_ROOM}
        },
        .flags = {RLANDBIT, SACREDBIT}  // Dark, safe
    });
    
    // SHAFT_ROOM - Shaft Room with basket
    createRoom({
        .id = SHAFT_ROOM,
        .name = "Shaft Room",
        .longDesc = "This is a large room, in the middle of which is a small shaft descending "
                    "through the floor into darkness below. To the west and the north are exits "
                    "from this room. Constructed over the top of the shaft is a metal framework "
                    "to which a heavy iron chain is attached.",
        .exits = {
            {WEST, BAT_ROOM},
            {NORTH, SMELLY_ROOM}
        },
        .blockedExits = {
            {DOWN, "You wouldn't fit and would die if you could."}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // SMELLY_ROOM - Smelly Room
    createRoom({
        .id = SMELLY_ROOM,
        .name = "Smelly Room",
        .longDesc = "This is a small nondescript room. However, from the direction of a small "
                    "descending staircase a foul odor can be detected. To the south is a narrow "
                    "tunnel.",
        .exits = {
            {DOWN, GAS_ROOM},
            {SOUTH, SHAFT_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // GAS_ROOM - Gas Room with bracelet
    createRoom({
        .id = GAS_ROOM,
        .name = "Gas Room",
        .longDesc = "This is a small room which smells strongly of coal gas. There is a short "
                    "climb up some stairs and a narrow tunnel leading east.",
        .exits = {
            {UP, SMELLY_ROOM},
            {EAST, LADDER_TOP}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // LADDER_TOP - Top of ladder
    createRoom({
        .id = LADDER_TOP,
        .name = "Ladder Top",
        .longDesc = "This is a very small room. In the corner is a rickety wooden ladder, "
                    "leading downward. It might be safe to descend. There is also a stairway "
                    "leading upward.",
        .exits = {
            {WEST, GAS_ROOM},
            {DOWN, LADDER_BOTTOM},
            {UP, TIMBER_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // LADDER_BOTTOM - Bottom of ladder
    createRoom({
        .id = LADDER_BOTTOM,
        .name = "Ladder Bottom",
        .longDesc = "This is a rather wide room. On one side is the bottom of a narrow wooden "
                    "ladder. To the west and the south are passages leaving the room.",
        .exits = {
            {UP, LADDER_TOP},
            {WEST, LOWER_SHAFT},
            {SOUTH, MACHINE_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // TIMBER_ROOM - Timber Room
    createRoom({
        .id = TIMBER_ROOM,
        .name = "Timber Room",
        .longDesc = "This is a long and narrow passage, which is cluttered with broken timbers. "
                    "A wide passage comes from the east and turns at the west end of the room "
                    "into a very narrow passageway. From the west comes a strong draft.",
        .exits = {
            {WEST, DEAD_END_5},
            {DOWN, LADDER_TOP},
            {EAST, MINE_4}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // LOWER_SHAFT - Lower Shaft with basket
    createRoom({
        .id = LOWER_SHAFT,
        .name = "Lower Shaft",
        .longDesc = "This is the bottom of a large vertical shaft. There are passages leading "
                    "off in all directions.",
        .exits = {
            {EAST, LADDER_BOTTOM},
            {NORTH, MACHINE_ROOM},
            {SOUTH, MACHINE_ROOM},
            {WEST, MACHINE_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });

    
    // MACHINE_ROOM - Machine Room with coal machine
    createRoom({
        .id = MACHINE_ROOM,
        .name = "Machine Room",
        .longDesc = "This is a large room full of assorted heavy machinery, whirring noisily. "
                    "The room smells of burned resistors. Along one wall of the room are three "
                    "buttons which are, respectively, round, triangular, and square. Naturally, "
                    "above these buttons are instructions written in EBCDIC. A large sign above "
                    "all three buttons says:\n\n"
                    "        ~DANGER~\n"
                    "        AUTHORIZED PERSONNEL ONLY\n\n"
                    "A fourth button by itself is colored blue. There are exits to the north "
                    "and the east.",
        .exits = {
            {NORTH, LADDER_BOTTOM},
            {EAST, LOWER_SHAFT}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // DEAD_END_5 - Dead End with coal
    createRoom({
        .id = DEAD_END_5,
        .name = "Dead End",
        .longDesc = "You have come to a dead end in the mine.",
        .exits = {
            {EAST, TIMBER_ROOM}
        },
        .flags = {RLANDBIT}  // Dark
    });
    
} // end initializeUndergroundRooms
