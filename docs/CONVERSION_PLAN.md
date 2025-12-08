# Zork I ZIL to C++ Conversion Plan

## Status: Foundation Complete ✓

The basic C++ framework is now working with:
- Core object system with properties and flags
- Basic parser structure
- Verb dispatch system
- Main game loop
- Compiles and runs successfully

## Conversion Progress

### Phase 1: Core Infrastructure ✓
- [x] Object system (flags, properties, containment)
- [x] Global state management
- [x] I/O functions
- [x] Basic types and constants
- [x] Main loop structure

### Phase 2: Parser (In Progress)
- [x] Basic tokenization
- [x] Verb lookup with synonyms
- [x] Direction parsing and handling
- [ ] Full syntax pattern matching (gsyntax.zil)
- [ ] Object recognition and disambiguation
- [ ] Preposition handling
- [ ] Multi-word object names
- [ ] Adjective matching

### Phase 3: World Building (In Progress)
- [x] Room system with exits and descriptions
- [x] Direction handling (N, S, E, W, NE, NW, SE, SW, UP, DOWN)
- [x] Room action handlers (M-LOOK)
- [x] West of House room (fully functional)
- [x] North of House room
- [x] South of House room
- [x] Stone Barrow room
- [x] Forest-1 room
- [x] Mailbox object with action handler
- [x] Global objects (WHITE-HOUSE, BOARD, FOREST)
- [ ] Remaining ~95 rooms
- [ ] Remaining ~145 objects

### Phase 4: Verbs (Next)
- [ ] Convert gverbs.zil (~2200 lines)
- [ ] Implement all verb handlers
- [ ] Pre-verb checks (PRE-TAKE, PRE-DROP, etc.)
- [ ] Verb-specific logic

### Phase 5: Actions
- [ ] Convert 1actions.zil (~4200 lines)
- [ ] Object-specific action handlers
- [ ] Complex interactions

### Phase 6: Game Systems
- [ ] Timer/interrupt system (gclock.zil)
- [ ] Score tracking
- [ ] Light/darkness mechanics
- [ ] Inventory weight limits
- [ ] Death and restart

## Architecture Mapping

### ZIL → C++
- `OBJECT` → `ZObject` class
- `ROUTINE` → C++ function
- `GLOBAL` → `Globals` singleton members
- `FSET/FCLEAR/FSET?` → `setFlag/clearFlag/hasFlag`
- `MOVE` → `moveTo()`
- `TELL` → `print()` / `printLine()`
- `VERB?` → verb ID comparison
- `PRSO/PRSI` → `globals.prso/prsi`

## Next Steps

1. Expand parser to handle full syntax patterns
2. Create object registry with all game objects
3. Implement room descriptions and connections
4. Add more verb implementations
5. Convert object action handlers

## File Mapping

| ZIL File | Lines | C++ Target | Status |
|----------|-------|------------|--------|
| gmacros.zil | 154 | Inline/utilities | Partial |
| gsyntax.zil | 561 | parser/ | Started |
| 1dungeon.zil | 2660 | world/ | TODO |
| gglobals.zil | 308 | core/globals | Partial |
| gclock.zil | 60 | core/clock | TODO |
| gmain.zil | 313 | main.cpp | Done |
| gparser.zil | 1407 | parser/ | TODO |
| gverbs.zil | 2216 | verbs/ | Started |
| 1actions.zil | 4177 | actions/ | TODO |

## Testing

Build and run:
```bash
cd zork_cpp/build
cmake ..
make
./zork1
```

Current commands working:
- look (with full room descriptions)
- inventory
- quit
- Directional movement: n, s, e, w, ne, nw, se, sw, up, down
- Blocked exits show appropriate messages

## Demo

```
> look
You are standing in an open field west of a white house, with a boarded
front door.
There is small mailbox here.

> north
You are facing the north side of a white house. There is no door here,
and all the windows are boarded up. To the north a narrow path winds through
the trees.

> west
You are standing in an open field west of a white house, with a boarded
front door.
There is small mailbox here.

> east
The door is boarded and you can't remove the boards.

> west
This is a forest, with trees in all directions. To the east, there appears to be sunlight.
```
