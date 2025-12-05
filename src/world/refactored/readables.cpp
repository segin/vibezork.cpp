// Readable objects - books, documents, and other text items
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

void initializeReadables() {
    using namespace ObjectIds;
    
    // ADVERTISEMENT - Leaflet in mailbox
    createObject({
        .id = ADVERTISEMENT,
        .desc = "leaflet",
        .synonyms = {"leaflet", "advertisement", "mail", "booklet", "pamphlet"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::READBIT, ObjectFlag::BURNBIT},
        .size = 2,
        .longDesc = "A small leaflet is on the ground.",
        .text = "WELCOME TO ZORK!\n\n"
                "ZORK is a game of adventure, danger, and low cunning. In it you will "
                "explore some of the most amazing territory ever seen by mortals. No "
                "computer should be without one!",
        .location = MAILBOX  // Inside the mailbox
    });

    
    // BOOK - Black book on altar
    createObject({
        .id = BOOK,
        .desc = "black book",
        .synonyms = {"book", "tome"},
        .adjectives = {"black"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::READBIT, ObjectFlag::BURNBIT},
        .size = 5,
        .text = "Commandment #12592\n\n"
                "Oh ye who go about saying unto each: 'Hello sailor':\n"
                "Dost thou know the magnitude of thy sin before the gods?\n"
                "Yea, verily, thou shalt be ground between two stones.\n"
                "Thy remains shall be scattered to the four winds.\n"
                "Thy soul shall be cast into the pit of eternal darkness.\n"
                "Surely thou shalt repent of thy cunning.",
        .location = ALTAR  // On the altar
    });

    
    // GUIDE - Tour guidebook in Dam Lobby
    createObject({
        .id = GUIDE,
        .desc = "tour guidebook",
        .synonyms = {"guide", "guidebook", "book"},
        .adjectives = {"tour"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::READBIT, ObjectFlag::BURNBIT},
        .size = 3,
        .text = "Flood Control Dam #3\n\n"
                "FCD#3 was constructed in year 783 of the Great Underground Empire to "
                "harness the mighty Frigid River. This work was supported by a grant of "
                "37 million zorkmids from your omnipotent local tyrant Lord Dimwit "
                "Flathead the Excessive. This impressive structure is composed of "
                "370,000 cubic feet of concrete, is 256 feet tall at the center, and "
                "193 feet wide at the top.",
        .location = RoomIds::DAM_LOBBY
    });

    
    // OWNERS_MANUAL - Boat owner's manual (with inflatable boat)
    createObject({
        .id = OWNERS_MANUAL,
        .desc = "owner's manual",
        .synonyms = {"manual", "book"},
        .adjectives = {"owners", "owner's"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::READBIT, ObjectFlag::BURNBIT},
        .size = 3,
        .text = "Congratulations! You are the proud owner of a FROBOZZ MAGIC BOAT, "
                "the finest boat in the world. Your boat is guaranteed against all "
                "defects for a period of 76 years. If you have any questions about "
                "the operation of your boat, please consult your local dealer.",
        .location = BOAT_INFLATABLE  // With the boat
    });
    
    // MAP - Map in trophy case (invisible initially)
    createObject({
        .id = MAP,
        .desc = "map",
        .synonyms = {"map", "chart"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::READBIT, 
                  ObjectFlag::BURNBIT, ObjectFlag::INVISIBLE},
        .size = 2,
        .text = "The map shows a complex maze of twisty passages, all alike.",
        .location = TROPHY_CASE  // In the trophy case
    });

    
    // BOAT_LABEL - Label on inflatable boat (not takeable)
    createObject({
        .id = BOAT_LABEL,
        .desc = "label",
        .synonyms = {"label", "tag"},
        .flags = {ObjectFlag::READBIT, ObjectFlag::TRYTAKEBIT},
        .size = 1,
        .text = "FROBOZZ MAGIC BOAT COMPANY\n"
                "  Inflatable Boat\n"
                "  Capacity: 2\n"
                "  Model: FMB-1\n"
                "  Serial Number: 123456789",
        .location = BOAT_INFLATABLE  // Attached to boat
    });
    
} // end initializeReadables
