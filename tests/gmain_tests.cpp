#include "core/globals.h"
#include "core/gmain.h"
#include "core/object.h"
#include "core/types.h"
#include "verbs/verbs.h"
#include "world/rooms.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <print>

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

int main() {
  std::println("========================================");
  std::println("Running GMAIN Tests (gmain.zil)");
  std::println("========================================");

  testConstants();
  testGlobals();
  testDApplyAndDDApply();
  testPerformDispatchHierarchy();
  testMetaVerbs();

  std::println("========================================");
  std::println("All GMAIN Tests Passed successfully!");
  std::println("========================================");
  return 0;
}
