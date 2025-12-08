// NPC objects - non-player characters and creatures
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

void initializeNPCs() {
    using namespace ObjectIds;
    
    // THIEF - Seedy-looking individual who wanders and steals
    createObject({
        .id = THIEF,
        .desc = "thief",
        .synonyms = {"thief", "robber", "burglar"},
        .adjectives = {"seedy", "sinister"},
        .flags = {ObjectFlag::FIGHTBIT, ObjectFlag::ACTORBIT, 
                  ObjectFlag::INVISIBLE, ObjectFlag::CONTBIT, ObjectFlag::OPENBIT},
        .strength = 5,
        .longDesc = "There is a suspicious-looking individual, holding a large bag, "
                    "leaning against one wall. He is armed with a deadly stiletto.",
        .location = RoomIds::ROUND_ROOM  // Per ZIL, starts in Round Room
    });

    
    // STILETTO - Thief's weapon
    createObject({
        .id = STILETTO,
        .desc = "stiletto",
        .synonyms = {"stiletto", "knife"},
        .adjectives = {"nasty"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::WEAPONBIT},
        .size = 5,
        .strength = 4,
        .location = THIEF  // Starts with thief
    });
    
    // TROLL - Nasty troll blocking passages
    createObject({
        .id = TROLL,
        .desc = "troll",
        .synonyms = {"troll"},
        .flags = {ObjectFlag::FIGHTBIT, ObjectFlag::ACTORBIT, ObjectFlag::OPENBIT},
        .strength = 2,  // Per ZIL STRENGTH 2
        .longDesc = "A nasty-looking troll, brandishing a bloody axe, "
                    "blocks all passages out of the room.",
        .location = RoomIds::TROLL_ROOM
    });

    
    // AXE - Troll's bloody axe
    createObject({
        .id = AXE,
        .desc = "bloody axe",
        .synonyms = {"axe"},
        .adjectives = {"bloody", "troll's"},
        .flags = {ObjectFlag::WEAPONBIT, ObjectFlag::NDESCBIT},
        .size = 25,
        .strength = 7,
        .location = TROLL  // Starts with troll
    });
    
    // CYCLOPS - Giant cyclops (essentially invincible in combat)
    createObject({
        .id = CYCLOPS,
        .desc = "cyclops",
        .synonyms = {"cyclops", "giant", "monster", "creature"},
        .flags = {ObjectFlag::FIGHTBIT, ObjectFlag::ACTORBIT},
        .strength = 10000,  // Per ZIL - essentially invincible
        .location = RoomIds::CYCLOPS_ROOM
    });

    
    // GRUE - Invisible creature in darkness (never seen)
    createObject({
        .id = GRUE,
        .desc = "grue",
        .synonyms = {"grue"},
        .flags = {ObjectFlag::INVISIBLE}
        // No location - exists only in darkness
    });
    
    // BAT - Vampire bat in Bat Room
    createObject({
        .id = BAT,
        .desc = "bat",
        .synonyms = {"bat", "vampire"},
        .adjectives = {"vampire", "deranged"},
        .flags = {ObjectFlag::ACTORBIT, ObjectFlag::TRYTAKEBIT},
        .location = RoomIds::BAT_ROOM
    });
    
    // GHOSTS - Spirits in Entrance to Hades
    createObject({
        .id = GHOSTS,
        .desc = "number of ghosts",
        .synonyms = {"ghosts", "spirits", "fiends", "force"},
        .adjectives = {"invisible", "evil"},
        .flags = {ObjectFlag::ACTORBIT, ObjectFlag::NDESCBIT},
        .location = RoomIds::ENTRANCE_TO_HADES
    });
    
} // end initializeNPCs
