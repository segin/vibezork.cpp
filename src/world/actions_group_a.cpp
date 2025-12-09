#include "core/globals.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "core/io.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "systems/death.h"

// Helper for STUPID-CONTAINER logic (Bag of Coins, Trunk)
// Returns true if handled
bool stupidContainerAction(const std::string& contentName) {
    auto& g = Globals::instance();
    
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        print("The ");
        print(contentName);
        printLine(" are safely inside; there's no need to do that.");
        return true;
    }
    
    if (g.prsa == V_LOOK_INSIDE || g.prsa == V_EXAMINE) {
        print("There are lots of ");
        print(contentName);
        printLine(" in there.");
        return true;
    }
    
    if (g.prsa == V_PUT || g.prsa == V_PUT_ON) { // PUT X IN Y
        if (g.prsi == g.prso) { // If putting INTO this container (self-reference check)
             printLine("Don't be silly. It wouldn't be a bag of coins anymore.");
             return true;
        }
    }
    
    return false;
}

bool bagOfCoinsAction() {
    return stupidContainerAction("coins");
}

bool trunkAction() {
    return stupidContainerAction("jewels");
}

// WEAPON-FUNCTION helper (shared by AXE-F and STILETTO-FUNCTION)
// ZIL: Prevents taking weapon if NPC wielder is present
// W = weapon object, V = villain/NPC wielding it
static bool weaponFunction(ZObject* weapon, ZObject* villain) {
    auto& g = Globals::instance();
    
    // If villain is not in current room, allow normal handling
    if (!villain || villain->getLocation() != g.here) {
        return false;
    }
    
    // Only applies to TAKE verb
    if (g.prsa != V_TAKE) {
        return false;
    }
    
    // Check if weapon is in villain's possession
    if (weapon->getLocation() == villain) {
        // Villain swings weapon out of reach
        print("The ");
        print(villain->getDesc());
        printLine(" swings it out of your reach.");
        return true;
    } else {
        // Weapon is white-hot (magical curse)
        print("The ");
        print(weapon->getDesc());
        printLine(" seems white-hot. You can't hold on to it.");
        return true;
    }
}

// AXE-F - Axe interaction with Troll
// ZIL: <COND (,TROLL-FLAG <>) (T <WEAPON-FUNCTION ,AXE ,TROLL>)>
bool axeAction() {
    auto& g = Globals::instance();
    
    // Check TROLL-FLAG - if true, troll is dead, allow normal handling
    // TROLL-FLAG is stored as a property on the troll or as global state
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    
    // If troll is dead (not in game), allow normal behavior
    if (!troll || troll->getLocation() == nullptr) {
        return false;
    }
    
    // If troll has NDESCBIT set, it's dead/unconscious
    if (troll->hasFlag(ObjectFlag::NDESCBIT)) {
        return false;
    }
    
    // Call weapon function
    return weaponFunction(g.prso, troll);
}

// GRUE-FUNCTION - Called when player is in darkness
bool grueAction() {
    printLine("It is pitch black. You are likely to be eaten by a grue.");
    return true;
}

// BARROW-DOOR-FCN
bool barrowDoorAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        printLine("The door is firmly closed and cannot be opened.");
        return true;
    }
    return false;
}

// CELLAR-FCN (Room action for Cellar)
void cellarAction(int rarg) {
    // M_LOOK: Handle darkness, trap door, etc.
    // For now, stub
}

// CHIMNEY-F
bool chimneyAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_UP || g.prsa == V_CLIMB_DOWN) {
        printLine("The chimney is too narrow.");
        return true;
    }
    return false;
}

// CLEARING-FCN (Room action for Clearing)
void clearingAction(int rarg) {
    // Stub - handle grating visibility, leaves pile, etc.
}

// CRACK-FCN
bool crackAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_LOOK_INSIDE || g.prsa == V_EXAMINE) {
        printLine("The crack is too small for you to see anything.");
        return true;
    }
    return false;
}

// CRETIN-FCN (handles "me", "self", "cretin")
bool cretinAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("That's interesting. You look like you.");
        return true;
    }
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        printLine("Suicide is not the answer.");
        return true;
    }
    return false;
}

// CYCLOPS-ROOM-FCN (Room action)
void cyclopsRoomAction(int rarg) {
    // Handle Cyclops waking, blocking exit, etc.
}

// DAM-ROOM-FCN (Room action for Dam)
void damRoomAction(int rarg) {
    // Stub
}

// DBOAT-FUNCTION (Deflated boat)
bool deflatedBoatAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_INFLATE) {
        // Check for pump
        ZObject* pump = g.getObject(ObjectIds::PUMP);
        if (pump && pump->getLocation() == g.winner) {
            printLine("The boat inflates and is now ready to board.");
            // Transform to inflated boat
            return true;
        } else {
            printLine("You don't have anything to inflate it with.");
            return true;
        }
    }
    return false;
}

// DEEP-CANYON-F
bool deepCanyonAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_DOWN) {
        printLine("It's a long way down. Are you sure you want to do that?");
        return true;
    }
    return false;
}

// DOME-ROOM-FCN (Room action)
void domeRoomAction(int rarg) {
    // Handle rope, jumping down, etc.
}

// FRONT-DOOR-FCN
bool frontDoorAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        printLine("The door is boarded and nailed shut.");
        return true;
    }
    return false;
}

// GARLIC-F
bool garlicAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EAT) {
        printLine("What disgusting taste you have!");
        g.prso->moveTo(nullptr); // Consumed
        return true;
    }
    return false;
}

// GHOSTS-F
bool ghostsAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        printLine("How can you attack a spirit with material weapons?");
        return true;
    }
    if (g.prsa == V_EXORCISE) {
        printLine("The spirits depart with a faint sighing sound.");
        g.prso->moveTo(nullptr);
        return true;
    }
    return false;
}

// GRANITE-WALL-F
bool graniteWallAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_ATTACK || g.prsa == V_PUSH || g.prsa == V_MOVE) {
        printLine("It's solid granite.");
        return true;
    }
    return false;
}

// GRATE-FUNCTION
bool grateAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN) {
        // Check for key or unlock first
        if (g.prso && g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The grate is already open.");
        } else {
            printLine("The grate is locked.");
        }
        return true;
    }
    return false;
}

// HOT-BELL-F
bool hotBellAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("The bell is too hot to take!");
        return true;
    }
    if (g.prsa == V_RING) {
        printLine("The bell is too hot to ring.");
        return true;
    }
    return false;
}

// IBOAT-FUNCTION (Inflated boat)
bool inflatedBoatAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_DEFLATE) {
        printLine("The boat deflates.");
        // Transform back
        return true;
    }
    return false;
}

// KNIFE-F - Knife object action
// ZIL: On TAKE, clears ATTIC-TABLE NDESCBIT (makes table visible)
// Source: 1actions.zil lines 926-929
bool knifeAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_TAKE) {
        // ZIL: <FCLEAR ,ATTIC-TABLE ,NDESCBIT>
        // When knife is taken, clear the NDESCBIT on attic table
        // This makes the table visible/described in room descriptions
        ZObject* atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
        if (atticTable) {
            atticTable->clearFlag(ObjectFlag::NDESCBIT);
        }
        // Return false to allow normal TAKE handling
    }
    return false;
}

// LARGE-BAG-F - Thief's bag object action
// ZIL: Prevents taking/opening bag while thief is alive/unconscious
// Source: 1actions.zil lines 2094-2112
bool largeBagAction() {
    auto& g = Globals::instance();
    
    // TAKE - blocked based on thief state
    if (g.prsa == V_TAKE) {
        ZObject* thief = g.getObject(ObjectIds::THIEF);
        // Check if thief is unconscious (collapsed on bag) vs dead
        // If thief has NDESCBIT, he's dead; otherwise he's alive/unconscious
        if (thief && !thief->hasFlag(ObjectFlag::NDESCBIT)) {
            printLine("Sadly for you, the robber collapsed on top of the bag. Trying to take it would wake him.");
        } else {
            printLine("The bag will be taken over his dead body.");
        }
        return true;
    }
    
    // PUT into bag blocked
    if (g.prsa == V_PUT && g.prsi && g.prsi->getId() == ObjectIds::BAG) {
        printLine("It would be a good trick.");
        return true;
    }
    
    // OPEN/CLOSE blocked
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        printLine("Getting close enough would be a good trick.");
        return true;
    }
    
    // EXAMINE/LOOK-INSIDE
    if (g.prsa == V_EXAMINE || g.prsa == V_LOOK_INSIDE) {
        printLine("The bag is underneath the thief, so one can't say what, if anything, is inside.");
        return true;
    }
    
    return false;
}

// LEAK-FUNCTION
bool leakAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_PLUG) {
        printLine("You can't plug the leak.");
        return true;
    }
    return false;
}

// LIVING-ROOM-FCN (Room action)
void livingRoomAction(int rarg) {
    // Handle rug, trap door, etc.
}

// LOUD-ROOM-FCN (Room action - echoes)
void loudRoomAction(int rarg) {
    // Any speech gets echoed and causes platinum bar to fall
}

// MACHINE-ROOM-FCN
void machineRoomAction(int rarg) {
    // Stub
}

// MAZE-11-FCN (Special maze room)
void maze11Action(int rarg) {
    // Contains the grating
}

// MOUNTAIN-RANGE-F
bool mountainRangeAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_UP || g.prsa == V_CLIMB_DOWN) {
        printLine("The mountains are impassable.");
        return true;
    }
    return false;
}

// MSWITCH-FUNCTION (Machine switch)
bool machineSwitchAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TURN || g.prsa == V_PUSH) {
        printLine("Click.");
        // Toggle machine state
        return true;
    }
    return false;
}

// NOT-HERE-OBJECT-F
bool notHereObjectAction() {
    printLine("You can't see any such thing.");
    return true;
}

// NULL-F (Does nothing, returns false)
bool nullAction() {
    return false;
}

// PUTTY-FCN
bool puttyAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_PLUG) {
        printLine("The putty isn't sticky enough.");
        return true;
    }
    return false;
}

// RAINBOW-FCN
bool rainbowAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_ON || g.prsa == V_WALK) {
        // Check if pot of gold at end
        printLine("Can you walk on a rainbow?");
        return true;
    }
    return false;
}

// RBOAT-FUNCTION (Rubber boat - punctured?)
bool puncturedBoatAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_INFLATE) {
        printLine("The boat has a hole in it and won't hold air.");
        return true;
    }
    return false;
}

// RESERVOIR-FCN
void reservoirAction(int rarg) {
    // Handle water level, swimming, etc.
}

// RESERVOIR-NORTH-FCN
void reservoirNorthAction(int rarg) {
    // Stub
}

// RESERVOIR-SOUTH-FCN
void reservoirSouthAction(int rarg) {
    // Stub
}

// ROBBER-FUNCTION (Thief NPC AI)
bool robberAction() {
    // Complex thief AI - stub for now
    return false;
}

// RUSTY-KNIFE-FCN - Cursed knife that kills player when used to attack
// ZIL Source: 1actions.zil lines 907-924
bool rustyKnifeAction() {
    auto& g = Globals::instance();
    
    // TAKE with sword present - sword glows
    if (g.prsa == V_TAKE) {
        ZObject* sword = g.getObject(ObjectIds::SWORD);
        if (sword && sword->getLocation() == g.winner) {
            printLine("As you touch the rusty knife, your sword gives a single pulse of blinding blue light.");
        }
        return false;  // Allow take to proceed
    }
    
    // ATTACK with rusty knife or SWING at something - kills player!
    if ((g.prsa == V_ATTACK && g.prsi && g.prsi->getId() == ObjectIds::RUSTY_KNIFE) ||
        (g.prsa == V_SWING && g.prso && g.prso->getId() == ObjectIds::RUSTY_KNIFE && g.prsi)) {
        // Remove knife and kill player
        ZObject* knife = g.getObject(ObjectIds::RUSTY_KNIFE);
        if (knife) knife->moveTo(nullptr);
        
        printLine("As the knife approaches its victim, your mind is submerged by an "
                  "overmastering will. Slowly, your hand turns, until the rusty blade "
                  "is an inch from your neck. The knife seems to sing as it savagely "
                  "slits your throat.");
        // Trigger death using jigsUp (the knife curse has already printed its message)
        DeathSystem::jigsUp("", DeathSystem::DeathCause::OTHER);
        return true;
    }
    
    return false;
}

// SAILOR-FCN
bool sailorAction() {
    // Handles "Hello, Sailor!" easter egg
    return false;
}

// SAND-FUNCTION
bool sandAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_DIG) {
        // Check for shovel
        ZObject* shovel = g.getObject(ObjectIds::SHOVEL);
        if (g.prsi == shovel || (shovel && shovel->getLocation() == g.winner)) {
            printLine("You dig in the sand and reveal a scarab!");
            // Reveal scarab
            return true;
        } else {
            printLine("You dig with your hands but find nothing.");
            return true;
        }
    }
    return false;
}

// SANDWICH-BAG-FCN
bool sandwichBagAction() {
    return stupidContainerAction("food");
}

// SCEPTRE-FUNCTION
bool sceptreAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_WAVE) {
        // Check if at rainbow
        printLine("A dazzling display of color!");
        return true;
    }
    return false;
}

// SLIDE-FUNCTION
bool slideAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_DOWN) {
        printLine("You slide down and land in the Cellar.");
        // Move player to Cellar
        return true;
    }
    return false;
}

// SONGBIRD-F
bool songbirdAction() {
    // Already handled by canaryAction mostly
    return false;
}

// SOUTH-TEMPLE-FCN
void southTempleAction(int rarg) {
    // Stub
}

// STAIRS-F
bool stairsAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_CLIMB_UP || g.prsa == V_CLIMB_DOWN) {
        printLine("Use up or down instead.");
        return true;
    }
    return false;
}

// STILETTO-FUNCTION (Thief's knife)
bool stilettoAction() {
    auto& g = Globals::instance();
    ZObject* thief = g.getObject(ObjectIds::THIEF);
    if (thief && thief->getLocation() == g.here) {
        if (g.prsa == V_TAKE) {
            printLine("The thief deftly snatches the stiletto out of your reach.");
            return true;
        }
    }
    return false;
}

// TEETH-F
bool teethAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The teeth are razor sharp.");
        return true;
    }
    if (g.prsa == V_TAKE) {
        printLine("The teeth are embedded in something. You can't take them.");
        return true;
    }
    return false;
}

// TOOL-CHEST-FCN
bool toolChestAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN) {
        if (!g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            g.prso->setFlag(ObjectFlag::OPENBIT);
            printLine("Opening the tool chest reveals a wrench and a screwdriver.");
        } else {
            printLine("It's already open.");
        }
        return true;
    }
    return false;
}

// TORCH-ROOM-FCN
void torchRoomAction(int rarg) {
    // Stub
}

// TRAP-DOOR-FCN
bool trapDoorAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN) {
        if (!g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            g.prso->setFlag(ObjectFlag::OPENBIT);
            printLine("The trap door opens to reveal a dark stairway descending into darkness.");
        } else {
            printLine("It's already open.");
        }
        return true;
    }
    if (g.prsa == V_CLOSE) {
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            g.prso->clearFlag(ObjectFlag::OPENBIT);
            printLine("The trap door slams shut.");
        } else {
            printLine("It's already closed.");
        }
        return true;
    }
    return false;
}

// TREASURE-ROOM-FCN
void treasureRoomAction(int rarg) {
    // Handle thief's lair logic
}

// TROLL-ROOM-F
void trollRoomAction(int rarg) {
    // Handle troll blocking passage
}

// TRUNK-F is already defined above

// CANYON-VIEW-F
bool canyonViewAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE || g.prsa == V_LOOK) {
        printLine("You can see a deep canyon below.");
        return true;
    }
    return false;
}

// CHALICE-FCN
bool chaliceAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        // If thief is present, he might grab it first
        return false;
    }
    return false;
}

// BAT-F
bool batAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_ATTACK || g.prsa == V_KILL) {
        printLine("The bat flutters just out of reach.");
        return true;
    }
    return false;
}

// BELL-F (Normal bell, not hot)
bool bellAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_RING) {
        printLine("Ding, dong.");
        return true;
    }
    return false;
}

// BOARDED-WINDOW-FCN
bool boardedWindowAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN || g.prsa == V_ATTACK) { // V_BREAK not defined, using V_ATTACK
        printLine("The window is boarded up and cannot be opened.");
        return true;
    }
    return false;
}

// BODY-FUNCTION (Dead adventurer's body)
bool bodyAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The body is that of a previous adventurer, long dead.");
        return true;
    }
    if (g.prsa == V_TAKE) {
        printLine("A ghost appears and drives you away!");
        return true;
    }
    return false;
}

// KITCHEN-FCN - Kitchen room handler
// ZIL: Handles M-LOOK (description with window state) and M-BEG (climb stairs)
// Source: 1actions.zil lines 385-401
void kitchenAction(int rarg) {
    auto& g = Globals::instance();
    
    // M-LOOK: Print room description with window state
    if (rarg == 0) {  // M-LOOK equivalent
        print("You are in the kitchen of the white house. A table seems to "
              "have been used recently for the preparation of food. A passage "
              "leads to the west and a dark staircase can be seen leading "
              "upward. A dark chimney leads down and to the east is a small "
              "window which is ");
        
        // Check kitchen window OPENBIT
        ZObject* window = g.getObject(ObjectIds::KITCHEN_WINDOW);
        if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("open.");
        } else {
            printLine("slightly ajar.");
        }
    }
    
    // M-BEG: Handle CLIMB-UP STAIRS
    if (rarg == 1) {  // M-BEG equivalent
        ZObject* stairs = g.getObject(ObjectIds::STAIRS);
        if (g.prsa == V_CLIMB_UP && g.prso == stairs) {
            // Execute walk up (handled by caller triggering direction movement)
            Verbs::vWalkDir(Direction::UP);
        }
        if (g.prsa == V_CLIMB_DOWN && g.prso == stairs) {
            printLine("There are no stairs leading down.");
        }
    }
}
