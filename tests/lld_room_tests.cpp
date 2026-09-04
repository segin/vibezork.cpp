// Unit tests for ZIL LLD-ROOM (zil/1actions.zil:1058-1148)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/systems/candle.h"
#include "../src/systems/death.h"
#include "../src/systems/timer.h"
#include "../src/verbs/verbs.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "test_framework.h"
#include <sstream>

// Output capture helper
class OutputCapture {
public:
  OutputCapture() : old_cout(std::cout.rdbuf()) {
    std::cout.rdbuf(buffer.rdbuf());
  }

  ~OutputCapture() { std::cout.rdbuf(old_cout); }

  std::string getOutput() const { return buffer.str(); }

private:
  std::stringstream buffer;
  std::streambuf *old_cout;
};

// Initialize world for testing
static void setupTestWorld() {
  initializeWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  g.xb = false;
  g.xc = false;
  TimerSystem::TimerManager::instance().clear();

  // Reset player death status
  DeathSystem::reset();
  DeathSystem::setDead(false);

  auto *hades = g.getObject(RoomIds::ENTRANCE_TO_HADES);
  g.here = hades;
  g.winner = g.player;
  if (g.player && hades) {
    g.player->moveTo(hades);
  }
}

// =============================================================================
// LLDRoomFcn Tests (1actions.zil lines 1058-1148)
// ZIL Logic:
// - M-LOOK:
//   - "You are outside a large gateway, on which is inscribed..."
//   - If !LLD-FLAG && !DEAD:
//     "The way through the gate is barred by evil spirits, who jeer at your attempts to pass."
// - M-BEG:
//   - EXORCISE:
//     - If !LLD-FLAG:
//       - If holds BELL, BOOK, CANDLES: "You must perform the ceremony."
//       - Else: "You aren't equipped for an exorcism."
//   - RING BELL:
//     - If !LLD-FLAG:
//       - SETG XB T
//       - Replace BELL with HOT-BELL at HERE
//       - If winner holds CANDLES: drop to HERE, clear ONBIT, disable I-CANDLES
//       - Enable QUEUE I-XB 6, enable QUEUE I-XBH 20
//   - READ BOOK:
//     - If XC && !LLD-FLAG:
//       - Spirits flee, REMOVE GHOSTS, SETG LLD-FLAG T, disable I-XC
// - M-END:
//   - If XB && winner holds lit CANDLES && !XC:
//     - SETG XC T, flames dance, disable I-XB, enable QUEUE I-XC 3
// - Interrupts:
//   - I-XB: if !XC && HERE==ENTRANCE-TO-HADES: tension broken message; SETG XB <>
//   - I-XC: SETG XC <>; I-XB
//   - I-XBH: remove HOT-BELL, move BELL to ENTRANCE-TO-HADES, cool message
// =============================================================================

TEST(LLDRoomFcn_LookSpiritsPresent) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;

  OutputCapture cap;
  lldRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("outside a large gateway") != std::string::npos);
  ASSERT_TRUE(output.find("Abandon every hope") != std::string::npos);
  ASSERT_TRUE(output.find("all ye who enter here!") != std::string::npos);
  ASSERT_TRUE(output.find("gate is open; through it you can see a desolation") != std::string::npos);
  ASSERT_TRUE(output.find("mangled") != std::string::npos);
  ASSERT_TRUE(output.find("bodies in one corner") != std::string::npos);
  ASSERT_TRUE(output.find("Thousands of voices, lamenting some hideous fate") != std::string::npos);
  ASSERT_TRUE(output.find("The way through the gate is barred by evil spirits") != std::string::npos);
}

TEST(LLDRoomFcn_LookSpiritsDispersed) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = true;

  OutputCapture cap;
  lldRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("outside a large gateway") != std::string::npos);
  ASSERT_TRUE(output.find("barred by evil spirits") == std::string::npos);
}

TEST(LLDRoomFcn_LookWhenDead) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  DeathSystem::setDead(true);

  OutputCapture cap;
  lldRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("outside a large gateway") != std::string::npos);
  ASSERT_TRUE(output.find("barred by evil spirits") == std::string::npos);
}

TEST(LLDRoomFcn_ExorciseWithoutEquipment) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  g.prsa = V_EXORCISE;

  // Move bell, book, candles elsewhere
  auto *bell = g.getObject(ObjectIds::BELL);
  auto *book = g.getObject(ObjectIds::BOOK);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  if (bell) bell->moveTo(nullptr);
  if (book) book->moveTo(nullptr);
  if (candles) candles->moveTo(nullptr);

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You aren't equipped for an exorcism.") != std::string::npos);
}

TEST(LLDRoomFcn_ExorciseWithPartialEquipment) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  g.prsa = V_EXORCISE;

  auto *bell = g.getObject(ObjectIds::BELL);
  auto *book = g.getObject(ObjectIds::BOOK);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  if (bell) bell->moveTo(g.player);
  if (book) book->moveTo(g.player);
  if (candles) candles->moveTo(nullptr); // Missing candles

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You aren't equipped for an exorcism.") != std::string::npos);
}

TEST(LLDRoomFcn_ExorciseWithFullEquipment) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  g.prsa = V_EXORCISE;

  auto *bell = g.getObject(ObjectIds::BELL);
  auto *book = g.getObject(ObjectIds::BOOK);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  if (bell) bell->moveTo(g.player);
  if (book) book->moveTo(g.player);
  if (candles) candles->moveTo(g.player);

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You must perform the ceremony.") != std::string::npos);
}

TEST(LLDRoomFcn_ExorciseWhenAlreadyDispersed) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = true;
  g.prsa = V_EXORCISE;

  auto *bell = g.getObject(ObjectIds::BELL);
  auto *book = g.getObject(ObjectIds::BOOK);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  if (bell) bell->moveTo(g.player);
  if (book) book->moveTo(g.player);
  if (candles) candles->moveTo(g.player);

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.empty());
}

TEST(LLDRoomFcn_RingBellCeremonyStep1) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = false;
  g.xb = false;

  auto *bell = g.getObject(ObjectIds::BELL);
  auto *hotBell = g.getObject(ObjectIds::HOT_BELL);
  auto *candles = g.getObject(ObjectIds::CANDLES);

  ASSERT_TRUE(bell != nullptr);
  ASSERT_TRUE(hotBell != nullptr);
  ASSERT_TRUE(candles != nullptr);

  bell->moveTo(g.player);
  candles->moveTo(g.player);
  candles->setFlag(ObjectFlag::ONBIT);
  CandleSystem::enableCandleTimer();

  g.prsa = V_RING;
  g.prso = bell;

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(g.xb);
  ASSERT_NE(bell->getLocation(), g.player);
  ASSERT_EQ(hotBell->getLocation(), g.here);
  ASSERT_EQ(candles->getLocation(), g.here);
  ASSERT_FALSE(candles->hasFlag(ObjectFlag::ONBIT));
  ASSERT_FALSE(TimerSystem::isTimerEnabled("I-CANDLES"));

  ASSERT_TRUE(output.find("red hot and falls to the ground") != std::string::npos);
  ASSERT_TRUE(output.find("paralyzed") != std::string::npos);
  ASSERT_TRUE(output.find("stop their jeering") != std::string::npos);
  ASSERT_TRUE(output.find("In your confusion, the candles drop to the ground (and they are out).") != std::string::npos);

  ASSERT_TRUE(TimerSystem::isTimerEnabled("I-XB"));
  ASSERT_TRUE(TimerSystem::isTimerEnabled("I-XBH"));
}

TEST(LLDRoomFcn_RingBellWhenAlreadyDispersed) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.lldFlag = true;
  g.xb = false;

  auto *bell = g.getObject(ObjectIds::BELL);
  bell->moveTo(g.player);
  g.prsa = V_RING;
  g.prso = bell;

  OutputCapture cap;
  lldRoom(M_BEG);

  ASSERT_FALSE(g.xb);
  ASSERT_EQ(bell->getLocation(), g.player);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_CandlesEndTurnStep2) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = false;

  auto *candles = g.getObject(ObjectIds::CANDLES);
  candles->moveTo(g.player);
  candles->setFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  lldRoom(M_END);

  std::string output = cap.getOutput();
  ASSERT_TRUE(g.xc);
  ASSERT_TRUE(output.find("flames flicker wildly") != std::string::npos);
  ASSERT_TRUE(output.find("earth beneath") != std::string::npos);
  ASSERT_TRUE(output.find("trembles") != std::string::npos);
  ASSERT_TRUE(output.find("legs nearly buckle") != std::string::npos);
  ASSERT_TRUE(output.find("spirits cower") != std::string::npos);
  ASSERT_TRUE(output.find("unearthly power") != std::string::npos);

  ASSERT_FALSE(TimerSystem::isTimerEnabled("I-XB"));
  ASSERT_TRUE(TimerSystem::isTimerEnabled("I-XC"));
}

TEST(LLDRoomFcn_CandlesEndTurnWithoutXb) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = false;
  g.xc = false;

  auto *candles = g.getObject(ObjectIds::CANDLES);
  candles->moveTo(g.player);
  candles->setFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  lldRoom(M_END);

  ASSERT_FALSE(g.xc);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_CandlesEndTurnUnlit) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = false;

  auto *candles = g.getObject(ObjectIds::CANDLES);
  candles->moveTo(g.player);
  candles->clearFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  lldRoom(M_END);

  ASSERT_FALSE(g.xc);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_ReadBookStep3) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xc = true;
  g.lldFlag = false;

  auto *book = g.getObject(ObjectIds::BOOK);
  auto *ghosts = g.getObject(ObjectIds::GHOSTS);
  book->moveTo(g.player);

  g.prsa = V_READ;
  g.prso = book;

  OutputCapture cap;
  lldRoom(M_BEG);

  std::string output = cap.getOutput();
  ASSERT_TRUE(g.lldFlag);
  ASSERT_TRUE(output.find("Each word of the prayer reverberates") != std::string::npos);
  ASSERT_TRUE(output.find("Begone, fiends!") != std::string::npos);
  ASSERT_TRUE(output.find("sensing") != std::string::npos);
  ASSERT_TRUE(output.find("flee through the walls") != std::string::npos);

  if (ghosts) {
    ASSERT_EQ(ghosts->getLocation(), nullptr);
  }
  ASSERT_FALSE(TimerSystem::isTimerEnabled("I-XC"));
}

TEST(LLDRoomFcn_ReadBookWithoutStep2) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xc = false;
  g.lldFlag = false;

  auto *book = g.getObject(ObjectIds::BOOK);
  book->moveTo(g.player);

  g.prsa = V_READ;
  g.prso = book;

  OutputCapture cap;
  lldRoom(M_BEG);

  ASSERT_FALSE(g.lldFlag);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_InterruptXbInHades) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = false;
  g.here = g.getObject(RoomIds::ENTRANCE_TO_HADES);

  OutputCapture cap;
  bool ret = iXb();

  ASSERT_FALSE(ret);
  ASSERT_FALSE(g.xb);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The tension of this ceremony is broken") != std::string::npos);
  ASSERT_TRUE(output.find("resume their hideous jeering.") != std::string::npos);
}

TEST(LLDRoomFcn_InterruptXbOutsideHades) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = false;
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);

  OutputCapture cap;
  bool ret = iXb();

  ASSERT_FALSE(ret);
  ASSERT_FALSE(g.xb);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_InterruptXbSuppressedWhenXc) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = true;
  g.here = g.getObject(RoomIds::ENTRANCE_TO_HADES);

  OutputCapture cap;
  bool ret = iXb();

  ASSERT_FALSE(ret);
  ASSERT_FALSE(g.xb);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_InterruptXc) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.xb = true;
  g.xc = true;
  g.here = g.getObject(RoomIds::ENTRANCE_TO_HADES);

  OutputCapture cap;
  bool ret = iXc();

  ASSERT_FALSE(ret);
  ASSERT_FALSE(g.xc);
  ASSERT_FALSE(g.xb);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The tension of this ceremony is broken") != std::string::npos);
  ASSERT_TRUE(output.find("resume their hideous jeering.") != std::string::npos);
}

TEST(LLDRoomFcn_InterruptXbhInHades) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *hotBell = g.getObject(ObjectIds::HOT_BELL);
  auto *bell = g.getObject(ObjectIds::BELL);
  auto *hades = g.getObject(RoomIds::ENTRANCE_TO_HADES);
  g.here = hades;

  hotBell->moveTo(hades);
  bell->moveTo(nullptr);

  OutputCapture cap;
  bool ret = iXbh();

  ASSERT_FALSE(ret);
  ASSERT_EQ(hotBell->getLocation(), nullptr);
  ASSERT_EQ(bell->getLocation(), hades);
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The bell appears to have cooled down.") != std::string::npos);
}

TEST(LLDRoomFcn_InterruptXbhOutsideHades) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *hotBell = g.getObject(ObjectIds::HOT_BELL);
  auto *bell = g.getObject(ObjectIds::BELL);
  auto *hades = g.getObject(RoomIds::ENTRANCE_TO_HADES);
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);

  hotBell->moveTo(hades);
  bell->moveTo(nullptr);

  OutputCapture cap;
  bool ret = iXbh();

  ASSERT_FALSE(ret);
  ASSERT_EQ(hotBell->getLocation(), nullptr);
  ASSERT_EQ(bell->getLocation(), hades);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(LLDRoomFcn_UnhandledRarg) {
  setupTestWorld();
  OutputCapture cap;
  for (int rarg : {M_ENTER, 0, 99}) {
    lldRoom(rarg);
  }
  ASSERT_TRUE(cap.getOutput().empty());
}

int main(int argc, char *argv[]) {
  std::cout << "Running LLDRoom Action Tests" << std::endl;
  std::cout << "============================" << std::endl;
  std::cout << std::endl;

  auto results = TestFramework::instance().runAll();

  int passed = 0, failed = 0;
  for (const auto &r : results) {
    if (r.passed)
      passed++;
    else
      failed++;
  }

  std::cout << std::endl;
  std::cout << "Results: " << passed << " passed, " << failed << " failed"
            << std::endl;

  return failed > 0 ? 1 : 0;
}
