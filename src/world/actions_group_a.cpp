#include "core/globals.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "core/io.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "systems/death.h"

// Helper for STUPID-CONTAINER logic (Bag of Coins, Trunk)
// Returns true if handled
// Helper for simple "stupid containers" that refuse insertion
// Updated to check containerId against PRSI
static bool stupidContainerAction(ObjectId containerId, const std::string& contentName) {
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
    
    // Logic: If putting X into THIS CONTAINER
    // ZIL: <AND <VERB? PUT> <EQUAL? ,PRSI .OBJ>>
    if (g.prsa == V_PUT || g.prsa == V_PUT_ON) { 
        if (g.prsi && g.prsi->getId() == containerId) {
             print("Don't be silly. It wouldn't be a ");
             print(g.prsi->getDesc()); // Use object name
             printLine(" anymore."); 
             return true;
        }
    }
    
    return false;
}

bool bagOfCoinsAction() {
    return stupidContainerAction(ObjectIds::BAG_OF_COINS, "coins");
}

bool trunkAction() {
    // FIXME: TRUNK object ID not defined yet. Commented out to fix compilation.
    // return stupidContainerAction(ObjectIds::TRUNK, "jewels");
    return false;
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

// BARROW-DOOR-FCN - Barrow door is too heavy to open/close
// ZIL: OPEN/CLOSE prints "The door is too heavy."
// Source: 1actions.zil lines 432-434
bool barrowDoorAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        printLine("The door is too heavy.");
        return true;
    }
    return false;
}

// BARROW-FCN - Stone barrow entrance, THROUGH goes west
// ZIL: <COND (<VERB? THROUGH> <DO-WALK ,P?WEST>)>
// Source: 1actions.zil lines 436-438
bool barrowAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_THROUGH || g.prsa == V_ENTER) {
        Verbs::vWalkDir(Direction::WEST);
        return true;
    }
    return false;
}

// CELLAR-FCN (Room action for Cellar)
// CELLAR-FCN - Cellar room handler
// ZIL: M-LOOK prints desc. M-ENTER slams TRAP-DOOR if open/untouched.
// Source: 1actions.zil lines 531-543
void cellarAction(int rarg) {
    auto& g = Globals::instance();

    if (rarg == M_LOOK) {
        printLine("You are in a dark and damp cellar with a narrow passageway leading north, and a crawlway to the south. On the west is the bottom of a steep metal ramp which is unclimbable.");
    }
    else if (rarg == M_ENTER) {
        ZObject* trapdoor = g.getObject(ObjectIds::TRAP_DOOR);
        if (trapdoor && trapdoor->hasFlag(ObjectFlag::OPENBIT) && !trapdoor->hasFlag(ObjectFlag::TOUCHBIT)) {
            trapdoor->unsetFlag(ObjectFlag::OPENBIT);
            trapdoor->setFlag(ObjectFlag::TOUCHBIT);
            printLine("The trap door crashes shut, and you hear someone barring it.");
        }
    }
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

// LEAK-FUNCTION - Dam leak repair with putty
// ZIL: PUT putty on leak or PLUG with putty fixes the dam
// Source: 1actions.zil lines 1362-1377
static int waterLevel = 1;  // >0 means dam is leaking

bool leakAction() {
    auto& g = Globals::instance();
    
    // Only active if WATER-LEVEL > 0 (dam is leaking)
    if (waterLevel <= 0) {
        return false;
    }
    
    // PUT putty on leak
    if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && 
        g.prso && g.prso->getId() == ObjectIds::PUTTY) {
        // FIX-MAINT-LEAK: repair the dam
        waterLevel = -1;
        printLine("By some miracle of Zorkian technology, you have managed to stop the leak in the dam.");
        return true;
    }
    
    // PLUG with putty
    if (g.prsa == V_PLUG) {
        if (g.prsi && g.prsi->getId() == ObjectIds::PUTTY) {
            // FIX-MAINT-LEAK: repair the dam
            waterLevel = -1;
            printLine("By some miracle of Zorkian technology, you have managed to stop the leak in the dam.");
            return true;
        } else {
            print("With ");
            if (g.prsi) print(g.prsi->getDesc());
            else print("that");
            printLine("? You must be joking.");
            return true;
        }
    }
    
    return false;
}

// LIVING-ROOM-FCN - Living room handler with dynamic description
// ZIL: M-LOOK shows door/trophy/rug/trap door state, M-END updates score
// Source: 1actions.zil lines 449-485
static bool rugMoved = false;  // RUG-MOVED flag
static bool magicFlag = false; // MAGIC-FLAG (cyclops door opened)

void livingRoomAction(int rarg) {
    auto& g = Globals::instance();
    
    // M-LOOK: Dynamic room description
    if (rarg == 0) {
        print("You are in the living room. There is a doorway to the east");
        
        // Check door state (magic cyclops door vs nailed shut)
        if (magicFlag) {
            print(". To the west is a cyclops-shaped opening in an old wooden door, "
                  "above which is some strange gothic lettering, ");
        } else {
            print(", a wooden door with strange gothic lettering to the west, "
                  "which appears to be nailed shut, ");
        }
        
        print("a trophy case, ");
        
        // Check rug/trap door state
        ZObject* trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
        bool trapOpen = trapDoor && trapDoor->hasFlag(ObjectFlag::OPENBIT);
        
        if (rugMoved && trapOpen) {
            printLine("and a rug lying beside an open trap door.");
        } else if (rugMoved) {
            printLine("and a closed trap door at your feet.");
        } else if (trapOpen) {
            printLine("and an open trap door at your feet.");
        } else {
            printLine("and a large oriental rug in the center of the room.");
        }
    }
    
    // M-END: Update score when touching trophy case
    if (rarg == 2) {
        if (g.prsa == V_TAKE || (g.prsa == V_PUT && g.prsi && 
            g.prsi->getId() == ObjectIds::TROPHY_CASE)) {
            // Score update handled by score system
        }
    }
}

// LOUD-ROOM-FCN - Loud room echo puzzle handler
// ZIL: ECHO command sets LOUD-FLAG and clears BAR SACREDBIT, making bar takeable
// Source: 1actions.zil lines 1660-1728
static bool loudFlag = false;  // LOUD-FLAG - room has been quieted
extern bool damGatesOpen;      // From actions.cpp - dam gates state

void loudRoomAction(int rarg) {
    auto& g = Globals::instance();
    
    // M-LOOK: Dynamic room description
    if (rarg == 0) {
        print("This is a large room with a ceiling which cannot be detected from "
              "the ground. There is a narrow passage from east to west and a stone "
              "stairway leading upward.");
        
        // Check if room is quiet or loud
        if (loudFlag || !damGatesOpen) {
            printLine(" The room is eerie in its quietness.");
        } else {
            printLine(" The room is deafeningly loud with an undetermined rushing sound. "
                      "The sound seems to reverberate from all of the walls, making it "
                      "difficult even to think.");
        }
    }
    
    // M-END: Eject player if room is too loud
    if (rarg == 2 && damGatesOpen && !loudFlag) {
        printLine("It is unbearably loud here, with an ear-splitting roar seeming to "
                  "come from all around you. There is a pounding in your head which won't "
                  "stop. With a tremendous effort, you scramble out of the room.");
        // Player gets ejected to random adjacent room (simplified: go west)
        Verbs::vWalkDir(Direction::WEST);
    }
}

// Handle ECHO command in loud room (called from verb handler)
bool handleEchoInLoudRoom() {
    auto& g = Globals::instance();
    
    // Only works in loud room when it's loud
    if (g.here && g.here->getId() != RoomIds::LOUD_ROOM) {
        return false;
    }
    
    if (!loudFlag && damGatesOpen) {
        // Saying ECHO quiets the room and makes platinum bar takeable
        loudFlag = true;
        
        // Clear SACREDBIT on platinum bar (makes it takeable by thief too)
        ZObject* bar = g.getObject(ObjectIds::BAR);
        if (bar) {
            bar->clearFlag(ObjectFlag::SACREDBIT);
        }
        
        printLine("The acoustics of the room change subtly.");
        return true;
    }
    
    // Room is already quiet - just echo
    printLine("Echo...");
    return true;
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
    return stupidContainerAction(ObjectIds::SANDWICH_BAG, "food");
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

// TEETH-F - Teeth object action handler
// ZIL Source: 1actions.zil lines 48-62
// Handles BRUSH verb with putty death, and EXAMINE/TAKE
bool teethAction() {
    auto& g = Globals::instance();
    
    // Handle BRUSH TEETH - ZIL checks prso is TEETH
    if (g.prsa == V_BRUSH && g.prso && g.prso->getId() == ObjectIds::TEETH) {
        // Check if brushing WITH PUTTY = death
        // ZIL: <AND <EQUAL? ,PRSI ,PUTTY> <IN? ,PRSI ,WINNER>>
        if (g.prsi && g.prsi->getId() == ObjectIds::PUTTY && 
            g.prsi->getLocation() == g.player) {
            DeathSystem::jigsUp(
                "Well, you seem to have been brushing your teeth with some sort of "
                "glue. As a result, your mouth gets glued together (with your nose) "
                "and you die of respiratory failure.",
                DeathSystem::DeathCause::OTHER);
            return true;
        }
        // BRUSH TEETH with nothing specified
        if (!g.prsi) {
            printLine("Dental hygiene is highly recommended, but I'm not sure what you want to brush them with.");
            return true;
        }
        // BRUSH TEETH with something else
        printLine("A nice idea, but with a " + g.prsi->getDesc() + "?");
        return true;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("The teeth are razor sharp.");
        return true;
    }
    
    // Handle TAKE - scenery can't be taken
    if (g.prsa == V_TAKE) {
        printLine("The teeth are embedded in something. You can't take them.");
        return true;
    }
    
    return false;
}



// TOOL-CHEST-FCN
bool toolChestAction() {
    auto& g = Globals::instance();
    
    // ZIL: EXAMINE -> "The chests are all empty."
    // ZIL: TAKE/OPEN/PUT -> Remove object, print "The chests are so rusty..."
    // Source: 1actions.zil lines 1332-1340
    
    if (g.prsa == V_EXAMINE) {
        printLine("The chests are all empty.");
        return true;
    }
    
    if (g.prsa == V_TAKE || g.prsa == V_OPEN || g.prsa == V_PUT) {
        printLine("The chests are so rusty and corroded that they crumble when you touch them.");
        g.prso->moveTo(nullptr); // REMOVE-CAREFULLY
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

// BAT-F - Bat attack/defense, garlic prevents being grabbed
// ZIL: TAKE/ATTACK - if garlic present "can't reach him", else fly-me teleports player
// Source: 1actions.zil lines 308-324
// BAT-DROPS table
static const std::vector<RoomId> BAT_DROPS = {
    RoomIds::MINE_1,
    RoomIds::MINE_2,
    RoomIds::MINE_3,
    RoomIds::MINE_4,
    RoomIds::LADDER_TOP,
    RoomIds::LADDER_BOTTOM,
    RoomIds::SQUEEKY_ROOM,
    RoomIds::COAL_MINE_1
};

// Helper for Fweep printing
static void fweep(int n) {
    for (int i = 0; i < n; ++i) {
        printLine("    Fweep!");
    }
    // ZIL CRLF is implicit in printLine usually, but ZIL does <CRLF> at end of FWEEP logic
    // We'll just stick to printLine for each fweep.
}

// FLY-ME logic
static void flyMe() {
    auto& g = Globals::instance();
    fweep(4);
    printLine(""); // CR CR in ZIL usually
    printLine("The bat grabs you by the scruff of your neck and lifts you away....");
    printLine("");

    // <GOTO <PICK-ONE ,BAT-DROPS> <>>
    if (!BAT_DROPS.empty()) {
        int idx = std::rand() % BAT_DROPS.size();
        RoomId targetId = BAT_DROPS[idx];
        ZObject* target = g.getObject(targetId);
        if (target) {
            bool moved = g.player->moveTo(target);
            if (moved) {
                // <COND (<NOT <EQUAL? ,HERE ,ENTRANCE-TO-HADES>> <V-FIRST-LOOK>)>
                // We assume V-FIRST-LOOK means describe room.
                // In C++, moveTo updates 'here'. 
                if (target->getId() != RoomIds::ENTRANCE_TO_HADES) {
                     // Force look/describe
                     // We don't have direct access to V-FIRST-LOOK, but room description is handled by main loop usually?
                     // If this is an action, we should probably print the room desc.
                     // But for now, let's assume the game loop picks it up or we call lookAction?
                     // Actions usually return true/false. If we changed room, the main loop should describe.
                     // Wait, V-FIRST-LOOK is specific.
                     // For fidelity, we should try to trigger room description.
                     // Note: We don't have a Look function we can call easily here without circular dep?
                     // Actually, we can just return true. Main loop in main.cpp checks 'here' change?
                     // In vibezork, 'performAction' is called. If we return true, it implies handled.
                     // We should verify if looking happens automatically on room change.
                     // In main.cpp: if (g.here != oldHere) describeRoom();
                }
            }
        }
    }
}

// BAT-F
bool batAction() {
    auto& g = Globals::instance();
    
    // <VERB? TELL> -> FWEEP 6
    if (g.prsa == V_TELL) {
        fweep(6);
        return true; 
        // ZIL: <SETG P-CONT <>> (Stop parsing?)
        // In this engine, returning true stops further processing.
    }

    // Handle TAKE, ATTACK, MUNG
    if (g.prsa == V_TAKE || g.prsa == V_ATTACK || g.prsa == V_KILL) {
        // Check for garlic - protects player from bat
        // ZIL: <EQUAL? <LOC ,GARLIC> ,WINNER ,HERE>
        ZObject* garlic = g.getObject(ObjectIds::GARLIC);
        bool hasGarlic = false;
        if (garlic) {
            hasGarlic = (garlic->getLocation() == g.player || 
                        garlic->getLocation() == g.here);
        }
        
        if (hasGarlic) {
            printLine("You can't reach him; he's on the ceiling.");
        } else {
            // FLY-ME
            flyMe();
        }
        return true;
    }
    
    return false;
}

// BELL-F (Normal bell, not hot)
bool bellAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_RING) {
        // ZIL: <COND (<AND <EQUAL? ,HERE ,LLD-ROOM> <NOT ,LLD-FLAG>> <RFALSE>) ...>
        if (g.here && g.here->getId() == ObjectIds::LAND_OF_LIVING_DEAD && !g.lldFlag) {
            return false;
        }
        
        printLine("Ding, dong.");
        return true;
    }
    return false;
}

// BOARDED-WINDOW-FCN - Boarded window blocking
// ZIL: OPEN = "windows are boarded", MUNG = "can't break windows open"
// Source: 1actions.zil lines 370-374
bool boardedWindowAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_OPEN) {
        printLine("The windows are boarded and can't be opened.");
        return true;
    }
    if (g.prsa == V_ATTACK || g.prsa == V_KILL || g.prsa == V_MUNG) { // MUNG = break
        printLine("You can't break the windows open.");
        return true;
    }
    return false;
}

// BODY-FUNCTION - Dead adventurer bodies
// ZIL: TAKE = "force keeps you", MUNG/BURN = death
// Source: 1actions.zil lines 2178-2185
bool bodyAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("A force keeps you from taking the bodies.");
        return true;
    }
    if (g.prsa == V_ATTACK || g.prsa == V_BURN) {
        // Death for disrespecting the bodies
        printLine("The voice of the guardian of the dungeon booms out from the darkness, "
                  "\"Your disrespect costs you your life!\" and places your head on a sharp pole.");
        DeathSystem::jigsUp("", DeathSystem::DeathCause::OTHER);
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

// BODY-FUNCTION - Dead body interactions
// ZIL: TAKE = "A force keeps you...", MUNG/BURN = Death
// source: 1dungeon.zil line 694, 1actions.zil line 2178
bool bodyAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("A force keeps you from taking the bodies.");
        return true;
    }
    if (g.prsa == V_ATTACK || g.prsa == V_KILL || g.prsa == V_MUNG || g.prsa == V_BURN) {
        DeathSystem::jigsUp("The voice of the guardian of the dungeon booms out from the darkness, \"Your disrespect costs you your life!\" and places your head on a sharp pole.");
        return true;
    }
    return false;
}
