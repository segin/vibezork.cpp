#include "gmain.h"
#include "gglobals.h"
#include "globals.h"
#include "io.h"
#include "parser/gparser.h"
#include "parser/parser.h"
#include "world/objects.h"
#include "systems/npc.h"
#include "systems/score.h"
#include "systems/timer.h"
#include "verbs/verbs.h"
#include "world/rooms.h"
#include <format>
#include <iostream>
#include <print>
#include <unordered_map>

// ZIL: ACTIONS table mapping
static std::unordered_map<VerbId, ActionHandler> verbHandlers_;

// ZIL: PREACTIONS table mapping
static std::unordered_map<VerbId, ActionHandler> preactions_;

void registerVerbHandler(VerbId verb, ActionHandler handler) {
  verbHandlers_[verb] = std::move(handler);
}

void registerPreaction(VerbId verb, ActionHandler handler) {
  preactions_[verb] = std::move(handler);
}

// ZIL: <GET ,ACTIONS .A> / <GET ,PREACTIONS .A> yield 0 when no routine is
// stored; a null handler registered for a verb counts as "no routine".
bool hasVerbHandler(VerbId verb) {
  auto it = verbHandlers_.find(verb);
  return it != verbHandlers_.end() && static_cast<bool>(it->second);
}

bool hasPreaction(VerbId verb) {
  auto it = preactions_.find(verb);
  return it != preactions_.end() && static_cast<bool>(it->second);
}

ActionHandler getVerbHandler(VerbId verb) {
  auto it = verbHandlers_.find(verb);
  return it != verbHandlers_.end() ? it->second : nullptr;
}

ActionHandler getPreaction(VerbId verb) {
  auto it = preactions_.find(verb);
  return it != preactions_.end() ? it->second : nullptr;
}

void initializeAllVerbHandlers() {
  if (!verbHandlers_.empty()) {
    return;
  }

  // Meta-game commands
  registerVerbHandler(V_VERBOSE, Verbs::vVerbose);
  registerVerbHandler(V_BRIEF, Verbs::vBrief);
  registerVerbHandler(V_SUPERBRIEF, Verbs::vSuperbrief);
  registerVerbHandler(V_DIAGNOSE, Verbs::vDiagnose);
  registerVerbHandler(V_INVENTORY, Verbs::vInventory);
  registerVerbHandler(V_QUIT, Verbs::vQuit);
  registerVerbHandler(V_RESTART, Verbs::vRestart);
  registerVerbHandler(V_RESTORE, Verbs::vRestore);
  registerVerbHandler(V_SAVE, Verbs::vSave);
  registerVerbHandler(V_SCORE, Verbs::vScore);
  registerVerbHandler(V_VERSION, Verbs::vVersion);
  registerVerbHandler(V_SCRIPT, Verbs::vScript);
  registerVerbHandler(V_UNSCRIPT, Verbs::vUnscript);

  // Manipulation
  registerVerbHandler(V_TAKE, Verbs::vTake);
  registerVerbHandler(V_DROP, Verbs::vDrop);
  registerVerbHandler(V_PUT, Verbs::vPut);
  registerVerbHandler(V_PUT_ON, Verbs::vPutOn);
  registerVerbHandler(V_GIVE, Verbs::vGive);

  // Examination
  registerVerbHandler(V_LOOK, Verbs::vLook);
  registerVerbHandler(V_EXAMINE, Verbs::vExamine);
  registerVerbHandler(V_READ, Verbs::vRead);
  registerVerbHandler(V_LOOK_INSIDE, Verbs::vLookInside);
  registerVerbHandler(V_SEARCH, Verbs::vSearch);

  // Containers
  registerVerbHandler(V_OPEN, Verbs::vOpen);
  registerVerbHandler(V_CLOSE, Verbs::vClose);
  registerVerbHandler(V_LOCK, Verbs::vLock);
  registerVerbHandler(V_UNLOCK, Verbs::vUnlock);

  // Movement
  registerVerbHandler(V_WALK, Verbs::vWalk);
  registerVerbHandler(V_ENTER, Verbs::vEnter);
  registerVerbHandler(V_EXIT, Verbs::vExit);
  registerVerbHandler(V_CLIMB_UP, Verbs::vClimbUp);
  registerVerbHandler(V_CLIMB_DOWN, Verbs::vClimbDown);
  registerVerbHandler(V_CLIMB_ON, Verbs::vClimbUp);
  registerVerbHandler(V_BOARD, Verbs::vBoard);
  registerVerbHandler(V_DISEMBARK, Verbs::vDisembark);

  // Combat
  registerVerbHandler(V_ATTACK, Verbs::vAttack);
  registerVerbHandler(V_THROW, Verbs::vThrow);
  registerVerbHandler(V_SWING, Verbs::vSwing);

  // Light
  registerVerbHandler(V_LAMP_ON, Verbs::vLampOn);
  registerVerbHandler(V_LAMP_OFF, Verbs::vLampOff);

  // Manipulation
  registerVerbHandler(V_TURN, Verbs::vTurn);
  registerVerbHandler(V_PUSH, Verbs::vPush);
  registerVerbHandler(V_PULL, Verbs::vPull);
  registerVerbHandler(V_MOVE, Verbs::vMove);
  registerVerbHandler(V_RAISE, Verbs::vRaise);
  registerVerbHandler(V_LOWER, Verbs::vLower);
  registerVerbHandler(V_WIND, Verbs::vWind);
  registerVerbHandler(V_MAKE, Verbs::vMake);
  registerVerbHandler(V_CUT, Verbs::vCut);
  registerVerbHandler(V_MELT, Verbs::vMelt);
  registerVerbHandler(V_PLAY, Verbs::vPlay);
  registerVerbHandler(V_PLUG, Verbs::vPlug);
  registerVerbHandler(V_POUR_ON, Verbs::vPourOn);
  registerVerbHandler(V_PUSH_TO, Verbs::vPushTo);
  registerVerbHandler(V_PUT_UNDER, Verbs::vPutUnder);
  registerVerbHandler(V_PUT_BEHIND, Verbs::vPutBehind);
  registerVerbHandler(V_SHAKE, Verbs::vShake);
  registerVerbHandler(V_SPIN, Verbs::vSpin);
  registerVerbHandler(V_SQUEEZE, Verbs::vSqueeze);
  registerVerbHandler(V_TIE_UP, Verbs::vTieUp);

  // Interaction
  registerVerbHandler(V_TIE, Verbs::vTie);
  registerVerbHandler(V_UNTIE, Verbs::vUntie);
  registerVerbHandler(V_LISTEN, Verbs::vListen);
  registerVerbHandler(V_SMELL, Verbs::vSmell);
  registerVerbHandler(V_TOUCH, Verbs::vTouch);
  registerVerbHandler(V_YELL, Verbs::vYell);
  registerVerbHandler(V_ANSWER, Verbs::vAnswer);
  registerVerbHandler(V_REPLY, Verbs::vReply);
  registerVerbHandler(V_COMMAND, Verbs::vCommand);
  registerVerbHandler(V_ECHO, Verbs::vEcho);
  registerVerbHandler(V_FOLLOW, Verbs::vFollow);
  registerVerbHandler(V_KISS, Verbs::vKiss);
  registerVerbHandler(V_MUMBLE, Verbs::vMumble);
  registerVerbHandler(V_REPENT, Verbs::vRepent);
  registerVerbHandler(V_SEND, Verbs::vSend);
  registerVerbHandler(V_WISH, Verbs::vWish);
  registerVerbHandler(V_SPRAY, Verbs::vSpray);

  // Consumption
  registerVerbHandler(V_EAT, Verbs::vEat);
  registerVerbHandler(V_DRINK, Verbs::vDrink);

  // Special actions
  registerVerbHandler(V_INFLATE, Verbs::vInflate);
  registerVerbHandler(V_DEFLATE, Verbs::vDeflate);
  registerVerbHandler(V_PRAY, Verbs::vPray);
  registerVerbHandler(V_EXORCISE, Verbs::vExorcise);
  registerVerbHandler(V_WAVE, Verbs::vWave);
  registerVerbHandler(V_RUB, Verbs::vRub);
  registerVerbHandler(V_RING, Verbs::vRing);
  registerVerbHandler(V_BURN, Verbs::vBurn);

  // Communication & Easter eggs
  // TALK TO and ASK are SYNTAX lines for V-TELL (gsyntax.zil:421, 484);
  // ZIL has no separate V-TALK or V-ASK routine.
  registerVerbHandler(V_TALK, Verbs::vTell);
  registerVerbHandler(V_ASK, Verbs::vTell);
  registerVerbHandler(V_TELL, Verbs::vTell);
  registerVerbHandler(V_ODYSSEUS, Verbs::vOdysseus);
  registerVerbHandler(V_HELLO, Verbs::vHello);
  registerVerbHandler(V_ZORK, Verbs::vZork);
  registerVerbHandler(V_PLUGH, Verbs::vPlugh);
  registerVerbHandler(V_FROBOZZ, Verbs::vFrobozz);

  // Additional common verbs
  registerVerbHandler(V_WAIT, Verbs::vWait);
  registerVerbHandler(V_SWIM, Verbs::vSwim);
  registerVerbHandler(V_BACK, Verbs::vBack);
  registerVerbHandler(V_JUMP, Verbs::vJump);
  registerVerbHandler(V_CURSE, Verbs::vCurse);
  registerVerbHandler(V_OVERBOARD, Verbs::vOverboard);
  registerVerbHandler(V_MUNG, Verbs::vMung);
  registerVerbHandler(V_WEAR, Verbs::vWear);
  registerVerbHandler(V_FIND, Verbs::vFind);
  registerVerbHandler(V_LEAP, Verbs::vLeap);
  registerVerbHandler(V_SAY, Verbs::vSay);
  registerVerbHandler(V_KICK, Verbs::vKick);
  registerVerbHandler(V_BREATHE, Verbs::vBreathe);
  registerVerbHandler(V_RAPE, Verbs::vRape);
  registerVerbHandler(V_CLIMB_FOO, Verbs::vClimbFoo);
  registerVerbHandler(V_THROUGH, Verbs::vThrough);
  registerVerbHandler(V_STAND, Verbs::vStand);
  registerVerbHandler(V_ALARM, Verbs::vAlarm);
  registerVerbHandler(V_WALK_AROUND, Verbs::vWalkAround);
  registerVerbHandler(V_WALK_TO, Verbs::vWalkTo);
  registerVerbHandler(V_LAUNCH, Verbs::vLaunch);
  registerVerbHandler(V_OIL, Verbs::vOil);
  registerVerbHandler(V_STAB, Verbs::vStab);
  registerVerbHandler(V_DRINK_FROM, Verbs::vDrinkFrom);
  registerVerbHandler(V_LOOK_UNDER, Verbs::vLookUnder);
  registerVerbHandler(V_LOOK_BEHIND, Verbs::vLookBehind);
  registerVerbHandler(V_FIRST_LOOK, Verbs::vFirstLook);
  registerVerbHandler(V_RANDOM, Verbs::vRandom);
  registerVerbHandler(V_RECORD, Verbs::vRecord);
  registerVerbHandler(V_UNRECORD, Verbs::vUnrecord);
  registerVerbHandler(V_VERIFY, Verbs::vVerify);
  registerVerbHandler(V_THROW_OFF, Verbs::vThrowOff);
  registerVerbHandler(V_HATCH, Verbs::vHatch);
  registerVerbHandler(V_KNOCK, Verbs::vKnock);
  registerVerbHandler(V_LEAVE, Verbs::vLeave);
  registerVerbHandler(V_LEAN_ON, Verbs::vLeanOn);
  registerVerbHandler(V_PUMP, Verbs::vPump);
  registerVerbHandler(V_STRIKE, Verbs::vStrike);
  registerVerbHandler(V_READ_PAGE, Verbs::vReadPage);
  registerVerbHandler(V_PICK, Verbs::vPick);
  registerVerbHandler(V_APPLY, Verbs::vApply);
  registerVerbHandler(V_LOOK_ON, Verbs::vLookOn);
  registerVerbHandler(V_SGIVE, Verbs::vSgive);
  registerVerbHandler(V_SKIP, Verbs::vSkip);
  registerVerbHandler(V_SSPRAY, Verbs::vSspray);
  registerVerbHandler(V_COMMAND_FILE, Verbs::vCommandFile);

  // Preactions (gverbs.zil)
  registerVerbHandler(V_STAY, Verbs::vStay);
  registerVerbHandler(V_TREASURE, Verbs::vTreasure);
  registerVerbHandler(V_WIN, Verbs::vWin);
  registerVerbHandler(V_BLAST, Verbs::vBlast);
  registerVerbHandler(V_BRUSH, Verbs::vBrush);
  registerVerbHandler(V_BUG, Verbs::vBug);
  registerVerbHandler(V_CHOMP, Verbs::vChomp);
  registerVerbHandler(V_COUNT, Verbs::vCount);
  registerVerbHandler(V_CROSS, Verbs::vCross);
  registerVerbHandler(V_INCANT, Verbs::vIncant);
  registerVerbHandler(V_CHANT, Verbs::vChant);
  registerVerbHandler(V_DIG, Verbs::vDig);
  registerVerbHandler(V_FILL, Verbs::vFill);
  registerVerbHandler(V_CLIMB_ON, Verbs::vClimbOn);

  registerPreaction(V_BOARD, Verbs::preBoard);
  registerPreaction(V_BURN, Verbs::preBurn);
  registerPreaction(V_DROP, Verbs::preDrop);
  registerPreaction(V_FILL, Verbs::preFill);
  registerPreaction(V_GIVE, Verbs::preGive);
  registerPreaction(V_MOVE, Verbs::preMove);
  registerPreaction(V_MUNG, Verbs::preMung);
  registerPreaction(V_PUT, Verbs::prePut);
  registerPreaction(V_READ, Verbs::preRead);
  registerPreaction(V_SGIVE, Verbs::preSGive);
  registerPreaction(V_TAKE, Verbs::preTake);
  registerPreaction(V_TURN, Verbs::preTurn);
}

// ZIL: <DEFINE D-APPLY (STR FCN "OPTIONAL" FOO "AUX" RES) ...> (gmain.zil:290-308)
int dApply(std::string_view str, const std::function<int()>& fcn) {
  if (!fcn) {
    return M_NOT_HANDLED;
  }
  auto &g = Globals::instance();
  if (g.debug) {
    if (str.empty()) {
      std::println("\n  Default ->");
    } else {
      std::print("\n  {} -> ", str);
    }
  }
  int res = fcn();
  if (g.debug && !str.empty()) {
    if (res == M_FATAL) {
      std::println("Fatal");
    } else if (res == M_NOT_HANDLED) {
      std::println("Not handled");
    } else {
      std::println("Handled");
    }
  }
  return res;
}

// ZIL: <ROUTINE DD-APPLY (STR OBJ FCN "OPTIONAL" (FOO <>)) ...> (gmain.zil:309-312)
int ddApply(std::string_view str, ZObject *obj, const std::function<int()>& fcn) {
  auto &g = Globals::instance();
  if (g.debug && obj) {
    std::print("[{}=]", obj->getDesc());
  }
  return dApply(str, fcn);
}

// ZIL: <ROUTINE PERFORM (A "OPTIONAL" (O <>) (I <>) "AUX" V OA OO OI) ...> (gmain.zil:182-288)
int perform(VerbId a, ZObject *o, ZObject *i) {
  initializeAllVerbHandlers();
  auto &g = Globals::instance();

  if (g.debug) {
    std::print("** PERFORM: PRSA = {}", a);
    if (o && a != V_WALK) {
      std::print(" | PRSO = {}", o->getDesc());
    }
    if (i) {
      std::print(" | PRSI = {}", i->getDesc());
    }
    std::println();
  }

  // ZIL: Save previous PRSA, PRSO, PRSI
  VerbId oa = g.prsa;
  ZObject *oo = g.prso;
  ZObject *oi = g.prsi;

  // ZIL: <COND (<AND <EQUAL? ,IT .I .O> <NOT <ACCESSIBLE? ,P-IT-OBJECT>>>
  //             <TELL "I don't see what you are referring to." CR>
  //             <RFATAL>)> (gmain.zil:194-197)
  ZObject *itObj = g.getObject(ObjectIds::IT);
  if (itObj && (i == itObj || o == itObj) && !GParser::isAccessible(g.it)) {
    printLine("I don't see what you are referring to.");
    return M_FATAL;
  }

  // ZIL: <COND (<==? .O ,IT> <SET O ,P-IT-OBJECT>)>
  //      <COND (<==? .I ,IT> <SET I ,P-IT-OBJECT>)> (gmain.zil:198-199)
  if (itObj && o == itObj) {
    o = g.it;
  }
  if (itObj && i == itObj) {
    i = g.it;
  }

  // ZIL: <SETG PRSA .A> <SETG PRSO .O>
  g.prsa = a;
  g.prso = o;

  // ZIL: <COND (<AND ,PRSO <NOT <EQUAL? ,PRSI ,IT>> <NOT <VERB? WALK>>>
  //             <SETG P-IT-OBJECT ,PRSO>)> (gmain.zil:202-203)
  // Note: ,PRSI here is still the previous command's value; the new PRSI is
  // stored on the next line, exactly as in the shipped PERFORM.
  if (g.prso && !(itObj && g.prsi == itObj) && a != V_WALK) {
    g.it = g.prso;
  }

  // ZIL: <SETG PRSI .I>
  g.prsi = i;

  int v = M_NOT_HANDLED;

  // ZIL: <COND (<AND <EQUAL? ,NOT-HERE-OBJECT ,PRSO ,PRSI>
  //                  <SET V <NOT-HERE-OBJECT-F>>> .V) (gmain.zil:205-206)
  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  bool notHereHandled = false;
  if (notHere && (g.prso == notHere || g.prsi == notHere)) {
    v = GGlobals::notHereObjectF() ? M_HANDLED : M_NOT_HANDLED;
    notHereHandled = v != M_NOT_HANDLED;
  }

  // ZIL Execution Hierarchy:
  // 1. Actor (WINNER) action: <DD-APPLY "Actor" ,WINNER <GETP ,WINNER ,P?ACTION>>
  if (!notHereHandled && g.winner && g.winner->hasAction()) {
    v = ddApply("Actor", g.winner, [&]() -> int {
      return g.winner->performAction();
    });
  }

  // 2. Room (M-BEG) action: <D-APPLY "Room (M-BEG)" <GETP <LOC ,WINNER> ,P?ACTION> ,M-BEG>
  // ZIL: a true result from the room ends PERFORM (gmain.zil:212).
  if (!v && g.here) {
    v = dApply("Room (M-BEG)", [&]() -> int {
      if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
        return room->performRoomAction(M_BEG);
      }
      return M_NOT_HANDLED;
    });
  }

  // 3. Preaction: <D-APPLY "Preaction" <GET ,PREACTIONS .A>>
  if (!v && hasPreaction(a)) {
    v = dApply("Preaction", [&]() -> int { return getPreaction(a)(); });
  }

  // 4. PRSI action: <AND .I <SET V <D-APPLY "PRSI" <GETP .I ,P?ACTION>>>>
  if (!v && g.prsi && g.prsi->hasAction()) {
    v = dApply("PRSI", [&]() -> int { return g.prsi->performAction(); });
  }

  // 5. Container action: <AND .O <NOT <==? .A ,V?WALK>> <LOC .O> <GETP <LOC .O> ,P?CONTFCN> ...>
  if (!v && g.prso && a != V_WALK && g.prso->getLocation()) {
    ZObject *loc = g.prso->getLocation();
    if (loc->hasContainerAction()) {
      v = ddApply("Container", loc, [&]() -> int {
        return loc->performContainerAction();
      });
    }
  }

  // 6. PRSO action: <AND .O <NOT <==? .A ,V?WALK>> <SET V <D-APPLY "PRSO" <GETP .O ,P?ACTION>>>>
  if (!v && g.prso && a != V_WALK && g.prso->hasAction()) {
    v = dApply("PRSO", [&]() -> int { return g.prso->performAction(); });
  }

  // 7. Default action: <SET V <D-APPLY <> <GET ,ACTIONS .A>>>
  if (!v && hasVerbHandler(a)) {
    v = dApply("", [&]() -> int { return getVerbHandler(a)(); });
  }

  // ZIL: <SETG PRSA .OA> <SETG PRSO .OO> <SETG PRSI .OI>
  g.prsa = oa;
  g.prso = oo;
  g.prsi = oi;

  return v;
}

// ZIL: <VERB? TELL BRIEF SUPER-BRIEF VERBOSE SAVE VERSION QUIT RESTART SCORE SCRIPT UNSCRIPT RESTORE> (gmain.zil:170-171)
bool isMetaVerb(VerbId verb) {
  switch (verb) {
  case V_TELL:
  case V_BRIEF:
  case V_SUPERBRIEF:
  case V_VERBOSE:
  case V_SAVE:
  case V_VERSION:
  case V_QUIT:
  case V_RESTART:
  case V_SCORE:
  case V_SCRIPT:
  case V_UNSCRIPT:
  case V_RESTORE:
    return true;
  default:
    return false;
  }
}

// ZIL: <ROUTINE MAIN-LOOP-1 () ...> (gmain.zil:38-173)
void mainLoop1() {
  initializeAllVerbHandlers();
  auto &g = Globals::instance();

  // ZIL: <COND (<SETG P-WON <PARSER>> ...) (T <SETG P-CONT <>>)>
  // PARSER prints the prompt and READs (gparser.zil:147-154).
  g.pWon = GParser::parser();
  if (!g.pWon) {
    g.pCont = 0;
    return;
  }

  ParsedCommand cmd = buildParsedCommand();
  executeCommand(cmd);

  // Update last action (ZIL: lines 158-160)
  g.lPrsa = cmd.verb;
  g.lPrso = cmd.directObj;
  g.lPrsi = cmd.indirectObj;

  // ZIL: MOVES is incremented only inside CLOCKER (gclock.zil:50), so
  // meta verbs and CLOCK-WAIT turns do not count.
  // Process timers if not meta-verb (ZIL: lines 170-172)
  if (g.pWon && !isMetaVerb(cmd.verb)) {
    // ZIL: <SET V <CLOCKER>> and nothing else. The troll fights through
    // I-FIGHT and the cyclops through I-CYCLOPS, both of them C-TABLE
    // interrupts, so there is no per-turn NPC call here.
    TimerSystem::tick();
  }
}

// ZIL: MAIN-LOOP-1 after <SETG P-WON <PARSER>> succeeded (gmain.zil:42-161)
int executeCommand(const ParsedCommand &cmdIn) {
  auto &g = Globals::instance();
  int v = M_NOT_HANDLED;
  // The match tables are edited in place by the IT substitution below.
  ParsedCommand cmd = cmdIn;

  // ZIL: <COND (<AND ,P-IT-OBJECT <ACCESSIBLE? ,P-IT-OBJECT>>
  //        ... replace ,IT in P-PRSI, else in P-PRSO ...)> (gmain.zil:45-64)
  ZObject *itObj = g.getObject(ObjectIds::IT);
  if (itObj && g.it && GParser::isAccessible(g.it)) {
    bool tmp = false;
    for (auto &entry : cmd.prsiTable) {
      if (entry == itObj) {
        entry = g.it;
        tmp = true;
        break;
      }
    }
    if (!tmp) {
      for (auto &entry : cmd.prsoTable) {
        if (entry == itObj) {
          entry = g.it;
          break;
        }
      }
    }
  }
  cmd.directObj = cmd.prsoTable.empty() ? nullptr : cmd.prsoTable.front();
  cmd.indirectObj = cmd.prsiTable.empty() ? nullptr : cmd.prsiTable.front();

  // ZIL: the parser leaves PRSA/PRSO/PRSI set (SYNTAX-FOUND, SNARF-OBJECTS,
  // gparser.zil:370-372 for directions); PERFORM saves and restores them, so
  // the room's M-END call sees this command's verb (gmain.zil:154).
  g.prsa = cmd.verb;
  g.prso = cmd.directObj;
  g.prsi = cmd.indirectObj;
  if (cmd.isDirection && !g.pWalkDir) {
    g.pWalkDir = cmd.direction;
  }

  // ZIL: MAIN-LOOP-1 lines 65-150. ICNT/OCNT are the table lengths; NUM,
  // TBL, OBJ and PTBL select which table drives the loop.
  const size_t icnt = cmd.prsiTable.size();
  const size_t ocnt = cmd.prsoTable.size();
  ZObject *obj = nullptr;
  bool ptbl = true;
  size_t num = 0;
  if (ocnt == 0) {
    num = 0;
  } else if (ocnt > 1) {
    obj = icnt == 0 ? nullptr : cmd.prsiTable[0];
    num = ocnt;
  } else if (icnt > 1) {
    ptbl = false;
    obj = cmd.prsoTable[0];
    num = icnt;
  } else {
    num = 1;
  }
  // ZIL: <COND (<AND <NOT .OBJ> <1? .ICNT>> <SET OBJ <GET ,P-PRSI 1>>)>
  if (!obj && icnt == 1) {
    obj = cmd.prsiTable[0];
  }

  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);

  if (cmd.verb == V_WALK && g.pWalkDir) {
    // ZIL: <COND (<AND <==? ,PRSA ,V?WALK> <NOT <ZERO? ,P-WALK-DIR>>>
    //             <SET V <PERFORM ,PRSA ,PRSO>>) (gmain.zil:79-81)
    v = perform(V_WALK, nullptr, nullptr);
  } else if (num == 0) {
    // ZIL: (<0? .NUM>
    //        <COND (<0? <BAND <GETB ,P-SYNTAX ,P-SBITS> ,P-SONUMS>>
    //               <SET V <PERFORM ,PRSA>> <SETG PRSO <>>)
    //              (<NOT ,LIT> <TELL "It's too dark to see." CR>)
    //              (T <TELL "It's not clear what you're referring to." CR>
    //                 <SET V <>>)>) (gmain.zil:82-90)
    if (cmd.objectsExpected <= 0) {
      v = perform(cmd.verb, nullptr, nullptr);
      g.prso = nullptr;
    } else if (!g.lit) {
      printLine("It's too dark to see.");
    } else {
      printLine("It's not clear what you're referring to.");
      v = M_NOT_HANDLED;
    }
  } else {
    // ZIL: <SETG P-NOT-HERE 0> <SETG P-MULT <>>
    //      <COND (<G? .NUM 1> <SETG P-MULT T>)> (gmain.zil:92-94)
    g.pNotHere = 0;
    g.pMult = num > 1;
    bool tmp = false;
    size_t cnt = 0;
    while (true) {
      if (++cnt > num) {
        // ZIL: gmain.zil:98-112
        if (g.pNotHere > 0) {
          print("The ");
          if (g.pNotHere != static_cast<int>(num)) {
            print("other ");
          }
          print("object");
          if (g.pNotHere != 1) {
            print("s");
          }
          print(" that you mentioned ");
          if (g.pNotHere != 1) {
            print("are");
          } else {
            print("is");
          }
          printLine("n't here.");
        } else if (!tmp) {
          printLine("There's nothing here you can take.");
        }
        break;
      }
      ZObject *obj1 = ptbl ? cmd.prsoTable[cnt - 1] : cmd.prsiTable[cnt - 1];
      ZObject *o = ptbl ? obj1 : obj;
      ZObject *i = ptbl ? obj : obj1;

      // ZIL: "multiple exceptions" (gmain.zil:120-145)
      if (num > 1 || cmd.nc1IsAll) {
        ZObject *vloc = g.winner ? g.winner->getLocation() : nullptr;
        if (o == notHere) {
          // ZIL: <SETG P-NOT-HERE <+ ,P-NOT-HERE 1>> <AGAIN>
          g.pNotHere++;
          continue;
        }
        if (cmd.verb == V_TAKE && i && cmd.nc1IsAll &&
            !(o && o->getLocation() == i)) {
          // ZIL: <AND <VERB? TAKE> .I <EQUAL? NC1[0] ,W?ALL> <NOT <IN? .O .I>>>
          continue;
        }
        if (cmd.getFlags == GParser::P_ALL && cmd.verb == V_TAKE && o) {
          ZObject *loc = o->getLocation();
          bool wrongPlace = loc != g.winner && loc != g.here && loc != vloc &&
                            loc != i &&
                            !(loc && loc->hasFlag(ObjectFlag::SURFACEBIT));
          bool notTakeable = !(o->hasFlag(ObjectFlag::TAKEBIT) ||
                               o->hasFlag(ObjectFlag::TRYTAKEBIT));
          if (wrongPlace || notTakeable) {
            continue;
          }
        }
        // ZIL: <COND (<EQUAL? .OBJ1 ,IT> <PRINTD ,P-IT-OBJECT>)
        //            (T <PRINTD .OBJ1>)> <TELL ": ">
        if (itObj && obj1 == itObj && g.it) {
          print(g.it->getDesc());
        } else if (obj1) {
          print(obj1->getDesc());
        }
        print(": ");
      }

      // ZIL: <SETG PRSO .O> <SETG PRSI .I> <SET TMP T>
      //      <SET V <PERFORM ,PRSA ,PRSO ,PRSI>>
      g.prso = o;
      g.prsi = i;
      tmp = true;
      v = perform(cmd.verb, o, i);
      // ZIL: <COND (<==? .V ,M-FATAL> <RETURN>)> (gmain.zil:150)
      if (v == M_FATAL) {
        break;
      }
    }
  }

  // Room action (M-END) (ZIL: lines 151-154), skipped after RFATAL
  if (v != M_FATAL && g.here) {
    if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
      v = room->performRoomAction(M_END);
    }
  }

  // ZIL: <COND (<==? .V ,M-FATAL> <SETG P-CONT <>>)> (gmain.zil:161)
  if (v == M_FATAL) {
    g.pCont = false;
  }
  return v;
}

// ZIL: <ROUTINE MAIN-LOOP () ...> (gmain.zil:34-36)
void mainLoop() {
  while (true) {
    if (std::cin.eof()) {
      break;
    }
    mainLoop1();
  }
}
