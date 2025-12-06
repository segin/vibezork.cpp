// House interior rooms - living room, kitchen, attic, cellar, gallery, studio
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"

void initializeHouseRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // LIVING_ROOM - Main room with trophy case
    createRoom({
        .id = LIVING_ROOM,
        .name = "Living Room",
        .longDesc = "You are in the living room. There is a doorway to the east, a wooden "
                    "door with strange gothic lettering to the west, which appears to be "
                    "nailed shut, a trophy case, and a large oriental rug in the center of "
                    "the room.",
        .exits = {
            {EAST, KITCHEN},
            {DOWN, CELLAR}  // Through trap door (requires rug moved)
        },
        .blockedExits = {
            {WEST, "The door is nailed shut."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });

    
    // KITCHEN - Kitchen with table
    createRoom({
        .id = KITCHEN,
        .name = "Kitchen",
        .longDesc = "You are in the kitchen of the white house. A table seems to have been "
                    "used recently for the preparation of food. A passage leads to the west "
                    "and a dark staircase can be seen leading upward. A dark chimney leads "
                    "down and to the east is a small window which is open.",
        .exits = {
            {WEST, LIVING_ROOM},
            {EAST, BEHIND_HOUSE},  // Through window
            {OUT, BEHIND_HOUSE},   // Through window
            {UP, ATTIC},
            {NORTH, GALLERY}
        },
        .blockedExits = {
            {DOWN, "Only Santa Claus climbs down chimneys."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });
    
    // ATTIC - Attic with knife and rope
    createRoom({
        .id = ATTIC,
        .name = "Attic",
        .longDesc = "This is the attic. The only exit is a stairway leading down.",
        .exits = {
            {DOWN, KITCHEN}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });

    
    // CELLAR - Dark cellar below living room
    createRoom({
        .id = CELLAR,
        .name = "Cellar",
        .longDesc = "You are in a dark and damp cellar with a narrow passageway leading "
                    "north, and a crawlway to the south. On the west is the bottom of a "
                    "steep metal ramp which is unclimbable.",
        .exits = {
            {NORTH, TROLL_ROOM},
            {SOUTH, EAST_OF_CHASM},
            {EAST, TROLL_ROOM},  // Alternative to north
            {UP, LIVING_ROOM}    // Through trap door
        },
        .blockedExits = {
            {WEST, "The ramp is too steep to climb."}
        },
        .flags = {RLANDBIT, SACREDBIT}  // Dark - no ONBIT
    });
    
    // GALLERY - Art gallery
    createRoom({
        .id = GALLERY,
        .name = "Gallery",
        .longDesc = "This is an art gallery. Most of the paintings which were here have "
                    "been stolen by vandals with exceptional taste. The vandals left through "
                    "either the north or west exits.",
        .exits = {
            {NORTH, STUDIO},
            {SOUTH, KITCHEN},
            {WEST, EAST_OF_CHASM}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });

    
    // STUDIO - Artist's studio
    createRoom({
        .id = STUDIO,
        .name = "Studio",
        .longDesc = "This appears to have been an artist's studio. The walls and floors are "
                    "splattered with paints of 69 different colors. Strangely enough, nothing "
                    "of value is hanging here. At the south end of the room is an open door "
                    "(also covered with paint). A dark and narrow chimney leads up from a "
                    "fireplace; although you might be able to get up it, it seems unlikely "
                    "you could get back down.",
        .exits = {
            {SOUTH, GALLERY}
        },
        .blockedExits = {
            {UP, "The chimney is too narrow and sooty."}
        },
        .flags = {RLANDBIT, ONBIT, SACREDBIT}
    });
    
} // end initializeHouseRooms
