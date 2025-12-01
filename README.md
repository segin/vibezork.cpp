# Zork I - C++ Port

A complete C++ port of the classic Zork I text adventure game, originally written in ZIL (Zork Implementation Language) by Infocom. This port faithfully recreates the original game experience using modern C++17.

## Features

### Complete Game Implementation
- **100+ rooms** - Full underground empire including the Great Underground Empire, maze, coal mine, and special areas
- **150+ objects** - All treasures, tools, containers, NPCs, and scenery from the original
- **80+ verbs** - Complete command vocabulary including manipulation, examination, combat, and special actions
- **All puzzles** - Every puzzle from the original game is fully implemented and solvable

### Game Systems
- **Parser** - Natural language parser with synonym support, adjectives, prepositions, and disambiguation
- **Combat** - Turn-based combat system with the Troll, Thief, and other enemies
- **Light/Darkness** - Lamp battery management and grue attacks in dark areas
- **Scoring** - Full 350-point scoring system with ranks
- **NPCs** - Wandering Thief, bridge-guarding Troll, and hungry Cyclops
- **Timers** - Lamp battery drain, candle burning, sword glow, and NPC movement
- **Save/Restore** - Full game state serialization

### Parser Features
- Verb synonyms (TAKE/GET/GRAB, EXAMINE/X/LOOK AT, etc.)
- Object synonyms and adjectives (SMALL BRASS LANTERN)
- Preposition handling (PUT X IN Y, ATTACK TROLL WITH SWORD)
- "ALL" and "ALL EXCEPT" support
- AGAIN (G) command to repeat last action
- OOPS correction for typos
- IT/THEM pronoun tracking
- Disambiguation ("Which do you mean?")

## Building

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build the Game
```bash
mkdir build && cd build
cmake ..
make
./zork1
```

### Build with Tests
```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make
ctest --output-on-failure
```

## Playing the Game

### Basic Commands
```
> look              - Describe current location
> inventory (i)     - List carried items
> take <object>     - Pick up an object
> drop <object>     - Put down an object
> examine <object>  - Look at something closely
> open <container>  - Open a container or door
> go north (n)      - Move in a direction
```

### Movement
- Compass directions: NORTH (N), SOUTH (S), EAST (E), WEST (W)
- Diagonal: NORTHEAST (NE), NORTHWEST (NW), SOUTHEAST (SE), SOUTHWEST (SW)
- Vertical: UP (U), DOWN (D)
- Special: ENTER, EXIT, CLIMB

### Game Management
```
> save              - Save your game
> restore           - Load a saved game
> score             - Check your score and rank
> diagnose          - Check your health
> quit              - Exit the game
```

## Project Structure

```
zork_cpp/
├── src/
│   ├── core/           # Core engine (objects, flags, I/O, globals)
│   ├── parser/         # Command parsing and verb registry
│   ├── verbs/          # Verb handler implementations
│   ├── world/          # Rooms, objects, and world initialization
│   ├── systems/        # Game systems (timer, combat, light, score, save)
│   └── main.cpp        # Entry point and main game loop
├── tests/              # Comprehensive test suite
├── docs/               # Additional documentation
└── zil/                # Original ZIL source files for reference
```

### Source Organization

| Directory | Description |
|-----------|-------------|
| `src/core/` | Object system, flags, properties, I/O, global state |
| `src/parser/` | Tokenization, verb recognition, syntax patterns, disambiguation |
| `src/verbs/` | All verb handler implementations |
| `src/world/` | Room definitions, object definitions, world initialization |
| `src/systems/` | Timer, combat, light/darkness, scoring, save/restore |

## Development

See [DEVELOPER_GUIDE.md](DEVELOPER_GUIDE.md) for:
- ZIL to C++ translation patterns
- Adding new rooms, objects, and verbs
- Code conventions and architecture
- Testing guidelines

## Testing

The project includes comprehensive tests:
- **Unit tests** - Core systems (objects, parser, rooms)
- **Integration tests** - Command sequences and game flow
- **Transcript tests** - Verified against original game behavior
- **Puzzle tests** - All puzzles are solvable
- **Performance tests** - Command response time validation

Run all tests:
```bash
cd build
ctest --output-on-failure
```

## Original Source

Based on the official Zork I source code (~12,000 lines of ZIL across 9 files), released as open source by Activision. The original ZIL files are included in the `zil/` directory for reference.

## License

This port is based on the Zork I source code released as open source. See [LICENSE](LICENSE) for details.

## Acknowledgments

- **Infocom** - Original Zork creators (Marc Blank, Dave Lebling, et al.)
- **Activision** - For releasing the source code
- **MIT** - Where Zork was originally developed
