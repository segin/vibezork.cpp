# Requirements Document

## Introduction

This document specifies the complete requirements for porting Zork I from ZIL (Zork Implementation Language) to modern C++17. The port must maintain exact fidelity to the original game's behavior, text output, and gameplay while creating a clean, maintainable C++ codebase. The original Zork I consists of approximately 12,000 lines of ZIL code across 9 files, containing ~100 rooms, ~150 objects, ~80 verbs, and complex game systems including NPCs, puzzles, combat, and scoring.

## Glossary

- **ZIL**: Zork Implementation Language, the original language Zork was written in
- **Parser System**: The command parsing and interpretation system that converts player text input into game actions
- **Object System**: The core data structure representing all game entities (rooms, items, NPCs, etc.)
- **Verb Handler**: A function that implements the behavior of a specific player command (TAKE, DROP, etc.)
- **Action Handler**: Object-specific or room-specific behavior triggered by verbs or events
- **Game World**: The collection of all rooms, objects, and their interconnections
- **Syntax Pattern**: A template defining valid command structures for verbs (e.g., "TAKE OBJECT" or "PUT OBJECT IN CONTAINER")
- **PRSO**: Parser Result Subject Object - the direct object of a command
- **PRSI**: Parser Result Subject Indirect - the indirect object of a command
- **Flag**: A boolean property of an object (TAKEBIT, OPENBIT, etc.)
- **Property**: A key-value attribute of an object (SIZE, CAPACITY, etc.)
- **Timer/Interrupt**: A scheduled event that fires periodically (thief movement, lamp battery, etc.)
- **Transcript**: A recorded playthrough used for automated testing
- **Disambiguation**: The process of asking the player which object they mean when multiple match

## Requirements

### Requirement 1: Parser Foundation

**User Story:** As a player, I want to type natural language commands so that I can interact with the game world intuitively.

#### Acceptance Criteria

1. WHEN the player enters a command, THE Parser System SHALL tokenize the input into individual words
2. WHEN the player enters a command with leading or trailing whitespace, THE Parser System SHALL normalize the input by trimming whitespace
3. WHEN the player enters a command with multiple consecutive spaces, THE Parser System SHALL treat them as single spaces
4. WHEN the player enters an empty command, THE Parser System SHALL prompt for input again without error
5. THE Parser System SHALL convert all input to lowercase for case-insensitive matching

### Requirement 2: Verb Recognition

**User Story:** As a player, I want the game to understand verb synonyms so that I can use natural variations of commands.

#### Acceptance Criteria

1. WHEN the player enters a verb synonym, THE Parser System SHALL map it to the canonical verb identifier
2. THE Parser System SHALL support all verb synonyms defined in gsyntax.zil (TAKE/GET/GRAB, EXAMINE/X/LOOK AT, etc.)
3. WHEN the player enters an unrecognized verb, THE Parser System SHALL display "I don't know the word [word]"
4. THE Parser System SHALL recognize verbs in the first position of the command
5. THE Parser System SHALL support multi-word verbs (LOOK AT, PICK UP, etc.)


### Requirement 3: Direction Parsing

**User Story:** As a player, I want to move through the game world using directional commands so that I can explore.

#### Acceptance Criteria

1. WHEN the player enters a direction command, THE Parser System SHALL recognize it as a movement verb
2. THE Parser System SHALL support all compass directions (NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST)
3. THE Parser System SHALL support abbreviated directions (N, S, E, W, NE, NW, SE, SW)
4. THE Parser System SHALL support vertical directions (UP, DOWN, U, D)
5. THE Parser System SHALL support special movement commands (ENTER, EXIT, CLIMB)

### Requirement 4: Object Recognition by Synonym

**User Story:** As a player, I want to refer to objects by any of their names so that I can use natural language.

#### Acceptance Criteria

1. WHEN the player refers to an object by any synonym, THE Parser System SHALL identify the correct object
2. THE Parser System SHALL match object synonyms defined in 1dungeon.zil
3. WHEN multiple objects share a synonym, THE Parser System SHALL use context to disambiguate
4. THE Parser System SHALL prioritize objects in the current room over objects in inventory
5. THE Parser System SHALL prioritize objects in inventory over objects in other rooms

### Requirement 5: Object Recognition by Adjective

**User Story:** As a player, I want to use adjectives to specify objects so that I can distinguish between similar items.

#### Acceptance Criteria

1. WHEN the player uses an adjective with a noun, THE Parser System SHALL match objects with that adjective
2. THE Parser System SHALL support multi-word object names (SMALL MAILBOX, RUSTY KNIFE, etc.)
3. WHEN an adjective narrows matches to one object, THE Parser System SHALL select that object
4. WHEN an adjective is used without a matching object, THE Parser System SHALL report "You can't see any such thing"
5. THE Parser System SHALL support multiple adjectives (SMALL BRASS LANTERN)

### Requirement 6: Syntax Pattern Matching

**User Story:** As a player, I want the game to understand complex command structures so that I can perform sophisticated actions.

#### Acceptance Criteria

1. THE Parser System SHALL support all syntax patterns defined in gsyntax.zil
2. THE Parser System SHALL validate commands against verb-specific syntax patterns
3. WHEN a command matches a pattern, THE Parser System SHALL extract PRSO and PRSI objects
4. WHEN a command doesn't match any pattern, THE Parser System SHALL display an appropriate error message
5. THE Parser System SHALL support patterns with prepositions (PUT OBJECT IN CONTAINER, ATTACK TROLL WITH SWORD)

### Requirement 7: Preposition Handling

**User Story:** As a player, I want to use prepositions in commands so that I can specify relationships between objects.

#### Acceptance Criteria

1. THE Parser System SHALL recognize all prepositions defined in gsyntax.zil (WITH, IN, ON, TO, FROM, etc.)
2. WHEN a preposition is present, THE Parser System SHALL identify the indirect object (PRSI)
3. THE Parser System SHALL validate that the verb accepts the given preposition
4. WHEN an invalid preposition is used, THE Parser System SHALL display "I don't understand that"
5. THE Parser System SHALL support commands with optional prepositions

### Requirement 8: Disambiguation

**User Story:** As a player, I want the game to ask for clarification when my command is ambiguous so that I can specify my intent.

#### Acceptance Criteria

1. WHEN multiple objects match the player's input, THE Parser System SHALL ask "Which [noun] do you mean?"
2. THE Parser System SHALL list the matching objects for the player to choose from
3. WHEN the player responds with a clarification, THE Parser System SHALL retry the original command
4. THE Parser System SHALL remember the disambiguated object for pronoun reference
5. THE Parser System SHALL limit disambiguation to visible and accessible objects


### Requirement 9: Special Parser Features

**User Story:** As a player, I want to use special commands like "all" and "again" so that I can play more efficiently.

#### Acceptance Criteria

1. WHEN the player types "all", THE Parser System SHALL apply the verb to all applicable objects
2. WHEN the player types "all except [object]", THE Parser System SHALL apply the verb to all objects except the specified one
3. WHEN the player types "again" or "g", THE Parser System SHALL repeat the last command
4. WHEN the player types "oops [word]", THE Parser System SHALL correct the last typo and retry
5. THE Parser System SHALL track IT and THEM pronouns to refer to previously mentioned objects

### Requirement 10: Object System Core

**User Story:** As a developer, I need a robust object system so that all game entities can be represented consistently.

#### Acceptance Criteria

1. THE Object System SHALL represent all game entities (rooms, items, NPCs) as objects
2. THE Object System SHALL support object flags (TAKEBIT, OPENBIT, CONTBIT, etc.)
3. THE Object System SHALL support object properties (SIZE, CAPACITY, VALUE, etc.)
4. THE Object System SHALL support object containment (objects can contain other objects)
5. THE Object System SHALL support object location tracking

### Requirement 11: Object Flags

**User Story:** As a developer, I need to set and check object flags so that I can implement game logic.

#### Acceptance Criteria

1. THE Object System SHALL provide methods to set, clear, and check flags
2. THE Object System SHALL support all flags defined in the original ZIL code
3. WHEN a flag is set, THE Object System SHALL persist it until explicitly cleared
4. THE Object System SHALL support checking multiple flags simultaneously
5. THE Object System SHALL use type-safe flag enumerations

### Requirement 12: Object Properties

**User Story:** As a developer, I need to store and retrieve object properties so that objects can have attributes.

#### Acceptance Criteria

1. THE Object System SHALL provide methods to get and set properties
2. THE Object System SHALL support integer property values
3. THE Object System SHALL support object reference property values
4. WHEN a property is not set, THE Object System SHALL return a default value or indicate absence
5. THE Object System SHALL support all properties defined in the original ZIL code

### Requirement 13: Object Containment

**User Story:** As a player, I want objects to contain other objects so that I can use containers and inventory.

#### Acceptance Criteria

1. THE Object System SHALL track which objects are contained in other objects
2. THE Object System SHALL support moving objects between containers
3. THE Object System SHALL enforce container capacity limits
4. THE Object System SHALL provide methods to iterate over contained objects
5. THE Object System SHALL support nested containment (containers in containers)

### Requirement 14: Room System

**User Story:** As a player, I want to move between rooms so that I can explore the game world.

#### Acceptance Criteria

1. THE Game World SHALL represent rooms as specialized objects
2. THE Game World SHALL support room exits in all compass directions
3. THE Game World SHALL support conditional exits (locked doors, etc.)
4. THE Game World SHALL support exit messages for blocked directions
5. THE Game World SHALL track the player's current room

### Requirement 15: Room Descriptions

**User Story:** As a player, I want to see room descriptions so that I can understand my surroundings.

#### Acceptance Criteria

1. WHEN the player enters a room, THE Game World SHALL display the room's description
2. WHEN the player types "look", THE Game World SHALL display the full room description
3. THE Game World SHALL list visible objects in the room
4. THE Game World SHALL list visible exits from the room
5. THE Game World SHALL support brief mode (short descriptions for visited rooms)


### Requirement 16: Room Actions

**User Story:** As a developer, I need rooms to respond to events so that I can implement room-specific behavior.

#### Acceptance Criteria

1. THE Game World SHALL support room action handlers
2. WHEN a room action is triggered, THE Game World SHALL invoke the room's action handler
3. THE Game World SHALL pass the action type to the handler (M-LOOK, M-ENTER, etc.)
4. THE Game World SHALL allow action handlers to override default behavior
5. THE Game World SHALL support all room actions defined in 1actions.zil

### Requirement 17: Complete Room Set

**User Story:** As a player, I want to explore all areas of Zork so that I can experience the complete game.

#### Acceptance Criteria

1. THE Game World SHALL implement all ~100 rooms from 1dungeon.zil
2. THE Game World SHALL implement all forest areas (FOREST-1 through FOREST-4, CLEARING, PATH)
3. THE Game World SHALL implement the complete house exterior and interior
4. THE Game World SHALL implement all underground areas
5. THE Game World SHALL implement all maze sections and special areas

### Requirement 18: Complete Object Set

**User Story:** As a player, I want all game objects to be present so that I can interact with the complete game world.

#### Acceptance Criteria

1. THE Game World SHALL implement all ~150 objects from 1dungeon.zil
2. THE Game World SHALL implement all treasure objects
3. THE Game World SHALL implement all tool objects
4. THE Game World SHALL implement all container objects
5. THE Game World SHALL implement all NPC objects and scenery objects

### Requirement 19: Object Initial State

**User Story:** As a player, I want objects to start in their correct locations so that the game begins properly.

#### Acceptance Criteria

1. THE Game World SHALL place all objects in their initial locations as defined in 1dungeon.zil
2. THE Game World SHALL set all initial object flags
3. THE Game World SHALL set all initial object properties
4. THE Game World SHALL establish all initial containment relationships
5. THE Game World SHALL initialize the player's starting location (WEST-OF-HOUSE)

### Requirement 20: Movement Verbs

**User Story:** As a player, I want to use movement commands so that I can navigate the game world.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement directional movement (NORTH, SOUTH, EAST, WEST, etc.)
2. THE Verb Handler SHALL implement ENTER to enter objects or locations
3. THE Verb Handler SHALL implement EXIT to leave containers or locations
4. THE Verb Handler SHALL implement CLIMB for vertical movement
5. THE Verb Handler SHALL display appropriate messages for blocked or invalid movement

### Requirement 21: Inventory Management Verbs

**User Story:** As a player, I want to pick up and drop objects so that I can manage my inventory.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement TAKE to pick up objects
2. THE Verb Handler SHALL implement DROP to put down objects
3. THE Verb Handler SHALL implement INVENTORY to list carried objects
4. THE Verb Handler SHALL enforce object TAKEBIT flag (only takeable objects can be taken)
5. THE Verb Handler SHALL enforce inventory weight and capacity limits

### Requirement 22: Container Manipulation Verbs

**User Story:** As a player, I want to put objects in containers so that I can organize items.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement PUT to place objects in containers
2. THE Verb Handler SHALL implement PUT-ON to place objects on surfaces
3. THE Verb Handler SHALL enforce container CONTBIT flag
4. THE Verb Handler SHALL enforce container capacity limits
5. THE Verb Handler SHALL provide appropriate feedback for successful and failed actions


### Requirement 23: Container State Verbs

**User Story:** As a player, I want to open and close containers so that I can access their contents.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement OPEN to open containers
2. THE Verb Handler SHALL implement CLOSE to close containers
3. THE Verb Handler SHALL enforce CONTBIT flag (only containers can be opened)
4. THE Verb Handler SHALL set and clear OPENBIT flag appropriately
5. THE Verb Handler SHALL reveal container contents when opened

### Requirement 24: Locking Verbs

**User Story:** As a player, I want to lock and unlock objects so that I can solve puzzles.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement LOCK to lock objects
2. THE Verb Handler SHALL implement UNLOCK to unlock objects
3. THE Verb Handler SHALL require appropriate keys for locking/unlocking
4. THE Verb Handler SHALL check for LOCKEDBIT flag
5. THE Verb Handler SHALL prevent opening locked containers

### Requirement 25: Examination Verbs

**User Story:** As a player, I want to examine objects closely so that I can discover details and clues.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement EXAMINE to show detailed object descriptions
2. THE Verb Handler SHALL implement LOOK-INSIDE to view container contents
3. THE Verb Handler SHALL implement SEARCH to find hidden objects
4. THE Verb Handler SHALL implement READ to read text on objects
5. THE Verb Handler SHALL display object-specific descriptions when available

### Requirement 26: Manipulation Verbs

**User Story:** As a player, I want to manipulate objects so that I can solve puzzles.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement TURN to rotate objects
2. THE Verb Handler SHALL implement PUSH to push objects
3. THE Verb Handler SHALL implement PULL to pull objects
4. THE Verb Handler SHALL implement MOVE to move objects
5. THE Verb Handler SHALL trigger object-specific actions when appropriate

### Requirement 27: Interaction Verbs

**User Story:** As a player, I want to interact with objects in various ways so that I can fully explore the game.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement TIE to tie objects together
2. THE Verb Handler SHALL implement UNTIE to untie objects
3. THE Verb Handler SHALL implement LISTEN to hear sounds
4. THE Verb Handler SHALL implement SMELL to smell objects
5. THE Verb Handler SHALL implement TOUCH to touch objects

### Requirement 28: Consumption Verbs

**User Story:** As a player, I want to eat and drink so that I can interact with consumable items.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement EAT to consume food items
2. THE Verb Handler SHALL implement DRINK to consume liquids
3. THE Verb Handler SHALL remove consumed items from the game
4. THE Verb Handler SHALL enforce FOODBIT and DRINKBIT flags
5. THE Verb Handler SHALL provide appropriate feedback for edible and inedible items

### Requirement 29: Combat Verbs

**User Story:** As a player, I want to fight enemies so that I can overcome obstacles.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement ATTACK to fight NPCs
2. THE Verb Handler SHALL implement KILL as a synonym for ATTACK
3. THE Verb Handler SHALL implement SWING to use weapons
4. THE Verb Handler SHALL implement THROW to throw objects
5. THE Verb Handler SHALL integrate with the combat system


### Requirement 30: Light Source Verbs

**User Story:** As a player, I want to control light sources so that I can see in dark areas.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement LAMP-ON to turn on the lamp
2. THE Verb Handler SHALL implement LAMP-OFF to turn off the lamp
3. THE Verb Handler SHALL implement LIGHT as a synonym for LAMP-ON
4. THE Verb Handler SHALL set ONBIT flag when light sources are activated
5. THE Verb Handler SHALL integrate with the light/darkness system

### Requirement 31: Special Action Verbs

**User Story:** As a player, I want to perform special actions so that I can solve unique puzzles.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement INFLATE to inflate objects
2. THE Verb Handler SHALL implement DEFLATE to deflate objects
3. THE Verb Handler SHALL implement PRAY for religious actions
4. THE Verb Handler SHALL implement EXORCISE for spiritual actions
5. THE Verb Handler SHALL implement WAVE, RUB, and RING for object-specific actions

### Requirement 32: Meta Game Verbs

**User Story:** As a player, I want to check my status and manage the game so that I can track progress.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement SCORE to display current score and rank
2. THE Verb Handler SHALL implement DIAGNOSE to show health status
3. THE Verb Handler SHALL implement VERBOSE to enable full descriptions
4. THE Verb Handler SHALL implement BRIEF to enable short descriptions
5. THE Verb Handler SHALL implement SUPERBRIEF to enable minimal descriptions

### Requirement 33: Game Control Verbs

**User Story:** As a player, I want to save, load, and restart the game so that I can manage my playthrough.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement SAVE to save game state
2. THE Verb Handler SHALL implement RESTORE to load game state
3. THE Verb Handler SHALL implement RESTART to begin a new game
4. THE Verb Handler SHALL implement QUIT to exit the game
5. THE Verb Handler SHALL implement VERSION to display game information

### Requirement 34: Pre-Verb Checks

**User Story:** As a developer, I need pre-verb validation so that invalid actions are caught early.

#### Acceptance Criteria

1. THE Verb Handler SHALL implement PRE-TAKE to validate taking objects
2. THE Verb Handler SHALL implement PRE-DROP to validate dropping objects
3. THE Verb Handler SHALL implement PRE-OPEN to validate opening containers
4. THE Verb Handler SHALL implement PRE-CLOSE to validate closing containers
5. THE Verb Handler SHALL implement pre-checks for all verbs that require validation

### Requirement 35: Object Action Handlers

**User Story:** As a developer, I need object-specific actions so that objects can have unique behaviors.

#### Acceptance Criteria

1. THE Action Handler SHALL support object-specific action functions
2. THE Action Handler SHALL invoke object actions when verbs are applied to objects
3. THE Action Handler SHALL allow objects to override default verb behavior
4. THE Action Handler SHALL pass verb and object context to action handlers
5. THE Action Handler SHALL support all object actions defined in 1actions.zil

### Requirement 36: Container Actions

**User Story:** As a player, I want containers to behave correctly so that I can use them to store items.

#### Acceptance Criteria

1. THE Action Handler SHALL implement MAILBOX action (anchored, cannot be taken)
2. THE Action Handler SHALL implement TROPHY-CASE action (treasure storage)
3. THE Action Handler SHALL implement COFFIN action (special container)
4. THE Action Handler SHALL implement BASKET action (balloon basket)
5. THE Action Handler SHALL implement SACK action (portable container)


### Requirement 37: Tool Actions

**User Story:** As a player, I want tools to work correctly so that I can solve puzzles.

#### Acceptance Criteria

1. THE Action Handler SHALL implement SWORD action (weapon, glows near enemies)
2. THE Action Handler SHALL implement LAMP action (light source with battery)
3. THE Action Handler SHALL implement ROPE action (climbing, tying)
4. THE Action Handler SHALL implement WRENCH action (tool for bolts)
5. THE Action Handler SHALL implement SCREWDRIVER action (tool for screws)

### Requirement 38: Treasure Actions

**User Story:** As a player, I want treasures to be valuable so that I can score points.

#### Acceptance Criteria

1. THE Action Handler SHALL mark all treasures with TRYTAKEBIT flag
2. THE Action Handler SHALL assign point values to treasures
3. THE Action Handler SHALL award points when treasures are placed in the trophy case
4. THE Action Handler SHALL implement special treasure behaviors (EGG opens, PAINTING has back, etc.)
5. THE Action Handler SHALL track which treasures have been scored

### Requirement 39: NPC Actions - Thief

**User Story:** As a player, I want the thief to behave as a challenging adversary so that the game is engaging.

#### Acceptance Criteria

1. THE Action Handler SHALL implement thief wandering behavior
2. THE Action Handler SHALL implement thief stealing behavior
3. THE Action Handler SHALL implement thief combat behavior
4. THE Action Handler SHALL implement thief treasure room behavior
5. THE Action Handler SHALL make the thief appear and disappear randomly

### Requirement 40: NPC Actions - Troll

**User Story:** As a player, I want the troll to guard the bridge so that I must defeat it to pass.

#### Acceptance Criteria

1. THE Action Handler SHALL implement troll blocking behavior at the bridge
2. THE Action Handler SHALL implement troll combat behavior
3. THE Action Handler SHALL implement troll death behavior (bridge becomes passable)
4. THE Action Handler SHALL implement troll vanishing behavior when killed
5. THE Action Handler SHALL prevent passage until troll is defeated

### Requirement 41: NPC Actions - Cyclops

**User Story:** As a player, I want the cyclops to be a unique puzzle so that I can solve it creatively.

#### Acceptance Criteria

1. THE Action Handler SHALL implement cyclops blocking behavior
2. THE Action Handler SHALL implement cyclops hunger behavior
3. THE Action Handler SHALL implement cyclops eating behavior (accepts food)
4. THE Action Handler SHALL implement cyclops leaving behavior when fed
5. THE Action Handler SHALL make the cyclops non-hostile if fed

### Requirement 42: Puzzle Object Actions

**User Story:** As a player, I want puzzle objects to work correctly so that I can solve the game's puzzles.

#### Acceptance Criteria

1. THE Action Handler SHALL implement MIRROR action (shows different reflections)
2. THE Action Handler SHALL implement PAINTING action (can be taken from wall, has back side)
3. THE Action Handler SHALL implement MACHINE action (complex button puzzle)
4. THE Action Handler SHALL implement BOAT action (water navigation)
5. THE Action Handler SHALL implement PUMP action (inflate/deflate boat)

### Requirement 43: Special Room Actions

**User Story:** As a player, I want special rooms to have unique behaviors so that the game world is interesting.

#### Acceptance Criteria

1. THE Action Handler SHALL implement LOUD-ROOM action (echoes sounds)
2. THE Action Handler SHALL implement ROUND-ROOM action (special acoustics)
3. THE Action Handler SHALL implement ENTRANCE-TO-HADES action (death and resurrection)
4. THE Action Handler SHALL implement TREASURE-ROOM action (thief's lair)
5. THE Action Handler SHALL implement MAZE actions (confusing navigation)


### Requirement 44: Timer System Foundation

**User Story:** As a developer, I need a timer system so that events can occur periodically.

#### Acceptance Criteria

1. THE Timer System SHALL support scheduling periodic events
2. THE Timer System SHALL invoke timer callbacks at specified intervals
3. THE Timer System SHALL support enabling and disabling timers
4. THE Timer System SHALL support one-shot and repeating timers
5. THE Timer System SHALL integrate with the main game loop

### Requirement 45: Thief Timer

**User Story:** As a player, I want the thief to move around so that encounters are unpredictable.

#### Acceptance Criteria

1. THE Timer System SHALL implement I-THIEF timer for thief movement
2. WHEN the thief timer fires, THE Timer System SHALL move the thief to a random accessible room
3. THE Timer System SHALL make the thief steal objects when in the same room as the player
4. THE Timer System SHALL make the thief attack the player occasionally
5. THE Timer System SHALL stop the thief timer when the thief is dead

### Requirement 46: Combat Timer

**User Story:** As a player, I want combat to progress in rounds so that fights are structured.

#### Acceptance Criteria

1. THE Timer System SHALL implement I-FIGHT timer for combat rounds
2. WHEN the combat timer fires, THE Timer System SHALL process one round of combat
3. THE Timer System SHALL calculate attack success based on weapon and enemy
4. THE Timer System SHALL apply damage to combatants
5. THE Timer System SHALL end combat when one combatant dies or flees

### Requirement 47: Lamp Timer

**User Story:** As a player, I want the lamp battery to drain so that light is a limited resource.

#### Acceptance Criteria

1. THE Timer System SHALL implement I-LANTERN timer for lamp battery
2. WHEN the lamp is on, THE Timer System SHALL decrement the battery counter
3. WHEN the battery reaches zero, THE Timer System SHALL turn off the lamp
4. THE Timer System SHALL warn the player when the battery is low
5. THE Timer System SHALL make the lamp unusable when the battery is depleted

### Requirement 48: Candle Timer

**User Story:** As a player, I want candles to burn down so that they are temporary light sources.

#### Acceptance Criteria

1. THE Timer System SHALL implement I-CANDLES timer for candle burning
2. WHEN candles are lit, THE Timer System SHALL decrement their duration
3. WHEN candles burn out, THE Timer System SHALL remove them from the game
4. THE Timer System SHALL warn the player when candles are nearly gone
5. THE Timer System SHALL make candles provide light while burning

### Requirement 49: Sword Glow Timer

**User Story:** As a player, I want the sword to glow near enemies so that I have warning of danger.

#### Acceptance Criteria

1. THE Timer System SHALL implement I-SWORD timer for sword glow checking
2. WHEN the sword timer fires, THE Timer System SHALL check for nearby enemies
3. WHEN enemies are near, THE Timer System SHALL make the sword glow
4. WHEN enemies leave, THE Timer System SHALL stop the sword from glowing
5. THE Timer System SHALL display messages when the sword starts or stops glowing

### Requirement 50: Light and Darkness System

**User Story:** As a player, I want dark rooms to require light sources so that exploration is challenging.

#### Acceptance Criteria

1. THE Light System SHALL track which rooms are naturally lit
2. THE Light System SHALL track which objects provide light
3. WHEN the player is in a dark room without light, THE Light System SHALL limit visibility
4. WHEN the player is in a dark room without light, THE Light System SHALL risk grue attacks
5. THE Light System SHALL update room lighting when light sources are turned on or off


### Requirement 51: Grue Attacks

**User Story:** As a player, I want to be eaten by a grue in the dark so that light sources are essential.

#### Acceptance Criteria

1. WHEN the player is in darkness, THE Light System SHALL warn about the grue
2. WHEN the player remains in darkness, THE Light System SHALL increase grue danger
3. WHEN the grue attacks, THE Light System SHALL kill the player
4. THE Light System SHALL display appropriate grue messages
5. THE Light System SHALL prevent grue attacks when any light source is present

### Requirement 52: Score Tracking

**User Story:** As a player, I want my score to increase as I collect treasures so that I can track progress.

#### Acceptance Criteria

1. THE Score System SHALL track the player's current score
2. THE Score System SHALL award points when treasures are placed in the trophy case
3. THE Score System SHALL track the maximum possible score (350 points)
4. THE Score System SHALL calculate the player's rank based on score
5. THE Score System SHALL display score and rank when requested

### Requirement 53: Move Counting

**User Story:** As a player, I want the game to count my moves so that I can track efficiency.

#### Acceptance Criteria

1. THE Score System SHALL increment the move counter for each command
2. THE Score System SHALL exclude certain commands from the move count (LOOK, INVENTORY, etc.)
3. THE Score System SHALL display the move count with the score
4. THE Score System SHALL track moves for the entire game session
5. THE Score System SHALL reset the move count when the game restarts

### Requirement 54: Rank Calculation

**User Story:** As a player, I want to earn ranks based on my score so that I can see my achievement level.

#### Acceptance Criteria

1. THE Score System SHALL assign ranks based on score thresholds
2. THE Score System SHALL support all ranks from the original game (Beginner, Amateur, etc.)
3. THE Score System SHALL display the rank with the score
4. THE Score System SHALL update the rank as the score increases
5. THE Score System SHALL award the "Master Adventurer" rank for maximum score

### Requirement 55: Combat System Foundation

**User Story:** As a player, I want to fight enemies so that I can overcome obstacles.

#### Acceptance Criteria

1. THE Combat System SHALL support player attacks on NPCs
2. THE Combat System SHALL support NPC attacks on the player
3. THE Combat System SHALL track health for combatants
4. THE Combat System SHALL calculate attack success based on weapons and armor
5. THE Combat System SHALL end combat when one combatant dies or flees

### Requirement 56: Weapon Effectiveness

**User Story:** As a player, I want different weapons to have different effectiveness so that weapon choice matters.

#### Acceptance Criteria

1. THE Combat System SHALL assign effectiveness values to weapons
2. THE Combat System SHALL make the sword the most effective weapon
3. THE Combat System SHALL make the knife less effective than the sword
4. THE Combat System SHALL allow bare-handed combat with low effectiveness
5. THE Combat System SHALL use weapon effectiveness in damage calculation

### Requirement 57: Enemy Behavior

**User Story:** As a player, I want enemies to fight back so that combat is challenging.

#### Acceptance Criteria

1. THE Combat System SHALL make enemies attack the player during combat
2. THE Combat System SHALL vary enemy strength (troll is stronger than thief)
3. THE Combat System SHALL make enemies flee when badly wounded
4. THE Combat System SHALL make enemies drop items when killed
5. THE Combat System SHALL remove dead enemies from the game


### Requirement 58: Death and Game Over

**User Story:** As a player, I want to die and restart so that failure has consequences.

#### Acceptance Criteria

1. THE Game System SHALL implement the JIGS-UP function for player death
2. WHEN the player dies, THE Game System SHALL display an appropriate death message
3. WHEN the player dies, THE Game System SHALL offer resurrection or restart options
4. THE Game System SHALL support multiple death scenarios (combat, grue, falling, etc.)
5. THE Game System SHALL track the number of deaths

### Requirement 59: Resurrection Mechanic

**User Story:** As a player, I want a chance to resurrect after death so that I can continue playing.

#### Acceptance Criteria

1. WHEN the player dies, THE Game System SHALL offer resurrection at the entrance to Hades
2. THE Game System SHALL limit the number of resurrections
3. WHEN resurrected, THE Game System SHALL move the player to the entrance to Hades
4. WHEN resurrected, THE Game System SHALL scatter the player's inventory
5. THE Game System SHALL make resurrection unavailable after multiple deaths

### Requirement 60: Save Game System

**User Story:** As a player, I want to save my progress so that I can continue later.

#### Acceptance Criteria

1. THE Game System SHALL serialize all game state to a file
2. THE Game System SHALL save player location, inventory, and score
3. THE Game System SHALL save all object locations and states
4. THE Game System SHALL save all flag and property values
5. THE Game System SHALL save timer states and counters

### Requirement 61: Restore Game System

**User Story:** As a player, I want to load saved games so that I can resume play.

#### Acceptance Criteria

1. THE Game System SHALL deserialize game state from a save file
2. THE Game System SHALL restore player location, inventory, and score
3. THE Game System SHALL restore all object locations and states
4. THE Game System SHALL restore all flag and property values
5. THE Game System SHALL restore timer states and counters

### Requirement 62: Restart Game

**User Story:** As a player, I want to restart the game so that I can begin fresh.

#### Acceptance Criteria

1. THE Game System SHALL reset all game state to initial values
2. THE Game System SHALL move all objects to their starting locations
3. THE Game System SHALL reset all flags and properties
4. THE Game System SHALL reset score and move count to zero
5. THE Game System SHALL confirm before restarting to prevent accidental loss

### Requirement 63: Inventory Weight Limits

**User Story:** As a player, I want inventory limits so that I must make strategic choices about what to carry.

#### Acceptance Criteria

1. THE Game System SHALL enforce a maximum inventory weight
2. THE Game System SHALL calculate total weight of carried objects
3. WHEN the player tries to take an object that would exceed the limit, THE Game System SHALL refuse
4. THE Game System SHALL display appropriate messages for weight limit violations
5. THE Game System SHALL allow dropping objects to make room for new ones

### Requirement 64: Container Capacity Limits

**User Story:** As a player, I want containers to have capacity limits so that they behave realistically.

#### Acceptance Criteria

1. THE Game System SHALL enforce capacity limits for containers
2. THE Game System SHALL calculate total size of objects in a container
3. WHEN the player tries to put an object that would exceed capacity, THE Game System SHALL refuse
4. THE Game System SHALL display appropriate messages for capacity violations
5. THE Game System SHALL support different capacity values for different containers


### Requirement 65: Verbose Mode

**User Story:** As a player, I want to see full room descriptions every time so that I don't miss details.

#### Acceptance Criteria

1. THE Game System SHALL support verbose mode
2. WHEN verbose mode is active, THE Game System SHALL display full room descriptions on every entry
3. THE Game System SHALL enable verbose mode with the VERBOSE command
4. THE Game System SHALL persist the verbose mode setting
5. THE Game System SHALL default to verbose mode for new games

### Requirement 66: Brief Mode

**User Story:** As a player, I want to see short descriptions for visited rooms so that I can move quickly.

#### Acceptance Criteria

1. THE Game System SHALL support brief mode
2. WHEN brief mode is active, THE Game System SHALL display short descriptions for previously visited rooms
3. THE Game System SHALL display full descriptions for unvisited rooms even in brief mode
4. THE Game System SHALL enable brief mode with the BRIEF command
5. THE Game System SHALL persist the brief mode setting

### Requirement 67: Superbrief Mode

**User Story:** As a player, I want minimal descriptions so that I can navigate very quickly.

#### Acceptance Criteria

1. THE Game System SHALL support superbrief mode
2. WHEN superbrief mode is active, THE Game System SHALL display only room names
3. THE Game System SHALL omit object lists in superbrief mode
4. THE Game System SHALL enable superbrief mode with the SUPERBRIEF command
5. THE Game System SHALL persist the superbrief mode setting

### Requirement 68: Diagnose Command

**User Story:** As a player, I want to check my health status so that I know how injured I am.

#### Acceptance Criteria

1. THE Game System SHALL implement the DIAGNOSE command
2. WHEN the player is healthy, THE Game System SHALL report "You are in perfect health"
3. WHEN the player is injured, THE Game System SHALL report specific injuries
4. THE Game System SHALL track injury severity
5. THE Game System SHALL update diagnosis based on combat damage

### Requirement 69: Version Command

**User Story:** As a player, I want to see version information so that I know which release I'm playing.

#### Acceptance Criteria

1. THE Game System SHALL implement the VERSION command
2. THE Game System SHALL display the game name and version number
3. THE Game System SHALL display the port information (C++ port)
4. THE Game System SHALL display copyright information
5. THE Game System SHALL match the original game's version format

### Requirement 70: Quit Confirmation

**User Story:** As a player, I want to confirm before quitting so that I don't accidentally exit.

#### Acceptance Criteria

1. WHEN the player types QUIT, THE Game System SHALL ask for confirmation
2. WHEN the player confirms, THE Game System SHALL exit the game
3. WHEN the player declines, THE Game System SHALL return to the game
4. THE Game System SHALL accept Y/N or YES/NO for confirmation
5. THE Game System SHALL remind the player to save before quitting

### Requirement 71: Text Output Fidelity

**User Story:** As a player, I want the text to match the original game so that the experience is authentic.

#### Acceptance Criteria

1. THE Game System SHALL output text that exactly matches the original Zork I
2. THE Game System SHALL use the same punctuation and capitalization
3. THE Game System SHALL use the same line breaks and spacing
4. THE Game System SHALL use the same error messages
5. THE Game System SHALL use the same success messages


### Requirement 72: Input Handling

**User Story:** As a player, I want robust input handling so that the game doesn't crash on unexpected input.

#### Acceptance Criteria

1. THE Game System SHALL handle empty input gracefully
2. THE Game System SHALL handle very long input gracefully
3. THE Game System SHALL handle special characters without crashing
4. THE Game System SHALL handle EOF (Ctrl+D) gracefully
5. THE Game System SHALL trim whitespace from input

### Requirement 73: Error Messages

**User Story:** As a player, I want helpful error messages so that I understand what went wrong.

#### Acceptance Criteria

1. WHEN the player enters an unknown word, THE Game System SHALL report "I don't know the word [word]"
2. WHEN the player enters an invalid command structure, THE Game System SHALL report "I don't understand that"
3. WHEN the player tries an impossible action, THE Game System SHALL explain why it's impossible
4. WHEN the player refers to an absent object, THE Game System SHALL report "You can't see any such thing"
5. THE Game System SHALL provide context-appropriate error messages

### Requirement 74: Pronoun References

**User Story:** As a player, I want to use pronouns so that I don't have to repeat object names.

#### Acceptance Criteria

1. THE Parser System SHALL track the last mentioned object for IT reference
2. THE Parser System SHALL track the last mentioned objects for THEM reference
3. WHEN the player uses IT, THE Parser System SHALL substitute the last object
4. WHEN the player uses THEM, THE Parser System SHALL substitute the last objects
5. THE Parser System SHALL update pronoun references after each command

### Requirement 75: Again Command

**User Story:** As a player, I want to repeat commands so that I can retry actions quickly.

#### Acceptance Criteria

1. THE Parser System SHALL track the last command entered
2. WHEN the player types AGAIN or G, THE Parser System SHALL repeat the last command
3. THE Parser System SHALL not repeat AGAIN itself
4. THE Parser System SHALL update the last command after each non-AGAIN command
5. THE Parser System SHALL handle AGAIN on the first turn gracefully

### Requirement 76: Oops Command

**User Story:** As a player, I want to correct typos so that I don't have to retype entire commands.

#### Acceptance Criteria

1. WHEN the player types OOPS followed by a word, THE Parser System SHALL replace the last unknown word
2. THE Parser System SHALL retry the corrected command automatically
3. THE Parser System SHALL only work immediately after an unknown word error
4. THE Parser System SHALL handle multiple corrections in sequence
5. THE Parser System SHALL report an error if OOPS is used inappropriately

### Requirement 77: Multi-word Object Names

**User Story:** As a player, I want to use full object names so that I can be specific.

#### Acceptance Criteria

1. THE Parser System SHALL recognize multi-word object names (SMALL MAILBOX, BRASS LANTERN, etc.)
2. THE Parser System SHALL match partial names when unambiguous
3. THE Parser System SHALL prioritize exact matches over partial matches
4. THE Parser System SHALL support adjective-noun combinations
5. THE Parser System SHALL support noun-only references when unambiguous

### Requirement 78: All and Except

**User Story:** As a player, I want to act on multiple objects at once so that I can be efficient.

#### Acceptance Criteria

1. WHEN the player types "all", THE Parser System SHALL identify all applicable objects
2. WHEN the player types "all except [object]", THE Parser System SHALL exclude the specified object
3. THE Parser System SHALL limit "all" to objects that make sense for the verb
4. THE Parser System SHALL report how many objects were affected
5. THE Parser System SHALL handle "all" with verbs like TAKE, DROP, and PUT


### Requirement 79: Object Visibility

**User Story:** As a player, I want to only interact with visible objects so that the game is realistic.

#### Acceptance Criteria

1. THE Parser System SHALL only match objects in the current room
2. THE Parser System SHALL only match objects in the player's inventory
3. THE Parser System SHALL only match objects in open containers
4. THE Parser System SHALL not match objects in closed containers
5. THE Parser System SHALL not match objects in other rooms

### Requirement 80: Object Accessibility

**User Story:** As a player, I want to only interact with accessible objects so that the game makes sense.

#### Acceptance Criteria

1. THE Game System SHALL prevent taking objects that are too heavy
2. THE Game System SHALL prevent taking objects that are anchored (TRYTAKEBIT)
3. THE Game System SHALL prevent opening objects that aren't containers
4. THE Game System SHALL prevent unlocking objects without the correct key
5. THE Game System SHALL provide appropriate feedback for inaccessible actions

### Requirement 81: Room Exit Validation

**User Story:** As a player, I want meaningful messages for blocked exits so that I understand obstacles.

#### Acceptance Criteria

1. WHEN an exit is blocked, THE Game System SHALL display a specific message explaining why
2. THE Game System SHALL support conditional exits (doors that can be opened)
3. THE Game System SHALL support one-way exits
4. THE Game System SHALL support exits that require special actions (CLIMB, ENTER)
5. THE Game System SHALL update exit availability based on game state

### Requirement 82: Special Movement

**User Story:** As a player, I want special movement commands so that I can navigate complex areas.

#### Acceptance Criteria

1. THE Game System SHALL support ENTER to enter objects (ENTER HOUSE, ENTER BOAT)
2. THE Game System SHALL support EXIT to leave the current location
3. THE Game System SHALL support CLIMB for vertical movement (CLIMB TREE, CLIMB STAIRS)
4. THE Game System SHALL support BOARD to enter vehicles (BOARD BOAT)
5. THE Game System SHALL support DISEMBARK to leave vehicles

### Requirement 83: Water Navigation

**User Story:** As a player, I want to navigate water areas with the boat so that I can explore the river.

#### Acceptance Criteria

1. THE Game System SHALL require the boat for water navigation
2. THE Game System SHALL require the boat to be inflated
3. THE Game System SHALL allow directional movement while in the boat
4. THE Game System SHALL prevent taking the boat while inside it
5. THE Game System SHALL handle boat deflation appropriately

### Requirement 84: Maze Navigation

**User Story:** As a player, I want the maze to be confusing so that it's a challenging puzzle.

#### Acceptance Criteria

1. THE Game System SHALL implement the maze with non-intuitive connections
2. THE Game System SHALL make maze rooms look similar
3. THE Game System SHALL allow the player to mark their path (drop objects)
4. THE Game System SHALL connect maze exits to the correct destinations
5. THE Game System SHALL implement all maze rooms from 1dungeon.zil

### Requirement 85: Treasure Scoring

**User Story:** As a player, I want treasures to be worth points so that collecting them is rewarding.

#### Acceptance Criteria

1. THE Score System SHALL assign point values to all treasures
2. THE Score System SHALL award points only when treasures are placed in the trophy case
3. THE Score System SHALL not award points for the same treasure twice
4. THE Score System SHALL track which treasures have been scored
5. THE Score System SHALL support the maximum score of 350 points


### Requirement 86: Performance

**User Story:** As a player, I want instant response to commands so that the game feels responsive.

#### Acceptance Criteria

1. THE Game System SHALL respond to commands in less than 10 milliseconds
2. THE Game System SHALL parse input efficiently
3. THE Game System SHALL use efficient data structures for object lookup
4. THE Game System SHALL minimize memory allocations during gameplay
5. THE Game System SHALL maintain consistent performance throughout the game

### Requirement 87: Memory Management

**User Story:** As a developer, I need proper memory management so that the game doesn't leak memory.

#### Acceptance Criteria

1. THE Game System SHALL use smart pointers instead of raw pointers
2. THE Game System SHALL use RAII for resource management
3. THE Game System SHALL not leak memory during normal gameplay
4. THE Game System SHALL not leak memory during save/restore operations
5. THE Game System SHALL use appropriate container types for collections

### Requirement 88: Cross-Platform Compatibility

**User Story:** As a player, I want the game to run on multiple platforms so that I can play anywhere.

#### Acceptance Criteria

1. THE Game System SHALL compile and run on Linux
2. THE Game System SHALL compile and run on macOS
3. THE Game System SHALL compile and run on Windows
4. THE Game System SHALL use platform-independent code where possible
5. THE Game System SHALL handle platform-specific differences appropriately

### Requirement 89: Build System

**User Story:** As a developer, I need a reliable build system so that I can compile the game easily.

#### Acceptance Criteria

1. THE Build System SHALL use CMake for cross-platform builds
2. THE Build System SHALL support debug and release configurations
3. THE Build System SHALL compile with C++17 standard
4. THE Build System SHALL link all required libraries
5. THE Build System SHALL produce a working executable

### Requirement 90: Code Quality

**User Story:** As a developer, I need clean code so that the project is maintainable.

#### Acceptance Criteria

1. THE Codebase SHALL follow consistent naming conventions
2. THE Codebase SHALL use const correctness
3. THE Codebase SHALL avoid code duplication
4. THE Codebase SHALL use meaningful variable and function names
5. THE Codebase SHALL organize code into logical modules

### Requirement 91: Testing Infrastructure

**User Story:** As a developer, I need a test framework so that I can verify correctness.

#### Acceptance Criteria

1. THE Test System SHALL provide a unit test framework
2. THE Test System SHALL provide integration test capabilities
3. THE Test System SHALL provide transcript-based testing
4. THE Test System SHALL report test results clearly
5. THE Test System SHALL support running individual tests or test suites

### Requirement 92: Unit Tests

**User Story:** As a developer, I need unit tests so that I can verify individual components.

#### Acceptance Criteria

1. THE Test System SHALL test object creation and manipulation
2. THE Test System SHALL test flag and property operations
3. THE Test System SHALL test parser tokenization and verb lookup
4. THE Test System SHALL test room navigation
5. THE Test System SHALL achieve high code coverage for core systems

### Requirement 93: Integration Tests

**User Story:** As a developer, I need integration tests so that I can verify system interactions.

#### Acceptance Criteria

1. THE Test System SHALL test complete command sequences
2. THE Test System SHALL test puzzle solutions
3. THE Test System SHALL test NPC interactions
4. THE Test System SHALL test game state persistence
5. THE Test System SHALL verify end-to-end gameplay scenarios


### Requirement 94: Transcript Tests

**User Story:** As a developer, I need transcript tests so that I can verify the game matches the original.

#### Acceptance Criteria

1. THE Test System SHALL replay commands from documented playthroughs
2. THE Test System SHALL verify output matches expected text
3. THE Test System SHALL test the complete game walkthrough
4. THE Test System SHALL use the MIT transcript as a test source
5. THE Test System SHALL catch regressions in game behavior

### Requirement 95: Documentation

**User Story:** As a developer, I need documentation so that I can understand the codebase.

#### Acceptance Criteria

1. THE Project SHALL include a README with build instructions
2. THE Project SHALL include a developer guide for ZIL to C++ conversion
3. THE Project SHALL include inline code comments for complex logic
4. THE Project SHALL include architecture documentation
5. THE Project SHALL include a conversion plan tracking progress

### Requirement 96: Game Completion

**User Story:** As a player, I want to win the game so that I can complete the adventure.

#### Acceptance Criteria

1. THE Game System SHALL allow the player to collect all treasures
2. THE Game System SHALL allow the player to achieve 350 points
3. THE Game System SHALL display a victory message when the game is won
4. THE Game System SHALL track game completion status
5. THE Game System SHALL allow continued play after winning

### Requirement 97: Puzzle Solvability

**User Story:** As a player, I want all puzzles to be solvable so that I can progress through the game.

#### Acceptance Criteria

1. THE Game System SHALL ensure all puzzles have valid solutions
2. THE Game System SHALL not create unwinnable states
3. THE Game System SHALL provide sufficient clues for puzzle solutions
4. THE Game System SHALL allow multiple solution paths where appropriate
5. THE Game System SHALL match the original game's puzzle difficulty

### Requirement 98: Text Consistency

**User Story:** As a player, I want consistent text formatting so that the game feels polished.

#### Acceptance Criteria

1. THE Game System SHALL use consistent capitalization
2. THE Game System SHALL use consistent punctuation
3. THE Game System SHALL use consistent article usage (a/an/the)
4. THE Game System SHALL use consistent verb tenses
5. THE Game System SHALL match the original game's writing style

### Requirement 99: Error Recovery

**User Story:** As a player, I want the game to recover from errors so that it doesn't crash.

#### Acceptance Criteria

1. THE Game System SHALL handle invalid input without crashing
2. THE Game System SHALL handle file I/O errors gracefully
3. THE Game System SHALL handle memory allocation failures gracefully
4. THE Game System SHALL provide meaningful error messages
5. THE Game System SHALL allow the player to continue after recoverable errors

### Requirement 100: Original Fidelity

**User Story:** As a player, I want the port to match the original game exactly so that the experience is authentic.

#### Acceptance Criteria

1. THE Game System SHALL produce identical output to the original Zork I
2. THE Game System SHALL implement all game mechanics from the original
3. THE Game System SHALL use the same puzzle solutions as the original
4. THE Game System SHALL use the same scoring system as the original
5. THE Game System SHALL preserve the original game's feel and atmosphere

---

## Summary

This requirements document specifies 100 requirements covering all aspects of the Zork I C++ port, including:
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

Each requirement follows the EARS pattern and INCOSE quality rules, ensuring clarity, testability, and completeness.

