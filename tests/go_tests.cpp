// Unit tests for ZIL GO (zil/1dungeon.zil:2637-2661)
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/object.h"
#include "../src/systems/melee_tables.h"
#include "../src/systems/timer.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "test_framework.h"
#include <sstream>

class OutputCapture {
public:
  OutputCapture() : old_(std::cout.rdbuf(buffer_.rdbuf())) {}
  ~OutputCapture() { std::cout.rdbuf(old_); }
  std::string get() const { return buffer_.str(); }

private:
  std::stringstream buffer_;
  std::streambuf *old_;
};

// Run GO with an empty stdin so MAIN-LOOP returns at once
static std::string runGo() {
  Globals::instance().reset();
  TimerSystem::clear();
  Melee::resetDefRes();
  initializeGame();
  InputRedirect input("");
  OutputCapture out;
  go();
  return out.get();
}

#define TEST(name)                                                             \
  void test_##name();                                                          \
  struct Register_##name {                                                     \
    Register_##name() { TestFramework::instance().addTest(#name, test_##name); } \
  } register_##name;                                                           \
  void test_##name()

// ZIL: <ENABLE <QUEUE I-FIGHT -1>> <QUEUE I-SWORD -1> <ENABLE <QUEUE I-THIEF -1>>
//      <QUEUE I-CANDLES 40> <QUEUE I-LANTERN 200> (1dungeon.zil:2638-2642)
TEST(StartupInterruptQueue) {
  runGo();
  auto &mgr = TimerSystem::TimerManager::instance();

  const auto *fight = mgr.find("I-FIGHT");
  ASSERT_TRUE(fight != nullptr);
  ASSERT_TRUE(fight->enabled);
  ASSERT_EQ(fight->tick, -1);

  const auto *sword = mgr.find("I-SWORD");
  ASSERT_TRUE(sword != nullptr);
  ASSERT_FALSE(sword->enabled);
  ASSERT_EQ(sword->tick, -1);

  const auto *thief = mgr.find("I-THIEF");
  ASSERT_TRUE(thief != nullptr);
  ASSERT_TRUE(thief->enabled);
  ASSERT_EQ(thief->tick, -1);

  const auto *candles = mgr.find("I-CANDLES");
  ASSERT_TRUE(candles != nullptr);
  ASSERT_FALSE(candles->enabled);
  ASSERT_EQ(candles->tick, 40);

  const auto *lantern = mgr.find("I-LANTERN");
  ASSERT_TRUE(lantern != nullptr);
  ASSERT_FALSE(lantern->enabled);
  ASSERT_EQ(lantern->tick, 200);

  // C-TABLE order: GO's QUEUE calls allocate downward, so CLOCKER sees
  // I-LANTERN first and I-FIGHT last (gclock.zil:26-39, 46-60)
  auto entries = mgr.entries();
  ASSERT_EQ(entries.size(), static_cast<size_t>(5));
  ASSERT_EQ(entries[0]->name, std::string("I-LANTERN"));
  ASSERT_EQ(entries[1]->name, std::string("I-CANDLES"));
  ASSERT_EQ(entries[2]->name, std::string("I-THIEF"));
  ASSERT_EQ(entries[3]->name, std::string("I-SWORD"));
  ASSERT_EQ(entries[4]->name, std::string("I-FIGHT"));

  // No move has been made yet
  ASSERT_EQ(Globals::instance().moves, 0);
}

// ZIL: <PUTP ,INFLATED-BOAT ,P?VTYPE ,NONLANDBIT> (1dungeon.zil:2643)
TEST(BoatVehicleType) {
  runGo();
  auto *boat = Globals::instance().getObject(ObjectIds::BOAT_INFLATED);
  ASSERT_TRUE(boat != nullptr);
  ASSERT_TRUE(boat->getVehicleType().has_value());
  ASSERT_TRUE(*boat->getVehicleType() == ObjectFlag::NONLANDBIT);
  ASSERT_FALSE(boat->hasFlag(ObjectFlag::NONLANDBIT));
}

// ZIL: the six <PUT ,DEFn-RES i <REST ,DEFx k>> forms (1dungeon.zil:2644-2649)
TEST(DefResPatched) {
  Melee::resetDefRes();
  ASSERT_FALSE(Melee::defResPatched());
  ASSERT_TRUE(Melee::DEF1_RES[1].empty());
  runGo();
  ASSERT_TRUE(Melee::defResPatched());
  // <REST ,DEF1 2> starts at word 1, <REST ,DEF1 4> at word 2
  ASSERT_EQ(Melee::DEF1_RES[0].size(), Melee::DEF1.size());
  ASSERT_EQ(Melee::DEF1_RES[1].size(), Melee::DEF1.size() - 1);
  ASSERT_EQ(Melee::DEF1_RES[2].size(), Melee::DEF1.size() - 2);
  ASSERT_EQ(Melee::DEF1_RES[1][0], Melee::DEF1[1]);
  ASSERT_EQ(Melee::DEF1_RES[2][0], Melee::DEF1[2]);
  ASSERT_EQ(Melee::DEF2_RES[2][0], Melee::DEF2B[1]);
  ASSERT_EQ(Melee::DEF2_RES[3][0], Melee::DEF2B[2]);
  ASSERT_EQ(Melee::DEF3_RES[1][0], Melee::DEF3A[1]);
  ASSERT_EQ(Melee::DEF3_RES[3][0], Melee::DEF3B[1]);
  // Sliding windows always expose at least the nine entries RANDOM 9 reads
  for (const auto &w : Melee::DEF1_RES) ASSERT_TRUE(w.size() >= 9);
  for (const auto &w : Melee::DEF2_RES) ASSERT_TRUE(w.size() >= 8);
  for (const auto &w : Melee::DEF3_RES) ASSERT_TRUE(w.size() >= 9);
}

// ZIL: DEF tables verbatim (1actions.zil:3258-3299)
TEST(DefTablesVerbatim) {
  using namespace Melee;
  ASSERT_EQ(DEF1.size(), static_cast<size_t>(13));
  ASSERT_EQ(DEF1[0], MISSED);
  ASSERT_EQ(DEF1[4], STAGGER);
  ASSERT_EQ(DEF1[6], UNCONSCIOUS);
  ASSERT_EQ(DEF1[8], KILLED);
  ASSERT_EQ(DEF1[12], KILLED);
  ASSERT_EQ(DEF2A.size(), static_cast<size_t>(10));
  ASSERT_EQ(DEF2A[9], UNCONSCIOUS);
  ASSERT_EQ(DEF2B.size(), static_cast<size_t>(12));
  ASSERT_EQ(DEF2B[8], UNCONSCIOUS);
  ASSERT_EQ(DEF2B[11], KILLED);
  ASSERT_EQ(DEF3A.size(), static_cast<size_t>(11));
  ASSERT_EQ(DEF3A[10], SERIOUS_WOUND);
  ASSERT_EQ(DEF3B.size(), static_cast<size_t>(11));
  ASSERT_EQ(DEF3B[3], STAGGER);
  // 1actions.zil:3294-3299 lists ten results: MISSED, two STAGGER, four
  // LIGHT-WOUND and three SERIOUS-WOUND. The earlier hand transcription
  // declared eleven slots, leaving a zero (no such blow result) at the end.
  ASSERT_EQ(DEF3C.size(), static_cast<size_t>(10));
  ASSERT_EQ(DEF3C[0], MISSED);
  ASSERT_EQ(DEF3C[1], STAGGER);
  ASSERT_EQ(DEF3C[3], LIGHT_WOUND);
  ASSERT_EQ(DEF3C[9], SERIOUS_WOUND);
  ASSERT_EQ(STRENGTH_MAX, 7);
  ASSERT_EQ(STRENGTH_MIN, 2);
  ASSERT_EQ(CURE_WAIT, 30);
}

// ZIL: HERE, THIS-IS-IT MAILBOX, LIT, WINNER/PLAYER, MOVE, V-VERSION + V-LOOK
TEST(StartupStateAndBanner) {
  std::string out = runGo();
  auto &g = Globals::instance();
  ASSERT_TRUE(g.here != nullptr);
  ASSERT_EQ(g.here->getId(), RoomIds::WEST_OF_HOUSE);
  ASSERT_TRUE(g.it != nullptr);
  ASSERT_EQ(g.it->getId(), ObjectIds::MAILBOX);
  ASSERT_TRUE(g.lit);
  ASSERT_TRUE(g.winner != nullptr);
  ASSERT_EQ(g.winner, g.player);
  ASSERT_EQ(g.winner->getId(), ObjectIds::ADVENTURER);
  ASSERT_EQ(g.winner->getLocation(), g.here);
  ASSERT_CONTAINS(out, "ZORK I: The Great Underground Empire");
  ASSERT_CONTAINS(out, "West of House");
  ASSERT_CONTAINS(out, "small mailbox");
}

int main() {
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
