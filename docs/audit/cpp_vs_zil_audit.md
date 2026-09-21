# Read-only audit, phase 2: C++ port (`src/`) vs. the ZIL (`zil/`)

Date: 2026-09-19. Goal: verify the C++ reproduces the ZIL, quirks included.
Nothing in the repo was modified. Build succeeds and all 74 ctest targets
pass, but the tests assert the port's own behaviour, not the ZIL's (see
section 9).

Line references: `zil/…:N` for ZIL, `src/…:N` for C++. Claims marked
"verified" were re-checked by the lead auditor with grep or a game run.

## 1. Architecture finding (root cause of most divergences)

The tree contains two implementations glued together:

- A from-scratch, requirements-style implementation that actually runs:
  `src/parser/parser.cpp` (own vocabulary and tokenizer), the first ~3100
  lines of `src/verbs/verbs.cpp`, the per-turn NPC/light/combat systems in
  `src/systems/`, and `src/core/gmain.cpp`'s multi-object loop.
- A later "ZIL:"-annotated layer that transcribes the ZIL closely but is
  largely NOT wired in: `src/parser/gsyntax.cpp` (all 267 SYNTAX lines,
  correct), `src/parser/gparser.cpp` (partial port of gparser.zil with
  stubbed PARSER/SYNTAX-CHECK/ORPHAN/CLAUSE-COPY/STUFF), and the tail of
  `src/verbs/verbs.cpp` (goTo, describeRoom, printCont, printContents,
  finish, yes, hackHack, hitSpot, shakeLoop, scoreUpd, ccount, otherSide,
  mungRoom, noGoTell, findIn, firster, seeInside, iTake).

Verified: `GSyntax::populateVerbRegistry` has no caller outside
`tests/gsyntax_tests.cpp`; the only runtime uses of `GParser` are
`bufferPrint` (gglobals.cpp:65,71) and `isLit` (actions.cpp:89); `goTo`,
`describeRoom`, `printCont` have zero callers outside verbs.cpp itself.
`tests/gparser_tests.cpp` and `tests/gsyntax_tests.cpp` exercise the dead
layer, which is why the suite passes while the game behaves differently.

Consequence: the faithful data exists but the player never reaches it.

## 2. Core engine, clock, main loop, PERFORM (src/core, src/systems/timer)

Faithful: PERFORM step order (gmain.cpp:314-366 vs gmain.zil:211-224) with
PRSA/PRSO/PRSI save/restore; meta-verb list that skips CLOCKER
(gmain.cpp:377-395); M-END call (459-463); CLOCK-WAIT single skip
(timer.cpp:110-113); PROB/ZPROB incl. ignored second arg and RANDOM 300
when not LUCKY (gmacros.h:210-223); PICK-ONE algorithm (gmacros.h:265-291);
RANDOM-ELEMENT; FLAMING?/OPENABLE?/ABS/BSET*/ENABLE/DISABLE; all gglobals
object texts (NOT-HERE-OBJECT-F, STAIRS-F, SAILOR-FCN, GRUE, ZORKMID,
PATH-OBJECT, CRETIN-FCN, GROUND) character-identical; LOAD-MAX/LOAD-ALLOWED
100; LUCKY true; GO startup sequence (main.cpp:37-67).

Divergences, behaviour-visible first:
1. Room M-BEG result discarded. `RoomActionFunc` is `std::function<void(int)>`
   (rooms.h:300, verified) and step 2 always returns M_NOT_HANDLED
   (gmain.cpp:321-328). ZIL stops PERFORM when the room returns true
   (gmain.zil:212). Affects KITCHEN-FCN 1actions:397, STONE-BARROW 404,
   LLD-ROOM 1071, 2559, 2574, TREE-ROOM 2890, 3006: double output or
   double movement.
2. Direction moves bypass PERFORM: gmain.cpp:450-452 calls vWalkDir
   directly, so WINNER action, room M-BEG and M-END never see PRSA=V?WALK
   (gmain.zil:79-81).
3. RFATAL cannot propagate: handlers return bool (gmain.cpp:333-364);
   `rfatal()` (gmacros.h:201) is unreachable. ZIL uses value 2 to abort the
   multi-object loop (gmain.zil:150), skip M-END (151), clear P-CONT (161).
   P-CONT is also not cleared on parse failure (gmain.zil:162-163).
4. IT handling replaced: ZIL substitutes IT in P-PRSI then P-PRSO
   (gmain.zil:45-64) and prints "I don't see what you are referring to." +
   RFATAL only when IT is present and inaccessible (194-197). C++ prints it
   whenever o==i==nullptr and g.it==nullptr for EXAMINE/TAKE/READ
   (gmain.cpp:292-296), so bare TAKE gets that instead of the orphan
   question. Parser keeps separate lastObject_/lastObjects_ and treats THEM
   as a list; ZIL THEM/HER/HIM are IT synonyms (gglobals.zil:44). The
   `PRSI != IT` guard (gmain.zil:202) is missing (gmain.cpp:303).
5. Multi-object loop rewritten: only `isAll` handled (gmain.cpp:434-449).
   Missing NUM>1 without ALL, P-NOT-HERE counting and "The [other]
   object[s] that you mentioned is/aren't here." (gmain.zil:98-109), the
   TAKE-ALL skip rules (127-139), the any-verb "There's nothing here you can
   take." (111-112; C++ prints non-ZIL "There's nothing here to {word}.",
   gmain.cpp:439), and the PRSO/PRSI role swap when P-PRSI is the multi
   table (72-76, 115-118).
6. Zero-object branch missing ("It's too dark to see." / "It's not clear
   what you're referring to.", gmain.zil:82-90).
7. MOVES: `g.moves++` for every parsed command incl. meta verbs and
   CLOCK-WAIT turns (gmain.cpp:470, verified); ZIL increments only inside
   CLOCKER (gclock.zil:50). A second counter `ScoreSystem::moves_` is
   consulted by V-SCORE when g.moves==0 (verbs.cpp:2193).
8. Startup interrupt queue differs from GO (1dungeon.zil:2638-2642): I-FIGHT
   created on combat start with an extra "Combat begins!" (combat.cpp:58-65);
   I-SWORD registered enabled (sword.cpp:152-155) vs queued disabled;
   I-THIEF interval 4 (npc.cpp:164) vs every turn; I-CANDLES interval 1
   (candle.cpp:79) vs QUEUE 40; I-LANTERN interval 1 (lamp.cpp:76) vs QUEUE
   200; LAMP-TABLE/CANDLE-TABLE stage countdown replaced by per-turn
   counters (`lampBattery 330`, globals.h:402).
9. CLOCKER traversal order reversed: ZIL scans from the most recently
   registered interrupt (gclock.zil:29-39, 46-60); C++ iterates in
   registration order (timer.cpp:260). Same-turn message order differs.
10. CLOCKER FLG set whenever a callback ran, callbacks are void
    (timer.cpp:287,295); ZIL sets it only when the routine returns true.
    Timers auto-reset via interval/repeating (timer.h:38-42); ZIL interrupts
    fire once and re-QUEUE themselves.
11. GROUND-FUNCTION and CRETIN-FCN call vDrop()/vTake() directly with a
    mutated g.prsa (gglobals.cpp:126-128, 179-181) instead of PERFORM
    (gglobals.zil:173, 229), skipping PRE-* and object actions and leaving
    prsa changed for M-END.
12. GRUE unreachable: created with desc "grue", INVISIBLE, no location
    (world_init.cpp:3827-3832, kept by getOrCreate gglobals.cpp:371-378);
    ZIL "lurking grue" in GLOBAL-OBJECTS, no flags (gglobals.zil:184-189).
    GROUND gains NDESCBIT+INVISIBLE (gglobals.cpp:366-367) that ZIL lacks.
13. NOT-HERE-PRINT inserts a space between XADJN and XNAM and substitutes
    "such thing" for an empty clause (gglobals.cpp:56-74); ZIL does neither
    (gglobals.zil:67,77-79).
14. Output layer (io.cpp:345-411) collapses whitespace, inserts spaces
    between adjacent prints at word boundaries, and treats embedded "\n" as
    hard breaks; Z-machine joins source newlines into spaces and wraps at
    screen width. Texts ported with literal "\n" (e.g. grue text
    gglobals.cpp:145-149) keep 1983 line breaks; split TELLs like
    `D ,PRSO "smanship"` would print "sword smanship". `tell()` always
    appends CRLF (io.h:322-325).
15. INFLATED-BOAT: ZIL sets property VTYPE=NONLANDBIT; C++ sets the flag on
    the boat (main.cpp:33-35), which GOTO's vehicle check cannot read.
16. Prompt "\n> " (gmain.cpp:402-403) vs ZIL ">" (gparser.zil:152). Empty
    input returns silently (gmain.cpp:407-415); "I beg your pardon?" exists
    nowhere in src.

Missing: DEF1/2/3 tables and DEF*-RES patching (verified: no DEF1/DEF2/DEF3
in src). P-NOT-HERE declared, never incremented.

Extra (no ZIL basis): "That command is too long." (gmain.cpp:418-420);
"Goodbye!" on EOF; processTrollTurn()/processCyclopsTurn() run outside the
clock every turn (gmain.cpp:475-476); negative tick marks a "demon"
(timer.cpp:189-191); V_KILL as a separate verb (gglobals.cpp:195); INTNUM
synonym "number"; extra state globals lampBattery, lampWarned, rugMoved,
gateFlag, gatesOpen, grunlock, waterLevel, grateRevealed, matchCount 5 (ZIL
MATCH-COUNT 6), buoyFlag (globals.h:401-430).

Vestigial ZIL modelled anyway: Timer::isDemon and the !pWon skip
(timer.cpp:272-276, asserted by gclock_tests.cpp:144-154); pDirection
(globals.h:456).

## 3. Parser (src/parser)

Faithful: gsyntax.cpp is a correct transcription of every SYNTAX line,
BUZZ list, preposition group, and verb synonym (POUR IN -> V-DROP 4948,
LOOK AT x WITH y -> V-READ 4134, EXAMINE IN -> V-LOOK-INSIDE 2648, TURN x
WITH (FIND RMUNGBIT) 7452, $VERIFY/#RANDOM/#COMMAND/#RECORD 944-1003, RAPE
5727, shut->TURN, see/seek->FIND, sit->CLIMB, temple->TREASURE). gparser.cpp
reproduces SEARCH-LIST/DO-SL (130-159), GLOBAL-CHECK (161-181), WHICH-PRINT
comma rule (183-197), noun-missing and too-dark strings (204, 224),
UNKNOWN-WORD/CANT-USE texts (419-432), NUMBER? partially (464-492),
ACCESSIBLE?/META-LOC (72-119). None of it runs.

The running `Parser` (parser.cpp), divergences ranked:
1. Hand-typed vocabulary subset (parser.cpp:19-159). Verified at runtime
   that `x mailbox`, `give lamp to mailbox`, `shut door`, `see leaflet`,
   `sit`, `pour water in bottle`, `what is a grue`, `stand up`, `blow in
   leaflet`, `walk north` all yield `I don't know the word "…"`. Missing
   words include x/describe/what/whats, give/donate/offer/feed/hand,
   walk/run/proceed/step, shut/set/flip, see/where/seek, sit, pour/spill,
   stand, blow, hold/carry/remove/grab/catch, stuff/insert/place/hide,
   taste/bite, incinerate/ignite, murder/slay/dispatch, fight/hurt/injure/
   hit, cross/ford, count, kick/taunt, knock/rap, lean, wake, wear, find,
   kiss, follow, answer, echo, mumble, repent, wish, spray, treasure, win,
   skip, stay, hatch, melt, plug, brush, bug, chomp, cut, launch, lock/
   unlock, apply, strike, pick, pump, squeeze, spin, shake, make, play.
2. No punctuation/clause handling: whitespace-only tokenizer
   (parser.cpp:227-235). `open mailbox.` -> unknown word "mailbox."; THEN
   and AND unknown; `n then s` executes only `n` (926-934). ZIL CLAUSE
   (gparser.zil:440-510), P-CONT (139-146), "dir then"/"dir, ..." (280-306)
   absent.
3. Multiple direct objects and BUT lists not parsed (977-1016 handles only
   ALL [except X]); MANY-CHECK message unreachable.
4. Object resolution ignores syntax scope bits and FIND bits (355,
   294-320); no GWIM, so `climb up`/`look around`/`stand up` do not orphan;
   `take out` printed "( leaflet)" (non-ZIL echo with stray space).
   Pseudo-object GLOBAL-CHECK and ROOMS-only rule replaced by hard-coded
   KITCHEN_WINDOW/GROUND exceptions (304-316). Adjective-only input accepted
   as a noun (379-401), unlike THIS-IT?.
5. Orphaning is a bespoke state machine (663-689, 870-919) without
   P-OTBL/ORPHAN-MERGE: no indirect-object prompt from the parser, no "It's
   difficult to repeat fragments.", no ACLAUSE-WIN; orphan abandoned if the
   reply starts with a verb word (876-879).
6. IT: `examine it` after leaving the room still described the mailbox
   (verified); "I don't know what "it" refers to." is C++-only (1025);
   her/him ignored.
7. Message text: "I don't understand that sentence." (956, 961) vs "That
   sentence isn't one I recognize." / "There was no verb in that
   sentence!"; "I don't understand that." (1055) vs CANT-USE; "I don't see
   that here." (914) has no source; "You can't see any X here!" emitted at
   parse time (1159) so FIND on absent objects cannot respond; WHICH prompt
   is a numbered menu with "(here)/(in your inventory)" reading its own
   line (511-531) vs the single-line question answered by the next command;
   "I don't understand that choice." C++-only.
8. AGAIN/OOPS: AGAIN re-parses stored text (832-840), lacks "Beg pardon?",
   "That would just repeat a mistake.", "AGAIN, x". OOPS messages "There was
   no word to correct." / "Oops what?" (845, 850) vs ZIL; correction by
   substring replacement on raw text (648-661).
9. ALL: excludes TRYTAKEBIT objects and anything not directly in HERE
   (608-646) vs gmain.zil:120-145; uses a non-ZIL INHIBIT flag (622);
   "everything" as ALL synonym (589).
10. NUMBER? not called at runtime (`take 1000` -> unknown word); LIT? in
    gparser.cpp:17-49 looks only one level into open room containers, not
    held containers.
11. Six-character truncation not emulated (`hasSynonym` exact match,
    object.cpp:63-75). Generic prepositions for/about/off/out/over/across/
    behind/around/down/up (188-220) accepted everywhere; "Use compass
    directions for movement." C++-only.

VerbRegistry (used only for preposition validation and PUT remapping,
parser.cpp:1053-1067) spot-check of 44 SYNTAX lines: present and correct
TAKE, TAKE FROM/OUT/OFF, DROP, PUT IN/ON, LOOK AT/IN, READ, LOCK/UNLOCK
WITH, ATTACK WITH, THROW AT, TURN ON/OFF, TURN WITH, TIE TO, LISTEN TO/FOR,
DRINK FROM, INFLATE WITH, SWING AT, CLIMB UP/DOWN/ON, BOARD/DISEMBARK, LOOK
UNDER/BEHIND, THROW OFF. Missing or wrong: bare ATTACK/KILL OBJECT present
(Zork II only, gsyntax.zil:92-94, 260-262); OPEN/CLOSE use CONTBIT instead
of FIND DOORBIT; TURN x WITH requires TOOLBIT; POUR IN -> V_PUT instead of
V-DROP; POUR FROM, LOOK AT WITH -> READ, EXAMINE IN/ON, LOOK ON, INFLAT,
TAKE OUT/UP (RMUNGBIT), CLIMB UP/DOWN (RMUNGBIT), GIVE (both forms),
SEARCH FOR/LOOK FOR -> FIND, TELL ABOUT, WAVE AT, BLOW OUT/UP, PUT OUT ->
LAMP-OFF, PUT ON -> WEAR, TAKE IN/ON (VEHBIT), WALK = V-WALK-AROUND all
missing; KNOCK DOWN reversed; PICK x UP and GO IN/OUT -> ENTER/EXIT are
C++ additions; $/# prefixes dropped from debug verbs (parser.cpp:155-158).

## 4. Generic verbs (src/verbs/verbs.cpp)

Faithful: V-BACK, V-ZORK, V-ADVENT, V-FROBOZZ, V-EXORCISE, V-RING, V-KISS,
V-MUMBLE, V-PLUG, V-PUT-UNDER, V-MAKE, V-WISH, V-PUSH-TO, V-LOOK-UNDER,
V-UNTIE, V-TIE, V-CURSES, V-MOVE default, V-EXAMINE default chain
(890-905), V-LOOK-INSIDE texts, V-LOOK-ON, V-SGIVE, V-SSPRAY, V-SEARCH,
V-SMELL, V-LISTEN, V-ATTACK refusal strings (1889-1916), V-HELLO with
object, V-SCORE text (rank thresholds off by one: C++ `>=330` vs ZIL
`G? 330`, i.e. one point early at 330/300/200/100/50/25), PRE-BOARD,
PRE-DROP, PRE-MOVE, PRE-READ, PRE-TURN's TURNBIT check, PRE-TAKE (minus
GROUND case), V-WIND, V-REPLY, V-OVERBOARD ("Ahoy -- " kept), V-LEAP
WHEEEEE order, PICK-ONE port.

Divergences, ranked:
a. ITAKE/V-TAKE (gverbs 1900-1964 vs verbs.cpp 344-451, 3698-3716): no DEAD
   "Your hand passes through its object.", no YUKS table ("You can't take
   that." instead), no closed-container kludge, weight sums top-level SIZE
   only (not recursive WEIGHT, worn != 1, no container SIZE), size default
   0->5 at runtime, "You're carrying too much." vs "Your load is too heavy"
   + ", especially in light of your condition.", no RFATAL, no fumble rule
   (CCOUNT>7, CNT*8 percent, "You're holding too many things already!"),
   no NDESCBIT clear/TOUCHBIT set/SCORE-OBJ, WEARBIT "You are now wearing
   the X." missing.
b. V-PUT (1085-1114 vs 1116-1223): check order wrong; requires CONTBIT
   ("You can't put something in that.") vs OPENBIT/OPENABLE?/VEHBIT; "The X
   is closed." vs "The X isn't open."; capacity default 0->100, size 0->10;
   no TOUCHBIT/SCORE-OBJ; RFATAL-truthiness quirk absent. prePut (3411)
   prints "You can't do that."/"You don't have the X." vs PRE-PUT = PRE-GIVE
   "That's easy for you to say since you don't even have the X."; preGive
   (3397) drops "even" and the object name.
c. GOTO/V-WALK/DESCRIBE-ROOM (vWalkDir 807-926, vLook 205-283): no vehicle
   VTYPE logic, no NO-GO-TELL, no RMUNGBIT refusal, no "comes to a rest on
   the shore.", no dark-to-dark 80 percent grue with SPRAYED? gurgling, no
   "You have moved into a dark place." + P-CONT clear, no SCORE-OBJ on room,
   no ENTRANCE-TO-HADES suppression, no "The X leaves the room."; unknown
   direction in dark never kills. Invented "The door is locked."; extra dam
   water-level block (911-918). Dark text "It is pitch dark. You are likely
   to be eaten by a grue." (light.cpp:155) vs "It is pitch black."; invented
   "You hear a faint rustling in the darkness." and a turn-counted grue
   death; MAZEBIT always-verbose missing; vehicle suffix missing; M-FLASH
   never applied; SUPER-BRIEF LOOK skips DESCRIBE-OBJECTS; "Only bats can
   see in the dark. And you're not one." absent.
d. PRINT-CONT/inventory/listing (224-282, 285-352, describeObject 3502):
   "There is a X here." ignores FDESC/TOUCHBIT rule; " (providing light)",
   " (outside the <vehicle>)", " (being worn)", DESCFCN/M-OBJDESC missing;
   "The X contains:" + 2-space bare desc vs "Sitting on the X is: ", "The X
   is holding: ", "Your collection of treasures consists of:", INDENTS depth
   5; vInventory uses A/An vowel logic vs fixed "A ".
e. V-OPEN (916-989): "You can't open that." vs "You must tell me how to do
   that to a X."; "It's already open." vs "It is already open."; invented
   LOCKEDBIT; CAPACITY!=0 rule ignored; single-untouched-FDESC case
   missing; PRINT-CONTENTS ", "/", and "/"and " + THIS-IS-IT replaced by
   A/AN and no "and"; door "The X opens." missing.
f. V-CLOSE (991-1041): no CAPACITY/SURFACEBIT gate, no "You cannot close
   that.", no "It is now pitch black." LIT re-evaluation (comment admits).
g. V-WAIT (2808-2813): prints "Time passes..." but never ticks CLOCKER x3
   nor sets CLOCK-WAIT.
h. YES?/QUIT/RESTART/FINISH/VERSION/SAVE/RESTORE/SCRIPT: vQuit reads a full
   lowercased line (ZIL: first word YES/Y); vRestart "Are you sure you want
   to restart? (yes/no)" vs "Do you wish to restart? (Y is affirmative): "
   + "Restarting."; vVersion (2493) prints "C++ Port - Release 1" /
   "Interpreter: C++23 Native" omitting "Infocom interactive fiction - a
   fantasy story", "1984, 1985, 1986", Release 119 / Serial 880429;
   vSave/vRestore prompt for filenames, "Game saved."/"Game restored." vs
   "Ok."/"Failed.", no V-FIRST-LOOK after restore; script strings invented;
   finish() dead.
i. V-TELL (2544-2610): "Tell whom?"/"You can't tell that anything."/"There
   is no response." vs "You can't talk to the X!" + RFATAL / "The X pauses
   for a moment, perhaps thinking that you should reread the manual." and
   actor-command mode; vAsk/vTalk have no ZIL counterpart.
j. Long tail of per-verb text/logic mismatches (all cited in the verbs
   agent report; highlights): vThrow never dies; vAttack routes to
   CombatSystem not HERO-BLOW; V-DRINK lacks water/bottle chain; lamp
   "It's already on/off." vs "It is already on/off.", no CRLF+V-LOOK on
   light in dark, no "It is now pitch black." on lamp off, invented "The
   lamp has no more power."; vInflate/vDeflate/vPray/vDisembark/vBoard/
   vEnter/vExit/vClimb*/vRead/vLock/vUnlock/vTouch/vPush/vWave/vRub/vHello/
   vJump/vSkip/vSay/vKick/vBreathe/vRape ("What a loony!" vs "What a (ahem!)
   strange idea.")/vAlarm/vWalkTo/vLaunch/vCut/vLower/vRaise/vMelt/vPlay/
   vPourOn/vShake/vSpin/vSqueeze/vTieUp/vAnswer/vCommand/vEcho/vFollow/
   vRepent/vSend/vSpray/vBlast/vBurn/vChant/vIncant/vDisenchant/vEnchant/
   vWin/vTreasure/vStay/vBrush/vBug/vCount/vCross/vHatch/vKnock/vLeave/
   vLeanOn/vPump/vStrike/vReadPage/vPick/vApply/vOil/vStab/vDrinkFrom/
   vLookBehind/vRandom/vRecord/vUnrecord/vVerify/vMung/vWear/vFind/vStand/
   vThrough/vGive/vTurn/preTurn/preBurn ("With a X?!?" vs "With a X??!?")/
   tellNoPrsi ("You must specify what to use." vs "You didn't say with
   what!")/preMung/preFill/preSGive/vDiagnose (health percent vs wound
   count + I-CURE ticks + "You can be killed by…")/V-SUPER-BRIEF clears
   VERBOSE. HO-HUM/YUKS/DUMMY/HELLOS/SWIMYUKS PICK-ONE tables are not
   defined anywhere in src (no "A valiant attempt." string).
k. Stubs in the "ZIL layer" that misrepresent the source: lkp prints text;
   otherSide returns door location; noGoTell wrong string; removeCarefully
   lacks P-IT-OBJECT clear/LIT recompute/"You are left in the dark...";
   scoreUpd lacks BASE-SCORE and the 350 side effects; seeInside wrong
   rule; shakeLoop invented "spills out"; ccount excludes NDESCBIT not
   WEARBIT; firster returns an object; yes() accepts whole line.

Missing entirely: V-DIG, real V-ECHO/V-RANDOM/V-COMMAND-FILE, V-LEAVE
(DO-WALK OUT), V-STAND vehicle case, FIND-WEAPON, AWAKEN hook, V-CLIMB-ON,
V-PUT-ON (GROUND->DROP, SURFACEBIT->PUT, "There's no good surface on the
X."), V-SWIM dungeon branch, V-STRIKE->LAMP-ON, V-FILL, real V-DRINK-FROM,
V-KNOCK door branch.

Extra: tryImpliedObject GWIM-in-verbs (62-200); A/AN articles (301-314,
963-975); LOCKEDBIT concept; "You can't see any such thing." inside verbs;
dead vSearchOld (1088-1113). No Zork II/III branches wrongly included.

## 5. NPCs, melee, death, light interrupts, save (src/systems, npc handlers)

Full 70-row routine table: scratchpad/phase2_npc_combat_table.md.

Dead C++ that holds the faithful values but never runs: `npcs.cpp
initializeNPCs` (STRENGTH 2/5/10000; the live `world_init.cpp` uses troll
8, thief 5, cyclops 10, adventurer 10 vs ZIL 0), `light.cpp checkGrue/
updateLighting`, `save.cpp SaveSystem`, `actions_group_a.cpp
cyclopsRoomAction/treasureRoomAction/robberAction`.

1. Melee system not ported (1actions 3236-3849 vs combat.cpp, npc.cpp:341/
   660/1043). ZIL: DEF table chosen by defender strength and attacker-minus-
   defender delta, RANDOM 9 result, nine F-* outcomes, FIGHT-STRENGTH =
   2 + SCORE/70 + P?STRENGTH, wounds as negative P?STRENGTH with
   LOAD-ALLOWED reduced 10/20 and I-CURE every 30 turns, V-BEST-ADV
   (sword vs troll, knife vs thief), THIEF-ENGROSSED cap, sword advantage
   applying to the troll's own blow, I-FIGHT demon with F-FIRST? (troll 33
   percent, thief 20), villain unconscious/wake-up escalation, STAGGERED,
   LOSE-WEAPON "Fortunately, you still have a X.", SITTING-DUCK. C++: HP
   model (player 20, enemy strength*5), 60 percent +-5/pt hit roll, +-30
   percent damage; none of the nine results modelled; 0 of 96 melee message
   strings present (HERO-MELEE 29, TROLL-MELEE 24, THIEF-MELEE 27,
   CYCLOPS-MELEE 16); invented "You deal 7 damage!", "Combat begins!",
   "The troll growls menacingly."; only the fog line (combat.cpp:308) and
   the thief retreat line (262, wrongly gated on HP<30 percent) survive.
   Villain death: F-DEAD dispatch replaced by invented trollDeath/
   thiefDeath text; DEADBIT/INVISIBLE instead of REMOVE-CAREFULLY; thief's
   "As the thief dies, the power of his magic decreases, and his treasures
   reappear:" / "The chalice is now safe to take." missing. vAttack
   (verbs.cpp:2091-2098) reaches startCombat only for bat/ghosts; troll/
   thief/cyclops use per-NPC invented rolls.
2. Death (death.cpp:155-337 vs JIGS-UP 4046-4099): C++ increments DEATHS
   before testing < 2, so the SECOND death ends the game (ZIL: third).
   Invented prompts "Do you wish to be resurrected? (Y/N)", "Do you wish to
   continue? (Y/N)", "Very well. The game is over."; "Bad luck, huh?" for
   LUCKY false absent; Hades path sets troll INVISIBLE instead of
   TROLL-FLAG and moves silently instead of GOTO; already-dead branch
   prints "The game is over." instead of FINISH; no RFATAL so the rest of
   the command continues.
3. KILL-INTERRUPTS (death.cpp:133): omits I-XB/I-XC and MATCH ONBIT clear;
   additionally kills I-THIEF, I-TROLL, I-FIGHT, so the thief stops forever
   after any death.
4. Cyclops (npc.cpp:1115-1301 vs 1515-1660): CYCLOMAD has 4 of 6 lines
   (missing "The cyclops is moving toward you in an unfriendly manner." and
   "You have two choices: 1. Leave  2. Become dinner."); wrath increments
   30 percent of turns after 3 turns, never kills (TODO npc.cpp:1141) vs
   +-1 per turn and JIGS-UP at |wrath|>5; GIVE WATER prints two invented
   lines; not-thirsty/garlic/"not so stupid" branches unreachable; GIVE
   LUNCH omits ENABLE QUEUE I-CYCLOPS -1 and MIN -1 (- COUNT); MUNG line
   "Do you think I'm as stupid as my father was?" absent; ODYSSEUS sets
   neither MAGIC-FLAG nor CYCLOPS-FLAG nor disables I-CYCLOPS; second
   ODYSSEUS branch (npc.cpp:1208) prints invented text.
5. Thief (npc.cpp:59-585): entirely invented (fixed room list, 4-turn timer
   vs I-THIEF -1, 70/50/30/20 steal odds, "Someone carrying a large bag is
   creeping through the shadows."). No ZIL encounter text exists (grep
   "lean and hungry", "Doing unto others", "robbed you blind", "rummaging
   through his bag", "scream of anguish", "gestures mysteriously", "left
   you in the dark" all empty). Missing: object-order room walk skipping
   SACREDBIT/non-RLANDBIT, ROB 75 percent of TOUCHBIT rooms, ROB-MAZE,
   STEAL-JUNK 10 percent (DOME-FLAG reset on rope), DROP-JUNK 30 percent,
   INVISIBLE toggling, THIEF-HERE, EGG-SOLVE on deposit, HACK-TREASURES,
   TREASURE-ROOM-FCN ambush, thief starting INVISIBLE in ROUND-ROOM.
   ROBBER-FUNCTION: EXAMINE text invented (ZIL "The thief is a slippery
   character with beady eyes..."); GIVE "The thief takes the X and places
   it in his bag." vs two ZIL variants; TELL/TAKE/LISTEN/HELLO-unconscious/
   THROW KNIFE (10 percent flee) absent. STILETTO TAKE "The thief deftly
   snatches the stiletto out of your reach." vs "The thief swings it out
   of your reach."
6. Troll (npc.cpp:595-846): EXAMINE constant instead of mutable LDESC
   ("A pathetically babbling troll is here." etc.); GIVE "The troll is not
   interested in your gift." vs ZIL catch/eat logic (20 percent "eats it
   hungrily. Poor troll, he dies from an internal hemorrhage"); TAKE/MOVE
   "The troll spits in your face", MUNG "laughs at your puny gesture",
   LISTEN, HELLO, TELL, axe-recovery F-BUSY? 75 percent absent;
   processTrollTurn invents unprompted attacks; axeAction
   (actions_group_a.cpp:145) tests NDESCBIT not TROLL-FLAG.
7. Light sources. Lamp: ZIL LAMP-TABLE 200+100+70+15 = 385 turns with
   "The lamp appears a bit dimmer." / "The lamp is definitely dimmer now."
   / "The lamp is nearly out." / "You'd better have more light than from
   the brass lantern." then RMUNGBIT and "A burned-out lamp won't light.";
   C++ 330 turns, warnings at 50/30/10 with "The lamp is getting dim.",
   "The lamp has gone out.", CAPACITY 0, "The lamp has no more power.".
   lampAction (actions.cpp:1073) handles LAMP-ON itself ("The lamp is now
   on.") and returns before enableLampTimer, so I-LANTERN never runs in
   play; THROW (broken lamp) absent. Candles: ZIL 40+20+10+5 = 75 turns,
   kept with RMUNGBIT; C++ 50 wax, invented warnings, object removed;
   CANDLES-FCN omits first-touch ENABLE I-CANDLES, RFATAL, " It's really
   dark in here....", "That wouldn't be smart.", says "(with the
   matchbook)" for "(with the match)". Match: ZIL MATCH-COUNT 6 reporting
   count-1, drafty-room refusal in LOWER-SHAFT/TIMBER-ROOM, I-MATCH 2 "The
   match has gone out.", "One of the matches starts to burn.", "I'm afraid
   that you have run out of matches."; C++ starts at 5, "One of the
   matches strikes and burns with a bright flame.", "You're out of
   matches.", no I-MATCH, no drafty check. Sword: ZIL three glow states via
   TVALUE with "Your sword has begun to glow very brightly." / "Your sword
   is glowing with a faint blue glow." / "Your sword is no longer
   glowing.", enabled on TAKE by ADVENTURER, self-disabling; C++ two states
   keyed on FIGHTBIT (misjudges invisible thief and sleeping cyclops),
   "faint blue light." / "Your sword stops glowing.", enabled at startup,
   EXAMINE invented.
8. Save/restore/quit/restart: vSave prompts "Enter save filename:", "Game
   saved." vs "Ok."/"Failed."; stores four properties and no globals
   (flags, deaths, wrath, timers, TVALUEs), so restore loses puzzle state;
   complete SaveSystem never called. vRestore "Game restored." + vLook vs
   "Ok." + V-FIRST-LOOK. vRestart text invented. FINISH drops "position",
   wraps differently, no loop on bad input.
9. V-SCORE ranks off by one (>=330 vs G? 330). V-DIAGNOSE fully invented.
   V-ATTACK omits the PRSI=HANDS bare-hands case and re-runs the PRSO
   action after PERFORM already did.
10. Grue: checkGrue (light.cpp:138) dead; if wired would add a turn-count
    death with no ZIL basis.

Missing entirely: melee tables/engine, I-CURE, AWAKEN, all thief routines,
THIEF-IN-TREASURE, TREASURE-ROOM-FCN, TROLL-ROOM-F, LIGHT-INT, I-MATCH,
STOLE-LIGHT?, ROBBER-C/U-DESC, DEF-RES patch, TROLL-FLAG on deaths.

## 6. Object and room actions (src/world/actions*.cpp, pseudo_actions.cpp, world_init.cpp)

Full 117-row table: scratchpad/phase2_actions_table.md. Tally of 117 ZIL
routines: 27 faithful and wired, 21 faithful but never registered, 34
divergent, 30 missing, 5 stubs.

Wiring map (verified by grepping every `.action =`, `setAction`,
`setRoomAction`):
- Faithful but unwired (`actions_group_a.cpp` with `// Source:` comments):
  CELLAR-FCN, CLEARING-FCN, DAM-ROOM-FCN, DOME-ROOM-FCN, RUSTY-KNIFE-FCN,
  KNIFE-F, TOOL-CHEST-FCN, PUTTY-FCN, TUBE-FUNCTION, RAINBOW-FCN, GARLIC-F,
  all ten *-PSEUDO routines (PSEUDO-OBJECT's action is cretinAction,
  special_objects.cpp:237), BAT-F. Unit tests call these directly, so they
  pass while the game never runs them.
- Wired but replaced by invented code in `actions.cpp`: RBOAT-FUNCTION
  (boatAction 1618), MACHINE-F (1840), BOLT-F (2031), BUTTON-F (2119),
  TORCH-OBJECT (1241), SWORD-FCN (1051), BASKET-F (557), TRAP-DOOR-FCN
  (group_a:2004), RUG-FCN (936), KITCHEN-WINDOW-F (982), CHIMNEY-F
  (group_a:210), SLIDE-FUNCTION (group_a:1891), SAND-FUNCTION
  (group_a:1857), SONGBIRD-F (stub).
- Rooms registered in world_init.cpp as static-text lambdas, so the ZIL
  room function is absent in play: LIVING-ROOM (408, always "nailed shut
  ... oriental rug"), KITCHEN (439, window always "open."), CELLAR (496,
  no trap-door slam), TROLL-ROOM (583), LOUD-ROOM (739), DAM-ROOM (1307),
  MAINTENANCE-ROOM, reservoirs (1185/1214/1239), GRATING-ROOM (1827),
  CYCLOPS-ROOM (2097), TREASURE-ROOM (2179, "You enter the treasure room
  cautiously."), DOME-ROOM (2241), TORCH-ROOM (2264, "elvish hacking
  rites"), NORTH/SOUTH-TEMPLE (2286/2312), MACHINE-ROOM (1076, text from
  another game), GRATING-CLEARING (306).

Critical:
1. Room actions cannot return a value (rooms.h:300, gmain.cpp:322-327), so
   every M-BEG interception (KITCHEN-FCN, TREE-ROOM, STONE-BARROW-FCN,
   NO-OBJS, SOUTH-TEMPLE-FCN, RBOAT WALK/LAUNCH, CANYON-VIEW-F) falls
   through to the default verb handler.
2. STONE-BARROW-FCN ending missing (actions.cpp:484 prints LDESC only):
   "Inside the Barrow", the Tandy-bit check (ZIL 419) and FINISH have no
   counterpart in src (0 grep hits). The game cannot be won as in ZIL.
3. DEAD-FUNCTION PRAY branch missing (group_a:561 vs ZIL 3152-3169): no
   resurrection at SOUTH-TEMPLE, no "Your prayers are not heard.", no
   catch-all "You can't even do that." + RFATAL.
4. Dam/reservoir subsystem absent: I-RFILL, I-REMPTY, I-MAINT-ROOM,
   DROWNINGS, WITH-TELL, RESERVOIR-*-FCN texts (0 hits for "water level
   here is now", "done drowned", "lifts gently", "no longer stay afloat");
   boltAction TODO at actions.cpp:2078/2083; blue button never reveals LEAK
   nor queues I-MAINT-ROOM (2152-2153).
5. River subsystem absent: RIVER-SPEEDS/NEXT/LAUNCH, I-RIVER, LAUNCH (TODO
   group_a:1120); puncture-in-water prints the death text but no JIGS-UP
   (1163, 1170); wired boatAction invented ("You are now in the boat.",
   "You need a pump to inflate the boat.").
6. Rainbow/sceptre puzzle unsolvable: SCEPTRE-FUNCTION stub (group_a:1880)
   and unwired; RAINBOW-FCN unwired; WALL-dump quirk absent.
7. NO-OBJS missing: EMPTY-HANDED never set, LIGHT-SHAFT 13 points never
   awarded, so 350 is unattainable.
8. No counterpart at all: LEAF-PILE, LEAVES-APPEAR, SKELETON,
   TREASURE-ROOM-FCN, THIEF-IN-TREASURE, LOUD-ROOM READ loop, BAT-D,
   WCLIF-OBJECT, CLIFF-OBJECT, WHITE-CLIFFS-FUNCTION, DUMB-CONTAINER,
   TOUCH-ALL, OTVAL-FROB.

High (deliberate ZIL quirks not reproduced):
9. KITCHEN-FCN: both C++ variants (group_a:1337-1340, 2249-2251) print
   "There are no stairs leading down." on CLIMB-DOWN; ZIL 400 makes it
   unreachable.
10. TREE-ROOM: LEAP hoisted to top level (actions.cpp:257) vs nested under
    DROP (ZIL 2914); dropped nest moved to PATH (277) vs left in the tree
    (2898-2903); M-LOOK lists PATH contents when non-empty (235) vs only
    when at least two objects (2885-2886).
11. BELL-F: ZIL 345 compares HERE with the routine LLD-ROOM so "Ding,
    dong." always prints; C++ (group_a:2170) tests LAND_OF_LIVING_DEAD.
12. FWEEP prints N lines (group_a:2095) vs N-1 (ZIL 326-330); FLY-ME uses
    std::rand (2112) instead of PICK-ONE over BAT-DROPS.
13. LEAK-FUNCTION PLUG with non-putty: WITH-TELL "With a X?" (ZIL 1370) vs
    the DAM text (group_a:1445) or invented "? You must be joking."
    (leakAction:1490).

Medium (text/state in wired code):
14. CHIMNEY-F "leads down ward" (group_a:217) vs "downward" (ZIL 547-551);
    invented climb texts (231, 266, 271, 277).
15. TRAP-DOOR-FCN texts (group_a:2009, 2019) vs ZIL 511-513 "The door
    reluctantly opens to reveal a rickety staircase descending into
    darkness." / "The door swings shut and closes."; cellar-side lock
    texts, RAISE, LOOK-UNDER missing.
16. BASKET-F "already at the top/bottom" vs PICK-ONE DUMMY; "The basket is
    at the other end of the chain." missing; LIT via room ONBIT not LIT?.
17. MOUNTAIN-RANGE-F "The mountains are impassable." vs "Don't you believe
    me? The mountains are impassable!"; CLIMB-FOO missing.
18. BOOM-ROOM banner 4 spaces (actions.cpp:137) vs 6 (ZIL 2467).
    DEEP-CANYON-F prints " You can hear ..." on a new line (group_a:684,
    689) vs same line. MIRROR-MIRROR, TREE-ROOM, inflatedBoatAction keep
    source line breaks as newlines (actions.cpp:1980, 269, 1151).
19. GRATE-FUNCTION: OPEN/CLOSE/PICK/PUT branches missing (ZIL 870-896);
    uses CLEARING where ZIL uses GRATING-CLEARING (group_a:889, 902).
20. EGG-OBJECT: no MUNG/HATCH, HANDS test uses ADVENTURER (actions.cpp:1457),
    second-attempt FIGHTBIT branch missing (ZIL 2937-2945).
21. HOT-BELL-F POUR-ON never prints "The bell appears to have cooled down."
    (no QUEUE I-XBH 0, group_a:970-989).
22. IBOAT-FUNCTION (group_a:470) omits tan-label TOUCHBIT, SETG DEFLATE,
    THIS-IS-IT; duplicate iboatFunction (1003) differs again.
23. ROPE-FUNCTION: CLIMB-DOWN returns true without DO-WALK (actions.cpp:1182);
    TIE-UP/UNTIE/DROP-in-dome/TAKE-when-tied absent.
24. LIVING-ROOM-FCN / LOUD-ROOM-FCN / KITCHEN-FCN(int) test rarg == 0/1/2
    for M_LOOK/M_BEG/M_END, but types.h:23-28 defines M_BEG=1, M_ENTER=2,
    M_LOOK=3, M_END=6. Broken even if wired.
25. Extra verbs with no ZIL basis: TEETH-F EXAMINE/TAKE, GRANITE-WALL-F
    EXAMINE/MOVE, CRACK-FCN LOOK-INSIDE, BOARDED-WINDOW ATTACK/KILL,
    BARROW-FCN ENTER, TROPHY-CASE PUT scoring, KITCHEN-WINDOW "already
    open/closed", STUPID-CONTAINER PUT-ON.

Faithful and wired: WEST-HOUSE, EAST-HOUSE, BOARD-F, GHOSTS-F,
BARROW-DOOR-FCN, BUBBLE-F, DAM-FUNCTION, BOTTLE-FUNCTION, FRONT-DOOR-FCN,
GUNK-FUNCTION, CAVE2-ROOM (prob(50,true) correct), BATS-ROOM, FALLS-ROOM,
RIVR4-ROOM, MIRROR-ROOM, LLD-ROOM + I-XB/I-XC/I-XBH, FOREST-ROOM,
CANARY-OBJECT, BAG-OF-COINS-F, DBOAT-FUNCTION, UP-CHIMNEY-FUNCTION,
TRAP-DOOR-EXIT, MAZE-DIODES, CANYON-VIEW-F.

### docs/zil_actions.md
The "114/114 (100%)" claim does not hold. 55 items marked [x] are not
faithful in the running game (list in the actions table file; includes
STONE-BARROW-FCN, SCEPTRE-FUNCTION, RBOAT-FUNCTION, TREASURE-ROOM-FCN,
DEAD-FUNCTION, KITCHEN-FCN, TREE-ROOM, all ten *-PSEUDO). 27 routines in
this slice are absent from the checklist entirely (OPEN-CLOSE,
LEAVES-APPEAR, LEAF-PILE, SKELETON, TORCH-OBJECT, TOUCH-ALL, OTVAL-FROB,
I-RFILL, I-REMPTY, I-MAINT-ROOM, WITH-TELL, MOVE-ALL, THIEF-IN-TREASURE,
BAT-D, NO-OBJS, WHITE-CLIFFS-FUNCTION, I-RIVER, WCLIF-OBJECT, CLIFF-OBJECT,
SLIDER, DUMB-CONTAINER, FIX-BOAT, FIX-MAINT-LEAK, INTEGRAL-PART, GO-NEXT,
FLY-ME, FWEEP).

## 7. World data (src/world/world_init.cpp vs 1dungeon.zil, gglobals.zil)

Method: the live init sequence (main.cpp -> initializeWorld() in
world_init.cpp) was dumped from a scratchpad build and diffed field by
field against an MDL parse of all 250 ZIL ROOM/OBJECT forms. Full report:
scratchpad/world_data_audit.md; complete 651-line mismatch list:
scratchpad/world_data_mismatches_full.txt. Note: the builder-style
initializeAllObjects()/initializeAllRooms() in rooms_*.cpp, treasures.cpp,
containers.cpp, scenery.cpp, npcs.cpp, tools.cpp, readables.cpp,
special_objects.cpp are never called; they are dead duplicates with their
own divergent data.

1. Counts: ZIL 110 rooms + 140 objects; C++ 110 rooms + 137 objects.
   Missing: BAT, KITCHEN-TABLE, ATTIC-TABLE, LEAK, MACHINE-SWITCH,
   CONTROL-PANEL (actions_group_a.cpp:1356 looks up ATTIC_TABLE and gets
   null). No ZIL basis: id 100 "trophy" (TAKEBIT, VALUE 5/TVALUE 5 in
   TREASURE-ROOM, a phantom 10-point treasure), id 108 "bag of coins"
   duplicating id 743 (743 has the handler but no location and TVALUE 0),
   id 502 "window".
2. SIZE: ZIL PROPDEF default 5; getProperty returns 0 when unset
   (object.cpp:11), so 73 objects weigh 0; 23 explicit wrong values (AXE
   25->15, BOOK 10->5, GARLIC 4->2, STILETTO 10->5, GUNK 10->0,
   INFLATABLE-BOAT 20->0, INFLATED-BOAT 20->50, PUNCTURED-BOAT 20->30,
   invented KNIFE 10, SKULL 15, MACHINE 100, LARGE-BAG 20, BOTTLE 8).
   STRENGTH: TROLL 8, CYCLOPS 10, ADVENTURER 10 (ZIL 2/10000/0) and
   STRENGTH repurposed as damage/fuel on AXE 7, KNIFE 5, SWORD 10, STILETTO
   4, TORCH 30, CANDLES 50, LAMP 100, MATCH 5. CAPACITY: LAMP 330,
   LARGE-BAG 100, TOOL-CHEST 40 (ZIL 0); MACHINE 20 (ZIL 50). Room VALUE:
   KITCHEN 10, CELLAR 25, EW-PASSAGE 5 are 0.
3. Initial locations (28 wrong): nowhere at start ALTAR, BOOK, GUIDE, MAP,
   OWNERS-MANUAL, PEDESTAL (and thus TORCH), TOOL-CHEST, BAG-OF-COINS 743,
   BOAT-LABEL, BUBBLE, KNIFE, FRONT-DOOR, MOUNTAIN-RANGE (world_init.cpp
   2519-2575 say "TODO: Set proper location"). Wrong room: BOLT in
   MAINTENANCE-ROOM (ZIL DAM-ROOM); all four buttons in MACHINE-ROOM (ZIL
   MAINTENANCE-ROOM, 3594-3621); AXE in TROLL-ROOM not in TROLL; EGG loose
   in UP-A-TREE not in NEST; BOTTLE/SACK on the floor not on the table;
   GUNK present from turn 1 without TAKEBIT; KITCHEN-WINDOW and RAINBOW in
   one room instead of LOCAL-GLOBALS; 20 LOCAL/GLOBAL objects have no
   location.
4. Flags: missing TRYTAKEBIT on CHALICE, KNIFE, ROPE, SWORD, WATER, CYCLOPS,
   THIEF, TROLL, STILETTO, AXE, LARGE-BAG; THIEF lacks INVISIBLE/CONTBIT/
   OPENBIT; TROLL lacks OPENBIT; BOOK lacks CONTBIT/TURNBIT; MAP lacks
   INVISIBLE; WATER lacks DRINKBIT; MATCH lacks READBIT; RAINBOW lacks
   CLIMBBIT; boats/label lack TAKEBIT/BURNBIT; buttons/BOLT/BUBBLE/MACHINE/
   CYCLOPS lack NDESCBIT; DEAD-END-1..4 lack MAZEBIT; RESERVOIR and
   IN-STREAM lack NONLANDBIT; ARAGAIN-FALLS and END-OF-RAINBOW lack ONBIT.
   Extra: SACREDBIT on CELLAR/GALLERY/STUDIO/END-OF-RAINBOW, ONBIT on
   ATTIC/STUDIO/RIVER-2..4/SANDY-BEACH, FIGHTBIT preset on all villains,
   OPENBIT on TROPHY-CASE and SACK, WEAPONBIT on TRIDENT, TOOLBIT on AXE.
   Correct: BARROW-DOOR OPENBIT, CHALICE TAKEBIT.
5. Exits: KITCHEN has a NORTH->GALLERY exit; RESERVOIR-SOUTH scrambled
   (SOUTH->DEEP-CANYON, SE->DAM-LOBBY, no EAST->DAM-ROOM; ZIL SE->
   DEEP-CANYON, EAST->DAM-ROOM); CANYON-VIEW EAST blocked instead of ->
   CLIFF-MIDDLE; GRATING-CLEARING EAST->CANYON-VIEW instead of FOREST-2
   plus spurious SW; CYCLOPS-ROOM extra NORTH refusal and WEST->MAZE-15;
   STRANGE-PASSAGE extra NORTH/OUT/SOUTH; EAST-OF-HOUSE extra NE/SE;
   IN-STREAM missing LAND->STREAM-VIEW; invented LAUNCH pseudo-exits and a
   RIVER-5 DOWN "over the falls"; CELLAR WEST refusal rewritten. Verified
   correct: 9 self-loops, 4 MAZE-DIODES PER exits, GRATING/TRAP-DOOR/
   CHIMNEY PER exits, KITCHEN DOWN FALSE-FLAG text, RIVER-1, walk-around
   tables, SCORE_MAX, all 11 conditional-exit flags. Room GLOBAL lists
   differ in 46 rooms: STAIRS missing from all 17 rooms that list it,
   FOREST/TREE/SONGBIRD/WHITE-HOUSE missing from forest rooms,
   GLOBAL-WATER/RIVER from water rooms, plus CRACK/LADDER/SLIDE/BODIES/
   BOARDED-WINDOW/CLIMBABLE-CLIFF. PSEUDO clauses not represented.
6. ACTION wiring: 25 objects with a ZIL ACTION have no C++ handler (AXE,
   BARROW, BODIES, BONES, BOOK, CHALICE, CLIMBABLE-CLIFF, GARLIC, KNIFE,
   LARGE-BAG, LEAVES, PEDESTAL, PUTTY, RAINBOW, RIVER, RUSTY-KNIFE, SAND,
   SCEPTRE, STILETTO, TOOL-CHEST, TRUNK, TUBE, WATER, WHITE-CLIFF,
   WOODEN-DOOR); WHITE-CLIFFS-NORTH/SOUTH lack WHITE-CLIFFS-FUNCTION.
7. Text: ZObject has no FDESC slot; 22 objects lose their first
   description, 4 keep only LDESC, several stash FDESC in TEXT (EGG,
   CANARY, BUOY, RUSTY-KNIFE, PAINTING), making them "readable".
   Fabricated TEXT: BOAT-LABEL ("Model: FMB-1, Serial Number: 123456789"),
   OWNERS-MANUAL (a boat warranty), MAP ("complex maze of twisty
   passages"); MATCH text missing; GUIDE drops the "1) You start your tour
   here in the Dam Lobby..." paragraph; ADVERTISEMENT hard-wraps; BOOK
   quote/spacing differ. Wording: GALLERY adds "which were here",
   STRANGE-PASSAGE "hole" vs "opening", MAZE-5 drops the skeleton
   sentence, CANYON-VIEW drops the climb-down sentence, TROLL-ROOM "Troll
   Room" vs "The Troll Room". DESC renames: cretin->adventurer,
   chalice->silver chalice, torch->ivory torch, ancient map->map,
   door->front door, carpet->rug, lurking grue->grue, beautiful jeweled
   scarab->jewel-encrusted scarab, green bubble->bubble, small piece of
   vitreous slag->vitreous slag, tan label->label, ZORK owner's manual->
   owner's manual, leather bag of coins->bag of coins. Correct: WINDING/
   TWISTING shared LDESC, "nondescript" mine text.
8. Vocabulary: 6-char truncations inconsistent (WHITE-HOUSE has BEAUTI/
   COLONI plus full words; PAINTING/SCARAB/BAUBLE only BEAUTIFUL; DAM lacks
   FCD#3, GATE, GATES); missing REFLECTION/ENORMOUS on mirrors, PRAYER/PAGE
   on BOOK, SWITCH on buttons, PDP10/DRYER/LID on MACHINE, TOOLS, H2O/
   LIQUID, PARCHMENT, AX, EYE; invented GOBLET, GRAIL, TOME, CHART,
   BURGLAR, RAFT. ADVENTURER carries ME/MYSELF/SELF (ZIL: on ME).

## 8. Tests and documentation

- 74/74 ctest targets pass. tests/gparser_tests.cpp, gsyntax_tests.cpp and
  the per-action tests call the unwired ZIL-faithful functions directly,
  so they certify code the game never executes.
- docs/zil_actions.md "114/114 (100%)" is not accurate: 55 [x] items are
  not faithful in play and 27 routines are absent from the checklist.
- README/PROJECT_STATUS claims ("complete", "all puzzles solvable") are
  contradicted by sections 5-7: the barrow ending, rainbow, dam/reservoir,
  river, thief and treasure-room mechanics are missing, and the 13-point
  lit-shaft bonus is never awarded.

## 9. Ranked summary

Structural (fix these first; everything else depends on them):
1. Two parallel implementations; the faithful layer (gsyntax/gparser, verbs
   tail, actions_group_a, npcs.cpp, save.cpp, builder-style world files) is
   dead. Decide which layer is canonical and delete or wire the other.
2. Room actions return void and direction moves bypass PERFORM, so no
   M-BEG interception works and WALK never reaches WINNER/room hooks.
3. RFATAL (value 2) cannot propagate through bool handlers.
4. Object data model lacks FDESC, VTYPE-as-property, and SIZE default 5;
   getProperty returns 0 for unset SIZE/CAPACITY.

Behavioural (ZIL mechanics absent or rewritten):
5. Melee engine and all 96 melee strings, wounds, I-CURE, LOAD-ALLOWED.
6. Thief, troll, cyclops behaviour; treasure-room; villain deaths.
7. Parser: vocabulary subset, no clause/punctuation handling, no multi-
   object lists, no GWIM/orphan-merge, invented messages.
8. Death rules (second vs third death), KILL-INTERRUPTS set, resurrection
   prompts.
9. Lamp/candle/match/sword tables and messages; I-LANTERN never enabled.
10. Dam/reservoir, river, rainbow/sceptre, barrow ending, NO-OBJS bonus.
11. Multi-object loop, IT, MOVES counting, CLOCKER order, startup queue.
12. World data: 28 initial locations, exits, flags, sizes, strengths,
    globals lists, missing tables/leak/panel/bat, phantom "trophy".

Quirk-fidelity (ZIL bugs that must be reproduced but are not):
13. Kitchen "no stairs leading down" reachable; tree LEAP hoisted; BELL-F
    LLD-ROOM comparison; FWEEP N vs N-1; leaf pile; DROWNINGS 7-8;
    ITAKE/V-PUT RFATAL truthiness; rank thresholds off by one; fixed "a "
    article replaced by A/AN logic; "Ahoy --" kept but other split TELLs
    reflowed by the output layer; NOT-HERE-PRINT spacing.

Text (hundreds of strings differ; lists in the section files): every
invented message and every renamed DESC changes transcripts against the
R119 story file.
