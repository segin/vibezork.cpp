# Zork C++ Developer Guide

This guide provides comprehensive documentation for developers working on the Zork I C++ port. It covers ZIL to C++ translation patterns, architecture decisions, and best practices.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [ZIL to C++ Translation Guide](#zil-to-c-translation-guide)
3. [Core Systems](#core-systems)
4. [Adding Content](#adding-content)
5. [Common Patterns](#common-patterns)
6. [Testing](#testing)
7. [Debugging](#debugging)

---

## Architecture Overview

### System Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         Main Loop                            │
│  (Read input → Parse → Execute verb → Update state → Output) │
└────────────┬────────────────────────────────────────────────┘
             │
    ┌────────┴────────┐
    │                 │
┌───▼────┐      ┌────▼─────┐
│ Parser │      │  Verbs   │
│ System │      │  System  │
└───┬────┘      └────┬─────┘
    │                │
    │    ┌───────────▼──────────┐
    │    │   Core Engine        │
    │    │  - Object System     │
    └────►  - Global State      │
         │  - I/O System        │
         └───────────┬──────────┘
                     │
         ┌───────────┴──────────┐
         │                      │
    ┌────▼─────┐         ┌─────▼────┐
    │  World   │         │  Game    │
    │  System  │         │ Systems  │
    │          │         │          │
    │ - Rooms  │         │ - Timers │
    │ - Objects│         │ - Score  │
    │ - Actions│         │ - Combat │
    └──────────┘         │ - Light  │
                         └──────────┘
```

### Directory Structure

```
src/
├── core/           # Core engine
│   ├── object.h/cpp    # ZObject and ZRoom classes
│   ├── globals.h/cpp   # Global state singleton
│   ├── io.h/cpp        # Input/output functions
│   ├── flags.h         # ObjectFlag enumeration
│   └── types.h         # Type definitions (ObjectId, VerbId, etc.)
├── parser/         # Command parsing
│   ├── parser.h/cpp    # Main parser class
│   ├── syntax.h/cpp    # Syntax pattern matching
│   └── verb_registry.h/cpp  # Verb synonym mapping
├── verbs/          # Verb implementations
│   └── verbs.h/cpp     # All verb handlers
├── world/          # Game world
│   ├── rooms.h/cpp     # Room definitions and ZRoom class
│   ├── objects.h       # Object ID constants
│   ├── world.h/cpp     # World initialization
│   └── missing_objects.cpp  # Additional objects
├── systems/        # Game systems
│   ├── timer.h/cpp     # Timer/interrupt system
│   ├── combat.h/cpp    # Combat system
│   ├── light.h/cpp     # Light/darkness system
│   ├── score.h/cpp     # Scoring system
│   ├── save.h/cpp      # Save/restore system
│   ├── lamp.h/cpp      # Lamp battery management
│   ├── candle.h/cpp    # Candle burning
│   ├── sword.h/cpp     # Sword glow system
│   ├── npc.h/cpp       # NPC behavior (thief, troll, cyclops)
│   └── death.h/cpp     # Death and resurrection
└── main.cpp        # Entry point and main loop
```

---

## ZIL to C++ Translation Guide

### Complete Mapping Table

| ZIL Construct | C++ Equivalent | Example |
|---------------|----------------|---------|
| `<OBJECT>` | `ZObject` instance | `auto obj = std::make_unique<ZObject>(id, "desc");` |
| `<ROOM>` | `ZRoom` instance | `auto room = std::make_unique<ZRoom>(id, "name", "desc");` |
| `<ROUTINE>` | C++ function | `bool myRoutine() { ... }` |
| `<GLOBAL>` | `Globals` member | `g.score`, `g.here`, `g.prso` |
| `<FSET>` | `setFlag()` | `obj->setFlag(ObjectFlag::TAKEBIT);` |
| `<FCLEAR>` | `clearFlag()` | `obj->clearFlag(ObjectFlag::OPENBIT);` |
| `<FSET?>` | `hasFlag()` | `if (obj->hasFlag(ObjectFlag::CONTBIT))` |
| `<GETP>` | `getProperty()` | `int size = obj->getProperty(P_SIZE);` |
| `<PUTP>` | `setProperty()` | `obj->setProperty(P_CAPACITY, 10);` |
| `<MOVE>` | `moveTo()` | `obj->moveTo(room);` |
| `<LOC>` | `getLocation()` | `auto loc = obj->getLocation();` |
| `<FIRST?>` | `getContents()` | `for (auto* item : container->getContents())` |
| `<IN?>` | Check location | `if (obj->getLocation() == container)` |
| `<TELL>` | `print()`/`printLine()` | `printLine("Hello");` |
| `<CRLF>` | `crlf()` | `crlf();` |
| `<COND>` | `if`/`else if` | `if (cond1) { } else if (cond2) { }` |
| `<AND>` | `&&` | `if (a && b)` |
| `<OR>` | `\|\|` | `if (a \|\| b)` |
| `<NOT>` | `!` | `if (!flag)` |
| `<EQUAL?>` | `==` | `if (obj->getId() == OBJ_LAMP)` |
| `<VERB?>` | Check `g.prsa` | `if (g.prsa == V_TAKE)` |
| `<PRSO>` | `g.prso` | `auto* obj = g.prso;` |
| `<PRSI>` | `g.prsi` | `auto* indirect = g.prsi;` |
| `<HERE>` | `g.here` | `auto* room = g.here;` |
| `<WINNER>` | `g.winner` | `auto* player = g.winner;` |
| `<RTRUE>` | `return true;` | `return true;` |
| `<RFALSE>` | `return false;` | `return false;` |
| `<RANDOM>` | `rand()` | `int r = rand() % 100;` |
| `<QUEUE>` | Timer registration | `TimerSystem::registerTimer(...)` |
| `<DEQUEUE>` | Timer removal | `TimerSystem::removeTimer(...)` |

### Objects

**ZIL:**
```lisp
<OBJECT MAILBOX
    (IN WEST-OF-HOUSE)
    (SYNONYM MAILBOX BOX)
    (ADJECTIVE SMALL)
    (DESC "small mailbox")
    (FLAGS CONTBIT TRYTAKEBIT)
    (CAPACITY 10)
    (ACTION MAILBOX-F)>
```

**C++:**
```cpp
auto mailbox = std::make_unique<ZObject>(OBJ_MAILBOX, "small mailbox");
mailbox->addSynonym("mailbox");
mailbox->addSynonym("box");
mailbox->addAdjective("small");
mailbox->setFlag(ObjectFlag::CONTBIT);
mailbox->setFlag(ObjectFlag::TRYTAKEBIT);
mailbox->setProperty(P_CAPACITY, 10);
mailbox->setAction(mailboxAction);
g.registerObject(OBJ_MAILBOX, std::move(mailbox));

// Place in room after registration
g.getObject(OBJ_MAILBOX)->moveTo(g.getObject(RoomIds::WEST_OF_HOUSE));
```

### Rooms

**ZIL:**
```lisp
<ROOM WEST-OF-HOUSE
    (IN ROOMS)
    (DESC "West of House")
    (LDESC "You are standing in an open field west of a white house...")
    (NORTH TO NORTH-OF-HOUSE)
    (SOUTH TO SOUTH-OF-HOUSE)
    (EAST "The door is boarded and you can't remove the boards.")
    (WEST TO FOREST-1)
    (ACTION WEST-HOUSE)
    (FLAGS RLANDBIT ONBIT SACREDBIT)>
```

**C++:**
```cpp
auto westOfHouse = std::make_unique<ZRoom>(
    RoomIds::WEST_OF_HOUSE,
    "West of House",
    "You are standing in an open field west of a white house..."
);
westOfHouse->setFlag(ObjectFlag::RLANDBIT);
westOfHouse->setFlag(ObjectFlag::ONBIT);
westOfHouse->setFlag(ObjectFlag::SACREDBIT);
westOfHouse->setRoomAction(westHouseAction);

// Set exits
westOfHouse->setExit(Direction::NORTH, RoomExit(RoomIds::NORTH_OF_HOUSE));
westOfHouse->setExit(Direction::SOUTH, RoomExit(RoomIds::SOUTH_OF_HOUSE));
westOfHouse->setExit(Direction::EAST, RoomExit("The door is boarded and you can't remove the boards."));
westOfHouse->setExit(Direction::WEST, RoomExit(RoomIds::FOREST_1));

g.registerObject(RoomIds::WEST_OF_HOUSE, std::move(westOfHouse));
```

### Room Actions

**ZIL:**
```lisp
<ROUTINE WEST-HOUSE (RARG)
    <COND (<EQUAL? .RARG ,M-LOOK>
           <TELL "You are standing in an open field west of a white house, with a boarded front door." CR>
           <COND (<FSET? ,WINDOW ,OPENBIT>
                  <TELL "There is a small open window on the west side of the house." CR>)
                 (T
                  <TELL "There is a small window on the west side of the house." CR>)>)>>
```

**C++:**
```cpp
void westHouseAction(int rarg) {
    auto& g = Globals::instance();
    
    if (rarg == M_LOOK) {
        printLine("You are standing in an open field west of a white house, with a boarded front door.");
        
        auto* window = g.getObject(OBJ_WINDOW);
        if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("There is a small open window on the west side of the house.");
        } else {
            printLine("There is a small window on the west side of the house.");
        }
    }
}
```

### Object Actions

**ZIL:**
```lisp
<ROUTINE MAILBOX-F ()
    <COND (<VERB? TAKE>
           <TELL "It is securely anchored." CR>
           <RTRUE>)
          (<VERB? OPEN>
           <COND (<FSET? ,MAILBOX ,OPENBIT>
                  <TELL "It's already open." CR>)
                 (T
                  <FSET ,MAILBOX ,OPENBIT>
                  <TELL "Opening the small mailbox reveals a leaflet." CR>)>
           <RTRUE>)>>
```

**C++:**
```cpp
bool mailboxAction() {
    auto& g = Globals::instance();
    auto* mailbox = g.getObject(OBJ_MAILBOX);
    
    if (g.prsa == V_TAKE) {
        printLine("It is securely anchored.");
        return true;
    }
    
    if (g.prsa == V_OPEN) {
        if (mailbox->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("It's already open.");
        } else {
            mailbox->setFlag(ObjectFlag::OPENBIT);
            printLine("Opening the small mailbox reveals a leaflet.");
        }
        return true;
    }
    
    return false;  // Not handled, use default behavior
}
```

### Verb Handlers

**ZIL:**
```lisp
<ROUTINE V-TAKE ()
    <COND (<NOT ,PRSO>
           <TELL "What do you want to take?" CR>
           <RFALSE>)
          (<NOT <FSET? ,PRSO ,TAKEBIT>>
           <TELL "You can't take that." CR>
           <RFALSE>)
          (T
           <MOVE ,PRSO ,WINNER>
           <TELL "Taken." CR>
           <RTRUE>)>>
```

**C++:**
```cpp
bool Verbs::vTake() {
    auto& g = Globals::instance();
    
    if (!g.prso) {
        printLine("What do you want to take?");
        return false;
    }
    
    // Let object action handle it first
    if (g.prso->performAction()) {
        return true;
    }
    
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        printLine("You can't take that.");
        return false;
    }
    
    g.prso->moveTo(g.winner);
    printLine("Taken.");
    return true;
}
```

---

## Core Systems

### Global State

Access global state through the singleton:

```cpp
auto& g = Globals::instance();

// Current location
ZRoom* room = dynamic_cast<ZRoom*>(g.here);

// Current actor (player)
ZObject* player = g.winner;

// Parser results
VerbId verb = g.prsa;      // Current verb
ZObject* direct = g.prso;   // Direct object
ZObject* indirect = g.prsi; // Indirect object

// Game state
int score = g.score;
int moves = g.moves;
bool isLit = g.lit;
```

### Object System

```cpp
// Create object
auto lamp = std::make_unique<ZObject>(OBJ_LAMP, "brass lantern");
lamp->addSynonym("lamp");
lamp->addSynonym("lantern");
lamp->addAdjective("brass");
lamp->setFlag(ObjectFlag::TAKEBIT);
lamp->setFlag(ObjectFlag::LIGHTBIT);
lamp->setProperty(P_SIZE, 15);

// Register and place
g.registerObject(OBJ_LAMP, std::move(lamp));
g.getObject(OBJ_LAMP)->moveTo(g.getObject(RoomIds::LIVING_ROOM));

// Check flags
if (obj->hasFlag(ObjectFlag::TAKEBIT)) { ... }

// Modify flags
obj->setFlag(ObjectFlag::ONBIT);
obj->clearFlag(ObjectFlag::OPENBIT);

// Properties
int capacity = obj->getProperty(P_CAPACITY);
obj->setProperty(P_VALUE, 10);

// Containment
obj->moveTo(container);
for (auto* item : container->getContents()) { ... }
```

### Timer System

```cpp
// Register a timer
TimerSystem::registerTimer("lamp", 200, []() {
    // Called every turn while lamp is on
    LampSystem::tick();
});

// Enable/disable
TimerSystem::enableTimer("lamp");
TimerSystem::disableTimer("lamp");

// Process all timers (called from main loop)
TimerSystem::tick();
```

### Combat System

```cpp
// Start combat
CombatSystem::startCombat(enemy);

// Process combat round
CombatSystem::processCombatRound();

// Check combat state
if (CombatSystem::isInCombat()) { ... }

// End combat
CombatSystem::endCombat();
```

---

## Adding Content

### Adding a New Room

1. **Add ID to `world/rooms.h`:**
```cpp
namespace RoomIds {
    constexpr ObjectId MY_NEW_ROOM = 1200;
}
```

2. **Create room in `world/world.cpp`:**
```cpp
void createMyNewRoom(Globals& g) {
    auto room = std::make_unique<ZRoom>(
        RoomIds::MY_NEW_ROOM,
        "My New Room",
        "This is a detailed description of the room."
    );
    
    // Set flags
    room->setFlag(ObjectFlag::RLANDBIT);
    
    // Set exits
    room->setExit(Direction::NORTH, RoomExit(RoomIds::SOME_OTHER_ROOM));
    room->setExit(Direction::SOUTH, RoomExit("You can't go that way."));
    
    // Set room action if needed
    room->setRoomAction(myNewRoomAction);
    
    g.registerObject(RoomIds::MY_NEW_ROOM, std::move(room));
}
```

3. **Connect from other rooms:**
```cpp
// In the other room's creation
otherRoom->setExit(Direction::SOUTH, RoomExit(RoomIds::MY_NEW_ROOM));
```

### Adding a New Object

1. **Add ID to `world/objects.h`:**
```cpp
namespace ObjectIds {
    constexpr ObjectId MY_NEW_OBJECT = 2100;
}
```

2. **Create object in `world/world.cpp`:**
```cpp
void createMyNewObject(Globals& g) {
    auto obj = std::make_unique<ZObject>(ObjectIds::MY_NEW_OBJECT, "shiny widget");
    
    // Synonyms and adjectives
    obj->addSynonym("widget");
    obj->addSynonym("thing");
    obj->addAdjective("shiny");
    
    // Flags
    obj->setFlag(ObjectFlag::TAKEBIT);
    
    // Properties
    obj->setProperty(P_SIZE, 5);
    obj->setProperty(P_VALUE, 10);  // If it's a treasure
    
    // Action handler
    obj->setAction(myNewObjectAction);
    
    // Register
    g.registerObject(ObjectIds::MY_NEW_OBJECT, std::move(obj));
    
    // Place in world
    g.getObject(ObjectIds::MY_NEW_OBJECT)->moveTo(g.getObject(RoomIds::SOME_ROOM));
}
```

### Adding a New Verb

1. **Add ID to `verbs/verbs.h`:**
```cpp
constexpr VerbId V_MY_VERB = 200;

namespace Verbs {
    bool vMyVerb();
}
```

2. **Implement handler in `verbs/verbs.cpp`:**
```cpp
bool Verbs::vMyVerb() {
    auto& g = Globals::instance();
    
    if (!g.prso) {
        printLine("What do you want to my-verb?");
        return false;
    }
    
    // Let object action handle it first
    if (g.prso->performAction()) {
        return true;
    }
    
    // Default behavior
    printLine("Nothing happens.");
    return true;
}
```

3. **Register in `main.cpp`:**
```cpp
verbHandlers[V_MY_VERB] = Verbs::vMyVerb;
```

4. **Add synonyms in `parser/verb_registry.cpp`:**
```cpp
registry.registerVerb(V_MY_VERB, {"myverb", "mv"});
```

---

## Common Patterns

### Checking Multiple Verbs

```cpp
// ZIL: <VERB? TAKE DROP PUT>
if (g.prsa == V_TAKE || g.prsa == V_DROP || g.prsa == V_PUT) {
    // Handle these verbs
}
```

### Checking Object Identity

```cpp
// ZIL: <EQUAL? ,PRSO ,LAMP ,CANDLES>
if (g.prso && (g.prso->getId() == OBJ_LAMP || g.prso->getId() == OBJ_CANDLES)) {
    // Handle lamp or candles
}
```

### Checking Object Location

```cpp
// ZIL: <IN? ,LAMP ,WINNER>
if (g.getObject(OBJ_LAMP)->getLocation() == g.winner) {
    // Lamp is in inventory
}

// ZIL: <IN? ,LAMP ,HERE>
if (g.getObject(OBJ_LAMP)->getLocation() == g.here) {
    // Lamp is in current room
}
```

### Conditional Exits

```cpp
// Door exit (checks if door is open)
room->setExit(Direction::NORTH, RoomExit::createDoor(
    RoomIds::TARGET_ROOM, 
    OBJ_DOOR
));

// Conditional exit (custom condition)
room->setExit(Direction::DOWN, RoomExit::createConditional(
    RoomIds::CELLAR,
    []() {
        auto& g = Globals::instance();
        auto* trapdoor = g.getObject(OBJ_TRAPDOOR);
        return trapdoor && trapdoor->hasFlag(ObjectFlag::OPENBIT);
    },
    "The trap door is closed."
));
```

### Output with Object Descriptions

```cpp
// ZIL: <TELL "You see " D ,OBJ " here." CR>
print("You see ");
print(obj->getDesc());
printLine(" here.");

// Or using string concatenation
printLine("You see " + obj->getDesc() + " here.");
```

---

## Testing

### Unit Test Example

```cpp
TEST(ObjectTest, FlagOperations) {
    ZObject obj(1, "test object");
    
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
    obj.setFlag(ObjectFlag::TAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    obj.clearFlag(ObjectFlag::TAKEBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
}
```

### Integration Test Example

```cpp
TEST(GameTest, TakeAndDrop) {
    initializeWorld();
    auto& g = Globals::instance();
    
    // Move to room with lamp
    g.here = g.getObject(RoomIds::LIVING_ROOM);
    
    // Take the lamp
    g.prso = g.getObject(OBJ_LAMP);
    g.prsa = V_TAKE;
    Verbs::vTake();
    
    // Verify lamp is in inventory
    ASSERT_EQ(g.getObject(OBJ_LAMP)->getLocation(), g.winner);
}
```

### Transcript Test Example

```cpp
TEST(TranscriptTest, OpenMailbox) {
    GameTester tester;
    tester.init();
    
    auto result = tester.execute("open mailbox");
    ASSERT_TRUE(result.contains("Opening"));
    ASSERT_TRUE(result.contains("leaflet"));
}
```

---

## Debugging

### Print Current State

```cpp
void debugState() {
    auto& g = Globals::instance();
    
    std::cerr << "=== Debug State ===" << std::endl;
    std::cerr << "Location: " << (g.here ? g.here->getDesc() : "null") << std::endl;
    std::cerr << "Verb: " << g.prsa << std::endl;
    std::cerr << "PRSO: " << (g.prso ? g.prso->getDesc() : "null") << std::endl;
    std::cerr << "PRSI: " << (g.prsi ? g.prsi->getDesc() : "null") << std::endl;
    std::cerr << "Score: " << g.score << std::endl;
    std::cerr << "Moves: " << g.moves << std::endl;
}
```

### Common Issues

1. **Object not found**: Check that the object ID is registered with `g.registerObject()`
2. **Action not called**: Verify the action is set with `obj->setAction()`
3. **Exit not working**: Check that both rooms have reciprocal exits
4. **Flag not persisting**: Ensure you're modifying the registered object, not a copy
5. **Parser not recognizing word**: Add synonyms to the verb registry or object

### Useful Debug Commands

Add these to your game for debugging:

```cpp
// In main.cpp, add to verbHandlers:
verbHandlers[V_DEBUG] = []() {
    debugState();
    return true;
};
```

---

## Resources

- **Spec**: `.kiro/specs/zork-complete-port/` - Full specification
- **Original ZIL**: `zil/` directory - Original source files
- **Tests**: `tests/` directory - Test examples
- **Design**: `.kiro/specs/zork-complete-port/design.md` - Architecture details
