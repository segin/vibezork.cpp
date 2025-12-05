// Tool objects - useful items for solving puzzles
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

// Forward declarations for action handlers
bool swordAction();
bool lampAction();
bool candlesAction();
bool matchesAction();
bool ropeAction();
bool wrenchAction();
bool screwdriverAction();
bool shovelAction();
bool pumpAction();

void initializeTools() {
    using namespace ObjectIds;
    
    // SWORD - Elvish sword in Living Room (glows near enemies)
    createObject({
        .id = SWORD,
        .desc = "sword",
        .synonyms = {"sword", "blade", "orcrist", "glamdring"},
        .adjectives = {"elvish", "elven", "old", "antique"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::WEAPONBIT},
        .size = 30,
        .strength = 10,
        .location = RoomIds::LIVING_ROOM,
        .action = swordAction
    });

    
    // KNIFE - Nasty knife in Attic (on table)
    createObject({
        .id = KNIFE,
        .desc = "nasty knife",
        .synonyms = {"knife", "knives", "blade"},
        .adjectives = {"nasty", "sharp"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::WEAPONBIT},
        .size = 10,
        .strength = 5,
        .location = ATTIC_TABLE  // On the attic table
    });
    
    // RUSTY_KNIFE - Rusty knife in Maze-5 (by skeleton)
    createObject({
        .id = RUSTY_KNIFE,
        .desc = "rusty knife",
        .synonyms = {"knife", "knives"},
        .adjectives = {"rusty"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TRYTAKEBIT, 
                  ObjectFlag::WEAPONBIT, ObjectFlag::TOOLBIT},
        .size = 20,
        .text = "Beside the skeleton is a rusty knife.",
        .location = RoomIds::MAZE_5
    });

    
    // LAMP - Brass lantern in Living Room (battery-powered)
    createObject({
        .id = LAMP,
        .desc = "brass lantern",
        .synonyms = {"lamp", "lantern", "light"},
        .adjectives = {"brass", "battery"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::LIGHTBIT},
        .size = 15,
        .capacity = 330,  // Battery level - about 330 turns
        .strength = 100,  // If used as weapon
        .longDesc = "There is a brass lantern (battery-powered) here.",
        .location = RoomIds::LIVING_ROOM,
        .action = lampAction
    });
    
    // CANDLES - Pair of candles in South Temple (starts lit)
    createObject({
        .id = CANDLES,
        .desc = "pair of candles",
        .synonyms = {"candles", "candle", "pair"},
        .adjectives = {"burning", "wax"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::LIGHTBIT, 
                  ObjectFlag::ONBIT, ObjectFlag::FLAMEBIT},
        .size = 10,
        .strength = 50,  // Wax remaining
        .location = RoomIds::SOUTH_TEMPLE,
        .action = candlesAction
    });

    
    // MATCH - Matchbook in Dam Lobby
    createObject({
        .id = MATCH,
        .desc = "matchbook",
        .synonyms = {"match", "matches", "matchbook"},
        .adjectives = {"match"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 2,
        .strength = 5,  // Number of matches remaining
        .longDesc = "There is a matchbook whose cover says \"Visit Beautiful FCD#3\" here.",
        .location = RoomIds::DAM_LOBBY,
        .action = matchesAction
    });
    
    // ROPE - Large coil of rope in Attic
    createObject({
        .id = ROPE,
        .desc = "rope",
        .synonyms = {"rope", "coil", "hemp"},
        .adjectives = {"large", "thick"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::SACREDBIT},
        .size = 10,
        .location = RoomIds::ATTIC,
        .action = ropeAction
    });

    
    // WRENCH - Wrench in Maintenance Room
    createObject({
        .id = WRENCH,
        .desc = "wrench",
        .synonyms = {"wrench", "tool"},
        .adjectives = {"metal"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 10,
        .location = RoomIds::MAINTENANCE_ROOM,
        .action = wrenchAction
    });
    
    // SCREWDRIVER - Screwdriver in Maintenance Room
    createObject({
        .id = SCREWDRIVER,
        .desc = "screwdriver",
        .synonyms = {"screwdriver", "driver", "tool"},
        .adjectives = {"screw"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 8,
        .location = RoomIds::MAINTENANCE_ROOM,
        .action = screwdriverAction
    });

    
    // SHOVEL - Shovel on Sandy Beach
    createObject({
        .id = SHOVEL,
        .desc = "shovel",
        .synonyms = {"shovel", "spade", "tool"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 15,
        .location = RoomIds::SANDY_BEACH,
        .action = shovelAction
    });
    
    // PUMP - Hand-held air pump in Reservoir North
    createObject({
        .id = PUMP,
        .desc = "hand-held air pump",
        .synonyms = {"pump", "air-pump", "tool"},
        .adjectives = {"hand-held", "small"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 8,
        .location = RoomIds::RESERVOIR_NORTH,
        .action = pumpAction
    });

    
    // KEYS - Skeleton key in Maze-5 (by skeleton)
    createObject({
        .id = KEYS,
        .desc = "skeleton key",
        .synonyms = {"key"},
        .adjectives = {"skeleton"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 10,
        .location = RoomIds::MAZE_5
    });
    
    // TUBE - Tube of putty in Maintenance Room
    createObject({
        .id = TUBE,
        .desc = "tube",
        .synonyms = {"tube", "tooth", "paste"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::READBIT},
        .size = 5,
        .capacity = 7,
        .longDesc = "There is an object which looks like a tube of toothpaste here.",
        .text = "---> Frobozz Magic Gunk Company <---\n      All-Purpose Gunk",
        .location = RoomIds::MAINTENANCE_ROOM
    });

    
    // PUTTY - Viscous material inside Tube
    createObject({
        .id = PUTTY,
        .desc = "viscous material",
        .synonyms = {"material", "gunk"},
        .adjectives = {"viscous"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TOOLBIT},
        .size = 6,
        .location = TUBE  // Inside the tube
    });
    
    // BURNED_OUT_LANTERN - In Maze-5 (by skeleton)
    createObject({
        .id = BURNED_OUT_LANTERN,
        .desc = "burned-out lantern",
        .synonyms = {"lantern", "lamp"},
        .adjectives = {"rusty", "burned", "dead", "useless"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 20,
        .text = "The deceased adventurer's useless lantern is here.",
        .location = RoomIds::MAZE_5
    });
    
    // BROKEN_LAMP - Created when lamp runs out (starts nowhere)
    createObject({
        .id = BROKEN_LAMP,
        .desc = "broken lantern",
        .synonyms = {"lamp", "lantern"},
        .adjectives = {"broken"},
        .flags = {ObjectFlag::TAKEBIT}
    });
    
} // end initializeTools
