// Container objects - objects that can hold other objects
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

// Forward declarations for action handlers
bool mailboxAction();
bool trophyCaseAction();
bool sackAction();
bool bagAction();
bool basketAction();
bool bottleAction();

void initializeContainers() {
    using namespace ObjectIds;
    
    // MAILBOX - Small mailbox at West of House
    createObject({
        .id = MAILBOX,
        .desc = "small mailbox",
        .synonyms = {"mailbox", "box"},
        .adjectives = {"small"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::TRYTAKEBIT},
        .capacity = 10,
        .location = RoomIds::WEST_OF_HOUSE,
        .action = mailboxAction
    });

    
    // TROPHY_CASE - Trophy case in Living Room
    createObject({
        .id = TROPHY_CASE,
        .desc = "trophy case",
        .synonyms = {"case"},
        .adjectives = {"trophy"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::TRANSBIT, ObjectFlag::NDESCBIT,
                  ObjectFlag::TRYTAKEBIT, ObjectFlag::SACREDBIT},
        .capacity = 100,
        .location = RoomIds::LIVING_ROOM,
        .action = trophyCaseAction
    });
    
    // SACK - Brown sack on kitchen table
    createObject({
        .id = SACK,
        .desc = "brown sack",
        .synonyms = {"sack", "bag"},
        .adjectives = {"brown", "elongated", "smelly"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, 
                  ObjectFlag::OPENBIT, ObjectFlag::BURNBIT},
        .size = 9,
        .capacity = 9,
        .longDesc = "On the table is an elongated brown sack, smelling of hot peppers.",
        .location = KITCHEN_TABLE,  // On the kitchen table
        .action = sackAction
    });

    
    // BAG - Large bag (thief's bag)
    createObject({
        .id = BAG,
        .desc = "large bag",
        .synonyms = {"bag", "sack"},
        .adjectives = {"large"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::OPENBIT},
        .size = 20,
        .capacity = 100,
        .location = THIEF,  // Initially with thief
        .action = bagAction
    });
    
    // RAISED_BASKET - Basket at top of shaft
    createObject({
        .id = RAISED_BASKET,
        .desc = "basket",
        .synonyms = {"basket", "cage", "dumbwaiter"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::TRANSBIT, 
                  ObjectFlag::OPENBIT, ObjectFlag::TRYTAKEBIT},
        .capacity = 50,
        .location = RoomIds::SHAFT_ROOM,
        .action = basketAction
    });

    
    // BASKET - Lowered basket at bottom of shaft
    createObject({
        .id = BASKET,
        .desc = "basket",
        .synonyms = {"basket", "cage", "dumbwaiter"},
        .adjectives = {"lowered"},
        .flags = {ObjectFlag::TRYTAKEBIT},
        .location = RoomIds::LOWER_SHAFT,
        .action = basketAction
    });
    
    // BUOY - Red buoy in River-4
    createObject({
        .id = BUOY,
        .desc = "red buoy",
        .synonyms = {"buoy"},
        .adjectives = {"red"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::OPENBIT},
        .size = 10,
        .capacity = 20,
        .location = RoomIds::RIVER_4
    });

    
    // NEST - Bird's nest in Up-a-Tree
    createObject({
        .id = NEST,
        .desc = "bird's nest",
        .synonyms = {"nest"},
        .adjectives = {"birds"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::OPENBIT,
                  ObjectFlag::BURNBIT, ObjectFlag::SEARCHBIT},
        .size = 5,
        .capacity = 20,
        .location = RoomIds::UP_A_TREE
    });
    
    // TOOL_CHEST - Group of tool chests in Maintenance Room
    createObject({
        .id = TOOL_CHEST,
        .desc = "group of tool chests",
        .synonyms = {"chest", "chests", "group", "toolchests"},
        .adjectives = {"tool"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::OPENBIT, 
                  ObjectFlag::TRYTAKEBIT, ObjectFlag::SACREDBIT},
        .capacity = 40,
        .location = RoomIds::MAINTENANCE_ROOM
    });

    
    // ALTAR - Altar in South Temple (surface)
    createObject({
        .id = ALTAR,
        .desc = "altar",
        .synonyms = {"altar"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::OPENBIT, 
                  ObjectFlag::SURFACEBIT, ObjectFlag::NDESCBIT},
        .capacity = 50,
        .location = RoomIds::SOUTH_TEMPLE
    });
    
    // PEDESTAL - White marble pedestal in Torch Room (surface)
    createObject({
        .id = PEDESTAL,
        .desc = "pedestal",
        .synonyms = {"pedestal"},
        .adjectives = {"white", "marble"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::OPENBIT, 
                  ObjectFlag::SURFACEBIT, ObjectFlag::NDESCBIT},
        .capacity = 30,
        .location = RoomIds::TORCH_ROOM
    });

    
    // BOTTLE - Glass bottle on kitchen table
    createObject({
        .id = BOTTLE,
        .desc = "glass bottle",
        .synonyms = {"bottle", "container"},
        .adjectives = {"glass", "clear"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::TRANSBIT},
        .size = 8,
        .capacity = 4,
        .longDesc = "A bottle is sitting on the table.",
        .location = KITCHEN_TABLE,  // On the kitchen table
        .action = bottleAction
    });
    
    // KITCHEN_TABLE - Kitchen table (surface)
    createObject({
        .id = KITCHEN_TABLE,
        .desc = "kitchen table",
        .synonyms = {"table"},
        .adjectives = {"kitchen"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CONTBIT, 
                  ObjectFlag::OPENBIT, ObjectFlag::SURFACEBIT},
        .capacity = 50,
        .location = RoomIds::KITCHEN
    });

    
    // ATTIC_TABLE - Table in Attic (surface)
    createObject({
        .id = ATTIC_TABLE,
        .desc = "table",
        .synonyms = {"table"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::CONTBIT, 
                  ObjectFlag::OPENBIT, ObjectFlag::SURFACEBIT},
        .capacity = 40,
        .location = RoomIds::ATTIC
    });
    
} // end initializeContainers
