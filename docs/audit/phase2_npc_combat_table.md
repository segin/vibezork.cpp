# Phase 2 slice: NPCs / melee / death / light interrupts / save — ZIL routine -> C++ status

Legend: OK = behaviour and text match; PARTIAL = some verbs/strings match, rest differ or missing; REWRITE = C++ exists but is an invented mechanic with no ZIL basis; MISSING = no C++ counterpart; DEAD = C++ code exists but is never called.

| ZIL routine (1actions.zil unless noted) | C++ location | Status |
|---|---|---|
| AXE-F 622-624 | actions_group_a.cpp:133 axeAction | PARTIAL: tests troll NDESCBIT instead of TROLL-FLAG |
| STILETTO-FUNCTION 626-627 | actions_group_a.cpp:1918 stilettoAction | REWRITE: invented text, no WEAPON-FUNCTION |
| WEAPON-FUNCTION 629-638 | actions_group_a.cpp:102 weaponFunction | OK (used by axe only) |
| TROLL-FCN 640-769 (all 6 modes) | npc.cpp:803 trollAction (verb mode only) | REWRITE: EXAMINE fixed text; GIVE/THROW/TAKE/MOVE/MUNG/LISTEN/HELLO/TELL logic absent; F-BUSY?/F-DEAD/F-UNCONSCIOUS/F-CONSCIOUS/F-FIRST? modes absent |
| TROLL-ROOM exits (1dungeon 1487-1490) | world_init.cpp:589-601 | OK text; condition uses NPC state not TROLL-FLAG alone |
| TROLL-ROOM-F 4160-4177 | actions_group_a.cpp:2034 trollRoomAction | MISSING (empty stub) |
| CYCLOPS-FCN 1515-1613 | npc.cpp:1171 cyclopsAction | PARTIAL: EXAMINE/TAKE/TIE/LISTEN/sleep texts OK; GIVE WATER prints two invented lines; GIVE LUNCH lacks I-CYCLOPS enable and <MIN -1 -COUNT>; THROW/ATTACK/MUNG go to invented cyclopsCombat; WINNER==CYCLOPS branch absent |
| I-CYCLOPS 1616-1632 | npc.cpp:1115 processCyclopsTurn (called every turn from gmain.cpp:476) | REWRITE: wrath grows 30% after 3 turns instead of +1 per tick; only 4 of 6 CYCLOMAD lines; no JIGS-UP at |wrath|>5 (TODO) |
| CYCLOPS-ROOM-FCN 1634-1660 | world_init.cpp:2097 lambda; actions_group_a.cpp:349 (dead duplicate) | PARTIAL: M-LOOK text OK, ordering of sleep/fled check swapped vs ZIL (ZIL: sleep&&!magic first); M-ENTER does nothing |
| CYCLOMAD 1662-1669 | npc.cpp:857 | PARTIAL: 4 of 6 entries |
| V-ODYSSEUS (gverbs 945-961) | verbs.cpp:2752 -> npc.cpp:1303 handleOdysseus/cyclopsFlee | PARTIAL: text OK, MAGIC-FLAG not set (g.magicFlag), CYCLOPS-FLAG not set, I-CYCLOPS not disabled; cyclopsAction V_ODYSSEUS branch prints a different invented line |
| THIEF-VS-ADVENTURER 1764-1868 | none | MISSING |
| STOLE-LIGHT? 1870-1875 | none | MISSING |
| HACK-TREASURES 1886-1893 | none | MISSING |
| DEPOSIT-BOOTY 1895-1908 | npc.cpp:440 thiefTreasureRoom | REWRITE (dumps everything incl. non-treasure, invented text) |
| ROB-MAZE 1917-1932 | none | MISSING |
| ROBBER-FUNCTION 1947-2071 | actions_group_a.cpp:1807 robberAction (stub) + world_init.cpp:3730 lambda | REWRITE: EXAMINE/GIVE texts invented; TELL/TAKE/LISTEN/HELLO/THROW-KNIFE/unconscious handling absent; F-* modes absent |
| ROBBER-C-DESC / ROBBER-U-DESC 2073-2079 | none | MISSING |
| LARGE-BAG-F 2081-2097 | actions_group_a.cpp:1366 | PARTIAL: TAKE lacks unconscious variant; other three verbs OK |
| CHALICE-FCN 2115-2128 | actions_group_a.cpp:2054 | PARTIAL: lacks ROBBER-U-DESC test and DUMB-CONTAINER fallthrough |
| TREASURE-ROOM-FCN 2130-2140 | world_init.cpp:2179 lambda | MISSING (prints invented "You enter the treasure room cautiously.") |
| THIEF-IN-TREASURE 2142-2151 | none | MISSING |
| LAMP-TABLE 2216-2224 | lamp.cpp:52-62 | REWRITE: 330 turns, warnings at 50/30/10 with invented text |
| LANTERN 2226-2251 | actions.cpp:1073 lampAction | REWRITE: no THROW, no RMUNGBIT, invented EXAMINE/ON/OFF text; never enables I-LANTERN |
| MATCH-FUNCTION 2255-2295 | actions.cpp:653 matchesAction | REWRITE: count off by one, no drafty-room check, no I-MATCH, invented text |
| I-MATCH 2297-2302 | none | MISSING |
| I-LANTERN 2304-2309 | lamp.cpp:11 lampTimerCallback | REWRITE |
| I-CANDLES 2311-2316 | candle.cpp:11 candleTimerCallback | REWRITE: 50 wax, warnings 10/5/2 invented, candles removed from game |
| LIGHT-INT 2318-2327 | none | MISSING |
| CANDLES-FCN 2334-2392 | actions.cpp:1302 candlesAction | PARTIAL: most texts OK; TOUCHBIT/enable-on-first-touch, PUT-in-burnable, RFATAL, darkness message absent; "(with the matchbook)" vs "(with the match)" |
| CANDLE-TABLE 2394-2402 | candle.cpp | REWRITE |
| SWORD-FCN 2418-2429 | actions.cpp:1051 swordAction | REWRITE: TAKE does not enable I-SWORD; EXAMINE text invented |
| I-SWORD 3851-3879 + INFESTED? 3881-3887 | sword.cpp:15 swordTimerCallback | REWRITE: two levels not three, FIGHTBIT instead of ACTORBIT/INVISIBLE, exit-type filter absent, texts differ |
| F-* / blow-result constants 3236-3260 | none | MISSING |
| DEF1..DEF3C, DEF*-RES 3262-3317 (+GO patch) | none | MISSING |
| STRENGTH-MAX/MIN, CURE-WAIT 3321-3325 | none | MISSING |
| DO-FIGHT 3331-3357 | combat.cpp:94 processCombatRound | REWRITE |
| REMARK 3363-3371 | none | MISSING |
| FIGHT-STRENGTH 3376-3382 | none (Combatant.strength = P_STRENGTH or 5) | MISSING |
| VILLAIN-STRENGTH 3384-3398 | none | MISSING (sword/knife advantage, THIEF-ENGROSSED) |
| FIND-WEAPON 3402-3411 | combat.cpp:45 (first WEAPONBIT) | PARTIAL |
| VILLAIN-BLOW 3413-3474 | combat.cpp:146-165 | REWRITE |
| HERO-BLOW 3476-3549 | combat.cpp:116-136; npc.cpp thiefCombat/trollCombat/cyclopsCombat | REWRITE |
| WINNER-RESULT 3553-3563 | none | MISSING |
| VILLAIN-RESULT 3565-3579 | combat.cpp:274 handleDeath | PARTIAL: fog text OK; no F-DEAD/F-UNCONSCIOUS dispatch |
| WINNING? 3583-3590 | combat.cpp:245 shouldEnemyFlee | REWRITE |
| I-CURE 3592-3602 | none | MISSING |
| HERO-MELEE / CYCLOPS-MELEE / TROLL-MELEE / THIEF-MELEE 3612-3785 | none | MISSING (0 of 96 strings present) |
| VILLAINS 3800-3803 | none | MISSING |
| I-FIGHT 3810-3843 | combat.cpp:58 "I-FIGHT" timer (only while startCombat active) | REWRITE |
| AWAKEN 3845-3849 | none | MISSING |
| I-THIEF 3890-3934 | npc.cpp:527 processThiefTurn via 4-turn timer | REWRITE |
| DROP-JUNK 3936-3951 | none | MISSING |
| RECOVER-STILETTO 3953-3956 | none | MISSING |
| STEAL-JUNK 3958-3977 | npc.cpp:255 thiefSteal | REWRITE |
| ROB 3979-3991 | npc.cpp:255 thiefSteal | REWRITE |
| V-DIAGNOSE 3993-4024 | verbs.cpp vDiagnose | REWRITE |
| V-SCORE 4026-4044 | verbs.cpp vScore | PARTIAL: rank thresholds use >= instead of > |
| JIGS-UP 4046-4099 | death.cpp:273 jigsUp | PARTIAL: see divergences 2-4 |
| RANDOMIZE-OBJECTS 4101-4123 | death.cpp:60 randomizeObjects | PARTIAL: approximated by id ranges |
| KILL-INTERRUPTS 4125-4135 | death.cpp:133 killInterrupts | PARTIAL: wrong set of interrupts |
| DEAD-FUNCTION 3113-3160 | actions_group_a.cpp:561 deadFunction | mostly OK (out of slice; not fully checked) |
| V-ATTACK (gverbs 176-190) | verbs.cpp:2047 vAttack | PARTIAL: HANDS check absent; HERO-BLOW replaced by startCombat |
| V-SAVE / V-RESTORE (gverbs 71-83) | verbs.cpp vSave/vRestore; save.cpp SaveSystem (DEAD) | REWRITE: prompts, texts, incomplete state |
| V-QUIT (gverbs 55-61) | verbs.cpp:354 vQuit | OK text; YES? prompt ">" not printed |
| V-RESTART (gverbs 63-69) | verbs.cpp vRestart | REWRITE text |
| FINISH (gverbs 33-53) | verbs.cpp finish | PARTIAL: text differs, no loop |
| CRETIN-FCN ATTACK ME (gglobals 237-241) | gglobals.cpp:195 | OK |
| GO interrupt setup (1dungeon 2638-2649) | main.cpp:17-26 + system initialize() | REWRITE: I-SWORD enabled at start (ZIL: queued, disabled); I-FIGHT/I-THIEF not queued as -1 demons; DEF-RES patch absent |
| Dead C++ with no ZIL basis | npcs.cpp initializeNPCs (never called); light.cpp checkGrue/updateLighting (never called); save.cpp SaveSystem (never called); actions_group_a.cpp cyclopsRoomAction/treasureRoomAction/robberAction (shadowed by world_init lambdas or stubs) | DEAD |
