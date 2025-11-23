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
    auto& g = Globals::instance();
    
    // PRE-OPEN checks (Requirement 23, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Open what?");
        return RTRUE;
    }
    
    // Verify object has CONTBIT flag (is a container)
    if (!g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        printLine("You can't open that.");
        return RTRUE;
    }
    
    // Check if already open
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("It's already open.");
        return RTRUE;
    }
    
    // Check if locked
    if (g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
        printLine("The " + g.prso->getDesc() + " is locked.");
        return RTRUE;
    }
    
    // Call object action handler first (Requirement 23)
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default OPEN behavior
    // Set OPENBIT flag
    g.prso->setFlag(ObjectFlag::OPENBIT);
    
    // Display contents if any
    const auto& contents = g.prso->getContents();
    if (!contents.empty()) {
        printLine("Opening the " + g.prso->getDesc() + " reveals:");
        for (const auto* obj : contents) {
            print("  ");
            printLine(obj->getDesc());
        }
    } else {
        printLine("Opened.");
    }
    
    return RTRUE;
}

bool vClose() {
    auto& g = Globals::instance();
    
    // PRE-CLOSE checks (Requirement 23, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Close what?");
        return RTRUE;
    }
    
    // Verify object has CONTBIT flag (is a container)
    if (!g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        printLine("You can't close that.");
        return RTRUE;
    }
    
    // Check if already closed
    if (!g.prso->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("It's already closed.");
        return RTRUE;
    }
    
    // Call object action handler first (Requirement 23)
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default CLOSE behavior
    // Clear OPENBIT flag
    g.prso->clearFlag(ObjectFlag::OPENBIT);
    printLine("Closed.");
    
    return RTRUE;
}

bool vLock() {
    auto& g = Globals::instance();
    
    // PRE-LOCK checks (Requirement 24, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Lock what?");
        return RTRUE;
    }
    
    // Check if key is specified
    if (!g.prsi) {
        printLine("Lock it with what?");
        return RTRUE;
    }
    
    // Verify object can be locked (has DOORBIT or CONTBIT flag)
    if (!g.prso->hasFlag(ObjectFlag::DOORBIT) && !g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        printLine("You can't lock that.");
        return RTRUE;
    }
    
    // Check if already locked
    if (g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
        printLine("It's already locked.");
        return RTRUE;
    }
    
    // Verify player has the key (must be in inventory or accessible)
    ZObject* keyLocation = g.prsi->getLocation();
    bool hasKey = false;
    
    if (keyLocation == g.winner) {
        // Key is in inventory
        hasKey = true;
    } else if (keyLocation == g.here) {
        // Key is in current room
        hasKey = true;
    } else if (keyLocation) {
        // Check if key is in an open container in the room or inventory
        if (keyLocation->hasFlag(ObjectFlag::CONTBIT) && 
            keyLocation->hasFlag(ObjectFlag::OPENBIT)) {
            ZObject* containerLocation = keyLocation->getLocation();
            if (containerLocation == g.here || containerLocation == g.winner) {
                hasKey = true;
            }
        }
    }
    
    if (!hasKey) {
        printLine("You don't have that.");
        return RTRUE;
    }
    
    // Verify the key has TOOLBIT flag (is a tool/key)
    if (!g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
        printLine("You can't lock anything with that.");
        return RTRUE;
    }
    
    // Call object action handler first (Requirement 24)
    // This allows objects to override default behavior (e.g., check for correct key)
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default LOCK behavior
    // Set LOCKEDBIT flag
    g.prso->setFlag(ObjectFlag::LOCKEDBIT);
    printLine("Locked.");
    
    return RTRUE;
}

bool vUnlock() {
    auto& g = Globals::instance();
    
    // PRE-UNLOCK checks (Requirement 24, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Unlock what?");
        return RTRUE;
    }
    
    // Check if key is specified
    if (!g.prsi) {
        printLine("Unlock it with what?");
        return RTRUE;
    }
    
    // Verify object is locked
    if (!g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
        printLine("It's not locked.");
        return RTRUE;
    }
    
    // Verify player has the key (must be in inventory or accessible)
    ZObject* keyLocation = g.prsi->getLocation();
    bool hasKey = false;
    
    if (keyLocation == g.winner) {
        // Key is in inventory
        hasKey = true;
    } else if (keyLocation == g.here) {
        // Key is in current room
        hasKey = true;
    } else if (keyLocation) {
        // Check if key is in an open container in the room or inventory
        if (keyLocation->hasFlag(ObjectFlag::CONTBIT) && 
            keyLocation->hasFlag(ObjectFlag::OPENBIT)) {
            ZObject* containerLocation = keyLocation->getLocation();
            if (containerLocation == g.here || containerLocation == g.winner) {
                hasKey = true;
            }
        }
    }
    
    if (!hasKey) {
        printLine("You don't have that.");
        return RTRUE;
    }
    
    // Verify the key has TOOLBIT flag (is a tool/key)
    if (!g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
        printLine("You can't unlock anything with that.");
        return RTRUE;
    }
    
    // Call object action handler first (Requirement 24)
    // This allows objects to override default behavior (e.g., check for correct key)
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default UNLOCK behavior
    // Clear LOCKEDBIT flag
    g.prso->clearFlag(ObjectFlag::LOCKEDBIT);
    printLine("Unlocked.");
    
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

bool vPut() {
    auto& g = Globals::instance();
    
    // PRE-PUT checks (Requirement 22, 34, 64)
    
    // Check if direct object is specified
    if (!g.prso) {
        printLine("Put what?");
        return RTRUE;
    }
    
    // Check if indirect object is specified
    if (!g.prsi) {
        printLine("Put it in what?");
        return RTRUE;
    }
    
    // Verify direct object is takeable (has TAKEBIT flag)
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        printLine("You can't take that.");
        return RTRUE;
    }
    
    // Check if direct object is accessible
    ZObject* objLocation = g.prso->getLocation();
    bool accessible = false;
    
    if (objLocation == g.here || objLocation == g.winner) {
        accessible = true;
    } else if (objLocation) {
        // Check if object is in an open container
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
    
    // Verify indirect object has CONTBIT flag (is a container)
    if (!g.prsi->hasFlag(ObjectFlag::CONTBIT)) {
        printLine("You can't put something in that.");
        return RTRUE;
    }
    
    // Check if container is open
    if (!g.prsi->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("The " + g.prsi->getDesc() + " is closed.");
        return RTRUE;
    }
    
    // Check container capacity (Requirement 64)
    int capacity = g.prsi->getProperty(P_CAPACITY);
    if (capacity == 0) {
        capacity = 100;  // Default capacity if not specified
    }
    
    // Calculate current contents size
    int currentSize = 0;
    for (const auto* obj : g.prsi->getContents()) {
        currentSize += obj->getProperty(P_SIZE);
    }
    
    // Get object size
    int objectSize = g.prso->getProperty(P_SIZE);
    if (objectSize == 0) {
        objectSize = 5;  // Default size if not specified
    }
    
    // Check if adding this object would exceed capacity
    if (currentSize + objectSize > capacity) {
        printLine("There's no room in the " + g.prsi->getDesc() + ".");
        return RTRUE;
    }
    
    // Call object action handlers (Requirement 22)
    // First check if direct object has special PUT behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Then check if indirect object (container) has special PUT behavior
    if (g.prsi->performAction()) {
        return RTRUE;
    }
    
    // Default PUT behavior
    // Move object to container
    g.prso->moveTo(g.prsi);
    printLine("Done.");
    
    return RTRUE;
}

// Manipulation Verbs (Requirement 26)

bool vTurn() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Turn what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Nothing obvious happens.");
    return RTRUE;
}

bool vPush() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Push what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Nothing obvious happens.");
    return RTRUE;
}

bool vPull() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Pull what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Nothing obvious happens.");
    return RTRUE;
}

bool vMove() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Move what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Nothing obvious happens.");
    return RTRUE;
}

// Interaction Verbs (Requirement 27)

bool vTie() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Tie what?");
        return RTRUE;
    }
    
    // Check if indirect object is specified (tie X to Y)
    if (!g.prsi) {
        printLine("Tie it to what?");
        return RTRUE;
    }
    
    // Call object action handlers
    // First check if direct object has special TIE behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Then check if indirect object has special TIE behavior
    if (g.prsi->performAction()) {
        return RTRUE;
    }
    
    // Default: Can't tie that
    printLine("You can't tie that.");
    return RTRUE;
}

bool vUntie() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Untie what?");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: It's not tied
    printLine("It's not tied.");
    return RTRUE;
}

bool vListen() {
    auto& g = Globals::instance();
    
    // If object is specified, listen to that object
    if (g.prso) {
        // Call object action handler
        if (g.prso->performAction()) {
            return RTRUE;
        }
        
        // Default for object
        printLine("You hear nothing unusual.");
        return RTRUE;
    }
    
    // No object specified - listen to the room
    // Room-specific listening behavior would be handled by room actions
    // For now, just give default message
    printLine("You hear nothing unusual.");
    return RTRUE;
}

bool vSmell() {
    auto& g = Globals::instance();
    
    // If object is specified, smell that object
    if (g.prso) {
        // Call object action handler
        if (g.prso->performAction()) {
            return RTRUE;
        }
        
        // Default for object
        printLine("You smell nothing unusual.");
        return RTRUE;
    }
    
    // No object specified - smell the room
    printLine("You smell nothing unusual.");
    return RTRUE;
}

bool vTouch() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Touch what?");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: You feel nothing unusual
    printLine("You feel nothing unusual.");
    return RTRUE;
}

// Consumption Verbs (Requirement 28)

bool vEat() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Eat what?");
        return RTRUE;
    }
    
    // Check if object has FOODBIT flag
    if (!g.prso->hasFlag(ObjectFlag::FOODBIT)) {
        printLine("That's not edible.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default EAT behavior
    // Remove object from game (consumed)
    g.prso->moveTo(nullptr);
    
    printLine("Thank you very much. It really hit the spot.");
    
    return RTRUE;
}

bool vDrink() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Drink what?");
        return RTRUE;
    }
    
    // Check if object has DRINKBIT flag
    if (!g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
        printLine("That's not drinkable.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default DRINK behavior
    // Remove object from game (consumed)
    g.prso->moveTo(nullptr);
    
    printLine("Thank you very much. It really hit the spot.");
    
    return RTRUE;
}

// Light Source Verbs (Requirement 30)

bool vLampOn() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Turn on what?");
        return RTRUE;
    }
    
    // Check if object has LIGHTBIT flag (is a light source)
    if (!g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
        printLine("You can't turn that on.");
        return RTRUE;
    }
    
    // Check if already on
    if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
        printLine("It's already on.");
        return RTRUE;
    }
    
    // Check if lamp has battery/fuel
    // For the lamp, check if it has been depleted
    if (g.prso->getProperty(P_CAPACITY) == 0) {
        printLine("The lamp has no more power.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default LAMP-ON behavior
    // Set ONBIT flag
    g.prso->setFlag(ObjectFlag::ONBIT);
    
    // Update room lighting
    // This will be handled by the light system when implemented
    // For now, just set the flag
    
    printLine("The " + g.prso->getDesc() + " is now on.");
    
    return RTRUE;
}

bool vLampOff() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Turn off what?");
        return RTRUE;
    }
    
    // Check if object has LIGHTBIT flag (is a light source)
    if (!g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
        printLine("You can't turn that off.");
        return RTRUE;
    }
    
    // Check if lamp is on
    if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
        printLine("It's already off.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default LAMP-OFF behavior
    // Clear ONBIT flag
    g.prso->clearFlag(ObjectFlag::ONBIT);
    
    // Update room lighting
    // This will be handled by the light system when implemented
    // For now, just clear the flag
    
    printLine("The " + g.prso->getDesc() + " is now off.");
    
    return RTRUE;
}

// Special Action Verbs (Requirement 31)

bool vInflate() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Inflate what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior (e.g., boat with pump)
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Can't inflate that
    printLine("You can't inflate that.");
    return RTRUE;
}

bool vDeflate() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Deflate what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior (e.g., boat with pump)
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Can't deflate that
    printLine("You can't deflate that.");
    return RTRUE;
}

bool vPray() {
    auto& g = Globals::instance();
    
    // PRAY is typically a room-specific action
    // Call room action handler if present
    ZRoom* room = dynamic_cast<ZRoom*>(g.here);
    if (room) {
        room->performRoomAction(M_PRAY);
    }
    
    // Default: Prayers are not answered
    printLine("Your prayers are not answered.");
    return RTRUE;
}

bool vExorcise() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Exorcise what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Nothing happens.");
    return RTRUE;
}

bool vWave() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Wave what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("You wave the " + g.prso->getDesc() + " around.");
    return RTRUE;
}

bool vRub() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Rub what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Nothing happens
    printLine("Rubbing the " + g.prso->getDesc() + " has no effect.");
    return RTRUE;
}

bool vRing() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Ring what?");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to override default behavior (e.g., bell)
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default: Can't ring that
    printLine("You can't ring that.");
    return RTRUE;
}

// Combat Verbs (Requirement 29)

bool vAttack() {
    auto& g = Globals::instance();
    
    // Check if target is specified
    if (!g.prso) {
        printLine("Attack what?");
        return RTRUE;
    }
    
    // Check if target is attackable (has FIGHTBIT flag)
    if (!g.prso->hasFlag(ObjectFlag::FIGHTBIT)) {
        printLine("You can't attack that.");
        return RTRUE;
    }
    
    // Check if we're in a sacred room (no fighting allowed)
    if (g.here && g.here->hasFlag(ObjectFlag::SACREDBIT)) {
        printLine("A mysterious force prevents you from attacking here.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows NPCs to have custom combat behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default ATTACK behavior
    // Check for weapon
    ZObject* weapon = nullptr;
    if (g.prsi) {
        // Weapon specified with "ATTACK X WITH Y"
        weapon = g.prsi;
    } else {
        // Look for weapon in inventory
        for (auto* obj : g.winner->getContents()) {
            if (obj->hasFlag(ObjectFlag::WEAPONBIT)) {
                weapon = obj;
                break;
            }
        }
    }
    
    if (!weapon) {
        printLine("You have no weapon.");
        return RTRUE;
    }
    
    // Simple combat: calculate damage based on weapon strength
    int weaponStrength = weapon->getProperty(P_STRENGTH);
    if (weaponStrength == 0) {
        weaponStrength = 1;  // Default weapon strength
    }
    
    int enemyStrength = g.prso->getProperty(P_STRENGTH);
    if (enemyStrength == 0) {
        enemyStrength = 5;  // Default enemy strength
    }
    
    // Simple combat resolution
    if (weaponStrength >= enemyStrength) {
        printLine("You strike the " + g.prso->getDesc() + " with the " + weapon->getDesc() + "!");
        printLine("The " + g.prso->getDesc() + " is defeated!");
        
        // Mark enemy as dead
        g.prso->setFlag(ObjectFlag::DEADBIT);
        
        // Remove FIGHTBIT so it can't be attacked again
        g.prso->clearFlag(ObjectFlag::FIGHTBIT);
    } else {
        printLine("You swing at the " + g.prso->getDesc() + " but miss!");
        printLine("The " + g.prso->getDesc() + " counterattacks!");
        
        // Enemy counterattack - for now just a message
        // Full combat system would track health and damage
        printLine("You are wounded!");
    }
    
    return RTRUE;
}

bool vKill() {
    // KILL is a synonym for ATTACK
    return vAttack();
}

bool vThrow() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Throw what?");
        return RTRUE;
    }
    
    // Check if target is specified
    if (!g.prsi) {
        printLine("Throw it at what?");
        return RTRUE;
    }
    
    // Check if object is in inventory or accessible
    ZObject* objLocation = g.prso->getLocation();
    if (objLocation != g.winner && objLocation != g.here) {
        printLine("You don't have that.");
        return RTRUE;
    }
    
    // Call object action handler first
    // This allows objects to have custom throw behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default THROW behavior
    // Calculate throw damage based on object size
    int objectSize = g.prso->getProperty(P_SIZE);
    if (objectSize == 0) {
        objectSize = 5;  // Default size
    }
    
    // Check if target is an NPC
    if (g.prsi->hasFlag(ObjectFlag::FIGHTBIT)) {
        int enemyStrength = g.prsi->getProperty(P_STRENGTH);
        if (enemyStrength == 0) {
            enemyStrength = 5;
        }
        
        if (objectSize >= enemyStrength / 2) {
            printLine("You throw the " + g.prso->getDesc() + " at the " + g.prsi->getDesc() + "!");
            printLine("It hits! The " + g.prsi->getDesc() + " is stunned!");
        } else {
            printLine("You throw the " + g.prso->getDesc() + " at the " + g.prsi->getDesc() + ".");
            printLine("It bounces off harmlessly.");
        }
    } else {
        printLine("You throw the " + g.prso->getDesc() + " at the " + g.prsi->getDesc() + ".");
        printLine("Nothing interesting happens.");
    }
    
    // Move object to target's location
    g.prso->moveTo(g.prsi->getLocation());
    
    return RTRUE;
}

bool vSwing() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("Swing what?");
        return RTRUE;
    }
    
    // Check if object is a weapon
    if (!g.prso->hasFlag(ObjectFlag::WEAPONBIT)) {
        printLine("That's not a weapon.");
        return RTRUE;
    }
    
    // Check if target is specified
    if (!g.prsi) {
        // No target - just swing the weapon
        printLine("You swing the " + g.prso->getDesc() + " around.");
        return RTRUE;
    }
    
    // Target specified - similar to ATTACK
    // Temporarily set prso to target and prsi to weapon
    ZObject* weapon = g.prso;
    ZObject* target = g.prsi;
    g.prso = target;
    g.prsi = weapon;
    
    bool result = vAttack();
    
    // Restore original values
    g.prso = weapon;
    g.prsi = target;
    
    return result;
}

// Meta-Game Verbs (Requirement 32, 65, 66, 67, 68)

bool vScore() {
    auto& g = Globals::instance();
    
    // Display current score
    print("Your score is ");
    print(std::to_string(g.score));
    print(" (total of 350 points), in ");
    print(std::to_string(g.moves));
    printLine(" moves.");
    
    // Display rank based on score
    std::string rank;
    if (g.score >= 350) {
        rank = "Master Adventurer";
    } else if (g.score >= 300) {
        rank = "Wizard";
    } else if (g.score >= 200) {
        rank = "Master";
    } else if (g.score >= 100) {
        rank = "Adventurer";
    } else if (g.score >= 50) {
        rank = "Junior Adventurer";
    } else if (g.score >= 25) {
        rank = "Novice Adventurer";
    } else {
        rank = "Beginner";
    }
    
    printLine("This gives you the rank of " + rank + ".");
    
    return RTRUE;
}

bool vDiagnose() {
    auto& g = Globals::instance();
    
    // Display player health status
    // For now, just display a simple message
    // Full implementation would track injuries and health
    
    printLine("You are in perfect health.");
    
    // If we had a health system, we would check for injuries here
    // and display them
    
    return RTRUE;
}

bool vVerbose() {
    auto& g = Globals::instance();
    
    // Set verbose mode flag
    g.verboseMode = true;
    g.briefMode = false;
    g.superbriefMode = false;
    
    printLine("Maximum verbosity.");
    
    return RTRUE;
}

bool vBrief() {
    auto& g = Globals::instance();
    
    // Set brief mode flag
    g.verboseMode = false;
    g.briefMode = true;
    g.superbriefMode = false;
    
    printLine("Brief descriptions.");
    
    return RTRUE;
}

bool vSuperbrief() {
    auto& g = Globals::instance();
    
    // Set superbrief mode flag
    g.verboseMode = false;
    g.briefMode = false;
    g.superbriefMode = true;
    
    printLine("Superbrief descriptions.");
    
    return RTRUE;
}

} // namespace Verbs
