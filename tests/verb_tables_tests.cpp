// Unit tests for the PICK-ONE message tables in gverbs.zil
// Source: zil/gverbs.zil:844-848, 1272-1276, 2030-2036, 2194-2217
#include "../src/core/gmacros.h"
#include "../src/verbs/verb_tables.h"
#include "test_framework.h"
#include <set>
#include <string>
#include <vector>

// Every entry must appear exactly once before any repeats: PICK-ONE swaps the
// chosen entry into the used-slot and only wraps when the table is exhausted
// (gmacros.zil:127-139).
static void assertCyclesWithoutRepeats(
    GMacros::ZilRandomTable<std::string> &table,
    const std::vector<std::string> &expected) {
  table.reset();
  ASSERT_EQ(table.size(), expected.size());
  std::set<std::string> want(expected.begin(), expected.end());
  std::set<std::string> seen;
  for (size_t i = 0; i < expected.size(); ++i) {
    std::string picked = table.pickOne();
    ASSERT_TRUE(want.count(picked) == 1);
    ASSERT_TRUE(seen.insert(picked).second);
  }
  ASSERT_EQ(seen.size(), expected.size());
}

TEST(YuksTableMatchesZil) {
  // The source comments out "Not bloody likely.", so the table holds four.
  assertCyclesWithoutRepeats(VerbTables::yuks(),
                             {"A valiant attempt.", "You can't be serious.",
                              "An interesting idea...", "What a concept!"});
}

TEST(HellosTableMatchesZil) {
  assertCyclesWithoutRepeats(
      VerbTables::hellos(),
      {"Hello.", "Good day.", "Nice weather we've been having lately.",
       "Goodbye."});
}

TEST(HoHumTableMatchesZil) {
  // Each entry starts with a space: HACK-HACK prints it straight after the
  // object's name (gverbs.zil:2024-2036).
  assertCyclesWithoutRepeats(VerbTables::hoHum(),
                             {" doesn't seem to work.",
                              " isn't notably helpful.", " has no effect."});
}

TEST(JumplossTableMatchesZil) {
  assertCyclesWithoutRepeats(
      VerbTables::jumploss(),
      {"You should have looked before you leaped.",
       "In the movies, your life would be passing before your eyes.",
       "Geronimo..."});
}

TEST(WheeeeeTableMatchesZil) {
  assertCyclesWithoutRepeats(
      VerbTables::wheeeee(),
      {"Very good. Now you can go to the second grade.",
       "Are you enjoying yourself?", "Wheeeeeeeeee!!!!!",
       "Do you expect me to applaud?"});
}

TEST(DummyTableMatchesZil) {
  assertCyclesWithoutRepeats(
      VerbTables::dummy(),
      {"Look around.", "Too late for that.", "Have your eyes checked."});
}

TEST(SwimyuksTableMatchesZil) {
  // A one-entry table always returns that entry (gmacros.zil:132-133).
  VerbTables::swimyuks().reset();
  ASSERT_EQ(VerbTables::swimyuks().size(), static_cast<size_t>(1));
  ASSERT_EQ(VerbTables::swimyuks().pickOne(), "You can't swim in the dungeon.");
  ASSERT_EQ(VerbTables::swimyuks().pickOne(), "You can't swim in the dungeon.");
}

int main() {
  std::cout << "Running Verb Table Tests\n========================\n\n";
  auto results = TestFramework::instance().runAll();
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  std::cout << "\n" << (results.size() - failed) << " passed, " << failed
            << " failed\n";
  return failed == 0 ? 0 : 1;
}
