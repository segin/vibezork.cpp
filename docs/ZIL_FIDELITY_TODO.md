# ZIL Fidelity TODO

Source: the read-only audit in `docs/audit/` (`zil_audit.md`, `cpp_vs_zil_audit.md`
and the per-slice tables). Goal: the running C++ reproduces Zork I Release 119
(`zil/`) verbatim, including the original's quirks and bugs. The compiled story
`zil/COMPILED/zork1.z3` under `dfrotz` is the behavioural oracle.

Rules (per AGENTS.md): one item at a time; mark `[/]` before starting; implement,
test, build, commit, push; then mark `[x]` and update the counters. Every routine
ported carries `// ZIL:` and `// Source: <file>:<lines>` comments. Never simplify
away a ZIL behaviour.

Progress: 54/69 (78%)

## Phase A: engine foundations (src/core, src/systems/timer)

- [x] A1 Room and object action handlers return a tri-state (M-NOT-HANDLED / M-HANDLED / M-FATAL) instead of void/bool; PERFORM stops when the room's M-BEG call returns true (gmain.zil:212; cpp_vs_zil_audit §2.1)
- [x] A2 RFATAL propagation: value 2 aborts the multi-object loop, skips M-END, clears P-CONT; P-CONT cleared on parse failure (gmain.zil:150-163; §2.3)
- [x] A3 Direction commands go through PERFORM with PRSA=V?WALK and P-WALK-DIR (gmain.zil:79-81; §2.2)
- [x] A4 Object model: FDESC property, VTYPE property, PROPDEF defaults SIZE 5 / CAPACITY 0 / VALUE 0 / TVALUE 0 honoured by getProperty; remove LOCKEDBIT/DEADBIT concepts (§2.15, §7.2, §7.7)
- [x] A5 MOVES incremented only inside CLOCKER; remove ScoreSystem::moves_ duplicate (gclock.zil:50; §2.7)
- [x] A6 CLOCKER fidelity: C-TABLE of 30 entries allocated downward and scanned from the newest, fires when tick==1 or negative, FLG only when the routine returns true, no auto-repeat, no demon concept (gclock.zil:21-60; §2.9-2.10)
- [x] A7 GO startup: queue I-FIGHT -1 enabled, I-SWORD -1 disabled, I-THIEF -1 enabled, I-CANDLES 40, I-LANTERN 200; DEF*-RES patch; INFLATED-BOAT VTYPE=NONLANDBIT; V-VERSION when WEST-OF-HOUSE untouched; V-LOOK (1dungeon.zil:2637-2660; §2.8)
- [x] A8 Output layer: TELL does not append CRLF, no automatic inter-print spacing, source newlines inside strings become spaces, wrap at screen width like the Z-machine; prompt is ">" (io.cpp; §2.14, §2.16)
- [x] A9 IT: substitution in P-PRSI then P-PRSO, PERFORM's ACCESSIBLE? check with "I don't see what you are referring to." + RFATAL, P-IT-OBJECT rule with the PRSI!=IT and WALK guards; THEM/HER/HIM are IT synonyms (gmain.zil:45-64, 194-203; §2.4)
- [x] A10 Multi-object loop per MAIN-LOOP-1: NUM>1 without ALL, "name: " prefix, the three "multiple exceptions", P-NOT-HERE and "The [other] object[s] that you mentioned is/aren't here.", any-verb "There's nothing here you can take.", PRSO/PRSI role swap (gmain.zil:65-150; §2.5)
- [x] A11 Zero-object branch: "It's too dark to see." / "It's not clear what you're referring to." (gmain.zil:82-90; §2.6)
- [x] A12 gglobals fidelity: GROUND-FUNCTION and CRETIN-FCN use PERFORM; GRUE is "lurking grue" in GLOBAL-OBJECTS without INVISIBLE; GROUND without NDESCBIT/INVISIBLE; NOT-HERE-PRINT spacing; "I beg your pardon?" on empty input; remove "That command is too long." and "Goodbye!" (gglobals.zil; §2.11-2.13, §2.16)

## Phase B: parser (src/parser)

- [x] B1 Lexer per PARSER/READ: P-INBUF 120 bytes, P-LEXV 59 entries, dictionary built from GSyntax tables plus object SYNONYM/ADJECTIVE words with 6-character truncation, WORD-PRINT echoing the typed word (gparser.zil:109-380, 658-663)
- [x] B2 CLAUSE, punctuation, THEN/AND/"."/"," splitting, P-CONT continuation, "dir then"/"dir, ..." rewrites, TO-after-TELL quote, leading-THEN rules, "There were too many nouns in that sentence.", "Please consult your manual..." (gparser.zil:139-146, 217-368, 440-510)
- [x] B3 OOPS: all messages, INBUF-STUFF/INBUF-ADD splice, first-word-only warning (gparser.zil:177-206, 402-428)
- [x] B4 AGAIN/G: "Beg pardon?", "It's difficult to repeat fragments.", "That would just repeat a mistake.", "AGAIN, x" via RESERVE-LEXV, P-OTBL restore, STUFF 29-entry limit (gparser.zil:132-138, 211-250, 387-400)
- [x] B5 SYNTAX-CHECK, GWIM (incl. the FIND RMUNGBIT no-object idiom and "(the X)"/"(with your hands)" echoes), ORPHAN, ORPHAN-MERGE, ACLAUSE-WIN/NCLAUSE-WIN, CANT-ORPHAN (gparser.zil:543-655, 707-926)
- [x] B6 SNARF-OBJECTS, SNARFEM, BUT-MERGE, GET-OBJECT two-pass search with the TRANSBIT trick, DO-SL/SEARCH-LIST with syntax scope bits, OBJ-FOUND, "(How about the X?)", "There seems to be a noun missing in that sentence!", "It's too dark to see!" (gparser.zil:928-1140, 1202-1243). (Oracle note resolved: "take all except mailbox" at West of House leaves the NDESCBIT front door in P-PRSO, which the TAKE-ALL rule skips, hence "There's nothing here you can take."; with nothing left, e.g. "drop all except leaflet", the game prints "It's not clear what you're referring to." as the source predicts)
- [x] B7 GLOBAL-CHECK with pseudo objects (PSEUDO-OBJECT action rewrite) and the ROOMS-only-for-LOOK-INSIDE/SEARCH/EXAMINE rule; WHICH-PRINT single-line question answered by orphaning (gparser.zil:1146-1200)
- [x] B8 MANY-CHECK, TAKE-CHECK, ITAKE-CHECK with "(Taken)", "You don't have the X.", "You don't have that!", HANDS/ME exemptions (gparser.zil:1244-1313)
- [x] B9 NUMBER? with all rejection rules and W?INTNUM substitution; INTNUM object (gparser.zil:512-534)
- [x] B10 LIT? via the object search with P-GWIMBIT=ONBIT, THIS-IT? byte adjective matching, ACCESSIBLE?/META-LOC, ALWAYS-LIT (gparser.zil:1315-1407)
- [x] B11 Wire GParser/GSyntax as the runtime parser; delete the hand-typed Parser vocabulary, VerbRegistry duplicates, numbered disambiguation menu, INHIBIT flag, "everything", GWIMBIT auto-select, extra prepositions, and every invented parser message (parser.cpp, verb_registry.cpp)
- [x] B12 Debug syntaxes $VERIFY, #RANDOM, #COMMAND, #RECORD, #UNRECORD with their prefixes; V-COMMAND-FILE/V-RECORD/V-UNRECORD/V-RANDOM/V-VERIFY per gverbs.zil:123-147

## Phase C: generic verbs (src/verbs, gverbs.zil)

- [x] C1 ITAKE, IDROP, CCOUNT, WEIGHT, V-TAKE, PRE-TAKE, V-DROP, PRE-DROP incl. DEAD branch, YUKS, closed-container kludge, load message with LOAD-ALLOWED suffix, RFATAL, fumble rule, TOUCHBIT/NDESCBIT/SCORE-OBJ, WEARBIT (gverbs.zil:1353-1389, 1900-1999)
- [x] C2 V-PUT (with the NOT-ITAKE truthiness quirk), PRE-PUT=PRE-GIVE, V-PUT-ON, V-PUT-UNDER, V-PUT-BEHIND, V-GIVE, PRE-GIVE, V-SGIVE, PRE-SGIVE (gverbs.zil:704-733, 1075-1135, 1206-1240)
- [x] C3 V-OPEN (single-untouched-FDESC case, PRINT-CONTENTS, THIS-IS-IT), V-CLOSE (CAPACITY/SURFACEBIT gate, "It is now pitch black."), SEE-INSIDE?, V-LOOK-INSIDE, V-LOOK-ON, V-SEARCH, V-EXAMINE (gverbs.zil:336-360, 574-600, 866-905, 966-994, 1730-1748, 1837-1840)
- [x] C4 DESCRIBE-ROOM (MAZEBIT always verbose, vehicle suffix, M-FLASH), DESCRIBE-OBJECTS ("Only bats can see in the dark. And you're not one."), DESCRIBE-OBJECT (FDESC/TOUCHBIT/LDESC rule, " (providing light)", " (outside the X)"), PRINT-CONT two-pass with INDENTS, FIRSTER, V-LOOK, V-FIRST-LOOK, V-INVENTORY, fixed "A "/"a " articles (gverbs.zil:29-31, 1622-1840)
- [x] C5 GOTO (VTYPE vehicle check, RMUNGBIT rooms, dark-to-dark 80 percent grue with SPRAYED?, "You have moved into a dark place.", SCORE-OBJ on rooms, ENTRANCE-TO-HADES suppression, "comes to a rest on the shore."), V-WALK (dark death rule), DO-WALK, NO-GO-TELL, V-WALK-AROUND, V-WALK-TO, OTHER-SIDE, LKP, MUNG-ROOM, THIS-IS-IT, GLOBAL-IN?, FIND-IN, HELD? (gverbs.zil:1521-1580, 2000-2216)
- [x] C6 SCORE-UPD (350 side effects), SCORE-OBJ, V-SCORE with exact G? thresholds, YES? (">" then first word YES/Y) (gverbs.zil:1841-1877; 1actions.zil:4026-4045)
- [x] C7 Game commands: V-VERBOSE, V-BRIEF, V-SUPER-BRIEF, V-QUIT, V-RESTART, V-RESTORE ("Ok."/"Failed.", V-FIRST-LOOK), V-SAVE, V-SCRIPT, V-UNSCRIPT, V-VERSION (exact text, Release 119 / Serial 880429), FINISH loop (gverbs.zil:9-121)
- [x] C8 V-WAIT: "Time passes..." then up to three CLOCKER ticks with early stop, CLOCK-WAIT (gverbs.zil:1514-1519)
- [x] C9 V-TELL actor-command mode (WINNER/HERE switch), V-SAY, V-COMMAND, V-HELLO with HELLOS, V-ANSWER, V-REPLY, V-YELL, V-CURSES, V-MUMBLE, V-REPENT, V-PRAY (gverbs.zil:151-176, 314-330, 738-766, 1050-1073, 1195-1204, 1389-1400, 1612-1618)
- [x] C10 Light verbs: V-LAMP-ON (CRLF + V-LOOK when dark), V-LAMP-OFF ("It is now pitch black."), V-BURN, PRE-BURN ("With a X??!?"), V-STRIKE, REMOVE-CAREFULLY (P-IT-OBJECT clear, LIT recompute, "You are left in the dark..."), TELL-NO-PRSI ("You didn't say with what!") (gverbs.zil:240-270, 610-628, 792-826, 1329-1350)
- [x] C11 V-ATTACK (HERO-BLOW dispatch, HANDS case), V-SWING, V-STAB with FIND-WEAPON, V-MUNG, PRE-MUNG, V-KICK, V-KISS, V-RAPE, V-ALARM with AWAKEN (gverbs.zil:176-199, 923-964, 1288-1327, 1360-1362)
- [x] C12 Remaining generic verbs A-L verbatim: V-ADVENT, V-BACK, V-BLAST, V-BOARD, PRE-BOARD, V-BREATHE, V-BRUSH, V-BUG, V-CHOMP, V-CLIMB-UP/DOWN/FOO/ON, V-COUNT, V-CROSS, V-CUT, V-DEFLATE, V-DIG, V-DISEMBARK, V-DISENCHANT, V-DRINK, V-DRINK-FROM, V-EAT, V-ECHO, V-ENCHANT, V-ENTER, V-EXIT, V-EXORCISE, V-FILL, PRE-FILL, V-FIND, V-FOLLOW, V-FROBOZZ, V-HATCH, V-INCANT, V-INFLATE, V-KNOCK, V-LAUNCH, V-LEAN-ON, V-LEAP with JUMPLOSS/WHEEEEE, V-LEAVE, V-LISTEN, V-LOCK, V-LOOK-BEHIND, V-LOOK-UNDER, V-LOWER
- [x] C13 Remaining generic verbs M-Z verbatim: V-MAKE, V-MELT, V-MOVE, PRE-MOVE, V-ODYSSEUS hook, V-OIL, V-OVERBOARD, V-PICK, V-PLAY, V-PLUG, V-POUR-ON, V-PUMP, V-PUSH, V-PUSH-TO, V-RAISE, V-READ, PRE-READ, V-READ-PAGE, V-RING, V-RUB, V-SEND, V-SHAKE with SHAKE-LOOP, V-SKIP, V-SMELL, V-SPIN, V-SPRAY, V-SSPRAY, V-SQUEEZE, V-STAND, V-STAY, V-SWIM with SWIMYUKS, V-THROUGH, V-THROW, V-THROW-OFF, V-TIE, V-TIE-UP, V-TREASURE, V-TURN, PRE-TURN, V-UNLOCK, V-UNTIE, V-WAVE, V-WEAR, V-WIN, V-WIND, V-WISH, V-ZORK
- [x] C14 Random tables as PICK-ONE tables: JUMPLOSS, WHEEEEE, HO-HUM, HELLOS, YUKS, DUMMY, SWIMYUKS; HACK-HACK, HIT-SPOT (gverbs.zil:518-532, 2024-2036, 2194-2216)

## Phase D: world data (src/world, 1dungeon.zil)

- [x] D1 All 110 rooms regenerated from 1dungeon.zil: DESC, LDESC, every exit form (UEXIT/NEXIT string/CEXIT flag/DEXIT door/FEXIT PER), FLAGS, VALUE, ACTION, GLOBAL lists, PSEUDO clauses (docs/audit/world_data_audit.md)
- [x] D2 All 140 objects regenerated from 1dungeon.zil and gglobals.zil: IN, SYNONYM/ADJECTIVE with truncations, DESC, FDESC, LDESC, TEXT, FLAGS, SIZE, CAPACITY, VALUE, TVALUE, STRENGTH, ACTION, DESCFCN; add BAT, KITCHEN-TABLE, ATTIC-TABLE, LEAK, MACHINE-SWITCH, CONTROL-PANEL; remove "trophy", duplicate bag of coins, "window"
- [x] D3 Delete the dead builder-style world files (rooms_*.cpp, treasures.cpp, containers.cpp, scenery.cpp, npcs.cpp, tools.cpp, readables.cpp, special_objects.cpp data parts). The invented state globals stay for now: every one is still read by live code that phases F and G own (lampBattery/matchCount by the lamp and match systems, rugMoved/gateFlag/gatesOpen/waterLevel/grunlock/grateRevealed/buoyFlag by the trap door, dam, grate and river handlers)
- [x] D4 Walk-around tables, DIRECTIONS incl. LAND, conditional-exit flag globals, SCORE-MAX, and a data-level test that diffs the live world against a parse of 1dungeon.zil

## Phase E: melee, NPCs, death (1actions.zil:622-769, 1491-2177, 3236-3990, 4046-4177)

- [x] E1 Melee data: F-* constants, DEF1/DEF2A/DEF2B/DEF3A/DEF3B/DEF3C, DEF*-RES sliding windows, HERO/CYCLOPS/TROLL/THIEF-MELEE message tables verbatim, VILLAINS table (1actions.zil:3236-3330, 3606-3803)
- [x] E2 Melee engine: DO-FIGHT, REMARK, FIGHT-STRENGTH, VILLAIN-STRENGTH (V-BEST-ADV, THIEF-ENGROSSED, sword advantage on the troll's own blow), VILLAIN-BLOW, HERO-BLOW, WINNER-RESULT, VILLAIN-RESULT, WINNING?, I-FIGHT, AWAKEN, I-CURE, CURE-WAIT, wounds as negative STRENGTH and LOAD-ALLOWED changes (1actions.zil:3331-3606, 3810-3849)
- [x] E3 Troll: TROLL-FCN all modes, WEAPON-FUNCTION, AXE-F, TROLL-ROOM-F, TROLL-FLAG (1actions.zil:622-769, 4160-4177)
- [x] E4 Thief: I-THIEF, THIEF-VS-ADVENTURER, ROB, STEAL-JUNK, ROB-MAZE, DROP-JUNK, DEPOSIT-BOOTY, RECOVER-STILETTO, HACK-TREASURES, STOLE-LIGHT?, ROBBER-FUNCTION, ROBBER-C-DESC/U-DESC, LARGE-BAG-F, STILETTO handling, CHALICE-FCN, TREASURE-ROOM-FCN, THIEF-IN-TREASURE, EGG-SOLVE (1actions.zil:1748-2177, 3851-3990)
- [x] E5 Cyclops: CYCLOPS-FCN, I-CYCLOPS, CYCLOMAD (six lines), CYCLOPS-ROOM-FCN, V-ODYSSEUS, MAGIC-FLAG/CYCLOPS-FLAG, MIN (1actions.zil:1491-1660, 2339)
- [x] E6 Death: JIGS-UP (third death ends, SOUTH-TEMPLE Hades rule, "Bad luck, huh?", no prompts), RANDOMIZE-OBJECTS, KILL-INTERRUPTS exact set, DEAD-FUNCTION incl. PRAY resurrection and "You can't even do that." + RFATAL (1actions.zil:3113-3175, 4046-4159)
- [x] E7 V-DIAGNOSE per 1actions.zil:3993-4025 (wounds, cure countdown, death count)
- [x] E8 Remove combat.cpp HP model, invented NPC prose, processTrollTurn/processCyclopsTurn, resurrection prompts

## Phase F: light sources (1actions.zil:2178-2445, 3851-3889)

- [x] F1 LANTERN, I-LANTERN, LAMP-TABLE, LIGHT-INT, BROKEN-LAMP (THROW), "A burned-out lamp won't light." (1actions.zil:2178-2325)
- [x] F2 CANDLES-FCN, I-CANDLES, CANDLE-TABLE, first-touch enable, RMUNGBIT end state (1actions.zil:2326-2415)
- [x] F3 MATCH-FUNCTION (count 6 reporting count-1, drafty rooms), I-MATCH (1actions.zil:2255-2300)
- [x] F4 SWORD-FCN, I-SWORD three glow states via TVALUE, INFESTED? (1actions.zil:2416-2445, 3851-3889)
- [ ] F5 TORCH-OBJECT; delete light.cpp/lamp.cpp/candle.cpp/sword.cpp invented systems

## Phase G: object and room actions (1actions.zil, docs/audit/phase2_actions_table.md)

- [ ] G1 House and cellar: WEST-HOUSE, EAST-HOUSE, WATER-F, KITCHEN-WINDOW-F, KITCHEN-FCN (duplicate CLIMB-UP test kept), LIVING-ROOM-FCN, RUG-FCN, TRAP-DOOR-FCN, TRAP-DOOR-EXIT, CELLAR-FCN, BASKET-F, BAT-F, FLY-ME, FWEEP (N-1 lines), BAT-D, CHIMNEY-F, UP-CHIMNEY-FUNCTION, BOARD-F, BOARDED-WINDOW-FCN, FRONT-DOOR-FCN, MAILBOX, ADVERTISEMENT, TROPHY-CASE, STONE-BARROW-FCN with Tandy-bit check and FINISH, BARROW-DOOR-FCN, BARROW-FCN, WHITE-HOUSE, TEETH-F, GRANITE-WALL-F, WALL (1actions.zil:7-620)
- [ ] G2 Grating, maze, knives: LEAF-PILE (burn always fatal), LEAVES-APPEAR, GRATE-FUNCTION, GRATING-EXIT, GRATING-CLEARING, MAZE-DIODES, RUSTY-KNIFE-FCN, KNIFE-F, SKELETON, BONES, BAG-OF-COINS-F, CLEARING-FCN, FOREST-F, MOUNTAIN-RANGE-F, PATH (1actions.zil:771-957)
- [ ] G3 Mirror, dome, Hades: MIRROR-MIRROR, MIRROR-ROOM, LLD-ROOM, I-XB, I-XC, I-XBH, BELL-F (LLD-ROOM routine comparison quirk), HOT-BELL-F with I-XBH 0, BOOK-F, DOME-ROOM-FCN, TORCH-ROOM-FCN, NORTH-TEMPLE, SOUTH-TEMPLE-FCN, ENGRAVINGS, ALTAR (1actions.zil:958-1152)
- [ ] G4 Dam and reservoir: BOLT-F, BUTTON-F, DAM-FUNCTION, DAM-ROOM-FCN, RESERVOIR-FCN, RESERVOIR-NORTH-FCN, RESERVOIR-SOUTH-FCN, I-RFILL, I-REMPTY, I-MAINT-ROOM with DROWNINGS (entries 7-8 unreachable), LEAK-FUNCTION, WITH-TELL, PUTTY-FCN, TUBE-FUNCTION, TOOL-CHEST-FCN, BUBBLE-F, CONTROL-PANEL, TRUNK-F, TRIDENT (1actions.zil:1153-1489)
- [ ] G5 Bottle and loud room: BOTTLE-FUNCTION, WATER-F pour/drink chain, LOUD-ROOM-FCN raw READ loop with ECHO clearing SACREDBIT, V-ECHO, BAR, DEEP-CANYON-F (1actions.zil:157-215, 1491-1514, 1660-1747)
- [ ] G6 Coal mine: BOOM-ROOM (6-space banner), BATS-ROOM, MACHINE-F, MSWITCH-FUNCTION, NO-OBJS with LIGHT-SHAFT bonus, TIMBER-ROOM/LOWER-SHAFT drafty rule, SLIDE-FUNCTION, SLIDER, SLIDE-ROOM, LADDER, SHAFT-ROOM, GUNK-FUNCTION, DIAMOND (1actions.zil:2446-2583)
- [ ] G7 River and beach: SCEPTRE-FUNCTION (WALL dump), RAINBOW-FCN, RBOAT-FUNCTION (removal loses non-treasures), I-RIVER, RIVER-SPEEDS/NEXT/LAUNCH, GO-NEXT, IBOAT-FUNCTION, DBOAT-FUNCTION, BOAT-LABEL, PUMP, FIX-BOAT, RIVER-FUNCTION, WHITE-CLIFFS-FUNCTION, WCLIF-OBJECT, CLIFF-OBJECT, FALLS-ROOM, RIVR4-ROOM, CANYON-VIEW-F, SAND-FUNCTION, SHOVEL, BUOY, SANDY-CAVE, POT-OF-GOLD, TORCH (1actions.zil:2585-2878)
- [ ] G8 Forest, egg, rope: TREE-ROOM (LEAP nested under DROP, nest stays in tree, two-object M-LOOK rule), FOREST-ROOM, I-FOREST-ROOM, SONGBIRD-F, NEST, EGG-OBJECT (FIGHTBIT second attempt), CANARY-OBJECT, BAUBLE, ROPE-FUNCTION, DOME-FLAG, RAILING, SLIDE (1actions.zil:2880-3112)
- [ ] G9 Death and pseudo objects: DEAD-FUNCTION verb list, all ten *-PSEUDO routines wired through PSEUDO-OBJECT, PSEUDO clauses in rooms (1actions.zil:3113-3234)
- [ ] G10 Containers and misc: DUMB-CONTAINER, STUPID-CONTAINER, TOUCH-ALL, OTVAL-FROB, TREASURE-INSIDE, OPEN-CLOSE, MOVE-ALL, INTEGRAL-PART, GARLIC-F, LUNCH, SANDWICH-BAG-FCN, GHOSTS-F, BODY-FUNCTION, CRACK-FCN, MACHINE-ROOM-FCN, STAIRS-F, CAVE2-ROOM, remove all invented extra verbs (1actions.zil:4101-4159 and scattered)

## Phase H: save, restore, restart, finish

- [ ] H1 Full state save/restore (every object location, flags, properties, every global, timer table, RNG state) with ZIL prompts and "Ok."/"Failed."; V-RESTART performs a true restart; FINISH loop per gverbs.zil:33-53

## Phase I: tests, docs, oracle

- [ ] I1 Transcript oracle test: run scripted sessions through dfrotz on zil/COMPILED/zork1.z3 and through the port, diff normalised output; seed the RNG identically where possible; add to ctest
- [ ] I2 Retire tests that assert invented behaviour or exercise dead code; keep and extend per-routine tests against ZIL text
- [ ] I3 Rewrite docs/zil_actions.md truthfully (every ZIL routine, status by the oracle), fix README.md and docs/PROJECT_STATUS.md claims
