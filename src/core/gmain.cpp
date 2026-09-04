#include "gmain.h"
#include "globals.h"
#include "io.h"
#include "parser/parser.h"
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
static std::unordered_map<VerbId, std::function<bool()>> verbHandlers_;

// ZIL: PREACTIONS table mapping
static std::unordered_map<VerbId, std::function<bool()>> preactions_;

void registerVerbHandler(VerbId verb, std::function<bool()> handler) {
  verbHandlers_[verb] = std::move(handler);
}

void registerPreaction(VerbId verb, std::function<bool()> handler) {
  preactions_[verb] = std::move(handler);
}

bool hasVerbHandler(VerbId verb) {
  return verbHandlers_.find(verb) != verbHandlers_.end();
}

bool hasPreaction(VerbId verb) {
  return preactions_.find(verb) != preactions_.end();
}

std::function<bool()> getVerbHandler(VerbId verb) {
  auto it = verbHandlers_.find(verb);
  return it != verbHandlers_.end() ? it->second : nullptr;
}

std::function<bool()> getPreaction(VerbId verb) {
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
  registerVerbHandler(V_PUT_ON, Verbs::vPut);
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
  registerVerbHandler(V_KILL, Verbs::vKill);
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
  registerVerbHandler(V_TALK, Verbs::vTalk);
  registerVerbHandler(V_ASK, Verbs::vAsk);
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

  // ZIL: <COND (<AND <EQUAL? ,IT .I .O> <NOT <ACCESSIBLE? ,P-IT-OBJECT>>> <TELL "I don't see what you are referring to." CR> <RFATAL>)>
  if (o == nullptr && i == nullptr && g.it == nullptr &&
      (a == V_EXAMINE || a == V_TAKE || a == V_READ)) {
    printLine("I don't see what you are referring to.");
    return M_FATAL;
  }

  // ZIL: <SETG PRSA .A> <SETG PRSO .O>
  g.prsa = a;
  g.prso = o;

  // ZIL: <COND (<AND ,PRSO <NOT <VERB? WALK>>> <SETG P-IT-OBJECT ,PRSO>)>
  if (g.prso && a != V_WALK) {
    g.it = g.prso;
  }

  // ZIL: <SETG PRSI .I>
  g.prsi = i;

  int v = M_NOT_HANDLED;

  // ZIL Execution Hierarchy:
  // 1. Actor (WINNER) action: <DD-APPLY "Actor" ,WINNER <GETP ,WINNER ,P?ACTION>>
  if (g.winner && g.winner->hasAction()) {
    v = ddApply("Actor", g.winner, [&]() -> int {
      return g.winner->performAction() ? M_HANDLED : M_NOT_HANDLED;
    });
  }

  // 2. Room (M-BEG) action: <D-APPLY "Room (M-BEG)" <GETP <LOC ,WINNER> ,P?ACTION> ,M-BEG>
  if (!v && g.here) {
    v = dApply("Room (M-BEG)", [&]() -> int {
      if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
        room->performRoomAction(M_BEG);
      }
      return M_NOT_HANDLED;
    });
  }

  // 3. Preaction: <D-APPLY "Preaction" <GET ,PREACTIONS .A>>
  if (!v && hasPreaction(a)) {
    v = dApply("Preaction", [&]() -> int {
      return getPreaction(a)() ? M_HANDLED : M_NOT_HANDLED;
    });
  }

  // 4. PRSI action: <AND .I <SET V <D-APPLY "PRSI" <GETP .I ,P?ACTION>>>>
  if (!v && g.prsi && g.prsi->hasAction()) {
    v = dApply("PRSI", [&]() -> int {
      return g.prsi->performAction() ? M_HANDLED : M_NOT_HANDLED;
    });
  }

  // 5. Container action: <AND .O <NOT <==? .A ,V?WALK>> <LOC .O> <GETP <LOC .O> ,P?CONTFCN> ...>
  if (!v && g.prso && a != V_WALK && g.prso->getLocation()) {
    ZObject *loc = g.prso->getLocation();
    if (loc->hasContainerAction()) {
      v = ddApply("Container", loc, [&]() -> int {
        return loc->performContainerAction() ? M_HANDLED : M_NOT_HANDLED;
      });
    }
  }

  // 6. PRSO action: <AND .O <NOT <==? .A ,V?WALK>> <SET V <D-APPLY "PRSO" <GETP .O ,P?ACTION>>>>
  if (!v && g.prso && a != V_WALK && g.prso->hasAction()) {
    v = dApply("PRSO", [&]() -> int {
      return g.prso->performAction() ? M_HANDLED : M_NOT_HANDLED;
    });
  }

  // 7. Default action: <SET V <D-APPLY <> <GET ,ACTIONS .A>>>
  if (!v && hasVerbHandler(a)) {
    v = dApply("", [&]() -> int {
      return getVerbHandler(a)() ? M_HANDLED : M_NOT_HANDLED;
    });
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

  std::println();
  std::print("> ");
  std::cout.flush();
  std::string input = readLine();

  if (input.empty()) {
    return;
  }

  size_t start = input.find_first_not_of(" \t\r\n");
  size_t end = input.find_last_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return;
  }
  input = input.substr(start, end - start + 1);

  if (input.length() > 1000) {
    printLine("That command is too long.");
    return;
  }

  ParsedCommand cmd = getGlobalParser().parse(input);

  if (cmd.verb == 0) {
    g.pWon = false;
    return;
  }

  g.pWon = true;
  int v = M_NOT_HANDLED;

  // Multi-object handling (ZIL: lines 91-150)
  if (cmd.isAll) {
    g.pMult = true;
    g.pNotHere = 0;

    if (cmd.allObjects.empty()) {
      printLine(std::format("There's nothing here to {}.", cmd.words[0]));
      return;
    }

    for (auto *obj : cmd.allObjects) {
      print(std::format("{}: ", obj->getDesc()));
      v = perform(cmd.verb, obj, cmd.indirectObj);
      if (v == M_FATAL) {
        break;
      }
    }
  } else if (cmd.isDirection) {
    g.pMult = false;
    Verbs::vWalkDir(cmd.direction);
  } else {
    g.pMult = false;
    v = perform(cmd.verb, cmd.directObj, cmd.indirectObj);
  }

  // Room action (M-END) (ZIL: line 154)
  if (v != M_FATAL && g.here) {
    if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
      room->performRoomAction(M_END);
    }
  }

  // Update last action (ZIL: lines 158-160)
  g.lPrsa = cmd.verb;
  g.lPrso = cmd.directObj;
  g.lPrsi = cmd.indirectObj;

  g.moves++;

  // Process timers if not meta-verb (ZIL: lines 170-172)
  if (g.pWon && !isMetaVerb(cmd.verb)) {
    TimerSystem::tick();
    NPCSystem::processTrollTurn();
    NPCSystem::processCyclopsTurn();
  }
}

// ZIL: <ROUTINE MAIN-LOOP () ...> (gmain.zil:34-36)
void mainLoop() {
  while (true) {
    if (std::cin.eof()) {
      std::println("\nGoodbye!");
      break;
    }
    mainLoop1();
  }
}
