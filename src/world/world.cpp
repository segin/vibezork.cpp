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
    // WEST and IN to KITCHEN require KITCHEN_WINDOW to be open - will be handled by conditional exits later
    
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
    
    g.lit = true;
}
