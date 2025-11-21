# Zork C++ Test Suite

## Overview

This test suite verifies the correctness of the Zork I C++ port by testing at multiple levels:

1. **Unit Tests** - Test individual components (objects, rooms, parser)
2. **Integration Tests** - Test game mechanics and interactions
3. **Transcript Tests** - Replay documented game sessions

## Test Files

- `test_framework.h` - Simple test framework with assertions
- `game_tester.h` - Helper for testing game commands
- `basic_tests.cpp` - Unit tests for core systems
- `transcript_tests.cpp` - Integration tests based on playthroughs

## Building Tests

```bash
cd zork_cpp/build
cmake .. -DBUILD_TESTS=ON
make
```

## Running Tests

```bash
# Run all tests
./tests/basic_tests
./tests/transcript_tests

# Or use ctest
ctest --verbose
```

## Test Strategy

### Unit Tests (basic_tests.cpp)

Test individual components in isolation:
- Object creation, flags, properties
- Room creation, exits, connections
- Parser tokenization and verb lookup
- Direction handling

### Transcript Tests (transcript_tests.cpp)

Replay actual game sessions to verify end-to-end behavior:
- Opening sequence (look, examine mailbox, etc.)
- Basic navigation around the house
- Entering the house
- Solving puzzles
- Full game walkthrough

## Adding New Tests

Use the TEST macro to define a new test:

```cpp
TEST(MyNewTest) {
    // Setup
    ZObject obj(1, "test");
    
    // Execute
    obj.setFlag(ObjectFlag::TAKEBIT);
    
    // Verify
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
}
```

## Transcript Test Data

Transcript tests are based on documented playthroughs. When adding transcript tests:

1. Extract commands from the playthrough
2. Identify key output phrases to verify
3. Create TranscriptStep entries
4. Run and verify behavior matches original

Example sources for test data:
- Community walkthroughs (publicly available)
- InvisiClues solutions (puzzle hints)
- Player-created guides

**Note**: We only test game behavior and mechanics, not copyrighted text verbatim. Tests verify that commands work correctly and produce appropriate responses.

## Test Coverage Goals

- **Phase 1**: Core systems (objects, rooms, parser) - 90%+ coverage
- **Phase 2**: Verb implementations - 80%+ coverage  
- **Phase 3**: Object actions - 70%+ coverage
- **Phase 4**: Full game walkthrough passes

## Continuous Integration

Tests should be run:
- Before each commit
- On pull requests
- Nightly builds
- Before releases

## Known Issues

- Transcript tests are placeholders until game engine is more complete
- Need to implement game state capture/restore for testing
- Performance tests not yet implemented
