// Unit tests for ZIL BOOM-ROOM (zil/1actions.zil:2446-2467)
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/systems/death.h"
#include "../src/verbs/verbs.h"
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

// Initialize world and death system for testing
static void setupTestWorld() {
  initializeWorld();
  DeathSystem::initialize();
  DeathSystem::setTestMode(true);
}

// =============================================================================
// BoomRoomFcn Tests (1actions.zil lines 2446-2467)
// ZIL Logic:
// - M-LOOK: Outputs Gas Room description
// - M-END:
//   - If LAMP-ON/BURN and PRSO is CANDLES, TORCH, or MATCH: DUMMY? = true
//   - If HELD? and ONBIT for CANDLES, TORCH, or MATCH:
//     - If DUMMY?:
//       TELL "How sad for an aspiring adventurer to light a " D ,PRSO
//            " in a room which reeks of gas. Fortunately, there is justice in the world." CR
//     - Else:
//       TELL "Oh dear. It appears that the smell coming from this room was coal gas.
//             I would have thought twice about carrying flaming objects in here." CR
//     - JIGS-UP "\n    ** BOOOOOOOOOOOM **"
// - Other RARG: does nothing
// =============================================================================

TEST(BoomRoomFcn_LookPrintsDescription) {
  setupTestWorld();
  OutputCapture cap;
  boomRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("smells strongly of coal gas") != std::string::npos);
  ASSERT_TRUE(output.find("narrow tunnel leading east") != std::string::npos);
}

TEST(BoomRoomFcn_OtherRargOutputsNothing) {
  setupTestWorld();
  for (int rarg : {M_ENTER, M_BEG, 0, 99}) {
    OutputCapture cap;
    boomRoom(rarg);
    ASSERT_TRUE(cap.getOutput().empty());
    ASSERT_EQ(DeathSystem::getDeathCount(), 0);
  }
}

TEST(BoomRoomFcn_NoFlamesCarriedDoesNotExplode) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  ASSERT_TRUE(gasRoom != nullptr);
  g.here = gasRoom;

  OutputCapture cap;
  boomRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(DeathSystem::getDeathCount(), 0);
}

TEST(BoomRoomFcn_BrassLanternDoesNotExplode) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *lantern = g.getObject(ObjectIds::LAMP);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && lantern != nullptr && winner != nullptr);

  g.here = gasRoom;
  lantern->moveTo(winner);
  lantern->setFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  boomRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(DeathSystem::getDeathCount(), 0);
}

TEST(BoomRoomFcn_CarryingLitCandlesExplodesWithStandardMessage) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && candles != nullptr && winner != nullptr);

  g.here = gasRoom;
  candles->moveTo(winner);
  candles->setFlag(ObjectFlag::ONBIT);
  g.prsa = V_WALK;
  g.prso = nullptr;

  OutputCapture cap;
  boomRoom(M_END);
  std::string output = cap.getOutput();

  ASSERT_TRUE(output.find("smell coming from this room was coal gas") !=
              std::string::npos);
  ASSERT_TRUE(output.find("BOOOOOOOOOOOM") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(BoomRoomFcn_CarryingLitTorchExplodes) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *torch = g.getObject(ObjectIds::TORCH);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && torch != nullptr && winner != nullptr);

  g.here = gasRoom;
  torch->moveTo(winner);
  torch->setFlag(ObjectFlag::ONBIT);
  g.prsa = V_WALK;
  g.prso = nullptr;

  OutputCapture cap;
  boomRoom(M_END);
  std::string output = cap.getOutput();

  ASSERT_TRUE(output.find("smell coming from this room was coal gas") !=
              std::string::npos);
  ASSERT_TRUE(output.find("BOOOOOOOOOOOM") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(BoomRoomFcn_CarryingLitMatchExplodes) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *match = g.getObject(ObjectIds::MATCH);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && match != nullptr && winner != nullptr);

  g.here = gasRoom;
  match->moveTo(winner);
  match->setFlag(ObjectFlag::ONBIT);
  g.prsa = V_WALK;
  g.prso = nullptr;

  OutputCapture cap;
  boomRoom(M_END);
  std::string output = cap.getOutput();

  ASSERT_TRUE(output.find("smell coming from this room was coal gas") !=
              std::string::npos);
  ASSERT_TRUE(output.find("BOOOOOOOOOOOM") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(BoomRoomFcn_LightingCandlesInGasRoomExplodesWithDummyMessage) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && candles != nullptr && winner != nullptr);

  g.here = gasRoom;
  candles->moveTo(winner);
  candles->setFlag(ObjectFlag::ONBIT);
  g.prsa = V_LAMP_ON;
  g.prso = candles;

  OutputCapture cap;
  boomRoom(M_END);
  std::string output = cap.getOutput();

  ASSERT_TRUE(output.find("How sad for an aspiring adventurer to light a") !=
              std::string::npos);
  ASSERT_TRUE(output.find("pair of candles") != std::string::npos);
  ASSERT_TRUE(output.find("Fortunately, there is justice in the world.") !=
              std::string::npos);
  ASSERT_TRUE(output.find("BOOOOOOOOOOOM") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(BoomRoomFcn_LightingTorchInGasRoomExplodesWithDummyMessage) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *torch = g.getObject(ObjectIds::TORCH);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && torch != nullptr && winner != nullptr);

  g.here = gasRoom;
  torch->moveTo(winner);
  torch->setFlag(ObjectFlag::ONBIT);
  g.prsa = V_BURN;
  g.prso = torch;

  OutputCapture cap;
  boomRoom(M_END);
  std::string output = cap.getOutput();

  ASSERT_TRUE(output.find("How sad for an aspiring adventurer to light a") !=
              std::string::npos);
  ASSERT_TRUE(output.find("torch") != std::string::npos);
  ASSERT_TRUE(output.find("Fortunately, there is justice in the world.") !=
              std::string::npos);
  ASSERT_TRUE(output.find("BOOOOOOOOOOOM") != std::string::npos);
  ASSERT_TRUE(DeathSystem::getDeathCount() > 0);
}

TEST(BoomRoomFcn_UnlitCandlesDoNotExplode) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  auto *winner = g.winner ? g.winner : g.player;
  ASSERT_TRUE(gasRoom != nullptr && candles != nullptr && winner != nullptr);

  g.here = gasRoom;
  candles->moveTo(winner);
  candles->clearFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  boomRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(DeathSystem::getDeathCount(), 0);
}

TEST(BoomRoomFcn_LitCandlesOnGroundDoNotExplode) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *gasRoom = g.getObject(RoomIds::GAS_ROOM);
  auto *candles = g.getObject(ObjectIds::CANDLES);
  ASSERT_TRUE(gasRoom != nullptr && candles != nullptr);

  g.here = gasRoom;
  candles->moveTo(gasRoom); // on floor
  candles->setFlag(ObjectFlag::ONBIT);

  OutputCapture cap;
  boomRoom(M_END);
  ASSERT_TRUE(cap.getOutput().empty());
  ASSERT_EQ(DeathSystem::getDeathCount(), 0);
}

int main(int argc, char *argv[]) {
  std::cout << "Running BoomRoom Action Tests" << std::endl;
  std::cout << "=============================" << std::endl;
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
