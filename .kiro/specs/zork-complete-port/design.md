# Design Document

## Overview

This document describes the technical design for the complete Zork I C++ port. The design maintains exact fidelity to the original ZIL implementation while leveraging modern C++17 features for clean, maintainable code. The architecture is organized into distinct subsystems that mirror the original ZIL file structure while using object-oriented design patterns appropriate for C++.

### Design Principles

1. **Exact Fidelity**: All game behavior must match the original Zork I exactly
2. **Modern C++**: Use C++17 features (smart pointers, std::optional, structured bindings, etc.)
3. **Clean Architecture**: Clear separation of concerns with well-defined interfaces
4. **Testability**: Design for easy unit and integration testing
5. **Performance**: Instant response times (<10ms per command)
6. **Maintainability**: Clear code organization and naming conventions

### High-Level Architecture

The system is organized into six major subsystems:

1. **Core Engine**: Object system, global state, I/O
2. **Parser**: Command parsing and interpretation
3. **World**: Rooms, objects, and game world data
4. **Verbs**: Command implementations
5. **Actions**: Object and room-specific behaviors
6. **Game Systems**: Timers, scoring, combat, light/darkness

## Architecture

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

### Data Flow

1. **Input**: Player types command
2. **Tokenization**: Parser splits input into words
3. **Verb Recognition**: Parser identifies verb and synonyms
4. **Object Recognition**: Parser identifies PRSO and PRSI
5. **Syntax Validation**: Parser validates command structure
6. **Pre-Verb Checks**: Validate action is possible
7. **Verb Execution**: Execute verb handler
8. **Action Handlers**: Invoke object/room-specific actions
9. **State Update**: Update game state (flags, properties, location)
10. **Timer Processing**: Process any scheduled events
11. **Output**: Display results to player


## Components and Interfaces

### Core Engine

#### ZObject Class

The fundamental building block representing all game entities.

```cpp
class ZObject {
public:
    ZObject(ObjectId id, std::string_view desc);
    virtual ~ZObject() = default;
    
    // Identity
    ObjectId getId() const;
    std::string_view getDesc() const;
    
    // Flags
    void setFlag(ObjectFlag flag);
    void clearFlag(ObjectFlag flag);
    bool hasFlag(ObjectFlag flag) const;
    
    // Properties
    void setProperty(PropertyId prop, int value);
    std::optional<int> getProperty(PropertyId prop) const;
    
    // Containment
    void moveTo(ZObject* newLocation);
    ZObject* getLocation() const;
    const std::vector<ZObject*>& getContents() const;
    
    // Synonyms and adjectives
    void addSynonym(std::string_view syn);
    void addAdjective(std::string_view adj);
    bool hasSynonym(std::string_view word) const;
    bool hasAdjective(std::string_view word) const;
    
    // Actions
    using ActionFunc = std::function<bool()>;
    void setAction(ActionFunc action);
    bool performAction();
    
private:
    ObjectId id_;
    std::string desc_;
    std::bitset<64> flags_;
    std::unordered_map<PropertyId, int> properties_;
    ZObject* location_ = nullptr;
    std::vector<ZObject*> contents_;
    std::vector<std::string> synonyms_;
    std::vector<std::string> adjectives_;
    ActionFunc action_;
};
```

**Design Rationale**: 
- Uses std::bitset for efficient flag storage
- Uses std::optional for properties to distinguish unset vs. zero
- Uses std::function for flexible action handlers
- Virtual destructor enables polymorphism for ZRoom

#### ZRoom Class

Specialized object representing rooms with exits and room actions.

```cpp
class ZRoom : public ZObject {
public:
    ZRoom(ObjectId id, std::string_view name, std::string_view desc);
    
    // Exits
    struct RoomExit {
        std::optional<ObjectId> targetRoom;
        std::optional<std::string> message;
        std::function<bool()> condition;
        
        RoomExit(ObjectId target);
        RoomExit(std::string_view msg);
        RoomExit(ObjectId target, std::function<bool()> cond);
    };
    
    void setExit(Direction dir, RoomExit exit);
    std::optional<RoomExit> getExit(Direction dir) const;
    const std::unordered_map<Direction, RoomExit>& getExits() const;
    
    // Room actions
    using RoomActionFunc = std::function<void(int)>;
    void setRoomAction(RoomActionFunc action);
    void performRoomAction(int actionType);
    
    // Description
    std::string_view getLongDesc() const;
    void setLongDesc(std::string_view desc);
    
private:
    std::unordered_map<Direction, RoomExit> exits_;
    RoomActionFunc roomAction_;
    std::string longDesc_;
};
```

**Design Rationale**:
- RoomExit supports three types: simple target, message, conditional
- Conditional exits use std::function for flexibility
- Separate room actions from object actions for clarity

#### Globals Singleton

Manages global game state accessible throughout the system.

```cpp
class Globals {
public:
    static Globals& instance();
    
    // Current state
    ZObject* here = nullptr;        // Current room
    ZObject* winner = nullptr;      // Current actor (usually player)
    ZObject* prso = nullptr;        // Direct object
    ZObject* prsi = nullptr;        // Indirect object
    int prsa = 0;                   // Current verb
    
    // Game state
    int score = 0;
    int moves = 0;
    bool lit = false;
    int deaths = 0;
    
    // Mode flags
    bool verboseMode = true;
    bool briefMode = false;
    bool superbriefMode = false;
    
    // Object registry
    void registerObject(ObjectId id, std::unique_ptr<ZObject> obj);
    ZObject* getObject(ObjectId id);
    const std::unordered_map<ObjectId, std::unique_ptr<ZObject>>& getAllObjects() const;
    
    // Initialization
    void reset();
    
private:
    Globals() = default;
    std::unordered_map<ObjectId, std::unique_ptr<ZObject>> objects_;
};
```

**Design Rationale**:
- Singleton pattern for global access (matches ZIL's global variables)
- Owns all objects via unique_ptr for automatic memory management
- Provides object registry for lookup by ID
- Reset method for game restart


### Parser System

#### Parser Class

Converts player text input into executable commands.

```cpp
class Parser {
public:
    struct ParseResult {
        int verb = 0;
        ZObject* directObj = nullptr;
        ZObject* indirectObj = nullptr;
        std::string error;
        bool success = false;
    };
    
    ParseResult parse(std::string_view input);
    
    // Special commands
    void setLastCommand(std::string_view cmd);
    std::string_view getLastCommand() const;
    void setLastObject(ZObject* obj);
    ZObject* getLastObject() const;
    
private:
    std::vector<std::string> tokenize(std::string_view input);
    std::optional<int> lookupVerb(std::string_view word);
    std::optional<Direction> lookupDirection(std::string_view word);
    std::vector<ZObject*> findObjects(std::string_view word, bool useAdjectives = true);
    ZObject* disambiguate(const std::vector<ZObject*>& candidates);
    
    std::string lastCommand_;
    ZObject* lastObject_ = nullptr;
    std::vector<ZObject*> lastObjects_;
};
```

**Design Rationale**:
- ParseResult struct encapsulates all parse information
- Separate methods for tokenization, verb lookup, object finding
- Tracks last command for AGAIN, last object for IT/THEM
- Returns std::optional for lookups that may fail

#### SyntaxPattern Class

Represents valid command structures for verbs.

```cpp
class SyntaxPattern {
public:
    enum class ElementType {
        VERB,
        OBJECT,
        PREPOSITION,
        DIRECTION
    };
    
    struct Element {
        ElementType type;
        std::vector<std::string> values;  // For prepositions
        ObjectFlag requiredFlag;           // For objects (TAKEBIT, etc.)
        bool optional = false;
    };
    
    SyntaxPattern(int verbId, std::vector<Element> pattern);
    
    bool matches(const std::vector<std::string>& tokens) const;
    ParseResult apply(const std::vector<std::string>& tokens) const;
    
private:
    int verbId_;
    std::vector<Element> pattern_;
};
```

**Design Rationale**:
- Flexible pattern matching for complex command structures
- Supports optional elements and multiple prepositions
- Can validate object flags during parsing
- Separates pattern definition from matching logic

#### Verb Registry

Maps verb synonyms to canonical verb IDs.

```cpp
class VerbRegistry {
public:
    void registerVerb(int verbId, std::vector<std::string> synonyms);
    void registerSyntax(int verbId, SyntaxPattern pattern);
    
    std::optional<int> lookupVerb(std::string_view word) const;
    const std::vector<SyntaxPattern>& getSyntaxPatterns(int verbId) const;
    
private:
    std::unordered_map<std::string, int> verbMap_;
    std::unordered_map<int, std::vector<SyntaxPattern>> syntaxMap_;
};
```

**Design Rationale**:
- Centralizes verb synonym mapping
- Associates syntax patterns with verbs
- Allows multiple syntax patterns per verb

### World System

#### World Initialization

```cpp
class World {
public:
    static void initialize(Globals& g);
    
private:
    static void createRooms(Globals& g);
    static void createObjects(Globals& g);
    static void setupInitialState(Globals& g);
    static void registerRoomActions(Globals& g);
    static void registerObjectActions(Globals& g);
};
```

**Design Rationale**:
- Static methods for one-time initialization
- Separate phases for rooms, objects, and state
- Centralizes world creation logic

#### Room Database

All ~100 rooms organized by area:

```cpp
// Room IDs organized by area
namespace RoomIds {
    // Exterior
    constexpr ObjectId WEST_OF_HOUSE = 1;
    constexpr ObjectId NORTH_OF_HOUSE = 2;
    constexpr ObjectId SOUTH_OF_HOUSE = 3;
    constexpr ObjectId BEHIND_HOUSE = 4;
    
    // Forest
    constexpr ObjectId FOREST_1 = 10;
    constexpr ObjectId FOREST_2 = 11;
    constexpr ObjectId FOREST_3 = 12;
    constexpr ObjectId FOREST_4 = 13;
    constexpr ObjectId CLEARING = 14;
    constexpr ObjectId FOREST_PATH = 15;
    
    // House Interior
    constexpr ObjectId LIVING_ROOM = 20;
    constexpr ObjectId KITCHEN = 21;
    constexpr ObjectId ATTIC = 22;
    constexpr ObjectId CELLAR = 23;
    
    // Underground
    constexpr ObjectId EAST_WEST_PASSAGE = 30;
    constexpr ObjectId ROUND_ROOM = 31;
    constexpr ObjectId LOUD_ROOM = 32;
    // ... etc
}
```

**Design Rationale**:
- Namespace organization by area for clarity
- Constexpr for compile-time constants
- Numeric ranges group related rooms


#### Object Database

All ~150 objects organized by type:

```cpp
namespace ObjectIds {
    // Treasures
    constexpr ObjectId TROPHY = 100;
    constexpr ObjectId EGG = 101;
    constexpr ObjectId CHALICE = 102;
    constexpr ObjectId TRIDENT = 103;
    constexpr ObjectId PAINTING = 104;
    
    // Tools
    constexpr ObjectId SWORD = 200;
    constexpr ObjectId LAMP = 201;
    constexpr ObjectId ROPE = 202;
    constexpr ObjectId KNIFE = 203;
    
    // Containers
    constexpr ObjectId MAILBOX = 300;
    constexpr ObjectId TROPHY_CASE = 301;
    constexpr ObjectId COFFIN = 302;
    constexpr ObjectId BASKET = 303;
    
    // NPCs
    constexpr ObjectId THIEF = 400;
    constexpr ObjectId TROLL = 401;
    constexpr ObjectId CYCLOPS = 402;
    
    // Scenery
    constexpr ObjectId WHITE_HOUSE = 500;
    constexpr ObjectId BOARD = 501;
    constexpr ObjectId WINDOW = 502;
    // ... etc
}
```

**Design Rationale**:
- Numeric ranges by object type for organization
- Easy to identify object category from ID
- Leaves room for expansion within categories

### Verb System

#### Verb Handler Interface

```cpp
using VerbHandler = std::function<bool()>;

class VerbDispatcher {
public:
    void registerVerb(int verbId, VerbHandler handler);
    bool executeVerb(int verbId);
    
private:
    std::unordered_map<int, VerbHandler> handlers_;
};
```

**Design Rationale**:
- Simple function-based handlers
- Returns bool to indicate if verb was handled
- Centralized dispatch for all verbs

#### Verb Implementation Pattern

Each verb follows a consistent pattern:

```cpp
bool vTake() {
    auto& g = Globals::instance();
    
    // Pre-checks
    if (!g.prso) {
        printLine("What do you want to take?");
        return false;
    }
    
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
        printLine("You can't take that.");
        return false;
    }
    
    // Check if object action handles it
    if (g.prso->performAction()) {
        return true;
    }
    
    // Default behavior
    g.prso->moveTo(g.winner);
    printLine("Taken.");
    return true;
}
```

**Design Rationale**:
- Consistent structure: pre-checks, action handler, default behavior
- Object actions can override default behavior
- Clear error messages for invalid actions

### Action System

#### Action Handler Pattern

Object and room actions follow a similar pattern:

```cpp
bool mailboxAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_TAKE) {
        printLine("It is securely anchored.");
        return true;  // Handled
    }
    
    if (g.prsa == V_OPEN) {
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("It's already open.");
        } else {
            g.prso->setFlag(ObjectFlag::OPENBIT);
            printLine("Opening the small mailbox reveals a leaflet.");
        }
        return true;
    }
    
    return false;  // Not handled, use default
}
```

**Design Rationale**:
- Check verb ID to determine behavior
- Return true if handled, false for default behavior
- Can check and modify object state

### Game Systems

#### Timer System

```cpp
class TimerSystem {
public:
    using TimerCallback = std::function<void()>;
    
    struct Timer {
        int interval;
        int counter;
        TimerCallback callback;
        bool enabled;
        bool repeating;
    };
    
    void registerTimer(std::string_view name, int interval, 
                      TimerCallback callback, bool repeating = true);
    void enableTimer(std::string_view name);
    void disableTimer(std::string_view name);
    void tick();  // Called each turn
    
private:
    std::unordered_map<std::string, Timer> timers_;
};
```

**Design Rationale**:
- Named timers for easy enable/disable
- Supports one-shot and repeating timers
- Callback-based for flexibility
- Tick method called from main loop


#### Score System

```cpp
class ScoreSystem {
public:
    void addScore(int points);
    int getScore() const;
    int getMoves() const;
    void incrementMoves();
    std::string_view getRank() const;
    
    void markTreasureScored(ObjectId treasureId);
    bool isTreasureScored(ObjectId treasureId) const;
    
private:
    int score_ = 0;
    int moves_ = 0;
    std::unordered_set<ObjectId> scoredTreasures_;
    
    static constexpr int MAX_SCORE = 350;
    std::string_view calculateRank() const;
};
```

**Design Rationale**:
- Tracks score, moves, and scored treasures
- Prevents double-scoring treasures
- Calculates rank based on score thresholds

#### Light System

```cpp
class LightSystem {
public:
    bool isRoomLit(ZObject* room);
    bool hasLightSource();
    void updateLighting();
    void checkGrue();
    
private:
    int darknessTurns_ = 0;
    bool warnedAboutGrue_ = false;
};
```

**Design Rationale**:
- Checks room natural lighting and light sources
- Tracks darkness duration for grue attacks
- Separate warning before grue attack

#### Combat System

```cpp
class CombatSystem {
public:
    struct Combatant {
        ZObject* object;
        int health;
        int strength;
        ZObject* weapon;
    };
    
    void startCombat(ZObject* enemy);
    void endCombat();
    bool isInCombat() const;
    void processCombatRound();
    
private:
    std::optional<Combatant> player_;
    std::optional<Combatant> enemy_;
    
    int calculateDamage(const Combatant& attacker, const Combatant& defender);
    bool attackSucceeds(const Combatant& attacker);
};
```

**Design Rationale**:
- Tracks combatant state (health, weapon)
- Round-based combat processing
- Weapon effectiveness affects damage calculation

## Data Models

### Object Flags

```cpp
enum class ObjectFlag {
    // Visibility
    INVISIBLE,      // Object is not visible
    TOUCHBIT,       // Object has been touched/seen
    
    // Takability
    TAKEBIT,        // Object can be taken
    TRYTAKEBIT,     // Object cannot be taken (anchored)
    
    // Container
    CONTBIT,        // Object is a container
    OPENBIT,        // Container is open
    TRANSBIT,       // Container is transparent
    
    // Light
    ONBIT,          // Light source is on
    LIGHTBIT,       // Object provides light
    
    // State
    LOCKEDBIT,      // Object is locked
    DEADBIT,        // NPC is dead
    FIGHTBIT,       // NPC is hostile
    
    // Room
    RLANDBIT,       // Room is on land
    RWATERBIT,      // Room is water
    SACREDBIT,      // Room is sacred (no fighting)
    
    // Special
    FOODBIT,        // Object is edible
    DRINKBIT,       // Object is drinkable
    BURNBIT,        // Object is burning
    FLAMEBIT,       // Object is flammable
};
```

### Object Properties

```cpp
enum class PropertyId {
    SIZE,           // Object size
    CAPACITY,       // Container capacity
    VALUE,          // Treasure value
    STRENGTH,       // Weapon/NPC strength
    TEXT,           // Readable text
    DESCFCN,        // Description function
    LDESC,          // Long description
    FDESC,          // First description
};
```

### Verb IDs

```cpp
namespace VerbIds {
    // Movement
    constexpr int V_WALK = 1;
    constexpr int V_CLIMB = 2;
    constexpr int V_ENTER = 3;
    constexpr int V_EXIT = 4;
    
    // Manipulation
    constexpr int V_TAKE = 10;
    constexpr int V_DROP = 11;
    constexpr int V_PUT = 12;
    constexpr int V_OPEN = 13;
    constexpr int V_CLOSE = 14;
    
    // Examination
    constexpr int V_LOOK = 20;
    constexpr int V_EXAMINE = 21;
    constexpr int V_READ = 22;
    constexpr int V_SEARCH = 23;
    
    // Combat
    constexpr int V_ATTACK = 30;
    constexpr int V_KILL = 31;
    constexpr int V_THROW = 32;
    
    // Meta
    constexpr int V_INVENTORY = 40;
    constexpr int V_SCORE = 41;
    constexpr int V_QUIT = 42;
    constexpr int V_SAVE = 43;
    constexpr int V_RESTORE = 44;
}
```


## Error Handling

### Parser Errors

```cpp
enum class ParseError {
    UNKNOWN_WORD,       // "I don't know the word 'foo'"
    AMBIGUOUS,          // "Which one do you mean?"
    NOT_VISIBLE,        // "You can't see any such thing"
    INVALID_SYNTAX,     // "I don't understand that"
    MISSING_OBJECT,     // "What do you want to take?"
};
```

**Error Handling Strategy**:
- Parser returns ParseResult with error information
- Errors are displayed to player with helpful messages
- Game continues after parse errors (no crashes)

### Runtime Errors

```cpp
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& msg) 
        : std::runtime_error(msg) {}
};
```

**Error Handling Strategy**:
- Use exceptions only for truly exceptional cases
- Most errors are handled gracefully with error messages
- File I/O errors throw exceptions
- Memory allocation failures propagate std::bad_alloc

### Save/Restore Errors

```cpp
enum class SaveError {
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    CORRUPT_FILE,
    VERSION_MISMATCH,
};

class SaveSystem {
public:
    std::expected<void, SaveError> save(std::string_view filename);
    std::expected<void, SaveError> restore(std::string_view filename);
};
```

**Error Handling Strategy**:
- Use std::expected (C++23) or custom Result type
- Provide specific error codes for different failures
- Display user-friendly error messages
- Allow game to continue after save/restore failure

## Testing Strategy

### Unit Testing

Test individual components in isolation:

```cpp
TEST(ObjectTest, FlagOperations) {
    ZObject obj(1, "test object");
    
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
    obj.setFlag(ObjectFlag::TAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    obj.clearFlag(ObjectFlag::TAKEBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
}

TEST(ParserTest, Tokenization) {
    Parser parser;
    auto tokens = parser.tokenize("take the small mailbox");
    
    ASSERT_EQ(tokens.size(), 4);
    ASSERT_EQ(tokens[0], "take");
    ASSERT_EQ(tokens[1], "the");
    ASSERT_EQ(tokens[2], "small");
    ASSERT_EQ(tokens[3], "mailbox");
}
```

### Integration Testing

Test system interactions:

```cpp
TEST(GameTest, TakeAndDrop) {
    Game game;
    game.init();
    
    auto result = game.execute("take lamp");
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(game.globals().winner->contains(lamp));
    
    result = game.execute("drop lamp");
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(game.globals().here->contains(lamp));
}
```

### Transcript Testing

Test against known playthroughs:

```cpp
TEST(TranscriptTest, OpeningSequence) {
    Game game;
    game.init();
    
    EXPECT_OUTPUT_CONTAINS(game.execute("look"),
        "You are standing in an open field");
    EXPECT_OUTPUT_CONTAINS(game.execute("open mailbox"),
        "Opening the small mailbox reveals a leaflet");
    EXPECT_OUTPUT_CONTAINS(game.execute("read leaflet"),
        "WELCOME TO ZORK!");
}
```

### Test Coverage Goals

- Core Engine: 95%+ coverage
- Parser: 90%+ coverage
- Verbs: 85%+ coverage
- Actions: 80%+ coverage
- Game Systems: 90%+ coverage

## Performance Considerations

### Command Response Time

**Target**: <10ms per command

**Strategies**:
- Use hash maps for O(1) object lookup
- Cache frequently accessed data
- Avoid unnecessary string allocations
- Use string_view for non-owning string references
- Minimize virtual function calls in hot paths

### Memory Usage

**Target**: <50MB total memory

**Strategies**:
- Use smart pointers to avoid leaks
- Reuse string storage where possible
- Use bitsets for flags (compact storage)
- Avoid unnecessary copies with move semantics

### Startup Time

**Target**: <100ms to initialize

**Strategies**:
- Lazy initialization where possible
- Efficient world building code
- Minimize allocations during init


## File Organization

### Directory Structure

```
src/
├── core/
│   ├── object.h/cpp          # ZObject and ZRoom classes
│   ├── globals.h/cpp         # Global state singleton
│   ├── io.h/cpp              # Input/output functions
│   ├── flags.h               # Flag enumerations
│   └── properties.h          # Property enumerations
├── parser/
│   ├── parser.h/cpp          # Main parser class
│   ├── syntax.h/cpp          # Syntax pattern matching
│   ├── lexer.h/cpp           # Tokenization
│   └── verb_registry.h/cpp   # Verb synonym mapping
├── verbs/
│   ├── verbs.h               # Verb ID constants
│   ├── movement.cpp          # Movement verbs
│   ├── manipulation.cpp      # Object manipulation verbs
│   ├── interaction.cpp       # Interaction verbs
│   ├── combat.cpp            # Combat verbs
│   └── meta.cpp              # Meta-game verbs
├── world/
│   ├── rooms.h               # Room ID constants
│   ├── objects.h             # Object ID constants
│   ├── world.h/cpp           # World initialization
│   ├── room_data.cpp         # Room creation
│   ├── object_data.cpp       # Object creation
│   └── initial_state.cpp     # Initial game state
├── actions/
│   ├── room_actions.cpp      # Room-specific actions
│   ├── object_actions.cpp    # Object-specific actions
│   ├── container_actions.cpp # Container behaviors
│   ├── tool_actions.cpp      # Tool behaviors
│   ├── treasure_actions.cpp  # Treasure behaviors
│   └── npc_actions.cpp       # NPC behaviors
├── systems/
│   ├── timer.h/cpp           # Timer/interrupt system
│   ├── score.h/cpp           # Scoring system
│   ├── light.h/cpp           # Light/darkness system
│   ├── combat.h/cpp          # Combat system
│   └── save.h/cpp            # Save/restore system
└── main.cpp                  # Entry point and main loop
```

### Header Organization

**Principle**: Minimize dependencies, maximize compilation speed

- Forward declare when possible
- Use Pimpl idiom for complex classes
- Keep headers focused and minimal
- Include guards or #pragma once

### Implementation Organization

**Principle**: Group related functionality

- One verb category per file (movement.cpp, manipulation.cpp)
- One object type per file (container_actions.cpp, tool_actions.cpp)
- Separate data from logic (room_data.cpp vs world.cpp)

## Build System

### CMakeLists.txt Structure

```cmake
cmake_minimum_required(VERSION 3.15)
project(zork1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core engine
add_library(zork_core
    src/core/object.cpp
    src/core/globals.cpp
    src/core/io.cpp
)

# Parser
add_library(zork_parser
    src/parser/parser.cpp
    src/parser/syntax.cpp
    src/parser/lexer.cpp
    src/parser/verb_registry.cpp
)

# World
add_library(zork_world
    src/world/world.cpp
    src/world/room_data.cpp
    src/world/object_data.cpp
    src/world/initial_state.cpp
)

# Verbs
add_library(zork_verbs
    src/verbs/movement.cpp
    src/verbs/manipulation.cpp
    src/verbs/interaction.cpp
    src/verbs/combat.cpp
    src/verbs/meta.cpp
)

# Actions
add_library(zork_actions
    src/actions/room_actions.cpp
    src/actions/object_actions.cpp
    src/actions/container_actions.cpp
    src/actions/tool_actions.cpp
    src/actions/treasure_actions.cpp
    src/actions/npc_actions.cpp
)

# Systems
add_library(zork_systems
    src/systems/timer.cpp
    src/systems/score.cpp
    src/systems/light.cpp
    src/systems/combat.cpp
    src/systems/save.cpp
)

# Main executable
add_executable(zork1 src/main.cpp)
target_link_libraries(zork1
    zork_core
    zork_parser
    zork_world
    zork_verbs
    zork_actions
    zork_systems
)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
```

**Design Rationale**:
- Separate libraries for each subsystem
- Enables parallel compilation
- Clear dependency structure
- Easy to add tests

## Migration from ZIL

### ZIL to C++ Mapping

| ZIL Construct | C++ Equivalent |
|---------------|----------------|
| `<OBJECT>` | `ZObject` instance |
| `<ROOM>` | `ZRoom` instance |
| `<ROUTINE>` | C++ function |
| `<GLOBAL>` | `Globals` member |
| `<FSET>` | `setFlag()` |
| `<FCLEAR>` | `clearFlag()` |
| `<FSET?>` | `hasFlag()` |
| `<GETP>` | `getProperty()` |
| `<PUTP>` | `setProperty()` |
| `<MOVE>` | `moveTo()` |
| `<LOC>` | `getLocation()` |
| `<FIRST?>` | `getContents()` |
| `<TELL>` | `print()` / `printLine()` |
| `<COND>` | `if` / `else if` |
| `<AND>` | `&&` |
| `<OR>` | `||` |
| `<NOT>` | `!` |
| `<EQUAL?>` | `==` |
| `<VERB?>` | `g.prsa == V_*` |

### Conversion Process

1. **Identify ZIL construct**: Determine what the ZIL code does
2. **Map to C++ pattern**: Use the mapping table above
3. **Preserve logic**: Maintain exact behavior
4. **Use modern C++**: Apply C++17 features where appropriate
5. **Test**: Verify behavior matches original

### Example Conversion

**ZIL**:
```lisp
<ROUTINE MAILBOX-F ()
    <COND (<AND <VERB? TAKE> <EQUAL? ,PRSO ,MAILBOX>>
           <TELL "It is securely anchored." CR>
           <RTRUE>)>>
```

**C++**:
```cpp
bool mailboxAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE && g.prso && g.prso->getId() == OBJ_MAILBOX) {
        printLine("It is securely anchored.");
        return true;
    }
    return false;
}
```

## Summary

This design provides:

1. **Clean Architecture**: Clear separation of concerns with well-defined interfaces
2. **Modern C++**: Leverages C++17 features for safety and expressiveness
3. **Testability**: Designed for comprehensive unit and integration testing
4. **Performance**: Optimized for instant response times
5. **Maintainability**: Organized code structure with clear naming conventions
6. **Fidelity**: Preserves exact behavior of original Zork I

The design supports incremental development, allowing implementation to proceed in phases while maintaining a working game at each stage.

