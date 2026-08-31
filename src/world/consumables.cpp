// Consumable objects - food and drink items
// Values sourced directly from ZIL 1dungeon.zil

#include "object_builder.h"

extern bool waterAction();

void initializeConsumables() {
    using namespace ObjectIds;
    
    // LUNCH - Hot pepper sandwich in sack
    createObject({
        .id = LUNCH,
        .desc = "lunch",
        .synonyms = {"lunch", "sandwich", "pepper"},
        .adjectives = {"hot", "pepper"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::FOODBIT},
        .size = 4,
        .longDesc = "A hot pepper sandwich is here.",
        .location = SACK  // Inside the sack
    });
    
    // GARLIC - Clove of garlic in sack
    createObject({
        .id = GARLIC,
        .desc = "clove of garlic",
        .synonyms = {"garlic", "clove"},
        .flags = {ObjectFlag::TAKEBIT, ObjectFlag::FOODBIT},
        .size = 2,
        .location = SACK  // Inside the sack
    });

    
    // WATER - Quantity of water in bottle (ZIL: 1dungeon.zil:100-106)
    createObject({
        .id = WATER,
        .desc = "quantity of water",
        .synonyms = {"water", "quantity", "liquid", "h2o"},
        .flags = {ObjectFlag::TRYTAKEBIT, ObjectFlag::TAKEBIT, ObjectFlag::DRINKBIT},
        .size = 4,
        .location = BOTTLE,  // Inside the bottle
        .action = waterAction
    });
    
} // end initializeConsumables
