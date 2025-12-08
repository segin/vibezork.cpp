// Vehicle objects - boats and other transportation
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

// Forward declarations for action handlers
bool boatAction();

void initializeVehicles() {
    using namespace ObjectIds;
    
    // BOAT_INFLATABLE - Deflated rubber boat at Dam Base
    createObject({
        .id = BOAT_INFLATABLE,
        .desc = "pile of plastic",
        .synonyms = {"boat", "raft", "pile", "plastic"},
        .adjectives = {"rubber", "deflated", "small"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::BURNBIT},
        .size = 10,
        .longDesc = "There is a folded pile of plastic here which has a small valve attached.",
        .location = RoomIds::DAM_BASE,
        .action = boatAction
    });

    
    // BOAT_INFLATED - Inflated rubber boat (created when inflated)
    createObject({
        .id = BOAT_INFLATED,
        .desc = "rubber boat",
        .synonyms = {"boat", "raft"},
        .adjectives = {"rubber", "inflated", "small"},
        .flags = {ObjectFlag::CONTBIT, ObjectFlag::VEHBIT, ObjectFlag::OPENBIT},
        .size = 50,
        .capacity = 100,
        .action = boatAction
        // No location - created when inflatable boat is inflated
    });
    
    // BOAT_PUNCTURED - Punctured rubber boat (created when punctured)
    createObject({
        .id = BOAT_PUNCTURED,
        .desc = "punctured boat",
        .synonyms = {"boat", "raft"},
        .adjectives = {"punctured", "rubber", "useless"},
        .flags = {ObjectFlag::TAKEBIT}
        // No location - created when boat is punctured
    });
    
} // end initializeVehicles
