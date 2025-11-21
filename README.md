# Zork I - C++ Port

This is a C++ port of the original Zork I source code, released as open source by Microsoft.

## Project Structure

- `src/core/` - Core game engine (objects, properties, flags)
- `src/parser/` - Text parser and command processing
- `src/verbs/` - Verb implementations
- `src/world/` - Rooms, objects, and game world
- `src/actions/` - Object-specific action handlers
- `src/main.cpp` - Entry point and main game loop

## Building

```bash
mkdir build && cd build
cmake ..
make
./zork1
```

## Original Source

Based on the official Zork I source code (~12K lines of ZIL across 9 files).
