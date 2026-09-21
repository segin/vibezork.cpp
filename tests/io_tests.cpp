// Unit tests for the output layer (src/core/io.*)
// ZIL: TELL (gmacros.zil:9-50) prints its items with no implicit newline;
// CRLF/CR is explicit. The Z-machine screen model wraps words at the screen
// width and never inserts characters between adjacent prints.
#include "../src/core/io.h"
#include "../src/core/object.h"
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

#define TEST(name)                                                             \
  void test_##name();                                                          \
  struct Register_##name {                                                     \
    Register_##name() { TestFramework::instance().addTest(#name, test_##name); } \
  } register_##name;                                                           \
  void test_##name()

static void reset(int width) {
  setScreenWidth(0);
  crlf(); // flush any pending word from a previous test
  setScreenWidth(width);
}

TEST(TellDoesNotAppendNewline) {
  reset(0);
  std::string out;
  {
    OutputCapture cap;
    tell("Taken");
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("Taken"));
}

TEST(TellWithExplicitCr) {
  reset(0);
  std::string out;
  {
    OutputCapture cap;
    tell("Time passes...", CR);
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("Time passes...\n"));
}

TEST(NoAutomaticSpacingBetweenPrints) {
  reset(0);
  ZObject sword(9100, "sword");
  std::string out;
  {
    OutputCapture cap;
    // ZIL: <TELL "Your skillful " D ,PRSO "smanship ..."> (gverbs.zil:396)
    tell("Your skillful ", &sword, "smanship", CR);
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("Your skillful swordsmanship\n"));
}

TEST(TellPrintsNumbers) {
  reset(0);
  std::string out;
  {
    OutputCapture cap;
    tell("Your score is ", 350, " (total of 350 points)", CR);
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("Your score is 350 (total of 350 points)\n"));
}

TEST(WrapsAtScreenWidth) {
  reset(20);
  std::string out;
  {
    OutputCapture cap;
    printLine("You are standing in an open field west of a white house.");
    out = cap.get();
  }
  // Each line fits in 20 columns and no word is split
  std::stringstream ss(out);
  std::string line;
  int lines = 0;
  while (std::getline(ss, line)) {
    ASSERT_TRUE(line.size() <= 20);
    ASSERT_FALSE(line.empty());
    ASSERT_TRUE(line.back() != ' ');
    lines++;
  }
  ASSERT_TRUE(lines >= 3);
  ASSERT_EQ(out, std::string("You are standing in\nan open field west\nof a white house.\n"));
}

TEST(WrapCountsAcrossPrints) {
  reset(10);
  std::string out;
  {
    OutputCapture cap;
    print("abc def");
    print(" ghi jkl");
    crlf();
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("abc def\nghi jkl\n"));
}

TEST(WidthZeroNeverWraps) {
  reset(0);
  std::string text(200, 'x');
  std::string out;
  {
    OutputCapture cap;
    printLine(text + " " + text);
    out = cap.get();
  }
  ASSERT_EQ(out, text + " " + text + "\n");
}

TEST(EmbeddedNewlineIsHardBreak) {
  reset(80);
  std::string out;
  {
    OutputCapture cap;
    // ZIL "|" hard newlines, as in the leaflet (1dungeon.zil:572-577)
    print("WELCOME TO ZORK!\n\nZORK is a game of adventure, danger, and low cunning.");
    crlf();
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("WELCOME TO ZORK!\n\nZORK is a game of adventure, danger, and low cunning.\n"));
}

TEST(MultipleSpacesPreserved) {
  reset(80);
  std::string out;
  {
    OutputCapture cap;
    // ZIL: "      ** BOOOOOOOOOOOM **" keeps its indentation (1actions.zil:2467)
    printLine("      ** BOOOOOOOOOOOM **");
    out = cap.get();
  }
  ASSERT_EQ(out, std::string("      ** BOOOOOOOOOOOM **\n"));
}

TEST(ColumnTracksOutput) {
  reset(80);
  {
    OutputCapture cap;
    print(">");
    ASSERT_EQ(getOutputColumn(), 1);
    crlf();
    ASSERT_EQ(getOutputColumn(), 0);
  }
}

int main() {
  auto results = TestFramework::instance().runAll();
  setScreenWidth(WRAP_WIDTH);
  int failed = 0;
  for (const auto &r : results) {
    if (!r.passed) failed++;
  }
  return failed == 0 ? 0 : 1;
}
