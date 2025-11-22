#include "verbs.h"
#include "core/globals.h"
#include "core/io.h"
#include "world/rooms.h"
#include "world/objects.h"

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
    auto& g = Globals::instance();
    
    // PRE-TAKE checks (Requirement 21, 34, 63, 80)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Take what?");
        return RTRUE;
    }
    
    // Check if object is already in inventory
    if (g.prso->getLocation() == g.winner) {
        printLine("You already have that.");
        return RTRUE;
    }
    
    // Check TRYTAKEBIT flag (anchored objects cannot be taken)
    if (g.prso->hasFlag(ObjectFlag::TRYTAKEBIT)) {
        printLine("You can't take that.");
        return RTRUE;
    }
    
    // Check TAKEBIT flag (only takeable objects can be taken)
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        printLine("You can't take that.");
        return RTRUE;
    }
    
    // Check if object is accessible (must be in current room, inventory, or open container)
    ZObject* objLocation = g.prso->getLocation();
    bool accessible = false;
    
    if (objLocation == g.here) {
        // Object is in current room
        accessible = true;
    } else if (objLocation == g.winner) {
        // Object is already in inventory (handled above)
        accessible = true;
    } else if (objLocation) {
        // Check if object is in an open container in the room or inventory
        if (objLocation->hasFlag(ObjectFlag::CONTBIT) && 
            objLocation->hasFlag(ObjectFlag::OPENBIT)) {
            ZObject* containerLocation = objLocation->getLocation();
            if (containerLocation == g.here || containerLocation == g.winner) {
                accessible = true;
            }
        }
    }
    
    if (!accessible) {
        printLine("You can't see any such thing.");
        return RTRUE;
    }
    
    // Check inventory weight limit (Requirement 63)
    // Calculate current inventory weight
    int currentWeight = 0;
    for (const auto* obj : g.winner->getContents()) {
        currentWeight += obj->getProperty(P_SIZE);
    }
    
    // Get object size
    int objectSize = g.prso->getProperty(P_SIZE);
    if (objectSize == 0) {
        objectSize = 5;  // Default size if not specified
    }
    
    // Check if adding this object would exceed the limit
    if (currentWeight + objectSize > g.loadAllowed) {
        printLine("You're carrying too much.");
        return RTRUE;
    }
    
    // Call object action handler if present (Requirement 35)
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default TAKE behavior
    // Move object to player inventory
    g.prso->moveTo(g.winner);
    printLine("Taken.");
    
    return RTRUE;
}

bool vDrop() {
    auto& g = Globals::instance();
    
    // PRE-DROP checks (Requirement 21, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Drop what?");
        return RTRUE;
    }
    
    // Check if object is in inventory
    if (g.prso->getLocation() != g.winner) {
        printLine("You aren't carrying that.");
        return RTRUE;
    }
    
    // Check if drop is allowed in current room
    // Some rooms might not allow dropping (e.g., sacred rooms, special locations)
    // For now, we allow dropping everywhere unless room action prevents it
    
    // Call object action handler if present (Requirement 35)
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default DROP behavior
    // Move object to current room
    g.prso->moveTo(g.here);
    printLine("Dropped.");
    
    return RTRUE;
}

bool vExamine() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Examine what?");
        return RTRUE;
    }
    
    // Check if object has an action handler that handles EXAMINE
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Display detailed object description
    printLine(g.prso->getDesc());
    
    // Show object state
    // Check if it's a container
    if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            const auto& contents = g.prso->getContents();
            if (contents.empty()) {
                printLine("The " + g.prso->getDesc() + " is empty.");
            } else {
                printLine("The " + g.prso->getDesc() + " contains:");
                for (const auto* obj : contents) {
                    print("  ");
                    printLine(obj->getDesc());
                }
            }
        } else {
            printLine("The " + g.prso->getDesc() + " is closed.");
        }
    }
    
    // Check if it's a light source
    if (g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The " + g.prso->getDesc() + " is on.");
        } else {
            printLine("The " + g.prso->getDesc() + " is off.");
        }
    }
    
    // Check if it's locked
    if (g.prso->hasFlag(ObjectFlag::DOORBIT) || g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        if (g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
            printLine("The " + g.prso->getDesc() + " is locked.");
        }
    }
    
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
    
    // Handle different exit types
    switch (exit->type) {
        case ExitType::DOOR: {
            // Check if door exists and is accessible
            if (exit->doorObject == 0) {
                printLine("You can't go that way.");
                return RTRUE;
            }
            
            ZObject* door = g.getObject(exit->doorObject);
            if (!door) {
                printLine("You can't go that way.");
                return RTRUE;
            }
            
            // Check if door is locked
            if (door->hasFlag(ObjectFlag::LOCKEDBIT)) {
                printLine("The door is locked.");
                return RTRUE;
            }
            
            // Check if door is closed
            if (!door->hasFlag(ObjectFlag::OPENBIT)) {
                printLine("The door is closed.");
                return RTRUE;
            }
            
            // Door is open, allow passage
            break;
        }
        
        case ExitType::SPECIAL: {
            // Special exits require specific verbs (CLIMB, ENTER, etc.)
            // For now, just block with message
            if (!exit->specialMessage.empty()) {
                printLine(exit->specialMessage);
            } else {
                printLine("You can't go that way.");
            }
            return RTRUE;
        }
        
        case ExitType::CONDITIONAL: {
            // Check condition if present
            if (exit->condition && !exit->condition()) {
                if (!exit->message.empty()) {
                    printLine(exit->message);
                } else {
                    printLine("You can't go that way.");
                }
                return RTRUE;
            }
            break;
        }
        
        case ExitType::ONE_WAY:
        case ExitType::NORMAL:
        default:
            // Normal exits - just check for message
            if (!exit->message.empty()) {
                printLine(exit->message);
                return RTRUE;
            }
            break;
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

bool trySpecialMovement(int verbId, Direction dir) {
    auto& g = Globals::instance();
    ZRoom* currentRoom = dynamic_cast<ZRoom*>(g.here);
    
    if (!currentRoom) {
        return false;
    }
    
    RoomExit* exit = currentRoom->getExit(dir);
    if (!exit || exit->type != ExitType::SPECIAL) {
        return false;
    }
    
    // Check if this is the required verb for this exit
    if (exit->requiredVerb != verbId) {
        if (!exit->specialMessage.empty()) {
            printLine(exit->specialMessage);
        } else {
            printLine("You can't do that here.");
        }
        return true;
    }
    
    // Check if door is involved
    if (exit->doorObject != 0) {
        ZObject* door = g.getObject(exit->doorObject);
        if (door) {
            if (door->hasFlag(ObjectFlag::LOCKEDBIT)) {
                printLine("The door is locked.");
                return true;
            }
            if (!door->hasFlag(ObjectFlag::OPENBIT)) {
                printLine("The door is closed.");
                return true;
            }
        }
    }
    
    // Check condition if present
    if (exit->condition && !exit->condition()) {
        if (!exit->message.empty()) {
            printLine(exit->message);
        } else {
            printLine("You can't do that.");
        }
        return true;
    }
    
    // Move to new room
    ZObject* newRoom = g.getObject(exit->targetRoom);
    if (newRoom) {
        g.here = newRoom;
        g.winner->moveTo(newRoom);
        Verbs::vLook();
    }
    
    return true;
}

bool vEnter() {
    auto& g = Globals::instance();
    
    // Try ENTER as special movement in all directions
    // Priority: IN, then cardinal directions
    if (trySpecialMovement(V_ENTER, Direction::IN)) return RTRUE;
    if (trySpecialMovement(V_ENTER, Direction::NORTH)) return RTRUE;
    if (trySpecialMovement(V_ENTER, Direction::SOUTH)) return RTRUE;
    if (trySpecialMovement(V_ENTER, Direction::EAST)) return RTRUE;
    if (trySpecialMovement(V_ENTER, Direction::WEST)) return RTRUE;
    
    // If PRSO is set, try to enter that object
    if (g.prso) {
        printLine("You can't enter that.");
        return RTRUE;
    }
    
    printLine("Enter what?");
    return RTRUE;
}

bool vExit() {
    auto& g = Globals::instance();
    
    // Try EXIT as special movement, priority to OUT
    if (trySpecialMovement(V_EXIT, Direction::OUT)) return RTRUE;
    
    // Try other directions
    if (trySpecialMovement(V_EXIT, Direction::NORTH)) return RTRUE;
    if (trySpecialMovement(V_EXIT, Direction::SOUTH)) return RTRUE;
    if (trySpecialMovement(V_EXIT, Direction::EAST)) return RTRUE;
    if (trySpecialMovement(V_EXIT, Direction::WEST)) return RTRUE;
    
    printLine("You can't exit here.");
    return RTRUE;
}

bool vClimbUp() {
    auto& g = Globals::instance();
    
    // Try CLIMB as special movement UP
    if (trySpecialMovement(V_CLIMB_UP, Direction::UP)) return RTRUE;
    
    // If PRSO is set, try to climb that object
    if (g.prso) {
        printLine("You can't climb that.");
        return RTRUE;
    }
    
    // Default to trying UP direction
    return Verbs::vWalkDir(Direction::UP);
}

bool vClimbDown() {
    auto& g = Globals::instance();
    
    // Try CLIMB as special movement DOWN
    if (trySpecialMovement(V_CLIMB_DOWN, Direction::DOWN)) return RTRUE;
    
    // If PRSO is set, try to climb down that object
    if (g.prso) {
        printLine("You can't climb down that.");
        return RTRUE;
    }
    
    // Default to trying DOWN direction
    return Verbs::vWalkDir(Direction::DOWN);
}

bool vBoard() {
    auto& g = Globals::instance();
    
    // If PRSO is set, try to board that object
    if (g.prso) {
        // Check if object is a vehicle
        if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
            // Move player into the vehicle
            g.winner->moveTo(g.prso);
            printLine("You board the " + g.prso->getDesc() + ".");
            return RTRUE;
        }
        printLine("You can't board that.");
        return RTRUE;
    }
    
    printLine("Board what?");
    return RTRUE;
}

bool vDisembark() {
    auto& g = Globals::instance();
    
    // Check if player is in a vehicle
    ZObject* location = g.winner->getLocation();
    if (location && location->hasFlag(ObjectFlag::VEHBIT)) {
        // Move player to vehicle's location
        ZObject* vehicleLocation = location->getLocation();
        if (vehicleLocation) {
            g.winner->moveTo(vehicleLocation);
            printLine("You disembark.");
            return RTRUE;
        }
    }
    
    printLine("You're not in anything.");
    return RTRUE;
}

bool vRead() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Read what?");
        return RTRUE;
    }
    
    // Check if object has READBIT flag
    if (!g.prso->hasFlag(ObjectFlag::READBIT)) {
        printLine("How does one read a " + g.prso->getDesc() + "?");
        return RTRUE;
    }
    
    // Check if object has text
    if (!g.prso->hasText()) {
        printLine("There is nothing written on the " + g.prso->getDesc() + ".");
        return RTRUE;
    }
    
    // Display the text
    printLine(g.prso->getText());
    return RTRUE;
}

bool vLookInside() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Look inside what?");
        return RTRUE;
    }
    
    // Check if object is a container
    if (!g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        printLine("The " + g.prso->getDesc() + " isn't a container.");
        return RTRUE;
    }
    
    // Check if container is open or transparent
    if (!g.prso->hasFlag(ObjectFlag::OPENBIT) && !g.prso->hasFlag(ObjectFlag::TRANSBIT)) {
        printLine("The " + g.prso->getDesc() + " is closed.");
        return RTRUE;
    }
    
    // List contents of container
    const auto& contents = g.prso->getContents();
    if (contents.empty()) {
        printLine("The " + g.prso->getDesc() + " is empty.");
    } else {
        printLine("The " + g.prso->getDesc() + " contains:");
        for (const auto* obj : contents) {
            print("  ");
            printLine(obj->getDesc());
        }
    }
    
    return RTRUE;
}

bool vSearch() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Search what?");
        return RTRUE;
    }
    
    // Check if object has an action handler that handles SEARCH
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Similar to LOOK-INSIDE but more thorough
    // Check if object is a container
    if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        // SEARCH can reveal contents even if closed (but not locked)
        if (g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
            printLine("The " + g.prso->getDesc() + " is locked.");
            return RTRUE;
        }
        
        const auto& contents = g.prso->getContents();
        if (contents.empty()) {
            printLine("You find nothing of interest.");
        } else {
            printLine("You find:");
            for (const auto* obj : contents) {
                print("  ");
                printLine(obj->getDesc());
            }
        }
    } else {
        // For non-containers, just give a generic message
        printLine("You find nothing unusual.");
    }
    
    return RTRUE;
}

} // namespace Verbs

