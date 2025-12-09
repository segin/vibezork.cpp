#include "verbs.h"
#include "core/globals.h"
#include "core/io.h"
#include "parser/parser.h"
#include "world/rooms.h"
#include "world/objects.h"
#include "world/world.h"
#include "../systems/npc.h"
#include "../systems/combat.h"
#include "../systems/death.h"
#include "systems/lamp.h"
#include <fstream>
#include <sstream>

// External state from actions.cpp
extern bool damGatesOpen;

namespace Verbs {

// Helper function to calculate total weight (size) of an object and all its contents recursively
// This matches the WEIGHT function from ZIL (Requirement 64.2)
static int calculateWeight(const ZObject* obj) {
    if (!obj) return 0;
    
    int weight = obj->getProperty(P_SIZE);
    
    // Add weight of all contents recursively
    for (const auto* content : obj->getContents()) {
        weight += calculateWeight(content);
    }
    
    return weight;
}

// Helper function to check if an object is accessible to the player
// An object is accessible if it's in the current room, player inventory,
// or in an open container in either location
static bool isObjectAccessible(const ZObject* obj) {
    if (!obj) return false;
    
    auto& g = Globals::instance();
    ZObject* location = obj->getLocation();
    
    // In inventory
    if (location == g.winner) return true;
    
    // In current room
    if (location == g.here) return true;
    
    // In an open container in room or inventory
    if (location && location->hasFlag(ObjectFlag::CONTBIT) && 
        location->hasFlag(ObjectFlag::OPENBIT)) {
        ZObject* containerLocation = location->getLocation();
        if (containerLocation == g.here || containerLocation == g.winner) {
            return true;
        }
    }
    
    return false;
}

bool vLook() {
    auto& g = Globals::instance();
    if (!g.here) {
        return RTRUE;
    }
    
    ZRoom* room = dynamic_cast<ZRoom*>(g.here);
    
    // Superbrief mode: only room name (Requirement 67)
    if (g.superbriefMode) {
        printLine(g.here->getDesc());
        return RTRUE;
    }
    
    // Determine if we should show full description
    bool showFullDesc = false;
    
    if (g.verboseMode) {
        // Verbose mode: always show full description (Requirement 65)
        showFullDesc = true;
    } else if (g.briefMode) {
        // Brief mode: full description for unvisited rooms, short for visited (Requirement 66)
        showFullDesc = !g.here->hasFlag(ObjectFlag::TOUCHBIT);
    }
    
    // Mark room as visited
    g.here->setFlag(ObjectFlag::TOUCHBIT);
    
    // Always print room name first (Requirement 71 - text output fidelity)
    print(g.here->getDesc());
    crlf();
    
    // Display room description
    if (showFullDesc) {
        // Show full description
        if (room) {
            if (room->hasRoomAction()) {
                // Room has custom action handler - let it handle display
                room->performRoomAction(M_LOOK);
            } else {
                // No custom action - display long description
                if (!room->getLongDesc().empty()) {
                    printLine(room->getLongDesc());
                }
            }
        }
    }
    
    // List visible objects in room (not in superbrief mode)
    const auto& contents = g.here->getContents();
    for (const auto* obj : contents) {
        if (!obj->hasFlag(ObjectFlag::NDESCBIT) && !obj->hasFlag(ObjectFlag::INVISIBLE)) {
            if (obj->hasLongDesc()) {
                // Use custom long description
                printLine(obj->getLongDesc());
            } else {
                // Default description with proper article
                print("There is a ");
                print(obj->getDesc());
                printLine(" here.");
            }
            
            // If object is an open container, show contents
            if (obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
                const auto& objContents = obj->getContents();
                if (!objContents.empty()) {
                    print("The ");
                    print(obj->getDesc());
                    printLine(" contains:");
                    for (const auto* item : objContents) {
                        print("  ");
                        printLine(item->getDesc());
                    }
                }
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
    // Authentic Zork: V-QUIT from gverbs.zil
    // First show score
    vScore();
    
    // Ask for confirmation - authentic message
    print("Do you wish to leave the game? (Y is affirmative): ");
    std::string response = readLine();
    
    // Convert to lowercase for comparison
    for (char& c : response) {
        c = std::tolower(c);
    }
    
    // Accept Y/YES
    if (response == "yes" || response == "y") {
        exit(0);
    }
    
    // Player declines - authentic response
    printLine("Ok.");
    return RTRUE;
}

bool vTake() {
    auto& g = Globals::instance();
    
    // PRE-TAKE checks (Requirement 21, 34, 63, 80)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to take?");
        getGlobalParser().setOrphanDirect(V_TAKE, "take");
        return RTRUE;
    }
    
    // Check if object is already in inventory - authentic ZIL: PRE-TAKE
    if (g.prso->getLocation() == g.winner) {
        printLine("You already have that!");
        return RTRUE;
    }

    // Check if taking FROM a specific object (syntax: TAKE OBJECT FROM OBJECT)
    if (g.prsi) {
        // Verify object is actually in the source
        if (g.prso->getLocation() != g.prsi) {
            print("The ");
            print(g.prso->getDesc());
            print(" isn't in the ");
            print(g.prsi->getDesc());
            printLine(".");
            return RTRUE;
        }
        
        // Verify source is accessible (open container or surface)
        // If it's a closed opaque container, we shouldn't be able to take from it
        // Note: TRANSBIT (transparent) allows looking inside, but NOT taking out if closed!
        if (!g.prsi->hasFlag(ObjectFlag::OPENBIT) && 
            !g.prsi->hasFlag(ObjectFlag::SURFACEBIT)) {
             print("The ");
             print(g.prsi->getDesc());
             printLine(" isn't open.");
             return RTRUE;
        }
    }
    
    // Call object action handler FIRST for TRYTAKEBIT objects
    // This allows objects like mailbox to give custom "anchored" messages
    if (g.prso->hasFlag(ObjectFlag::TRYTAKEBIT)) {
        if (g.prso->performAction()) {
            return RTRUE;
        }
        // Default message if no custom handler
        printLine("You can't take that.");
        return RTRUE;
    }
    
    // Check TAKEBIT flag (only takeable objects can be taken)
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        printLine("You can't take that.");
        return RTRUE;
    }
    
    // Check if object is accessible (must be in current room, inventory, or open container)
    if (!isObjectAccessible(g.prso)) {
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
        printLine("What do you want to drop?");
        getGlobalParser().setOrphanDirect(V_DROP, "drop");
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
        printLine("What do you want to examine?");
        getGlobalParser().setOrphanDirect(V_EXAMINE, "examine");
        return RTRUE;
    }
    
    // Check if object has an action handler that handles EXAMINE
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-EXAMINE logic:
    // 1. If object has P?TEXT, print it
    // 2. Else if container or door, do V-LOOK-INSIDE
    // 3. Else "There's nothing special about the [object]."
    
    if (g.prso->hasText()) {
        printLine(g.prso->getText());
    } else if (g.prso->hasFlag(ObjectFlag::CONTBIT) || g.prso->hasFlag(ObjectFlag::DOORBIT)) {
        // Delegate to look-inside behavior
        return vLookInside();
    } else {
        print("There's nothing special about the ");
        print(g.prso->getDesc());
        printLine(".");
    }
    
    return RTRUE;
}

bool vOpen() {
    auto& g = Globals::instance();
    
    // PRE-OPEN checks (Requirement 23, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to open?");
        getGlobalParser().setOrphanDirect(V_OPEN, "open");
        return RTRUE;
    }
    
    // Call object action handler first (Requirement 23)
    // This allows objects like doors/windows to override default behavior
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Verify object has CONTBIT or DOORBIT flag (is openable)
    if (!g.prso->hasFlag(ObjectFlag::CONTBIT) && !g.prso->hasFlag(ObjectFlag::DOORBIT)) {
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
    
    // Default OPEN behavior - authentic ZIL V-OPEN
    g.prso->setFlag(ObjectFlag::OPENBIT);
    g.prso->setFlag(ObjectFlag::TOUCHBIT);
    
    // Display contents if any - authentic formatting
    const auto& contents = g.prso->getContents();
    if (contents.empty() || g.prso->hasFlag(ObjectFlag::TRANSBIT)) {
        // Empty or transparent container
        printLine("Opened.");
    } else {
        // Has contents - "Opening the X reveals Y."
        print("Opening the ");
        print(g.prso->getDesc());
        print(" reveals ");
        // Print contents inline
        bool first = true;
        for (const auto* obj : contents) {
            if (!first) print(", ");
            print(obj->getDesc());
            first = false;
        }
        printLine(".");
    }
    
    return RTRUE;
}

bool vClose() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to close?");
        getGlobalParser().setOrphanDirect(V_CLOSE, "close");
        return RTRUE;
    }
    
    // Authentic ZIL V-CLOSE logic
    // Must have CONTBIT or DOORBIT, and not be a surface with 0 capacity
    if (!g.prso->hasFlag(ObjectFlag::CONTBIT) && !g.prso->hasFlag(ObjectFlag::DOORBIT)) {
        print("You must tell me how to do that to a ");
        print(g.prso->getDesc());
        printLine(".");
        return RTRUE;
    }
    
    // Check if already closed
    if (!g.prso->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("It is already closed.");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default CLOSE behavior
    g.prso->clearFlag(ObjectFlag::OPENBIT);
    
    // Different message for doors vs containers
    if (g.prso->hasFlag(ObjectFlag::DOORBIT)) {
        print("The ");
        print(g.prso->getDesc());
        printLine(" is now closed.");
    } else {
        printLine("Closed.");
    }
    
    // Check if closing this made the room dark
    // (authentic ZIL checks LIT? after closing)
    
    return RTRUE;
}

bool vLock() {
    auto& g = Globals::instance();
    
    // PRE-LOCK checks (Requirement 24, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to lock?");
        getGlobalParser().setOrphanDirect(V_LOCK, "lock");
        return RTRUE;
    }
    
    // Check if key is specified
    if (!g.prsi) {
        printLine("What do you want to lock it with?");
        getGlobalParser().setOrphanIndirect(V_LOCK, g.prso, "with");
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
    if (!isObjectAccessible(g.prsi)) {
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
    
    // Default LOCK behavior - lock the object if all prechecks passed
    g.prso->setFlag(ObjectFlag::LOCKEDBIT);
    printLine("Locked.");
    
    return RTRUE;
}

bool vUnlock() {
    auto& g = Globals::instance();
    
    // PRE-UNLOCK checks (Requirement 24, 34)
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to unlock?");
        getGlobalParser().setOrphanDirect(V_UNLOCK, "unlock");
        return RTRUE;
    }
    
    // Check if key is specified
    if (!g.prsi) {
        printLine("What do you want to unlock it with?");
        getGlobalParser().setOrphanIndirect(V_UNLOCK, g.prso, "with");
        return RTRUE;
    }
    
    // Verify object is locked
    if (!g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
        printLine("It's not locked.");
        return RTRUE;
    }
    
    // Verify player has the key (must be in inventory or accessible)
    if (!isObjectAccessible(g.prsi)) {
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
    
    // Default UNLOCK behavior - unlock the object if all prechecks passed
    g.prso->clearFlag(ObjectFlag::LOCKEDBIT);
    printLine("Unlocked.");
    
    return RTRUE;
}

bool vWalk() {
    // Bare "go" without direction - authentic Zork response
    printLine("Use compass directions for movement.");
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
    
    // Check for dam-related water rooms (Requirement 70.2: Dam puzzle)
    // Block entry to reservoir and stream when dam gates are closed
    if (!damGatesOpen) {
        ObjectId targetId = exit->targetRoom;
        if (targetId == RoomIds::RESERVOIR || 
            targetId == RoomIds::IN_STREAM) {
            printLine("The water level is too high to enter. The reservoir is full.");
            return RTRUE;
        }
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

    }
    
    return true;
}

bool vEnter() {
    auto& g = Globals::instance();
    
    // Try ENTER as special movement first
    if (trySpecialMovement(V_ENTER, Direction::IN)) return RTRUE;
    
    // Try regular IN direction (e.g., entering through window)
    ZRoom* currentRoom = dynamic_cast<ZRoom*>(g.here);
    if (currentRoom) {
        RoomExit* inExit = currentRoom->getExit(Direction::IN);
        if (inExit && inExit->type == ExitType::NORMAL) {
            return vWalkDir(Direction::IN);
        }
    }
    
    // If PRSO is set, try to enter that object
    if (g.prso) {
        // Check if it's a vehicle
        if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
            g.winner->moveTo(g.prso);
            printLine("You are now in the " + g.prso->getDesc() + ".");
            return RTRUE;
        }
        printLine("You can't enter that.");
        return RTRUE;
    }
    
    printLine("You can't go that way.");
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
    
    printLine("What do you want to board?");
    getGlobalParser().setOrphanDirect(V_BOARD, "board");
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
        // Look for readable objects in scope
        std::vector<ZObject*> readableObjects;
        
        // Check current room
        for (const auto* obj : g.here->getContents()) {
            if (obj->hasFlag(ObjectFlag::READBIT) && !obj->hasFlag(ObjectFlag::INVISIBLE)) {
                readableObjects.push_back(const_cast<ZObject*>(obj));
            }
        }
        
        // Check inventory
        for (const auto* obj : g.winner->getContents()) {
            if (obj->hasFlag(ObjectFlag::READBIT)) {
                readableObjects.push_back(const_cast<ZObject*>(obj));
            }
        }
        
        // Check open containers in room and inventory
        auto checkContainer = [&](ZObject* container) {
            if (container->hasFlag(ObjectFlag::CONTBIT) && container->hasFlag(ObjectFlag::OPENBIT)) {
                for (const auto* obj : container->getContents()) {
                    if (obj->hasFlag(ObjectFlag::READBIT)) {
                        readableObjects.push_back(const_cast<ZObject*>(obj));
                    }
                }
            }
        };
        
        for (const auto* obj : g.here->getContents()) {
            checkContainer(const_cast<ZObject*>(obj));
        }
        for (const auto* obj : g.winner->getContents()) {
            checkContainer(const_cast<ZObject*>(obj));
        }
        
        if (readableObjects.empty()) {
            printLine("What do you want to read?");
            getGlobalParser().setOrphanDirect(V_READ, "read");
            return RTRUE;
        }
        
        if (readableObjects.size() == 1) {
            // Implicit object selection
            g.prso = readableObjects[0];
            print("(" + g.prso->getDesc() + ")\n");
        } else {
            printLine("What do you want to read?");
            getGlobalParser().setOrphanDirect(V_READ, "read");
            return RTRUE;
        }
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
    
    // Check if player is holding the object
    if (g.prso->getLocation() != g.winner) {
        // Check if object can be taken
        if (!g.prso->hasFlag(ObjectFlag::TAKEBIT) || g.prso->hasFlag(ObjectFlag::TRYTAKEBIT)) {
            printLine("You can't take that.");
            return RTRUE;
        }
        
        // Check if object is accessible
        ZObject* objLocation = g.prso->getLocation();
        bool accessible = false;
        
        if (objLocation == g.here) {
            accessible = true;
        } else if (objLocation && objLocation->hasFlag(ObjectFlag::CONTBIT) && 
                   objLocation->hasFlag(ObjectFlag::OPENBIT)) {
            ZObject* containerLocation = objLocation->getLocation();
            if (containerLocation == g.here || containerLocation == g.winner) {
                accessible = true;
            }
        }
        
        if (!accessible) {
            printLine("You can't see any such thing.");
            return RTRUE;
        }
        
        // Check inventory weight limit
        int currentWeight = 0;
        for (const auto* obj : g.winner->getContents()) {
            currentWeight += obj->getProperty(P_SIZE);
        }
        
        int objectSize = g.prso->getProperty(P_SIZE);
        if (objectSize == 0) {
            objectSize = 5;
        }
        
        if (currentWeight + objectSize > g.loadAllowed) {
            printLine("You're carrying too much.");
            return RTRUE;
        }
        
        // Implicit TAKE
        print("(Taken)\n");
        g.prso->moveTo(g.winner);
    }
    
    // Display the text
    printLine(g.prso->getText());
    return RTRUE;
}

bool vLookInside() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to look inside?");
        getGlobalParser().setOrphanDirect(V_LOOK_INSIDE, "look inside");
        return RTRUE;
    }
    
    // Authentic ZIL V-LOOK-INSIDE logic
    if (g.prso->hasFlag(ObjectFlag::DOORBIT)) {
        // Door handling
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            print("The ");
            print(g.prso->getDesc());
            printLine(" is open, but I can't tell what's beyond it.");
        } else {
            print("The ");
            print(g.prso->getDesc());
            printLine(" is closed.");
        }
        return RTRUE;
    }
    
    if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        // Actor check - "There is nothing special to be seen."
        if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
            printLine("There is nothing special to be seen.");
            return RTRUE;
        }
        
        // Check if we can see inside (open or transparent)
        bool canSeeInside = g.prso->hasFlag(ObjectFlag::OPENBIT) || 
                           g.prso->hasFlag(ObjectFlag::TRANSBIT);
        
        if (canSeeInside) {
            const auto& contents = g.prso->getContents();
            if (contents.empty()) {
                print("The ");
                print(g.prso->getDesc());
                printLine(" is empty.");
            } else {
                // Print contents
                print("The ");
                print(g.prso->getDesc());
                printLine(" contains:");
                for (const auto* obj : contents) {
                    print("  ");
                    printLine(obj->getDesc());
                }
            }
        } else {
            print("The ");
            print(g.prso->getDesc());
            printLine(" is closed.");
        }
        return RTRUE;
    }
    
    // Not a container or door
    print("You can't look inside a ");
    print(g.prso->getDesc());
    printLine(".");
    return RTRUE;
}

bool vSearch() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to search?");
        getGlobalParser().setOrphanDirect(V_SEARCH, "search");
        return RTRUE;
    }
    
    // Check if object has an action handler that handles SEARCH
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-SEARCH: "You find nothing unusual."
    printLine("You find nothing unusual.");
    return RTRUE;
}

bool vSearchOld() {
    // Keeping old implementation for reference - can be removed
    auto& g = Globals::instance();
    
    if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
        if (g.prso->hasFlag(ObjectFlag::LOCKEDBIT)) {
            print("The ");
            print(g.prso->getDesc());
            printLine(" is locked.");
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
        printLine("What do you want to put?");
        getGlobalParser().setOrphanDirect(V_PUT, "put");
        return RTRUE;
    }
    
    // Check if indirect object is specified
    if (!g.prsi) {
        printLine("What do you want to put it in?");
        getGlobalParser().setOrphanIndirect(V_PUT, g.prso, "in");
        return RTRUE;
    }
    
    // Call indirect object's action handler first (ZIL behavior)
    // This allows special objects like GROUND to intercept PUT
    if (g.prsi->performAction()) {
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
    // ZIL formula: (WEIGHT(container) + WEIGHT(object) - SIZE(container)) > CAPACITY(container)
    // This means the net contents (excluding the container's own size) must fit within capacity
    int capacity = g.prsi->getProperty(P_CAPACITY);
    if (capacity == 0) {
        capacity = 100;  // Default capacity if not specified
    }
    
    // Calculate weight of container (including all nested contents)
    int containerWeight = calculateWeight(g.prsi);
    
    // Calculate weight of object to be added (including all nested contents)
    int objectWeight = calculateWeight(g.prso);
    
    // Get container's own size
    int containerSize = g.prsi->getProperty(P_SIZE);
    if (containerSize == 0) {
        containerSize = 10;  // Default size if not specified
    }
    
    // Check if adding this object would exceed capacity
    // Formula: (containerWeight + objectWeight - containerSize) > capacity
    if (containerWeight + objectWeight - containerSize > capacity) {
        printLine("There's no room.");
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
        printLine("What do you want to turn?");
        getGlobalParser().setOrphanDirect(V_TURN, "turn");
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
        printLine("What do you want to push?");
        getGlobalParser().setOrphanDirect(V_PUSH, "push");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-PUSH uses HACK-HACK
    print("Pushing the ");
    print(g.prso->getDesc());
    printLine(" isn't notably useful.");
    return RTRUE;
}

bool vPull() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to pull?");
        getGlobalParser().setOrphanDirect(V_PULL, "pull");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // V-PULL redirects to V-MOVE in ZIL
    return vMove();
}

bool vMove() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to move?");
        getGlobalParser().setOrphanDirect(V_MOVE, "move");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-MOVE
    if (g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        print("Moving the ");
        print(g.prso->getDesc());
        printLine(" reveals nothing.");
    } else {
        print("You can't move the ");
        print(g.prso->getDesc());
        printLine(".");
    }
    return RTRUE;
}

// Interaction Verbs (Requirement 27)

bool vTie() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to tie?");
        getGlobalParser().setOrphanDirect(V_TIE, "tie");
        return RTRUE;
    }
    
    // Check if indirect object is specified (tie X to Y)
    if (!g.prsi) {
        printLine("What do you want to tie it to?");
        getGlobalParser().setOrphanIndirect(V_TIE, g.prso, "to");
        return RTRUE;
    }
    
    // Call object action handlers
    if (g.prso->performAction()) {
        return RTRUE;
    }
    if (g.prsi->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-TIE
    if (g.prsi == g.winner) {
        printLine("You can't tie anything to yourself.");
    } else {
        print("You can't tie the ");
        print(g.prso->getDesc());
        printLine(" to that.");
    }
    return RTRUE;
}

bool vUntie() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to untie?");
        getGlobalParser().setOrphanDirect(V_UNTIE, "untie");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-UNTIE
    printLine("This cannot be tied, so it cannot be untied!");
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
        
        // Authentic ZIL V-LISTEN
        print("The ");
        print(g.prso->getDesc());
        printLine(" makes no sound.");
        return RTRUE;
    }
    
    // No object specified - listen to the room
    ZRoom* room = dynamic_cast<ZRoom*>(g.here);
    if (room) {
        room->performRoomAction(M_LISTEN);
    }
    
    // Default - no specific message for room listening in ZIL
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
        
        // Authentic ZIL V-SMELL
        print("It smells like a ");
        print(g.prso->getDesc());
        printLine(".");
        return RTRUE;
    }
    
    // No object specified - smell the room
    printLine("You smell nothing unusual.");
    return RTRUE;
}

bool vYell() {
    auto& g = Globals::instance();
    
    // Call room action handler for special yell behavior
    ZRoom* room = dynamic_cast<ZRoom*>(g.here);
    if (room) {
        room->performRoomAction(M_YELL);
    }
    
    // Default message
    printLine("Aaaarrrrgggghhhh!");
    return RTRUE;
}

bool vTouch() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
         printLine("Touch what?");
         return RTRUE;
    }
    
    // Call object action handler
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // ZIL default
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
         print("The ");
         print(g.prso->getDesc());
         printLine(" dislikes being touched.");
         return RTRUE;
    }
    
    print("You feel nothing unexpected as you touch the ");
    print(g.prso->getDesc());
    printLine(".");
    return RTRUE;
}

bool vGive() {
    auto& g = Globals::instance();
    
    // Check if player has the object being given
    if (g.prso->getLocation() != g.winner) {
        printLine("You're not holding that!");
        return RTRUE;
    }
    
    // Check if recipient is valid (PRSI is set via syntax `GIVE OBJ TO OBJ`)
    if (!g.prsi) {
        printLine("Give it to whom?");
        return RTRUE;
    }
    
    // Interact with recipient (PRSI) via M-GIVE action
    // In Zork, giving to an actor triggers their action handler with M-GIVE logic
    // We simulate this by calling performAction() on PRSI.
    // The action handler (e.g., Troll, Cyclops) checks for M-GIVE.
    // If PRSI performs action, it handled the gift (accepted/refused).
    if (g.prsi->performAction()) {
        return RTRUE;
    }
    
    // Default if recipient doesn't handle M-GIVE
    print("The ");
    print(g.prsi->getDesc());
    printLine(" refuses it politely.");
    return RTRUE;
}


// Consumption Verbs (Requirement 28)

bool vEat() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to eat?");
        getGlobalParser().setOrphanDirect(V_EAT, "eat");
        return RTRUE;
    }
    
    // Authentic ZIL V-EAT logic
    if (g.prso->hasFlag(ObjectFlag::FOODBIT)) {
        // Check if player is holding it
        if (g.prso->getLocation() != g.winner && 
            (!g.prso->getLocation() || g.prso->getLocation()->getLocation() != g.winner)) {
            printLine("You're not holding that.");
            return RTRUE;
        }
        
        // Call object action handler first
        if (g.prso->performAction()) {
            return RTRUE;
        }
        
        // Remove and print message
        g.prso->moveTo(nullptr);
        printLine("Thank you very much. It really hit the spot.");
        return RTRUE;
    }
    
    // Not food - check if drinkable
    if (g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
        // Redirect to drink
        return Verbs::vDrink();
    }
    
    // Neither food nor drink
    print("I don't think that the ");
    print(g.prso->getDesc());
    printLine(" would agree with you.");
    return RTRUE;
}

bool vDrink() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to drink?");
        getGlobalParser().setOrphanDirect(V_DRINK, "drink");
        return RTRUE;
    }
    
    // Authentic ZIL - V-DRINK calls V-EAT
    // But check DRINKBIT specifically
    if (g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
        // Call object action handler first
        if (g.prso->performAction()) {
            return RTRUE;
        }
        
        // Remove and print message
        g.prso->moveTo(nullptr);
        printLine("Thank you very much. I was rather thirsty (from all this talking, probably).");
        return RTRUE;
    }
    
    // Not drinkable
    print("I don't think that the ");
    print(g.prso->getDesc());
    printLine(" would agree with you.");
    return RTRUE;
}

// Light Source Verbs (Requirement 30)

bool vLampOn() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to turn on?");
        getGlobalParser().setOrphanDirect(V_LAMP_ON, "turn on");
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
    
    // Check if lamp has battery/fuel (Requirement 47)
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
    
    // Enable lamp timer if this is the brass lantern (Requirement 47)
    if (g.prso->getId() == ObjectIds::LAMP) {
        LampSystem::enableLampTimer();
    }
    
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
        printLine("What do you want to turn off?");
        getGlobalParser().setOrphanDirect(V_LAMP_OFF, "turn off");
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
    
    // Disable lamp timer if this is the brass lantern (Requirement 47)
    if (g.prso->getId() == ObjectIds::LAMP) {
        LampSystem::disableLampTimer();
    }
    
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
        printLine("What do you want to inflate?");
        getGlobalParser().setOrphanDirect(V_INFLATE, "inflate");
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
        printLine("What do you want to deflate?");
        getGlobalParser().setOrphanDirect(V_DEFLATE, "deflate");
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
    
    // Authentic ZIL V-PRAY default
    printLine("If you pray enough, your prayers may be answered.");
    return RTRUE;
}

bool vExorcise() {
    auto& g = Globals::instance();
    
    // Call object action handler first if object specified
    if (g.prso && g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-EXORCISE
    printLine("What a bizarre concept!");
    return RTRUE;
}

bool vWave() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to wave?");
        getGlobalParser().setOrphanDirect(V_WAVE, "wave");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-WAVE uses HACK-HACK
    print("Waving the ");
    print(g.prso->getDesc());
    printLine(" isn't notably useful.");
    return RTRUE;
}

bool vRub() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to rub?");
        getGlobalParser().setOrphanDirect(V_RUB, "rub");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-RUB uses HACK-HACK
    print("Fiddling with the ");
    print(g.prso->getDesc());
    printLine(" isn't notably useful.");
    return RTRUE;
}

bool vRing() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to ring?");
        getGlobalParser().setOrphanDirect(V_RING, "ring");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-RING
    printLine("How, exactly, can you ring that?");
    return RTRUE;
}

// Combat Verbs (Requirement 29)

bool vAttack() {
    auto& g = Globals::instance();
    
    // Check if target is specified
    if (!g.prso) {
        printLine("What do you want to attack?");
        getGlobalParser().setOrphanDirect(V_ATTACK, "attack");
        return RTRUE;
    }
    
    // Authentic ZIL V-ATTACK logic
    // Check if target is an actor
    if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
        print("I've known strange people, but fighting a ");
        print(g.prso->getDesc());
        printLine("?");
        return RTRUE;
    }
    
    // Check for weapon - must have one
    if (!g.prsi) {
        print("Trying to attack a ");
        print(g.prso->getDesc());
        printLine(" with your bare hands is suicidal.");
        return RTRUE;
    }
    
    // Check if holding the weapon
    if (g.prsi->getLocation() != g.winner) {
        print("You aren't even holding the ");
        print(g.prsi->getDesc());
        printLine(".");
        return RTRUE;
    }
    
    // Check if it's actually a weapon
    if (!g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
        print("Trying to attack the ");
        print(g.prso->getDesc());
        print(" with a ");
        print(g.prsi->getDesc());
        printLine(" is suicidal.");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Start combat using the combat system
    CombatSystem::startCombat(g.prso, g.prsi);
    CombatSystem::processCombatRound();
    
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
        printLine("What do you want to throw?");
        getGlobalParser().setOrphanDirect(V_THROW, "throw");
        return RTRUE;
    }
    
    // Call object action handler first
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Authentic ZIL V-THROW - drop the object first
    g.prso->moveTo(g.here);
    
    // Check if throwing at self
    if (g.prsi == g.winner) {
        print("A terrific throw! The ");
        print(g.prso->getDesc());
        printLine(" hits you squarely in the head. Normally, this wouldn't do much damage, but by incredible mischance, you fall over backwards trying to duck, and break your neck, justice being swift and merciful in the Great Underground Empire.");
        // This should trigger death - for now just return
        return RTRUE;
    }
    
    // Check if throwing at an actor
    if (g.prsi && g.prsi->hasFlag(ObjectFlag::ACTORBIT)) {
        print("The ");
        print(g.prsi->getDesc());
        print(" ducks as the ");
        print(g.prso->getDesc());
        printLine(" flies by and crashes to the ground.");
    } else {
        printLine("Thrown.");
    }
    
    return RTRUE;
}

bool vSwing() {
    auto& g = Globals::instance();
    
    // Check if object is specified
    if (!g.prso) {
        printLine("What do you want to swing?");
        getGlobalParser().setOrphanDirect(V_SWING, "swing");
        return RTRUE;
    }
    
    // Authentic ZIL V-SWING
    if (!g.prsi) {
        // No target - just swing
        printLine("Whoosh!");
        return RTRUE;
    }
    
    // Target specified - perform attack with swapped objects
    // SWING weapon AT target -> ATTACK target WITH weapon
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
    
    // Check if in combat and track health
    if (CombatSystem::isInCombat()) {
        auto& combatManager = CombatSystem::CombatManager::instance();
        auto playerCombatant = combatManager.getPlayerCombatant();
        
        if (playerCombatant && playerCombatant->object == g.winner) {
            int health = playerCombatant->health;
            int maxHealth = playerCombatant->maxHealth;
            
            if (health == maxHealth) {
                printLine("You are in perfect health.");
            } else if (health > maxHealth * 3 / 4) {
                printLine("You have a few grazes.");
            } else if (health > maxHealth / 2) {
                printLine("You have some serious wounds.");
            } else if (health > 0) {
                printLine("You are staggering.");
            } else {
                printLine("You are dead.");
            }
            return RTRUE;
        }
    }
    
    // Default / Out of combat
    // Since health is ephemeral in current CombatSystem, assume perfect health when not fighting
    printLine("You are in perfect health.");
    
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

// Game Control Verbs (Requirement 33, 60, 61, 62, 69, 70)

bool vSave() {
    auto& g = Globals::instance();
    
    // Prompt for filename (Requirement 60)
    printLine("Enter save filename:");
    std::string filename = readLine();
    
    // Handle empty filename
    if (filename.empty()) {
        printLine("Save cancelled.");
        return RTRUE;
    }
    
    // Add .sav extension if not present
    if (filename.find('.') == std::string::npos) {
        filename += ".sav";
    }
    
    // Try to open file for writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        printLine("Error: Could not create save file.");
        return RTRUE;
    }
    
    // Serialize game state (Requirement 60)
    // Format: Simple text-based format for readability and debugging
    
    // Write header
    file << "ZORK1_SAVE_V1\n";
    
    // Save player location (Requirement 60.2)
    if (g.here) {
        file << "LOCATION:" << g.here->getId() << "\n";
    } else {
        file << "LOCATION:0\n";
    }
    
    // Save score and moves (Requirement 60.2)
    file << "SCORE:" << g.score << "\n";
    file << "MOVES:" << g.moves << "\n";
    
    // Save display mode flags
    file << "VERBOSE:" << (g.verboseMode ? 1 : 0) << "\n";
    file << "BRIEF:" << (g.briefMode ? 1 : 0) << "\n";
    file << "SUPERBRIEF:" << (g.superbriefMode ? 1 : 0) << "\n";
    
    // Save load limits
    file << "LOADMAX:" << g.loadMax << "\n";
    file << "LOADALLOWED:" << g.loadAllowed << "\n";
    
    // Save all object states (Requirement 60.3, 60.4)
    file << "OBJECTS_BEGIN\n";
    for (const auto& [id, obj] : g.getAllObjects()) {
        if (!obj) continue;
        
        // Save object ID
        file << "OBJ:" << id << "\n";
        
        // Save object location (Requirement 60.3)
        ZObject* loc = obj->getLocation();
        if (loc) {
            file << "LOC:" << loc->getId() << "\n";
        } else {
            file << "LOC:-1\n";
        }
        
        // Save object flags (Requirement 60.4)
        // We save flags as a bitmask
        uint32_t flags = 0;
        for (int i = 0; i < 32; i++) {
            if (obj->hasFlag(static_cast<ObjectFlag>(1 << i))) {
                flags |= (1 << i);
            }
        }
        file << "FLAGS:" << flags << "\n";
        
        // Save key properties (Requirement 60.4)
        file << "P_SIZE:" << obj->getProperty(P_SIZE) << "\n";
        file << "P_CAPACITY:" << obj->getProperty(P_CAPACITY) << "\n";
        file << "P_VALUE:" << obj->getProperty(P_VALUE) << "\n";
        file << "P_STRENGTH:" << obj->getProperty(P_STRENGTH) << "\n";
        
        file << "OBJ_END\n";
    }
    file << "OBJECTS_END\n";
    
    // Close file
    file.close();
    
    if (file.fail()) {
        printLine("Error: Failed to write save file.");
        return RTRUE;
    }
    
    printLine("Game saved.");
    return RTRUE;
}

bool vRestore() {
    auto& g = Globals::instance();
    
    // Prompt for filename (Requirement 61)
    printLine("Enter save filename to restore:");
    std::string filename = readLine();
    
    // Handle empty filename
    if (filename.empty()) {
        printLine("Restore cancelled.");
        return RTRUE;
    }
    
    // Add .sav extension if not present
    if (filename.find('.') == std::string::npos) {
        filename += ".sav";
    }
    
    // Try to open file for reading
    std::ifstream file(filename);
    if (!file) {
        printLine("Error: Could not open save file.");
        return RTRUE;
    }
    
    // Read and verify header
    std::string line;
    if (!std::getline(file, line) || line != "ZORK1_SAVE_V1") {
        printLine("Error: Invalid or corrupted save file.");
        return RTRUE;
    }
    
    // Temporary storage for parsed values
    ObjectId playerLocation = 0;
    int savedScore = 0;
    int savedMoves = 0;
    bool savedVerbose = true;
    bool savedBrief = false;
    bool savedSuperbrief = false;
    int savedLoadMax = 100;
    int savedLoadAllowed = 100;
    
    // Map of object ID to saved state
    struct ObjectState {
        ObjectId location = static_cast<ObjectId>(-1);
        uint32_t flags = 0;
        int size = 0;
        int capacity = 0;
        int value = 0;
        int strength = 0;
    };
    std::map<ObjectId, ObjectState> objectStates;
    
    // Parse save file (Requirement 61.1)
    ObjectId currentObjId = 0;
    bool inObjects = false;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            if (line == "OBJECTS_BEGIN") {
                inObjects = true;
                continue;
            }
            if (line == "OBJECTS_END") {
                inObjects = false;
                continue;
            }
            if (line == "OBJ_END") {
                continue;
            }
            continue;
        }
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        if (!inObjects) {
            // Parse global state
            if (key == "LOCATION") {
                playerLocation = static_cast<ObjectId>(std::stoi(value));
            } else if (key == "SCORE") {
                savedScore = std::stoi(value);
            } else if (key == "MOVES") {
                savedMoves = std::stoi(value);
            } else if (key == "VERBOSE") {
                savedVerbose = (std::stoi(value) != 0);
            } else if (key == "BRIEF") {
                savedBrief = (std::stoi(value) != 0);
            } else if (key == "SUPERBRIEF") {
                savedSuperbrief = (std::stoi(value) != 0);
            } else if (key == "LOADMAX") {
                savedLoadMax = std::stoi(value);
            } else if (key == "LOADALLOWED") {
                savedLoadAllowed = std::stoi(value);
            }
        } else {
            // Parse object state
            if (key == "OBJ") {
                currentObjId = static_cast<ObjectId>(std::stoi(value));
                objectStates[currentObjId] = ObjectState();
            } else if (key == "LOC") {
                objectStates[currentObjId].location = static_cast<ObjectId>(std::stoi(value));
            } else if (key == "FLAGS") {
                objectStates[currentObjId].flags = static_cast<uint32_t>(std::stoul(value));
            } else if (key == "P_SIZE") {
                objectStates[currentObjId].size = std::stoi(value);
            } else if (key == "P_CAPACITY") {
                objectStates[currentObjId].capacity = std::stoi(value);
            } else if (key == "P_VALUE") {
                objectStates[currentObjId].value = std::stoi(value);
            } else if (key == "P_STRENGTH") {
                objectStates[currentObjId].strength = std::stoi(value);
            }
        }
    }
    
    file.close();
    
    // Apply restored state (Requirement 61.2, 61.3, 61.4)
    
    // Restore player location
    ZObject* newLocation = g.getObject(playerLocation);
    if (newLocation) {
        g.here = newLocation;
        if (g.winner) {
            g.winner->moveTo(newLocation);
        }
    }
    
    // Restore score and moves
    g.score = savedScore;
    g.moves = savedMoves;
    
    // Restore display modes
    g.verboseMode = savedVerbose;
    g.briefMode = savedBrief;
    g.superbriefMode = savedSuperbrief;
    
    // Restore load limits
    g.loadMax = savedLoadMax;
    g.loadAllowed = savedLoadAllowed;
    
    // Restore object states
    for (const auto& [id, state] : objectStates) {
        ZObject* obj = g.getObject(id);
        if (!obj) continue;
        
        // Restore location
        if (state.location == static_cast<ObjectId>(-1)) {
            obj->moveTo(nullptr);
        } else {
            ZObject* loc = g.getObject(state.location);
            if (loc) {
                obj->moveTo(loc);
            }
        }
        
        // Restore flags
        for (int i = 0; i < 32; i++) {
            ObjectFlag flag = static_cast<ObjectFlag>(1 << i);
            if (state.flags & (1 << i)) {
                obj->setFlag(flag);
            } else {
                obj->clearFlag(flag);
            }
        }
        
        // Restore properties
        if (state.size > 0) obj->setProperty(P_SIZE, state.size);
        if (state.capacity > 0) obj->setProperty(P_CAPACITY, state.capacity);
        if (state.value > 0) obj->setProperty(P_VALUE, state.value);
        if (state.strength > 0) obj->setProperty(P_STRENGTH, state.strength);
    }
    
    printLine("Game restored.");
    
    // Show current location
    Verbs::vLook();
    
    return RTRUE;
}

bool vRestart() {
    // Confirm with player (Requirement 62.5)
    printLine("Are you sure you want to restart? (yes/no)");
    std::string response = readLine();
    
    // Convert to lowercase for comparison
    for (char& c : response) {
        c = std::tolower(c);
    }
    
    if (response != "yes" && response != "y") {
        printLine("Restart cancelled.");
        return RTRUE;
    }
    
    auto& g = Globals::instance();
    
    // Reset all game state (Requirement 62.1, 62.4)
    g.score = 0;
    g.moves = 0;
    g.verboseMode = true;
    g.briefMode = false;
    g.superbriefMode = false;
    g.loadMax = 100;
    g.loadAllowed = 100;
    g.lit = false;
    g.pCont = false;
    g.quoteFlag = false;
    
    // Clear all objects and reinitialize world (Requirement 62.2, 62.3)
    g.reset();
    
    // Reinitialize the world
    initializeWorld();
    
    printLine("ZORK I: The Great Underground Empire");
    printLine("Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.");
    printLine("ZORK is a registered trademark of Infocom, Inc.");
    printLine("C++ Port - Release 1");
    crlf();
    
    // Show starting location
    Verbs::vLook();
    
    return RTRUE;
}

bool vVersion() {
    // Display game name and version (Requirement 69.2)
    printLine("ZORK I: The Great Underground Empire");
    
    // Display port information (Requirement 69.3)
    printLine("C++ Port - Release 1");
    
    // Display copyright (Requirement 69.4)
    printLine("Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.");
    printLine("ZORK is a registered trademark of Infocom, Inc.");
    
    // Display interpreter info
    printLine("Interpreter: C++17 Native");
    
    return RTRUE;
}

bool vScript() {
    auto& g = Globals::instance();
    if (g.scripting) {
        printLine("Scripting is already on.");
        return RTRUE;
    }
    g.scripting = true;
    printLine("Scripting initiated.");
    return RTRUE;
}

bool vUnscript() {
    auto& g = Globals::instance();
    if (!g.scripting) {
        printLine("Scripting is already off.");
        return RTRUE;
    }
    g.scripting = false;
    printLine("Scripting ended.");
    return RTRUE;
}

// Communication Verbs

bool vTalk() {
    auto& g = Globals::instance();
    
    // Check if target is specified
    if (!g.prso) {
        printLine("Talk to whom?");
        return RTRUE;
    }
    
    // Check if target is an actor/NPC
    if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
        printLine("You can't talk to that.");
        return RTRUE;
    }
    
    // Call object action handler for custom responses
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default response
    printLine("There is no response.");
    return RTRUE;
}

bool vAsk() {
    auto& g = Globals::instance();
    
    // Check if target is specified
    if (!g.prso) {
        printLine("Ask whom?");
        return RTRUE;
    }
    
    // Check if target is an actor/NPC
    if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
        printLine("You can't ask that.");
        return RTRUE;
    }
    
    // Call object action handler for custom responses
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default response
    printLine("There is no response.");
    return RTRUE;
}

bool vTell() {
    auto& g = Globals::instance();
    
    // Check if target is specified
    if (!g.prso) {
        printLine("Tell whom?");
        return RTRUE;
    }
    
    // Check if target is an actor/NPC
    if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
        printLine("You can't tell that anything.");
        return RTRUE;
    }
    
    // Call object action handler for custom responses
    if (g.prso->performAction()) {
        return RTRUE;
    }
    
    // Default response
    printLine("There is no response.");
    return RTRUE;
}

bool vOdysseus() {
    // Special verb for the cyclops puzzle
    // This is called when player types "odysseus" or "ulysses"
    return NPCSystem::handleOdysseus() ? RTRUE : RFALSE;
}

// Easter eggs / special words - authentic ZIL responses

bool vHello() {
    auto& g = Globals::instance();
    
    // If object specified, greet that object
    if (g.prso) {
        if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
            print("The ");
            print(g.prso->getDesc());
            printLine(" bows his head to you in greeting.");
        } else {
            print("It's a well known fact that only schizophrenics say \"Hello\" to a ");
            print(g.prso->getDesc());
            printLine(".");
        }
        return RTRUE;
    }
    
    // No object - generic hello responses
    // ZIL has a PICK-ONE from HELLOS table
    static const char* hellos[] = {
        "Hello.",
        "Good day.",
        "Nice weather we've been having lately.",
        "Goodbye."
    };
    static int helloIdx = 0;
    printLine(hellos[helloIdx % 4]);
    helloIdx++;
    return RTRUE;
}

bool vZork() {
    // Authentic ZIL V-ZORK
    printLine("At your service!");
    return RTRUE;
}

bool vPlugh() {
    // Authentic ZIL V-ADVENT (handles PLUGH and XYZZY)
    // Reference to Colossal Cave Adventure
    printLine("A hollow voice says \"Fool.\"");
    return RTRUE;
}

bool vFrobozz() {
    // Authentic ZIL V-FROBOZZ
    printLine("The FROBOZZ Corporation created, owns, and operates this dungeon.");
    return RTRUE;
}

// Additional common verbs - authentic ZIL responses

bool vWait() {
    // Authentic ZIL V-WAIT - passes time
    printLine("Time passes...");
    // In full implementation, this would call CLOCKER multiple times
    return RTRUE;
}

bool vSwim() {
    // Authentic ZIL V-SWIM
    printLine("Go jump in a lake!");
    return RTRUE;
}

bool vBack() {
    // Authentic ZIL V-BACK
    printLine("Sorry, my memory is poor. Please give a direction.");
    return RTRUE;
}

bool vJump() {
    // Authentic ZIL V-LEAP/V-SKIP responses
    static const char* responses[] = {
        "Very good. Now you can go to the second grade.",
        "Are you enjoying yourself?",
        "Wheeeeeeeeee!!!!!",
        "Do you expect me to applaud?"
    };
    static int idx = 0;
    printLine(responses[idx % 4]);
    idx++;
    return RTRUE;
}

bool vCurse() {
    auto& g = Globals::instance();
    
    // Authentic ZIL V-CURSES
    if (g.prso) {
        if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
            printLine("Insults of this nature won't help you.");
        } else {
            printLine("What a loony!");
        }
    } else {
        printLine("Such language in a high-class establishment like this!");
    }
    return RTRUE;
}

// New ZIL Audit Verbs implementation
bool vMung() {
    printLine("Trying to destroy things is a waste of time.");
    return RTRUE;
}

bool vWear() {
    printLine("You can't wear that!");
    return RTRUE;
}

bool vFind() {
    print("You can't see any ");
    if (Globals::instance().prso) print(Globals::instance().prso->getDesc());
    printLine(" here.");
    return RTRUE;
}

bool vLeap() {
    return vJump(); // Share logic with JUMP
}

bool vSay() {
    printLine("Talking to yourself is a sign of impending mental collapse.");
    return RTRUE;
}

bool vKick() {
    printLine("Violence isn't the answer to this one.");
    return RTRUE;
}

bool vBreathe() {
    printLine("How can you breathe that?");
    return RTRUE;
}

bool vRape() {
    printLine("What a loony!");
    return RTRUE;
}


// Phase 10.3 Batch 1: Movement & Positioning

bool vClimbFoo() {
    auto& g = Globals::instance();
    if (g.prso && g.prso->hasFlag(ObjectFlag::CLIMBBIT)) {
        print("Climbing the ");
        print(g.prso->getDesc());
        printLine(" doesn't get you anywhere.");
        return RTRUE;
    }
    printLine("You can't climb that.");
    return RTRUE;
}

bool vThrough() {
    auto& g = Globals::instance();
    if (g.prso) {
         if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
             printLine("You should use 'BOARD' to enter a vehicle."); 
             return RTRUE;
         }
         print("You hit your head against the ");
         print(g.prso->getDesc());
         printLine(" as you attempt this feat.");
         return RTRUE;
    }
    return RTRUE;
}

bool vStand() {
    printLine("You are already standing.");
    return RTRUE;
}

bool vAlarm() {
    print("The ");
    if (Globals::instance().prso) print(Globals::instance().prso->getDesc());
    printLine(" isn't sleeping.");
    return RTRUE;
}

bool vWalkAround() {
    printLine("Use compass directions for movement.");
    return RTRUE;
}

bool vWalkTo() {
    printLine("You should use a direction.");
    return RTRUE;
}

bool vLaunch() {
    auto& g = Globals::instance();
    if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
        printLine("You can't launch that.");
    } else {
        printLine("That's not a vehicle!");
    }
    return RTRUE;
}


// Phase 10.3 Batch 2: Manipulation

bool vCut() {
    printLine("It doesn't seem to work.");
    return RTRUE;
}

bool vLower() {
    printLine("You can't lower that.");
    return RTRUE;
}

bool vRaise() {
    printLine("You can't raise that.");
    return RTRUE;
}

bool vMake() {
    printLine("You can't do that.");
    return RTRUE;
}

bool vMelt() {
    printLine("It's not clear that needs melting.");
    return RTRUE;
}

bool vPlay() {
    printLine("That's silly!");
    return RTRUE;
}

bool vPlug() {
    printLine("This has no effect.");
    return RTRUE;
}

bool vPourOn() {
    printLine("You can't pour that on anything.");
    return RTRUE;
}

bool vPushTo() {
    printLine("You can't push things to that.");
    return RTRUE;
}

bool vPutUnder() {
    printLine("You can't do that.");
    return RTRUE;
}

bool vPutBehind() {
    printLine("You can't do that.");
    return RTRUE;
}

bool vShake() {
    printLine("Shaken.");
    return RTRUE;
}

bool vSpin() {
    printLine("You can't spin that.");
    return RTRUE;
}

bool vSqueeze() {
    printLine("You can't squeeze that.");
    return RTRUE;
}

bool vWind() {
    printLine("You can't wind that up.");
    return RTRUE;
}

bool vTieUp() {
    printLine("You can't tie that up.");
    return RTRUE;
}


// Phase 10.3 Batch 3: Interactions

bool vAnswer() {
    printLine("There is no one waiting for an answer.");
    return RTRUE;
}

bool vReply() {
    printLine("It is hardly likely that the ");
    if (Globals::instance().prso) print(Globals::instance().prso->getDesc());
    printLine(" is interested.");
    return RTRUE;
}

bool vCommand() {
    if (Globals::instance().prso) {
         print("The ");
         print(Globals::instance().prso->getDesc());
         printLine(" pays no attention.");
    } else {
         printLine("Command whom?");
    }
    return RTRUE;
}

bool vEcho() {
    printLine("Echo... echo... echo...");
    return RTRUE;
}

bool vFollow() {
    printLine("You're not following anything.");
    return RTRUE;
}

bool vKiss() {
    printLine("I'd sooner kiss a pig.");
    return RTRUE;
}

bool vMumble() {
    printLine("You'll have to speak up if you expect me to hear you!");
    return RTRUE;
}

bool vRepent() {
    printLine("It could well be too late!");
    return RTRUE;
}

bool vSend() {
    printLine("Why would you send for that?");
    return RTRUE;
}

bool vWish() {
    printLine("With luck, your wish will come true.");
    return RTRUE;
}

bool vSpray() {
    printLine("You can't spray that.");
    return RTRUE;
}


// Phase 10.3 Batch 4: Magic/Misc

bool vBlast() {
    printLine("Start small, why don't you?");
    return RTRUE;
}

bool vBurn() {
    printLine("You can't burn that.");
    return RTRUE;
}

bool vChant() {
    printLine("Chanting won't help you.");
    return RTRUE;
}

bool vDisenchant() {
    printLine("You can't disenchant that.");
    return RTRUE;
}

bool vEnchant() {
    printLine("You're not a wizard.");
    return RTRUE;
}

bool vIncant() {
    printLine("The incantation fails.");
    return RTRUE;
}

bool vWin() {
    printLine("Nature, red in tooth and claw...");
    return RTRUE;
}

bool vTreasure() {
    printLine("This is a treasure/adventure.");
    return RTRUE;
}

bool vStay() {
    printLine("You will be captured.");
    return RTRUE;
}




// Phase 10.3 Batch 5: Cleanup & Edge Cases

bool vBrush() {
    printLine("You can't brush that.");
    return RTRUE;
}

bool vBug() {
    printLine("If you find a bug, please start an issue on the tracker."); // Modernized
    return RTRUE;
}

bool vChomp() {
    printLine("Preposterous!");
    // ZIL: "I don't know how to do that. I win in all cases."
    return RTRUE;
}

bool vCount() {
    printLine("You have lost your mind.");
    return RTRUE;
}

bool vCross() {
    printLine("You can't cross that.");
    return RTRUE;
}

bool vHatch() {
    printLine("I don't think that can be hatched.");
    // ZIL: "Bizarre!"
    return RTRUE;
}

bool vKnock() {
    printLine("Why knock on that?");
    return RTRUE;
}

bool vLeave() {
    printLine("Leave what?");
    return RTRUE;
}

bool vLeanOn() {
    printLine("That's not a good idea.");
    return RTRUE;
}

bool vPump() {
    printLine("It's not a pump.");
    return RTRUE;
}

bool vStrike() {
    printLine("You have to be more specific!"); // Intransitive response
    return RTRUE;
}

bool vReadPage() {
    printLine("You can't read that.");
    return RTRUE;
}


bool vPick() {
    printLine("Pick what?");
    return RTRUE;
}

bool vApply() {
    printLine("Apply what?");
    return RTRUE;
}

} // namespace Verbs
