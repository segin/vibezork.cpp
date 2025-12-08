// Special objects - bells, mirrors, and other puzzle items
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

void initializeSpecialObjects() {
    using namespace ObjectIds;
    
    // BELL - Brass bell in North Temple
    createObject({
        .id = BELL,
        .desc = "brass bell",
        .synonyms = {"bell"},
        .adjectives = {"small", "brass"},
        .flags = {ObjectFlag::TAKEBIT},
        .location = RoomIds::NORTH_TEMPLE
    });
    
    // HOT_BELL - Red hot brass bell (created when bell is heated)
    createObject({
        .id = HOT_BELL,
        .desc = "red hot brass bell",
        .synonyms = {"bell"},
        .adjectives = {"brass", "hot", "red", "small"},
        .flags = {ObjectFlag::TRYTAKEBIT},
        .longDesc = "On the ground is a red hot bell."
        // No location - created when bell is heated
    });

    
    // MIRROR_1 - Mirror in Mirror Room 1
    createObject({
        .id = MIRROR_1,
        .desc = "mirror",
        .synonyms = {"mirror", "surface"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TRYTAKEBIT},
        .location = RoomIds::MIRROR_ROOM_1
    });
    
    // MIRROR_2 - Mirror in Mirror Room 2
    createObject({
        .id = MIRROR_2,
        .desc = "mirror",
        .synonyms = {"mirror", "surface"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TRYTAKEBIT},
        .location = RoomIds::MIRROR_ROOM_2
    });
    
    // DAM - Flood Control Dam #3
    createObject({
        .id = DAM,
        .desc = "dam",
        .synonyms = {"dam", "gate", "gates"},
        .adjectives = {"flood", "control"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::DAM_ROOM
    });

    
    // BOLT - Metal bolt on dam
    createObject({
        .id = BOLT,
        .desc = "bolt",
        .synonyms = {"bolt"},
        .adjectives = {"metal"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TURNBIT},
        .location = RoomIds::DAM_ROOM
    });
    
    // BUBBLE - Green bubble (appears in maintenance room)
    createObject({
        .id = BUBBLE,
        .desc = "green bubble",
        .synonyms = {"bubble"},
        .adjectives = {"green"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE}
        // Location set dynamically
    });
    
    // LEAK - Leak in maintenance room (invisible until dam opened)
    createObject({
        .id = LEAK,
        .desc = "leak",
        .synonyms = {"leak", "drip", "pipe"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE},
        .location = RoomIds::MAINTENANCE_ROOM
    });

    
    // MACHINE - Coal machine in Machine Room
    createObject({
        .id = MACHINE,
        .desc = "machine",
        .synonyms = {"machine", "lid", "slot", "switch"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::NDESCBIT, ObjectFlag::TRYTAKEBIT},
        .capacity = 50,
        .location = RoomIds::MACHINE_ROOM
    });
    
    // MACHINE_SWITCH - Switch on machine
    createObject({
        .id = MACHINE_SWITCH,
        .desc = "switch",
        .synonyms = {"switch"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::TURNBIT},
        .location = RoomIds::MACHINE_ROOM
    });
    
    // GUNK - Vitreous slag (created by machine)
    createObject({
        .id = GUNK,
        .desc = "slag",
        .synonyms = {"slag", "ite", "ite-lite"},
        .adjectives = {"vitreous"},
        .flags = {ObjectFlag::TAKEBIT}
        // No location - created by machine
    });

    
    // Control panel buttons in Maintenance Room
    createObject({
        .id = YELLOW_BUTTON,
        .desc = "yellow button",
        .synonyms = {"button"},
        .adjectives = {"yellow"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::MAINTENANCE_ROOM
    });
    
    createObject({
        .id = BROWN_BUTTON,
        .desc = "brown button",
        .synonyms = {"button"},
        .adjectives = {"brown"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::MAINTENANCE_ROOM
    });
    
    createObject({
        .id = RED_BUTTON,
        .desc = "red button",
        .synonyms = {"button"},
        .adjectives = {"red"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::MAINTENANCE_ROOM
    });
    
    createObject({
        .id = BLUE_BUTTON,
        .desc = "blue button",
        .synonyms = {"button"},
        .adjectives = {"blue"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::MAINTENANCE_ROOM
    });

    
    // RAINBOW - Rainbow at Aragain Falls
    createObject({
        .id = RAINBOW,
        .desc = "rainbow",
        .synonyms = {"rainbow"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::ARAGAIN_FALLS
    });
    
    // CONTROL_PANEL - Control panel in Dam Room
    createObject({
        .id = CONTROL_PANEL,
        .desc = "control panel",
        .synonyms = {"panel"},
        .adjectives = {"control"},
        .flags = {ObjectFlag::NDESCBIT},
        .location = RoomIds::DAM_ROOM
    });
    
} // end initializeSpecialObjects
