---
status: in-progress
priority: high
---

# Zork I Complete C++ Port Specification

## Overview

Convert the complete Zork I source code (~12,000 lines of ZIL across 9 files) to modern C++17, maintaining exact game behavior and text output while creating a clean, maintainable architecture.

## Goals

1. **Exact Fidelity**: Game behavior and text must match the original Zork I exactly
2. **Clean Architecture**: Modern C++ with proper OOP, no legacy cruft
3. **Maintainability**: Well-organized code that's easy to understand and extend
4. **Testability**: Comprehensive test suite to verify correctness

## Current Status

### Completed ✓
- Core object system with properties, flags, containment
- Room system with exits, descriptions, action handlers
- Basic parser (tokenization, verb lookup, direction handling)
- Main game loop
- I/O system
- 5 rooms: West/North/South of House, Stone Barrow, Forest-1
- 4 objects: Mailbox, White House, Board, Forest
- Basic verbs: look, inventory, quit, directional movement

### In Progress
- Parser enhancement for full syntax matching
- Object recognition system

## Implementation Plan

### Phase 1: Parser Enhancement (Priority: HIGH)

**File**: `src/parser/parser.cpp`, `src/parser/syntax.h`

Convert `gparser.zil` (1407 lines) and `gsyntax.zil` (561 lines):

#### Tasks:
1. **Syntax Pattern System**
   - Create `SyntaxPattern` class to represent verb syntax rules
   - Support patterns like: `TAKE OBJECT (FIND TAKEBIT) (ON-GROUND MANY)`
   - Handle prepositions: WITH, IN, ON, TO, FROM, etc.
   - Support object filters: FIND flags, location requirements

2. **Object Recognition**
   - Match objects by synonyms (e.g., "mailbox", "box")
   - Match by adjectives (e.g., "small mailbox")
   - Handle multi-word names
   - Implement disambiguation ("Which one do you mean?")
   - Support "all" and "all except"

3. **Verb Synonyms**
   - Map all synonyms from gsyntax.zil
   - Examples: TAKE/GET/GRAB, EXAMINE/LOOK AT/DESCRIBE

4. **Parser State**
   - Track IT/THEM references
   - Handle AGAIN/G command
   - Support OOPS correction

**Acceptance Criteria**:
- Can parse: "take the small mailbox"
- Can parse: "put sword in trophy case"
- Can parse: "attack troll with sword"
- Handles "take all except lamp"
- Proper disambiguation when multiple objects match

---

### Phase 2: World Building (Priority: HIGH)

**Files**: `src/world/dungeon.cpp`, `src/world/objects_db.cpp`

Convert `1dungeon.zil` (2660 lines):

#### Tasks:
1. **All Rooms** (~100 rooms)
   - Forest areas (FOREST-1 through FOREST-4, CLEARING, PATH)
   - House exterior (complete)
   - House interior (LIVING-ROOM, KITCHEN, ATTIC, CELLAR)
   - Underground (EAST-WEST-PASSAGE, ROUND-ROOM, LOUD-ROOM, etc.)
   - Maze sections (MAZE-1 through MAZE-15)
   - Special areas (TREASURE-ROOM, ENTRANCE-TO-HADES, etc.)

2. **All Objects** (~150 objects)
   - Treasures (TROPHY, PAINTING, EGG, CHALICE, TRIDENT, etc.)
   - Tools (SWORD, LAMP, ROPE, WRENCH, SCREWDRIVER, etc.)
   - Containers (TROPHY-CASE, COFFIN, BASKET, SACK, etc.)
   - NPCs (THIEF, TROLL, CYCLOPS)
   - Scenery objects (WINDOW, DOOR, RUG, etc.)
   - Special items (BOAT, PUMP, CANDLES, MATCHES, etc.)

3. **Room Connections**
   - All directional exits
   - Conditional exits (doors, gates, etc.)
   - Special movement (ENTER, CLIMB, etc.)

4. **Initial State**
   - Object starting locations
   - Initial flag states
   - Property values

**Acceptance Criteria**:
- All 100+ rooms accessible
- All objects in correct starting locations
- Room descriptions match original exactly
- All exits work correctly

---

### Phase 3: Verb Implementation (Priority: HIGH)

**Files**: `src/verbs/verbs.cpp`, `src/verbs/verb_handlers.cpp`

Convert `gverbs.zil` (2216 lines):

#### Tasks:
1. **Movement Verbs**
   - WALK, CLIMB, ENTER, EXIT, BOARD, DISEMBARK

2. **Manipulation Verbs**
   - TAKE, DROP, PUT, PUT-ON, THROW
   - OPEN, CLOSE, LOCK, UNLOCK
   - TURN, PUSH, PULL, MOVE
   - TIE, UNTIE

3. **Interaction Verbs**
   - EXAMINE, LOOK, LOOK-INSIDE, SEARCH
   - READ, LISTEN, SMELL
   - EAT, DRINK
   - ATTACK, KILL, SWING

4. **Special Verbs**
   - LAMP-ON, LAMP-OFF
   - INFLATE, DEFLATE
   - PRAY, EXORCISE
   - WAVE, RUB, RING

5. **Meta Verbs**
   - INVENTORY, SCORE, DIAGNOSE
   - SAVE, RESTORE, RESTART
   - VERBOSE, BRIEF, SUPERBRIEF
   - QUIT, VERSION

**Acceptance Criteria**:
- All verbs from gsyntax.zil implemented
- Proper error messages for invalid actions
- Pre-verb checks (PRE-TAKE, PRE-DROP, etc.)
- Verb-specific logic matches original

---

### Phase 4: Object Actions (Priority: MEDIUM)

**Files**: `src/actions/object_actions.cpp`, `src/actions/room_actions.cpp`

Convert `1actions.zil` (4177 lines):

#### Tasks:
1. **Room Actions**
   - WEST-HOUSE, EAST-HOUSE, etc.
   - Special room behaviors (LOUD-ROOM echo, etc.)

2. **Object Actions**
   - Container actions (TROPHY-CASE, MAILBOX, etc.)
   - Tool actions (SWORD, LAMP, ROPE, etc.)
   - NPC actions (THIEF, TROLL, CYCLOPS)
   - Puzzle objects (MIRROR, PAINTING, MACHINE, etc.)

3. **Complex Interactions**
   - Multi-object puzzles
   - State-dependent behaviors
   - Triggered events

**Acceptance Criteria**:
- All object-specific behaviors work
- Puzzles solvable
- NPCs behave correctly

---

### Phase 5: Game Systems (Priority: MEDIUM)

**Files**: `src/core/clock.cpp`, `src/core/score.cpp`, `src/core/light.cpp`

Convert `gclock.zil` (60 lines) and implement game mechanics:

#### Tasks:
1. **Timer/Interrupt System**
   - I-THIEF (thief wandering)
   - I-FIGHT (combat rounds)
   - I-CANDLES (candle burning)
   - I-LANTERN (lamp battery)
   - I-SWORD (sword glow)

2. **Light/Darkness**
   - Track light sources
   - Grue attacks in darkness
   - Room lighting states

3. **Score System**
   - Treasure scoring
   - Move counting
   - Rank calculation

4. **Combat System**
   - Attack mechanics
   - Weapon effectiveness
   - NPC health/behavior

5. **Death and Restart**
   - JIGS-UP function
   - Resurrection mechanics
   - Game over handling

**Acceptance Criteria**:
- Timers fire correctly
- Light/dark mechanics work
- Score calculated properly
- Combat system functional

---

### Phase 6: Testing (Priority: HIGH)

**Files**: `tests/`, `tests/transcript_test.cpp`

#### Tasks:
1. **Unit Tests**
   - Object system tests
   - Parser tests
   - Verb handler tests
   - Room navigation tests

2. **Integration Tests**
   - Complete puzzle solutions
   - NPC interactions
   - Game state persistence

3. **Transcript Tests**
   - Extract commands from MIT transcript
   - Replay full game walkthrough
   - Verify output matches expected

4. **Regression Tests**
   - Test suite for bug fixes
   - Edge case coverage

**Test Data Sources**:
- MIT transcript: https://web.mit.edu/marleigh/www/portfolio/Files/zork/transcript.html
- InvisiClues solutions
- Community walkthroughs

**Acceptance Criteria**:
- 90%+ code coverage
- Full transcript playthrough passes
- All puzzles solvable
- No crashes or undefined behavior

---

## Technical Requirements

### Code Standards
- C++17 standard
- No raw pointers (use smart pointers)
- RAII for resource management
- Const correctness
- Clear naming conventions

### Architecture Patterns
- Singleton for global state (Globals)
- Factory pattern for object creation
- Command pattern for verbs
- Observer pattern for timers/events
- Strategy pattern for NPC AI

### Performance
- Instant response to commands (<10ms)
- Minimal memory footprint (<50MB)
- Fast save/load (<100ms)

### Compatibility
- Linux, macOS, Windows
- Terminal/console interface
- UTF-8 text encoding

---

## File Structure

```
zork_cpp/
├── src/
│   ├── core/           # Core engine
│   │   ├── object.cpp/h
│   │   ├── globals.cpp/h
│   │   ├── io.cpp/h
│   │   ├── clock.cpp/h
│   │   ├── score.cpp/h
│   │   └── light.cpp/h
│   ├── parser/         # Command parsing
│   │   ├── parser.cpp/h
│   │   ├── syntax.cpp/h
│   │   └── lexer.cpp/h
│   ├── verbs/          # Verb implementations
│   │   ├── verbs.cpp/h
│   │   ├── movement.cpp
│   │   ├── manipulation.cpp
│   │   └── interaction.cpp
│   ├── world/          # Game world
│   │   ├── rooms.cpp/h
│   │   ├── objects.cpp/h
│   │   ├── dungeon.cpp
│   │   └── world.cpp/h
│   ├── actions/        # Object/room actions
│   │   ├── object_actions.cpp
│   │   ├── room_actions.cpp
│   │   └── npc_actions.cpp
│   └── main.cpp
├── tests/
│   ├── unit/
│   ├── integration/
│   └── transcript/
├── data/
│   └── transcript.txt
└── docs/
    ├── CONVERSION_PLAN.md
    └── DEMO.md
```

---

## Testing Strategy

### Transcript-Based Testing

Extract commands from the MIT transcript and create automated tests:

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

### Puzzle Tests

```cpp
TEST(PuzzleTest, TrollBridge) {
    Game game;
    game.loadState("before_troll.sav");
    
    // Try to cross without killing troll
    EXPECT_OUTPUT_CONTAINS(game.execute("east"),
        "The troll fends you off");
    
    // Kill troll with sword
    game.execute("attack troll with sword");
    EXPECT_TRUE(game.getObject("TROLL")->hasFlag(DEADBIT));
    
    // Now can cross
    EXPECT_SUCCESS(game.execute("east"));
}
```

---

## Milestones

### Milestone 1: Playable Demo (Current + 2 weeks)
- Parser handles 80% of commands
- 50 rooms implemented
- 50 objects implemented
- 20 verbs working
- Can play first 30 minutes of game

### Milestone 2: Feature Complete (Current + 6 weeks)
- All rooms implemented
- All objects implemented
- All verbs implemented
- All puzzles solvable
- Timer system working

### Milestone 3: Polish & Test (Current + 8 weeks)
- Full transcript test passing
- All edge cases handled
- Performance optimized
- Documentation complete

### Milestone 4: Release (Current + 10 weeks)
- All tests passing
- Cross-platform builds
- Release packages
- Public announcement

---

## Success Criteria

The port is complete when:

1. ✓ All rooms from 1dungeon.zil are implemented
2. ✓ All objects from 1dungeon.zil are implemented
3. ✓ All verbs from gverbs.zil are implemented
4. ✓ All actions from 1actions.zil are implemented
5. ✓ Timer system from gclock.zil is working
6. ✓ Full MIT transcript test passes
7. ✓ Game is winnable (can achieve 350 points)
8. ✓ No crashes or undefined behavior
9. ✓ Text output matches original exactly
10. ✓ Code is clean, documented, and maintainable

---

## References

- Original ZIL source files (9 files, ~12K lines)
- MIT transcript: https://web.mit.edu/marleigh/www/portfolio/Files/zork/transcript.html
- Zork I documentation and maps
- InvisiClues hint book
- Community walkthroughs and solutions
