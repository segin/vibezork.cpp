#include "world.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include <memory>

// Global flag for winning the game
bool wonFlag = false;

// Room action functions
void westHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        print("You are standing in an open field west of a white house, with a boarded front door.");
        if (wonFlag) {
            print(" A secret path leads southwest into the forest.");
        }
        crlf();
    }
}

void northHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees.");
    }
}

void southHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the south side of a white house. There is no door here, and all the windows are boarded.");
    }
}

void behindHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are behind the white house. A path leads into the forest to the east. In one corner of the house there is a small window which is slightly ajar.");
    }
}

void stoneBarrowAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are standing in front of a massive barrow of stone. In the east face is a huge stone door which is open. You cannot see into the dark of the tomb.");
    }
}

// Object action functions
bool mailboxAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE && g.prso && g.prso->getId() == OBJ_MAILBOX) {
        printLine("It is securely anchored.");
        return RTRUE;
    }
    return RFALSE;
}

bool whiteHouseAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The house is a beautiful colonial house which is painted white. It is clear that the owners must have been extremely wealthy.");
        return RTRUE;
    }
    return RFALSE;
}

bool boardAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("The boards are securely fastened.");
        return RTRUE;
    }
    return RFALSE;
}

bool forestAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The forest is a deep, dark, and foreboding place.");
        return RTRUE;
    }
    return RFALSE;
}

bool kitchenWindowAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The window is slightly ajar, but not enough to allow entry.");
        return RTRUE;
    }
    return RFALSE;
}

void initializeWorld() {
    auto& g = Globals::instance();
    
    // Create West of House room
    auto westOfHouse = std::make_unique<ZRoom>(
        ROOM_WEST_OF_HOUSE,
        "West of House",
        "You are standing in an open field west of a white house, with a boarded front door."
    );
    westOfHouse->setFlag(ObjectFlag::RLANDBIT);
    westOfHouse->setFlag(ObjectFlag::ONBIT);
    westOfHouse->setFlag(ObjectFlag::SACREDBIT);
    westOfHouse->setRoomAction(westHouseAction);
    
    // Set up exits
    westOfHouse->setExit(Direction::NORTH, RoomExit(ROOM_NORTH_OF_HOUSE));
    westOfHouse->setExit(Direction::SOUTH, RoomExit(ROOM_SOUTH_OF_HOUSE));
    westOfHouse->setExit(Direction::NE, RoomExit(ROOM_NORTH_OF_HOUSE));
    westOfHouse->setExit(Direction::SE, RoomExit(ROOM_SOUTH_OF_HOUSE));
    westOfHouse->setExit(Direction::WEST, RoomExit(ROOM_FOREST_1));
    westOfHouse->setExit(Direction::EAST, RoomExit("The door is boarded and you can't remove the boards."));
    // SW and IN to STONE_BARROW require WON-FLAG - will be handled by conditional exits later
    
    g.here = westOfHouse.get();
    g.registerObject(ROOM_WEST_OF_HOUSE, std::move(westOfHouse));
    
    // Create North of House room
    auto northOfHouse = std::make_unique<ZRoom>(
        ROOM_NORTH_OF_HOUSE,
        "North of House",
        "You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees."
    );
    northOfHouse->setFlag(ObjectFlag::RLANDBIT);
    northOfHouse->setFlag(ObjectFlag::ONBIT);
    northOfHouse->setFlag(ObjectFlag::SACREDBIT);
    northOfHouse->setRoomAction(northHouseAction);
    
    // Set up exits
    northOfHouse->setExit(Direction::SW, RoomExit(ROOM_WEST_OF_HOUSE));
    northOfHouse->setExit(Direction::SE, RoomExit(ROOM_EAST_OF_HOUSE));
    northOfHouse->setExit(Direction::WEST, RoomExit(ROOM_WEST_OF_HOUSE));
    northOfHouse->setExit(Direction::EAST, RoomExit(ROOM_EAST_OF_HOUSE));
    northOfHouse->setExit(Direction::NORTH, RoomExit(RoomIds::FOREST_PATH));  // Path to forest
    northOfHouse->setExit(Direction::SOUTH, RoomExit("The windows are all boarded."));
    
    g.registerObject(ROOM_NORTH_OF_HOUSE, std::move(northOfHouse));
    
    // Create South of House room
    auto southOfHouse = std::make_unique<ZRoom>(
        ROOM_SOUTH_OF_HOUSE,
        "South of House",
        "You are facing the south side of a white house. There is no door here, and all the windows are boarded."
    );
    southOfHouse->setFlag(ObjectFlag::RLANDBIT);
    southOfHouse->setFlag(ObjectFlag::ONBIT);
    southOfHouse->setFlag(ObjectFlag::SACREDBIT);
    southOfHouse->setRoomAction(southHouseAction);
    
    // Set up exits
    southOfHouse->setExit(Direction::WEST, RoomExit(ROOM_WEST_OF_HOUSE));
    southOfHouse->setExit(Direction::EAST, RoomExit(ROOM_EAST_OF_HOUSE));
    southOfHouse->setExit(Direction::NE, RoomExit(ROOM_EAST_OF_HOUSE));
    southOfHouse->setExit(Direction::NW, RoomExit(ROOM_WEST_OF_HOUSE));
    southOfHouse->setExit(Direction::SOUTH, RoomExit(RoomIds::FOREST_3));  // Forest to south
    southOfHouse->setExit(Direction::NORTH, RoomExit("The windows are all boarded."));
    
    g.registerObject(ROOM_SOUTH_OF_HOUSE, std::move(southOfHouse));
    
    // Create Behind House room (EAST_OF_HOUSE in ZIL)
    auto behindHouse = std::make_unique<ZRoom>(
        ROOM_EAST_OF_HOUSE,
        "Behind House",
        "You are behind the white house. A path leads into the forest to the east. In one corner of the house there is a small window which is slightly ajar."
    );
    behindHouse->setFlag(ObjectFlag::RLANDBIT);
    behindHouse->setFlag(ObjectFlag::ONBIT);
    behindHouse->setFlag(ObjectFlag::SACREDBIT);
    behindHouse->setRoomAction(behindHouseAction);
    
    // Set up exits
    behindHouse->setExit(Direction::NORTH, RoomExit(ROOM_NORTH_OF_HOUSE));
    behindHouse->setExit(Direction::SOUTH, RoomExit(ROOM_SOUTH_OF_HOUSE));
    behindHouse->setExit(Direction::SW, RoomExit(ROOM_SOUTH_OF_HOUSE));
    behindHouse->setExit(Direction::NW, RoomExit(ROOM_NORTH_OF_HOUSE));
    behindHouse->setExit(Direction::EAST, RoomExit(RoomIds::CLEARING));
    behindHouse->setExit(Direction::WEST, RoomExit(RoomIds::KITCHEN));  // Through window
    behindHouse->setExit(Direction::IN, RoomExit(RoomIds::KITCHEN));  // Through window
    
    g.registerObject(ROOM_EAST_OF_HOUSE, std::move(behindHouse));
    
    // Create Stone Barrow room
    auto stoneBarrow = std::make_unique<ZRoom>(
        ROOM_STONE_BARROW,
        "Stone Barrow",
        "You are standing in front of a massive barrow of stone. In the east face is a huge stone door which is open. You cannot see into the dark of the tomb."
    );
    stoneBarrow->setFlag(ObjectFlag::RLANDBIT);
    stoneBarrow->setFlag(ObjectFlag::ONBIT);
    stoneBarrow->setFlag(ObjectFlag::SACREDBIT);
    stoneBarrow->setRoomAction(stoneBarrowAction);
    
    // Set up exits
    stoneBarrow->setExit(Direction::NE, RoomExit(ROOM_WEST_OF_HOUSE));
    // IN to underground entrance will be added when underground rooms are implemented
    
    g.registerObject(ROOM_STONE_BARROW, std::move(stoneBarrow));
    
    // Create Forest-1
    auto forest1 = std::make_unique<ZRoom>(
        ROOM_FOREST_1,
        "Forest",
        "This is a forest, with trees in all directions. To the east, there appears to be sunlight."
    );
    forest1->setFlag(ObjectFlag::RLANDBIT);
    forest1->setFlag(ObjectFlag::ONBIT);
    forest1->setFlag(ObjectFlag::SACREDBIT);
    forest1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a forest, with trees in all directions. To the east, there appears to be sunlight.");
        }
    });
    // Exits will be set up in forest navigation section
    forest1->setExit(Direction::UP, RoomExit("There is no tree here suitable for climbing."));
    forest1->setExit(Direction::WEST, RoomExit("You would need a machete to go further west."));
    
    g.registerObject(ROOM_FOREST_1, std::move(forest1));
    
    // Create Forest-2
    auto forest2 = std::make_unique<ZRoom>(
        RoomIds::FOREST_2,
        "Forest",
        "This is a dimly lit forest, with large trees all around."
    );
    forest2->setFlag(ObjectFlag::RLANDBIT);
    forest2->setFlag(ObjectFlag::ONBIT);
    forest2->setFlag(ObjectFlag::SACREDBIT);
    forest2->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a dimly lit forest, with large trees all around.");
        }
    });
    // Exits will be set up in forest navigation section
    forest2->setExit(Direction::UP, RoomExit("There is no tree here suitable for climbing."));
    forest2->setExit(Direction::NORTH, RoomExit("The forest becomes impenetrable to the north."));
    
    g.registerObject(RoomIds::FOREST_2, std::move(forest2));
    
    // Create Forest-3
    auto forest3 = std::make_unique<ZRoom>(
        RoomIds::FOREST_3,
        "Forest",
        "This is a dimly lit forest, with large trees all around."
    );
    forest3->setFlag(ObjectFlag::RLANDBIT);
    forest3->setFlag(ObjectFlag::ONBIT);
    forest3->setFlag(ObjectFlag::SACREDBIT);
    forest3->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a dimly lit forest, with large trees all around.");
        }
    });
    // Exits will be set up in forest navigation section
    forest3->setExit(Direction::UP, RoomExit("There is no tree here suitable for climbing."));
    forest3->setExit(Direction::EAST, RoomExit("The rank undergrowth prevents eastward movement."));
    forest3->setExit(Direction::SOUTH, RoomExit("Storm-tossed trees block your way."));
    
    g.registerObject(RoomIds::FOREST_3, std::move(forest3));
    
    // Create Mountains room
    auto mountains = std::make_unique<ZRoom>(
        RoomIds::MOUNTAINS,
        "Forest",
        "The forest thins out, revealing impassable mountains."
    );
    mountains->setFlag(ObjectFlag::RLANDBIT);
    mountains->setFlag(ObjectFlag::ONBIT);
    mountains->setFlag(ObjectFlag::SACREDBIT);
    mountains->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("The forest thins out, revealing impassable mountains.");
        }
    });
    // Mountains is a dead end - all directions lead back to FOREST_2
    mountains->setExit(Direction::UP, RoomExit("The mountains are impassable."));
    mountains->setExit(Direction::NORTH, RoomExit(RoomIds::FOREST_2));
    mountains->setExit(Direction::EAST, RoomExit("The mountains are impassable."));
    mountains->setExit(Direction::SOUTH, RoomExit(RoomIds::FOREST_2));
    mountains->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_2));
    
    g.registerObject(RoomIds::MOUNTAINS, std::move(mountains));
    
    // Create Clearing room
    auto clearing = std::make_unique<ZRoom>(
        RoomIds::CLEARING,
        "Clearing",
        "You are in a small clearing in a well marked forest path that extends to the east and west."
    );
    clearing->setFlag(ObjectFlag::RLANDBIT);
    clearing->setFlag(ObjectFlag::ONBIT);
    clearing->setFlag(ObjectFlag::SACREDBIT);
    clearing->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a small clearing in a well marked forest path that extends to the east and west.");
        }
    });
    // Exits will be set up in forest navigation section
    clearing->setExit(Direction::UP, RoomExit("There is no tree here suitable for climbing."));
    clearing->setExit(Direction::EAST, RoomExit(RoomIds::CANYON_VIEW));
    clearing->setExit(Direction::NORTH, RoomExit(RoomIds::FOREST_2));
    clearing->setExit(Direction::SOUTH, RoomExit(RoomIds::FOREST_3));
    clearing->setExit(Direction::WEST, RoomExit(ROOM_EAST_OF_HOUSE));
    
    g.registerObject(RoomIds::CLEARING, std::move(clearing));
    
    // Create Grating Clearing room
    auto gratingClearing = std::make_unique<ZRoom>(
        RoomIds::GRATING_CLEARING,
        "Clearing",
        "You are in a clearing, with a forest surrounding you on all sides. A path leads south."
    );
    gratingClearing->setFlag(ObjectFlag::RLANDBIT);
    gratingClearing->setFlag(ObjectFlag::ONBIT);
    gratingClearing->setFlag(ObjectFlag::SACREDBIT);
    gratingClearing->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a clearing, with a forest surrounding you on all sides. A path leads south.");
        }
    });
    // Exits will be set up in forest navigation section
    gratingClearing->setExit(Direction::NORTH, RoomExit("The forest becomes impenetrable to the north."));
    gratingClearing->setExit(Direction::EAST, RoomExit(RoomIds::FOREST_2));
    gratingClearing->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_1));
    gratingClearing->setExit(Direction::SOUTH, RoomExit(RoomIds::FOREST_PATH));
    // DOWN to underground will be added when grating is implemented
    
    g.registerObject(RoomIds::GRATING_CLEARING, std::move(gratingClearing));
    
    // Create Forest Path room
    auto forestPath = std::make_unique<ZRoom>(
        RoomIds::FOREST_PATH,
        "Forest Path",
        "This is a path winding through a dimly lit forest. The path heads north-south here. One particularly large tree with some low branches stands at the edge of the path."
    );
    forestPath->setFlag(ObjectFlag::RLANDBIT);
    forestPath->setFlag(ObjectFlag::ONBIT);
    forestPath->setFlag(ObjectFlag::SACREDBIT);
    forestPath->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a path winding through a dimly lit forest. The path heads north-south here. One particularly large tree with some low branches stands at the edge of the path.");
        }
    });
    // Exits will be set up in forest navigation section
    forestPath->setExit(Direction::UP, RoomExit(RoomIds::UP_A_TREE));
    forestPath->setExit(Direction::NORTH, RoomExit(RoomIds::GRATING_CLEARING));
    forestPath->setExit(Direction::EAST, RoomExit(RoomIds::FOREST_2));
    forestPath->setExit(Direction::SOUTH, RoomExit(ROOM_NORTH_OF_HOUSE));
    forestPath->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_1));
    
    g.registerObject(RoomIds::FOREST_PATH, std::move(forestPath));
    
    // Create Up a Tree room
    auto upATree = std::make_unique<ZRoom>(
        RoomIds::UP_A_TREE,
        "Up a Tree",
        "You are about 10 feet above the ground nestled among some large branches. The nearest branch above you is above your reach."
    );
    upATree->setFlag(ObjectFlag::RLANDBIT);
    upATree->setFlag(ObjectFlag::ONBIT);
    upATree->setFlag(ObjectFlag::SACREDBIT);
    upATree->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are about 10 feet above the ground nestled among some large branches. The nearest branch above you is above your reach.");
        }
    });
    upATree->setExit(Direction::UP, RoomExit("You cannot climb any higher."));
    upATree->setExit(Direction::DOWN, RoomExit(RoomIds::FOREST_PATH));
    
    g.registerObject(RoomIds::UP_A_TREE, std::move(upATree));
    
    // Create Canyon View room
    auto canyonView = std::make_unique<ZRoom>(
        RoomIds::CANYON_VIEW,
        "Canyon View",
        "You are at the top of the Great Canyon on its west wall. From here there is a marvelous view of the canyon and parts of the Frigid River upstream. Across the canyon, the walls of the White Cliffs join the mighty ramparts of the Flathead Mountains to the east. Following the Canyon upstream to the north, Aragain Falls may be seen, complete with rainbow. The mighty Frigid River flows out from a great dark cavern. To the west and south can be seen an immense forest, stretching for miles around. A path leads northwest."
    );
    canyonView->setFlag(ObjectFlag::RLANDBIT);
    canyonView->setFlag(ObjectFlag::ONBIT);
    canyonView->setFlag(ObjectFlag::SACREDBIT);
    canyonView->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are at the top of the Great Canyon on its west wall. From here there is a marvelous view of the canyon and parts of the Frigid River upstream. Across the canyon, the walls of the White Cliffs join the mighty ramparts of the Flathead Mountains to the east. Following the Canyon upstream to the north, Aragain Falls may be seen, complete with rainbow. The mighty Frigid River flows out from a great dark cavern. To the west and south can be seen an immense forest, stretching for miles around. A path leads northwest.");
        }
    });
    canyonView->setExit(Direction::WEST, RoomExit(RoomIds::CLEARING));
    canyonView->setExit(Direction::NW, RoomExit(RoomIds::CLEARING));
    canyonView->setExit(Direction::NORTH, RoomExit("The canyon is too wide to cross."));
    canyonView->setExit(Direction::EAST, RoomExit("The canyon is too wide to cross."));
    canyonView->setExit(Direction::SOUTH, RoomExit("The canyon is too wide to cross."));
    canyonView->setExit(Direction::DOWN, RoomExit("The canyon is too deep to climb down."));
    
    g.registerObject(RoomIds::CANYON_VIEW, std::move(canyonView));
    
    // ===== HOUSE INTERIOR ROOMS =====
    
    // Create Living Room
    auto livingRoom = std::make_unique<ZRoom>(
        RoomIds::LIVING_ROOM,
        "Living Room",
        "You are in the living room. There is a doorway to the east, a wooden door with strange gothic lettering to the west, which appears to be nailed shut, a trophy case, and a large oriental rug in the center of the room."
    );
    livingRoom->setFlag(ObjectFlag::RLANDBIT);
    livingRoom->setFlag(ObjectFlag::ONBIT);
    livingRoom->setFlag(ObjectFlag::SACREDBIT);
    livingRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in the living room. There is a doorway to the east, a wooden door with strange gothic lettering to the west, which appears to be nailed shut, a trophy case, and a large oriental rug in the center of the room.");
        }
    });
    
    // Set up exits for Living Room
    livingRoom->setExit(Direction::EAST, RoomExit(RoomIds::KITCHEN));
    livingRoom->setExit(Direction::WEST, RoomExit("The door is nailed shut."));
    livingRoom->setExit(Direction::DOWN, RoomExit(RoomIds::CELLAR));  // Through trap door (will need rug moved)
    
    g.registerObject(RoomIds::LIVING_ROOM, std::move(livingRoom));
    
    // Create Kitchen
    auto kitchen = std::make_unique<ZRoom>(
        RoomIds::KITCHEN,
        "Kitchen",
        "You are in the kitchen of the white house. A table seems to have been used recently for the preparation of food. A passage leads to the west and a dark staircase can be seen leading upward. A dark chimney leads down and to the east is a small window which is open."
    );
    kitchen->setFlag(ObjectFlag::RLANDBIT);
    kitchen->setFlag(ObjectFlag::ONBIT);
    kitchen->setFlag(ObjectFlag::SACREDBIT);
    kitchen->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in the kitchen of the white house. A table seems to have been used recently for the preparation of food. A passage leads to the west and a dark staircase can be seen leading upward. A dark chimney leads down and to the east is a small window which is open.");
        }
    });
    
    // Set up exits for Kitchen
    kitchen->setExit(Direction::WEST, RoomExit(RoomIds::LIVING_ROOM));
    kitchen->setExit(Direction::EAST, RoomExit(ROOM_EAST_OF_HOUSE));  // Through window
    kitchen->setExit(Direction::UP, RoomExit(RoomIds::ATTIC));
    kitchen->setExit(Direction::DOWN, RoomExit("Only Santa Claus climbs down chimneys."));
    
    g.registerObject(RoomIds::KITCHEN, std::move(kitchen));
    
    // Create Attic
    auto attic = std::make_unique<ZRoom>(
        RoomIds::ATTIC,
        "Attic",
        "This is the attic. The only exit is a stairway leading down."
    );
    attic->setFlag(ObjectFlag::RLANDBIT);
    attic->setFlag(ObjectFlag::ONBIT);
    attic->setFlag(ObjectFlag::SACREDBIT);
    attic->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is the attic. The only exit is a stairway leading down.");
        }
    });
    
    // Set up exits for Attic
    attic->setExit(Direction::DOWN, RoomExit(RoomIds::KITCHEN));
    
    g.registerObject(RoomIds::ATTIC, std::move(attic));
    
    // Create Cellar
    auto cellar = std::make_unique<ZRoom>(
        RoomIds::CELLAR,
        "Cellar",
        "You are in a dark and damp cellar with a narrow passageway leading north, and a crawlway to the south. On the west is the bottom of a steep metal ramp which is unclimbable."
    );
    // Cellar is dark - no ONBIT flag
    cellar->setFlag(ObjectFlag::RLANDBIT);
    cellar->setFlag(ObjectFlag::SACREDBIT);
    cellar->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a dark and damp cellar with a narrow passageway leading north, and a crawlway to the south. On the west is the bottom of a steep metal ramp which is unclimbable.");
        }
    });
    
    // Set up exits for Cellar
    cellar->setExit(Direction::NORTH, RoomExit(RoomIds::TROLL_ROOM));  // To underground
    cellar->setExit(Direction::SOUTH, RoomExit("The crawlway is too small to enter."));
    cellar->setExit(Direction::WEST, RoomExit("The ramp is too steep to climb."));
    cellar->setExit(Direction::UP, RoomExit(RoomIds::LIVING_ROOM));  // Through trap door
    cellar->setExit(Direction::EAST, RoomExit(RoomIds::TROLL_ROOM));  // Alternative to north
    
    g.registerObject(RoomIds::CELLAR, std::move(cellar));
    
    // Create Gallery
    auto gallery = std::make_unique<ZRoom>(
        RoomIds::GALLERY,
        "Gallery",
        "This is an art gallery. Most of the paintings which were here have been stolen by vandals with exceptional taste. The vandals left through either the north or west exits."
    );
    gallery->setFlag(ObjectFlag::RLANDBIT);
    gallery->setFlag(ObjectFlag::ONBIT);
    gallery->setFlag(ObjectFlag::SACREDBIT);
    gallery->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is an art gallery. Most of the paintings which were here have been stolen by vandals with exceptional taste. The vandals left through either the north or west exits.");
        }
    });
    
    // Set up exits for Gallery
    gallery->setExit(Direction::NORTH, RoomExit(RoomIds::STUDIO));
    gallery->setExit(Direction::WEST, RoomExit(RoomIds::KITCHEN));
    
    g.registerObject(RoomIds::GALLERY, std::move(gallery));
    
    // Create Studio
    auto studio = std::make_unique<ZRoom>(
        RoomIds::STUDIO,
        "Studio",
        "This appears to have been an artist's studio. The walls and floors are splattered with paints of 69 different colors. Strangely enough, nothing of value is hanging here. At the south end of the room is an open door (also covered with paint). A dark and narrow chimney leads up from a fireplace; although you might be able to get up it, it seems unlikely you could get back down."
    );
    studio->setFlag(ObjectFlag::RLANDBIT);
    studio->setFlag(ObjectFlag::ONBIT);
    studio->setFlag(ObjectFlag::SACREDBIT);
    studio->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This appears to have been an artist's studio. The walls and floors are splattered with paints of 69 different colors. Strangely enough, nothing of value is hanging here. At the south end of the room is an open door (also covered with paint). A dark and narrow chimney leads up from a fireplace; although you might be able to get up it, it seems unlikely you could get back down.");
        }
    });
    
    // Set up exits for Studio
    studio->setExit(Direction::SOUTH, RoomExit(RoomIds::GALLERY));
    studio->setExit(Direction::UP, RoomExit("The chimney is too narrow and sooty."));
    
    g.registerObject(RoomIds::STUDIO, std::move(studio));
    
    // Update Kitchen to connect to Gallery
    auto* kitchenPtr = dynamic_cast<ZRoom*>(g.getObject(RoomIds::KITCHEN));
    if (kitchenPtr) {
        kitchenPtr->setExit(Direction::NORTH, RoomExit(RoomIds::GALLERY));
    }
    
    // ===== UNDERGROUND PASSAGES =====
    
    // Create Troll Room
    auto trollRoom = std::make_unique<ZRoom>(
        RoomIds::TROLL_ROOM,
        "Troll Room",
        "This is a small room with passages to the east and south and a forbidding hole leading west. Bloodstains and deep scratches (perhaps made by an axe) mar the walls."
    );
    // Troll room is dark - no ONBIT flag
    trollRoom->setFlag(ObjectFlag::RLANDBIT);
    trollRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a small room with passages to the east and south and a forbidding hole leading west. Bloodstains and deep scratches (perhaps made by an axe) mar the walls.");
        }
    });
    
    // Set up exits for Troll Room
    trollRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::CELLAR));
    trollRoom->setExit(Direction::EAST, RoomExit(RoomIds::EW_PASSAGE));  // Will need troll flag check
    trollRoom->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_1));  // Will need troll flag check
    
    g.registerObject(RoomIds::TROLL_ROOM, std::move(trollRoom));
    
    // Create East of Chasm room
    auto eastOfChasm = std::make_unique<ZRoom>(
        RoomIds::EAST_OF_CHASM,
        "East of Chasm",
        "You are on the east edge of a chasm, the bottom of which cannot be seen. A narrow passage goes north, and the path you are on continues to the east."
    );
    // Dark room - no ONBIT flag
    eastOfChasm->setFlag(ObjectFlag::RLANDBIT);
    eastOfChasm->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are on the east edge of a chasm, the bottom of which cannot be seen. A narrow passage goes north, and the path you are on continues to the east.");
        }
    });
    
    // Set up exits for East of Chasm
    eastOfChasm->setExit(Direction::NORTH, RoomExit(RoomIds::CELLAR));
    eastOfChasm->setExit(Direction::EAST, RoomExit(RoomIds::GALLERY));
    eastOfChasm->setExit(Direction::DOWN, RoomExit("The chasm probably leads straight to the infernal regions."));
    
    g.registerObject(RoomIds::EAST_OF_CHASM, std::move(eastOfChasm));
    
    // Update Cellar to connect to East of Chasm
    auto* cellarPtr = dynamic_cast<ZRoom*>(g.getObject(RoomIds::CELLAR));
    if (cellarPtr) {
        cellarPtr->setExit(Direction::SOUTH, RoomExit(RoomIds::EAST_OF_CHASM));
    }
    
    // Update Gallery to connect to East of Chasm
    auto* galleryPtr = dynamic_cast<ZRoom*>(g.getObject(RoomIds::GALLERY));
    if (galleryPtr) {
        galleryPtr->setExit(Direction::WEST, RoomExit(RoomIds::EAST_OF_CHASM));
    }
    
    // Create East-West Passage
    auto ewPassage = std::make_unique<ZRoom>(
        RoomIds::EW_PASSAGE,
        "East-West Passage",
        "This is a narrow east-west passageway. There is a narrow stairway leading down at the north end of the room."
    );
    // Dark room - no ONBIT flag
    ewPassage->setFlag(ObjectFlag::RLANDBIT);
    ewPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a narrow east-west passageway. There is a narrow stairway leading down at the north end of the room.");
        }
    });
    
    // Set up exits for East-West Passage
    ewPassage->setExit(Direction::EAST, RoomExit(RoomIds::ROUND_ROOM));
    ewPassage->setExit(Direction::WEST, RoomExit(RoomIds::TROLL_ROOM));
    ewPassage->setExit(Direction::DOWN, RoomExit(RoomIds::CHASM_ROOM));
    ewPassage->setExit(Direction::NORTH, RoomExit(RoomIds::CHASM_ROOM));
    
    g.registerObject(RoomIds::EW_PASSAGE, std::move(ewPassage));
    
    // Create North-South Passage
    auto nsPassage = std::make_unique<ZRoom>(
        RoomIds::NS_PASSAGE,
        "North-South Passage",
        "This is a high north-south passage, which forks to the northeast."
    );
    // Dark room - no ONBIT flag
    nsPassage->setFlag(ObjectFlag::RLANDBIT);
    nsPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a high north-south passage, which forks to the northeast.");
        }
    });
    
    // Set up exits for North-South Passage
    nsPassage->setExit(Direction::NORTH, RoomExit(RoomIds::CHASM_ROOM));
    nsPassage->setExit(Direction::NE, RoomExit(RoomIds::DEEP_CANYON));
    nsPassage->setExit(Direction::SOUTH, RoomExit(RoomIds::ROUND_ROOM));
    
    g.registerObject(RoomIds::NS_PASSAGE, std::move(nsPassage));
    
    // Create Chasm Room
    auto chasmRoom = std::make_unique<ZRoom>(
        RoomIds::CHASM_ROOM,
        "Chasm",
        "A chasm runs southwest to northeast and the path follows it. You are on the south side of the chasm, where a crack opens into a passage."
    );
    // Dark room - no ONBIT flag
    chasmRoom->setFlag(ObjectFlag::RLANDBIT);
    chasmRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("A chasm runs southwest to northeast and the path follows it. You are on the south side of the chasm, where a crack opens into a passage.");
        }
    });
    
    // Set up exits for Chasm Room
    chasmRoom->setExit(Direction::NE, RoomExit(RoomIds::RESERVOIR_SOUTH));
    chasmRoom->setExit(Direction::SW, RoomExit(RoomIds::EW_PASSAGE));
    chasmRoom->setExit(Direction::UP, RoomExit(RoomIds::EW_PASSAGE));
    chasmRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::NS_PASSAGE));
    chasmRoom->setExit(Direction::DOWN, RoomExit("Are you out of your mind?"));
    
    g.registerObject(RoomIds::CHASM_ROOM, std::move(chasmRoom));
    
    // Create Round Room
    auto roundRoom = std::make_unique<ZRoom>(
        RoomIds::ROUND_ROOM,
        "Round Room",
        "This is a circular stone room with passages in all directions. Several of them have unfortunately been blocked by cave-ins."
    );
    // Dark room - no ONBIT flag
    roundRoom->setFlag(ObjectFlag::RLANDBIT);
    roundRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a circular stone room with passages in all directions. Several of them have unfortunately been blocked by cave-ins.");
        }
    });
    
    // Set up exits for Round Room - it connects to many rooms
    roundRoom->setExit(Direction::EAST, RoomExit(RoomIds::LOUD_ROOM));
    roundRoom->setExit(Direction::WEST, RoomExit(RoomIds::EW_PASSAGE));
    roundRoom->setExit(Direction::NORTH, RoomExit(RoomIds::NS_PASSAGE));
    roundRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::NARROW_PASSAGE));
    roundRoom->setExit(Direction::SE, RoomExit(RoomIds::ENGRAVINGS_CAVE));
    
    g.registerObject(RoomIds::ROUND_ROOM, std::move(roundRoom));
    
    // Create Loud Room
    auto loudRoom = std::make_unique<ZRoom>(
        RoomIds::LOUD_ROOM,
        "Loud Room",
        "This is a large room with a ceiling which cannot be detected from the ground. There is a narrow passage from east to west and a stone stairway leading upward. The room is extremely noisy. In fact, it is difficult to hear yourself think."
    );
    // Dark room - no ONBIT flag
    loudRoom->setFlag(ObjectFlag::RLANDBIT);
    loudRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room with a ceiling which cannot be detected from the ground. There is a narrow passage from east to west and a stone stairway leading upward. The room is extremely noisy. In fact, it is difficult to hear yourself think.");
        }
        // TODO: Add echo action handler for sounds
    });
    
    // Set up exits for Loud Room
    loudRoom->setExit(Direction::EAST, RoomExit(RoomIds::DAMP_CAVE));
    loudRoom->setExit(Direction::WEST, RoomExit(RoomIds::ROUND_ROOM));
    loudRoom->setExit(Direction::UP, RoomExit(RoomIds::DEEP_CANYON));
    
    g.registerObject(RoomIds::LOUD_ROOM, std::move(loudRoom));
    
    // Create Deep Canyon
    auto deepCanyon = std::make_unique<ZRoom>(
        RoomIds::DEEP_CANYON,
        "Deep Canyon",
        "You are on the south edge of a deep canyon. Passages lead off to the east, northwest and southwest. You can hear the sound of flowing water from below."
    );
    // Dark room - no ONBIT flag
    deepCanyon->setFlag(ObjectFlag::RLANDBIT);
    deepCanyon->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are on the south edge of a deep canyon. Passages lead off to the east, northwest and southwest. You can hear the sound of flowing water from below.");
        }
    });
    
    // Set up exits for Deep Canyon
    deepCanyon->setExit(Direction::NW, RoomExit(RoomIds::RESERVOIR_SOUTH));
    deepCanyon->setExit(Direction::EAST, RoomExit(RoomIds::DAM_ROOM));
    deepCanyon->setExit(Direction::SW, RoomExit(RoomIds::NS_PASSAGE));
    deepCanyon->setExit(Direction::DOWN, RoomExit(RoomIds::LOUD_ROOM));
    
    g.registerObject(RoomIds::DEEP_CANYON, std::move(deepCanyon));
    
    // Create Damp Cave
    auto dampCave = std::make_unique<ZRoom>(
        RoomIds::DAMP_CAVE,
        "Damp Cave",
        "This cave has exits to the west and east, and narrows to a crack toward the south. The earth is particularly damp here."
    );
    // Dark room - no ONBIT flag
    dampCave->setFlag(ObjectFlag::RLANDBIT);
    dampCave->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This cave has exits to the west and east, and narrows to a crack toward the south. The earth is particularly damp here.");
        }
    });
    
    // Set up exits for Damp Cave
    dampCave->setExit(Direction::WEST, RoomExit(RoomIds::LOUD_ROOM));
    dampCave->setExit(Direction::EAST, RoomExit(RoomIds::WHITE_CLIFFS_NORTH));
    dampCave->setExit(Direction::SOUTH, RoomExit("It is too narrow for most insects."));
    
    g.registerObject(RoomIds::DAMP_CAVE, std::move(dampCave));
    
    // Create Cold Passage
    auto coldPassage = std::make_unique<ZRoom>(
        RoomIds::COLD_PASSAGE,
        "Cold Passage",
        "This is a cold and damp corridor where a long east-west passageway turns into a southward path."
    );
    // Dark room - no ONBIT flag
    coldPassage->setFlag(ObjectFlag::RLANDBIT);
    coldPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a cold and damp corridor where a long east-west passageway turns into a southward path.");
        }
    });
    
    // Set up exits for Cold Passage
    coldPassage->setExit(Direction::SOUTH, RoomExit(RoomIds::MIRROR_ROOM_1));
    coldPassage->setExit(Direction::WEST, RoomExit(RoomIds::SLIDE_ROOM));
    
    g.registerObject(RoomIds::COLD_PASSAGE, std::move(coldPassage));
    
    // Create Narrow Passage
    auto narrowPassage = std::make_unique<ZRoom>(
        RoomIds::NARROW_PASSAGE,
        "Narrow Passage",
        "This is a long and narrow corridor where a long north-south passageway briefly narrows even further."
    );
    // Dark room - no ONBIT flag
    narrowPassage->setFlag(ObjectFlag::RLANDBIT);
    narrowPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a long and narrow corridor where a long north-south passageway briefly narrows even further.");
        }
    });
    
    // Set up exits for Narrow Passage
    narrowPassage->setExit(Direction::NORTH, RoomExit(RoomIds::ROUND_ROOM));
    narrowPassage->setExit(Direction::SOUTH, RoomExit(RoomIds::MIRROR_ROOM_2));
    
    g.registerObject(RoomIds::NARROW_PASSAGE, std::move(narrowPassage));
    
    // Create Slide Room
    auto slideRoom = std::make_unique<ZRoom>(
        RoomIds::SLIDE_ROOM,
        "Slide Room",
        "This is a small chamber, which appears to have been part of a coal mine. On the south wall of the chamber the letters \"Granite Wall\" are etched in the rock. To the east is a long passage, and there is a steep metal slide twisting downward. To the north is a small opening."
    );
    // Dark room - no ONBIT flag
    slideRoom->setFlag(ObjectFlag::RLANDBIT);
    slideRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a small chamber, which appears to have been part of a coal mine. On the south wall of the chamber the letters \"Granite Wall\" are etched in the rock. To the east is a long passage, and there is a steep metal slide twisting downward. To the north is a small opening.");
        }
    });
    
    // Set up exits for Slide Room
    slideRoom->setExit(Direction::EAST, RoomExit(RoomIds::COLD_PASSAGE));
    slideRoom->setExit(Direction::NORTH, RoomExit(RoomIds::MINE_ENTRANCE));
    slideRoom->setExit(Direction::DOWN, RoomExit(RoomIds::CELLAR));
    
    g.registerObject(RoomIds::SLIDE_ROOM, std::move(slideRoom));
    
    // Create Mine Entrance
    auto mineEntrance = std::make_unique<ZRoom>(
        RoomIds::MINE_ENTRANCE,
        "Mine Entrance",
        "You are standing at the entrance of what might have been a coal mine. The shaft enters the west wall, and there is another exit on the south end of the room."
    );
    // Dark room - no ONBIT flag
    mineEntrance->setFlag(ObjectFlag::RLANDBIT);
    mineEntrance->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are standing at the entrance of what might have been a coal mine. The shaft enters the west wall, and there is another exit on the south end of the room.");
        }
    });
    
    // Set up exits for Mine Entrance
    mineEntrance->setExit(Direction::SOUTH, RoomExit(RoomIds::SLIDE_ROOM));
    mineEntrance->setExit(Direction::IN, RoomExit(RoomIds::SQUEEKY_ROOM));
    mineEntrance->setExit(Direction::WEST, RoomExit(RoomIds::SQUEEKY_ROOM));
    
    g.registerObject(RoomIds::MINE_ENTRANCE, std::move(mineEntrance));
    
    // Create Squeaky Room
    auto squeekyRoom = std::make_unique<ZRoom>(
        RoomIds::SQUEEKY_ROOM,
        "Squeaky Room",
        "You are in a small room. Strange squeaky sounds may be heard coming from the passage at the north end. You may also escape to the east."
    );
    // Dark room - no ONBIT flag
    squeekyRoom->setFlag(ObjectFlag::RLANDBIT);
    squeekyRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a small room. Strange squeaky sounds may be heard coming from the passage at the north end. You may also escape to the east.");
        }
    });
    
    // Set up exits for Squeaky Room
    squeekyRoom->setExit(Direction::NORTH, RoomExit(RoomIds::BAT_ROOM));
    squeekyRoom->setExit(Direction::EAST, RoomExit(RoomIds::MINE_ENTRANCE));
    
    g.registerObject(RoomIds::SQUEEKY_ROOM, std::move(squeekyRoom));
    
    // Create Bat Room
    auto batRoom = std::make_unique<ZRoom>(
        RoomIds::BAT_ROOM,
        "Bat Room",
        "You are in a small room which has doors only to the east and south."
    );
    // Dark room - no ONBIT flag
    batRoom->setFlag(ObjectFlag::RLANDBIT);
    batRoom->setFlag(ObjectFlag::SACREDBIT);
    batRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a small room which has doors only to the east and south.");
        }
    });
    
    // Set up exits for Bat Room
    batRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::SQUEEKY_ROOM));
    batRoom->setExit(Direction::EAST, RoomExit(RoomIds::SHAFT_ROOM));
    
    g.registerObject(RoomIds::BAT_ROOM, std::move(batRoom));
    
    // Create Shaft Room
    auto shaftRoom = std::make_unique<ZRoom>(
        RoomIds::SHAFT_ROOM,
        "Shaft Room",
        "This is a large room, in the middle of which is a small shaft descending through the floor into darkness below. To the west and the north are exits from this room. Constructed over the top of the shaft is a metal framework to which a heavy iron chain is attached."
    );
    // Dark room - no ONBIT flag
    shaftRoom->setFlag(ObjectFlag::RLANDBIT);
    shaftRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room, in the middle of which is a small shaft descending through the floor into darkness below. To the west and the north are exits from this room. Constructed over the top of the shaft is a metal framework to which a heavy iron chain is attached.");
        }
    });
    
    // Set up exits for Shaft Room
    shaftRoom->setExit(Direction::DOWN, RoomExit("You wouldn't fit and would die if you could."));
    shaftRoom->setExit(Direction::WEST, RoomExit(RoomIds::BAT_ROOM));
    shaftRoom->setExit(Direction::NORTH, RoomExit(RoomIds::SMELLY_ROOM));
    
    g.registerObject(RoomIds::SHAFT_ROOM, std::move(shaftRoom));
    
    // Create Smelly Room
    auto smellyRoom = std::make_unique<ZRoom>(
        RoomIds::SMELLY_ROOM,
        "Smelly Room",
        "This is a small nondescript room. However, from the direction of a small descending staircase a foul odor can be detected. To the south is a narrow tunnel."
    );
    // Dark room - no ONBIT flag
    smellyRoom->setFlag(ObjectFlag::RLANDBIT);
    smellyRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a small nondescript room. However, from the direction of a small descending staircase a foul odor can be detected. To the south is a narrow tunnel.");
        }
    });
    
    // Set up exits for Smelly Room
    smellyRoom->setExit(Direction::DOWN, RoomExit(RoomIds::GAS_ROOM));
    smellyRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::SHAFT_ROOM));
    
    g.registerObject(RoomIds::SMELLY_ROOM, std::move(smellyRoom));
    
    // Create Gas Room
    auto gasRoom = std::make_unique<ZRoom>(
        RoomIds::GAS_ROOM,
        "Gas Room",
        "This is a small room which smells strongly of coal gas. There is a short climb up some stairs and a narrow tunnel leading east."
    );
    // Dark room - no ONBIT flag
    gasRoom->setFlag(ObjectFlag::RLANDBIT);
    gasRoom->setFlag(ObjectFlag::SACREDBIT);
    gasRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a small room which smells strongly of coal gas. There is a short climb up some stairs and a narrow tunnel leading east.");
        }
    });
    
    // Set up exits for Gas Room
    gasRoom->setExit(Direction::UP, RoomExit(RoomIds::SMELLY_ROOM));
    gasRoom->setExit(Direction::EAST, RoomExit(RoomIds::MINE_1));
    
    g.registerObject(RoomIds::GAS_ROOM, std::move(gasRoom));
    
    // Create Ladder Top
    auto ladderTop = std::make_unique<ZRoom>(
        RoomIds::LADDER_TOP,
        "Ladder Top",
        "This is a very small room. In the corner is a rickety wooden ladder, leading downward. It might be safe to descend. There is also a staircase leading upward."
    );
    // Dark room - no ONBIT flag
    ladderTop->setFlag(ObjectFlag::RLANDBIT);
    ladderTop->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a very small room. In the corner is a rickety wooden ladder, leading downward. It might be safe to descend. There is also a staircase leading upward.");
        }
    });
    
    // Set up exits for Ladder Top
    ladderTop->setExit(Direction::DOWN, RoomExit(RoomIds::LADDER_BOTTOM));
    ladderTop->setExit(Direction::UP, RoomExit(RoomIds::MINE_4));
    
    g.registerObject(RoomIds::LADDER_TOP, std::move(ladderTop));
    
    // Create Ladder Bottom
    auto ladderBottom = std::make_unique<ZRoom>(
        RoomIds::LADDER_BOTTOM,
        "Ladder Bottom",
        "This is a rather wide room. On one side is the bottom of a narrow wooden ladder. To the west and the south are passages leaving the room."
    );
    // Dark room - no ONBIT flag
    ladderBottom->setFlag(ObjectFlag::RLANDBIT);
    ladderBottom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a rather wide room. On one side is the bottom of a narrow wooden ladder. To the west and the south are passages leaving the room.");
        }
    });
    
    // Set up exits for Ladder Bottom
    ladderBottom->setExit(Direction::SOUTH, RoomExit(RoomIds::DEAD_END_5));
    ladderBottom->setExit(Direction::WEST, RoomExit(RoomIds::TIMBER_ROOM));
    ladderBottom->setExit(Direction::UP, RoomExit(RoomIds::LADDER_TOP));
    
    g.registerObject(RoomIds::LADDER_BOTTOM, std::move(ladderBottom));
    
    // Create Timber Room
    auto timberRoom = std::make_unique<ZRoom>(
        RoomIds::TIMBER_ROOM,
        "Timber Room",
        "This is a long and narrow passage, which is cluttered with broken timbers. A wide passage comes from the east and turns at the west end of the room into a very narrow passageway. From the west comes a strong draft."
    );
    // Dark room - no ONBIT flag
    timberRoom->setFlag(ObjectFlag::RLANDBIT);
    timberRoom->setFlag(ObjectFlag::SACREDBIT);
    timberRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a long and narrow passage, which is cluttered with broken timbers. A wide passage comes from the east and turns at the west end of the room into a very narrow passageway. From the west comes a strong draft.");
        }
    });
    
    // Set up exits for Timber Room
    timberRoom->setExit(Direction::EAST, RoomExit(RoomIds::LADDER_BOTTOM));
    timberRoom->setExit(Direction::WEST, RoomExit("You cannot fit through this passage with that load."));  // Requires empty hands
    
    g.registerObject(RoomIds::TIMBER_ROOM, std::move(timberRoom));
    
    // Create Lower Shaft
    auto lowerShaft = std::make_unique<ZRoom>(
        RoomIds::LOWER_SHAFT,
        "Drafty Room",
        "This is a small drafty room in which is the bottom of a long shaft. To the south is a passageway and to the east a very narrow passage. In the shaft can be seen a heavy iron chain."
    );
    // Dark room - no ONBIT flag
    lowerShaft->setFlag(ObjectFlag::RLANDBIT);
    lowerShaft->setFlag(ObjectFlag::SACREDBIT);
    lowerShaft->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a small drafty room in which is the bottom of a long shaft. To the south is a passageway and to the east a very narrow passage. In the shaft can be seen a heavy iron chain.");
        }
    });
    
    // Set up exits for Lower Shaft
    lowerShaft->setExit(Direction::SOUTH, RoomExit(RoomIds::MACHINE_ROOM));
    lowerShaft->setExit(Direction::OUT, RoomExit("You cannot fit through this passage with that load."));  // Requires empty hands
    lowerShaft->setExit(Direction::EAST, RoomExit("You cannot fit through this passage with that load."));  // Requires empty hands
    
    g.registerObject(RoomIds::LOWER_SHAFT, std::move(lowerShaft));
    
    // Create Machine Room
    auto machineRoom = std::make_unique<ZRoom>(
        RoomIds::MACHINE_ROOM,
        "Machine Room",
        "This is a large room full of assorted heavy machinery, whirring noisily. The room smells of burned resistors. Along one wall of the room are three buttons which are, respectively, round, triangular, and square. Naturally, above these buttons are instructions written in EBCDIC. A large sign above the buttons says \"DANGER: DEADLY RADIATION BEYOND THIS POINT!\" There are exits to the north and east."
    );
    // Dark room - no ONBIT flag
    machineRoom->setFlag(ObjectFlag::RLANDBIT);
    machineRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room full of assorted heavy machinery, whirring noisily. The room smells of burned resistors. Along one wall of the room are three buttons which are, respectively, round, triangular, and square. Naturally, above these buttons are instructions written in EBCDIC. A large sign above the buttons says \"DANGER: DEADLY RADIATION BEYOND THIS POINT!\" There are exits to the north and east.");
        }
    });
    
    // Set up exits for Machine Room
    machineRoom->setExit(Direction::NORTH, RoomExit(RoomIds::LOWER_SHAFT));
    
    g.registerObject(RoomIds::MACHINE_ROOM, std::move(machineRoom));
    
    // Create Dead End 5 (coal mine dead end)
    auto deadEnd5 = std::make_unique<ZRoom>(
        RoomIds::DEAD_END_5,
        "Dead End",
        "You have come to a dead end in the mine."
    );
    // Dark room - no ONBIT flag
    deadEnd5->setFlag(ObjectFlag::RLANDBIT);
    deadEnd5->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have come to a dead end in the mine.");
        }
    });
    
    // Set up exits for Dead End 5
    deadEnd5->setExit(Direction::NORTH, RoomExit(RoomIds::LADDER_BOTTOM));
    
    g.registerObject(RoomIds::DEAD_END_5, std::move(deadEnd5));
    
    // Create Coal Mine rooms (Mine 1-4)
    auto mine1 = std::make_unique<ZRoom>(
        RoomIds::MINE_1,
        "Coal Mine",
        "This is a nondescript part of a coal mine."
    );
    mine1->setFlag(ObjectFlag::RLANDBIT);
    mine1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a nondescript part of a coal mine.");
        }
    });
    mine1->setExit(Direction::NORTH, RoomExit(RoomIds::GAS_ROOM));
    mine1->setExit(Direction::EAST, RoomExit(RoomIds::MINE_1));  // Loops to itself
    mine1->setExit(Direction::NE, RoomExit(RoomIds::MINE_2));
    
    g.registerObject(RoomIds::MINE_1, std::move(mine1));
    
    auto mine2 = std::make_unique<ZRoom>(
        RoomIds::MINE_2,
        "Coal Mine",
        "This is a nondescript part of a coal mine."
    );
    mine2->setFlag(ObjectFlag::RLANDBIT);
    mine2->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a nondescript part of a coal mine.");
        }
    });
    mine2->setExit(Direction::NORTH, RoomExit(RoomIds::MINE_2));  // Loops to itself
    mine2->setExit(Direction::SOUTH, RoomExit(RoomIds::MINE_1));
    mine2->setExit(Direction::SE, RoomExit(RoomIds::MINE_3));
    
    g.registerObject(RoomIds::MINE_2, std::move(mine2));
    
    auto mine3 = std::make_unique<ZRoom>(
        RoomIds::MINE_3,
        "Coal Mine",
        "This is a nondescript part of a coal mine."
    );
    mine3->setFlag(ObjectFlag::RLANDBIT);
    mine3->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a nondescript part of a coal mine.");
        }
    });
    mine3->setExit(Direction::SOUTH, RoomExit(RoomIds::MINE_3));  // Loops to itself
    mine3->setExit(Direction::SW, RoomExit(RoomIds::MINE_4));
    mine3->setExit(Direction::EAST, RoomExit(RoomIds::MINE_2));
    
    g.registerObject(RoomIds::MINE_3, std::move(mine3));
    
    auto mine4 = std::make_unique<ZRoom>(
        RoomIds::MINE_4,
        "Coal Mine",
        "This is a nondescript part of a coal mine."
    );
    mine4->setFlag(ObjectFlag::RLANDBIT);
    mine4->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a nondescript part of a coal mine.");
        }
    });
    mine4->setExit(Direction::NORTH, RoomExit(RoomIds::MINE_3));
    mine4->setExit(Direction::WEST, RoomExit(RoomIds::MINE_4));  // Loops to itself
    mine4->setExit(Direction::DOWN, RoomExit(RoomIds::LADDER_TOP));
    
    g.registerObject(RoomIds::MINE_4, std::move(mine4));
    
    // ===== RESERVOIR AND DAM AREA =====
    
    // Create Reservoir South
    auto reservoirSouth = std::make_unique<ZRoom>(
        RoomIds::RESERVOIR_SOUTH,
        "Reservoir South",
        "You are in a long room on the south shore of a large lake, far too deep and wide for crossing. There is a path along the stream to the east or west, and a steep pathway climbing southwest along the edge of a chasm."
    );
    // Dark room - no ONBIT flag
    reservoirSouth->setFlag(ObjectFlag::RLANDBIT);
    reservoirSouth->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a long room on the south shore of a large lake, far too deep and wide for crossing. There is a path along the stream to the east or west, and a steep pathway climbing southwest along the edge of a chasm.");
        }
    });
    
    // Set up exits for Reservoir South
    reservoirSouth->setExit(Direction::SE, RoomExit(RoomIds::DEEP_CANYON));
    reservoirSouth->setExit(Direction::SW, RoomExit(RoomIds::CHASM_ROOM));
    reservoirSouth->setExit(Direction::EAST, RoomExit(RoomIds::DAM_ROOM));
    reservoirSouth->setExit(Direction::WEST, RoomExit(RoomIds::STREAM_VIEW));
    reservoirSouth->setExit(Direction::NORTH, RoomExit("You would drown."));  // Requires LOW-TIDE flag
    
    g.registerObject(RoomIds::RESERVOIR_SOUTH, std::move(reservoirSouth));
    
    // Create Reservoir
    auto reservoir = std::make_unique<ZRoom>(
        RoomIds::RESERVOIR,
        "Reservoir",
        "You are on the lake. The water is cold and the current is strong. It is difficult to stay afloat."
    );
    // Water room - NONLANDBIT
    reservoir->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are on the lake. The water is cold and the current is strong. It is difficult to stay afloat.");
        }
    });
    
    // Set up exits for Reservoir
    reservoir->setExit(Direction::NORTH, RoomExit(RoomIds::RESERVOIR_NORTH));
    reservoir->setExit(Direction::SOUTH, RoomExit(RoomIds::RESERVOIR_SOUTH));
    reservoir->setExit(Direction::UP, RoomExit(RoomIds::IN_STREAM));
    reservoir->setExit(Direction::WEST, RoomExit(RoomIds::IN_STREAM));
    reservoir->setExit(Direction::DOWN, RoomExit("The dam blocks your way."));
    
    g.registerObject(RoomIds::RESERVOIR, std::move(reservoir));
    
    // Create Reservoir North
    auto reservoirNorth = std::make_unique<ZRoom>(
        RoomIds::RESERVOIR_NORTH,
        "Reservoir North",
        "You are in a long room on the north shore of a large lake, far too deep and wide for crossing."
    );
    // Dark room - no ONBIT flag
    reservoirNorth->setFlag(ObjectFlag::RLANDBIT);
    reservoirNorth->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a long room on the north shore of a large lake, far too deep and wide for crossing.");
        }
    });
    
    // Set up exits for Reservoir North
    reservoirNorth->setExit(Direction::NORTH, RoomExit(RoomIds::ATLANTIS_ROOM));
    reservoirNorth->setExit(Direction::SOUTH, RoomExit("You would drown."));  // Requires LOW-TIDE flag
    
    g.registerObject(RoomIds::RESERVOIR_NORTH, std::move(reservoirNorth));
    
    // Create Stream View
    auto streamView = std::make_unique<ZRoom>(
        RoomIds::STREAM_VIEW,
        "Stream View",
        "You are standing on a path beside a gently flowing stream. The path follows the stream, which flows from west to east."
    );
    // Dark room - no ONBIT flag
    streamView->setFlag(ObjectFlag::RLANDBIT);
    streamView->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are standing on a path beside a gently flowing stream. The path follows the stream, which flows from west to east.");
        }
    });
    
    // Set up exits for Stream View
    streamView->setExit(Direction::EAST, RoomExit(RoomIds::RESERVOIR_SOUTH));
    streamView->setExit(Direction::WEST, RoomExit("The stream emerges from a spot too small for you to enter."));
    
    g.registerObject(RoomIds::STREAM_VIEW, std::move(streamView));
    
    // Create In Stream
    auto inStream = std::make_unique<ZRoom>(
        RoomIds::IN_STREAM,
        "Stream",
        "You are on the gently flowing stream. The upstream route is too narrow to navigate, and the downstream route is invisible due to twisting walls. There is a narrow beach to land on."
    );
    // Water room - NONLANDBIT
    inStream->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are on the gently flowing stream. The upstream route is too narrow to navigate, and the downstream route is invisible due to twisting walls. There is a narrow beach to land on.");
        }
    });
    
    // Set up exits for In Stream
    inStream->setExit(Direction::UP, RoomExit("The channel is too narrow."));
    inStream->setExit(Direction::WEST, RoomExit("The channel is too narrow."));
    inStream->setExit(Direction::DOWN, RoomExit(RoomIds::RESERVOIR));
    inStream->setExit(Direction::EAST, RoomExit(RoomIds::RESERVOIR));
    
    g.registerObject(RoomIds::IN_STREAM, std::move(inStream));
    
    // Create Dam Room
    auto damRoom = std::make_unique<ZRoom>(
        RoomIds::DAM_ROOM,
        "Dam",
        "You are standing on the top of the Flood Control Dam #3, which was quite a tourist attraction in times far distant. There are paths to the north, south, and west, and a scramble down."
    );
    damRoom->setFlag(ObjectFlag::RLANDBIT);
    damRoom->setFlag(ObjectFlag::ONBIT);
    damRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are standing on the top of the Flood Control Dam #3, which was quite a tourist attraction in times far distant. There are paths to the north, south, and west, and a scramble down.");
        }
    });
    
    // Set up exits for Dam Room
    damRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::DEEP_CANYON));
    damRoom->setExit(Direction::DOWN, RoomExit(RoomIds::DAM_BASE));
    damRoom->setExit(Direction::EAST, RoomExit(RoomIds::DAM_BASE));
    damRoom->setExit(Direction::NORTH, RoomExit(RoomIds::DAM_LOBBY));
    damRoom->setExit(Direction::WEST, RoomExit(RoomIds::RESERVOIR_SOUTH));
    
    g.registerObject(RoomIds::DAM_ROOM, std::move(damRoom));
    
    // Create Dam Lobby
    auto damLobby = std::make_unique<ZRoom>(
        RoomIds::DAM_LOBBY,
        "Dam Lobby",
        "This room appears to have been the waiting room for groups touring the dam. There are open doorways here to the north and east marked \"Private\", and there is a path leading south over the top of the dam."
    );
    damLobby->setFlag(ObjectFlag::RLANDBIT);
    damLobby->setFlag(ObjectFlag::ONBIT);
    damLobby->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This room appears to have been the waiting room for groups touring the dam. There are open doorways here to the north and east marked \"Private\", and there is a path leading south over the top of the dam.");
        }
    });
    
    // Set up exits for Dam Lobby
    damLobby->setExit(Direction::SOUTH, RoomExit(RoomIds::DAM_ROOM));
    damLobby->setExit(Direction::NORTH, RoomExit(RoomIds::MAINTENANCE_ROOM));
    damLobby->setExit(Direction::EAST, RoomExit(RoomIds::MAINTENANCE_ROOM));
    
    g.registerObject(RoomIds::DAM_LOBBY, std::move(damLobby));
    
    // Create Maintenance Room
    auto maintenanceRoom = std::make_unique<ZRoom>(
        RoomIds::MAINTENANCE_ROOM,
        "Maintenance Room",
        "This is what appears to have been the maintenance room for Flood Control Dam #3. Apparently, this room has been ransacked recently, for most of the valuable equipment is gone. On the wall in front of you is a group of buttons colored blue, yellow, brown, and red. There are doorways to the west and south."
    );
    // Dark room - no ONBIT flag
    maintenanceRoom->setFlag(ObjectFlag::RLANDBIT);
    maintenanceRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is what appears to have been the maintenance room for Flood Control Dam #3. Apparently, this room has been ransacked recently, for most of the valuable equipment is gone. On the wall in front of you is a group of buttons colored blue, yellow, brown, and red. There are doorways to the west and south.");
        }
    });
    
    // Set up exits for Maintenance Room
    maintenanceRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::DAM_LOBBY));
    maintenanceRoom->setExit(Direction::WEST, RoomExit(RoomIds::DAM_LOBBY));
    
    g.registerObject(RoomIds::MAINTENANCE_ROOM, std::move(maintenanceRoom));
    
    // Create Dam Base
    auto damBase = std::make_unique<ZRoom>(
        RoomIds::DAM_BASE,
        "Dam Base",
        "You are at the base of Flood Control Dam #3, which looms above you and to the north. The river Frigid is flowing by here. Along the river are the White Cliffs which seem to form giant walls stretching from north to south along the shores of the river as it winds its way downstream."
    );
    damBase->setFlag(ObjectFlag::RLANDBIT);
    damBase->setFlag(ObjectFlag::ONBIT);
    damBase->setFlag(ObjectFlag::SACREDBIT);
    damBase->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are at the base of Flood Control Dam #3, which looms above you and to the north. The river Frigid is flowing by here. Along the river are the White Cliffs which seem to form giant walls stretching from north to south along the shores of the river as it winds its way downstream.");
        }
    });
    
    // Set up exits for Dam Base
    damBase->setExit(Direction::NORTH, RoomExit(RoomIds::DAM_ROOM));
    damBase->setExit(Direction::UP, RoomExit(RoomIds::DAM_ROOM));
    
    g.registerObject(RoomIds::DAM_BASE, std::move(damBase));
    
    // ===== SPECIAL UNDERGROUND ROOMS =====
    
    // Create Engravings Cave
    auto engravingsCave = std::make_unique<ZRoom>(
        RoomIds::ENGRAVINGS_CAVE,
        "Engravings Cave",
        "You have entered a low cave with passages leading northwest and east."
    );
    // Dark room - no ONBIT flag
    engravingsCave->setFlag(ObjectFlag::RLANDBIT);
    engravingsCave->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have entered a low cave with passages leading northwest and east.");
        }
    });
    
    // Set up exits for Engravings Cave
    engravingsCave->setExit(Direction::NW, RoomExit(RoomIds::ROUND_ROOM));
    engravingsCave->setExit(Direction::EAST, RoomExit(RoomIds::DOME_ROOM));
    
    g.registerObject(RoomIds::ENGRAVINGS_CAVE, std::move(engravingsCave));
    
    // ===== MAZE ROOMS =====
    // The maze is a confusing network of twisty passages, all alike
    // Rooms have similar descriptions to disorient the player
    
    // Create MAZE_1 (entrance from Troll Room)
    auto maze1 = std::make_unique<ZRoom>(
        RoomIds::MAZE_1,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze1->setFlag(ObjectFlag::RLANDBIT);
    maze1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_1, std::move(maze1));
    
    // Create MAZE_2
    auto maze2 = std::make_unique<ZRoom>(
        RoomIds::MAZE_2,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze2->setFlag(ObjectFlag::RLANDBIT);
    maze2->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_2, std::move(maze2));
    
    // Create MAZE_3
    auto maze3 = std::make_unique<ZRoom>(
        RoomIds::MAZE_3,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze3->setFlag(ObjectFlag::RLANDBIT);
    maze3->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_3, std::move(maze3));
    
    // Create MAZE_4
    auto maze4 = std::make_unique<ZRoom>(
        RoomIds::MAZE_4,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze4->setFlag(ObjectFlag::RLANDBIT);
    maze4->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_4, std::move(maze4));
    
    // Create MAZE_5
    auto maze5 = std::make_unique<ZRoom>(
        RoomIds::MAZE_5,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze5->setFlag(ObjectFlag::RLANDBIT);
    maze5->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_5, std::move(maze5));
    
    // Create MAZE_6
    auto maze6 = std::make_unique<ZRoom>(
        RoomIds::MAZE_6,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze6->setFlag(ObjectFlag::RLANDBIT);
    maze6->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_6, std::move(maze6));
    
    // Create MAZE_7
    auto maze7 = std::make_unique<ZRoom>(
        RoomIds::MAZE_7,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze7->setFlag(ObjectFlag::RLANDBIT);
    maze7->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_7, std::move(maze7));
    
    // Create MAZE_8
    auto maze8 = std::make_unique<ZRoom>(
        RoomIds::MAZE_8,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze8->setFlag(ObjectFlag::RLANDBIT);
    maze8->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_8, std::move(maze8));
    
    // Create MAZE_9
    auto maze9 = std::make_unique<ZRoom>(
        RoomIds::MAZE_9,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze9->setFlag(ObjectFlag::RLANDBIT);
    maze9->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_9, std::move(maze9));
    
    // Create MAZE_10
    auto maze10 = std::make_unique<ZRoom>(
        RoomIds::MAZE_10,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze10->setFlag(ObjectFlag::RLANDBIT);
    maze10->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_10, std::move(maze10));
    
    // Create MAZE_11
    auto maze11 = std::make_unique<ZRoom>(
        RoomIds::MAZE_11,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze11->setFlag(ObjectFlag::RLANDBIT);
    maze11->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_11, std::move(maze11));
    
    // Create MAZE_12
    auto maze12 = std::make_unique<ZRoom>(
        RoomIds::MAZE_12,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze12->setFlag(ObjectFlag::RLANDBIT);
    maze12->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_12, std::move(maze12));
    
    // Create MAZE_13
    auto maze13 = std::make_unique<ZRoom>(
        RoomIds::MAZE_13,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze13->setFlag(ObjectFlag::RLANDBIT);
    maze13->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_13, std::move(maze13));
    
    // Create MAZE_14
    auto maze14 = std::make_unique<ZRoom>(
        RoomIds::MAZE_14,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze14->setFlag(ObjectFlag::RLANDBIT);
    maze14->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_14, std::move(maze14));
    
    // Create MAZE_15
    auto maze15 = std::make_unique<ZRoom>(
        RoomIds::MAZE_15,
        "Maze",
        "This is part of a maze of twisty little passages, all alike."
    );
    maze15->setFlag(ObjectFlag::RLANDBIT);
    maze15->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is part of a maze of twisty little passages, all alike.");
        }
    });
    g.registerObject(RoomIds::MAZE_15, std::move(maze15));
    
    // Create Grating Room (exit from maze)
    auto gratingRoom = std::make_unique<ZRoom>(
        RoomIds::GRATING_ROOM,
        "Grating Room",
        "You are in a small room near the maze. There are twisty passages in the immediate vicinity."
    );
    gratingRoom->setFlag(ObjectFlag::RLANDBIT);
    gratingRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a small room near the maze. There are twisty passages in the immediate vicinity.");
        }
    });
    g.registerObject(RoomIds::GRATING_ROOM, std::move(gratingRoom));
    
    // Create Dead End rooms
    auto deadEnd1 = std::make_unique<ZRoom>(
        RoomIds::DEAD_END_1,
        "Dead End",
        "You have come to a dead end in the maze."
    );
    deadEnd1->setFlag(ObjectFlag::RLANDBIT);
    deadEnd1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have come to a dead end in the maze.");
        }
    });
    g.registerObject(RoomIds::DEAD_END_1, std::move(deadEnd1));
    
    auto deadEnd2 = std::make_unique<ZRoom>(
        RoomIds::DEAD_END_2,
        "Dead End",
        "You have come to a dead end in the maze."
    );
    deadEnd2->setFlag(ObjectFlag::RLANDBIT);
    deadEnd2->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have come to a dead end in the maze.");
        }
    });
    g.registerObject(RoomIds::DEAD_END_2, std::move(deadEnd2));
    
    auto deadEnd3 = std::make_unique<ZRoom>(
        RoomIds::DEAD_END_3,
        "Dead End",
        "You have come to a dead end in the maze."
    );
    deadEnd3->setFlag(ObjectFlag::RLANDBIT);
    deadEnd3->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have come to a dead end in the maze.");
        }
    });
    g.registerObject(RoomIds::DEAD_END_3, std::move(deadEnd3));
    
    auto deadEnd4 = std::make_unique<ZRoom>(
        RoomIds::DEAD_END_4,
        "Dead End",
        "You have come to a dead end in the maze."
    );
    deadEnd4->setFlag(ObjectFlag::RLANDBIT);
    deadEnd4->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have come to a dead end in the maze.");
        }
    });
    g.registerObject(RoomIds::DEAD_END_4, std::move(deadEnd4));
    
    // Set up forest navigation (confusing interconnections)
    // FOREST-1 connections
    auto* f1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::FOREST_1));
    if (f1) {
        f1->setExit(Direction::NORTH, RoomExit(RoomIds::GRATING_CLEARING));
        f1->setExit(Direction::EAST, RoomExit(RoomIds::FOREST_PATH));
        f1->setExit(Direction::SOUTH, RoomExit(RoomIds::FOREST_3));
    }
    
    // FOREST-2 connections
    auto* f2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::FOREST_2));
    if (f2) {
        f2->setExit(Direction::EAST, RoomExit(RoomIds::MOUNTAINS));
        f2->setExit(Direction::SOUTH, RoomExit(RoomIds::CLEARING));
        f2->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_PATH));
    }
    
    // FOREST-3 connections
    auto* f3 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::FOREST_3));
    if (f3) {
        f3->setExit(Direction::NORTH, RoomExit(RoomIds::CLEARING));
        f3->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_1));
        f3->setExit(Direction::NW, RoomExit(ROOM_SOUTH_OF_HOUSE));
    }
    
    // ===== MAZE NAVIGATION =====
    // Set up confusing maze connections based on original Zork I
    // The maze is intentionally disorienting with non-intuitive paths
    // Solution path exists but requires careful mapping
    
    // MAZE_1 - Entrance from Troll Room
    auto* m1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_1));
    if (m1) {
        m1->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_2));
        m1->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_4));
        m1->setExit(Direction::UP, RoomExit(RoomIds::MAZE_3));
        m1->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_1));  // Loops back
        m1->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_1));   // Loops back
    }
    
    // MAZE_2
    auto* m2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_2));
    if (m2) {
        m2->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_3));
        m2->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_5));
        m2->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_2));   // Loops back
        m2->setExit(Direction::DOWN, RoomExit(RoomIds::MAZE_6));
    }
    
    // MAZE_3
    auto* m3 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_3));
    if (m3) {
        m3->setExit(Direction::UP, RoomExit(RoomIds::MAZE_4));
        m3->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_5));
        m3->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_3));   // Loops back
        m3->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_3));   // Loops back
    }
    
    // MAZE_4
    auto* m4 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_4));
    if (m4) {
        m4->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_5));
        m4->setExit(Direction::SOUTH, RoomExit(RoomIds::DEAD_END_1));
        m4->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_6));
        m4->setExit(Direction::UP, RoomExit(RoomIds::MAZE_4));     // Loops back
    }
    
    // MAZE_5
    auto* m5 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_5));
    if (m5) {
        m5->setExit(Direction::SW, RoomExit(RoomIds::MAZE_6));
        m5->setExit(Direction::NE, RoomExit(RoomIds::MAZE_7));
        m5->setExit(Direction::SE, RoomExit(RoomIds::MAZE_8));
        m5->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_5));   // Loops back
    }
    
    // MAZE_6
    auto* m6 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_6));
    if (m6) {
        m6->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_7));
        m6->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_8));
        m6->setExit(Direction::UP, RoomExit(RoomIds::MAZE_9));
        m6->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_6));  // Loops back
    }
    
    // MAZE_7
    auto* m7 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_7));
    if (m7) {
        m7->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_8));
        m7->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_9));
        m7->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_10));
        m7->setExit(Direction::UP, RoomExit(RoomIds::MAZE_7));     // Loops back
    }
    
    // MAZE_8
    auto* m8 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_8));
    if (m8) {
        m8->setExit(Direction::NE, RoomExit(RoomIds::MAZE_9));
        m8->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_10));
        m8->setExit(Direction::DOWN, RoomExit(RoomIds::MAZE_11));
        m8->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_8));   // Loops back
    }
    
    // MAZE_9
    auto* m9 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_9));
    if (m9) {
        m9->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_10));
        m9->setExit(Direction::SOUTH, RoomExit(RoomIds::DEAD_END_2));
        m9->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_12));
        m9->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_9));  // Loops back
    }
    
    // MAZE_10
    auto* m10 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_10));
    if (m10) {
        m10->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_11));
        m10->setExit(Direction::DOWN, RoomExit(RoomIds::MAZE_12));
        m10->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_13));
        m10->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_10)); // Loops back
    }
    
    // MAZE_11
    auto* m11 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_11));
    if (m11) {
        m11->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_12));
        m11->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_13));
        m11->setExit(Direction::WEST, RoomExit(RoomIds::DEAD_END_3));
        m11->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_11));  // Loops back
    }
    
    // MAZE_12
    auto* m12 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_12));
    if (m12) {
        m12->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_13));
        m12->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_14));
        m12->setExit(Direction::UP, RoomExit(RoomIds::MAZE_15));
        m12->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_12));  // Loops back
    }
    
    // MAZE_13
    auto* m13 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_13));
    if (m13) {
        m13->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_14));
        m13->setExit(Direction::DOWN, RoomExit(RoomIds::MAZE_15));
        m13->setExit(Direction::EAST, RoomExit(RoomIds::DEAD_END_4));
        m13->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_13)); // Loops back
    }
    
    // MAZE_14
    auto* m14 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_14));
    if (m14) {
        m14->setExit(Direction::NW, RoomExit(RoomIds::MAZE_15));
        m14->setExit(Direction::SW, RoomExit(RoomIds::GRATING_ROOM));  // Exit to grating room
        m14->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_14));  // Loops back
        m14->setExit(Direction::UP, RoomExit(RoomIds::MAZE_14));    // Loops back
    }
    
    // MAZE_15
    auto* m15 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::MAZE_15));
    if (m15) {
        m15->setExit(Direction::NORTH, RoomExit(RoomIds::GRATING_ROOM));  // Exit to grating room
        m15->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_15));  // Loops back
        m15->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_15)); // Loops back
        m15->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_15));  // Loops back
    }
    
    // Dead ends in maze - all lead back to their source
    auto* de1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_1));
    if (de1) {
        de1->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_4));
    }
    
    auto* de2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_2));
    if (de2) {
        de2->setExit(Direction::NORTH, RoomExit(RoomIds::MAZE_9));
    }
    
    auto* de3 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_3));
    if (de3) {
        de3->setExit(Direction::EAST, RoomExit(RoomIds::MAZE_11));
    }
    
    auto* de4 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::DEAD_END_4));
    if (de4) {
        de4->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_13));
    }
    
    // Grating Room - connects maze to main underground
    auto* gratingRm = dynamic_cast<ZRoom*>(g.getObject(RoomIds::GRATING_ROOM));
    if (gratingRm) {
        gratingRm->setExit(Direction::SOUTH, RoomExit(RoomIds::MAZE_14));
        gratingRm->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_15));
        gratingRm->setExit(Direction::UP, RoomExit(RoomIds::GRATING_CLEARING));  // To surface via grating
        // Note: UP exit will require grating to be open in actual gameplay
    }
    
    // Update Troll Room to have entrance to maze (west exit)
    // This was already set up in the Troll Room creation, but verify it's correct
    auto* trollRm = dynamic_cast<ZRoom*>(g.getObject(RoomIds::TROLL_ROOM));
    if (trollRm) {
        // West exit to maze entrance (MAZE_1) - requires troll to be defeated
        trollRm->setExit(Direction::WEST, RoomExit(RoomIds::MAZE_1));
    }
    
    // Update Grating Clearing to have down exit to Grating Room
    auto* gratingClearingPtr = dynamic_cast<ZRoom*>(g.getObject(RoomIds::GRATING_CLEARING));
    if (gratingClearingPtr) {
        gratingClearingPtr->setExit(Direction::DOWN, RoomExit(RoomIds::GRATING_ROOM));
        // Note: DOWN exit will require grating to be open in actual gameplay
    }
    
    // ===== SPECIAL AREA ROOMS =====
    
    // Create Cyclops Room
    auto cyclopsRoom = std::make_unique<ZRoom>(
        RoomIds::CYCLOPS_ROOM,
        "Cyclops Room",
        "This is a large room with a ceiling which cannot be detected from the ground. There is a narrow passage from east to west and a stone stairway leading upward. The room is eerie in its quietness."
    );
    cyclopsRoom->setFlag(ObjectFlag::RLANDBIT);
    cyclopsRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room with a ceiling which cannot be detected from the ground. There is a narrow passage from east to west and a stone stairway leading upward. The room is eerie in its quietness.");
        }
    });
    cyclopsRoom->setExit(Direction::EAST, RoomExit(RoomIds::STRANGE_PASSAGE));
    cyclopsRoom->setExit(Direction::UP, RoomExit(RoomIds::TREASURE_ROOM));
    // Note: UP exit will be conditional on cyclops being defeated/fed
    g.registerObject(RoomIds::CYCLOPS_ROOM, std::move(cyclopsRoom));
    
    // Create Strange Passage
    auto strangePassage = std::make_unique<ZRoom>(
        RoomIds::STRANGE_PASSAGE,
        "Strange Passage",
        "This is a long passage. To the west is one entrance. On the east there is an old wooden door, with a large hole in it (about cyclops sized)."
    );
    strangePassage->setFlag(ObjectFlag::RLANDBIT);
    strangePassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a long passage. To the west is one entrance. On the east there is an old wooden door, with a large hole in it (about cyclops sized).");
        }
    });
    strangePassage->setExit(Direction::WEST, RoomExit(RoomIds::CYCLOPS_ROOM));
    // East exit will be added when connecting to other underground areas
    g.registerObject(RoomIds::STRANGE_PASSAGE, std::move(strangePassage));
    
    // Create Treasure Room (Thief's Lair)
    auto treasureRoom = std::make_unique<ZRoom>(
        RoomIds::TREASURE_ROOM,
        "Treasure Room",
        "This is a large room, whose east wall is solid granite. A number of discarded bags, which crumble at your touch, are scattered about on the floor. There is an exit down a staircase."
    );
    treasureRoom->setFlag(ObjectFlag::RLANDBIT);
    treasureRoom->setProperty(P_VALUE, 25);  // Room has value for scoring
    treasureRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room, whose east wall is solid granite. A number of discarded bags, which crumble at your touch, are scattered about on the floor. There is an exit down a staircase.");
        }
        // M_ENTER action will trigger thief encounter (to be implemented with thief system)
    });
    treasureRoom->setExit(Direction::DOWN, RoomExit(RoomIds::CYCLOPS_ROOM));
    g.registerObject(RoomIds::TREASURE_ROOM, std::move(treasureRoom));
    
    // Create Entrance to Hades
    auto entranceToHades = std::make_unique<ZRoom>(
        RoomIds::ENTRANCE_TO_HADES,
        "Entrance to Hades",
        "You are outside a large gateway, on which is inscribed:\n\n  Abandon every hope all ye who enter here\n\nThe gate is open; through it you can see a desolation, with a pile of mangled bodies in one corner. Thousands of voices, lamenting some hideous fate, can be heard."
    );
    entranceToHades->setFlag(ObjectFlag::RLANDBIT);
    entranceToHades->setFlag(ObjectFlag::ONBIT);
    entranceToHades->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are outside a large gateway, on which is inscribed:");
            crlf();
            printLine("  Abandon every hope all ye who enter here");
            crlf();
            printLine("The gate is open; through it you can see a desolation, with a pile of mangled bodies in one corner. Thousands of voices, lamenting some hideous fate, can be heard.");
        }
    });
    entranceToHades->setExit(Direction::UP, RoomExit(RoomIds::TINY_CAVE));
    entranceToHades->setExit(Direction::IN, RoomExit(RoomIds::LAND_OF_LIVING_DEAD));
    entranceToHades->setExit(Direction::SOUTH, RoomExit(RoomIds::LAND_OF_LIVING_DEAD));
    // Note: IN and SOUTH exits will be conditional on LLD-FLAG in actual gameplay
    g.registerObject(RoomIds::ENTRANCE_TO_HADES, std::move(entranceToHades));
    
    // Create Land of the Living Dead
    auto landOfLivingDead = std::make_unique<ZRoom>(
        RoomIds::LAND_OF_LIVING_DEAD,
        "Land of the Dead",
        "You have entered the Land of the Living Dead. Thousands of lost souls can be heard weeping and moaning. In the corner are stacked the remains of dozens of previous adventurers less fortunate than yourself. A passage exits to the north."
    );
    landOfLivingDead->setFlag(ObjectFlag::RLANDBIT);
    landOfLivingDead->setFlag(ObjectFlag::ONBIT);
    landOfLivingDead->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You have entered the Land of the Living Dead. Thousands of lost souls can be heard weeping and moaning. In the corner are stacked the remains of dozens of previous adventurers less fortunate than yourself. A passage exits to the north.");
        }
    });
    landOfLivingDead->setExit(Direction::OUT, RoomExit(RoomIds::ENTRANCE_TO_HADES));
    landOfLivingDead->setExit(Direction::NORTH, RoomExit(RoomIds::ENTRANCE_TO_HADES));
    g.registerObject(RoomIds::LAND_OF_LIVING_DEAD, std::move(landOfLivingDead));
    
    // Create Dome Room
    auto domeRoom = std::make_unique<ZRoom>(
        RoomIds::DOME_ROOM,
        "Dome Room",
        "You are at the periphery of a large dome, which forms the ceiling of another room below. Protecting you from a precipitous drop is a wooden railing which circles the dome."
    );
    domeRoom->setFlag(ObjectFlag::RLANDBIT);
    domeRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are at the periphery of a large dome, which forms the ceiling of another room below. Protecting you from a precipitous drop is a wooden railing which circles the dome.");
        }
    });
    domeRoom->setExit(Direction::WEST, RoomExit(RoomIds::ENGRAVINGS_CAVE));
    domeRoom->setExit(Direction::DOWN, RoomExit(RoomIds::TORCH_ROOM));
    // Note: DOWN exit will be conditional on DOME-FLAG (rope tied) in actual gameplay
    g.registerObject(RoomIds::DOME_ROOM, std::move(domeRoom));
    
    // Create Torch Room
    auto torchRoom = std::make_unique<ZRoom>(
        RoomIds::TORCH_ROOM,
        "Torch Room",
        "This is a large room with a prominent doorway leading to a down staircase. To the west is a narrow twisting tunnel, through which is coming a horrible stench. Above you is a large dome painted with scenes depicting elvish hacking rites. Up around the edge of the dome (20 feet up) is a wooden railing. In the center of the room there is a white marble pedestal."
    );
    torchRoom->setFlag(ObjectFlag::RLANDBIT);
    torchRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a large room with a prominent doorway leading to a down staircase. To the west is a narrow twisting tunnel, through which is coming a horrible stench. Above you is a large dome painted with scenes depicting elvish hacking rites. Up around the edge of the dome (20 feet up) is a wooden railing. In the center of the room there is a white marble pedestal.");
        }
    });
    torchRoom->setExit(Direction::UP, RoomExit("You cannot reach the rope."));
    torchRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::NORTH_TEMPLE));
    torchRoom->setExit(Direction::DOWN, RoomExit(RoomIds::NORTH_TEMPLE));
    g.registerObject(RoomIds::TORCH_ROOM, std::move(torchRoom));
    
    // Create North Temple
    auto northTemple = std::make_unique<ZRoom>(
        RoomIds::NORTH_TEMPLE,
        "Temple",
        "This is the north end of a large temple. On the east wall is an ancient inscription, probably a prayer in a long-forgotten language. Below the prayer is a staircase leading down. The west wall is solid granite. The exit to the north end of the room is through huge marble pillars."
    );
    northTemple->setFlag(ObjectFlag::RLANDBIT);
    northTemple->setFlag(ObjectFlag::ONBIT);
    northTemple->setFlag(ObjectFlag::SACREDBIT);
    northTemple->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is the north end of a large temple. On the east wall is an ancient inscription, probably a prayer in a long-forgotten language. Below the prayer is a staircase leading down. The west wall is solid granite. The exit to the north end of the room is through huge marble pillars.");
        }
    });
    northTemple->setExit(Direction::DOWN, RoomExit(RoomIds::EGYPT_ROOM));
    northTemple->setExit(Direction::EAST, RoomExit(RoomIds::EGYPT_ROOM));
    northTemple->setExit(Direction::NORTH, RoomExit(RoomIds::TORCH_ROOM));
    northTemple->setExit(Direction::OUT, RoomExit(RoomIds::TORCH_ROOM));
    northTemple->setExit(Direction::UP, RoomExit(RoomIds::TORCH_ROOM));
    northTemple->setExit(Direction::SOUTH, RoomExit(RoomIds::SOUTH_TEMPLE));
    g.registerObject(RoomIds::NORTH_TEMPLE, std::move(northTemple));
    
    // Create South Temple (Altar)
    auto southTemple = std::make_unique<ZRoom>(
        RoomIds::SOUTH_TEMPLE,
        "Altar",
        "This is the south end of a large temple. In front of you is what appears to be an altar. In one corner is a small hole in the floor which leads into darkness. You probably could not get back up it."
    );
    southTemple->setFlag(ObjectFlag::RLANDBIT);
    southTemple->setFlag(ObjectFlag::ONBIT);
    southTemple->setFlag(ObjectFlag::SACREDBIT);
    southTemple->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is the south end of a large temple. In front of you is what appears to be an altar. In one corner is a small hole in the floor which leads into darkness. You probably could not get back up it.");
        }
    });
    southTemple->setExit(Direction::NORTH, RoomExit(RoomIds::NORTH_TEMPLE));
    southTemple->setExit(Direction::DOWN, RoomExit(RoomIds::TINY_CAVE));
    // Note: DOWN exit will be conditional on COFFIN-CURE flag in actual gameplay
    g.registerObject(RoomIds::SOUTH_TEMPLE, std::move(southTemple));
    
    // Create Egyptian Room
    auto egyptRoom = std::make_unique<ZRoom>(
        RoomIds::EGYPT_ROOM,
        "Egyptian Room",
        "This is a room which looks like an Egyptian tomb. There is an ascending staircase to the west."
    );
    egyptRoom->setFlag(ObjectFlag::RLANDBIT);
    egyptRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a room which looks like an Egyptian tomb. There is an ascending staircase to the west.");
        }
    });
    egyptRoom->setExit(Direction::WEST, RoomExit(RoomIds::NORTH_TEMPLE));
    egyptRoom->setExit(Direction::UP, RoomExit(RoomIds::NORTH_TEMPLE));
    g.registerObject(RoomIds::EGYPT_ROOM, std::move(egyptRoom));
    
    // Create Mirror Room 1
    auto mirrorRoom1 = std::make_unique<ZRoom>(
        RoomIds::MIRROR_ROOM_1,
        "Mirror Room",
        "You are in a large square room with tall ceilings. On the south wall is an enormous mirror which fills the entire wall. There are exits on the other three sides of the room."
    );
    mirrorRoom1->setFlag(ObjectFlag::RLANDBIT);
    mirrorRoom1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a large square room with tall ceilings. On the south wall is an enormous mirror which fills the entire wall. There are exits on the other three sides of the room.");
        }
    });
    mirrorRoom1->setExit(Direction::NORTH, RoomExit(RoomIds::COLD_PASSAGE));
    mirrorRoom1->setExit(Direction::WEST, RoomExit(RoomIds::TWISTING_PASSAGE));
    mirrorRoom1->setExit(Direction::EAST, RoomExit(RoomIds::SMALL_CAVE));
    g.registerObject(RoomIds::MIRROR_ROOM_1, std::move(mirrorRoom1));
    
    // Create Mirror Room 2
    auto mirrorRoom2 = std::make_unique<ZRoom>(
        RoomIds::MIRROR_ROOM_2,
        "Mirror Room",
        "You are in a large square room with tall ceilings. On the south wall is an enormous mirror which fills the entire wall. There are exits on the other three sides of the room."
    );
    mirrorRoom2->setFlag(ObjectFlag::RLANDBIT);
    mirrorRoom2->setFlag(ObjectFlag::ONBIT);
    mirrorRoom2->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("You are in a large square room with tall ceilings. On the south wall is an enormous mirror which fills the entire wall. There are exits on the other three sides of the room.");
        }
    });
    mirrorRoom2->setExit(Direction::WEST, RoomExit(RoomIds::WINDING_PASSAGE));
    mirrorRoom2->setExit(Direction::NORTH, RoomExit(RoomIds::NARROW_PASSAGE));
    mirrorRoom2->setExit(Direction::EAST, RoomExit(RoomIds::TINY_CAVE));
    g.registerObject(RoomIds::MIRROR_ROOM_2, std::move(mirrorRoom2));
    
    // Create Small Cave
    auto smallCave = std::make_unique<ZRoom>(
        RoomIds::SMALL_CAVE,
        "Cave",
        "This is a tiny cave with entrances west and north, and a staircase leading down."
    );
    smallCave->setFlag(ObjectFlag::RLANDBIT);
    smallCave->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a tiny cave with entrances west and north, and a staircase leading down.");
        }
    });
    smallCave->setExit(Direction::NORTH, RoomExit(RoomIds::MIRROR_ROOM_1));
    smallCave->setExit(Direction::DOWN, RoomExit(RoomIds::ATLANTIS_ROOM));
    smallCave->setExit(Direction::SOUTH, RoomExit(RoomIds::ATLANTIS_ROOM));
    smallCave->setExit(Direction::WEST, RoomExit(RoomIds::TWISTING_PASSAGE));
    g.registerObject(RoomIds::SMALL_CAVE, std::move(smallCave));
    
    // Create Tiny Cave
    auto tinyCave = std::make_unique<ZRoom>(
        RoomIds::TINY_CAVE,
        "Cave",
        "This is a tiny cave with entrances west and north, and a dark, forbidding staircase leading down."
    );
    tinyCave->setFlag(ObjectFlag::RLANDBIT);
    tinyCave->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a tiny cave with entrances west and north, and a dark, forbidding staircase leading down.");
        }
    });
    tinyCave->setExit(Direction::NORTH, RoomExit(RoomIds::MIRROR_ROOM_2));
    tinyCave->setExit(Direction::WEST, RoomExit(RoomIds::WINDING_PASSAGE));
    tinyCave->setExit(Direction::DOWN, RoomExit(RoomIds::ENTRANCE_TO_HADES));
    g.registerObject(RoomIds::TINY_CAVE, std::move(tinyCave));
    
    // Create Winding Passage
    auto windingPassage = std::make_unique<ZRoom>(
        RoomIds::WINDING_PASSAGE,
        "Winding Passage",
        "This is a winding passage. It seems that there are only exits on the east and north."
    );
    windingPassage->setFlag(ObjectFlag::RLANDBIT);
    windingPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a winding passage. It seems that there are only exits on the east and north.");
        }
    });
    windingPassage->setExit(Direction::NORTH, RoomExit(RoomIds::MIRROR_ROOM_2));
    windingPassage->setExit(Direction::EAST, RoomExit(RoomIds::TINY_CAVE));
    g.registerObject(RoomIds::WINDING_PASSAGE, std::move(windingPassage));
    
    // Create Twisting Passage
    auto twistingPassage = std::make_unique<ZRoom>(
        RoomIds::TWISTING_PASSAGE,
        "Twisting Passage",
        "This is a winding passage. It seems that there are only exits on the east and north."
    );
    twistingPassage->setFlag(ObjectFlag::RLANDBIT);
    twistingPassage->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a winding passage. It seems that there are only exits on the east and north.");
        }
    });
    twistingPassage->setExit(Direction::NORTH, RoomExit(RoomIds::MIRROR_ROOM_1));
    twistingPassage->setExit(Direction::EAST, RoomExit(RoomIds::SMALL_CAVE));
    g.registerObject(RoomIds::TWISTING_PASSAGE, std::move(twistingPassage));
    
    // Create Atlantis Room
    auto atlantisRoom = std::make_unique<ZRoom>(
        RoomIds::ATLANTIS_ROOM,
        "Atlantis Room",
        "This is an ancient room, long under water. There is an exit to the south and a staircase leading up."
    );
    atlantisRoom->setFlag(ObjectFlag::RLANDBIT);
    atlantisRoom->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is an ancient room, long under water. There is an exit to the south and a staircase leading up.");
        }
    });
    atlantisRoom->setExit(Direction::UP, RoomExit(RoomIds::SMALL_CAVE));
    atlantisRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::RESERVOIR_NORTH));
    g.registerObject(RoomIds::ATLANTIS_ROOM, std::move(atlantisRoom));
    
    // Create Mailbox object
    auto mailbox = std::make_unique<ZObject>(OBJ_MAILBOX, "small mailbox");
    mailbox->addSynonym("mailbox");
    mailbox->addSynonym("box");
    mailbox->addAdjective("small");
    mailbox->setFlag(ObjectFlag::CONTBIT);
    mailbox->setFlag(ObjectFlag::TRYTAKEBIT);
    mailbox->setProperty(P_CAPACITY, 10);
    mailbox->setAction(mailboxAction);
    mailbox->moveTo(g.getObject(ROOM_WEST_OF_HOUSE));
    
    g.registerObject(OBJ_MAILBOX, std::move(mailbox));
    
    // Create Trophy Case container
    auto trophyCase = std::make_unique<ZObject>(ObjectIds::TROPHY_CASE, "trophy case");
    trophyCase->addSynonym("case");
    trophyCase->addAdjective("trophy");
    trophyCase->setFlag(ObjectFlag::CONTBIT);
    trophyCase->setFlag(ObjectFlag::TRANSBIT);  // Transparent - can see contents
    trophyCase->setFlag(ObjectFlag::OPENBIT);   // Initially open
    trophyCase->setFlag(ObjectFlag::TRYTAKEBIT); // Cannot be taken (anchored)
    trophyCase->setFlag(ObjectFlag::NDESCBIT);   // Don't describe separately
    trophyCase->setFlag(ObjectFlag::SEARCHBIT);  // Can be searched
    trophyCase->setProperty(P_CAPACITY, 10000);  // Large capacity for treasures
    // TODO: Add action handler for scoring treasures
    trophyCase->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    
    g.registerObject(ObjectIds::TROPHY_CASE, std::move(trophyCase));
    
    // Create SACK (Brown sack/sandwich bag) - portable container
    auto sack = std::make_unique<ZObject>(ObjectIds::SACK, "brown sack");
    sack->addSynonym("sack");
    sack->addSynonym("bag");
    sack->addAdjective("brown");
    sack->addAdjective("elongated");
    sack->addAdjective("smelly");
    sack->setFlag(ObjectFlag::TAKEBIT);
    sack->setFlag(ObjectFlag::CONTBIT);
    sack->setFlag(ObjectFlag::OPENBIT);    // Initially open
    sack->setFlag(ObjectFlag::BURNBIT);    // Can be burned
    sack->setProperty(P_CAPACITY, 9);
    sack->setProperty(P_SIZE, 9);
    // TODO: Add action handler
    sack->moveTo(g.getObject(RoomIds::KITCHEN));  // On kitchen table
    
    g.registerObject(ObjectIds::SACK, std::move(sack));
    
    // Create BAG (Large bag - thief's bag) - portable container
    auto bag = std::make_unique<ZObject>(ObjectIds::BAG, "large bag");
    bag->addSynonym("bag");
    bag->addSynonym("sack");
    bag->addAdjective("large");
    bag->setFlag(ObjectFlag::TAKEBIT);
    bag->setFlag(ObjectFlag::CONTBIT);
    bag->setFlag(ObjectFlag::OPENBIT);     // Initially open
    bag->setProperty(P_CAPACITY, 100);     // Large capacity
    bag->setProperty(P_SIZE, 20);
    // TODO: Add action handler
    // Initially with thief - will be placed when thief is created
    
    g.registerObject(ObjectIds::BAG, std::move(bag));
    
    // Create RAISED_BASKET (Basket at top of shaft) - special container
    auto raisedBasket = std::make_unique<ZObject>(ObjectIds::RAISED_BASKET, "basket");
    raisedBasket->addSynonym("basket");
    raisedBasket->addSynonym("cage");
    raisedBasket->addSynonym("dumbwaiter");
    raisedBasket->setFlag(ObjectFlag::CONTBIT);
    raisedBasket->setFlag(ObjectFlag::TRANSBIT);  // Transparent
    raisedBasket->setFlag(ObjectFlag::OPENBIT);   // Open
    raisedBasket->setFlag(ObjectFlag::TRYTAKEBIT); // Cannot be taken
    raisedBasket->setProperty(P_CAPACITY, 50);
    // TODO: Add action handler for basket mechanics
    raisedBasket->moveTo(g.getObject(RoomIds::SHAFT_ROOM));
    
    g.registerObject(ObjectIds::RAISED_BASKET, std::move(raisedBasket));
    
    // Create LOWERED_BASKET (Basket at bottom of shaft) - special container
    auto loweredBasket = std::make_unique<ZObject>(ObjectIds::BASKET, "basket");
    loweredBasket->addSynonym("basket");
    loweredBasket->addSynonym("cage");
    loweredBasket->addSynonym("dumbwaiter");
    loweredBasket->addAdjective("lowered");
    loweredBasket->setFlag(ObjectFlag::TRYTAKEBIT); // Cannot be taken
    // Note: This basket is not a container when lowered - it becomes one when raised
    // TODO: Add action handler for basket mechanics
    loweredBasket->moveTo(g.getObject(RoomIds::LOWER_SHAFT));
    
    g.registerObject(ObjectIds::BASKET, std::move(loweredBasket));
    
    // Create BUOY (Red buoy - portable container)
    auto buoy = std::make_unique<ZObject>(ObjectIds::BUOY, "red buoy");
    buoy->addSynonym("buoy");
    buoy->addAdjective("red");
    buoy->setFlag(ObjectFlag::TAKEBIT);
    buoy->setFlag(ObjectFlag::CONTBIT);
    buoy->setFlag(ObjectFlag::OPENBIT);
    buoy->setProperty(P_CAPACITY, 20);
    buoy->setProperty(P_SIZE, 10);
    // TODO: Add action handler
    // Will be placed in River-4 when that room is created
    
    g.registerObject(ObjectIds::BUOY, std::move(buoy));
    
    // Create NEST (Bird's nest - portable container)
    auto nest = std::make_unique<ZObject>(ObjectIds::NEST, "bird's nest");
    nest->addSynonym("nest");
    nest->addAdjective("birds");
    nest->setFlag(ObjectFlag::TAKEBIT);
    nest->setFlag(ObjectFlag::CONTBIT);
    nest->setFlag(ObjectFlag::OPENBIT);
    nest->setFlag(ObjectFlag::BURNBIT);
    nest->setFlag(ObjectFlag::SEARCHBIT);
    nest->setProperty(P_CAPACITY, 20);
    // TODO: Add action handler
    nest->moveTo(g.getObject(RoomIds::UP_A_TREE));
    
    g.registerObject(ObjectIds::NEST, std::move(nest));
    
    // Create TOOL_CHEST (Group of tool chests - anchored container)
    auto toolChest = std::make_unique<ZObject>(ObjectIds::TOOL_CHEST, "group of tool chests");
    toolChest->addSynonym("chest");
    toolChest->addSynonym("chests");
    toolChest->addSynonym("group");
    toolChest->addSynonym("toolchests");
    toolChest->addAdjective("tool");
    toolChest->setFlag(ObjectFlag::CONTBIT);
    toolChest->setFlag(ObjectFlag::OPENBIT);
    toolChest->setFlag(ObjectFlag::TRYTAKEBIT);  // Cannot be taken
    toolChest->setFlag(ObjectFlag::SACREDBIT);
    // TODO: Add action handler
    // Will be placed in Maintenance Room when that room is created
    
    g.registerObject(ObjectIds::TOOL_CHEST, std::move(toolChest));
    
    // Create ALTAR (Altar - surface container)
    auto altar = std::make_unique<ZObject>(ObjectIds::ALTAR, "altar");
    altar->addSynonym("altar");
    altar->setFlag(ObjectFlag::CONTBIT);
    altar->setFlag(ObjectFlag::OPENBIT);
    altar->setFlag(ObjectFlag::SURFACEBIT);
    altar->setFlag(ObjectFlag::NDESCBIT);
    altar->setProperty(P_CAPACITY, 50);
    // TODO: Add action handler
    // Will be placed in South Temple when that room is created
    
    g.registerObject(ObjectIds::ALTAR, std::move(altar));
    
    // Create PEDESTAL (White marble pedestal - surface container)
    auto pedestal = std::make_unique<ZObject>(ObjectIds::PEDESTAL, "pedestal");
    pedestal->addSynonym("pedestal");
    pedestal->addAdjective("white");
    pedestal->addAdjective("marble");
    pedestal->setFlag(ObjectFlag::CONTBIT);
    pedestal->setFlag(ObjectFlag::OPENBIT);
    pedestal->setFlag(ObjectFlag::SURFACEBIT);
    pedestal->setFlag(ObjectFlag::NDESCBIT);
    pedestal->setProperty(P_CAPACITY, 30);
    // TODO: Add action handler
    // Will be placed in Torch Room when that room is created
    
    g.registerObject(ObjectIds::PEDESTAL, std::move(pedestal));
    
    // Create White House global object
    auto whiteHouse = std::make_unique<ZObject>(OBJ_WHITE_HOUSE, "white house");
    whiteHouse->addSynonym("house");
    whiteHouse->addAdjective("white");
    whiteHouse->addAdjective("beautiful");
    whiteHouse->addAdjective("colonial");
    whiteHouse->setFlag(ObjectFlag::NDESCBIT);
    whiteHouse->setAction(whiteHouseAction);
    
    g.registerObject(OBJ_WHITE_HOUSE, std::move(whiteHouse));
    
    // Create Board global object
    auto board = std::make_unique<ZObject>(OBJ_BOARD, "board");
    board->addSynonym("boards");
    board->addSynonym("board");
    board->setFlag(ObjectFlag::NDESCBIT);
    board->setAction(boardAction);
    
    g.registerObject(OBJ_BOARD, std::move(board));
    
    // Create Forest global object
    auto forest = std::make_unique<ZObject>(OBJ_FOREST, "forest");
    forest->addSynonym("forest");
    forest->addSynonym("trees");
    forest->addSynonym("pines");
    forest->addSynonym("hemlocks");
    forest->setFlag(ObjectFlag::NDESCBIT);
    forest->setAction(forestAction);
    
    g.registerObject(OBJ_FOREST, std::move(forest));
    
    // Create Kitchen Window global object
    auto kitchenWindow = std::make_unique<ZObject>(OBJ_KITCHEN_WINDOW, "kitchen window");
    kitchenWindow->addSynonym("window");
    kitchenWindow->addAdjective("kitchen");
    kitchenWindow->addAdjective("small");
    kitchenWindow->setFlag(ObjectFlag::NDESCBIT);
    kitchenWindow->setFlag(ObjectFlag::DOORBIT);
    kitchenWindow->setAction(kitchenWindowAction);
    
    g.registerObject(OBJ_KITCHEN_WINDOW, std::move(kitchenWindow));
    
    // Create Adventurer object
    auto adventurer = std::make_unique<ZObject>(OBJ_ADVENTURER, "adventurer");
    adventurer->addSynonym("adventurer");
    adventurer->setFlag(ObjectFlag::NDESCBIT);
    adventurer->setFlag(ObjectFlag::INVISIBLE);
    adventurer->setFlag(ObjectFlag::SACREDBIT);
    adventurer->setFlag(ObjectFlag::ACTORBIT);
    adventurer->moveTo(g.here);
    
    g.winner = adventurer.get();
    g.player = adventurer.get();
    g.registerObject(OBJ_ADVENTURER, std::move(adventurer));
    
    // ===== TREASURE OBJECTS =====
    
    // Create TROPHY (Zorkmid trophy)
    auto trophy = std::make_unique<ZObject>(ObjectIds::TROPHY, "trophy");
    trophy->addSynonym("trophy");
    trophy->addSynonym("cup");
    trophy->addAdjective("large");
    trophy->addAdjective("zorkmid");
    trophy->setFlag(ObjectFlag::TAKEBIT);
    trophy->setProperty(P_VALUE, 5);
    trophy->setProperty(P_TVALUE, 5);
    trophy->setProperty(P_SIZE, 5);
    trophy->moveTo(g.getObject(RoomIds::TREASURE_ROOM));
    g.registerObject(ObjectIds::TROPHY, std::move(trophy));
    
    // Create EGG (Jewel-encrusted egg)
    auto egg = std::make_unique<ZObject>(ObjectIds::EGG, "jewel-encrusted egg");
    egg->addSynonym("egg");
    egg->addAdjective("jewel");
    egg->addAdjective("jeweled");
    egg->addAdjective("encrusted");
    egg->setFlag(ObjectFlag::TAKEBIT);
    egg->setFlag(ObjectFlag::CONTBIT);
    egg->setFlag(ObjectFlag::OPENBIT);
    egg->setProperty(P_VALUE, 5);
    egg->setProperty(P_TVALUE, 5);
    egg->setProperty(P_SIZE, 5);
    egg->setProperty(P_CAPACITY, 5);
    egg->moveTo(g.getObject(RoomIds::UP_A_TREE));  // In nest in tree
    g.registerObject(ObjectIds::EGG, std::move(egg));
    
    // Create CHALICE (Chalice)
    auto chalice = std::make_unique<ZObject>(ObjectIds::CHALICE, "chalice");
    chalice->addSynonym("chalice");
    chalice->addSynonym("goblet");
    chalice->addSynonym("grail");
    chalice->setFlag(ObjectFlag::TAKEBIT);
    chalice->setProperty(P_VALUE, 10);
    chalice->setProperty(P_TVALUE, 10);
    chalice->setProperty(P_SIZE, 10);
    chalice->moveTo(g.getObject(RoomIds::TREASURE_ROOM));
    g.registerObject(ObjectIds::CHALICE, std::move(chalice));
    
    // Create TRIDENT (Crystal trident)
    auto trident = std::make_unique<ZObject>(ObjectIds::TRIDENT, "crystal trident");
    trident->addSynonym("trident");
    trident->addSynonym("fork");
    trident->addAdjective("crystal");
    trident->addAdjective("poseidon");
    trident->setFlag(ObjectFlag::TAKEBIT);
    trident->setFlag(ObjectFlag::WEAPONBIT);
    trident->setProperty(P_VALUE, 4);
    trident->setProperty(P_TVALUE, 11);
    trident->setProperty(P_SIZE, 10);
    trident->moveTo(g.getObject(RoomIds::ATLANTIS_ROOM));
    g.registerObject(ObjectIds::TRIDENT, std::move(trident));
    
    // Create JEWELS (Trunk of jewels)
    auto jewels = std::make_unique<ZObject>(ObjectIds::JEWELS, "trunk of jewels");
    jewels->addSynonym("jewels");
    jewels->addSynonym("trunk");
    jewels->addSynonym("jewelry");
    jewels->addSynonym("gems");
    jewels->addAdjective("precious");
    jewels->setFlag(ObjectFlag::TAKEBIT);
    jewels->setProperty(P_VALUE, 5);
    jewels->setProperty(P_TVALUE, 5);
    jewels->setProperty(P_SIZE, 10);
    jewels->moveTo(g.getObject(RoomIds::TREASURE_ROOM));
    g.registerObject(ObjectIds::JEWELS, std::move(jewels));
    
    // Create COINS (Bag of coins)
    auto coins = std::make_unique<ZObject>(ObjectIds::COINS, "bag of coins");
    coins->addSynonym("coins");
    coins->addSynonym("bag");
    coins->addSynonym("coin");
    coins->addSynonym("zorkmids");
    coins->addAdjective("leather");
    coins->addAdjective("large");
    coins->setFlag(ObjectFlag::TAKEBIT);
    coins->setProperty(P_VALUE, 5);
    coins->setProperty(P_TVALUE, 5);
    coins->setProperty(P_SIZE, 10);
    coins->moveTo(g.getObject(RoomIds::MAZE_11));
    g.registerObject(ObjectIds::COINS, std::move(coins));
    
    // Create DIAMOND (Huge diamond)
    auto diamond = std::make_unique<ZObject>(ObjectIds::DIAMOND, "huge diamond");
    diamond->addSynonym("diamond");
    diamond->addSynonym("gem");
    diamond->addAdjective("huge");
    diamond->addAdjective("enormous");
    diamond->setFlag(ObjectFlag::TAKEBIT);
    diamond->setProperty(P_VALUE, 10);
    diamond->setProperty(P_TVALUE, 10);
    diamond->setProperty(P_SIZE, 5);
    diamond->moveTo(g.getObject(RoomIds::EAST_OF_CHASM));
    g.registerObject(ObjectIds::DIAMOND, std::move(diamond));
    
    // Create EMERALD (Emerald bracelet)
    auto emerald = std::make_unique<ZObject>(ObjectIds::EMERALD, "emerald bracelet");
    emerald->addSynonym("bracelet");
    emerald->addSynonym("emerald");
    emerald->addAdjective("emerald");
    emerald->addAdjective("beautiful");
    emerald->setFlag(ObjectFlag::TAKEBIT);
    emerald->setProperty(P_VALUE, 5);
    emerald->setProperty(P_TVALUE, 5);
    emerald->setProperty(P_SIZE, 2);
    emerald->moveTo(g.getObject(RoomIds::SOUTH_TEMPLE));
    g.registerObject(ObjectIds::EMERALD, std::move(emerald));
    
    // Create PAINTING (Painting - special treasure that can be taken from wall)
    auto painting = std::make_unique<ZObject>(ObjectIds::PAINTING, "painting");
    painting->addSynonym("painting");
    painting->addSynonym("picture");
    painting->addAdjective("beautiful");
    painting->addAdjective("old");
    painting->setFlag(ObjectFlag::TAKEBIT);
    painting->setProperty(P_VALUE, 4);
    painting->setProperty(P_TVALUE, 6);
    painting->setProperty(P_SIZE, 15);
    painting->moveTo(g.getObject(RoomIds::GALLERY));
    // TODO: Add action handler for examining back side
    g.registerObject(ObjectIds::PAINTING, std::move(painting));
    
    // Create COFFIN (Gold coffin - both treasure and container)
    auto coffin = std::make_unique<ZObject>(ObjectIds::COFFIN, "gold coffin");
    coffin->addSynonym("coffin");
    coffin->addSynonym("casket");
    coffin->addSynonym("treasure");
    coffin->addAdjective("solid");
    coffin->addAdjective("gold");
    coffin->setFlag(ObjectFlag::TAKEBIT);
    coffin->setFlag(ObjectFlag::CONTBIT);
    coffin->setFlag(ObjectFlag::SACREDBIT);
    coffin->setFlag(ObjectFlag::SEARCHBIT);
    coffin->setProperty(P_VALUE, 10);
    coffin->setProperty(P_TVALUE, 10);
    coffin->setProperty(P_SIZE, 55);
    coffin->setProperty(P_CAPACITY, 35);
    // TODO: Add action handler
    coffin->moveTo(g.getObject(RoomIds::EGYPT_ROOM));
    g.registerObject(ObjectIds::COFFIN, std::move(coffin));
    
    // Create JADE (Jade figurine)
    auto jade = std::make_unique<ZObject>(ObjectIds::JADE, "jade figurine");
    jade->addSynonym("figurine");
    jade->addSynonym("jade");
    jade->addSynonym("statue");
    jade->addAdjective("jade");
    jade->addAdjective("small");
    jade->setFlag(ObjectFlag::TAKEBIT);
    jade->setProperty(P_VALUE, 5);
    jade->setProperty(P_TVALUE, 5);
    jade->setProperty(P_SIZE, 3);
    jade->moveTo(g.getObject(RoomIds::MAZE_15));
    g.registerObject(ObjectIds::JADE, std::move(jade));
    
    // Create BRACELET (Sapphire bracelet)
    auto bracelet = std::make_unique<ZObject>(ObjectIds::BRACELET, "sapphire bracelet");
    bracelet->addSynonym("bracelet");
    bracelet->addSynonym("sapphire");
    bracelet->addAdjective("sapphire");
    bracelet->addAdjective("beautiful");
    bracelet->setFlag(ObjectFlag::TAKEBIT);
    bracelet->setProperty(P_VALUE, 5);
    bracelet->setProperty(P_TVALUE, 5);
    bracelet->setProperty(P_SIZE, 2);
    bracelet->moveTo(g.getObject(RoomIds::SOUTH_TEMPLE));
    g.registerObject(ObjectIds::BRACELET, std::move(bracelet));
    
    // Create BAR (Platinum bar)
    auto bar = std::make_unique<ZObject>(ObjectIds::BAR, "platinum bar");
    bar->addSynonym("bar");
    bar->addSynonym("platinum");
    bar->addAdjective("platinum");
    bar->addAdjective("heavy");
    bar->setFlag(ObjectFlag::TAKEBIT);
    bar->setProperty(P_VALUE, 10);
    bar->setProperty(P_TVALUE, 10);
    bar->setProperty(P_SIZE, 10);
    bar->moveTo(g.getObject(RoomIds::LOUD_ROOM));
    g.registerObject(ObjectIds::BAR, std::move(bar));
    
    // Create POT_OF_GOLD (Pot of gold)
    auto potOfGold = std::make_unique<ZObject>(ObjectIds::POT_OF_GOLD, "pot of gold");
    potOfGold->addSynonym("pot");
    potOfGold->addSynonym("gold");
    potOfGold->addAdjective("gold");
    potOfGold->addAdjective("golden");
    potOfGold->setFlag(ObjectFlag::TAKEBIT);
    potOfGold->setProperty(P_VALUE, 10);
    potOfGold->setProperty(P_TVALUE, 10);
    potOfGold->setProperty(P_SIZE, 10);
    potOfGold->moveTo(g.getObject(RoomIds::END_OF_RAINBOW));
    g.registerObject(ObjectIds::POT_OF_GOLD, std::move(potOfGold));
    
    // Create SCARAB (Scarab)
    auto scarab = std::make_unique<ZObject>(ObjectIds::SCARAB, "scarab");
    scarab->addSynonym("scarab");
    scarab->addSynonym("beetle");
    scarab->addAdjective("beautiful");
    scarab->addAdjective("ancient");
    scarab->setFlag(ObjectFlag::TAKEBIT);
    scarab->setProperty(P_VALUE, 5);
    scarab->setProperty(P_TVALUE, 5);
    scarab->setProperty(P_SIZE, 2);
    scarab->moveTo(g.getObject(RoomIds::EGYPT_ROOM));
    g.registerObject(ObjectIds::SCARAB, std::move(scarab));
    
    // Create TORCH (Ivory torch - also a light source)
    auto torch = std::make_unique<ZObject>(ObjectIds::TORCH, "ivory torch");
    torch->addSynonym("torch");
    torch->addAdjective("ivory");
    torch->setFlag(ObjectFlag::TAKEBIT);
    torch->setFlag(ObjectFlag::LIGHTBIT);
    torch->setFlag(ObjectFlag::ONBIT);
    torch->setProperty(P_VALUE, 6);
    torch->setProperty(P_TVALUE, 14);
    torch->setProperty(P_SIZE, 8);
    torch->moveTo(g.getObject(RoomIds::NORTH_TEMPLE));
    g.registerObject(ObjectIds::TORCH, std::move(torch));
    
    // Create SCEPTRE (Egyptian sceptre - also a weapon)
    auto sceptre = std::make_unique<ZObject>(ObjectIds::SCEPTRE, "sceptre");
    sceptre->addSynonym("sceptre");
    sceptre->addSynonym("scepter");
    sceptre->addAdjective("egyptian");
    sceptre->addAdjective("gold");
    sceptre->setFlag(ObjectFlag::TAKEBIT);
    sceptre->setFlag(ObjectFlag::WEAPONBIT);
    sceptre->setProperty(P_VALUE, 4);
    sceptre->setProperty(P_TVALUE, 6);
    sceptre->setProperty(P_SIZE, 8);
    sceptre->moveTo(g.getObject(RoomIds::EGYPT_ROOM));
    g.registerObject(ObjectIds::SCEPTRE, std::move(sceptre));
    
    // Create CANARY (Clockwork canary)
    auto canary = std::make_unique<ZObject>(ObjectIds::CANARY, "clockwork canary");
    canary->addSynonym("canary");
    canary->addSynonym("bird");
    canary->addAdjective("clockwork");
    canary->addAdjective("mechanical");
    canary->addAdjective("wind");
    canary->setFlag(ObjectFlag::TAKEBIT);
    canary->setProperty(P_VALUE, 4);
    canary->setProperty(P_TVALUE, 8);
    canary->setProperty(P_SIZE, 3);
    canary->moveTo(g.getObject(RoomIds::MINE_4));  // Coal mine area
    // TODO: Add action handler for winding and breaking
    g.registerObject(ObjectIds::CANARY, std::move(canary));
    
    // Create BAUBLE (Brass bauble)
    auto bauble = std::make_unique<ZObject>(ObjectIds::BAUBLE, "brass bauble");
    bauble->addSynonym("bauble");
    bauble->addSynonym("trinket");
    bauble->addAdjective("brass");
    bauble->addAdjective("small");
    bauble->setFlag(ObjectFlag::TAKEBIT);
    bauble->setProperty(P_VALUE, 1);
    bauble->setProperty(P_TVALUE, 1);
    bauble->setProperty(P_SIZE, 2);
    bauble->moveTo(g.getObject(RoomIds::MAZE_5));
    g.registerObject(ObjectIds::BAUBLE, std::move(bauble));
    
    // ===== TOOL OBJECTS =====
    
    // Create SWORD (Elvish sword - weapon that glows near enemies)
    auto sword = std::make_unique<ZObject>(ObjectIds::SWORD, "sword");
    sword->addSynonym("sword");
    sword->addSynonym("blade");
    sword->addSynonym("orcrist");
    sword->addSynonym("glamdring");
    sword->addAdjective("elvish");
    sword->addAdjective("elven");
    sword->addAdjective("old");
    sword->addAdjective("antique");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->setFlag(ObjectFlag::WEAPONBIT);
    sword->setProperty(P_SIZE, 30);
    // TODO: Add action handler for glowing near enemies
    sword->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    g.registerObject(ObjectIds::SWORD, std::move(sword));
    
    // Create KNIFE (Nasty knife - weapon, less effective than sword)
    auto knife = std::make_unique<ZObject>(ObjectIds::KNIFE, "nasty knife");
    knife->addSynonym("knife");
    knife->addSynonym("knives");
    knife->addSynonym("blade");
    knife->addAdjective("nasty");
    knife->addAdjective("sharp");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->setFlag(ObjectFlag::WEAPONBIT);
    knife->setProperty(P_SIZE, 10);
    knife->moveTo(g.getObject(RoomIds::ATTIC));
    g.registerObject(ObjectIds::KNIFE, std::move(knife));
    
    // Create LAMP (Battery-powered brass lantern - light source)
    auto lamp = std::make_unique<ZObject>(ObjectIds::LAMP, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->addSynonym("light");
    lamp->addAdjective("brass");
    lamp->addAdjective("battery");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    // Lamp starts off - player must turn it on
    lamp->setProperty(P_SIZE, 15);
    // TODO: Add battery timer and action handler
    lamp->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    g.registerObject(ObjectIds::LAMP, std::move(lamp));
    
    // Create CANDLES (Pair of candles - temporary light source)
    auto candles = std::make_unique<ZObject>(ObjectIds::CANDLES, "pair of candles");
    candles->addSynonym("candles");
    candles->addSynonym("candle");
    candles->addSynonym("pair");
    candles->addAdjective("burning");
    candles->addAdjective("wax");
    candles->setFlag(ObjectFlag::TAKEBIT);
    candles->setFlag(ObjectFlag::LIGHTBIT);
    candles->setFlag(ObjectFlag::ONBIT);  // Candles start lit
    candles->setFlag(ObjectFlag::FLAMEBIT);
    candles->setProperty(P_SIZE, 10);
    // TODO: Add burn-down timer
    candles->moveTo(g.getObject(RoomIds::SOUTH_TEMPLE));
    g.registerObject(ObjectIds::CANDLES, std::move(candles));
    
    // Create MATCH (Matchbook - one-time use light source)
    auto match = std::make_unique<ZObject>(ObjectIds::MATCH, "matchbook");
    match->addSynonym("match");
    match->addSynonym("matches");
    match->addSynonym("matchbook");
    match->addAdjective("match");
    match->setFlag(ObjectFlag::TAKEBIT);
    match->setProperty(P_SIZE, 2);
    // TODO: Add action handler for lighting
    match->moveTo(g.getObject(RoomIds::DAM_LOBBY));
    g.registerObject(ObjectIds::MATCH, std::move(match));
    
    // Create ROPE (Large coil of rope - for climbing and tying)
    auto rope = std::make_unique<ZObject>(ObjectIds::ROPE, "rope");
    rope->addSynonym("rope");
    rope->addSynonym("coil");
    rope->addSynonym("hemp");
    rope->addAdjective("large");
    rope->addAdjective("thick");
    rope->setFlag(ObjectFlag::TAKEBIT);
    rope->setProperty(P_SIZE, 10);
    // TODO: Add action handler for tying and climbing
    rope->moveTo(g.getObject(RoomIds::ATTIC));
    g.registerObject(ObjectIds::ROPE, std::move(rope));
    
    // Create WRENCH (Wrench - tool for bolts)
    auto wrench = std::make_unique<ZObject>(ObjectIds::WRENCH, "wrench");
    wrench->addSynonym("wrench");
    wrench->addSynonym("tool");
    wrench->addAdjective("metal");
    wrench->setFlag(ObjectFlag::TAKEBIT);
    wrench->setFlag(ObjectFlag::TOOLBIT);
    wrench->setProperty(P_SIZE, 10);
    wrench->moveTo(g.getObject(RoomIds::MAINTENANCE_ROOM));
    g.registerObject(ObjectIds::WRENCH, std::move(wrench));
    
    // Create SCREWDRIVER (Screwdriver - tool for screws)
    auto screwdriver = std::make_unique<ZObject>(ObjectIds::SCREWDRIVER, "screwdriver");
    screwdriver->addSynonym("screwdriver");
    screwdriver->addSynonym("driver");
    screwdriver->addSynonym("tool");
    screwdriver->addAdjective("screw");
    screwdriver->setFlag(ObjectFlag::TAKEBIT);
    screwdriver->setFlag(ObjectFlag::TOOLBIT);
    screwdriver->setProperty(P_SIZE, 8);
    screwdriver->moveTo(g.getObject(RoomIds::MAINTENANCE_ROOM));
    g.registerObject(ObjectIds::SCREWDRIVER, std::move(screwdriver));
    
    // Create SHOVEL (Shovel - tool for digging)
    auto shovel = std::make_unique<ZObject>(ObjectIds::SHOVEL, "shovel");
    shovel->addSynonym("shovel");
    shovel->addSynonym("spade");
    shovel->addSynonym("tool");
    shovel->setFlag(ObjectFlag::TAKEBIT);
    shovel->setFlag(ObjectFlag::TOOLBIT);
    shovel->setProperty(P_SIZE, 15);
    shovel->moveTo(g.getObject(RoomIds::SANDY_BEACH));
    g.registerObject(ObjectIds::SHOVEL, std::move(shovel));
    
    // Create PUMP (Hand-held air pump - for inflating boat)
    auto pump = std::make_unique<ZObject>(ObjectIds::PUMP, "hand-held air pump");
    pump->addSynonym("pump");
    pump->addSynonym("air-pump");
    pump->addSynonym("tool");
    pump->addAdjective("hand-held");
    pump->addAdjective("small");
    pump->setFlag(ObjectFlag::TAKEBIT);
    pump->setFlag(ObjectFlag::TOOLBIT);
    pump->setProperty(P_SIZE, 8);
    // TODO: Add action handler for inflating/deflating boat
    pump->moveTo(g.getObject(RoomIds::RESERVOIR_NORTH));
    g.registerObject(ObjectIds::PUMP, std::move(pump));
    
    // Create BOTTLE (Glass bottle - container and tool)
    auto bottle = std::make_unique<ZObject>(ObjectIds::BOTTLE, "glass bottle");
    bottle->addSynonym("bottle");
    bottle->addSynonym("container");
    bottle->addAdjective("glass");
    bottle->addAdjective("clear");
    bottle->setFlag(ObjectFlag::TAKEBIT);
    bottle->setFlag(ObjectFlag::CONTBIT);
    bottle->setFlag(ObjectFlag::TRANSBIT);  // Transparent
    bottle->setProperty(P_SIZE, 8);
    bottle->setProperty(P_CAPACITY, 4);
    // TODO: Add action handler for filling/emptying
    bottle->moveTo(g.getObject(RoomIds::KITCHEN));
    g.registerObject(ObjectIds::BOTTLE, std::move(bottle));
    
    // ===== NPCs =====
    
    // Create THIEF NPC
    auto thief = std::make_unique<ZObject>(ObjectIds::THIEF, "thief");
    thief->addSynonym("thief");
    thief->addSynonym("robber");
    thief->addSynonym("burglar");
    thief->setFlag(ObjectFlag::FIGHTBIT);    // Hostile NPC
    thief->setFlag(ObjectFlag::ACTORBIT);    // Is an actor/NPC
    thief->setProperty(P_STRENGTH, 5);       // Combat strength
    // TODO: Add action handler for thief behavior (stealing, combat, wandering)
    // Initial location varies - thief wanders. Start in MAZE_1 for now
    thief->moveTo(g.getObject(RoomIds::MAZE_1));
    g.registerObject(ObjectIds::THIEF, std::move(thief));
    
    g.lit = true;
}
