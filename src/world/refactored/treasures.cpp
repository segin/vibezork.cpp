// Treasure objects - items with VALUE/TVALUE that can be scored
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

void initializeTreasures() {
    using namespace ObjectIds;
    
    // SKULL - Crystal skull in Land of Living Dead
    createObject({
        .id = SKULL,
        .desc = "crystal skull",
        .synonyms = {"skull", "head", "treasure"},
        .adjectives = {"crystal"},
        .flags = {ObjectFlag::TAKEBIT},
        .value = 10,
        .tvalue = 10,
        .text = "Lying in one corner of the room is a beautifully carved "
                "crystal skull. It appears to be grinning at you rather nastily.",
        .location = RoomIds::LAND_OF_LIVING_DEAD
    });
    
    // TRIDENT - Crystal trident in Atlantis Room
    createObject({
        .id = TRIDENT,
        .desc = "crystal trident",
        .synonyms = {"trident", "fork", "treasure"},
        .adjectives = {"crystal"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::WEAPONBIT},
        .size = 20,
        .value = 4,
        .tvalue = 11,
        .location = RoomIds::ATLANTIS_ROOM
    });

    
    // CHALICE - Silver chalice in Treasure Room
    createObject({
        .id = CHALICE,
        .desc = "silver chalice",
        .synonyms = {"chalice", "cup", "treasure"},
        .adjectives = {"silver"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::TRYTAKEBIT, ObjectFlag::CONTBIT},
        .size = 10,
        .capacity = 5,
        .value = 10,
        .tvalue = 5,
        .location = RoomIds::TREASURE_ROOM
    });
    
    // COFFIN - Gold coffin in Egyptian Room
    createObject({
        .id = COFFIN,
        .desc = "gold coffin",
        .synonyms = {"coffin", "casket", "treasure"},
        .adjectives = {"gold", "golden", "egyptian"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT},
        .size = 55,
        .capacity = 35,
        .value = 10,
        .tvalue = 15,
        .location = RoomIds::EGYPTIAN_ROOM
    });

    
    // TORCH - Ivory torch on pedestal in Torch Room
    createObject({
        .id = TORCH,
        .desc = "ivory torch",
        .synonyms = {"torch", "treasure"},
        .adjectives = {"ivory"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::LIGHTBIT, ObjectFlag::ONBIT, 
                  ObjectFlag::FLAMEBIT},
        .size = 20,
        .value = 14,
        .tvalue = 6,
        .location = PEDESTAL  // On the pedestal
    });
    
    // SCEPTRE - Egyptian sceptre in Coffin
    createObject({
        .id = SCEPTRE,
        .desc = "sceptre",
        .synonyms = {"sceptre", "scepter", "treasure"},
        .adjectives = {"egyptian"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::WEAPONBIT},
        .size = 3,
        .value = 4,
        .tvalue = 6,
        .location = COFFIN  // Inside the coffin
    });

    
    // BAR - Platinum bar in Loud Room
    createObject({
        .id = BAR,
        .desc = "platinum bar",
        .synonyms = {"bar", "platinum", "treasure"},
        .adjectives = {"platinum", "large"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::SACREDBIT},
        .size = 20,
        .value = 10,
        .tvalue = 5,
        .location = RoomIds::LOUD_ROOM
    });
    
    // POT_OF_GOLD - At end of rainbow (invisible until rainbow solid)
    createObject({
        .id = POT_OF_GOLD,
        .desc = "pot of gold",
        .synonyms = {"pot", "gold", "treasure"},
        .adjectives = {"large"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::INVISIBLE},
        .size = 15,
        .value = 10,
        .tvalue = 10,
        .location = RoomIds::END_OF_RAINBOW
    });

    
    // SCARAB - Jeweled scarab (hidden in sand)
    createObject({
        .id = SCARAB,
        .desc = "jewel-encrusted scarab",
        .synonyms = {"scarab", "beetle", "treasure"},
        .adjectives = {"jewel", "encrusted", "jeweled"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::INVISIBLE},
        .size = 8,
        .value = 5,
        .tvalue = 5,
        .location = RoomIds::SANDY_CAVE
    });
    
    // JEWELS - Trunk of jewels (hidden in reservoir)
    createObject({
        .id = JEWELS,
        .desc = "trunk of jewels",
        .synonyms = {"trunk", "jewels", "treasure"},
        .adjectives = {"old"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::INVISIBLE},
        .size = 35,
        .value = 15,
        .tvalue = 5,
        .location = RoomIds::RESERVOIR
    });

    
    // COINS - Bag of coins in Maze-5
    createObject({
        .id = COINS,
        .desc = "bag of coins",
        .synonyms = {"bag", "coins", "treasure"},
        .adjectives = {"leather"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 15,
        .value = 10,
        .tvalue = 5,
        .location = RoomIds::MAZE_5
    });
    
    // DIAMOND - Huge diamond (appears when thief killed)
    createObject({
        .id = DIAMOND,
        .desc = "huge diamond",
        .synonyms = {"diamond", "gem", "treasure"},
        .adjectives = {"huge", "enormous"},
        .flags = {ObjectFlag::TAKEBIT},
        .value = 10,
        .tvalue = 10
        // No location - appears when thief is killed
    });

    
    // EMERALD - Large emerald (inside buoy)
    createObject({
        .id = EMERALD,
        .desc = "large emerald",
        .synonyms = {"emerald", "gem", "treasure"},
        .adjectives = {"large"},
        .flags = {ObjectFlag::TAKEBIT},
        .value = 5,
        .tvalue = 10,
        .location = BUOY  // Inside the buoy
    });
    
    // PAINTING - Painting in Gallery
    createObject({
        .id = PAINTING,
        .desc = "painting",
        .synonyms = {"painting", "art", "treasure", "canvas"},
        .adjectives = {"beautiful"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 15,
        .value = 4,
        .tvalue = 6,
        .location = RoomIds::GALLERY
    });

    
    // JADE - Jade figurine in Bat Room
    createObject({
        .id = JADE,
        .desc = "jade figurine",
        .synonyms = {"jade", "figurine", "treasure", "statue"},
        .adjectives = {"jade", "small"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 10,
        .value = 5,
        .tvalue = 5,
        .location = RoomIds::BAT_ROOM
    });
    
    // BRACELET - Sapphire bracelet in Gas Room
    createObject({
        .id = BRACELET,
        .desc = "sapphire-encrusted bracelet",
        .synonyms = {"bracelet", "treasure"},
        .adjectives = {"sapphire", "encrusted"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 10,
        .value = 5,
        .tvalue = 5,
        .location = RoomIds::GAS_ROOM
    });

    
    // EGG - Jewel-encrusted egg (in nest, in tree)
    createObject({
        .id = EGG,
        .desc = "jewel-encrusted egg",
        .synonyms = {"egg", "treasure"},
        .adjectives = {"jewel", "encrusted", "jeweled"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT},
        .size = 6,
        .capacity = 6,
        .value = 5,
        .tvalue = 5,
        .location = NEST  // Inside the nest
    });
    
    // BROKEN_EGG - Created when egg is opened wrong
    createObject({
        .id = BROKEN_EGG,
        .desc = "broken jewel-encrusted egg",
        .synonyms = {"egg", "treasure"},
        .adjectives = {"broken", "jewel", "encrusted"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::CONTBIT, ObjectFlag::OPENBIT},
        .size = 6,
        .capacity = 6
        // No location - created when egg breaks
    });

    
    // CANARY - Clockwork canary (inside egg)
    createObject({
        .id = CANARY,
        .desc = "clockwork canary",
        .synonyms = {"canary", "bird", "treasure"},
        .adjectives = {"clockwork", "golden"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 4,
        .value = 6,
        .tvalue = 4,
        .location = EGG  // Inside the egg
    });
    
    // BROKEN_CANARY - Created when canary breaks
    createObject({
        .id = BROKEN_CANARY,
        .desc = "broken clockwork canary",
        .synonyms = {"canary", "bird"},
        .adjectives = {"broken", "clockwork"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 4
        // No location - created when canary breaks
    });

    
    // BAUBLE - Brass bauble (from thief)
    createObject({
        .id = BAUBLE,
        .desc = "brass bauble",
        .synonyms = {"bauble", "treasure"},
        .adjectives = {"brass", "beautiful"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 3,
        .value = 1,
        .tvalue = 1
        // No location - given by thief
    });
    
    // TROPHY - Trophy (in trophy case)
    createObject({
        .id = TROPHY,
        .desc = "gold trophy",
        .synonyms = {"trophy", "treasure"},
        .adjectives = {"gold", "golden"},
        .flags = {ObjectFlag::TAKEBIT},
        .size = 10,
        .value = 5,
        .tvalue = 5,
        .location = TROPHY_CASE
    });
    
} // end initializeTreasures
