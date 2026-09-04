// Unit tests for ZIL MIRROR-ROOM (zil/1actions.zil:958-969)
#include "../src/core/globals.h"
#include "../src/core/object.h"
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
  g.mirrorMung = false;
  g.lucky = true;
}

// =============================================================================
// MirrorRoomFcn Tests (1actions.zil lines 958-969, 971-1012)
// ZIL Logic:
// - M-LOOK:
//   - "You are in a large square room with tall ceilings..."
//   - If MIRROR-MUNG: "Unfortunately, the mirror has been destroyed by your recklessness."
// - Other rargs: no-op
// - Mirror object interaction (MIRROR-MIRROR):
//   - EXAMINE: "There is an ugly person staring back at you." (or broken if munged)
//   - TAKE: "The mirror is many times your size. Give up."
//   - MUNG/ATTACK/THROW: sets MIRROR-MUNG=true, LUCKY=false
//   - RUB: teleports between Mirror Room 1 and Mirror Room 2
// =============================================================================

TEST(MirrorRoomFcn_LookIntactMirror) {
  setupTestWorld();
  OutputCapture cap;
  mirrorRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You are in a large square room with tall ceilings") != std::string::npos);
  ASSERT_TRUE(output.find("enormous mirror which fills the entire wall") != std::string::npos);
  ASSERT_TRUE(output.find("Unfortunately, the mirror has been destroyed") == std::string::npos);
}

TEST(MirrorRoomFcn_LookDestroyedMirror) {
  setupTestWorld();
  auto &g = Globals::instance();
  g.mirrorMung = true;

  OutputCapture cap;
  mirrorRoom(M_LOOK);

  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You are in a large square room with tall ceilings") != std::string::npos);
  ASSERT_TRUE(output.find("Unfortunately, the mirror has been destroyed by your recklessness.") != std::string::npos);
}

TEST(MirrorRoomFcn_UnhandledRarg) {
  setupTestWorld();
  OutputCapture cap;
  mirrorRoom(M_BEG);
  mirrorRoom(M_END);
  mirrorRoom(M_ENTER);
  ASSERT_TRUE(cap.getOutput().empty());
}

TEST(MirrorRoomFcn_MirrorExamineIntact) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  g.prso = mirror;
  g.prsa = V_EXAMINE;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("There is an ugly person staring back at you.") != std::string::npos);
}

TEST(MirrorRoomFcn_MirrorExamineDestroyed) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  g.mirrorMung = true;
  g.prso = mirror;
  g.prsa = V_EXAMINE;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The mirror is broken into many pieces.") != std::string::npos);
}

TEST(MirrorRoomFcn_MirrorTakeRefused) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  g.prso = mirror;
  g.prsa = V_TAKE;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("The mirror is many times your size. Give up.") != std::string::npos);
}

TEST(MirrorRoomFcn_MirrorBreakMung) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  g.prso = mirror;
  g.prsa = V_MUNG;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("You have broken the mirror") != std::string::npos);
  ASSERT_TRUE(output.find("seven years' supply of") != std::string::npos);
  ASSERT_TRUE(g.mirrorMung);
  ASSERT_FALSE(g.lucky);
}

TEST(MirrorRoomFcn_MirrorBreakAgain) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  g.mirrorMung = true;
  g.prso = mirror;
  g.prsa = V_ATTACK;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("Haven't you done enough damage already?") != std::string::npos);
}

TEST(MirrorRoomFcn_MirrorRubTeleports) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mr1 = g.getObject(RoomIds::MIRROR_ROOM_1);
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  auto *sword = g.getObject(ObjectIds::SWORD);

  g.here = mr1;
  g.winner = g.player;
  g.player->moveTo(mr1);
  sword->moveTo(mr1);

  g.prso = mirror;
  g.prsi = nullptr;
  g.prsa = V_RUB;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("There is a rumble from deep within the earth and the room shakes.") != std::string::npos);
  ASSERT_EQ(g.here->getId(), RoomIds::MIRROR_ROOM_2);
  ASSERT_EQ(sword->getLocation()->getId(), RoomIds::MIRROR_ROOM_2);
}

TEST(MirrorRoomFcn_MirrorRubWithObjectTingles) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mr1 = g.getObject(RoomIds::MIRROR_ROOM_1);
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);
  auto *sword = g.getObject(ObjectIds::SWORD);

  g.here = mr1;
  g.winner = g.player;
  g.player->moveTo(mr1);

  g.prso = mirror;
  g.prsi = sword;
  g.prsa = V_RUB;

  OutputCapture cap;
  ASSERT_TRUE(mirrorAction());
  std::string output = cap.getOutput();
  ASSERT_TRUE(output.find("faint tingling transmitted through the sword") != std::string::npos);
  ASSERT_EQ(g.here->getId(), RoomIds::MIRROR_ROOM_1);
}

TEST(MirrorRoomFcn_MirrorRubWhenDestroyedDoesNothing) {
  setupTestWorld();
  auto &g = Globals::instance();
  auto *mr1 = g.getObject(RoomIds::MIRROR_ROOM_1);
  auto *mirror = g.getObject(ObjectIds::MIRROR_1);

  g.here = mr1;
  g.mirrorMung = true;
  g.prso = mirror;
  g.prsi = nullptr;
  g.prsa = V_RUB;

  OutputCapture cap;
  ASSERT_FALSE(mirrorAction());
  ASSERT_EQ(g.here->getId(), RoomIds::MIRROR_ROOM_1);
}

int main(int argc, char *argv[]) {
  std::cout << "Running MirrorRoom Action Tests" << std::endl;
  std::cout << "===============================" << std::endl;
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
