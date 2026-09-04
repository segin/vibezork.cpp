# ZIL Systems Chart
Scanned from `zil` files. (Generic ROUTINES).

| System / Routine | File | Purpose | Logic |
|------------------|------|---------|-------|
| ACCESSIBLE? | gparser.zil | Reachability test | Verifies whether player can touch object based on containment and visibility (gparser.zil:1372-1396) |
| ACLAUSE-WIN | gparser.zil | Adjective orphan resolution | Resolves orphaned clause using supplied adjective (gparser.zil:634-643) |
| AWAKEN | 1actions.zil | | |
| BAD-EGG | 1actions.zil | | |
| BAT-D | 1actions.zil | | |
| BATS-ROOM | 1actions.zil | | |
| BLACK-BOOK | 1actions.zil | | |
| BOOM-ROOM | 1actions.zil | | |
| BREATHE | 1actions.zil | | |
| BUFFER-PRINT | gparser.zil | Token buffer printer | Formats and outputs tokens from word buffer (gparser.zil:819-849) |
| BUT-MERGE | gparser.zil | Exclusion merge | Filters out excluded BUT/EXCEPT objects from candidate table (gparser.zil:945-958) |
| CANARY-OBJECT | 1actions.zil | | |
| CANT-ORPHAN | gparser.zil | Ambiguity query failure | Prints confusion response when non-player winner cannot be orphaned (gparser.zil:777-779) |
| CANT-USE | gparser.zil | Word syntax error | Reports unrecognized grammatical usage of word (gparser.zil:677-686) |
| CAVE2-ROOM | 1actions.zil | | |
| CCOUNT | gverbs.zil | Container item counter | Counts non-NDESC items within a container (gverbs.zil:273-275) |
| CRETIN-FCN | gglobals.zil | Self / player pseudo handler | Handles TELL, GIVE, MAKE, DISEMBARK, EAT, ATTACK, THROW, TAKE, EXAMINE for player/self (gglobals.zil:221-265) |
| CHAIN-PSEUDO | 1actions.zil | | |
| CHASM-PSEUDO | 1actions.zil | | |
| CLAUSE | gparser.zil | Noun clause scanner | Scans noun phrase tokens and records starting position in ITBL (gparser.zil:440-510) |
| CLAUSE-ADD | gparser.zil | Orphan clause append | Appends word token to orphan clause buffer (gparser.zil:882-886) |
| CLAUSE-COPY | gparser.zil | Clause transfer | Copies tokens between clause buffers with optional word substitution (gparser.zil:860-879) |
| CLIFF-OBJECT | 1actions.zil | | |
| CLOCKER | gclock.zil | Master interrupt clock execution | Handles CLOCK-WAIT, runs demons and interrupts, decrements ticks, fires callbacks, and tracks turns (gclock.zil:43-61) |
| DD-APPLY | gmain.zil | Debug object action dispatch wrapper | Invokes dApply with optional object name debug logging (gmain.zil:309-312) |
| DEPOSIT-BOOTY | 1actions.zil | | |
| DESCRIBE-OBJECT | gverbs.zil | Object description printer | Prints long or short description depending on touch status (gverbs.zil:408-417) |
| DESCRIBE-OBJECTS | gverbs.zil | Room objects description loop | Iterates through visible objects in room and describes them (gverbs.zil:420-433) |
| DESCRIBE-ROOM | gverbs.zil | Room description dispatcher | Displays room title and description according to verbosity mode (gverbs.zil:435-467) |
| DO-FIGHT | 1actions.zil | | |
| DO-SL | gparser.zil | Location-scoped search | Searches locations according to search bit flags (gparser.zil:1202-1210) |
| DO-WALK | gverbs.zil | Directional walking routine | Moves adventurer in the specified direction (gverbs.zil:470-473) |
| DOME-PSEUDO | 1actions.zil | | |
| DOOR-PSEUDO | 1actions.zil | | |
| DROP-JUNK | 1actions.zil | | |
| DUMB-CONTAINER | 1actions.zil | | |
| EAST-HOUSE | 1actions.zil | | |
| EGG-OBJECT | 1actions.zil | | |
| FALLS-ROOM | 1actions.zil | | |
| FIGHT-STRENGTH | 1actions.zil | | |
| FIND-IN | gverbs.zil | Flag-based child lookup | Searches container for first object matching a flag (gverbs.zil:656-663) |
| FIND-WEAPON | 1actions.zil | | |
| FINISH | gverbs.zil | Game termination dialog | Prints final score and prompts for RESTART, RESTORE, or QUIT (gverbs.zil:33-54) |
| FIRSTER | gverbs.zil | First visible child helper | Returns first non-NDESC object in container (gverbs.zil:665-671) |
| FIX-BOAT | 1actions.zil | | |
| FIX-MAINT-LEAK | 1actions.zil | | |
| FLY-ME | 1actions.zil | | |
| FOREST-ROOM | 1actions.zil | | |
| FOREST-ROOM? | 1actions.zil | | |
| FWEEP | 1actions.zil | | |
| GAS-PSEUDO | 1actions.zil | | |
| GATE-PSEUDO | 1actions.zil | | |
| GET-OBJECT | gparser.zil | Noun clause object resolution | Resolves candidates in room, inventory, and globals for direct/indirect objects (gparser.zil:1040-1140) |
| GLOBAL-CHECK | gparser.zil | Global object candidate lookup | Matches candidates against local-globals, global-objects, and pseudo-objects (gparser.zil:1169-1200) |
| GLOBAL-IN? | gverbs.zil | Global object accessibility check | Checks if object is in room, local globals, or global objects (gverbs.zil:715-720) |
| GO | 1dungeon.zil | | |
| GO-NEXT | 1actions.zil | | |
| GOTO | gverbs.zil | Room movement engine | Moves winner, updates lighting/darkness, triggers M-ENTER & M-LOOK (gverbs.zil:733-772) |
| GROUND-FUNCTION | gglobals.zil | Ground interaction handler | Handles PUT/PUT-ON redirecting to DROP, Sandy Cave sand handling, and digging (gglobals.zil:170-183) |
| GRUE-FUNCTION | gglobals.zil | Grue examination & lore handler | Handles EXAMINE, FIND, and LISTEN for grue (gglobals.zil:191-206) |
| GRATING-EXIT | 1dungeon.zil | | |
| GWIM | gparser.zil | Contextual object inference | Infers unambiguous candidate matching required bits (gparser.zil:901-926) |
| HACK-HACK | gverbs.zil | Boilerplate action handler | Prints boilerplate response for nonsensical commands (gverbs.zil:720-725) |
| HACK-TREASURES | 1actions.zil | | |
| HELD? | gverbs.zil | Recursive inventory check | Verifies if object is held directly or inside carried containers (gverbs.zil:722-727) |
| HERO-BLOW | 1actions.zil | | |
| HIT-SPOT | gverbs.zil | Fiddling response printer | Prints standard fiddling failure response (gverbs.zil:728-732) |
| I-CANDLES | 1actions.zil | | |
| I-CURE | 1actions.zil | | |
| I-CYCLOPS | 1actions.zil | | |
| I-FIGHT | 1actions.zil | | |
| I-FOREST-ROOM | 1actions.zil | | |
| I-LANTERN | 1actions.zil | | |
| I-MAINT-ROOM | 1actions.zil | | |
| I-MATCH | 1actions.zil | | |
| I-REMPTY | 1actions.zil | | |
| I-RFILL | 1actions.zil | | |
| I-RIVER | 1actions.zil | | |
| I-SWORD | 1actions.zil | | |
| I-THIEF | 1actions.zil | | |
| I-XB | 1actions.zil | | |
| I-XBH | 1actions.zil | | |
| I-XC | 1actions.zil | | |
| IDROP | gverbs.zil | Drop execution helper | Drops PRSO from inventory into current room (gverbs.zil:773-780) |
| INBUF-ADD | gparser.zil | Input buffer word appending | Appends word token into input buffer (gparser.zil:410-423) |
| INBUF-STUFF | gparser.zil | Input buffer transfer | Copies raw bytes between input buffers (gparser.zil:402-406) |
| INFESTED? | 1actions.zil | | |
| INT | gclock.zil | Interrupt entry lookup/allocation | Searches C-TABLE for routine, allocates new interrupt or demon entry if not present (gclock.zil:26-40) |
| INTEGRAL-PART | 1actions.zil | | |
| ITAKE | gverbs.zil | Take execution helper | Moves PRSO to inventory with weight capacity & value scoring (gverbs.zil:781-806) |
| ITAKE-CHECK | gparser.zil | Automatic take enforcer | Performs implicit take on unheld objects if required by syntax (gparser.zil:1248-1292) |
| JIGS-UP | 1actions.zil | | |
| KILL-INTERRUPTS | 1actions.zil | | |
| LAKE-PSEUDO | 1actions.zil | | |
| LANTERN | 1actions.zil | | |
| LEAF-PILE | 1actions.zil | | |
| LEAVES-APPEAR | 1actions.zil | | |
| LIGHT-INT | 1actions.zil | | |
| LIT? | gparser.zil | Light test | Evaluates whether current room or adventurer is illuminated (gparser.zil:1333-1355) |
| LKP | gverbs.zil | Readable text printer | Outputs text for readable surfaces (gverbs.zil:880-890) |
| LLD-ROOM | 1actions.zil | | |
| MAIN-LOOP | gmain.zil | Main interaction loop | Continuous command-processing loop calling MAIN-LOOP-1 (gmain.zil:34-36) |
| MAIN-LOOP-1 | gmain.zil | Single-turn interaction loop | Parses input, processes multi-object syntax, calls PERFORM, executes end-of-turn handlers & clockers (gmain.zil:38-173) |
| MANY-CHECK | gparser.zil | Multi-object validator | Enforces syntax restrictions preventing multi-object targeting (gparser.zil:1294-1313) |
| MAZE-DIODES | 1actions.zil | | |
| META-LOC | gparser.zil | Top-level container locator | Recursively retrieves containing room or global object table (gparser.zil:1398-1407) |
| MIN | 1actions.zil | | |
| MIRROR-MIRROR | 1actions.zil | | |
| MIRROR-ROOM | 1actions.zil | | |
| MOVE-ALL | 1actions.zil | | |
| MUNG-ROOM | gverbs.zil | Room description destruction | Sets RMUNGBIT and alters room long description (gverbs.zil:2183-2189) |
| NAILS-PSEUDO | 1actions.zil | | |
| NCLAUSE-WIN | gparser.zil | Noun orphan resolution | Merges completed noun phrase into pending orphan command (gparser.zil:645-653) |
| NO-GO-TELL | gverbs.zil | Direction blocked printer | Displays message when direction has no exit (gverbs.zil:940-955) |
| NO-OBJS | 1actions.zil | | |
| NOT-HERE-OBJECT-F | gglobals.zil | Missing object error handler | Handles commands targeting non-present items with customized error messages (gglobals.zil:52-74) |
| NOT-HERE-PRINT | gglobals.zil | Missing item name printer | Outputs missing noun phrase or orphan adjective/noun tokens (gglobals.zil:76-84) |
| NULL-F | gglobals.zil | No-op false routine | Dummy routine taking optional arguments and always returning false (gglobals.zil:85-87) |
| NUMBER? | gparser.zil | Number & time parser | Parses integers and HH:MM time expressions into P-NUMBER (gparser.zil:512-535) |
| OBJ-FOUND | gparser.zil | Candidate match recorder | Appends resolved object to candidate table without duplicates (gparser.zil:1239-1242) |
| OPEN-CLOSE | 1actions.zil | | |
| ORPHAN | gparser.zil | Incomplete command save | Preserves parsed verb and partial clauses for orphaning (gparser.zil:782-808) |
| ORPHAN-MERGE | gparser.zil | Orphan command continuation | Merges continuation input into existing orphaned command (gparser.zil:543-630) |
| OTHER-SIDE | gverbs.zil | Door destination lookup | Returns room on other side of a door (gverbs.zil:1050-1065) |
| OTVAL-FROB | 1actions.zil | | |
| PAINT-PSEUDO | 1actions.zil | | |
| PARSER | gparser.zil | Master parser pipeline | Tokenizes and validates grammar, resolves objects, and directs action execution (gparser.zil:109-380) |
| PATH-OBJECT | gglobals.zil | Path / trail handler | Handles TAKE/FOLLOW, FIND, DIG for PATHOBJ (gglobals.zil:282-288) |
| PERFORM | gmain.zil | Central action dispatch hierarchy | Resolves IT, sets PRSA/PRSO/PRSI, and executes 7-tier dispatch: Actor -> Room(M-BEG) -> Preactions -> PRSI -> Container -> PRSO -> Verb (gmain.zil:182-288) |
| PICK-ONE | gmacros.zil | Non-repeating randomized cycle selector | Randomly samples without replacement until table exhausted, then restarts permutation cycle (gmacros.zil:127-140) |
| PRE-BOARD | gverbs.zil | Boarding preaction | Validates vehicle location and player state before boarding (gverbs.zil:201-223) |
| PRE-BURN | gverbs.zil | Burning preaction | Requires PRSI with ONBIT or FLAMEBIT (gverbs.zil:243-250) |
| PRE-DROP | gverbs.zil | Dropping preaction | Handles disembarking if dropping currently ridden vehicle (gverbs.zil:474-478) |
| PRE-FILL | gverbs.zil | Filling preaction | Infers local water or reports nothing to fill with (gverbs.zil:646-654) |
| PRE-GIVE | gverbs.zil | Giving preaction | Verifies direct object is held before giving (gverbs.zil:708-713) |
| PRE-MOVE | gverbs.zil | Moving preaction | Blocks moving held items with juggling message (gverbs.zil:910-922) |
| PRE-MUNG | gverbs.zil | Destroying preaction | Checks weapon requirement for destroying objects (gverbs.zil:923-937) |
| PRE-PUT | gverbs.zil | Putting preaction | Requires TAKEBIT and being held by player (gverbs.zil:1075-1081) |
| PRE-READ | gverbs.zil | Reading preaction | Requires light and transparent reading instrument (gverbs.zil:1137-1144) |
| PRE-SGIVE | gverbs.zil | Reverse giving preaction | Swaps PRSO/PRSI and delegates to V-GIVE (gverbs.zil:1206-1210) |
| PRE-TAKE | gverbs.zil | Taking preaction | Validates item not already held, accessible, or inside closed containers (gverbs.zil:1353-1380) |
| PRE-TURN | gverbs.zil | Turning preaction | Requires TURNBIT on object and TOOLBIT on instrument (gverbs.zil:1488-1494) |
| PREP-FIND | gparser.zil | Preposition word lookup | Translates internal preposition code into vocabulary word (gparser.zil:888-893) |
| PREP-PRINT | gparser.zil | Preposition word printer | Prints leading space and word for preposition code (gparser.zil:851-858) |
| PRINT-CONT | gverbs.zil | Container content formatter | Prints formatted listing of container or surface contents (gverbs.zil:1150-1204) |
| PRINT-CONTENTS | gverbs.zil | Object contents printer | Iterates through child objects and prints indentation list (gverbs.zil:1200-1204) |
| QUEUE | gclock.zil | Schedule interrupt ticks | Retrieves interrupt via INT and sets countdown tick value C-TICK (gclock.zil:21-24) |
| RANDOM-ELEMENT | gmacros.zil | Uniform random element picker | Selects random item from table using uniform distribution (gmacros.zil:124-125) |
| RANDOMIZE-OBJECTS | 1actions.zil | | |
| RECOVER-STILETTO | 1actions.zil | | |
| REMARK | 1actions.zil | | |
| REMOVE-CAREFULLY | gverbs.zil | Container object extraction | Safely removes object from parent container (gverbs.zil:1212-1219) |
| RIVR4-ROOM | 1actions.zil | | |
| ROB | 1actions.zil | | |
| ROB-MAZE | 1actions.zil | | |
| SAILOR-FCN | gglobals.zil | Sailor interaction handler | Handles TELL, EXAMINE, and HELLO Sailor counter progression (gglobals.zil:122-162) |
| SCORE-OBJ | gverbs.zil | Object value scoring | Awards score points from object P?VALUE and clears value to 0 (gverbs.zil:1220-1234) |
| SCORE-UPD | gverbs.zil | Score modification & win check | Adds points to player score and checks against SCORE-MAX for victory (gverbs.zil:1236-1248) |
| SEARCH-LIST | gparser.zil | Containment hierarchy search | Recursively descends object containers matching search criteria (gparser.zil:1216-1237) |
| SEE-INSIDE? | gverbs.zil | Container transparency check | Returns true if container is open, transparent, or a surface (gverbs.zil:1251-1258) |
| SHAKE-LOOP | gverbs.zil | Container spill execution | Empties open container contents onto the ground when shaken (gverbs.zil:1260-1280) |
| SKELETON | 1actions.zil | | |
| SLIDER | 1actions.zil | | |
| SNARF-OBJECTS | gparser.zil | Clause objects resolution | Resolves direct and indirect noun phrases for parser (gparser.zil:928-943) |
| SNARFEM | gparser.zil | Noun phrase parser | Extracts nouns, adjectives, ALL, BUT, and coordinates from tokens (gparser.zil:978-1030) |
| STEAL-JUNK | 1actions.zil | | |
| STAIRS-F | gglobals.zil | Stairs navigation helper | Prompts player to specify up or down when moving THROUGH stairs (gglobals.zil:110-113) |
| STOLE-LIGHT? | 1actions.zil | | |
| STREAM-PSEUDO | 1actions.zil | | |
| STUFF | gparser.zil | Lexv buffer transfer | Copies parsed token structures between parser lexv buffers (gparser.zil:387-399) |
| STUPID-CONTAINER | 1actions.zil | | |
| SYNTAX-CHECK | gparser.zil | Grammar syntax match | Checks input structure against verb definitions and initiates orphaning if needed (gparser.zil:707-775) |
| SYNTAX-FOUND | gparser.zil | Grammar pattern recorder | Sets current syntax pattern and assigns PRSA (gparser.zil:895-898) |
| TAKE-CHECK | gparser.zil | Implicit acquisition check | Verifies direct and indirect objects are held if required by syntax (gparser.zil:1244-1246) |
| TELL-NO-PRSI | gverbs.zil | Missing instrument notification | Prints "You didn't say with what!" (gverbs.zil:240-242) |
| THIEF-IN-TREASURE | 1actions.zil | | |
| THIEF-VS-ADVENTURER | 1actions.zil | | |
| THING-PRINT | gparser.zil | Noun phrase buffer printer | Prints direct or indirect object tokens from ITBL buffer (gparser.zil:810-817) |
| THIS-IS-IT | gverbs.zil | Direct object pronoun tracking | Updates P-IT-OBJECT to reference specified object (gverbs.zil:1420-1424, 2191-2192) |
| THIS-IT? | gparser.zil | Object candidate match test | Tests object for match against target noun, adjective, and flags (gparser.zil:1357-1370) |
| TORCH-OBJECT | 1actions.zil | | |
| TOUCH-ALL | 1actions.zil | | |
| TRAP-DOOR-EXIT | 1actions.zil | | |
| TREASURE-INSIDE | 1dungeon.zil | | |
| TREE-ROOM | 1actions.zil | | |
| UNKNOWN-WORD | gparser.zil | Vocabulary error reporter | Formats and outputs unrecognized word error (gparser.zil:665-675) |
| UNTIE-FROM | 1actions.zil | | |
| VILLAIN-BLOW | 1actions.zil | | |
| VILLAIN-RESULT | 1actions.zil | | |
| VILLAIN-STRENGTH | 1actions.zil | | |
| WCLIF-OBJECT | 1actions.zil | | |
| WEIGHT | gverbs.zil | Recursive weight calculation | Sums object P?SIZE plus recursive weight of all contents (gverbs.zil:1475-1486) |
| WEST-HOUSE | 1actions.zil | | |
| WHICH-PRINT | gparser.zil | Disambiguation prompt | Formats "Which <noun> do you mean, the X or the Y?" (gparser.zil:1146-1166) |
| WINNER-RESULT | 1actions.zil | | |
| WINNING? | 1actions.zil | | |
| WITH-TELL | 1actions.zil | | |
| WORD-PRINT | gparser.zil | Raw word printer | Outputs raw characters of word token (gparser.zil:658-663) |
| WT? | gparser.zil | Word part-of-speech test | Tests token part of speech bit mask (gparser.zil:430-436) |
| YES? | gverbs.zil | Interactive confirmation prompt | Prompts user with > and returns true if YES or Y (gverbs.zil:1515-1530) |
| ZMEMQ | gparser.zil | Table membership check | Tests if item exists in vector or table (gparser.zil:1315-1322) |
| ZMEMQB | gparser.zil | Byte table membership check | Tests if byte value exists in byte array (gparser.zil:1324-1329) |
| ZPROB | gmacros.zil | Luck-weighted probability test | Rolls against 1-100 if lucky or 1-300 if unlucky (gmacros.zil:119-123) |
| ZORKMID-FUNCTION | gglobals.zil | Currency examination handler | Handles EXAMINE and FIND for Zorkmid currency (gglobals.zil:296-302) |
| BUZZ-TABLE | gsyntax.zil | Parser buzzwords table | 21 command and general buzzwords ignored by parser (gsyntax.zil:9-12) |
| DIR-SYNONYMS | gsyntax.zil | Direction synonyms table | 10 direction synonym groups mapping abbreviations to canonical directions (gsyntax.zil:25-34) |
| PREP-SYNONYMS | gsyntax.zil | Preposition synonyms table | 4 preposition synonym groups (with, in, on, under) (gsyntax.zil:20-23) |
| SYNTAX-TABLE | gsyntax.zil | Grammar syntax table | 267 syntax patterns defining verb structures, search scopes, find flags, and action dispatches (gsyntax.zil:40-562) |
| VERB-SYNONYMS | gsyntax.zil | Verb synonyms table | 61 verb synonym groups mapping player inputs to canonical verbs (gsyntax.zil:45-560) |
