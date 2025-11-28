# Implementation Plan

This task list implements the complete Zork I C++ port in execution order. Each task builds on previous tasks, ensuring no orphaned code. Tasks are numbered for sequential execution.

**Legend**:
- `[ ]` - Not started
- `[-]` - In progress  
- `[x]` - Complete
- `*` suffix - Optional task (can be skipped)

---

## Phase 1: Parser Enhancement (Tasks 1-25)

The parser is the foundation - it must be robust before we can add more content.

- [x] 1. Enhance parser with syntax pattern system
  - Implement SyntaxPattern class to represent verb syntax rules
  - Support pattern elements: VERB, OBJECT, PREPOSITION, DIRECTION
  - Support optional elements and multiple prepositions
  - Support object flag requirements (FIND TAKEBIT, etc.)
  - _Requirements: 6_

- [x] 1.1 Create SyntaxPattern class structure
  - Define ElementType enum (VERB, OBJECT, PREPOSITION, DIRECTION)
  - Define Element struct with type, values, requiredFlag, optional
  - Implement SyntaxPattern constructor
  - _Requirements: 6_

- [x] 1.2 Implement pattern matching logic
  - Implement matches() method to check if tokens match pattern
  - Handle optional elements correctly
  - Handle multiple preposition choices
  - _Requirements: 6_

- [x] 1.3 Implement pattern application
  - Implement apply() method to extract objects from matched pattern
  - Set PRSO and PRSI based on pattern
  - Return ParseResult with extracted information
  - _Requirements: 6_

- [x] 1.4 Write unit tests for SyntaxPattern
  - Test simple patterns (VERB OBJECT)
  - Test patterns with prepositions (VERB OBJECT PREP OBJECT)
  - Test optional elements
  - Test flag requirements
  - _Requirements: 6, 92_

- [x] 2. Implement VerbRegistry for synonym mapping
  - Create VerbRegistry class
  - Implement registerVerb() to map synonyms to verb IDs
  - Implement registerSyntax() to associate patterns with verbs
  - Implement lookupVerb() for synonym resolution
  - Implement getSyntaxPatterns() to get patterns for a verb
  - _Requirements: 2_

- [x] 2.1 Register all verb synonyms from gsyntax.zil
  - Add TAKE/GET/GRAB/CARRY synonyms
  - Add EXAMINE/X/LOOK AT/DESCRIBE synonyms
  - Add DROP/PUT DOWN/RELEASE synonyms
  - Add all other verb synonyms from original
  - _Requirements: 2_

- [x] 2.2 Register all syntax patterns from gsyntax.zil
  - Add patterns for manipulation verbs (TAKE, DROP, PUT, etc.)
  - Add patterns for examination verbs (EXAMINE, READ, etc.)
  - Add patterns for interaction verbs (OPEN, CLOSE, LOCK, etc.)
  - Add patterns for combat verbs (ATTACK WITH, THROW AT, etc.)
  - _Requirements: 6_


- [x] 3. Enhance object recognition with synonyms and adjectives
  - Modify Parser::findObjects() to match by synonyms
  - Add support for adjective matching
  - Support multi-word object names
  - Prioritize objects by location (room > inventory > elsewhere)
  - _Requirements: 4, 5, 77_

- [x] 3.1 Implement synonym matching
  - Check object synonyms against input words
  - Handle partial matches when unambiguous
  - Return all matching objects for disambiguation
  - _Requirements: 4_

- [x] 3.2 Implement adjective matching
  - Check object adjectives against input words
  - Support adjective + noun combinations
  - Support multiple adjectives (SMALL BRASS LANTERN)
  - Narrow matches when adjectives are used
  - _Requirements: 5, 77_

- [x] 3.3 Implement location-based prioritization
  - Prioritize objects in current room
  - Then objects in player inventory
  - Then objects in open containers
  - Exclude objects in closed containers
  - Exclude objects in other rooms
  - _Requirements: 79_

- [x] 3.4 Write unit tests for object recognition
  - Test synonym matching
  - Test adjective matching
  - Test multi-word names
  - Test location prioritization
  - _Requirements: 4, 5, 77, 92_

- [x] 4. Implement disambiguation system
  - Implement Parser::disambiguate() method
  - Display "Which [noun] do you mean?" prompt
  - List matching objects for player to choose
  - Parse player's clarification response
  - Retry original command with disambiguated object
  - _Requirements: 8_

- [x] 4.1 Create disambiguation prompt
  - Format list of matching objects
  - Display object descriptions for clarity
  - Accept player's choice (number or name)
  - _Requirements: 8_

- [x] 4.2 Handle disambiguation response
  - Parse player's clarification
  - Match response to one of the candidates
  - Update PRSO/PRSI with selected object
  - Retry the original command
  - _Requirements: 8_

- [x] 4.3 Write unit tests for disambiguation
  - Test with multiple matching objects
  - Test player selection by number
  - Test player selection by name
  - Test invalid disambiguation responses
  - _Requirements: 8, 92_

- [x] 5. Implement preposition handling
  - Parse prepositions from input (WITH, IN, ON, TO, FROM, etc.)
  - Identify indirect object (PRSI) after preposition
  - Validate preposition is valid for the verb
  - Support optional prepositions
  - _Requirements: 7_

- [x] 5.1 Add preposition recognition
  - Create list of valid prepositions
  - Identify preposition position in tokens
  - Extract words after preposition as PRSI
  - _Requirements: 7_

- [x] 5.2 Validate preposition for verb
  - Check if verb accepts the given preposition
  - Display error for invalid prepositions
  - Support verbs with multiple valid prepositions
  - _Requirements: 7_

- [x] 5.3 Write unit tests for prepositions
  - Test "PUT OBJECT IN CONTAINER"
  - Test "ATTACK TROLL WITH SWORD"
  - Test invalid prepositions
  - Test optional prepositions
  - _Requirements: 7, 92_

- [x] 6. Implement special parser features
  - Implement "all" and "all except" support
  - Implement AGAIN/G command
  - Implement OOPS correction
  - Implement IT/THEM pronoun tracking
  - _Requirements: 9, 74, 75, 76, 78_

- [x] 6.1 Implement "all" support
  - Recognize "all" keyword in input
  - Find all applicable objects for the verb
  - Apply verb to each object in sequence
  - Report results for each object
  - _Requirements: 78_

- [x] 6.2 Implement "all except" support
  - Recognize "all except [object]" pattern
  - Find all applicable objects except the specified one
  - Apply verb to each object
  - _Requirements: 78_

- [x] 6.3 Implement AGAIN command
  - Track last command entered
  - When player types AGAIN or G, repeat last command
  - Don't repeat AGAIN itself
  - Handle AGAIN on first turn gracefully
  - _Requirements: 75_

- [x] 6.4 Implement OOPS correction
  - Track last unknown word
  - When player types OOPS [word], replace unknown word
  - Retry corrected command automatically
  - Only work immediately after unknown word error
  - _Requirements: 76_

- [x] 6.5 Implement pronoun tracking
  - Track last mentioned object for IT
  - Track last mentioned objects for THEM
  - Substitute pronouns with tracked objects
  - Update pronouns after each command
  - _Requirements: 74_

- [x] 6.6 Write unit tests for special features
  - Test "take all"
  - Test "take all except lamp"
  - Test AGAIN command
  - Test OOPS correction
  - Test IT/THEM pronouns
  - _Requirements: 9, 74, 75, 76, 78, 92_


- [x] 7. Integrate enhanced parser with main loop
  - Update main loop to use new parser features
  - Handle ParseResult errors appropriately
  - Display helpful error messages
  - Test parser with various command types
  - _Requirements: 1, 73_

- [x] 7.1 Write integration tests for parser
  - Test complete command parsing flow
  - Test error handling
  - Test edge cases (empty input, very long input, special characters)
  - _Requirements: 72, 73, 93_

---

## Phase 2: World Building - Rooms (Tasks 8-35)

Now that the parser is robust, we need content for it to operate on. Start with rooms since objects need locations.

- [x] 8. Create room ID constants for all areas
  - Define RoomIds namespace with all ~100 room IDs
  - Organize by area (exterior, forest, house, underground, maze, special)
  - Use constexpr for compile-time constants
  - Use numeric ranges to group related rooms
  - _Requirements: 17_

- [x] 8.1 Define exterior room IDs
  - WEST_OF_HOUSE, NORTH_OF_HOUSE, SOUTH_OF_HOUSE, BEHIND_HOUSE
  - STONE_BARROW, CLEARING, FOREST_PATH
  - _Requirements: 17_

- [x] 8.2 Define forest room IDs
  - FOREST_1, FOREST_2, FOREST_3, FOREST_4
  - Additional forest areas from 1dungeon.zil
  - _Requirements: 17_

- [x] 8.3 Define house interior room IDs
  - LIVING_ROOM, KITCHEN, ATTIC, CELLAR
  - TROPHY_ROOM, STUDIO, GALLERY
  - All other house rooms from 1dungeon.zil
  - _Requirements: 17_

- [x] 8.4 Define underground room IDs
  - EAST_WEST_PASSAGE, ROUND_ROOM, LOUD_ROOM
  - NORTH_SOUTH_PASSAGE, CHASM, RESERVOIR
  - All other underground rooms from 1dungeon.zil
  - _Requirements: 17_

- [x] 8.5 Define maze room IDs
  - MAZE_1 through MAZE_15
  - GRATING_ROOM, GRATING_CLEARING
  - _Requirements: 17, 84_

- [x] 8.6 Define special area room IDs
  - TREASURE_ROOM, ENTRANCE_TO_HADES
  - LAND_OF_LIVING_DEAD, TEMPLE
  - All other special rooms from 1dungeon.zil
  - _Requirements: 17_

- [x] 9. Implement exterior rooms (West/North/South/Behind House area)
  - Create all exterior rooms with descriptions
  - Set up exits between exterior rooms
  - Add room flags (RLANDBIT, ONBIT, SACREDBIT)
  - Implement room actions where needed
  - _Requirements: 14, 15, 17_

- [x] 9.1 Expand West of House room
  - Already exists, verify completeness
  - Ensure all exits are correct
  - Verify description matches original
  - _Requirements: 14, 15_

- [x] 9.2 Expand North of House room
  - Already exists, verify completeness
  - Add path to forest
  - Verify all exits
  - _Requirements: 14, 15_

- [x] 9.3 Expand South of House room
  - Already exists, verify completeness
  - Add window details
  - Verify all exits
  - _Requirements: 14, 15_

- [x] 9.4 Create Behind House room
  - Add description
  - Set up exits to other exterior rooms
  - Add window to kitchen
  - _Requirements: 14, 15_

- [x] 9.5 Create Stone Barrow room
  - Already exists, verify completeness
  - Ensure entrance to underground is correct
  - _Requirements: 14, 15_

- [x] 10. Implement forest rooms
  - Create FOREST_1 through FOREST_4
  - Create CLEARING and FOREST_PATH
  - Set up confusing forest navigation
  - Add appropriate descriptions
  - _Requirements: 14, 15, 17_

- [x] 10.1 Create forest rooms with descriptions
  - FOREST_1 (already exists, verify)
  - FOREST_2, FOREST_3, FOREST_4
  - Make descriptions similar but subtly different
  - _Requirements: 14, 15_

- [x] 10.2 Create CLEARING room
  - Add description
  - Connect to forest and house areas
  - Add grating entrance to underground
  - _Requirements: 14, 15_

- [x] 10.3 Create FOREST_PATH room
  - Add description
  - Connect to north of house and forest
  - _Requirements: 14, 15_

- [x] 10.4 Set up forest navigation
  - Create confusing exits between forest rooms
  - Some exits loop back to same room
  - Some exits lead to unexpected rooms
  - _Requirements: 14, 81_

- [x] 11. Implement house interior rooms
  - Create living room, kitchen, attic, cellar
  - Create trophy room, studio, gallery
  - Set up exits between house rooms
  - Add appropriate descriptions and flags
  - _Requirements: 14, 15, 17_

- [x] 11.1 Create Living Room
  - Add detailed description
  - Add exits to other house rooms
  - Add trophy case location
  - Set RLANDBIT, ONBIT, SACREDBIT flags
  - _Requirements: 14, 15_

- [x] 11.2 Create Kitchen
  - Add description
  - Add exits (including window to outside)
  - Add table and other scenery
  - _Requirements: 14, 15_

- [x] 11.3 Create Attic
  - Add description
  - Add rope and table
  - Add exit down to house
  - _Requirements: 14, 15_

- [x] 11.4 Create Cellar
  - Add description
  - Add trap door entrance
  - Connect to underground passages
  - _Requirements: 14, 15_

- [x] 11.5 Create additional house rooms
  - Trophy Room, Studio, Gallery
  - All other house rooms from 1dungeon.zil
  - Set up exits between all house rooms
  - _Requirements: 14, 15, 17_


- [x] 12. Implement underground rooms (passages and chambers)
  - Create all underground passage rooms
  - Create special chambers (Round Room, Loud Room, etc.)
  - Set up complex underground navigation
  - Add appropriate lighting flags
  - _Requirements: 14, 15, 17_

- [x] 12.1 Create main passages
  - EAST_WEST_PASSAGE, NORTH_SOUTH_PASSAGE
  - Connect to cellar and other areas
  - Set lighting flags (most are dark)
  - _Requirements: 14, 15_

- [x] 12.2 Create Round Room
  - Add description with multiple exits
  - Connect to many other rooms
  - Add appropriate flags
  - _Requirements: 14, 15_

- [x] 12.3 Create Loud Room
  - Add description
  - Implement echo action handler
  - Connect to adjacent rooms
  - _Requirements: 14, 15, 43_

- [x] 12.4 Create remaining underground rooms
  - Chasm, Reservoir, Dam, Maintenance Room
  - Troll Room, Maze entrance areas
  - All other underground rooms from 1dungeon.zil
  - Set up all exits and connections
  - _Requirements: 14, 15, 17_

- [x] 13. Implement maze rooms
  - Create MAZE_1 through MAZE_15
  - Set up confusing maze navigation
  - Make rooms look similar
  - Add correct exits to maze solution
  - _Requirements: 14, 15, 17, 84_

- [x] 13.1 Create all maze rooms
  - MAZE_1 through MAZE_15
  - Use similar descriptions
  - Set appropriate flags
  - _Requirements: 14, 15, 84_

- [x] 13.2 Set up maze navigation
  - Create non-intuitive connections
  - Some exits loop back
  - Some exits lead to dead ends
  - Implement correct solution path
  - _Requirements: 14, 81, 84_

- [x] 13.3 Add maze entrance and exit
  - Connect maze to main underground
  - Add grating room connections
  - _Requirements: 14, 84_

- [x] 14. Implement special area rooms
  - Create Treasure Room (thief's lair)
  - Create Entrance to Hades
  - Create Temple, Altar, and other special rooms
  - Add unique descriptions and behaviors
  - _Requirements: 14, 15, 17_

- [x] 14.1 Create Treasure Room
  - Add description
  - Set up as thief's lair
  - Add special access requirements
  - _Requirements: 14, 15, 43_

- [x] 14.2 Create Entrance to Hades
  - Add description
  - Implement resurrection mechanics location
  - Add special room action
  - _Requirements: 14, 15, 43, 59_

- [x] 14.3 Create Temple and religious areas
  - Temple, Altar, Egyptian Room
  - Add appropriate descriptions
  - Set SACREDBIT flag where appropriate
  - _Requirements: 14, 15_

- [x] 14.4 Create remaining special rooms
  - All other unique rooms from 1dungeon.zil
  - Implement special behaviors
  - _Requirements: 14, 15, 17_

- [x] 15. Implement conditional and special exits
  - Add locked door exits
  - Add exits requiring special actions (CLIMB, ENTER)
  - Add one-way exits
  - Add exits with custom messages
  - _Requirements: 81, 82_

- [x] 15.1 Implement door exits
  - Doors that can be opened/closed
  - Locked doors requiring keys
  - Check door state before allowing passage
  - _Requirements: 81_

- [x] 15.2 Implement special movement exits
  - CLIMB exits (trees, stairs, etc.)
  - ENTER exits (house, boat, etc.)
  - Exits requiring specific verbs
  - _Requirements: 82_

- [x] 15.3 Implement conditional exits
  - Exits that check game state
  - Exits that require items
  - Exits that require puzzle solutions
  - _Requirements: 81_

- [x] 15.4 Write unit tests for room navigation
  - Test basic directional movement
  - Test conditional exits
  - Test special movement
  - Test blocked exits
  - _Requirements: 14, 81, 82, 92_

---

## Phase 3: World Building - Objects (Tasks 16-45)

With rooms in place, add all objects that populate the world.

- [x] 16. Create object ID constants for all types
  - Define ObjectIds namespace with all ~150 object IDs
  - Organize by type (treasures, tools, containers, NPCs, scenery)
  - Use constexpr for compile-time constants
  - Use numeric ranges to group related objects
  - _Requirements: 18_

- [x] 16.1 Define treasure object IDs
  - TROPHY, EGG, CHALICE, TRIDENT, PAINTING
  - COFFIN_TREASURE, JEWELS, COINS, etc.
  - All treasures from 1dungeon.zil
  - _Requirements: 18_

- [x] 16.2 Define tool object IDs
  - SWORD, LAMP, ROPE, KNIFE, WRENCH, SCREWDRIVER
  - SHOVEL, TORCH, CANDLES, MATCHES
  - All tools from 1dungeon.zil
  - _Requirements: 18_

- [x] 16.3 Define container object IDs
  - MAILBOX, TROPHY_CASE, COFFIN, BASKET, SACK
  - BOTTLE, CASE, BAG
  - All containers from 1dungeon.zil
  - _Requirements: 18_

- [x] 16.4 Define NPC object IDs
  - THIEF, TROLL, CYCLOPS
  - GRUE (for darkness)
  - Any other NPCs from 1dungeon.zil
  - _Requirements: 18_

- [x] 16.5 Define scenery object IDs
  - WHITE_HOUSE, BOARD, WINDOW, DOOR
  - TREE, RUG, PAINTING_WALL
  - All scenery objects from 1dungeon.zil
  - _Requirements: 18_

- [x] 16.6 Define special object IDs
  - BOAT, PUMP, MACHINE, MIRROR
  - BUTTON, SWITCH, LEVER
  - All special puzzle objects from 1dungeon.zil
  - _Requirements: 18_


- [x] 17. Implement treasure objects
  - Create all treasure objects with descriptions
  - Set TRYTAKEBIT and TAKEBIT flags
  - Set VALUE properties for scoring
  - Add synonyms and adjectives
  - Place in initial locations
  - _Requirements: 18, 19, 38_

- [x] 17.1 Create basic treasures
  - TROPHY, EGG, CHALICE, TRIDENT
  - Set descriptions, synonyms, flags
  - Set point values
  - _Requirements: 18, 38_

- [x] 17.2 Create jewel treasures
  - JEWELS, COINS, DIAMONDS, EMERALD
  - Set descriptions, synonyms, flags
  - Set point values
  - _Requirements: 18, 38_

- [x] 17.3 Create special treasures
  - PAINTING (can be taken from wall, has back side)
  - COFFIN (treasure and container)
  - Other treasures with special behaviors
  - _Requirements: 18, 38, 42_

- [x] 17.4 Place all treasures in initial locations
  - Distribute treasures throughout the world
  - Match original placement from 1dungeon.zil
  - _Requirements: 19_

- [x] 18. Implement tool objects
  - Create all tool objects with descriptions
  - Set appropriate flags (TAKEBIT, etc.)
  - Set properties (STRENGTH for weapons, etc.)
  - Add synonyms and adjectives
  - Place in initial locations
  - _Requirements: 18, 19, 37_

- [x] 18.1 Create weapon tools
  - SWORD (glows near enemies, weapon)
  - KNIFE (weapon, less effective than sword)
  - Set TAKEBIT flag and STRENGTH property
  - _Requirements: 18, 37_

- [x] 18.2 Create light source tools
  - LAMP (battery-powered light)
  - TORCH (temporary light)
  - CANDLES (burn down over time)
  - MATCHES (one-time use)
  - Set LIGHTBIT flag
  - _Requirements: 18, 37_

- [x] 18.3 Create utility tools
  - ROPE (climbing, tying)
  - WRENCH (for bolts)
  - SCREWDRIVER (for screws)
  - SHOVEL (digging)
  - Set TAKEBIT flag
  - _Requirements: 18, 37_

- [x] 18.4 Create special tools
  - PUMP (inflate/deflate boat)
  - BOTTLE (container and tool)
  - Other special tools from 1dungeon.zil
  - _Requirements: 18, 37_

- [x] 18.5 Place all tools in initial locations
  - Distribute tools throughout the world
  - Match original placement from 1dungeon.zil
  - _Requirements: 19_

- [x] 19. Implement container objects
  - Create all container objects with descriptions
  - Set CONTBIT flag
  - Set CAPACITY properties
  - Set OPENBIT flag for initially open containers
  - Add synonyms and adjectives
  - Place in initial locations
  - _Requirements: 18, 19, 36_

- [x] 19.1 Create MAILBOX container
  - Already exists, verify completeness
  - Ensure TRYTAKEBIT flag (anchored)
  - Contains leaflet initially
  - _Requirements: 18, 36_

- [x] 19.2 Create TROPHY_CASE container
  - Large capacity for treasures
  - Set CONTBIT and OPENBIT flags
  - Located in living room
  - _Requirements: 18, 36_

- [x] 19.3 Create portable containers
  - SACK, BAG, BASKET
  - Set TAKEBIT and CONTBIT flags
  - Set appropriate capacities
  - _Requirements: 18, 36_

- [x] 19.4 Create special containers
  - COFFIN (treasure and container)
  - BOTTLE (liquid container)
  - Other special containers from 1dungeon.zil
  - _Requirements: 18, 36_

- [x] 19.5 Place all containers in initial locations
  - Distribute containers throughout the world
  - Match original placement from 1dungeon.zil
  - _Requirements: 19_

- [x] 20. Implement NPC objects
  - Create all NPC objects with descriptions
  - Set appropriate flags (FIGHTBIT for hostile NPCs)
  - Set properties (STRENGTH, health, etc.)
  - Add synonyms and adjectives
  - Place in initial locations
  - _Requirements: 18, 19, 39, 40, 41_

- [x] 20.1 Create THIEF NPC
  - Add description
  - Set FIGHTBIT flag
  - Set STRENGTH property
  - Add synonyms (thief, robber)
  - Initial location varies (wandering)
  - _Requirements: 18, 39_

- [x] 20.2 Create TROLL NPC
  - Add description
  - Set FIGHTBIT flag
  - Set STRENGTH property (stronger than thief)
  - Add synonyms (troll)
  - Located at troll room initially
  - _Requirements: 18, 40_

- [x] 20.3 Create CYCLOPS NPC
  - Add description
  - Set FIGHTBIT flag initially
  - Set STRENGTH property
  - Add synonyms (cyclops, giant)
  - Located at cyclops room initially
  - _Requirements: 18, 41_

- [x] 20.4 Create GRUE object
  - Add description (never seen, only felt)
  - Used for darkness attacks
  - Not a physical object in the world
  - _Requirements: 18, 51_

- [x] 21. Implement scenery objects
  - Create all scenery objects with descriptions
  - Set TRYTAKEBIT flag (cannot be taken)
  - Add synonyms and adjectives
  - Place in appropriate rooms
  - _Requirements: 18, 19_

- [x] 21.1 Create house scenery
  - WHITE_HOUSE (already exists, verify)
  - BOARD (already exists, verify)
  - WINDOW, DOOR, CHIMNEY
  - _Requirements: 18_

- [x] 21.2 Create natural scenery
  - TREE, FOREST (already exists, verify)
  - RIVER, STREAM, RAINBOW
  - MOUNTAIN, SKY, GROUND
  - _Requirements: 18_

- [x] 21.3 Create interior scenery
  - RUG, PAINTING_WALL, MANTLE
  - TABLE, CHAIR, CHANDELIER
  - All other interior scenery from 1dungeon.zil
  - _Requirements: 18_

- [x] 21.4 Place all scenery in appropriate rooms
  - Match original placement from 1dungeon.zil
  - _Requirements: 19_


- [x] 22. Implement special puzzle objects
  - Create all puzzle objects with descriptions
  - Set appropriate flags and properties
  - Add synonyms and adjectives
  - Place in initial locations
  - _Requirements: 18, 19, 42_

- [x] 22.1 Create BOAT and PUMP
  - BOAT (inflatable, vehicle)
  - PUMP (inflates/deflates boat)
  - Set appropriate flags
  - Located at dam/reservoir area
  - _Requirements: 18, 42, 83_

- [x] 22.2 Create MACHINE puzzle
  - MACHINE (complex button puzzle)
  - BUTTONS, SWITCHES, LEVERS
  - Set up puzzle state
  - Located in machine room
  - _Requirements: 18, 42_

- [x] 22.3 Create MIRROR puzzle
  - MIRROR (shows different reflections)
  - Set up special behavior
  - Located in mirror room
  - _Requirements: 18, 42_

- [x] 22.4 Create other puzzle objects
  - All remaining puzzle objects from 1dungeon.zil
  - Set up special behaviors
  - Place in appropriate locations
  - _Requirements: 18, 42_

- [x] 23. Implement readable objects
  - Create objects with text (LEAFLET, BOOK, etc.)
  - Set TEXT property with readable content
  - Add READ verb support
  - _Requirements: 18, 25_

- [x] 23.1 Create LEAFLET
  - Add "WELCOME TO ZORK!" text
  - Set TEXT property
  - Initially in mailbox
  - _Requirements: 18, 25_

- [x] 23.2 Create BOOK and GUIDEBOOK
  - Add readable text
  - Set TEXT property
  - Place in appropriate locations
  - _Requirements: 18, 25_

- [x] 23.3 Create other readable objects
  - NEWSPAPER, LABEL, INSCRIPTION
  - All other readable objects from 1dungeon.zil
  - Set TEXT properties
  - _Requirements: 18, 25_

- [ ] 24. Set up initial game state
  - Place player at WEST_OF_HOUSE
  - Set initial object locations
  - Set initial flags and properties
  - Initialize global state
  - _Requirements: 19_

- [x] 24.1 Initialize player state
  - Set player location to WEST_OF_HOUSE
  - Set player as winner (current actor)
  - Initialize inventory as empty
  - _Requirements: 19_

- [x] 24.2 Verify all object placements
  - Check all objects are in correct initial locations
  - Verify containment relationships
  - Ensure no orphaned objects
  - _Requirements: 19_

- [x] 24.3 Set initial flags
  - Set ONBIT for lit rooms
  - Set OPENBIT for initially open containers
  - Set other initial flags from 1dungeon.zil
  - _Requirements: 19_

- [x] 24.4 Write unit tests for world initialization
  - Test all rooms exist
  - Test all objects exist
  - Test initial placements
  - Test initial flags
  - _Requirements: 19, 92_

---

## Phase 4: Core Verbs (Tasks 25-50)

With the world populated, implement the verbs that operate on it.

- [x] 25. Implement examination verbs
  - Implement EXAMINE verb
  - Implement LOOK-INSIDE verb
  - Implement SEARCH verb
  - Implement READ verb
  - Add pre-verb checks
  - _Requirements: 25_

- [x] 25.1 Implement EXAMINE verb
  - Display detailed object description
  - Show object state (open/closed, on/off, etc.)
  - Call object action handler if present
  - _Requirements: 25_

- [x] 25.2 Implement LOOK-INSIDE verb
  - Check if object is a container
  - Check if container is open or transparent
  - List contents of container
  - _Requirements: 25_

- [x] 25.3 Implement SEARCH verb
  - Similar to LOOK-INSIDE but more thorough
  - May reveal hidden objects
  - Call object action handler if present
  - _Requirements: 25_

- [x] 25.4 Implement READ verb
  - Check if object has TEXT property
  - Display text content
  - Handle objects that can't be read
  - _Requirements: 25_

- [x] 25.5 Write unit tests for examination verbs
  - Test EXAMINE on various objects
  - Test LOOK-INSIDE on containers
  - Test READ on readable objects
  - _Requirements: 25, 92_

- [x] 26. Implement TAKE verb with full validation
  - Check if object has TAKEBIT flag
  - Check if object has TRYTAKEBIT flag (anchored)
  - Check inventory weight limits
  - Check if object is accessible
  - Call object action handler if present
  - Move object to player inventory
  - _Requirements: 21, 63, 80_

- [x] 26.1 Implement PRE-TAKE checks
  - Verify object exists and is visible
  - Check TAKEBIT flag
  - Check TRYTAKEBIT flag (refuse if set)
  - Check inventory weight limit
  - _Requirements: 21, 34, 63, 80_

- [x] 26.2 Implement TAKE verb handler
  - Call object action handler first
  - If not handled, perform default take
  - Move object to player inventory
  - Display success message
  - _Requirements: 21_

- [x] 26.3 Write unit tests for TAKE verb
  - Test taking normal objects
  - Test taking anchored objects (should fail)
  - Test weight limit
  - Test taking from containers
  - _Requirements: 21, 63, 92_

- [x] 27. Implement DROP verb
  - Check if object is in inventory
  - Move object to current room
  - Display success message
  - _Requirements: 21_

- [x] 27.1 Implement PRE-DROP checks
  - Verify object is in inventory
  - Check if drop is allowed in current room
  - _Requirements: 21, 34_

- [x] 27.2 Implement DROP verb handler
  - Call object action handler first
  - If not handled, perform default drop
  - Move object to current room
  - Display success message
  - _Requirements: 21_

- [x] 27.3 Write unit tests for DROP verb
  - Test dropping carried objects
  - Test dropping objects not in inventory (should fail)
  - _Requirements: 21, 92_


- [x] 28. Implement PUT verb (put object in container)
  - Check if indirect object is a container
  - Check if container is open
  - Check container capacity
  - Move object to container
  - _Requirements: 22, 64_

- [x] 28.1 Implement PRE-PUT checks
  - Verify direct object is takeable
  - Verify indirect object has CONTBIT flag
  - Check if container is open
  - Check container capacity
  - _Requirements: 22, 34, 64_

- [x] 28.2 Implement PUT verb handler
  - Call object action handlers
  - If not handled, perform default put
  - Move object to container
  - Display success message
  - _Requirements: 22_

- [x] 28.3 Write unit tests for PUT verb
  - Test putting objects in containers
  - Test putting in closed containers (should fail)
  - Test capacity limits
  - _Requirements: 22, 64, 92_

- [x] 29. Implement OPEN and CLOSE verbs
  - Check if object has CONTBIT flag
  - Set or clear OPENBIT flag
  - Reveal contents when opening
  - _Requirements: 23_

- [x] 29.1 Implement PRE-OPEN checks
  - Verify object has CONTBIT flag
  - Check if already open
  - Check if locked
  - _Requirements: 23, 34_

- [x] 29.2 Implement OPEN verb handler
  - Call object action handler first
  - If not handled, set OPENBIT flag
  - Display contents if any
  - Display success message
  - _Requirements: 23_

- [x] 29.3 Implement PRE-CLOSE checks
  - Verify object has CONTBIT flag
  - Check if already closed
  - _Requirements: 23, 34_

- [x] 29.4 Implement CLOSE verb handler
  - Call object action handler first
  - If not handled, clear OPENBIT flag
  - Display success message
  - _Requirements: 23_

- [x] 29.5 Write unit tests for OPEN and CLOSE verbs
  - Test opening and closing containers
  - Test opening locked containers (should fail)
  - Test opening already open containers
  - _Requirements: 23, 92_

- [x] 30. Implement LOCK and UNLOCK verbs
  - Check if object can be locked
  - Check for correct key
  - Set or clear LOCKEDBIT flag
  - _Requirements: 24_

- [x] 30.1 Implement PRE-LOCK checks
  - Verify object can be locked
  - Check if already locked
  - Verify player has correct key
  - _Requirements: 24, 34_

- [x] 30.2 Implement LOCK verb handler
  - Call object action handler first
  - If not handled, set LOCKEDBIT flag
  - Display success message
  - _Requirements: 24_

- [x] 30.3 Implement PRE-UNLOCK checks
  - Verify object is locked
  - Verify player has correct key
  - _Requirements: 24, 34_

- [x] 30.4 Implement UNLOCK verb handler
  - Call object action handler first
  - If not handled, clear LOCKEDBIT flag
  - Display success message
  - _Requirements: 24_

- [x] 30.5 Write unit tests for LOCK and UNLOCK verbs
  - Test locking and unlocking
  - Test without correct key (should fail)
  - _Requirements: 24, 92_

- [x] 31. Implement manipulation verbs (TURN, PUSH, PULL, MOVE)
  - Implement TURN verb for rotating objects
  - Implement PUSH verb
  - Implement PULL verb
  - Implement MOVE verb
  - Most behavior is object-specific
  - _Requirements: 26_

- [x] 31.1 Implement TURN verb
  - Call object action handler
  - Default: "Nothing happens"
  - _Requirements: 26_

- [x] 31.2 Implement PUSH verb
  - Call object action handler
  - Default: "Nothing happens"
  - _Requirements: 26_

- [x] 31.3 Implement PULL verb
  - Call object action handler
  - Default: "Nothing happens"
  - _Requirements: 26_

- [x] 31.4 Implement MOVE verb
  - Call object action handler
  - Default: "Nothing happens"
  - _Requirements: 26_

- [x] 31.5 Write unit tests for manipulation verbs
  - Test on objects with special behaviors
  - Test on objects without special behaviors
  - _Requirements: 26, 92_

- [x] 32. Implement interaction verbs (TIE, LISTEN, SMELL, TOUCH)
  - Implement TIE and UNTIE verbs
  - Implement LISTEN verb
  - Implement SMELL verb
  - Implement TOUCH verb
  - Most behavior is object-specific
  - _Requirements: 27_

- [x] 32.1 Implement TIE verb
  - Check if objects can be tied
  - Call object action handlers
  - Default: "You can't tie that"
  - _Requirements: 27_

- [x] 32.2 Implement UNTIE verb
  - Check if objects are tied
  - Call object action handlers
  - Default: "It's not tied"
  - _Requirements: 27_

- [x] 32.3 Implement LISTEN verb
  - Call room action handler
  - Call object action handler if object specified
  - Default: "You hear nothing unusual"
  - _Requirements: 27_

- [x] 32.4 Implement SMELL verb
  - Call object action handler if object specified
  - Default: "You smell nothing unusual"
  - _Requirements: 27_

- [x] 32.5 Implement TOUCH verb
  - Call object action handler
  - Default: "You feel nothing unusual"
  - _Requirements: 27_

- [x] 32.6 Write unit tests for interaction verbs
  - Test on objects with special behaviors
  - Test default behaviors
  - _Requirements: 27, 92_

- [x] 33. Implement consumption verbs (EAT, DRINK)
  - Implement EAT verb
  - Implement DRINK verb
  - Remove consumed items from game
  - _Requirements: 28_

- [x] 33.1 Implement EAT verb
  - Check if object has FOODBIT flag
  - Call object action handler
  - Remove object from game if consumed
  - Default: "That's not edible"
  - _Requirements: 28_

- [x] 33.2 Implement DRINK verb
  - Check if object has DRINKBIT flag
  - Call object action handler
  - Remove object from game if consumed
  - Default: "That's not drinkable"
  - _Requirements: 28_

- [x] 33.3 Write unit tests for consumption verbs
  - Test eating food items
  - Test eating non-food items (should fail)
  - Test drinking liquids
  - _Requirements: 28, 92_


- [x] 34. Implement light source verbs (LAMP-ON, LAMP-OFF, LIGHT)
  - Implement LAMP-ON verb
  - Implement LAMP-OFF verb
  - Implement LIGHT as synonym
  - Set ONBIT flag appropriately
  - Integrate with light system
  - _Requirements: 30_

- [x] 34.1 Implement LAMP-ON verb
  - Check if object has LIGHTBIT flag
  - Check if lamp has battery/fuel
  - Set ONBIT flag
  - Update room lighting
  - Display success message
  - _Requirements: 30_

- [x] 34.2 Implement LAMP-OFF verb
  - Check if object has LIGHTBIT flag
  - Check if lamp is on
  - Clear ONBIT flag
  - Update room lighting
  - Display success message
  - _Requirements: 30_

- [x] 34.3 Write unit tests for light verbs
  - Test turning lamp on and off
  - Test with depleted battery
  - Test on non-light objects
  - _Requirements: 30, 92_

- [x] 35. Implement special action verbs (INFLATE, DEFLATE, PRAY, etc.)
  - Implement INFLATE verb
  - Implement DEFLATE verb
  - Implement PRAY verb
  - Implement EXORCISE verb
  - Implement WAVE, RUB, RING verbs
  - _Requirements: 31_

- [x] 35.1 Implement INFLATE verb
  - Check for pump and boat
  - Call object action handler
  - Default: "You can't inflate that"
  - _Requirements: 31_

- [x] 35.2 Implement DEFLATE verb
  - Check for pump and boat
  - Call object action handler
  - Default: "You can't deflate that"
  - _Requirements: 31_

- [x] 35.3 Implement PRAY verb
  - Call room action handler
  - Default: "Your prayers are not answered"
  - _Requirements: 31_

- [x] 35.4 Implement EXORCISE verb
  - Call object action handler
  - Default: "Nothing happens"
  - _Requirements: 31_

- [x] 35.5 Implement WAVE, RUB, RING verbs
  - Call object action handlers
  - Provide appropriate default messages
  - _Requirements: 31_

- [x] 35.6 Write unit tests for special action verbs
  - Test on appropriate objects
  - Test default behaviors
  - _Requirements: 31, 92_

- [x] 36. Implement combat verbs (ATTACK, KILL, THROW, SWING)
  - Implement ATTACK verb
  - Implement KILL as synonym
  - Implement THROW verb
  - Implement SWING verb
  - Integrate with combat system
  - _Requirements: 29_

- [x] 36.1 Implement ATTACK verb
  - Check if target is attackable
  - Check for weapon
  - Start combat if not already in combat
  - Call combat system
  - _Requirements: 29_

- [x] 36.2 Implement THROW verb
  - Check if object is throwable
  - Calculate throw damage
  - Apply damage to target
  - Move object to target's location
  - _Requirements: 29_

- [x] 36.3 Implement SWING verb
  - Check for weapon
  - Similar to ATTACK
  - _Requirements: 29_

- [x] 36.4 Write unit tests for combat verbs
  - Test attacking NPCs
  - Test with and without weapons
  - Test throwing objects
  - _Requirements: 29, 92_

- [x] 37. Implement meta-game verbs (SCORE, DIAGNOSE, VERBOSE, etc.)
  - Implement SCORE verb
  - Implement DIAGNOSE verb
  - Implement VERBOSE verb
  - Implement BRIEF verb
  - Implement SUPERBRIEF verb
  - _Requirements: 32, 65, 66, 67, 68_

- [x] 37.1 Implement SCORE verb
  - Display current score
  - Display move count
  - Display rank
  - _Requirements: 32_

- [x] 37.2 Implement DIAGNOSE verb
  - Display player health status
  - List injuries if any
  - _Requirements: 68_

- [x] 37.3 Implement VERBOSE verb
  - Set verbose mode flag
  - Display confirmation message
  - _Requirements: 65_

- [x] 37.4 Implement BRIEF verb
  - Set brief mode flag
  - Display confirmation message
  - _Requirements: 66_

- [x] 37.5 Implement SUPERBRIEF verb
  - Set superbrief mode flag
  - Display confirmation message
  - _Requirements: 67_

- [x] 37.6 Write unit tests for meta-game verbs
  - Test SCORE display
  - Test DIAGNOSE
  - Test mode switching
  - _Requirements: 32, 65, 66, 67, 68, 92_

- [x] 38. Implement game control verbs (SAVE, RESTORE, RESTART, QUIT, VERSION)
  - Implement SAVE verb
  - Implement RESTORE verb
  - Implement RESTART verb
  - Implement QUIT verb (already exists, verify)
  - Implement VERSION verb
  - _Requirements: 33, 60, 61, 62, 69, 70_

- [x] 38.1 Implement SAVE verb
  - Prompt for filename
  - Serialize game state
  - Write to file
  - Handle errors gracefully
  - _Requirements: 60_

- [x] 38.2 Implement RESTORE verb
  - Prompt for filename
  - Read from file
  - Deserialize game state
  - Handle errors gracefully
  - _Requirements: 61_

- [x] 38.3 Implement RESTART verb
  - Confirm with player
  - Reset all game state
  - Reinitialize world
  - _Requirements: 62_

- [x] 38.4 Verify QUIT verb
  - Already implemented, ensure it works correctly
  - Confirm before quitting
  - Remind to save
  - _Requirements: 70_

- [x] 38.5 Implement VERSION verb
  - Display game name and version
  - Display port information
  - Display copyright
  - _Requirements: 69_

- [x] 38.6 Write unit tests for game control verbs
  - Test save and restore
  - Test restart
  - Test version display
  - _Requirements: 60, 61, 62, 69, 92_

---

## Phase 5: Object and Room Actions (Tasks 39-65)

Implement object-specific and room-specific behaviors.

- [x] 39. Implement container action handlers
  - Implement mailbox action (already exists, verify)
  - Implement trophy case action
  - Implement coffin action
  - Implement basket action
  - Implement sack action
  - _Requirements: 36_

- [x] 39.1 Verify mailbox action
  - Already implemented, ensure completeness
  - Anchored (can't be taken)
  - Opens to reveal leaflet
  - _Requirements: 36_

- [x] 39.2 Implement trophy case action
  - Large capacity
  - Scores treasures when placed inside
  - Display special message for treasures
  - _Requirements: 36, 85_

- [x] 39.3 Implement coffin action
  - Container and treasure
  - Special opening behavior
  - _Requirements: 36_

- [x] 39.4 Implement basket action
  - Part of balloon/basket system
  - Special behaviors when in balloon
  - _Requirements: 36_

- [x] 39.5 Implement other container actions
  - Sack, bag, bottle, etc.
  - Any special behaviors from 1actions.zil
  - _Requirements: 36_


- [x] 40. Implement tool action handlers
  - Implement sword action
  - Implement lamp action
  - Implement rope action
  - Implement wrench action
  - Implement screwdriver action
  - _Requirements: 37_

- [x] 40.1 Implement sword action
  - Glows when near enemies
  - Effective weapon in combat
  - Special messages for glow
  - _Requirements: 37, 49_

- [x] 40.2 Implement lamp action
  - Provides light when on
  - Battery drains over time
  - Warnings when battery low
  - Can't be turned on when depleted
  - _Requirements: 37, 47_

- [x] 40.3 Implement rope action
  - Used for climbing
  - Used for tying
  - Special behaviors in specific locations
  - _Requirements: 37_

- [x] 40.4 Implement wrench and screwdriver actions
  - Used for specific puzzles
  - Special behaviors with bolts/screws
  - _Requirements: 37_

- [x] 40.5 Implement other tool actions
  - Shovel, torch, candles, matches
  - All other tools from 1actions.zil
  - _Requirements: 37_

- [x] 41. Implement treasure action handlers
  - Implement special treasure behaviors
  - Implement painting action (has back side)
  - Implement egg action (opens)
  - Implement other treasure-specific actions
  - _Requirements: 38_

- [x] 41.1 Implement painting action
  - Can be taken from wall
  - Has back side with different description
  - Special EXAMINE behavior
  - _Requirements: 38, 42_

- [x] 41.2 Implement egg action
  - Can be opened
  - Contains canary
  - Special behaviors
  - _Requirements: 38, 42_

- [x] 41.3 Implement other treasure actions
  - Any special behaviors from 1actions.zil
  - Most treasures have default behavior
  - _Requirements: 38_

- [x] 42. Implement thief NPC actions
  - Implement thief wandering behavior
  - Implement thief stealing behavior
  - Implement thief combat behavior
  - Implement thief treasure room behavior
  - Implement thief death behavior
  - _Requirements: 39_

- [x] 42.1 Implement thief wandering
  - Thief moves randomly through accessible rooms
  - Appears and disappears
  - Triggered by timer
  - _Requirements: 39, 45_

- [x] 42.2 Implement thief stealing
  - Thief steals objects from player
  - Takes treasures preferentially
  - Moves stolen items to treasure room
  - _Requirements: 39_

- [x] 42.3 Implement thief combat
  - Thief fights back when attacked
  - Uses knife as weapon
  - Can flee when wounded
  - _Requirements: 39, 55_

- [x] 42.4 Implement thief treasure room
  - Thief's lair contains stolen items
  - Special behavior in treasure room
  - _Requirements: 39_

- [x] 42.5 Implement thief death
  - Thief drops all stolen items
  - Thief stops wandering
  - Treasure room becomes accessible
  - _Requirements: 39_

- [x] 43. Implement troll NPC actions
  - Implement troll blocking behavior
  - Implement troll combat behavior
  - Implement troll death behavior
  - _Requirements: 40_

- [x] 43.1 Implement troll blocking
  - Troll blocks passage at bridge
  - Prevents player from crossing
  - Attacks if player tries to pass
  - _Requirements: 40_

- [x] 43.2 Implement troll combat
  - Troll fights back when attacked
  - Strong opponent
  - Can be killed with sword
  - _Requirements: 40, 55_

- [x] 43.3 Implement troll death
  - Troll vanishes when killed
  - Bridge becomes passable
  - Troll drops axe
  - _Requirements: 40_

- [x] 44. Implement cyclops NPC actions
  - Implement cyclops blocking behavior
  - Implement cyclops hunger behavior
  - Implement cyclops eating behavior
  - Implement cyclops leaving behavior
  - _Requirements: 41_

- [x] 44.1 Implement cyclops blocking
  - Cyclops blocks passage
  - Initially hostile
  - _Requirements: 41_

- [x] 44.2 Implement cyclops hunger
  - Cyclops is hungry
  - Accepts food offerings
  - _Requirements: 41_

- [x] 44.3 Implement cyclops eating
  - Cyclops eats offered food
  - Becomes non-hostile when fed
  - Leaves the area
  - _Requirements: 41_

- [x] 44.4 Implement cyclops combat
  - Can be fought if not fed
  - Very strong opponent
  - _Requirements: 41, 55_

- [x] 45. Implement puzzle object actions
  - Implement boat and pump actions
  - Implement machine puzzle actions
  - Implement mirror actions
  - Implement other puzzle-specific actions
  - _Requirements: 42_

- [x] 45.1 Implement boat and pump actions
  - Pump inflates and deflates boat
  - Boat must be inflated to use
  - Boat allows water navigation
  - _Requirements: 42, 83_

- [x] 45.2 Implement machine puzzle actions
  - Complex button/switch puzzle
  - Multiple states and combinations
  - Produces specific results
  - _Requirements: 42_

- [x] 45.3 Implement mirror actions
  - Shows different reflections
  - Special EXAMINE behavior
  - Part of puzzle solution
  - _Requirements: 42_

- [x] 45.4 Implement other puzzle actions
  - All remaining puzzle objects from 1actions.zil
  - Implement specific behaviors
  - _Requirements: 42_

- [x] 46. Implement special room actions
  - Implement loud room action (echo)
  - Implement round room action
  - Implement entrance to hades action
  - Implement treasure room action
  - Implement maze room actions
  - _Requirements: 43_

- [x] 46.1 Implement loud room action
  - Echoes sounds
  - Special LISTEN behavior
  - Special YELL behavior
  - _Requirements: 43_

- [x] 46.2 Implement round room action
  - Special acoustics
  - Multiple exits
  - _Requirements: 43_

- [x] 46.3 Implement entrance to hades action
  - Resurrection location
  - Special behaviors
  - _Requirements: 43, 59_

- [x] 46.4 Implement treasure room action
  - Thief's lair
  - Contains stolen treasures
  - Special access requirements
  - _Requirements: 43_

- [x] 46.5 Implement maze room actions
  - Confusing navigation
  - Similar descriptions
  - _Requirements: 43, 84_

- [x] 46.6 Implement other special room actions
  - All remaining special rooms from 1actions.zil
  - Implement specific behaviors
  - _Requirements: 43_


---

## Phase 6: Game Systems (Tasks 47-70)

Implement the core game systems that tie everything together.

- [x] 47. Implement timer system foundation
  - Create TimerSystem class
  - Implement timer registration
  - Implement timer enable/disable
  - Implement tick() method for main loop
  - Support one-shot and repeating timers
  - _Requirements: 44_

- [x] 47.1 Create TimerSystem class structure
  - Define Timer struct with interval, counter, callback
  - Implement timer storage (map by name)
  - _Requirements: 44_

- [x] 47.2 Implement timer registration
  - registerTimer() method
  - Support interval and callback
  - Support repeating vs one-shot
  - _Requirements: 44_

- [x] 47.3 Implement timer control
  - enableTimer() and disableTimer() methods
  - Check enabled flag before firing
  - _Requirements: 44_

- [x] 47.4 Implement timer tick
  - Called from main loop each turn
  - Decrement counters
  - Fire callbacks when counter reaches zero
  - Reset repeating timers
  - _Requirements: 44_

- [x] 47.5 Write unit tests for timer system
  - Test timer registration
  - Test timer firing
  - Test enable/disable
  - Test repeating timers
  - _Requirements: 44, 92_

- [x] 48. Implement thief timer (I-THIEF)
  - Register thief movement timer
  - Implement thief movement callback
  - Move thief to random accessible room
  - Implement thief stealing when in same room
  - Implement thief attacks
  - _Requirements: 45_

- [x] 48.1 Register thief timer
  - Set appropriate interval
  - Enable when game starts
  - Disable when thief is dead
  - _Requirements: 45_

- [x] 48.2 Implement thief movement
  - Choose random accessible room
  - Move thief to that room
  - Display message if player sees thief
  - _Requirements: 45_

- [x] 48.3 Implement thief stealing
  - Check if thief is in same room as player
  - Steal random object from player or room
  - Prefer treasures
  - Move stolen item to treasure room
  - _Requirements: 45_

- [x] 48.4 Implement thief attacks
  - Occasionally attack player
  - Use combat system
  - _Requirements: 45, 55_

- [x] 49. Implement combat timer (I-FIGHT)
  - Register combat round timer
  - Implement combat round callback
  - Process one round of combat
  - Calculate attacks and damage
  - End combat when appropriate
  - _Requirements: 46_

- [x] 49.1 Register combat timer
  - Enable when combat starts
  - Disable when combat ends
  - _Requirements: 46_

- [x] 49.2 Implement combat round processing
  - Player attacks enemy
  - Enemy attacks player
  - Calculate damage
  - Apply damage to combatants
  - Check for death
  - _Requirements: 46, 55_

- [ ] 50. Implement lamp timer (I-LANTERN)
  - Register lamp battery timer
  - Decrement battery when lamp is on
  - Warn when battery is low
  - Turn off lamp when battery depleted
  - _Requirements: 47_

- [ ] 50.1 Register lamp timer
  - Enable when lamp is turned on
  - Disable when lamp is turned off
  - _Requirements: 47_

- [ ] 50.2 Implement battery drain
  - Decrement battery counter
  - Check battery level
  - _Requirements: 47_

- [ ] 50.3 Implement low battery warning
  - Warn at specific battery levels
  - "The lamp is getting dim"
  - _Requirements: 47_

- [ ] 50.4 Implement battery depletion
  - Turn off lamp when battery reaches zero
  - Make lamp unusable
  - Display message
  - _Requirements: 47_

- [ ] 51. Implement candle timer (I-CANDLES)
  - Register candle burning timer
  - Decrement candle duration when lit
  - Warn when candles are nearly gone
  - Remove candles when burned out
  - _Requirements: 48_

- [ ] 51.1 Register candle timer
  - Enable when candles are lit
  - Disable when candles burn out
  - _Requirements: 48_

- [ ] 51.2 Implement candle burning
  - Decrement duration counter
  - Check duration level
  - _Requirements: 48_

- [ ] 51.3 Implement candle warnings
  - Warn when nearly gone
  - "The candles are getting short"
  - _Requirements: 48_

- [ ] 51.4 Implement candle burnout
  - Remove candles from game
  - Update lighting
  - Display message
  - _Requirements: 48_

- [ ] 52. Implement sword glow timer (I-SWORD)
  - Register sword glow check timer
  - Check for nearby enemies
  - Make sword glow when enemies near
  - Stop glow when enemies leave
  - _Requirements: 49_

- [ ] 52.1 Register sword glow timer
  - Enable when player has sword
  - Check periodically
  - _Requirements: 49_

- [ ] 52.2 Implement enemy proximity check
  - Check current room for enemies
  - Check adjacent rooms for enemies
  - _Requirements: 49_

- [ ] 52.3 Implement sword glow
  - Set glow flag when enemies near
  - Clear glow flag when enemies leave
  - Display messages for state changes
  - _Requirements: 49_

- [ ] 53. Implement light and darkness system
  - Create LightSystem class
  - Implement room lighting checks
  - Implement light source tracking
  - Implement lighting updates
  - Integrate with main loop
  - _Requirements: 50_

- [ ] 53.1 Create LightSystem class
  - Track light sources
  - Track room lighting state
  - _Requirements: 50_

- [ ] 53.2 Implement isRoomLit() method
  - Check if room has ONBIT flag (naturally lit)
  - Check for light sources in room
  - Check for light sources in player inventory
  - Return true if any light present
  - _Requirements: 50_

- [ ] 53.3 Implement hasLightSource() method
  - Check player inventory for lit light sources
  - Check current room for lit light sources
  - _Requirements: 50_

- [ ] 53.4 Implement updateLighting() method
  - Called when light sources change
  - Update room descriptions based on lighting
  - _Requirements: 50_

- [ ] 53.5 Write unit tests for light system
  - Test room lighting checks
  - Test light source detection
  - Test lighting updates
  - _Requirements: 50, 92_

- [ ] 54. Implement grue attacks
  - Track darkness duration
  - Warn about grue in darkness
  - Increase danger over time
  - Kill player if too long in darkness
  - _Requirements: 51_

- [ ] 54.1 Implement darkness tracking
  - Increment counter when in darkness
  - Reset counter when in light
  - _Requirements: 51_

- [ ] 54.2 Implement grue warnings
  - First warning: "It is pitch black. You are likely to be eaten by a grue."
  - Subsequent warnings increase urgency
  - _Requirements: 51_

- [ ] 54.3 Implement grue attack
  - After several turns in darkness, grue attacks
  - Kill player
  - Display death message
  - _Requirements: 51, 58_

- [ ] 54.4 Write unit tests for grue
  - Test darkness warnings
  - Test grue attack
  - Test light prevents grue
  - _Requirements: 51, 92_


- [ ] 55. Implement score system
  - Create ScoreSystem class
  - Implement score tracking
  - Implement move counting
  - Implement rank calculation
  - Implement treasure scoring
  - _Requirements: 52, 53, 54, 85_

- [ ] 55.1 Create ScoreSystem class
  - Track current score
  - Track move count
  - Track scored treasures
  - _Requirements: 52, 53_

- [ ] 55.2 Implement score tracking
  - addScore() method
  - getScore() method
  - Maximum score constant (350)
  - _Requirements: 52, 85_

- [ ] 55.3 Implement move counting
  - incrementMoves() method
  - getMoves() method
  - Exclude certain commands from count
  - _Requirements: 53_

- [ ] 55.4 Implement rank calculation
  - Define rank thresholds
  - calculateRank() method
  - getRank() method
  - Ranks: Beginner, Amateur, Novice, etc. up to Master Adventurer
  - _Requirements: 54_

- [ ] 55.5 Implement treasure scoring
  - markTreasureScored() method
  - isTreasureScored() method
  - Prevent double-scoring
  - Award points when treasure placed in trophy case
  - _Requirements: 85_

- [ ] 55.6 Write unit tests for score system
  - Test score tracking
  - Test move counting
  - Test rank calculation
  - Test treasure scoring
  - _Requirements: 52, 53, 54, 85, 92_

- [ ] 56. Implement combat system
  - Create CombatSystem class
  - Implement combat state tracking
  - Implement attack calculations
  - Implement damage application
  - Implement combat resolution
  - _Requirements: 55, 56, 57_

- [ ] 56.1 Create CombatSystem class
  - Define Combatant struct
  - Track player and enemy combatants
  - Track combat state
  - _Requirements: 55_

- [ ] 56.2 Implement combat start/end
  - startCombat() method
  - endCombat() method
  - isInCombat() method
  - _Requirements: 55_

- [ ] 56.3 Implement attack calculation
  - calculateDamage() method
  - attackSucceeds() method
  - Factor in weapon effectiveness
  - Factor in combatant strength
  - _Requirements: 55, 56_

- [ ] 56.4 Implement damage application
  - Apply damage to health
  - Check for death
  - Handle enemy fleeing
  - Handle enemy dropping items
  - _Requirements: 55, 57_

- [ ] 56.5 Implement combat round processing
  - processCombatRound() method
  - Player attacks enemy
  - Enemy attacks player
  - Display combat messages
  - _Requirements: 55_

- [ ] 56.6 Write unit tests for combat system
  - Test combat start/end
  - Test attack calculations
  - Test damage application
  - Test combat rounds
  - _Requirements: 55, 56, 57, 92_

- [ ] 57. Implement weapon effectiveness
  - Define weapon strength values
  - Implement weapon lookup
  - Use weapon strength in damage calculation
  - Sword is most effective
  - Knife is less effective
  - Bare hands are least effective
  - _Requirements: 56_

- [ ] 57.1 Define weapon properties
  - Set STRENGTH property for weapons
  - Sword: high strength
  - Knife: medium strength
  - Bare hands: low strength
  - _Requirements: 56_

- [ ] 57.2 Implement weapon effectiveness in combat
  - Look up weapon strength
  - Use in damage calculation
  - Display appropriate messages
  - _Requirements: 56_

- [ ] 58. Implement enemy behavior
  - Implement enemy attack logic
  - Implement enemy strength variation
  - Implement enemy fleeing
  - Implement enemy death
  - _Requirements: 57_

- [ ] 58.1 Implement enemy attacks
  - Enemy attacks player during combat
  - Use enemy strength in calculation
  - Display attack messages
  - _Requirements: 57_

- [ ] 58.2 Implement enemy strength
  - Troll is stronger than thief
  - Cyclops is very strong
  - Set STRENGTH property appropriately
  - _Requirements: 57_

- [ ] 58.3 Implement enemy fleeing
  - Enemy flees when badly wounded
  - Move to adjacent room
  - End combat
  - _Requirements: 57_

- [ ] 58.4 Implement enemy death
  - Set DEADBIT flag
  - Drop carried items
  - Remove from game or make inactive
  - Display death message
  - _Requirements: 57_

- [ ] 59. Implement death and game over
  - Implement JIGS-UP function
  - Display death messages
  - Offer resurrection or restart
  - Track death count
  - _Requirements: 58_

- [ ] 59.1 Implement JIGS-UP function
  - Called when player dies
  - Display appropriate death message
  - Offer options (resurrect, restart, quit)
  - _Requirements: 58_

- [ ] 59.2 Implement death scenarios
  - Combat death
  - Grue death
  - Falling death
  - Other death scenarios from original
  - _Requirements: 58_

- [ ] 59.3 Implement death tracking
  - Increment death counter
  - Limit resurrections based on deaths
  - _Requirements: 58_

- [ ] 60. Implement resurrection mechanic
  - Move player to Entrance to Hades
  - Scatter player's inventory
  - Limit number of resurrections
  - Make resurrection unavailable after multiple deaths
  - _Requirements: 59_

- [ ] 60.1 Implement resurrection offer
  - Display resurrection option after death
  - Accept player's choice
  - _Requirements: 59_

- [ ] 60.2 Implement resurrection process
  - Move player to Entrance to Hades
  - Scatter inventory items
  - Restore player health
  - Continue game
  - _Requirements: 59_

- [ ] 60.3 Implement resurrection limits
  - Track number of resurrections
  - Deny resurrection after limit
  - Force restart or quit
  - _Requirements: 59_

- [ ] 61. Implement save game system
  - Create SaveSystem class
  - Implement game state serialization
  - Write to file
  - Handle errors gracefully
  - _Requirements: 60_

- [ ] 61.1 Create SaveSystem class
  - Define save file format
  - Implement serialization methods
  - _Requirements: 60_

- [ ] 61.2 Implement state serialization
  - Serialize player location and inventory
  - Serialize all object locations and states
  - Serialize all flags and properties
  - Serialize score and moves
  - Serialize timer states
  - _Requirements: 60_

- [ ] 61.3 Implement file writing
  - Write serialized state to file
  - Handle file I/O errors
  - Display success/error messages
  - _Requirements: 60_

- [ ] 61.4 Write unit tests for save system
  - Test serialization
  - Test file writing
  - Test error handling
  - _Requirements: 60, 92_


- [ ] 62. Implement restore game system
  - Implement game state deserialization
  - Read from file
  - Restore all game state
  - Handle errors gracefully
  - _Requirements: 61_

- [ ] 62.1 Implement state deserialization
  - Deserialize player location and inventory
  - Deserialize all object locations and states
  - Deserialize all flags and properties
  - Deserialize score and moves
  - Deserialize timer states
  - _Requirements: 61_

- [ ] 62.2 Implement file reading
  - Read from save file
  - Handle file not found
  - Handle corrupt files
  - Handle version mismatches
  - _Requirements: 61_

- [ ] 62.3 Implement state restoration
  - Apply deserialized state to game
  - Verify state consistency
  - Display success/error messages
  - _Requirements: 61_

- [ ] 62.4 Write unit tests for restore system
  - Test deserialization
  - Test file reading
  - Test error handling
  - Test round-trip (save then restore)
  - _Requirements: 61, 92_

- [ ] 63. Implement inventory weight limits
  - Define maximum inventory weight
  - Calculate total carried weight
  - Enforce weight limit in TAKE verb
  - Display appropriate messages
  - _Requirements: 63_

- [ ] 63.1 Define weight system
  - Set SIZE property for all objects
  - Define maximum carry weight
  - _Requirements: 63_

- [ ] 63.2 Implement weight calculation
  - Sum SIZE of all carried objects
  - Check against maximum
  - _Requirements: 63_

- [ ] 63.3 Enforce weight limit
  - Check in TAKE verb
  - Refuse if would exceed limit
  - Display "You're carrying too much"
  - _Requirements: 63_

- [ ] 63.4 Write unit tests for weight limits
  - Test weight calculation
  - Test weight limit enforcement
  - Test taking heavy objects
  - _Requirements: 63, 92_

- [ ] 64. Implement container capacity limits
  - Set CAPACITY property for containers
  - Calculate total size in container
  - Enforce capacity in PUT verb
  - Display appropriate messages
  - _Requirements: 64_

- [ ] 64.1 Set container capacities
  - Set CAPACITY property for all containers
  - Trophy case: large capacity
  - Mailbox: small capacity
  - Other containers: appropriate capacities
  - _Requirements: 64_

- [ ] 64.2 Implement capacity calculation
  - Sum SIZE of objects in container
  - Check against CAPACITY
  - _Requirements: 64_

- [ ] 64.3 Enforce capacity limit
  - Check in PUT verb
  - Refuse if would exceed capacity
  - Display "There's no room"
  - _Requirements: 64_

- [ ] 64.4 Write unit tests for capacity limits
  - Test capacity calculation
  - Test capacity enforcement
  - Test putting large objects in small containers
  - _Requirements: 64, 92_

- [ ] 65. Implement description modes (verbose, brief, superbrief)
  - Implement mode tracking
  - Implement verbose mode behavior
  - Implement brief mode behavior
  - Implement superbrief mode behavior
  - Integrate with room descriptions
  - _Requirements: 65, 66, 67_

- [ ] 65.1 Implement mode tracking
  - Track current mode in Globals
  - Default to verbose mode
  - _Requirements: 65_

- [ ] 65.2 Implement verbose mode
  - Display full room description every time
  - Display all objects
  - Display all exits
  - _Requirements: 65_

- [ ] 65.3 Implement brief mode
  - Display full description for unvisited rooms
  - Display short description for visited rooms
  - Display objects and exits
  - _Requirements: 66_

- [ ] 65.4 Implement superbrief mode
  - Display only room name
  - Omit object lists
  - Omit exit lists
  - _Requirements: 67_

- [ ] 65.5 Integrate with room display
  - Check mode when displaying room
  - Format output accordingly
  - _Requirements: 65, 66, 67_

- [ ] 65.6 Write unit tests for description modes
  - Test verbose mode
  - Test brief mode
  - Test superbrief mode
  - Test mode switching
  - _Requirements: 65, 66, 67, 92_

---

## Phase 7: Testing and Polish (Tasks 66-80)

Comprehensive testing and final polish to ensure quality.

- [ ] 66. Create comprehensive unit test suite
  - Test object system thoroughly
  - Test parser thoroughly
  - Test all verb handlers
  - Test all game systems
  - Achieve 90%+ code coverage
  - _Requirements: 92_

- [ ] 66.1 Expand object system tests
  - Test all flag operations
  - Test all property operations
  - Test containment operations
  - Test object creation and destruction
  - _Requirements: 92_

- [ ] 66.2 Expand parser tests
  - Test all tokenization cases
  - Test all verb recognition
  - Test all object recognition
  - Test all syntax patterns
  - Test all special features
  - _Requirements: 92_

- [ ] 66.3 Expand verb tests
  - Test all verb handlers
  - Test pre-verb checks
  - Test error cases
  - Test edge cases
  - _Requirements: 92_

- [ ] 66.4 Expand system tests
  - Test timer system
  - Test score system
  - Test combat system
  - Test light system
  - Test save/restore system
  - _Requirements: 92_

- [ ] 67. Create integration test suite
  - Test complete command sequences
  - Test puzzle solutions
  - Test NPC interactions
  - Test game state transitions
  - _Requirements: 93_

- [ ] 67.1 Test command sequences
  - Test multi-step actions
  - Test state changes
  - Test object interactions
  - _Requirements: 93_

- [ ] 67.2 Test puzzle solutions
  - Test each major puzzle
  - Verify solutions work
  - Test alternative approaches
  - _Requirements: 93, 97_

- [ ] 67.3 Test NPC interactions
  - Test thief behavior
  - Test troll behavior
  - Test cyclops behavior
  - Test combat scenarios
  - _Requirements: 93_

- [ ] 67.4 Test game state persistence
  - Test save and restore
  - Test restart
  - Test state consistency
  - _Requirements: 93_

- [ ] 68. Create transcript-based tests
  - Extract commands from MIT transcript
  - Create test cases from transcript
  - Verify output matches expected
  - Test complete game walkthrough
  - _Requirements: 94_

- [ ] 68.1 Extract test data from MIT transcript
  - Parse transcript for commands
  - Extract expected outputs
  - Create test data structure
  - _Requirements: 94_

- [ ] 68.2 Implement transcript test runner
  - Execute commands from transcript
  - Compare output to expected
  - Report differences
  - _Requirements: 94_

- [ ] 68.3 Create opening sequence test
  - Test game start
  - Test initial exploration
  - Test mailbox and leaflet
  - _Requirements: 94_

- [ ] 68.4 Create full walkthrough test
  - Test complete game solution
  - Verify all puzzles solvable
  - Verify game is winnable
  - _Requirements: 94, 96, 97_


- [ ] 69. Verify text output fidelity
  - Compare all text output to original
  - Verify punctuation and capitalization
  - Verify line breaks and spacing
  - Verify error messages
  - Verify success messages
  - _Requirements: 71, 98_

- [ ] 69.1 Audit room descriptions
  - Compare all room descriptions to original
  - Verify exact text match
  - Fix any discrepancies
  - _Requirements: 71_

- [ ] 69.2 Audit object descriptions
  - Compare all object descriptions to original
  - Verify exact text match
  - Fix any discrepancies
  - _Requirements: 71_

- [ ] 69.3 Audit verb messages
  - Compare all verb output to original
  - Verify exact text match
  - Fix any discrepancies
  - _Requirements: 71_

- [ ] 69.4 Audit error messages
  - Compare all error messages to original
  - Verify exact text match
  - Fix any discrepancies
  - _Requirements: 71, 73_

- [ ] 70. Verify puzzle solvability
  - Test each puzzle solution
  - Verify no unwinnable states
  - Verify sufficient clues
  - Verify multiple solution paths where appropriate
  - _Requirements: 97_

- [ ] 70.1 Test major puzzles
  - Troll bridge puzzle
  - Cyclops puzzle
  - Machine puzzle
  - Maze puzzle
  - All other major puzzles
  - _Requirements: 97_

- [ ] 70.2 Test treasure collection
  - Verify all treasures are accessible
  - Verify all treasures can be scored
  - Verify 350 points achievable
  - _Requirements: 97, 96_

- [ ] 70.3 Test for unwinnable states
  - Verify no actions create unwinnable state
  - Test edge cases
  - Test unusual action sequences
  - _Requirements: 97_

- [ ] 71. Performance optimization
  - Profile command response time
  - Optimize hot paths
  - Optimize object lookup
  - Optimize parser
  - Verify <10ms response time
  - _Requirements: 86_

- [ ] 71.1 Profile performance
  - Measure command response times
  - Identify bottlenecks
  - _Requirements: 86_

- [ ] 71.2 Optimize object lookup
  - Use efficient data structures
  - Cache frequently accessed objects
  - _Requirements: 86_

- [ ] 71.3 Optimize parser
  - Minimize string allocations
  - Use string_view where possible
  - Optimize tokenization
  - _Requirements: 86_

- [ ] 71.4 Verify performance targets
  - Test response time for various commands
  - Ensure all commands respond in <10ms
  - _Requirements: 86_

- [ ] 72. Memory leak detection
  - Run valgrind or similar tool
  - Fix any memory leaks
  - Verify proper cleanup
  - Test save/restore for leaks
  - _Requirements: 87_

- [ ] 72.1 Test for memory leaks
  - Run memory leak detection tools
  - Test normal gameplay
  - Test save/restore
  - Test restart
  - _Requirements: 87_

- [ ] 72.2 Fix identified leaks
  - Fix any memory leaks found
  - Verify smart pointer usage
  - Verify RAII compliance
  - _Requirements: 87_

- [ ] 73. Cross-platform testing
  - Test on Linux
  - Test on macOS
  - Test on Windows
  - Fix platform-specific issues
  - _Requirements: 88_

- [ ] 73.1 Test Linux build
  - Build on Linux
  - Run all tests
  - Fix any issues
  - _Requirements: 88_

- [ ] 73.2 Test macOS build
  - Build on macOS
  - Run all tests
  - Fix any issues
  - _Requirements: 88_
N
- [ ] 73.3 Test Windows build
  - Build on Windows
  - Run all tests
  - Fix any issues
  - _Requirements: 88_

- [ ] 74. Code quality review
  - Review naming conventions
  - Review const correctness
  - Review code duplication
  - Review code organizati- Apply modern C++ best practices
  - _Requirements: 90_

- [ ] 74.1 Review naming conventions
  - Verify consistent naming
  - Fix any inconsistencies
  - _Requirements: 90_

- [ ] 74.2 Review const correctness
  - Add const where appropriate
  - Verify const methods
  - _Requirements: 90_

- [ ] 74.3 Review code duplication
  - Identify duplicated code
  - Refactor to eliminate duplication
  - _Requirements: 90_

- [ ] 74.4 Apply modern C++ features
  - Use std::optional where appropriate
  - Use structured bindings where appropriate
  - Use auto where appropriate
  - Follow steering guidelines
  - _Requirements: 90_

- [ ] 75. Documentation review
  - Review README
  - Review developer guide
  - Review inline comments
  - Review architecture documentation
  - Ensure completeness
  - _Requirements: 95_

- [ ] 75.1 Update README
  - Verify build instructions
  - Add usage examples
  - Add feature list
  - _Requirements: 95_

- [ ] 75.2 Update developer guide
  - Verify ZIL to C++ mappings
  - Add examples
  - Document patterns
  - _Requirements: 95_

- [ ] 75.3 Review inline comments
  - Add comments for complex logic
  - Remove obsolete comments
  - Ensure clarity
  - _Requirements: 95_

- [ ] 75.4 Update architecture documentation
  - Document final architecture
  - Update diagrams if needed
  - Document design decisions
  - _Requirements: 95_

- [ ] 76. Final integration testing
  - Run complete test suite
  - Fix any failing tests
  - Verify all requirements met
  - Test complete game playthrough
  - _Requirements: 93, 94, 96_

- [ ] 76.1 Run all unit tests
  - Execute complete unit test suite
  - Fix any failures
  - Verify coverage goals
  - _Requirements: 92_

- [ ] 76.2 Run all integration tests
  - Execute complete integration test suite
  - Fix any failures
  - _Requirements: 93_

- [ ] 76.3 Run all transcript tests
  - Execute complete transcript test suite
  - Fix any failures
  - Verify output matches original
  - _Requirements: 94_

- [ ] 76.4 Manual playthrough
  - Play through complete game
  - Verify all features work
  - Verify game is winnable
  - _Requirements: 96_

- [ ] 77. Bug fixing and polish
  - Fix any remaining bugs
  - Polish user experience
  - Improve error messages
  - Add quality-of-life improvements
  - _Requirements: 99_

- [ ] 77.1 Fix identified bugs
  - Address all known bugs
  - Test fixes thoroughly
  - _Requirements: 99_

- [ ] 77.2 Polish error messages
  - Ensure all error messages are helpful
  - Ensure consistent tone
  - _Requirements: 73, 99_

- [ ] 77.3 Improve user experience
  - Add helpful hints where appropriate
  - Improve command feedback
  - _Requirements: 99_

- [ ] 78. Final verification against original
  - Play original Zork I
  - Play C++ port side-by-side
  - Verify identical behavior
  - Verify identical text output
  - Document any intentional differences
  - _Requirements: 100_

- [ ] 78.1 Side-by-side comparison
  - Test same commands in both versions
  - Compare outputs
  - Verify behavior matches
  - _Requirements: 100_

- [ ] 78.2 Verify puzzle solutions
  - Test all puzzle solutions in both versions
  - Verify same solutions work
  - _Requirements: 100_

- [ ] 78.3 Verify scoring
  - Compare scoring in both versions
  - Verify same point values
  - Verify same maximum score
  - _Requirements: 100_

- [ ] 78.4 Document differences
  - Document any intentional differences
  - Justify any deviations from original
  - _Requirements: 100_

- [ ] 79. Prepare release build
  - Configure release build settings
  - Optimize for release
  - Create release packages
  - Test release builds
  - _Requirements: 89_

- [ ] 79.1 Configure release build
  - Set optimization flags
  - Disable debug code
  - Configure for release
  - _Requirements: 89_

- [ ] 79.2 Build release packages
  - Build for Linux
  - Build for macOS
  - Build for Windows
  - _Requirements: 88, 89_

- [ ] 79.3 Test release builds
  - Test each platform's release build
  - Verify performance
  - Verify functionality
  - _Requirements: 89_

- [ ] 80. Final documentation and release
  - Finalize all documentation
  - Create release notes
  - Tag release version
  - Prepare for distribution
  - _Requirements: 95_

- [ ] 80.1 Finalize documentation
  - Complete README
  - Complete developer guide
  - Complete architecture docs
  - _Requirements: 95_

- [ ] 80.2 Create release notes
  - Document features
  - Document known issues
  - Document system requirements
  - _Requirements: 95_

- [ ] 80.3 Tag release
  - Create git tag for release
  - Push to repository
  - _Requirements: 95_

---

## Summary

This implementation plan contains 80 top-level tasks with 150+ sub-tasks, organized into 7 phases:

1. **Parser Enhancement (Tasks 1-7)**: Build robust command parsing with full syntax support
2. **World Building - Rooms (Tasks 8-15)**: Implement all ~100 rooms with exits and descriptions
3. **World Building - Objects (Tasks 16-24)**: Implement all ~150 objects with properties and behaviors
4. **Core Verbs (Tasks 25-38)**: Implement all verb handlers for player commands
5. **Object and Room Actions (Tasks 39-46)**: Implement object-specific and room-specific behaviors
6. **Game Systems (Tasks 47-65)**: Implement timers, scoring, combat, light/darkness, save/restore
7. **Testing and Polish (Tasks 66-80)**: Comprehensive testing, optimization, and final polish

Each task builds on previous tasks, ensuring no orphaned code. The order is designed for incremental development with a working game at each stage. Optional tasks (marked with `*`) can be skipped to focus on core functionality.

**Total Requirements Covered**: All 100 requirements
**Estimated Total Sub-tasks**: 150+
**Execution Order**: Sequential (1 → 80)

