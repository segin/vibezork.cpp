# Zork I Complete Port - Specification

This directory contains the complete specification for porting Zork I from ZIL to C++17.

## Files

### requirements.md
**100 requirements** organized by functional area, following EARS patterns and INCOSE quality rules.

Coverage:
- Parser system (Requirements 1-9)
- Object system (Requirements 10-13)
- Room system (Requirements 14-19)
- Verb handlers (Requirements 20-34)
- Action handlers (Requirements 35-43)
- Timer system (Requirements 44-49)
- Light/darkness (Requirements 50-51)
- Scoring (Requirements 52-54)
- Combat (Requirements 55-57)
- Death/resurrection (Requirements 58-59)
- Save/restore (Requirements 60-62)
- Game mechanics (Requirements 63-85)
- Technical requirements (Requirements 86-95)
- Completion criteria (Requirements 96-100)

### design.md
Comprehensive technical design covering:
- High-level architecture
- Component interfaces (ZObject, ZRoom, Parser, etc.)
- Data models (flags, properties, verb IDs)
- System interactions
- Error handling strategy
- Testing strategy
- Performance considerations
- File organization
- Build system
- ZIL to C++ migration guide

### tasks.md
**80 top-level tasks** with **405 total sub-tasks** organized into 7 phases:

1. **Parser Enhancement (Tasks 1-7)**: Syntax patterns, object recognition, disambiguation
2. **World Building - Rooms (Tasks 8-15)**: All ~100 rooms with exits and descriptions
3. **World Building - Objects (Tasks 16-24)**: All ~150 objects with properties and behaviors
4. **Core Verbs (Tasks 25-38)**: All verb handlers for player commands
5. **Object and Room Actions (Tasks 39-46)**: Object-specific and room-specific behaviors
6. **Game Systems (Tasks 47-65)**: Timers, scoring, combat, light/darkness, save/restore
7. **Testing and Polish (Tasks 66-80)**: Comprehensive testing, optimization, final polish

## Execution Order

Tasks are numbered for sequential execution (1 → 80). Each task builds on previous tasks, ensuring:
- No orphaned code
- Working game at each stage
- Incremental progress
- Clear dependencies

## Optional Tasks

Tasks marked with `*` suffix are optional (primarily unit tests). These can be skipped to focus on core functionality, though they're recommended for quality assurance.

## Statistics

- **Requirements**: 100 (all with acceptance criteria)
- **Top-level tasks**: 80
- **Total sub-tasks**: 405
- **Phases**: 7
- **Estimated lines of code**: ~10,000-15,000 C++
- **Original ZIL code**: ~12,000 lines across 9 files

## Coverage

This spec covers the complete Zork I port:
- ✓ ~100 rooms (all areas: exterior, forest, house, underground, maze, special)
- ✓ ~150 objects (treasures, tools, containers, NPCs, scenery, puzzles)
- ✓ ~80 verbs (movement, manipulation, interaction, combat, meta-game)
- ✓ All game systems (timers, scoring, combat, light/darkness, save/restore)
- ✓ All puzzles and NPC behaviors
- ✓ Complete testing strategy

## Usage

1. **Review requirements.md** to understand what needs to be built
2. **Review design.md** to understand how to build it
3. **Execute tasks.md** sequentially, marking tasks complete as you go
4. Use the task status tool to track progress

## Success Criteria

The port is complete when:
1. All 100 requirements are met
2. All 80 top-level tasks are complete
3. Game is playable from start to finish
4. All puzzles are solvable
5. Maximum score (350 points) is achievable
6. Text output matches original exactly
7. All tests pass
8. Code is clean and maintainable

## Next Steps

Start with Task 1: "Enhance parser with syntax pattern system"

This is the foundation that everything else builds on. The parser must be robust before adding more content.
