# World DATA audit: C++ runtime world vs zil/1dungeon.zil + gglobals.zil

Method: built libzork_core.a in the scratchpad, ran a dumper that executes the
same initialization as src/main.cpp (initializeWorld + verb handlers + NPC/lamp/
candle/sword init + GO's setup) and serialised every registered object (desc,
ldesc, text, synonyms, adjectives, flags, properties, location, exits, room
globals). Parsed every ROOM/OBJECT form in the two ZIL files with an MDL reader
(250 forms = zork1.record's count) and compared field by field.
Full mismatch list: world_data_mismatches_full.txt (651 lines).
Scripts: dump_world2.cpp, zil_extract.py, compare_world.py.

## 0. Which C++ data is live
src/main.cpp calls initializeWorld() (src/world/world_init.cpp, 201 creation
sites). The alternate builders initializeAllObjects()/initializeAllRooms()
declared in src/world/world_objects.h and implemented in rooms_*.cpp,
treasures.cpp, containers.cpp, tools.cpp, scenery.cpp, npcs.cpp, readables.cpp,
special_objects.cpp, consumables.cpp, vehicles.cpp, player.cpp are NEVER called
anywhere in src/ or tests/. They are dead duplicates carrying their own (often
different) data, e.g. rooms_house.cpp gives LIVING-ROOM a DOWN->CELLAR exit and
ATTIC a SOUTH exit. Findings below concern the live world_init.cpp data.

## 1. Counts
ZIL: 110 rooms + 140 objects (122 in 1dungeon, 18 in gglobals) = 250.
C++ live: 110 rooms + 137 objects = 247.
Missing in C++ (never registered): BAT (1dungeon 1080), KITCHEN-TABLE (247),
ATTIC-TABLE (262), LEAK (584), MACHINE-SWITCH (766), CONTROL-PANEL (392).
actions_group_a.cpp:1356 looks up ATTIC_TABLE and gets nullptr.
C++ objects with no ZIL basis: id 100 TROPHY "trophy" (TAKEBIT, VALUE 5,
TVALUE 5, placed in TREASURE-ROOM: a phantom 10-point treasure); id 108
COINS "bag of coins" (in MAZE-5, VALUE 10 TVALUE 5) duplicating id 743
BAG_OF_COINS (nowhere, TVALUE 0, has CONTBIT); id 502 WINDOW "window"
(nowhere). Only one of 108/743 is reachable; 743 is the one with the action
handler (bagOfCoinsAction), 108 is the one the player can find.

## 2. Systemic property divergences
- SIZE default: ZIL PROPDEF SIZE 5 (zork1.zil:24) means every object without
  an explicit SIZE weighs 5. ZObject::getProperty returns 0 for unset
  (core/object.cpp:11-14), and createObject skips size<=0. 73 objects have
  ZIL-default 5 vs C++ 0 (all scenery/global objects, mailbox, trophy case,
  buttons, boats' pile, thief, troll, cyclops ...). 23 more have explicit
  wrong values: AXE 25->15, BOOK 10->5, GARLIC 4->2, STILETTO 10->5, GUNK
  10->0, INFLATABLE-BOAT 20->0, INFLATED-BOAT 20->50, PUNCTURED-BOAT 20->30,
  BOAT-LABEL 2->1, and invented values for BAUBLE 2, BOTTLE 8, CANARY 3,
  BROKEN-CANARY 3, EMERALD 2, GUIDE 3, KNIFE 10, LARGE-BAG 20, LUNCH 4,
  MACHINE 100, OWNERS-MANUAL 3, PUMP 8, SCREWDRIVER 8, SKULL 15 (ZIL: 5).
  Load/capacity arithmetic (LOAD-ALLOWED 100, PUT "There's no room.") cannot
  match the original with these numbers.
- STRENGTH: ZIL only sets TROLL 2, THIEF 5, CYCLOPS 10000, ADVENTURER 0
  (gglobals 271). C++: TROLL 8, CYCLOPS 10, ADVENTURER 10, and STRENGTH is
  overloaded as weapon damage / fuel on AXE 7, KNIFE 5, SWORD 10, STILETTO 4,
  TORCH 30, CANDLES 50, LAMP 100, MATCH 5. None of these exist in ZIL; the
  fight tables key on STRENGTH deltas so combat cannot reproduce.
- CAPACITY: LAMP 330 (used as battery; ZIL 0), LARGE-BAG 100 (ZIL 0),
  TOOL-CHEST 40 (ZIL 0), MACHINE 20 (ZIL 50). TROPHY-CASE 10000 matches.
- Room VALUE: KITCHEN 10, CELLAR 25, EW-PASSAGE 5 are 0 in C++;
  TREASURE-ROOM 25 matches. 40 room-entry points cannot be awarded from data.
- VALUE/TVALUE on objects all match except BAG-OF-COINS (id 743 TVALUE 0;
  the duplicate id 108 has 10/5) and the phantom TROPHY.

## 3. Initial locations (28 wrong, HIGH)
Nowhere at start although ZIL places them: ALTAR (SOUTH-TEMPLE), BOOK (on
ALTAR), GUIDE (DAM-LOBBY), MAP (TROPHY-CASE), OWNERS-MANUAL (STUDIO),
PEDESTAL (TORCH-ROOM; TORCH is moved onto the pedestal so the torch is also
unreachable), TOOL-CHEST (MAINTENANCE-ROOM), BAG-OF-COINS 743 (MAZE-5),
BOAT-LABEL (INFLATED-BOAT), BUBBLE (DAM-ROOM), KNIFE (ATTIC-TABLE, which
does not exist), LARGE-BAG (THIEF; NPC init puts it there afterwards, so OK
at runtime), FRONT-DOOR (WEST-OF-HOUSE), MOUNTAIN-RANGE (MOUNTAINS).
world_init.cpp 2519-2575 has literal "TODO: Set proper location" comments.
Wrong room: BOLT in MAINTENANCE-ROOM (ZIL DAM-ROOM 1dungeon 1075);
YELLOW/BROWN/RED/BLUE buttons in MACHINE-ROOM (ZIL MAINTENANCE-ROOM 1165-
1190; world_init 3594-3621 use RoomIds::MACHINE_ROOM); AXE in TROLL-ROOM
(ZIL in TROLL); EGG loose in UP-A-TREE (ZIL in NEST); BOTTLE and SANDWICH-BAG
on the KITCHEN floor (ZIL on KITCHEN-TABLE); THIEF starts in MAZE-2 after NPC
init (ZIL ROUND-ROOM 1dungeon 970, though I-THIEF moves him anyway); GUNK
starts in MACHINE-ROOM (ZIL: no location until the machine puzzle) and lacks
TAKEBIT. KITCHEN-WINDOW is physically in EAST-OF-HOUSE and RAINBOW in
CANYON-VIEW instead of LOCAL-GLOBALS. 20 LOCAL-GLOBALS/GLOBAL-OBJECTS objects
have no location at all (model choice: room->addGlobal lists are used
instead; see 5).

## 4. Flags
Missing (HIGH): TRYTAKEBIT on CHALICE, KNIFE, ROPE, SWORD, WATER, CYCLOPS,
THIEF, TROLL, STILETTO, AXE, LARGE-BAG (the "can't take it" and thief-steal
guards); THIEF lacks INVISIBLE, CONTBIT, OPENBIT (ZIL 1dungeon 970-979, needed
so the thief starts hidden and his loot shows); TROLL lacks OPENBIT; AXE lacks
TAKEBIT/NDESCBIT; BOOK lacks CONTBIT and TURNBIT (1dungeon 266); MAP lacks
INVISIBLE (it must appear only on winning); WATER lacks DRINKBIT; MATCH lacks
READBIT; RAINBOW lacks CLIMBBIT; INFLATED-BOAT lacks TAKEBIT/BURNBIT/
SEARCHBIT; PUNCTURED-BOAT and BOAT-LABEL lack TAKEBIT/BURNBIT; all four
buttons, BOLT, BUBBLE, MACHINE, CYCLOPS lack NDESCBIT. Rooms: DEAD-END-1..4
lack MAZEBIT (breaks the always-verbose maze rule), RESERVOIR and IN-STREAM
lack NONLANDBIT, ARAGAIN-FALLS and END-OF-RAINBOW lack ONBIT (dark!).
Extra with no ZIL basis (MED): SACREDBIT on CELLAR, GALLERY, STUDIO,
END-OF-RAINBOW (blocks thief); ONBIT on ATTIC, STUDIO, RIVER-2/3/4,
SANDY-BEACH; FIGHTBIT preset on THIEF/TROLL/CYCLOPS; TOOLBIT on AXE;
WEAPONBIT on TRIDENT; OPENBIT on TROPHY-CASE and SANDWICH-BAG (ZIL closed);
CONTBIT on BAG-OF-COINS, INFLATED-BOAT; TRYTAKEBIT on buttons, BOAT-LABEL,
PUNCTURED-BOAT, RAINBOW; BURNBIT on MAP, OWNERS-MANUAL; INVISIBLE on GROUND,
GRUE; NDESCBIT on WALL, GRANITE-WALL, GROUND.
Correct: BARROW-DOOR has OPENBIT; CHALICE has TAKEBIT (but not TRYTAKEBIT).

## 5. Exits and room globals
Wrong/extra exits (HIGH), all in world_init.cpp:
- KITCHEN NORTH -> GALLERY (no ZIL basis; l.445-455).
- RESERVOIR-SOUTH (l.1190-1194): ZIL SE->DEEP-CANYON, SW->CHASM-ROOM,
  EAST->DAM-ROOM, WEST->STREAM-VIEW. C++: SOUTH->DEEP-CANYON, SE->DAM-LOBBY,
  no EAST. The dam is unreachable from the reservoir side.
- CANYON-VIEW (l.383-388): EAST must go to CLIFF-MIDDLE; C++ blocks EAST and
  adds a NORTH refusal.
- GRATING-CLEARING (l.312-314): EAST must go to FOREST-2 (C++ CANYON-VIEW);
  extra SW->FOREST-2.
- CYCLOPS-ROOM: extra NORTH refusal and extra WEST->MAZE-15 (ZIL only NW).
- STRANGE-PASSAGE (l.2163-2168): extra NORTH/OUT->LIVING-ROOM,
  SOUTH->CYCLOPS-ROOM.
- EAST-OF-HOUSE: extra NE/SE->CLEARING.
- IN-STREAM: LAND->STREAM-VIEW missing.
- LAUNCH pseudo-exits on SANDY-BEACH, SHORE, WHITE-CLIFFS-NORTH/SOUTH and
  DOWN "over the falls" on RIVER-5, EAST refusal on RIVER-3, UP refusal on
  WHITE-CLIFFS-SOUTH: no ZIL basis (LAUNCH is a verb driven by RIVER-LAUNCH
  tables in 1actions 2693-2699, not a direction; ZIL DIRECTIONS has LAND but
  no LAUNCH).
- CELLAR WEST text: ZIL "You try to ascend the ramp, but it is impossible,
  and you slide back down." vs C++ "The ramp is too steep to climb."
- Door exits CELLAR UP (TRAP-DOOR) and GRATING-ROOM UP (GRATE) are
  conditional lambdas rather than DEXIT door references (functional if the
  lambda tests OPENBIT; note the ZIL DEXIT also prints "The X is closed."
  automatically). KITCHEN/EAST-OF-HOUSE window exits use the DOOR type.
Verified OK: the 9 self-loop exits, the 4 MAZE-DIODES PER exits (PROCEDURAL),
GRATING-EXIT / TRAP-DOOR-EXIT / UP-CHIMNEY PER exits, KITCHEN DOWN
FALSE-FLAG with "Only Santa Claus climbs down chimneys.", RIVER-1 with no
static inbound exit, TROLL-ROOM/LIVING-ROOM conditional texts, walk-around
tables (dungeon.cpp), SCORE_MAX 350, all 11 conditional-exit flags exist in
Globals.
Room GLOBAL lists: 46 rooms differ. STAIRS is missing from all 17 rooms that
list it in ZIL (ATLANTIS, CHASM-ROOM, CYCLOPS-ROOM, DEEP-CANYON, EGYPT,
EW-PASSAGE, GAS-ROOM, LADDER-TOP, LOUD-ROOM, NORTH-TEMPLE, RESERVOIR-NORTH,
SMALL-CAVE, SMELLY-ROOM, TINY-CAVE, TORCH-ROOM, TREASURE-ROOM, CELLAR);
FOREST/TREE/SONGBIRD/WHITE-HOUSE missing from FOREST-1/2/3, CLEARING,
MOUNTAINS; GLOBAL-WATER/RIVER missing from all river/shore rooms, DAM-ROOM,
DAM-BASE, CANYON-BOTTOM, CLIFF-MIDDLE, IN-STREAM, STREAM-VIEW; CRACK from
CHASM-ROOM/DAMP-CAVE; LADDER from LADDER-TOP/BOTTOM; SLIDE from CELLAR and
SLIDE-ROOM; BODIES from LAND-OF-LIVING-DEAD; BOARDED-WINDOW from NORTH/
SOUTH-OF-HOUSE; CLIMBABLE-CLIFF from CANYON-BOTTOM/CLIFF-MIDDLE. Extra:
CYCLOPS, TROLL, CHALICE, GRATE (MAZE-11), PEDESTAL/RAILING (TORCH-ROOM),
RAILING (DOME-ROOM) added as room globals (they are real objects in ZIL).
PSEUDO clauses (16 rooms) are not represented in room data at all; verify in
the pseudo-object logic slice.

## 6. Object action / DESCFCN wiring (data side)
ZIL ACTION present but C++ object has no handler: AXE-F, BARROW-FCN,
BODY-FUNCTION, SKELETON (BONES), BLACK-BOOK, CHALICE-FCN, CLIFF-OBJECT,
GARLIC-F, KNIFE-F, LARGE-BAG-F, LEAF-PILE, DUMB-CONTAINER (PEDESTAL),
PUTTY-FCN, RAINBOW-FCN, RIVER-FUNCTION, RUSTY-KNIFE-FCN, SAND-FUNCTION,
SCEPTRE-FUNCTION, STILETTO-FUNCTION, TOOL-CHEST-FCN, TRUNK-F, TUBE-FUNCTION,
WATER-F, WCLIF-OBJECT, FRONT-DOOR-FCN on WOODEN-DOOR (25 objects). Rooms
WHITE-CLIFFS-NORTH/SOUTH lack WHITE-CLIFFS-FUNCTION. Some may be handled
via verb code instead; the logic slice should confirm.

## 7. Text
Model gap: ZObject has one longDesc_ and no FDESC slot. 22 objects lose their
FDESC entirely (EGG, NEST, KNIFE, SWORD, ROPE, TORCH, GUIDE, MAP, BOOK,
CANDLES, TRIDENT, POT-OF-GOLD, BUOY, RUSTY-KNIFE, OWNERS-MANUAL, CANARY,
BROKEN-CANARY, BURNED-OUT-LANTERN ...) and 4 that have both keep only LDESC
(LAMP, PAINTING, SCEPTRE, TRUNK). Several store the FDESC in the TEXT slot
instead (EGG, CANARY, BUOY, RUSTY-KNIFE, PAINTING), which would make them
"readable". LDESC missing for INFLATABLE-BOAT, LOWERED/RAISED-BASKET,
BAG-OF-COINS.
Wrong wording: GALLERY "paintings which were here have been stolen" (ZIL:
"paintings have been stolen"); STRANGE-PASSAGE "large hole" (ZIL "large
opening"); MAZE-5 drops the skeleton sentence; CANYON-VIEW drops "It is
possible to climb down into the canyon from here."; TROLL-ROOM DESC "Troll
Room" (ZIL "The Troll Room"); DESC changes: ADVENTURER "adventurer" (ZIL
"cretin"), CHALICE "silver chalice" (ZIL "chalice"), TORCH "ivory torch"
("torch"), MAP "map" ("ancient map"), FRONT-DOOR "front door" ("door"), RUG
"rug" ("carpet"), GRUE "grue" ("lurking grue"), SCARAB "jewel-encrusted
scarab" ("beautiful jeweled scarab"), BUBBLE "bubble" ("green bubble"), GUNK
"vitreous slag" ("small piece of vitreous slag"), BOAT-LABEL "label" ("tan
label"), OWNERS-MANUAL ("ZORK owner's manual"), BAG-OF-COINS ("leather bag of
coins"). These change every "Taken."/inventory line.
TEXT fabricated or wrong: BOAT-LABEL (C++ invents "Model: FMB-1 / Serial
Number: 123456789"; ZIL is the Frobozz instructions/warranty), OWNERS-MANUAL
(C++ invents a boat warranty; ZIL is the ZORK I congratulation), MAP (C++ "a
complex maze of twisty passages"; ZIL describes the three clearings and "To
Stone Barrow"), MATCH text missing entirely, GUIDE truncated (drops the
"1) You start your tour here in the Dam Lobby..." paragraph and the leading
tab), BOOK uses 'Hello sailor' single quotes and one space (ZIL double quotes,
two spaces), ADVERTISEMENT inserts hard line breaks (ZIL prints one
paragraph). 23 rooms whose ZIL description is produced by the room action
(WEST-OF-HOUSE, KITCHEN, LIVING-ROOM, CELLAR, EAST-OF-HOUSE, ...) carry a
static C++ LDESC; harmless only if the M-LOOK handler always wins.
Whitespace/newline conventions: ZIL "|" = newline and source newlines =
spaces; C++ strings mostly follow this except the cases above.

## 8. Vocabulary
6-char truncations: WHITE-HOUSE keeps BEAUTI/COLONI alongside full words;
PAINTING/BAUBLE/SCARAB use BEAUTIFUL only; INFLAT boats use INFLATABLE /
INFLATED; DAM lacks FCD#3 (and GATE/GATES); TEETH has OVERBOARD (OK).
Missing ZIL words: MIRROR-1/2 REFLECTION and ENORMOUS; BOOK PRAYER/PAGE/
BOOKS; GUIDE BOOKS/GUIDEBOOKS; LUNCH FOOD/DINNER; WATER LIQUID/H2O; MAP
PARCHMENT; BOAT-LABEL FINEPRINT/PRINT; MACHINE PDP10/DRYER/LID; buttons
SWITCH; AXE AX; CYCLOPS EYE; THIEF MAN/PERSON; SCARAB BUG; TORCH IVORY;
CHALICE SILVER; BRACELET JEWEL; TRUNK CHEST; SCREWDRIVER/SHOVEL/WRENCH/PUMP
TOOLS; adjectives NASTY (troll), HUNGRY/GIANT (cyclops), VICIOUS (stiletto),
UNRUSTY (knife), SMALL (leaflet, bubble, gunk, manual), TAN/FINE (label),
ANTIQUE/OLD/ANCIENT (map), FLAMING (torch), OWN (trident), SHARP/ANCIENT/
ENAMELED (sceptre), LARGE (book), BOARDED (front door), THIEFS (bag). Many
invented words added (GOBLET, GRAIL, GEM, TOME, CHART, BURGLAR, RAFT, ...).
ADVENTURER carries ME/MYSELF/SELF (ZIL puts those on ME).

## Severity ranking
1. HIGH: 6 ZIL objects missing, 3 phantom objects (10-point fake TROPHY);
   14 objects nowhere at start (torch, book, map, guide, manual, tool chest,
   altar, pedestal, bubble, coins); buttons/bolt in wrong rooms; reservoir/
   canyon/clearing/kitchen exit errors; SIZE default 0 vs 5 and 23 wrong
   sizes; STRENGTH/CAPACITY overloads; room VALUEs 0; missing TRYTAKEBIT/
   INVISIBLE/CONTBIT/NDESCBIT/MAZEBIT/NONLANDBIT/ONBIT flags.
2. MED: 46 room GLOBAL lists (STAIRS, water, forest), fabricated TEXT for
   label/manual/map, DESC renames, missing FDESC model, vocabulary gaps.
3. LOW: extra room actions, static LDESC on action-described rooms,
   whitespace differences, dead duplicate initializer files.
Uncertain: whether verb code compensates for missing object ACTION handlers
(logic slice); whether nullptr locations for LOCAL-GLOBALS objects are
handled by scope code; whether NPC init's thief placement is intended.
