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
        print("You are standing in an open field west of a white house, with a boarded\nfront door.");
        if (wonFlag) {
            print(" A secret path leads southwest into the forest.");
        }
        crlf();
    }
}

void northHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the north side of a white house. There is no door here,\nand all the windows are boarded up. To the north a narrow path winds through\nthe trees.");
    }
}

void southHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the south side of a white house. There is no door here,\nand all the windows are boarded.");
    }
}

void stoneBarrowAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are standing in front of a massive barrow of stone. In the east face\nis a huge stone door which is open. You cannot see into the dark of the tomb.");
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

void initializeWorld() {
    auto& g = Globals::instance();
    
    // Create West of House room
    auto westOfHouse = std::make_unique<ZRoom>(
        ROOM_WEST_OF_HOUSE,
        "West of House",
        ""  // Long desc handled by action function
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
    // SW and IN to STONE_BARROW require WON-FLAG
    
    g.here = westOfHouse.get();
    g.registerObject(ROOM_WEST_OF_HOUSE, std::move(westOfHouse));
    
    // Create North of House room
    auto northOfHouse = std::make_unique<ZRoom>(
        ROOM_NORTH_OF_HOUSE,
        "North of House",
        ""
    );
    northOfHouse->setFlag(ObjectFlag::RLANDBIT);
    northOfHouse->setFlag(ObjectFlag::ONBIT);
    northOfHouse->setFlag(ObjectFlag::SACREDBIT);
    northOfHouse->setRoomAction(northHouseAction);
    
    northOfHouse->setExit(Direction::SW, RoomExit(ROOM_WEST_OF_HOUSE));
    northOfHouse->setExit(Direction::SE, RoomExit(ROOM_EAST_OF_HOUSE));
    northOfHouse->setExit(Direction::WEST, RoomExit(ROOM_WEST_OF_HOUSE));
    northOfHouse->setExit(Direction::EAST, RoomExit(ROOM_EAST_OF_HOUSE));
    northOfHouse->setExit(Direction::SOUTH, RoomExit("The windows are all boarded."));
    
    g.registerObject(ROOM_NORTH_OF_HOUSE, std::move(northOfHouse));
    
    // Create South of House room
    auto southOfHouse = std::make_unique<ZRoom>(
        ROOM_SOUTH_OF_HOUSE,
        "South of House",
        ""
    );
    southOfHouse->setFlag(ObjectFlag::RLANDBIT);
    southOfHouse->setFlag(ObjectFlag::ONBIT);
    southOfHouse->setFlag(ObjectFlag::SACREDBIT);
    southOfHouse->setRoomAction(southHouseAction);
    
    southOfHouse->setExit(Direction::WEST, RoomExit(ROOM_WEST_OF_HOUSE));
    southOfHouse->setExit(Direction::EAST, RoomExit(ROOM_EAST_OF_HOUSE));
    southOfHouse->setExit(Direction::NE, RoomExit(ROOM_EAST_OF_HOUSE));
    southOfHouse->setExit(Direction::NW, RoomExit(ROOM_WEST_OF_HOUSE));
    southOfHouse->setExit(Direction::NORTH, RoomExit("The windows are all boarded."));
    
    g.registerObject(ROOM_SOUTH_OF_HOUSE, std::move(southOfHouse));
    
    // Create Stone Barrow room
    auto stoneBarrow = std::make_unique<ZRoom>(
        ROOM_STONE_BARROW,
        "Stone Barrow",
        ""
    );
    stoneBarrow->setFlag(ObjectFlag::RLANDBIT);
    stoneBarrow->setFlag(ObjectFlag::ONBIT);
    stoneBarrow->setFlag(ObjectFlag::SACREDBIT);
    stoneBarrow->setRoomAction(stoneBarrowAction);
    
    stoneBarrow->setExit(Direction::NE, RoomExit(ROOM_WEST_OF_HOUSE));
    
    g.registerObject(ROOM_STONE_BARROW, std::move(stoneBarrow));
    
    // Create Forest-1 placeholder
    auto forest1 = std::make_unique<ZRoom>(
        ROOM_FOREST_1,
        "Forest",
        ""
    );
    forest1->setFlag(ObjectFlag::RLANDBIT);
    forest1->setFlag(ObjectFlag::ONBIT);
    forest1->setRoomAction([](int rarg) {
        if (rarg == M_LOOK) {
            printLine("This is a forest, with trees in all directions. To the east, there appears to be sunlight.");
        }
    });
    forest1->setExit(Direction::EAST, RoomExit(ROOM_WEST_OF_HOUSE));
    
    g.registerObject(ROOM_FOREST_1, std::move(forest1));
    
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
