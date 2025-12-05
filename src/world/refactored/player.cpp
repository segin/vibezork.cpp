// Player object - the adventurer
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

void initializePlayer() {
    using namespace ObjectIds;
    
    // ADVENTURER - The player character
    createObject({
        .id = ADVENTURER,
        .desc = "adventurer",
        .synonyms = {"adventurer", "me", "myself", "self"},
        .flags = {ObjectFlag::NDESCBIT, ObjectFlag::INVISIBLE, 
                  ObjectFlag::SACREDBIT, ObjectFlag::ACTORBIT},
        .strength = 10,  // Player strength
        .location = RoomIds::WEST_OF_HOUSE  // Starting location
    });
    
} // end initializePlayer
