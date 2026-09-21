# Phase 2: 1actions.zil object/room actions vs C++ (fidelity table)

Status key: F = faithful (text and logic), F* = faithful but NOT WIRED (dead code, never registered as an ACTION), D = divergent, D* = divergent and not wired, M = missing (no C++ counterpart), S = stub, X = extra behaviour not in ZIL.
"wired" = registered via ObjectDef `.action` / `setAction` / `setRoomAction` and therefore reachable from PERFORM.

| ZIL routine (1actions.zil) | C++ | Status | Note |
|---|---|---|---|
| WEST-HOUSE 7 | actions.cpp:25 westHouseAction (wired) | F | |
| EAST-HOUSE 17 | actions.cpp:53 behindHouseAction (wired) | F | |
| OPEN-CLOSE 28 | none | M | PICK-ONE DUMMY "already open/closed" replies never used; callers reimplement with invented text |
| BOARD-F 44 | actions.cpp:885 (wired) | F | |
| TEETH-F 48 | group_a:1933 (wired) | X | adds EXAMINE "razor sharp" and TAKE "embedded" (not in ZIL) |
| GRANITE-WALL-F 64 | group_a:814 (wired) | X | adds EXAMINE and MOVE verbs |
| SONGBIRD-F 82 | group_a:1902 songbirdAction (wired) | S | returns false; all 4 messages missing |
| WHITE-HOUSE-F 92 | actions.cpp:808 (wired) | F | THROUGH mapped to V_ENTER; GOTO replaced by manual move+vLook |
| GO-NEXT 131 | actions.cpp:786 goNext | D | returns bool not 0/1/2; never returns 2 (failed GOTO) |
| FOREST-F 136 | actions.cpp:897 (wired) | X | extra "You aren't even in the forest." when GO-NEXT finds no entry; DISEMBARK also on V_EXIT |
| MOUNTAIN-RANGE-F 152 | group_a:1639 (wired) | D | text "The mountains are impassable." vs "Don't you believe me? The mountains are impassable!"; no CLIMB-FOO |
| WATER-F 157 | group_a:2357 waterAction (wired for WATER, GLOBAL-WATER) | D | rewritten: no vehicle puddle, no SGIVE/THROUGH/BOARD (SWIMYUKS), no GLOBAL-WATER swap, no PUT-in-RIVER/"Nice try."; invented "You have nothing to fill.", "The bottle is not empty.", "There is no water here to fill with." |
| KITCHEN-WINDOW-F 241 | actions.cpp:982 (wired) | D | invented "The window is already open/closed", "The window is open/closed" on EXAMINE, "not open far enough"; ZIL DO-WALKs unconditionally |
| GHOSTS-F 264 | group_a:778 (wired) | F | P-CONT clear omitted |
| BASKET-F 277 | actions.cpp:557 (wired) | D | "already at the top/bottom" invented (ZIL PICK-ONE DUMMY); missing "other end of the chain", THIS-IS-IT, LIT recompute via LIT? |
| BAT-F 308 | group_a:2131 (NOT wired) | D* | KILL vs MUNG; FWEEP count wrong |
| FLY-ME 317 | group_a:2102 | D | std::rand instead of PICK-ONE cycling; FWEEP 4 prints 4 lines (ZIL prints 3) |
| FWEEP 326 | group_a:2095 | D | prints N lines; ZIL prints N-1 |
| BAT-DROPS 332 | group_a:2089 | D | plain vector, no PICK-ONE state |
| BELL-F 343 | group_a:2166 (wired) | D | compares HERE to LAND_OF_LIVING_DEAD; ZIL compares to routine LLD-ROOM (never true) so "Ding, dong." always prints |
| HOT-BELL-F 351 | group_a:938 (wired) | D | POUR-ON: no I-XBH re-queue/immediate call, "The bell appears to have cooled down." never printed |
| BOARDED-WINDOW-FCN 370 | group_a:2184 (wired) | X | adds ATTACK/KILL |
| NAILS-PSEUDO 376 | pseudo_actions.cpp:13 | F* | PSEUDO-OBJECT action is cretinAction; no pseudo dispatch |
| CRACK-FCN 381 | group_a:316 (wired) | X | invented LOOK-INSIDE reply |
| KITCHEN-FCN 385 | group_a:1311 bool kitchenAction (wired as OBJECT action on room, world_init:462); group_a:2222 void kitchenAction(int) (unwired) | D | both "fix" the ZIL bug (CLIMB-DOWN says "no stairs leading down"; ZIL never prints it); (int) version uses rarg==0 for M_LOOK (M_LOOK is 3); engine room action is world_init:439 lambda with hard-coded "window which is open." |
| STONE-BARROW-FCN 403 | actions.cpp:484 (wired) | M | only M_LOOK LDESC; ending text/Tandy-bit/FINISH absent ("Inside the Barrow" 0 hits) |
| BARROW-DOOR-FCN 432 | group_a:162 (wired) | F | |
| BARROW-FCN 436 | group_a:174 | X | adds V_ENTER |
| TROPHY-CASE-FCN 442 | actions.cpp:514 (wired) | X | adds PUT scoring (ZIL scores via LIVING-ROOM-FCN M-END / OTVAL-FROB) |
| LIVING-ROOM-FCN 449 | group_a:1504 (NOT wired; world_init:408 static lambda used) | D* | rarg 0/2 constants wrong (M_LOOK=3, M_END=6); M-END scoring not done; running game shows static "nailed shut ... oriental rug" always |
| TOUCH-ALL 487 | none | M | |
| OTVAL-FROB 496 | none | M | |
| TRAP-DOOR-FCN 504 | group_a:2004 (wired) | D | invented text ("opens to reveal a dark stairway", "slams shut"); no RAISE, LOOK-UNDER, cellar-side lock logic, PICK-ONE DUMMY |
| CELLAR-FCN 531 | group_a:187 (NOT wired; world_init:496 lambda) | F* | slam missing trailing blank line (CR CR) |
| CHIMNEY-F 545 | group_a:210 (wired) | D | "down ward"/"up ward" (ZIL "downward"); invented CLIMB logic and texts |
| UP-CHIMNEY-FUNCTION 553 | dungeon.cpp:104 | F | |
| TRAP-DOOR-EXIT 567 | dungeon.cpp:71 | F | |
| RUG-FCN 579 | actions.cpp:936 (wired) | D | no RAISE; LOOK-UNDER mapped to EXAMINE/LOOK-INSIDE; invented "The rug is lying on one side of the room."; no CLIMB-ON; no THIS-IS-IT |
| LEAVES-APPEAR 774 | none | M | |
| LEAF-PILE 786 | none | M | LEAVES object has no action; burn-always-fatal quirk absent |
| CLEARING-FCN 815 | group_a:288 (NOT wired; world_init:306 lambda) | F* | |
| MAZE-11-FCN 833 | group_a:1620 (NOT wired; world_init:1827 lambda) | D* | wrong text (maze text, "in the floor"), no GRUNLOCK/skull lock line |
| GRATE-FUNCTION 850 | group_a:861 (wired) | D | OPEN/CLOSE (leaves fall, GRATING-ROOM ONBIT), PICK, PUT-through branches missing; uses RoomIds::CLEARING where ZIL uses GRATING-CLEARING; invented "Unlock it with what?" |
| MAZE-DIODES 898 | dungeon.cpp:140 | F | |
| RUSTY-KNIFE-FCN 907 | group_a:1814 | F* | not wired |
| KNIFE-F 926 | group_a:1349 | F* | not wired |
| SKELETON 931 | none | M | BONES has no action |
| TORCH-OBJECT 944 | actions.cpp:1241 torchAction (wired) | D | fully invented (fuel, "too wet", "now burning") |
| MIRROR-ROOM 958 | actions.cpp:337 (wired) | F | |
| MIRROR-MIRROR 971 | actions.cpp:1922 (wired) | D | GOTO replaced by manual move (no M-ENTER/look); "seven years' supply of" line break preserved as newline |
| TORCH-ROOM-FCN 1018 | group_a:1999 stub (NOT wired; world_init:2264 invented text) | M | no DOME-FLAG rope line |
| DOME-ROOM-FCN 1030 | group_a:698 (NOT wired; world_init:2241 lambda) | F* | LEAP death, dead-pull, rope line absent in game |
| LLD-ROOM 1058 | actions.cpp:352 (wired) | F | |
| I-XB/I-XC/I-XBH 1131-1148 | actions.cpp:446-481 | F | |
| DAM-ROOM-FCN 1156 | group_a:434 (NOT wired; world_init:1307 lambda) | F* | |
| BOLT-F 1187 | actions.cpp:2031 (wired) | D | invented EXAMINE; "won't turn with that"/"need a tool" vs "won't turn using the X."; no I-RFILL/I-REMPTY queueing (TODO); no RESERVOIR-SOUTH/LOUD-ROOM TOUCHBIT clears; no TAKE/OIL |
| BUBBLE-F 1219 | actions.cpp:2099 (wired) | F | |
| INTEGRAL-PART 1223 | inline | F | |
| I-RFILL 1226 | none | M | |
| I-REMPTY 1263 | none | M | |
| DROWNINGS 1284 | none | M | |
| BUTTON-F 1298 | actions.cpp:2119 (wired) | D | invented EXAMINE; READ "greek" missing; blue: LEAK not revealed, I-MAINT-ROOM not queued; brown/yellow: DAM-ROOM TOUCHBIT not cleared |
| TOOL-CHEST-FCN 1332 | group_a:1976 | F* | not wired |
| I-MAINT-ROOM 1343 | none | M | flood/drowning absent |
| LEAK-FUNCTION 1362 | group_a:1419 leakFunction, :1458 leakAction | D* | neither wired; PLUG-with-other replies wrong (ZIL WITH-TELL "With a X?"); FIX-MAINT-LEAK never de-queues I-MAINT-ROOM |
| FIX-MAINT-LEAK 1372 | inline | D | |
| PUTTY-FCN 1379 | group_a:1724 | F* | not wired |
| TUBE-FUNCTION 1386 | group_a:2260 | F* | not wired |
| DAM-FUNCTION 1400 | actions.cpp:1995 (wired) | F | extra unreachable "You must specify" branch |
| WITH-TELL 1412 | none | M | |
| RESERVOIR-SOUTH-FCN 1415 | group_a:1802 stub; world_init:1185 lambda | D | invented static text, none of the 4 states |
| RESERVOIR-FCN 1444 | group_a:1792 stub; world_init:1214 lambda | D | invented text; no M-END warning |
| RESERVOIR-NORTH-FCN 1464 | group_a:1797 stub; world_init:1239 lambda | D | invented text |
| BOTTLE-FUNCTION 1491 | actions.cpp:697 (wired) | F | THROW lacks PRSO==BOTTLE guard; ATTACK/KILL added to MUNG |
| LOUD-ROOM-FCN 1660 | group_a:1552 (NOT wired; world_init:739 invented text) | D* | wrong rarg constants; eject goes WEST not PICK-ONE LOUD-RUNS; no raw READ loop; ECHO handler never called by vEcho |
| DEEP-CANYON-F 1730 | group_a:676 (wired) | D | " You can hear..." printed on a new line with leading space (ZIL same line) |
| MOVE-ALL 2114 | none | M | |
| CHALICE-FCN 2123 | group_a:2054 | D* | not wired; no ROBBER-U-DESC check; no DUMB-CONTAINER fallback |
| TREASURE-ROOM-FCN 2138 | group_a:2029 stub; world_init:2179 invented ("You enter the treasure room cautiously.") | M | |
| THIEF-IN-TREASURE 2151 | none | M | |
| FRONT-DOOR-FCN 2163 | group_a:732 (wired) | F | |
| CAVE2-ROOM 2416 | actions.cpp:76 (wired) | F | |
| SWORD-FCN 2432 | actions.cpp:1051 swordAction (wired) | D | invented text; keys on ONBIT not TVALUE 1/2; no I-SWORD enable on TAKE |
| BOOM-ROOM 2446 | actions.cpp:99 (wired) | D | BOOM banner has 4 leading spaces (ZIL 6) |
| BAT-D 2469 | none | M | DESCFCN absent |
| BATS-ROOM 2478 | actions.cpp:144 (wired) | F | inherits FLY-ME/FWEEP defects |
| MACHINE-ROOM-FCN 2488 | group_a:1613 stub; world_init:1076 text from a different game | M | |
| MACHINE-F 2502 | actions.cpp:1840 machineAction (wired) | D | fully invented (TURN on machine, "whirs and crackles", "You open the machine lid.") |
| MSWITCH-FUNCTION 2531 | group_a:1651 | D* | not wired; invented "You need a tool to turn the switch." |
| GUNK-FUNCTION 2553 | group_a:922 (wired) | F | |
| NO-OBJS 2558 | none | M | EMPTY-HANDED never set; LIGHT-SHAFT 13 points unobtainable |
| SOUTH-TEMPLE-FCN 2573 | group_a:1908 stub (NOT wired; world_init:2312 invented PRAY/LISTEN) | M | COFFIN-CURE never set |
| WHITE-CLIFFS-FUNCTION 2585 | none | M | DEFLATE flag never set here |
| SCEPTRE-FUNCTION 2592 | group_a:1880 stub (NOT wired) | M | rainbow puzzle unsolvable |
| FALLS-ROOM 2621 | actions.cpp:163 (wired) | F | |
| RAINBOW-FCN 2634 | group_a:1741 | F* | not wired; GOTO replaced by manual move |
| DBOAT-FUNCTION 2652 | group_a:518 (wired) | F | |
| FIX-BOAT 2664 | inline | F | |
| RIVER-FUNCTION 2669 | group_a:2301 | D* | not wired; LEAP/THROUGH mapped to JUMP/ENTER/SWIM |
| RIVER-SPEEDS/NEXT/LAUNCH 2692 | none | M | |
| I-RIVER 2708 | none | M | |
| RBOAT-FUNCTION 2722 | group_a:1052 inflatedBoatAction (NOT wired); actions.cpp:1618 boatAction wired instead | D | boatAction is fully invented; inflatedBoatAction: LAUNCH is TODO, punctures never JIGS-UP (TODO), NONLANDBIT test inverted to RLANDBIT, ROB of contents omitted, THIS-IS-IT omitted |
| BREATHE 2817 | none | M | dead in ZIL too |
| IBOAT-FUNCTION 2820 | group_a:470 (wired); group_a:1003 duplicate (unwired) | D | wired one omits tan-label line, DEFLATE clear, THIS-IS-IT; duplicate prints label unconditionally |
| RIVR4-ROOM 2844 | actions.cpp:178 (wired) | F | |
| SAND-FUNCTION 2855 | group_a:1857 (called from GROUND-FUNCTION) | D | invented; BEACH-DIG/BDIGS/scarab/collapse absent |
| BDIGS 2871 | none | M | |
| TREE-ROOM 2880 | actions.cpp:219 (wired) | D | LEAP hoisted out of DROP (ZIL bug "fixed"); lists PATH contents with 1 item (ZIL needs 2); nest moved to PATH (ZIL leaves nest in tree); "seriously\ndamaged" line break; CLIMB-DOWN accepts null PRSO |
| EGG-OBJECT 2919 | actions.cpp:1435 (wired) | D | OPEN only (no MUNG); HANDS check uses ADVENTURER; FIGHTBIT "Not to say..." branch missing; HATCH missing; final fallthrough missing |
| BAD-EGG 2960 | actions.cpp:1498 | D | prints hard-coded text instead of BROKEN-CANARY FDESC; moves BROKEN-CANARY (ZIL relies on data) |
| CANARY-OBJECT 2970 | actions.cpp:1543 (wired) | F | SING-SONG replaced by bauble-location test (functionally close) |
| FOREST-ROOM? / I-FOREST-ROOM 2992 | actions.cpp:195/205 | F | timer interval semantics unverified |
| FOREST-ROOM 3004 | actions.cpp:313 (wired) | F | |
| WCLIF-OBJECT 3011 | none | M | |
| CLIFF-OBJECT 3015 | none | M | |
| ROPE-FUNCTION 3030 | actions.cpp:1134 (wired) | D | CLIMB-DOWN returns true without DO-WALK; TIE-UP actor logic, UNTIE, DROP-in-dome, TAKE-when-tied all missing; RMUNGBIT used as "is a room" test |
| UNTIE-FROM 3080 | none | M | dead in ZIL too |
| SLIDE-FUNCTION 3086 / SLIDER 3098 | group_a:1891 (wired) | D | invented text, no movement, no PUT/SLIDER |
| SANDWICH-BAG-FCN 3106 | group_a:1875 (unwired; sack wired to stub sackAction) | D* | uses STUPID-CONTAINER "food" (wrong); ZIL only SMELL "It smells of hot peppers." |
| DEAD-FUNCTION 3113 | group_a:561 (wired by death.cpp:225) | D | PRAY resurrection branch missing; catch-all "You can't even do that." + RFATAL missing; LOOK ignores FIRST? HERE and uses g.lit not HERE ONBIT; missing trailing CRLF |
| LAKE/STREAM/CHASM/DOME/GATE/DOOR/PAINT/GAS-PSEUDO 3177-3231 | pseudo_actions.cpp | F* | none wired (PSEUDO-OBJECT action = cretinAction) |
| RANDOMIZE-OBJECTS 4101 | death.cpp:60 | (other agent) | |
| KILL-INTERRUPTS 4125 | death.cpp:133 | (other agent) | |
| BAG-OF-COINS-F 4137 | group_a:89 (wired) | F | |
| TRUNK-F 4140 | group_a:93 | S | returns false (FIXME) although TRUNK id exists |
| STUPID-CONTAINER 4143 | group_a:57 | X | also handles PUT-ON |
| DUMB-CONTAINER 4154 | none | M | |
| GARLIC-F 4160 | group_a:762 | F* | not wired |
| CHAIN-PSEUDO 4167 | pseudo_actions.cpp:201 | F* | not wired |
| TROLL-ROOM-F 4175 | group_a:2034 stub; world_init:583 lambda | M | THIS-IS-IT troll on enter absent |
