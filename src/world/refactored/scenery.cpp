// Scenery objects - non-takeable environmental objects
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

// Forward declarations for action handlers
bool whiteHouseAction();
bool boardAction();
bool forestAction();
bool kitchenWindowAction();
bool groundAction();

void initializeScenery() {
    using namespace ObjectIds;
    
    // WHITE_HOUSE - Global object for house exterior
    createObject({
        .id = WHITE_HOUSE,
        .desc = "white house",
        .synonyms = {"house"},
        .adjectives = {"white", "beautiful", "colonial"},
        .flags = {ObjectFlag::NDESCBIT},
        .action = whiteHouseAction
    });
    
    // BOARD - Boards on windows
    createObject({
        .id = BOARD,
        .desc = "board",
        .synonyms = {"boards", "board"},
        .flags = {ObjectFlag::NDESCBIT},
        .action = boardAction
    });

    
    // FOREST - Global forest object
    createObject({
        .id = FOREST,
        .desc = "forest",
        .synonyms = {"forest", "trees", "pines", "hemlocks"},
        .flags = {ObjectFlag::NDESCBIT},
        .action = forestAction
    });
    
    // TREE - Large tree (climbable)
    createObject({
        .id = TREE,
        .desc = "tree",
        .synonyms = {"tree", "branch"},
        .adjectives = {"large", "storm"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CLIMBBIT}
    });
    
    // SONGBIRD - Songbird in forest
    createObject({
        .id = SONGBIRD,
        .desc = "songbird",
        .synonyms = {"bird", "songbird"},
        .adjectives = {"song"},
        .flags = {ObjectFlag::NDESCBIT}
    });

    
    // BOARDED_WINDOW - Boarded window on house
    createObject({
        .id = BOARDED_WINDOW,
        .desc = "boarded window",
        .synonyms = {"window"},
        .adjectives = {"boarded"},
        .flags = {ObjectFlag::NDESCBIT}
    });
    
    // KITCHEN_WINDOW - Small window (slightly ajar)
    createObject({
        .id = KITCHEN_WINDOW,
        .desc = "small window",
        .synonyms = {"window"},
        .adjectives = {"small", "kitchen"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::DOORBIT},
        .location = RoomIds::BEHIND_HOUSE,
        .action = kitchenWindowAction
    });
    
    // FRONT_DOOR - Front door of house
    createObject({
        .id = FRONT_DOOR,
        .desc = "front door",
        .synonyms = {"door"},
        .adjectives = {"front"},
        .flags = {ObjectFlag::DOORBIT, ObjectFlag::NDESCBIT}
    });

    
    // TRAP_DOOR - Trap door in Living Room (hidden under rug)
    createObject({
        .id = TRAP_DOOR,
        .desc = "trap door",
        .synonyms = {"door", "trapdoor", "trap-door", "cover"},
        .adjectives = {"trap", "dusty"},
        .flags = {ObjectFlag::DOORBIT, ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE},
        .location = RoomIds::LIVING_ROOM
    });
    
    // WOODEN_DOOR - Wooden door in Living Room (west)
    createObject({
        .id = WOODEN_DOOR,
        .desc = "wooden door",
        .synonyms = {"door", "lettering", "writing"},
        .adjectives = {"wooden", "gothic", "strange", "west"},
        .flags = {ObjectFlag::READBIT, ObjectFlag::DOORBIT, 
                  ObjectFlag::NDESCBIT, ObjectFlag::TRANSBIT},
        .text = "The engravings translate to \"This space intentionally left blank.\"",
        .location = RoomIds::LIVING_ROOM
    });

    
    // BARROW_DOOR - Stone door in Stone Barrow
    createObject({
        .id = BARROW_DOOR,
        .desc = "stone door",
        .synonyms = {"door"},
        .adjectives = {"huge", "stone"},
        .flags = {ObjectFlag::DOORBIT, ObjectFlag::NDESCBIT, ObjectFlag::OPENBIT},
        .location = RoomIds::STONE_BARROW
    });
    
    // GRATE - Grating (hidden until leaves moved)
    createObject({
        .id = GRATE,
        .desc = "grating",
        .synonyms = {"grate", "grating"},
        .flags = {ObjectFlag::DOORBIT, ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE}
    });
    
    // RUG - Rug in Living Room
    createObject({
        .id = RUG,
        .desc = "rug",
        .synonyms = {"rug", "carpet"},
        .adjectives = {"oriental", "large"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TRYTAKEBIT},
        .location = RoomIds::LIVING_ROOM
    });

    
    // LADDER - Wooden ladder (climbable)
    createObject({
        .id = LADDER,
        .desc = "wooden ladder",
        .synonyms = {"ladder"},
        .adjectives = {"wooden", "rickety", "narrow"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CLIMBBIT}
    });
    
    // SLIDE - Chute/slide (climbable)
    createObject({
        .id = SLIDE,
        .desc = "chute",
        .synonyms = {"chute", "ramp", "slide"},
        .adjectives = {"steep", "metal", "twisting"},
        .flags = {ObjectFlag::CLIMBBIT}
    });
    
    // STAIRS - Stairs
    createObject({
        .id = STAIRS,
        .desc = "stairs",
        .synonyms = {"stairs", "staircase", "stairway"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CLIMBBIT}
    });

    
    // RAILING - Wooden railing in Dome Room
    createObject({
        .id = RAILING,
        .desc = "wooden railing",
        .synonyms = {"railing", "rail"},
        .adjectives = {"wooden"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::DOME_ROOM
    });
    
    // WALL - Generic wall
    createObject({
        .id = WALL,
        .desc = "surrounding wall",
        .synonyms = {"wall", "walls"},
        .adjectives = {"surrounding"},
        .flags = {ObjectFlag::NDESCBIT}
    });
    
    // GRANITE_WALL - Granite wall
    createObject({
        .id = GRANITE_WALL,
        .desc = "granite wall",
        .synonyms = {"wall"},
        .adjectives = {"granite"},
        .flags = {ObjectFlag::NDESCBIT}
    });

    
    // CRACK - Narrow crack
    createObject({
        .id = CRACK,
        .desc = "crack",
        .synonyms = {"crack"},
        .adjectives = {"narrow"},
        .flags = {ObjectFlag::NDESCBIT}
    });
    
    // CLIMBABLE_CLIFF - Rocky cliff (climbable)
    createObject({
        .id = CLIMBABLE_CLIFF,
        .desc = "cliff",
        .synonyms = {"wall", "cliff", "walls", "ledge"},
        .adjectives = {"rocky", "sheer"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CLIMBBIT}
    });
    
    // WHITE_CLIFF - White cliffs (climbable)
    createObject({
        .id = WHITE_CLIFF,
        .desc = "white cliffs",
        .synonyms = {"cliff", "cliffs"},
        .adjectives = {"white"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CLIMBBIT}
    });

    
    // CHIMNEY - Chimney
    createObject({
        .id = CHIMNEY,
        .desc = "chimney",
        .synonyms = {"chimney"},
        .flags = {ObjectFlag::NDESCBIT}
    });
    
    // BARROW - Stone barrow
    createObject({
        .id = BARROW,
        .desc = "stone barrow",
        .synonyms = {"barrow", "tomb"},
        .adjectives = {"massive", "stone"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::STONE_BARROW
    });
    
    // RIVER - River
    createObject({
        .id = RIVER,
        .desc = "river",
        .synonyms = {"river", "frigid"},
        .adjectives = {"frigid"},
        .flags = {ObjectFlag::NDESCBIT}
    });

    
    // GLOBAL_WATER - Global water object
    createObject({
        .id = GLOBAL_WATER,
        .desc = "water",
        .synonyms = {"water", "quantity"},
        .flags = {ObjectFlag::DRINKBIT}
    });
    
    // MOUNTAIN_RANGE - Mountains
    createObject({
        .id = MOUNTAIN_RANGE,
        .desc = "mountains",
        .synonyms = {"mountains", "mountain", "range"},
        .flags = {ObjectFlag::NDESCBIT}
    });
    
    // TEETH - Overboard teeth
    createObject({
        .id = TEETH,
        .desc = "set of teeth",
        .synonyms = {"overboard", "teeth"},
        .flags = {ObjectFlag::NDESCBIT}
    });

    
    // BODIES - Pile of bodies in Hades
    createObject({
        .id = BODIES,
        .desc = "pile of bodies",
        .synonyms = {"bodies", "body", "remains", "pile"},
        .adjectives = {"mangled"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TRYTAKEBIT}
    });
    
    // BONES - Skeleton in Maze-5
    createObject({
        .id = BONES,
        .desc = "skeleton",
        .synonyms = {"bones", "skeleton", "body"},
        .flags = {ObjectFlag::TRYTAKEBIT, ObjectFlag::NDESCBIT},
        .location = RoomIds::MAZE_5
    });
    
    // GROUND - Global ground object (for PUT X ON GROUND)
    createObject({
        .id = GROUND,
        .desc = "ground",
        .synonyms = {"ground", "floor", "dirt", "sand"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE},
        .action = groundAction
    });

    
    // LEAVES - Pile of leaves in Grating Clearing
    createObject({
        .id = LEAVES,
        .desc = "pile of leaves",
        .synonyms = {"leaves", "leaf", "pile"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::BURNBIT, ObjectFlag::TRYTAKEBIT},
        .size = 25,
        .longDesc = "On the ground is a pile of leaves.",
        .location = RoomIds::GRATING_CLEARING
    });
    
    // COAL - Small pile of coal in Dead End 5
    createObject({
        .id = COAL,
        .desc = "small pile of coal",
        .synonyms = {"coal", "pile", "heap"},
        .adjectives = {"small"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::BURNBIT},
        .size = 20,
        .location = RoomIds::DEAD_END_5
    });
    
    // TIMBERS - Broken timbers in Timber Room
    createObject({
        .id = TIMBERS,
        .desc = "broken timber",
        .synonyms = {"timbers", "pile"},
        .adjectives = {"wooden", "broken"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 50,
        .location = RoomIds::TIMBER_ROOM
    });

    
    // SAND - Sand in Sandy Cave
    createObject({
        .id = SAND,
        .desc = "sand",
        .synonyms = {"sand"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::SANDY_CAVE
    });
    
    // PRAYER - Ancient prayer inscription in North Temple
    createObject({
        .id = PRAYER,
        .desc = "prayer",
        .synonyms = {"prayer", "inscription"},
        .adjectives = {"ancient", "old"},
        .flags = {ObjectFlag::READBIT, ObjectFlag::SACREDBIT, ObjectFlag::NDESCBIT},
        .text = "The prayer is inscribed in an ancient script, rarely used today. It seems "
                "to be a philippic against small insects, absent-mindedness, and the picking "
                "up and dropping of small objects. The final verse consigns trespassers to "
                "the land of the dead. All evidence indicates that the beliefs of the ancient "
                "Zorkers were obscure.",
        .location = RoomIds::NORTH_TEMPLE
    });

    
    // ENGRAVINGS - Wall engravings in Engravings Cave
    createObject({
        .id = ENGRAVINGS,
        .desc = "wall with engravings",
        .synonyms = {"wall", "engravings", "inscription"},
        .adjectives = {"old", "ancient"},
        .flags = {ObjectFlag::READBIT, ObjectFlag::SACREDBIT},
        .longDesc = "There are old engravings on the walls here.",
        .text = "The engravings were incised in the living rock of the cave wall by "
                "an unknown hand. They depict, in symbolic form, the beliefs of the "
                "ancient Zorkers. Skillfully interwoven with the bas reliefs are excerpts "
                "illustrating the major religious tenets of that time. Unfortunately, a "
                "later age seems to have considered them blasphemous and just as skillfully "
                "excised them.",
        .location = RoomIds::ENGRAVINGS_CAVE
    });
    
} // end initializeScenery
