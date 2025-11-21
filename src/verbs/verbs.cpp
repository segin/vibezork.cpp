#include "verbs.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/rooms.h"

namespace Verbs {

bool vLook() {
    auto& g = Globals::instance();
    if (g.here) {
        // Call room action with M-LOOK
        ZRoom* room = dynamic_cast<ZRoom*>(g.here);
        if (room) {
            room->performRoomAction(M_LOOK);
        } else {
            printLine(g.here->getDesc());
        }
        
        // List visible objects in room
        const auto& contents = g.here->getContents();
        for (const auto* obj : contents) {
            if (!obj->hasFlag(ObjectFlag::NDESCBIT) && !obj->hasFlag(ObjectFlag::INVISIBLE)) {
                print("There is ");
                print(obj->getDesc());
                printLine(" here.");
            }
        }
    }
    return RTRUE;
}

bool vInventory() {
    auto& g = Globals::instance();
    if (!g.winner) {
        return RFALSE;
    }
    
    const auto& contents = g.winner->getContents();
    if (contents.empty()) {
        printLine("You are empty-handed.");
    } else {
        printLine("You are carrying:");
        for (const auto* obj : contents) {
            print("  ");
            printLine(obj->getDesc());
        }
    }
    return RTRUE;
}

bool vQuit() {
    printLine("Do you really want to quit? (yes/no)");
    std::string response = readLine();
    if (response == "yes" || response == "y") {
        printLine("Thanks for playing!");
        exit(0);
    }
    return RTRUE;
}

bool vTake() {
    printLine("Taken.");
    return RTRUE;
}

bool vDrop() {
    printLine("Dropped.");
    return RTRUE;
}

bool vExamine() {
    printLine("You see nothing special.");
    return RTRUE;
}

bool vOpen() {
    printLine("Opened.");
    return RTRUE;
}

bool vClose() {
    printLine("Closed.");
    return RTRUE;
}

bool vWalk() {
    auto& g = Globals::instance();
    ZRoom* currentRoom = dynamic_cast<ZRoom*>(g.here);
    
    if (!currentRoom) {
        printLine("You can't go that way.");
        return RTRUE;
    }
    
    // Direction is stored in parser state - for now just say can't go
    // Full implementation would check parser for direction
    printLine("You can't go that way.");
    return RTRUE;
}

bool vWalkDir(Direction dir) {
    auto& g = Globals::instance();
    ZRoom* currentRoom = dynamic_cast<ZRoom*>(g.here);
    
    if (!currentRoom) {
        printLine("You can't go that way.");
        return RTRUE;
    }
    
    RoomExit* exit = currentRoom->getExit(dir);
    if (!exit) {
        printLine("You can't go that way.");
        return RTRUE;
    }
    
    // Check if it's a message (blocked exit)
    if (!exit->message.empty()) {
        printLine(exit->message);
        return RTRUE;
    }
    
    // Check condition if present
    if (exit->condition && !exit->condition()) {
        printLine("You can't go that way.");
        return RTRUE;
    }
    
    // Move to new room
    ZObject* newRoom = g.getObject(exit->targetRoom);
    if (newRoom) {
        g.here = newRoom;
        g.winner->moveTo(newRoom);
        vLook();
    }
    
    return RTRUE;
}

} // namespace Verbs
