# Zork I C++ Port - Working Demo

## What's Implemented

This C++ port now has a fully functional game engine with:

### Core Systems
- **Object System**: Complete with properties, flags, and containment hierarchy
- **Room System**: Rooms with exits, descriptions, and action handlers
- **Parser**: Tokenization, verb recognition, and direction handling
- **Main Loop**: Game loop matching the original ZIL structure
- **I/O**: Text output and input handling

### World Content
Successfully converted from the original ZIL source:

#### Rooms (5 of ~100)
1. **West of House** - The iconic starting location
2. **North of House** - North side with path to forest
3. **South of House** - South side
4. **Stone Barrow** - Secret location (requires winning)
5. **Forest** - West of the house

#### Objects (4 of ~150)
1. **Mailbox** - The famous small mailbox (with "securely anchored" action)
2. **White House** - Global scenery object
3. **Board** - Boarded windows/door
4. **Forest** - Forest scenery

### Working Features

**Navigation**
- All 8 compass directions (N, S, E, W, NE, NW, SE, SW)
- Up/Down directions
- Blocked exits with custom messages
- Conditional exits (e.g., Stone Barrow requires WON-FLAG)

**Commands**
- `look` - Full room descriptions with visible objects
- `inventory` / `i` - List carried items
- `quit` / `q` - Exit game with confirmation
- Direction commands: `north`, `n`, `south`, `s`, etc.

**Room Descriptions**
Exact text from original Zork I:
```
You are standing in an open field west of a white house, with a boarded
front door.
```

**Object Actions**
- Mailbox: "It is securely anchored." (when trying to take)
- Blocked doors: "The door is boarded and you can't remove the boards."

## Sample Gameplay

```
ZORK I: The Great Underground Empire
Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.
ZORK is a registered trademark of Infocom, Inc.

You are standing in an open field west of a white house, with a boarded
front door.
There is small mailbox here.

> north
You are facing the north side of a white house. There is no door here,
and all the windows are boarded up. To the north a narrow path winds through
the trees.

> south
The windows are all boarded.

> west
You are standing in an open field west of a white house, with a boarded
front door.
There is small mailbox here.

> west
This is a forest, with trees in all directions. To the east, there appears to be sunlight.

> east
You are standing in an open field west of a white house, with a boarded
front door.
There is small mailbox here.

> inventory
You are empty-handed.

> quit
Do you really want to quit? (yes/no)
yes
Thanks for playing!
```

## Architecture Highlights

### ZIL → C++ Mapping

| ZIL Concept | C++ Implementation |
|-------------|-------------------|
| `<OBJECT>` | `ZObject` class |
| `<ROOM>` | `ZRoom` class (inherits ZObject) |
| `<ROUTINE>` | C++ function |
| `<GLOBAL>` | `Globals` singleton |
| `FSET/FCLEAR/FSET?` | `setFlag/clearFlag/hasFlag` |
| `MOVE` | `moveTo()` |
| `TELL` | `print()` / `printLine()` |
| `M-LOOK` | Room action argument |
| Direction exits | `RoomExit` with conditions |

### Code Quality
- Modern C++17
- Clean separation of concerns
- Type-safe enums for flags and IDs
- Smart pointers for memory management
- Virtual functions for polymorphism
- Lambda functions for action handlers

## Next Steps

To complete the full Zork I conversion:

1. **Parser Enhancement** (~1400 lines from gparser.zil)
   - Full syntax pattern matching
   - Object recognition by synonyms/adjectives
   - Preposition handling
   - Disambiguation

2. **Remaining Rooms** (~95 rooms from 1dungeon.zil)
   - Underground areas
   - Maze sections
   - Puzzle rooms

3. **Remaining Objects** (~145 objects)
   - Treasures
   - Tools and weapons
   - NPCs (thief, troll, etc.)

4. **Verb Implementations** (~2200 lines from gverbs.zil)
   - Take, drop, open, close
   - Attack, read, turn
   - Complex interactions

5. **Action Handlers** (~4200 lines from 1actions.zil)
   - Object-specific behaviors
   - Puzzle logic
   - NPC interactions

6. **Game Systems**
   - Timer/interrupt system (gclock.zil)
   - Score tracking
   - Light/darkness
   - Combat system

## Building

```bash
cd zork_cpp/build
cmake ..
make
./zork1
```

## Source Files

- `src/core/` - Object system, globals, I/O (6 files)
- `src/parser/` - Command parsing (2 files)
- `src/verbs/` - Verb handlers (2 files)
- `src/world/` - Rooms and objects (4 files)
- `src/main.cpp` - Entry point and game loop

Total: ~800 lines of C++ implementing the foundation for the full ~12,000 line ZIL source.
