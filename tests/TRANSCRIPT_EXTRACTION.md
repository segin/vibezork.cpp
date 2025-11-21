# Transcript Test Extraction Guide

## Purpose

This document describes how to extract test commands from documented Zork playthroughs to create automated integration tests.

## Source Material

The MIT transcript (https://web.mit.edu/marleigh/www/portfolio/Files/zork/transcript.html) provides a complete playthrough of Zork I. We can use this to:

1. Verify our implementation matches the original game behavior
2. Create regression tests
3. Ensure puzzles are solvable
4. Test edge cases and interactions

## Extraction Process

### Step 1: Identify Test Scenarios

Break the transcript into logical test scenarios:
- **Opening sequence**: Initial commands (look, examine, inventory)
- **Navigation**: Moving between rooms
- **Object manipulation**: Taking, dropping, examining items
- **Puzzle solutions**: Specific puzzle-solving sequences
- **NPC interactions**: Dealing with thief, troll, etc.
- **Winning sequence**: Final steps to complete the game

### Step 2: Extract Commands

For each scenario, extract the player commands (lines starting with ">"):

Example from transcript:
```
> look
> open mailbox
> take leaflet
> read leaflet
```

### Step 3: Identify Key Outputs

For each command, identify key phrases in the output that verify correct behavior:

```cpp
{"look", {"open field", "white house", "mailbox"}},
{"open mailbox", {"Opening", "leaflet"}},
{"take leaflet", {"Taken"}},
{"read leaflet", {"WELCOME TO ZORK"}}
```

### Step 4: Create Test Cases

Structure as TranscriptStep entries:

```cpp
TEST(OpeningSequence) {
    std::vector<TranscriptStep> steps = {
        {"look", {"open field", "white house", "mailbox"}},
        {"open mailbox", {"Opening", "leaflet"}},
        {"take leaflet", {"Taken"}},
        {"read leaflet", {"WELCOME TO ZORK"}},
    };
    runTranscript(steps);
}
```

## Test Categories

### Category 1: Basic Mechanics (Priority: HIGH)
- Movement commands
- Object examination
- Inventory management
- Basic interactions

### Category 2: Puzzle Solutions (Priority: HIGH)
- Each major puzzle as a separate test
- Verify puzzle can be solved
- Test alternate solutions if they exist

### Category 3: NPC Behavior (Priority: MEDIUM)
- Thief encounters
- Troll bridge
- Cyclops interaction
- Other NPCs

### Category 4: Edge Cases (Priority: MEDIUM)
- Invalid commands
- Impossible actions
- State-dependent behaviors
- Death scenarios

### Category 5: Full Walkthrough (Priority: LOW)
- Complete game from start to finish
- Verify 350 points achievable
- Long-running test (several minutes)

## Example Test Extraction

From the MIT transcript, here's a sample extraction:

### Original Transcript Excerpt
```
West of House
You are standing in an open field west of a white house, with a boarded front door.
There is a small mailbox here.

> open mailbox
Opening the small mailbox reveals a leaflet.

> take leaflet
Taken.

> read leaflet
"WELCOME TO ZORK!
[... rest of leaflet text ...]"
```

### Extracted Test
```cpp
TEST(MailboxSequence) {
    std::vector<TranscriptStep> steps = {
        {"open mailbox", {"Opening", "small mailbox", "leaflet"}},
        {"take leaflet", {"Taken"}},
        {"read leaflet", {"WELCOME TO ZORK"}},
    };
    runTranscript(steps);
}
```

## Guidelines

### DO:
- Extract command sequences that test specific features
- Verify key output phrases (not entire text)
- Test both success and failure cases
- Group related commands into logical tests
- Document what each test verifies

### DON'T:
- Copy entire text outputs verbatim (copyright concern)
- Create tests that are too granular (one command per test)
- Test implementation details (test behavior, not code)
- Include player commentary or meta-text
- Assume specific timing or randomness

## Test Data Format

Store extracted test data in a structured format:

```cpp
// tests/transcript_data.h
namespace TranscriptData {
    const std::vector<TranscriptStep> OPENING_SEQUENCE = {
        {"look", {"open field", "white house"}},
        {"inventory", {"empty-handed"}},
        // ... more steps
    };
    
    const std::vector<TranscriptStep> TROLL_PUZZLE = {
        {"go to troll bridge", {}},
        {"attack troll with sword", {"troll", "dead"}},
        {"east", {"East-West Passage"}},
        // ... more steps
    };
}
```

## Validation

Before committing test data:

1. **Verify commands are valid**: Each command should be parseable
2. **Check output phrases**: Phrases should be distinctive and stable
3. **Test independence**: Each test should set up its own state
4. **Run against original**: If possible, verify against actual Zork I

## Future Enhancements

- Automated transcript parsing tool
- State snapshot/restore for test setup
- Performance benchmarking
- Randomness seeding for deterministic tests
- Coverage analysis to identify untested areas

## Contributing

When adding transcript tests:

1. Document the source of the commands
2. Explain what the test verifies
3. Keep tests focused and maintainable
4. Update this guide with new patterns or insights
