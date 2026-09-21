#include "core/globals.h"
#include "systems/timer.h"
#include "core/gglobals.h"
#include "core/gmacros.h"
#include "core/gmain.h"
#include "core/io.h"
#include "core/object.h"
#include "core/types.h"
#include "parser/gparser.h"
#include "parser/parser.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <print>
#include <sstream>

// ZIL: Test suite for gmain.zil routines and constants
// Source: zil/gmain.zil

void testConstants() {
  std::println("Testing GMAIN constants...");

  assert(SERIAL == 0);
  assert(M_FATAL == 2);
  assert(M_HANDLED == 1);
  assert(M_NOT_HANDLED == 0);
  assert(M_OBJECT == 0);
  assert(M_BEG == 1);
  assert(M_ENTER == 2);
  assert(M_LOOK == 3);
  assert(M_FLASH == 4);
  assert(M_OBJDESC == 5);
  assert(M_END == 6);
  assert(RFATAL == 2);

  std::println("✓ Constants verified against gmain.zil:5-30");
}

void testGlobals() {
  std::println("Testing GMAIN globals...");
  auto &g = Globals::instance();
  g.reset();

  assert(!g.pWon);
  assert(!g.pMult);
  assert(g.pNotHere == 0);
  assert(g.lPrsa == 0);
  assert(g.lPrso == nullptr);
  assert(g.lPrsi == nullptr);
  assert(!g.debug);

  g.pWon = true;
  g.pMult = true;
  g.pNotHere = 3;
  g.lPrsa = V_TAKE;
  g.debug = true;

  assert(g.pWon);
  assert(g.pMult);
  assert(g.pNotHere == 3);
  assert(g.lPrsa == V_TAKE);

  g.reset();
  assert(!g.pWon);
  assert(!g.pMult);
  assert(g.pNotHere == 0);
  assert(!g.debug);

  std::println("✓ Globals verified against gmain.zil:7-10, 174-176");
}

void testDApplyAndDDApply() {
  std::println("Testing D-APPLY and DD-APPLY...");
  auto &g = Globals::instance();
  g.reset();

  // Test null function
  int res = dApply("Test", nullptr);
  assert(res == M_NOT_HANDLED);

  // Test normal execution
  bool executed = false;
  res = dApply("Test", [&]() -> int {
    executed = true;
    return M_HANDLED;
  });
  assert(executed);
  assert(res == M_HANDLED);

  // Test DD-APPLY with object
  auto testObj = std::make_unique<ZObject>(5001, "brass lantern");
  executed = false;
  res = ddApply("Object", testObj.get(), [&]() -> int {
    executed = true;
    return M_FATAL;
  });
  assert(executed);
  assert(res == M_FATAL);

  // Test debug logging mode
  g.debug = true;
  res = dApply("", [&]() -> int { return M_HANDLED; });
  assert(res == M_HANDLED);
  res = ddApply("DebugObj", testObj.get(), [&]() -> int { return M_NOT_HANDLED; });
  assert(res == M_NOT_HANDLED);
  g.debug = false;

  std::println("✓ D-APPLY & DD-APPLY verified against gmain.zil:290-312");
}

void testPerformDispatchHierarchy() {
  std::println("Testing PERFORM dispatch hierarchy...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5002, "adventurer");
  auto roomObj = std::make_unique<ZRoom>(5003, "Living Room", "Living room desc");
  auto swordObj = std::make_unique<ZObject>(5004, "elvish sword");
  auto caseObj = std::make_unique<ZObject>(5005, "trophy case");

  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = roomObj.get();

  // 1. Test Default Action execution
  bool defaultInvoked = false;
  registerVerbHandler(V_PRAY, [&]() -> bool {
    defaultInvoked = true;
    return true;
  });
  int res = perform(V_PRAY, nullptr, nullptr);
  assert(res == M_HANDLED);
  assert(defaultInvoked);

  // 2. Test PRSO Action precedence over Default Action
  bool prsoInvoked = false;
  defaultInvoked = false;
  swordObj->setAction([&]() -> bool {
    prsoInvoked = true;
    return true;
  });
  res = perform(V_PRAY, swordObj.get(), nullptr);
  assert(res == M_HANDLED);
  assert(prsoInvoked);
  assert(!defaultInvoked);

  // 3. Test Container Action precedence over PRSO Action
  bool containerInvoked = false;
  prsoInvoked = false;
  swordObj->moveTo(caseObj.get());
  caseObj->setContainerAction([&]() -> bool {
    containerInvoked = true;
    return true;
  });
  res = perform(V_PRAY, swordObj.get(), nullptr);
  assert(res == M_HANDLED);
  assert(containerInvoked);
  assert(!prsoInvoked);

  // 4. Test PRSI Action precedence over Container and PRSO Action
  bool prsiInvoked = false;
  containerInvoked = false;
  auto targetObj = std::make_unique<ZObject>(5006, "target");
  targetObj->setAction([&]() -> bool {
    prsiInvoked = true;
    return true;
  });
  res = perform(V_PRAY, swordObj.get(), targetObj.get());
  assert(res == M_HANDLED);
  assert(prsiInvoked);
  assert(!containerInvoked);

  // 5. Test Preaction precedence over PRSI Action
  bool preactionInvoked = false;
  prsiInvoked = false;
  registerPreaction(V_PRAY, [&]() -> bool {
    preactionInvoked = true;
    return true;
  });
  res = perform(V_PRAY, swordObj.get(), targetObj.get());
  assert(res == M_HANDLED);
  assert(preactionInvoked);
  assert(!prsiInvoked);

  // 6. Test Winner/Actor Action precedence over everything
  bool actorInvoked = false;
  preactionInvoked = false;
  playerObj->setAction([&]() -> bool {
    actorInvoked = true;
    return true;
  });
  res = perform(V_PRAY, swordObj.get(), targetObj.get());
  assert(res == M_HANDLED);
  assert(actorInvoked);
  assert(!preactionInvoked);

  // 7. Test state preservation across PERFORM calls
  g.prsa = V_LOOK;
  g.prso = roomObj.get();
  g.prsi = nullptr;
  playerObj->setAction(nullptr);
  registerPreaction(V_PRAY, nullptr);

  perform(V_TAKE, swordObj.get(), nullptr);
  assert(g.prsa == V_LOOK);
  assert(g.prso == roomObj.get());
  assert(g.prsi == nullptr);

  // 8. Test IT update on PRSO
  perform(V_TAKE, swordObj.get(), nullptr);
  assert(g.it == swordObj.get());

  std::println("✓ PERFORM hierarchy verified against gmain.zil:182-288");
}


// ZIL: <COND ... (<SET V <APPLY <GETP <LOC ,WINNER> ,P?ACTION> ,M-BEG>> .V) ...>
// A true result from the room's M-BEG call ends PERFORM (gmain.zil:212).
void testRoomMBegStopsDispatch() {
  std::println("Testing room M-BEG result stops PERFORM...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5101, "adventurer");
  auto roomObj = std::make_unique<ZRoom>(5102, "Kitchen", "Kitchen desc");
  auto objA = std::make_unique<ZObject>(5103, "sack");
  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = roomObj.get();

  bool defaultInvoked = false;
  bool prsoInvoked = false;
  registerVerbHandler(V_PRAY, [&]() -> bool {
    defaultInvoked = true;
    return true;
  });
  objA->setAction([&]() -> bool {
    prsoInvoked = true;
    return true;
  });

  // Room returns M_NOT_HANDLED at M-BEG: dispatch continues to PRSO.
  int begCalls = 0;
  roomObj->setRoomAction([&](int rarg) -> int {
    if (rarg == M_BEG) {
      ++begCalls;
    }
    return M_NOT_HANDLED;
  });
  int res = perform(V_PRAY, objA.get(), nullptr);
  assert(res == M_HANDLED);
  assert(begCalls == 1);
  assert(prsoInvoked);
  assert(!defaultInvoked);

  // Room returns M_HANDLED at M-BEG: nothing after it runs.
  prsoInvoked = false;
  roomObj->setRoomAction([&](int rarg) -> int {
    return rarg == M_BEG ? M_HANDLED : M_NOT_HANDLED;
  });
  res = perform(V_PRAY, objA.get(), nullptr);
  assert(res == M_HANDLED);
  assert(!prsoInvoked);
  assert(!defaultInvoked);

  // Room returns M_FATAL at M-BEG: PERFORM returns M_FATAL unchanged.
  roomObj->setRoomAction([&](int rarg) -> int {
    return rarg == M_BEG ? M_FATAL : M_NOT_HANDLED;
  });
  res = perform(V_PRAY, objA.get(), nullptr);
  assert(res == M_FATAL);
  assert(!prsoInvoked);

  // Object and verb handlers pass M_FATAL through as well.
  roomObj->setRoomAction(nullptr);
  objA->setAction([&]() -> int { return M_FATAL; });
  res = perform(V_PRAY, objA.get(), nullptr);
  assert(res == M_FATAL);
  assert(!defaultInvoked);

  registerVerbHandler(V_PRAY, nullptr);
  std::println("✓ Room M-BEG / M-FATAL propagation verified against gmain.zil:211-224");
}


// ZIL: MAIN-LOOP-1 multi-object loop and RFATAL handling (gmain.zil:96-161)
void testRfatalAbortsLoopAndSkipsMEnd() {
  std::println("Testing RFATAL aborts the object loop and skips M-END...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5201, "adventurer");
  auto roomObj = std::make_unique<ZRoom>(5202, "Cellar", "Cellar desc");
  auto objA = std::make_unique<ZObject>(5203, "sack");
  auto objB = std::make_unique<ZObject>(5204, "bottle");
  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = roomObj.get();

  int mEndCalls = 0;
  roomObj->setRoomAction([&](int rarg) -> int {
    if (rarg == M_END) {
      ++mEndCalls;
    }
    return M_NOT_HANDLED;
  });

  int aCalls = 0;
  int bCalls = 0;
  objA->setAction([&]() -> int {
    ++aCalls;
    return GMacros::rfatal();
  });
  objB->setAction([&]() -> int {
    ++bCalls;
    return M_HANDLED;
  });

  ParsedCommand cmd;
  cmd.verb = V_PRAY;
  cmd.words = {"pray"};
  cmd.isAll = true;
  cmd.allObjects = {objA.get(), objB.get()};
  cmd.prsoTable = cmd.allObjects;

  // Fatal on the first object: loop aborts, M-END skipped, P-CONT cleared.
  g.pCont = true;
  int v = executeCommand(cmd);
  assert(v == M_FATAL);
  assert(aCalls == 1);
  assert(bCalls == 0);
  assert(mEndCalls == 0);
  assert(!g.pCont);

  // Non-fatal: both objects run and M-END is called once.
  objA->setAction([&]() -> int {
    ++aCalls;
    return M_HANDLED;
  });
  g.pCont = true;
  v = executeCommand(cmd);
  assert(v == M_NOT_HANDLED); // value of the room's M-END call
  assert(aCalls == 2);
  assert(bCalls == 1);
  assert(mEndCalls == 1);
  assert(g.pCont);

  // A fatal M-END result also clears P-CONT (gmain.zil:154, 161).
  roomObj->setRoomAction([&](int rarg) -> int {
    return rarg == M_END ? M_FATAL : M_NOT_HANDLED;
  });
  cmd.isAll = false;
  cmd.allObjects.clear();
  cmd.directObj = objB.get();
  g.pCont = true;
  v = executeCommand(cmd);
  assert(v == M_FATAL);
  assert(!g.pCont);

  std::println("✓ RFATAL propagation verified against gmain.zil:96-161");
}


// ZIL: a direction goes through PERFORM with PRSA=V?WALK and P-WALK-DIR set
// (gparser.zil:370-374, gmain.zil:79-81), so the room's M-BEG/M-END hooks
// and V-WALK all see the WALK verb.
void testDirectionThroughPerform() {
  std::println("Testing direction commands go through PERFORM as V?WALK...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5301, "adventurer");
  auto roomA = std::make_unique<ZRoom>(5302, "Room A", "Room A desc");
  auto roomB = std::make_unique<ZRoom>(5303, "Room B", "Room B desc");
  roomA->setFlag(ObjectFlag::ONBIT);
  roomB->setFlag(ObjectFlag::ONBIT);
  roomA->setExit(Direction::NORTH, RoomExit(5303));
  ZRoom *a = roomA.get();
  ZRoom *b = roomB.get();
  g.registerObject(5302, std::move(roomA));
  g.registerObject(5303, std::move(roomB));
  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = a;
  playerObj->moveTo(a);

  VerbId begVerb = 0;
  VerbId endVerb = 0;
  a->setRoomAction([&](int rarg) -> int {
    if (rarg == M_BEG) {
      begVerb = g.prsa;
    }
    return M_NOT_HANDLED;
  });
  b->setRoomAction([&](int rarg) -> int {
    if (rarg == M_END) {
      endVerb = g.prsa;
    }
    return M_NOT_HANDLED;
  });

  ParsedCommand cmd;
  cmd.verb = V_WALK;
  cmd.words = {"north"};
  cmd.isDirection = true;
  cmd.direction = Direction::NORTH;
  g.pWalkDir = Direction::NORTH;

  int v = executeCommand(cmd);
  assert(begVerb == V_WALK);
  assert(g.here == b);
  assert(playerObj->getLocation() == b);
  assert(endVerb == V_WALK);
  assert(v != M_FATAL);

  // A room that handles WALK at M-BEG blocks the move (ZIL: room value true).
  g.here = a;
  playerObj->moveTo(a);
  a->setRoomAction([&](int rarg) -> int {
    return (rarg == M_BEG && g.prsa == V_WALK) ? M_HANDLED : M_NOT_HANDLED;
  });
  g.pWalkDir = Direction::NORTH;
  executeCommand(cmd);
  assert(g.here == a);

  // No exit: V-WALK fails with RFATAL, so M-END is skipped.
  a->setRoomAction(nullptr);
  int endCalls = 0;
  a->setRoomAction([&](int rarg) -> int {
    if (rarg == M_END) {
      ++endCalls;
    }
    return M_NOT_HANDLED;
  });
  cmd.direction = Direction::SOUTH;
  g.pWalkDir = Direction::SOUTH;
  v = executeCommand(cmd);
  assert(v == M_FATAL);
  assert(endCalls == 0);
  assert(g.here == a);

  std::println("✓ Direction dispatch verified against gmain.zil:79-81");
}

// ZIL: IT handling in MAIN-LOOP-1 (gmain.zil:45-64) and PERFORM (194-206)
void testItSubstitution() {
  std::println("Testing IT substitution and P-IT-OBJECT rules...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5401, "adventurer");
  auto roomObj = std::make_unique<ZRoom>(5402, "Attic", "Attic desc");
  auto elsewhere = std::make_unique<ZRoom>(5403, "Cellar", "Cellar desc");
  auto objA = std::make_unique<ZObject>(5404, "rope");
  auto objB = std::make_unique<ZObject>(5405, "knife");
  auto itUnique = std::make_unique<ZObject>(ObjectIds::IT, "random object");
  auto notHereUnique =
      std::make_unique<ZObject>(ObjectIds::NOT_HERE_OBJECT, "such thing");
  ZObject *itObj = itUnique.get();
  ZObject *notHere = notHereUnique.get();
  notHere->setAction(GGlobals::notHereObjectF);
  g.registerObject(ObjectIds::IT, std::move(itUnique));
  g.registerObject(ObjectIds::NOT_HERE_OBJECT, std::move(notHereUnique));
  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = roomObj.get();
  playerObj->moveTo(roomObj.get());
  objA->moveTo(roomObj.get());
  objB->moveTo(roomObj.get());
  roomObj->setFlag(ObjectFlag::ONBIT);
  g.lit = true;

  ZObject *seenPrso = nullptr;
  ZObject *seenPrsi = nullptr;
  int defaultCalls = 0;
  registerVerbHandler(V_PRAY, [&]() -> int {
    ++defaultCalls;
    seenPrso = g.prso;
    seenPrsi = g.prsi;
    return M_HANDLED;
  });

  // 1. IT in P-PRSO is replaced by an accessible P-IT-OBJECT before PERFORM.
  g.it = objA.get();
  ParsedCommand cmd;
  cmd.verb = V_PRAY;
  cmd.words = {"pray", "it"};
  cmd.prsoTable = {itObj};
  int v = executeCommand(cmd);
  assert(v != M_FATAL);
  assert(seenPrso == objA.get());
  assert(g.it == objA.get());

  // 2. IT in P-PRSI is replaced first; P-PRSO's IT is then left alone and
  //    resolved by PERFORM itself.
  g.it = objA.get();
  cmd.prsoTable = {objB.get()};
  cmd.prsiTable = {itObj};
  executeCommand(cmd);
  assert(seenPrso == objB.get());
  assert(seenPrsi == objA.get());

  // 3. Inaccessible P-IT-OBJECT: "I don't see what you are referring to."
  //    and RFATAL, so M-END is skipped and P-CONT cleared.
  int endCalls = 0;
  roomObj->setRoomAction([&](int rarg) -> int {
    if (rarg == M_END) {
      ++endCalls;
    }
    return M_NOT_HANDLED;
  });
  objA->moveTo(elsewhere.get());
  g.it = objA.get();
  g.pCont = true;
  defaultCalls = 0;
  cmd.prsoTable = {itObj};
  cmd.prsiTable.clear();
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    v = executeCommand(cmd);
    std::cout.rdbuf(old);
    assert(buf.str().find("I don't see what you are referring to.") !=
           std::string::npos);
  }
  assert(v == M_FATAL);
  assert(defaultCalls == 0);
  assert(endCalls == 0);
  assert(!g.pCont);

  // 4. No P-IT-OBJECT at all behaves the same way.
  g.it = nullptr;
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    v = executeCommand(cmd);
    std::cout.rdbuf(old);
    assert(buf.str().find("I don't see what you are referring to.") !=
           std::string::npos);
  }
  assert(v == M_FATAL);

  // 5. P-IT-OBJECT is set to PRSO, except for WALK and when the previous
  //    PRSI was IT (gmain.zil:202-203).
  g.it = nullptr;
  g.prsi = nullptr;
  perform(V_PRAY, objB.get(), nullptr);
  assert(g.it == objB.get());
  g.it = nullptr;
  registerVerbHandler(V_WALK, []() -> int { return M_HANDLED; });
  perform(V_WALK, objB.get(), nullptr);
  registerVerbHandler(V_WALK, Verbs::vWalk);
  assert(g.it == nullptr);
  g.it = nullptr;
  g.prsi = itObj;
  perform(V_PRAY, objB.get(), nullptr);
  assert(g.it == nullptr);

  // 6. NOT-HERE-OBJECT as PRSO runs NOT-HERE-OBJECT-F instead of the verb.
  defaultCalls = 0;
  g.pNc1 = {"lamp"};
  cmd.prsoTable = {notHere};
  {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    v = executeCommand(cmd);
    std::cout.rdbuf(old);
    assert(buf.str().find("You can't see any lamp here!") !=
           std::string::npos);
  }
  assert(defaultCalls == 0);
  assert(v != M_FATAL); // V is the room's M-END value afterwards (gmain.zil:154)

  registerVerbHandler(V_PRAY, nullptr);
  std::println("✓ IT handling verified against gmain.zil:45-64, 194-206");
}

// ZIL: MAIN-LOOP-1 multi-object loop and "multiple exceptions"
// (gmain.zil:65-150)
void testMultiObjectLoop() {
  std::println("Testing the multi-object loop...");
  auto &g = Globals::instance();
  g.reset();
  initializeAllVerbHandlers();

  auto playerObj = std::make_unique<ZObject>(5501, "adventurer");
  auto roomObj = std::make_unique<ZRoom>(5502, "Kitchen", "Kitchen desc");
  auto sack = std::make_unique<ZObject>(5503, "brown sack");
  auto bottle = std::make_unique<ZObject>(5504, "glass bottle");
  auto table = std::make_unique<ZObject>(5505, "kitchen table");
  auto water = std::make_unique<ZObject>(5506, "quantity of water");
  auto box = std::make_unique<ZObject>(5507, "box");
  auto notHereUnique =
      std::make_unique<ZObject>(ObjectIds::NOT_HERE_OBJECT, "such thing");
  ZObject *notHere = notHereUnique.get();
  notHere->setAction(GGlobals::notHereObjectF);
  g.registerObject(ObjectIds::NOT_HERE_OBJECT, std::move(notHereUnique));
  g.player = playerObj.get();
  g.winner = playerObj.get();
  g.here = roomObj.get();
  roomObj->setFlag(ObjectFlag::ONBIT);
  g.lit = true;
  playerObj->moveTo(roomObj.get());
  table->moveTo(roomObj.get());
  table->setFlag(ObjectFlag::SURFACEBIT);
  table->setFlag(ObjectFlag::OPENBIT);
  sack->moveTo(table.get());
  sack->setFlag(ObjectFlag::TAKEBIT);
  bottle->moveTo(table.get());
  bottle->setFlag(ObjectFlag::TAKEBIT);
  bottle->setFlag(ObjectFlag::OPENBIT);
  water->moveTo(bottle.get());
  water->setFlag(ObjectFlag::TAKEBIT);
  box->moveTo(roomObj.get());
  box->setFlag(ObjectFlag::TRYTAKEBIT);

  std::vector<ZObject *> performed;
  registerVerbHandler(V_PRAY, [&]() -> int {
    performed.push_back(g.prso);
    return M_HANDLED;
  });
  registerVerbHandler(V_TAKE, [&]() -> int {
    performed.push_back(g.prso);
    print("Taken.");
    crlf();
    return M_HANDLED;
  });

  auto run = [&](ParsedCommand &cmd) {
    std::stringstream buf;
    auto *old = std::cout.rdbuf(buf.rdbuf());
    executeCommand(cmd);
    crlf(); // flush the word-wrap buffer
    std::cout.rdbuf(old);
    return buf.str();
  };

  // 1. Two objects: each is prefixed "name: " and performed in order.
  //    (The handlers here print nothing, so the prefix ends the line and the
  //    output layer drops its trailing space.)
  ParsedCommand cmd;
  cmd.verb = V_PRAY;
  cmd.words = {"pray"};
  cmd.prsoTable = {sack.get(), bottle.get()};
  std::string out = run(cmd);
  assert(performed.size() == 2 && performed[0] == sack.get() &&
         performed[1] == bottle.get());
  assert(out.find("brown sack:") != std::string::npos);
  assert(out.find("glass bottle:") != std::string::npos);
  assert(g.pMult);

  // 2. One object without ALL: no prefix, P-MULT false.
  performed.clear();
  cmd.prsoTable = {sack.get()};
  out = run(cmd);
  assert(performed.size() == 1);
  assert(out.find("brown sack:") == std::string::npos);
  assert(!g.pMult);

  // 3. NOT-HERE-OBJECT counting: both missing -> plural, no "other".
  performed.clear();
  cmd.prsoTable = {notHere, notHere};
  out = run(cmd);
  assert(performed.empty());
  assert(out.find("The objects that you mentioned aren't here.") !=
         std::string::npos);

  // 4. One of two missing -> "The other object that you mentioned isn't here."
  performed.clear();
  cmd.prsoTable = {sack.get(), notHere};
  out = run(cmd);
  assert(performed.size() == 1);
  assert(out.find("brown sack:") != std::string::npos);
  assert(out.find("The other object that you mentioned isn't here.") !=
         std::string::npos);

  // 5. Nothing performed at all (all skipped) -> the any-verb message.
  //    TAKE ALL with only a non-takeable object on offer.
  performed.clear();
  cmd.verb = V_TAKE;
  cmd.words = {"take", "all"};
  cmd.nc1IsAll = true;
  cmd.getFlags = GParser::P_ALL;
  cmd.prsoTable = {table.get()};
  out = run(cmd);
  assert(performed.empty());
  assert(out.find("There's nothing here you can take.") != std::string::npos);

  // 6. TAKE ALL skip rules: the table (no TAKEBIT) is skipped, the sack and
  //    bottle (on a SURFACEBIT object) are taken, the water (inside the
  //    bottle) is skipped, the TRYTAKEBIT box is performed.
  performed.clear();
  cmd.prsoTable = {table.get(), sack.get(), bottle.get(), water.get(),
                   box.get()};
  out = run(cmd);
  assert(performed.size() == 3);
  assert(performed[0] == sack.get() && performed[1] == bottle.get() &&
         performed[2] == box.get());
  assert(out.find("quantity of water:") == std::string::npos);

  // 7. TAKE ALL FROM <container>: objects not inside the named container are
  //    skipped (gmain.zil:127-131).
  performed.clear();
  cmd.prsiTable = {bottle.get()};
  cmd.prsoTable = {sack.get(), water.get()};
  out = run(cmd);
  assert(performed.size() == 1 && performed[0] == water.get());
  cmd.prsiTable.clear();

  // 8. Role swap: when P-PRSI holds the list, PRSO is fixed and the loop
  //    runs over the indirect objects (gmain.zil:72-76, 115-118).
  performed.clear();
  std::vector<ZObject *> seenPrsi;
  registerVerbHandler(V_PRAY, [&]() -> int {
    performed.push_back(g.prso);
    seenPrsi.push_back(g.prsi);
    return M_HANDLED;
  });
  cmd.verb = V_PRAY;
  cmd.words = {"pray"};
  cmd.nc1IsAll = false;
  cmd.getFlags = 0;
  cmd.prsoTable = {sack.get()};
  cmd.prsiTable = {bottle.get(), box.get()};
  out = run(cmd);
  assert(performed.size() == 2);
  assert(performed[0] == sack.get() && performed[1] == sack.get());
  assert(seenPrsi[0] == bottle.get() && seenPrsi[1] == box.get());
  assert(out.find("glass bottle:") != std::string::npos);
  assert(out.find("box:") != std::string::npos);

  registerVerbHandler(V_PRAY, nullptr);
  registerVerbHandler(V_TAKE, Verbs::vTake);
  std::println("✓ Multi-object loop verified against gmain.zil:65-150");
}

void testMetaVerbs() {
  std::println("Testing meta-verb recognition...");

  assert(isMetaVerb(V_TELL));
  assert(isMetaVerb(V_BRIEF));
  assert(isMetaVerb(V_SUPERBRIEF));
  assert(isMetaVerb(V_VERBOSE));
  assert(isMetaVerb(V_SAVE));
  assert(isMetaVerb(V_VERSION));
  assert(isMetaVerb(V_QUIT));
  assert(isMetaVerb(V_RESTART));
  assert(isMetaVerb(V_SCORE));
  assert(isMetaVerb(V_SCRIPT));
  assert(isMetaVerb(V_UNSCRIPT));
  assert(isMetaVerb(V_RESTORE));

  assert(!isMetaVerb(V_TAKE));
  assert(!isMetaVerb(V_LOOK));
  assert(!isMetaVerb(V_ATTACK));
  assert(!isMetaVerb(V_OPEN));

  std::println("✓ Meta-verbs verified against gmain.zil:170-171");
}

// ZIL: MOVES is incremented only inside CLOCKER (gclock.zil:49-51), and
// CLOCKER is skipped for the meta verbs (gmain.zil:169-172) and for one
// call after CLOCK-WAIT (gclock.zil:45)
void testMovesCountedOnlyByClocker() {
  std::println("Testing MOVES counting...");
  auto &g = Globals::instance();
  g.reset();
  TimerSystem::clear();
  g.pWon = true;
  g.moves = 0;

  // A normal command's CLOCKER pass counts one move
  TimerSystem::clocker();
  assert(g.moves == 1);

  // CLOCK-WAIT skips the pass and the move
  g.clockWait = true;
  TimerSystem::clocker();
  assert(g.moves == 1);
  TimerSystem::clocker();
  assert(g.moves == 2);

  // Meta verbs never reach CLOCKER
  assert(isMetaVerb(V_SCORE));
  assert(isMetaVerb(V_VERSION));
  assert(!isMetaVerb(V_INVENTORY));

  std::println("✓ MOVES counting verified against gclock.zil:50");
}

int main() {
  std::println("========================================");
  std::println("Running GMAIN Tests (gmain.zil)");
  std::println("========================================");

  testConstants();
  testGlobals();
  testDApplyAndDDApply();
  testPerformDispatchHierarchy();
  testRoomMBegStopsDispatch();
  testRfatalAbortsLoopAndSkipsMEnd();
  testDirectionThroughPerform();
  testItSubstitution();
  testMultiObjectLoop();
  testMetaVerbs();
  testMovesCountedOnlyByClocker();

  std::println("========================================");
  std::println("All GMAIN Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
