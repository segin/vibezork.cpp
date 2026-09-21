# Read-only audit: `zil/` (reference ZIL source for the C++ port)

Date: 2026-09-19. Scope: `zil/` only. Nothing was modified.

## 1. Provenance and integrity

- `zil/` is a byte-identical copy of the "Final Revision" snapshot of the
  `historicalsource/zork1` GitHub repository. The repo's first three commits
  (34cc828 "Revision 88 (Original Source)", 87a3b78 "Final Revision",
  5e9a9ce "Extensions on Z-Machine Fixed.") are authored by
  `historicalsource <historicalsoftware@textfiles.com>`; commit 280fffd later
  moved the files into `zil/` with no content change. Every `.zil` file at
  HEAD is identical to its 87a3b78 counterpart (verified with `git diff`).
- The first import (34cc828) was a DIFFERENT game version: Release 88, serial
  840726 (its `zork1.zip` is Z3 R88/840726). 87a3b78 replaced it with the
  Release 119 / serial 880429 source. Anyone consulting `git log -p` on these
  files will see R88 -> R119 Infocom changes (bag CAPACITY 15->9, SIZE 3->9;
  garlic SIZE 4 added; BARROW-DOOR gained OPENBIT; leaflet/guide/boat-label
  text reflowed; "non-descript" -> "nondescript"; FOREST added to house-room
  GLOBAL lists; MAIN-LOOP split into MAIN-LOOP/MAIN-LOOP-1; DEMON routine and
  MOVES wraparound at 999 removed from gclock; AGAIN/G/OOPS moved from SYNTAX
  to BUZZ; `IS x IN y`, `HAND`, `DROP RELEASE` syntaxes removed; RMUNGBIT
  "no-object" idiom introduced for CLIMB UP/DOWN, TAKE OUT, TURN x WITH).
  These are Infocom's own revisions, not local edits.
- `zil/zork1.zip` and `zil/COMPILED/zork1.z3` are byte-identical
  (md5 1d4606016ea58ee038da53d994392323), Z-machine v3, Release 119,
  serial 880429. This matches `zork1.chart` (rel 119, 86838 bytes, 250 obj,
  155 globals, 684 words) and `zork1.errors` (assembled 29 Apr 1988).
- `zork1.record` is the ZILCH compiler log for that build and pins the exact
  source versions used: ZORK1.ZIL.29, GMACROS.ZIL.8, GSYNTAX.ZIL.153,
  1DUNGEON.ZIL.122, GGLOBALS.ZIL.64, GCLOCK.ZIL.6, GMAIN.ZIL.42,
  GPARSER.ZIL.45, GVERBS.ZIL.429, 1ACTIONS.ZIL.257. The `.zil` files carry no
  version stamps, so equality with these versions is assumed, not provable.
- All text files are 7-bit ASCII with LF endings and no CRs.
- Form-feed handling is inconsistent across files: 1actions (30), 1dungeon
  (15), gparser (11) and gmain (2) contain real 0x0C page breaks, while
  gsyntax (lines 5, 36, 79) and gverbs (5, 149, 1620) contain the literal two
  characters `^L`, and gglobals line 88 contains the literal `/^L`. This is a
  transcription artifact of the historical dump, harmless to a port.
- `file(1)` mislabels gparser.zil and parser.cmp as "HTML document" (see the
  parser section); content is plain ZIL.

## 2. Files that are NOT game source

| File | What it is | Needed by port? |
|---|---|---|
| zork1.zil | Top-level build file: INSERT-FILE order, PROPDEFs (SIZE 5, CAPACITY 0, VALUE 0, TVALUE 0) | Yes: property defaults |
| zork1.record | Compiler log; lists routine compile order, 250 objects, 148 "real" globals, 30 properties, 32 flags | Reference only |
| zork1.errors | Assembler log; unused symbols list | Reference only |
| zork1.chart | One-line build stats | No |
| zork1.serial | Contains "1" (serial counter for the build tool) | No |
| zork1freq.xzap | ZAP abbreviation table (`.FSTR` strings with frequency counts) | No |
| parser.cmp | See parser section | No |
| zork1.zip, COMPILED/zork1.z3 | The shipped R119 story file (identical) | Yes: oracle for behavioural testing |

`zork1.errors` unused-symbol list: UNTIE-FROM, BREATHE, CYCLOPS-MELEE,
TROLL-MELEE, P-DIRECTION, DEF2A, DEF3C, THIEF-MELEE.

## 3. Shared "trilogy" files (gmacros, gclock, gmain, gglobals, gsyntax)

These are generic Zork I-III files and contain `ZORK-NUMBER` conditionals
(gglobals 6, gsyntax 5, gmain 1, gparser 1, gverbs 63). Only the ZORK-NUMBER=1
branch is live for this port; Zork II/III branches (VIKING-SHIP, INVIS,
I-CLEFT, FROTZ/OZMOO buzzwords, Zork II bare ATTACK/KILL syntaxes) are dead.

### gmacros.zil (154 lines)
- `TELL` macro maps `D`/`A`/`N`/`C` indicators to PRINTD/PRINTA/PRINTN/PRINTC.
  Note `A`/`AN` -> PRINTA, but the PRINTA routine was deleted in R119 (present
  in R88); no R119 code uses `TELL A ...`, so it is harmless.
- `PROB` (l.115): with one arg it is `base > RANDOM(100)`; with a second arg it
  calls ZPROB, which uses RANDOM(300) when LUCKY is false. LUCKY is defined
  twice: `<GLOBAL LUCKY 1>` gverbs.zil:1886 and `<GLOBAL LUCKY T>`
  1actions.zil:969 (REDEFINE T makes the later win). Both are truthy.
- `PICK-ONE` (l.127) is a shuffle-without-replacement over a table whose
  element 0 is the length and element 1 the count of already-used entries;
  ports must reproduce the in-place swap to match message ordering.
- `C-ENABLED?` is SETG'd to 0 here (l.5) and re-declared as a CONSTANT in
  gclock.zil:15; harmless duplicate.
- `ABS`, `FLAMING?`, `OPENABLE?` (DOORBIT or CONTBIT) macros.

### gclock.zil (60 lines)
- Interrupt table: 180 words, 6 bytes per entry (ENABLED?, TICK, RTN), so at
  most 30 distinct interrupts. Allocation grows downward from 180.
- CLOCKER semantics: entry fires when TICK is 1 (or negative) before the
  decrement; negative tick = every turn. MOVES increments once per CLOCKER
  pass, i.e. only for parsed, non-meta commands (gmain.zil:169-172 skips
  CLOCKER for TELL BRIEF SUPER-BRIEF VERBOSE SAVE VERSION QUIT RESTART SCORE
  SCRIPT UNSCRIPT RESTORE). CLOCK-WAIT skips exactly one CLOCKER call.
- `C-DEMONS` and the `DEMON` optional arg of INT are vestigial in R119: no
  call passes DEMON=T (the DEMON routine was removed between R88 and R119),
  and CLOCKER is never invoked with P-WON false, so the C-DEMONS branch at
  l.46 is dead. Comments in 1actions ("moved to DEMONS", l.1756, 1884, 1913,
  1935, 1937, 3329, 3806) refer to a section heading, not to this mechanism.
- The interrupts queued at startup (1dungeon.zil GO, l.2638-2642):
  I-FIGHT (-1, enabled), I-SWORD (-1, NOT enabled), I-THIEF (-1, enabled),
  I-CANDLES 40 (not enabled), I-LANTERN 200 (not enabled).

### gmain.zil (313 lines)
- `MAIN-LOOP-1` is the per-command driver. Key behaviours a port must copy:
  - IT substitution in P-PRSI first, then P-PRSO (l.45-64).
  - WALK with P-WALK-DIR set bypasses object handling (l.79-81).
  - Zero objects but syntax wants one: "It's too dark to see." if not LIT,
    else "It's not clear what you're referring to." (l.82-90).
  - Multi-object loop with the "multiple exceptions" (l.120-145): NOT-HERE
    objects counted into P-NOT-HERE; TAKE ALL skips objects not in the
    named container, not in WINNER/HERE/vehicle/surface, or lacking
    TAKEBIT/TRYTAKEBIT; each processed object is prefixed "name: ".
  - After the loop: "The [other] object[s] that you mentioned is/aren't
    here." or, if nothing at all was performed, "There's nothing here you can
    take." (l.98-112). The latter string is emitted for ANY verb, not just
    TAKE (original quirk).
  - Room ACTION called with M-END after every non-fatal command (l.154).
  - The block that saved L-PRSA/L-PRSO/L-PRSI is commented out (l.155-160);
    AGAIN is handled inside the parser instead.
- `PERFORM` exists in two variants selected at compile time by `PREDGEN`
  (l.180-312). zork1.record shows only PERFORM compiled (no D-APPLY/DD-APPLY
  routines), so the FIRST variant (l.182-228) is the shipped one. Dispatch
  order: WINNER action -> room action (M-BEG) -> PREACTIONS[verb] -> PRSI
  action -> CONTFCN of PRSO's container -> PRSO action -> ACTIONS[verb].
  IT that is not ACCESSIBLE? yields "I don't see what you are referring to."
  and RFATAL. P-IT-OBJECT is set to PRSO unless PRSI is IT or verb is WALK
  (the second variant lacks the PRSI check; ignore it).
- `<CONSTANT SERIAL 0>` (l.5) is never referenced.
- Comment `;"GO now lives in SPECIAL.ZIL"` (l.31) is stale; GO is at the end
  of 1dungeon.zil (l.2637).

### gglobals.zil (308 lines)
- `GLOBAL-OBJECTS` FLAGS list (l.8-10) and the bizarre `LOCAL-GLOBALS`
  property block (l.12-24: PSEUDO "FOOBAR" V-WALK, FDESC "F", VTYPE 1 ...) are
  compiler hacks that force flag and property numbers to exist; they are not
  game data. `<OBJECT ROOMS (IN TO ROOMS)>` (l.28) similarly forces P?IN to be
  a direction property. A port must not treat these as real objects.
- Global objects defined here: INTNUM, PSEUDO-OBJECT, IT, NOT-HERE-OBJECT,
  BLESSINGS, STAIRS, SAILOR, GROUND, GRUE, LUNGS, ME, ADVENTURER, PATHOBJ,
  ZORKMID, HANDS.
- `NOT-HERE-OBJECT-F` (l.52) prints "You can't see any <words> here!" using
  the raw input words (BUFFER-PRINT) or P-XADJN/P-XNAM when P-OFLAG is set.
- `GROUND-FUNCTION` delegates to SAND-FUNCTION in SANDY-CAVE (l.175-177) and
  turns PUT x ON/IN GROUND into DROP.
- `CRETIN-FCN` (ME): ATTACK ME WITH weapon -> JIGS-UP "If you insist....
  Poof, you're dead!"; EXAMINE ME in a mirror room -> "Your image in the
  mirror looks tired."; GIVE x TO ME -> TAKE x.
- `LOAD-MAX` and `LOAD-ALLOWED` both 100 (l.92-94).
- ADVENTURER has `(ACTION 0)`, STRENGTH 0, no location (l.267-272).

### gsyntax.zil (561 lines)
- BUZZ words: AGAIN G OOPS; A AN THE IS AND OF THEN ALL ONE BUT EXCEPT . , "
  YES NO Y HERE (l.9-11).
- Direction synonyms encode the 6-character dictionary limit: NE/NORTHE,
  SE/SOUTHE (l.32, 34); likewise the verb `INFLAT` (l.246).
- Debug/meta syntaxes present in the shipped build: $VERIFY, #RANDOM,
  #COMMAND, #RECORD, #UNRECORD (l.69-77).
- `(FIND RMUNGBIT)` is used as a "no object expected, GWIM will fail" idiom
  for CLIMB UP/DOWN, LOOK AROUND/UP/DOWN, STAND UP, TAKE UP, TAKE OUT, TURN x
  WITH (l.130-132, 298-300, 451, 471, 473, 506).
- Synonym surprises that a port must keep: SHUT is a synonym of TURN, not
  CLOSE (l.514), so "SHUT OFF LAMP" works but "SHUT DOOR" goes to V-TURN;
  SEE is a synonym of FIND (l.221); SIT of CLIMB (l.138); TEMPLE of TREASURE
  (l.503); STARE/GAZE of LOOK (l.310); FEED/OFFER/DONATE/HAND of GIVE
  (l.233-235); REMOVE/CATCH of TAKE (l.480); TAUNT of KICK (l.258).
- Offensive vocabulary in the original: CURSE synonyms SHIT FUCK DAMN
  (l.154); RAPE/MOLEST verb -> V-RAPE (l.398-399).
- POUR x IN y and POUR x FROM y map to V-DROP, not V-PUT (l.361, 363).
- LOOK AT x WITH y -> V-READ (l.307-308); EXAMINE IN/ON x -> V-LOOK-INSIDE.

## 4. gparser.zil (1407 lines) and parser.cmp (2111 lines)

### Why `file` says "HTML document"
libmagic's HTML heuristic matches a `<TABLE` token inside its start-of-file
scan window: gparser.zil has `<TABLE 0 0 0 0>` at byte 415 (l.22) and
parser.cmp at byte 1608. 1actions/gverbs also contain `<TABLE` but far past
the window. Both files are plain 7-bit ASCII. Harmless.

### What parser.cmp is
A TOPS-20 SRCCOM difference listing (header l.2:
`;COMPARISON OF SS:<ZORK1>GPARSER.ZIL.37 AND SS:<Z6>PARSER.ZIL.273`), 82
hunks. "Z6" is Spellbreaker (internal Zork VI): the right-hand side has
CUBE-NAME?/CUBE-LIST, TIME-CUBE, MOBY-FIND, GENERIC-OBJECT?. It is not ZIL,
is not in zork1.zil's INSERT-FILE list, and contributes nothing to the build.
The ZORK1 side (GPARSER.ZIL.37) is OLDER than the repo's gparser.zil (which
the record says is .45): .37 uses P-CCSRC/P-CSPTR/P-CEPTR and CLAUSE-PRINT,
absent from gparser.zil, whereas gparser.zil's P-CCTBL and RESERVE-PTR
("AGAIN, x") mechanics match the Z6 side. So R119 back-ported Spellbreaker-era
parser changes; parser.cmp is archival only and should not be used as a spec.

### gparser.zil inventory
Globals/tables: PRSA/PRSI/PRSO (14-16), P-TABLE/P-ONEOBJ/P-SYNTAX (18-20),
P-CCTBL (22), P-LEN/P-DIR/HERE/WINNER (29-32), P-LEXV/AGAIN-LEXV/RESERVE-LEXV
(59 four-byte entries, 34-40), P-INBUF/OOPS-INBUF (120 bytes, 44-54), P-CONT,
P-IT-OBJECT, P-OFLAG, P-MERGED, P-ACLAUSE, P-ANAM, P-AADJ (58-68), LEXV
layout constants (73-80; the trailing comments on 73-79 are off by one line),
P-ITBL/P-OTBL (10 words) and slot constants (82-99), QUOTE-FLAG,
P-END-ON-PREP (101-102), P-ACT/P-WALK-DIR/AGAIN-DIR (382-384), P-NUMBER (536),
P-DIRECTION (538), syntax-record layout (693-705), P-GWIMBIT (899), match
tables P-PRSO/P-PRSI/P-BUTS/P-MERGE (50 words) and P-GETFLAGS/P-ALL/P-ONE/
P-INHIBIT (960-976), location bits SH SC SIR SOG STAKE SMANY SHAVE
(1032-1038), P-XNAM/P-XADJ/P-XADJN (1142-1144), P-SRCBOT/TOP/ALL (1212-1214),
ALWAYS-LIT (1331).

Routines: PARSER 109-380; STUFF 387; INBUF-STUFF 402; INBUF-ADD 410; WT? 430;
CLAUSE 440-510; NUMBER? 512-534; ORPHAN-MERGE 543-632; ACLAUSE-WIN 634;
NCLAUSE-WIN 645; WORD-PRINT 658; UNKNOWN-WORD 665; CANT-USE 677;
SYNTAX-CHECK 707-775; CANT-ORPHAN 777; ORPHAN 782; THING-PRINT 810;
BUFFER-PRINT 819; PREP-PRINT 851; CLAUSE-COPY 860; CLAUSE-ADD 882;
PREP-FIND 888; SYNTAX-FOUND 895; GWIM 901; SNARF-OBJECTS 928; BUT-MERGE 945;
SNARFEM 978-1030; GET-OBJECT 1040-1140; WHICH-PRINT 1146; GLOBAL-CHECK 1169;
DO-SL 1202; SEARCH-LIST 1216; OBJ-FOUND 1239; TAKE-CHECK 1244; ITAKE-CHECK
1248; MANY-CHECK 1294; ZMEMQ 1315; ZMEMQB 1324; LIT? 1333; THIS-IT? 1357;
ACCESSIBLE? 1372; META-LOC 1398. Every routine is referenced; none dead.

Pipeline: PARSER copies the previous P-ITBL to P-OTBL, resets WINNER/HERE/LIT,
chooses input (RESERVE-LEXV leftover from "AGAIN, x" / P-CONT continuation
after THEN or "." / fresh READ), handles OOPS (177-206) and AGAIN/G
(211-250), lexes (217-368) with directions short-circuiting to V?WALK
(370-375), then ORPHAN-MERGE (377) -> SYNTAX-CHECK (GWIM fill-in or orphan
question) -> SNARF-OBJECTS -> SNARFEM -> GET-OBJECT (DO-SL/SEARCH-LIST/
GLOBAL-CHECK, WHICH-PRINT disambiguation) -> MANY-CHECK -> TAKE-CHECK/
ITAKE-CHECK (implicit "(Taken)").

### Dead code and quirks in gparser.zil
- Dead: P-DIRECTION (538) only inside the ZORK-NUMBER 3 branch (1055-1064),
  matching zork1.errors. P-DIR (30), P-ACT (382), P-AADJ (68), P-ADVERB (962)
  are written but never read. LAST-PSEUDO-LOC is commented out (60,
  1376-1380), so ACCESSIBLE? does not location-check pseudo objects. Unused
  AUX locals: BTS (1202), FLS (1228), BUTLEN (945).
- STUFF copies at most 29 LEXV entries (387) although tables hold 59; AGAIN
  of a longer input is truncated.
- Messages a port must reproduce verbatim: "I beg your pardon?" (176);
  `I don't know the word "X".` (665-675) but "Nothing happens." when the verb
  is SAY; `You used the word "X" in a way that I don't understand.` (677-687);
  OOPS family: "I can't help your clumsiness.", "Warning: only the first word
  after OOPS is used.", "There was no word to replace!" (177-206); AGAIN
  family: "Beg pardon?", "It's difficult to repeat fragments.", "That would
  just repeat a mistake." (211-250); "There were too many nouns in that
  sentence." (332); "Please consult your manual for the correct way to talk
  to other people or creatures." (352-355); "There was no verb in that
  sentence!" (710); "That sentence isn't one I recognize." (737); "That
  question can't be answered." for FIND (759); orphan prompt "What do you
  want to <verb> [obj] <prep>?" (764-775); GWIM echoes "(the X)", "(prep the
  X)", "(out of the X)", "(with your hands)" (901-926); "There seems to be a
  noun missing in that sentence!" (1069, 1121); "(How about the X?)" for a
  random P-ONE pick (1078-1082); "It's too dark to see!" (1129); "Which X do
  you mean, the A, the B or the C?" with the comma before "or" only for more
  than two candidates (1146-1167); `You can't use multiple [in]direct objects
  with "verb".` (1300-1310); "(Taken)", "You don't have the X.", "You don't
  have that!", "I don't see what you're referring to." (1248-1292).
- Direction parsing (280-306): bare dir, WALK dir, "dir." / "dir then",
  "dir, ..." (comma/AND rewritten to THEN) bypass syntax checking. "TO" after
  TELL becomes a quote (255-257); a leading THEN with no verb becomes "the"
  or synthesises TELL+quote (261-266).
- NUMBER? (512-534): digits and hh:mm; rejects running sum >10000, final
  >1000, hour >23; hour <8 gets +12; writes W?INTNUM into P-LEXV even on the
  path that returns false (530 before 531).
- GET-OBJECT search order (1060-1066) temporarily clears TRANSBIT on PLAYER
  while scanning the room; two-pass search (syntax bits then everything);
  NOT-HERE-OBJECT substituted when lit or verb is TELL (1122-1128).
- GLOBAL-CHECK pseudo objects (1181-1194) self-modify PSEUDO-OBJECT's ACTION
  property and the two words preceding it; ROOMS is searched only for
  LOOK-INSIDE/SEARCH/EXAMINE (1198-1200). A port needs an explicit model.
- LIT? (1333-1355) reuses the object search with P-GWIMBIT=ONBIT. THIS-IT?
  (1357) matches synonyms by word and adjectives by byte (ZMEMQB) because v3
  adjective ids are bytes.
- Six-character truncation is not in this file; it is Z-machine v3 dictionary
  behaviour. WORD-PRINT echoes the full typed word from P-INBUF.
- Cross-game comment `;"ADDED 4/27 FOR TURTLE,UP"` (467) is Enchanter-era.

## 5. gverbs.zil (2216 lines)

### Inventory
189 routines: 147 V-*, 12 PRE-*, 30 helpers. 63 ZORK-NUMBER conditionals
(25 Z1, 21 Z2, 17 Z3); 34 NULL-F stubs. Sections: Game Commands 1-147
(V-VERBOSE/BRIEF/SUPER-BRIEF 13-25, V-INVENTORY 29, FINISH 33-53, V-QUIT 55,
V-RESTART 63, V-RESTORE 71, V-SAVE 78, V-SCRIPT/UNSCRIPT 86-96, V-VERSION
98-121, V-VERIFY 123, V-COMMAND-FILE 130, V-RANDOM 134, V-RECORD/UNRECORD
141-147); Real Verb Functions 151-1618 alphabetical with PRE-BOARD 201,
PRE-BURN 243, PRE-DROP 474, PRE-FILL 646, PRE-GIVE 708, PRE-MOVE 910,
PRE-MUNG 923, PRE-PUT 1075, PRE-READ 1137, PRE-SGIVE 1206, PRE-TAKE 1353,
PRE-TURN 1488 and helpers TELL-NO-PRSI 240, HIT-SPOT 518, REMOVE-CAREFULLY
610, SHAKE-LOOP 1242; Descriptions 1622-1840 (V-FIRST-LOOK 1630,
DESCRIBE-ROOM 1635, DESCRIBE-OBJECTS 1681, DESCRIBE-OBJECT 1693,
PRINT-CONTENTS 1730, PRINT-CONT 1750, FIRSTER 1818, SEE-INSIDE? 1837);
Scoring 1841-1877 (SCORE-UPD 1851, SCORE-OBJ 1867, YES? 1872); Death globals
1879-1892; Object Manipulation 1894-1999 (ITAKE 1900, IDROP 1966, CCOUNT
1979, WEIGHT 1988); Miscellaneous 2000-2216 (exit constants 2001-2013,
INDENTS 2015, HACK-HACK 2024, NO-GO-TELL 2038, GOTO 2045-2137, LKP 2139,
DO-WALK 2148, GLOBAL-IN? 2152, FIND-IN 2156, HELD? 2167, OTHER-SIDE 2173,
MUNG-ROOM 2183, THIS-IS-IT 2191, string tables).

Globals here: VERBOSE, SUPER-BRIEF, HS, JUMPLOSS, WHEEEEE, LIT, SPRAYED?,
DESC-OBJECT, MOVES, SCORE, BASE-SCORE, WON-FLAG, DEAD, DEATHS, LUCKY,
FUMBLE-NUMBER 7, FUMBLE-PROB 8, INDENTS, HO-HUM, SWIMYUKS, HELLOS, YUKS,
DUMMY. Exit-type constants REXIT 0 UEXIT 1 NEXIT 2 FEXIT 3 CEXIT 4 DEXIT 5.

Not here although a port might look here: LIT? (gparser 1333), V-SCORE
(1actions 4026; ranks 350 Master Adventurer, >330 Wizard, >300 Master, >200
Adventurer, >100 Junior Adventurer, >50 Novice Adventurer, >25 Amateur
Adventurer, else Beginner), V-DIAGNOSE (1actions 3993), JIGS-UP (1actions
4046), RANDOMIZE-OBJECTS (4101), KILL-INTERRUPTS (4125).

### Cross-check with gsyntax.zil
All 148 distinct routines named in SYNTAX lines exist (146 here, V-SCORE and
V-DIAGNOSE in 1actions). All 12 PRE-* routines are referenced. No V-* in
gverbs lacks a SYNTAX. BLOW IN x -> V-BREATHE (230) performs INFLATE with
LUNGS; there is no BREATHE verb, so `<VERB? BREATHE>` at 1actions 3227 is
dead and routine BREATHE (1actions 2817) is unreachable, matching
zork1.errors.

### Dead code
DESC-OBJECT (1691) written, never read. Unused AUX: SCOR in V-QUIT (55), AV
in V-BOARD (224). V-LEAP (829-831) hardcodes 2 and 4 instead of NEXIT/CEXIT.
SWIMYUKS (2194) used only from 1actions 160. Commented-out forms 569, 1669,
1783. AUX variable literally named SHIT in PRINT-CONT (1751).

### Behaviours to replicate exactly
- ITAKE (1900-1964): DEAD -> TAKEBIT -> closed-container kludge (silent
  RFALSE, used by gparser 1273) -> load check (returns RFATAL=2, message
  gains ", especially in light of your condition." when LOAD-ALLOWED <
  LOAD-MAX) -> fumble (only VERB? TAKE, CCOUNT(WINNER) > 7 excluding worn
  items, probability CNT*8 percent) -> move. Weight check skipped when the
  object is already inside something held. V-TAKE (1383) tests `EQUAL? ITAKE
  T`, so a fatal result suppresses "Taken.".
- V-PUT (1085-1114) tests `NOT ITAKE`; RFATAL (2) is truthy, so an
  over-weight object prints the load message and is still moved. Marked
  uncertain whether reachable, since the HAVE syntax bit usually triggers
  ITAKE-CHECK first. Capacity rule: WEIGHT(PRSI)+WEIGHT(PRSO)-SIZE(PRSI) >
  CAPACITY -> "There's no room."; WEIGHT is recursive and worn items count 1.
- PRE-PUT (1075) just calls PRE-GIVE, so PUT of an unheld object says
  "That's easy for you to say since you don't even have the X."
- V-OPEN (966-994): a container holding exactly one untouched FDESC object
  prints "The X opens." plus the FDESC instead of "Opening the X reveals".
  V-CLOSE (336) only works when CAPACITY != 0 and not SURFACEBIT and
  re-evaluates LIT ("It is now pitch black.").
- Articles: fixed "a " and "The " everywhere (1706, 1713, 1739); "a X" for
  vowel-initial names is canonical.
- DESCRIBE-ROOM (1635): MAZEBIT rooms get TOUCHBIT cleared every time
  (1653-1656) so maze rooms are always described in full; ", in the
  <vehicle>" suffix (1661-1663); M-LOOK true suppresses LDESC, else M-FLASH
  in brief mode. Dark: "Only bats can see in the dark. And you're not one."
  (1688).
- PRINT-CONT (1750-1816): FDESC pass first, then generic; skips player and
  current vehicle, then lists the vehicle at LEVEL+1; INDENTS depth 5;
  returns false when nothing printed (relied on by V-LOOK-INSIDE 877).
  FIRSTER (1818): "Your collection of treasures consists of:" for
  TROPHY-CASE; "Sitting on the X is: " / "The X is holding: " keep a trailing
  space before CR.
- SCORE-UPD (1851): hitting exactly 350 with WON-FLAG clear sets WON-FLAG,
  un-hides MAP, clears WEST-OF-HOUSE TOUCHBIT, prints the whisper.
  SCORE-OBJ zeroes P?VALUE after awarding; GOTO calls SCORE-OBJ on rooms
  (2118).
- YES? (1872): prints ">" and accepts only YES or Y as first word.
- GOTO (2045): vehicle check via P?VTYPE room flag (2050-2062); RMUNGBIT
  rooms print LDESC and refuse; dark-to-dark has 80 percent grue death, or
  "sinister gurgling" if SPRAYED?; re-entering ENTRANCE-TO-HADES from itself
  suppresses double description (2129-2133); "The X comes to a rest on the
  shore." on landing.
- V-WALK (1521): non-exit in the dark, 80 percent grue death for ADVENTURER
  in land rooms; else "You can't go that way."; failures RFATAL, which clears
  P-CONT (gmain 161).
- V-WAIT (1514): "Time passes..." then up to 3 CLOCKER ticks, stopping early
  when an event returns true; sets CLOCK-WAIT.
- REMOVE-CAREFULLY (610): clears P-IT-OBJECT when it was the object; "You
  are left in the dark..." on light loss.
- V-TELL (1389) with P-CONT switches WINNER to the actor and HERE to its
  location; failure RFATAL. V-THROW ME (1448) is a death.
- V-VERSION prints copyright 1981-1986 and Release/Serial read from the
  story header (bytes 18-23), not from source.
- Random tables cycle via PICK-ONE: JUMPLOSS 3, WHEEEEE 4, HO-HUM 3, HELLOS
  4, YUKS 4 (one entry commented out), DUMMY 3, SWIMYUKS 1.
- Text oddities: "smanship" suffix (396) yields "knifesmanship"; "Ahoy -- X
  overboard!" double hyphen (1003); "occurences" in a comment (722); `D,
  PRSO` spacing (1222, 1238).

### File-level
No trailing newline at EOF (l.2216). 8 lines with trailing whitespace (57,
526, 530, 710, 952, 1425, 2154, 2214). Literal `^L` text at 5, 149, 1620.

## 6. 1dungeon.zil (2660 lines)

### Inventory
110 ROOMs, 122 OBJECTs, 4 ROUTINEs (TREASURE-INSIDE 745, GRATING-EXIT 1300,
CANYON-VIEW-F 2466, GO 2637), 17 GLOBALs. DIRECTIONS (13 incl. LAND) l.5,
SCORE-MAX 350 l.7, FALSE-FLAG l.9. Rooms by SUBTITLE: outside 13, house 3,
cellar 5, maze 20 (MAZE-1..15, DEAD-END-1..4, GRATING-ROOM), cyclops 3,
reservoir 5, mirror 9, round room 7, Hades 2, dome/temple/Egypt 6, dam 3,
river 17, coal mine 17. Conditional-exit flags 1267-1277; walk-around tables
HOUSE-AROUND, FOREST-AROUND, IN-HOUSE-AROUND, ABOVE-GROUND 2620-2634. Room
VALUEs: KITCHEN 10, CELLAR 25, EW-PASSAGE 5, TREASURE-ROOM 25.

Treasures (VALUE/TVALUE): SKULL 10/10, SCEPTRE 4/6, CHALICE 10/5, TRIDENT
4/11, COFFIN 10/15, DIAMOND 10/10, JADE 5/5, BAG-OF-COINS 10/5, EMERALD 5/10,
PAINTING 4/6, BAR 10/5, POT-OF-GOLD 10/10, BRACELET 5/5, SCARAB 5/5, TORCH
14/6, TRUNK 15/5, EGG 5/5, BAUBLE 1/1, CANARY 6/4; BROKEN-EGG -/2,
BROKEN-CANARY -/1; SWORD has a redundant (TVALUE 0) l.923.

Score reconciliation: object VALUE 143 + attainable TVALUE 129 (132 minus
the mutually exclusive broken egg 2 and broken canary 1) + room VALUE 65 +
LIGHT-SHAFT 13 (1actions 2569/2578) = 350 exactly.

NPCs: GHOSTS, BAT, CYCLOPS (STRENGTH 10000), THIEF (STRENGTH 5, INVISIBLE),
TROLL (STRENGTH 2). Objects with no initial location, spawned at runtime:
HOT-BELL, BROKEN-LAMP, DIAMOND, INFLATED-BOAT, GUNK, PUNCTURED-BOAT,
BROKEN-EGG, BAUBLE. GLOBAL-OBJECTS here: TEETH, WALL, GRANITE-WALL.
LOCAL-GLOBALS: BOARD, SONGBIRD, WHITE-HOUSE, FOREST, TREE, GLOBAL-WATER,
KITCHEN-WINDOW, CHIMNEY, SLIDE, BOARDED-WINDOW, CRACK, GRATE, BODIES,
RAINBOW, RIVER, LADDER, CLIMBABLE-CLIFF, WHITE-CLIFF.

### Cross-references
All 113 ACTION/DESCFCN/PER routines and all 10 PSEUDO routines resolve (110
in 1actions, 3 local). Every IN and GLOBAL target exists. No exit points to
an undefined room. No duplicate names. Note GO (l.2637) queues the five
startup interrupts and calls MAIN-LOOP; the comment at 2636 says it "must
live here", contradicting the stale gmain comment.

### Data findings
- RIVER-1 (2185) has no static incoming exit; it is reached only via the
  LAUNCH tables in 1actions 2693-2699.
- Nine intentional self-loop exits: 1543, 1605, 1625, 1645, 1710 (maze),
  2570, 2578, 2587, 2597 (mine).
- Four one-way exits via `PER MAZE-DIODES` whose real targets are only in
  comments (1554 -> MAZE-4, 1614 -> DEAD-END-1, 1642 -> MAZE-11, 1682 ->
  MAZE-5); truth is 1actions l.898.
- KITCHEN DOWN (1435): `TO STUDIO IF FALSE-FLAG ELSE "Only Santa Claus..."`,
  a never-true flag used to emit a refusal string.
- GRATING-ROOM's ACTION is MAZE-11-FCN (1671), a stale name.
- 6-char truncations in data: BEAUTI, COLONI (50), INFLAT (603, 722);
  `FCD\#3` (399); OVERBOARD as a TEETH synonym (21, deliberate parser trick).
- Odd flags: CHALICE has TAKEBIT and TRYTAKEBIT (361); BOOK has CONTBIT
  (266); MIRROR-1/2 list ENORMOUS as a noun (672, 678); WATER and
  GLOBAL-WATER share WATER-F; WOODEN-DOOR shares FRONT-DOOR-FCN (911).
- Editorial comments: `(ADJECTIVE LARGE STORM ;"-TOSSED")` 61; 336, 359,
  1787; stray `;COFFIN-CURE` on DEEP-CANYON NW (1924); old five-letter room
  names in trailing comments (`;"was RIVR1"`) 2185-2607.
- WINDING-PASSAGE (1978) and TWISTING-PASSAGE (1988) share an LDESC.
- Blank line after `(LDESC` at 2118, 2294, 2480; leading-space anomalies
  1662, 1707; 19 lines with trailing whitespace.

## 7. 1actions.zil (4177 lines)

### Inventory
186 ROUTINEs, 48 GLOBALs, 24 CONSTANTs, 3 GDECLs, no OBJECT/ROOM
definitions. 32 room functions (RARG), 16 interrupts (I-*), 10 *-PSEUDO
routines, about 110 object action functions, the rest helpers/engine.

| Lines | Section | Notable routines |
|---|---|---|
| 7-620 | White house / cellar | WEST-HOUSE, EAST-HOUSE, WATER-F 157, LIVING-ROOM-FCN 449, TRAP-DOOR-FCN, RUG-FCN, STONE-BARROW-FCN 403 |
| 622-769 | Troll | WEAPON-FUNCTION, TROLL-FCN 640 |
| 771-957 | Grating / maze / knives | LEAF-PILE, GRATE-FUNCTION, MAZE-DIODES 898, RUSTY-KNIFE-FCN, SKELETON |
| 958-1152 | Mirror, dome, Hades | MIRROR-MIRROR 971, LLD-ROOM 1058, I-XB/I-XC/I-XBH |
| 1153-1489 | Dam / reservoir | BOLT-F, I-RFILL, I-REMPTY, BUTTON-F, I-MAINT-ROOM, LEAK/PUTTY/TUBE |
| 1491-1747 | Bottle, cyclops, loud room | CYCLOPS-FCN 1515, I-CYCLOPS, LOUD-ROOM-FCN 1660 |
| 1748-2177 | Thief | THIEF-VS-ADVENTURER 1764, ROBBER-FUNCTION 1947, DEPOSIT-BOOTY, ROB-MAZE, CHALICE-FCN, TREASURE-ROOM-FCN |
| 2178-2415 | Light sources | LANTERN, MATCH-FUNCTION, I-LANTERN, I-CANDLES, LIGHT-INT, LAMP-TABLE/CANDLE-TABLE |
| 2416-2583 | Sword, coal mine | SWORD-FCN, BOOM-ROOM, BATS-ROOM, MACHINE-F, MSWITCH-FUNCTION |
| 2585-2878 | River / beach | SCEPTRE-FUNCTION, RBOAT-FUNCTION 2722, I-RIVER, IBOAT-FUNCTION, SAND-FUNCTION |
| 2880-3112 | Forest, egg, rope, slide | TREE-ROOM, EGG-OBJECT, CANARY-OBJECT, ROPE-FUNCTION 3030, SLIDE-FUNCTION |
| 3113-3234 | Death, pseudo-objects | DEAD-FUNCTION 3113, 10 *-PSEUDO |
| 3236-3330 | Melee data | F-* constants, DEF1..DEF3C, DEF*-RES, *-MELEE tables, VILLAINS |
| 3331-3606 | Melee engine | DO-FIGHT, FIGHT-STRENGTH, VILLAIN-STRENGTH, VILLAIN-BLOW 3413, HERO-BLOW 3476, WINNING?, I-CURE |
| 3808-3990 | "DEMONS" | I-FIGHT, AWAKEN, I-SWORD, I-THIEF 3890, DROP-JUNK, RECOVER-STILETTO, STEAL-JUNK, ROB |
| 3993-4177 | Special-cased | V-DIAGNOSE, V-SCORE 4026, JIGS-UP 4046, RANDOMIZE-OBJECTS 4101, KILL-INTERRUPTS 4125, TROLL-ROOM-F |

### Cross-file dependencies
Every called routine and every referenced global resolves somewhere in
zil/ (automated cross-reference). The only symbols without a textual
definition are flag bits (defined implicitly by FLAGS clauses) and the
ZILCH-provided LOW-DIRECTION. MIN (2339) is a local routine shadowing the
MDL builtin. GO in 1dungeon.zil (2644-2649) patches DEF1-RES/DEF2-RES/
DEF3-RES at runtime (see finding a).

### Findings
a. DEF*-RES tables (3301-3317) are compiled with literal 0 slots because the
   `<REST ,DEFn k>` forms are commented out; GO fills them at startup with
   `<PUT ,DEF1-RES 1 <REST ,DEF1 2>>` etc. Consequence for a port: each
   13/12/11-word DEF table is read as a sliding 9-entry window (RANDOM 9) at
   word offset 0, 1 or 2 chosen by attacker/defender strength delta; entries
   past the window are never read (DEF2A's trailing UNCONSCIOUS, DEF3C's
   last SERIOUS-WOUND). Verified in the story file at 0x2bd4/0x2bda/0x2be2
   and by patched-story runs under dfrotz.
b. Unused symbols reconcile with zork1.errors: UNTIE-FROM (3080, no SYNTAX
   maps to it), BREATHE (2817; V-BREATHE in gverbs is used instead).
   CYCLOPS/TROLL/THIEF-MELEE and DEF2A/DEF3C are "unused" as globals only
   because they are referenced through table initialisers. Commented-out
   globals LEAVES-GONE (770), EGYPT-FLAG (1052); commented-out code at
   1776-1783, 2000-2003, 3156, 4085, 3120, 1723.
c. PROB's second argument is ignored. `<PROB 75 90>` (647), `<PROB 60 80>`
   (1935), `<PROB 50 80>` (2420), `<PROB 10 0>` (1965), `<PROB 30 T>`
   (3940), `<PROB 10 T>` (3964) all mean "base percent if LUCKY, else base
   against RANDOM 300" (gmacros 115-122). `.HERO?` at 3446 sits inside the
   discarded argument and never compiles. A port that honours the second
   number is wrong.
d. Unreachable branches: KITCHEN-FCN 396-400 tests CLIMB-UP STAIRS twice, so
   "There are no stairs leading down." never prints. TREE-ROOM 2909-2911 has
   the LEAP clause nested inside DROP. LEAF-PILE 790-796 calls
   REMOVE-CAREFULLY before testing IN? HERE, so "The leaves burn." is
   unreachable and burning the leaves always kills. DROWNINGS (1284)
   entries 7-8 are never printed because I-MAINT-ROOM drowns the player at
   level 14 first. HERO-MELEE lacks HESITATE/SITTING-DUCK rows, safe only
   because HERO-BLOW never sets OUT?. BOOM-ROOM 2447-2448 re-tests M-END.
e. Quirks to replicate: STONE-BARROW-FCN 425 reads story-header byte 1 bit 3
   (the Tandy flag) to suppress the Zork II/III advertisement. LOUD-ROOM-FCN
   1685-1727 bypasses the parser with a raw READ loop (only SAVE/RESTORE/
   QUIT/directions/ECHO/BUG handled) and ECHO clears SACREDBIT on the
   platinum bar. JIGS-UP: third death ends the game; a second death after
   touching SOUTH-TEMPLE sends the player to Hades with DEAD-FUNCTION as the
   player's action. SCEPTRE 2611 dumps rainbow objects into WALL when
   un-solidified. RBOAT-FUNCTION 2779-2782 removes the boat then ROBs back
   only TVALUE>0 items, so non-treasure contents (including the label) are
   lost. MATCH-FUNCTION reports MATCH-COUNT-1, so a count of 6 gives five
   usable matches. TRAP-DOOR-FCN 514 uses bare LIVING-ROOM without a comma
   (works under ZILCH). VILLAIN-STRENGTH's sword advantage also applies to
   the troll's own attack because PRSI is still SWORD during the clock tick
   (confirmed empirically).

### File-level
Pure ASCII, LF, no trailing newline at EOF (l.4177). Trailing whitespace on
446, 814, 1680, 2226, 2467, 2723, 4173. 30 real form-feeds. Header says
1983; assembled as 1ACTIONS.ZAP.21 into R119 on 1988-04-29. Every table
layout checked in the binary matches this source.

## 8. Summary of findings by importance

Blocking or high value for the port:
1. PROB second argument is ignored (finding 7c). Any port logic using the
   "unlucky" percentages literally is wrong; the real rule is RANDOM 300.
2. DEF*-RES sliding-window combat tables are filled at runtime by GO
   (7a); the original tables are the only spec for hit outcomes.
3. PERFORM dispatch order and the multi-object loop semantics in gmain
   (section 3) including the any-verb "There's nothing here you can take."
4. MAZE-DIODES targets live in code, not in the room data (6).
5. Vestigial mechanisms that must NOT be ported as designed: C-DEMONS
   (gclock), LAST-PSEUDO-LOC, DEMON, SERIAL, P-DIRECTION/P-DIR/P-ACT/
   P-AADJ/P-ADVERB, DESC-OBJECT.
6. parser.cmp is an older-vs-Spellbreaker diff listing, not a spec; ignore.

Medium:
7. Original dead branches (kitchen stairs, tree leap, leaf burn, drownings
   7-8) should be replicated as dead, or consciously fixed and documented.
8. Text-level canon: fixed "a " article, trailing spaces in FIRSTER lines,
   double hyphen in "Ahoy --", "knifesmanship", shared LDESC for the two
   passages, "nondescript" spelling (R119, not R88).
9. Six-character dictionary truncation shapes data (BEAUTI, COLONI, INFLAT,
   NORTHE/SOUTHE) and the port must decide whether to emulate it.

Low / housekeeping:
10. Files with no trailing newline (gverbs, 1actions); literal `^L` text vs
    real form-feeds; trailing whitespace; stale comments (GO location,
    GRATING-ROOM's MAZE-11-FCN name).
11. The R88 snapshot in git history is a second reference version; useful
    for diffing but must not be confused with the R119 target.
