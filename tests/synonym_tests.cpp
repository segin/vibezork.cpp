#include "test_framework.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"

TEST(DirectionSynonyms) {
    Parser parser;
    
    // North
    auto cmd = parser.parse("north");
    ASSERT_EQ(cmd.verb, V_WALK);
    ASSERT_TRUE(cmd.isDirection);
    ASSERT_EQ(cmd.direction, Direction::NORTH);
    
    cmd = parser.parse("n");
    ASSERT_EQ(cmd.direction, Direction::NORTH);
    
    // South
    cmd = parser.parse("south");
    ASSERT_EQ(cmd.direction, Direction::SOUTH);
    cmd = parser.parse("s");
    ASSERT_EQ(cmd.direction, Direction::SOUTH);
    
    // East
    cmd = parser.parse("east");
    ASSERT_EQ(cmd.direction, Direction::EAST);
    cmd = parser.parse("e");
    ASSERT_EQ(cmd.direction, Direction::EAST);
    
    // West
    cmd = parser.parse("west");
    ASSERT_EQ(cmd.direction, Direction::WEST);
    cmd = parser.parse("w");
    ASSERT_EQ(cmd.direction, Direction::WEST);
    
    // NE
    cmd = parser.parse("ne");
    ASSERT_EQ(cmd.direction, Direction::NE);
    cmd = parser.parse("northeast");
    ASSERT_EQ(cmd.direction, Direction::NE);
    
    // NW
    cmd = parser.parse("nw");
    ASSERT_EQ(cmd.direction, Direction::NW);
    cmd = parser.parse("northwest");
    ASSERT_EQ(cmd.direction, Direction::NW);
    
    // SE
    cmd = parser.parse("se");
    ASSERT_EQ(cmd.direction, Direction::SE);
    cmd = parser.parse("southeast");
    ASSERT_EQ(cmd.direction, Direction::SE);
    
    // SW
    cmd = parser.parse("sw");
    ASSERT_EQ(cmd.direction, Direction::SW);
    cmd = parser.parse("southwest");
    ASSERT_EQ(cmd.direction, Direction::SW);
    
    // Up/Down
    cmd = parser.parse("up");
    ASSERT_EQ(cmd.direction, Direction::UP);
    cmd = parser.parse("u");
    ASSERT_EQ(cmd.direction, Direction::UP);
    
    cmd = parser.parse("down");
    ASSERT_EQ(cmd.direction, Direction::DOWN);
    cmd = parser.parse("d");
    ASSERT_EQ(cmd.direction, Direction::DOWN);
    
    // In/Out
    cmd = parser.parse("in");
    ASSERT_EQ(cmd.direction, Direction::IN);
    cmd = parser.parse("inside");
    ASSERT_EQ(cmd.direction, Direction::IN);
    
    cmd = parser.parse("out");
    ASSERT_EQ(cmd.direction, Direction::OUT);
    cmd = parser.parse("outside");
    ASSERT_EQ(cmd.direction, Direction::OUT);
}

TEST(VerbSynonymsTake) {
    Parser parser;
    auto& g = Globals::instance();
    g.reset();
    
    // TAKE requires an object. Without one, it should orphan.
    auto cmd = parser.parse("take");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
    
    // Clear orphan for next test
    parser.clearOrphan();
    
    cmd = parser.parse("get");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
}

TEST(VerbSynonymsLook) {
    Parser parser;
    auto cmd = parser.parse("look");
    ASSERT_EQ(cmd.verb, V_LOOK);
    
    cmd = parser.parse("l");
    ASSERT_EQ(cmd.verb, V_LOOK);
}

TEST(VerbSynonymsAttack) {
    Parser parser;
    auto& g = Globals::instance();
    g.reset();
    
    auto cmd = parser.parse("attack");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
    
    parser.clearOrphan();
    
    cmd = parser.parse("kill");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
}


int main() {
    std::cout << "Running Synonym Tests\n";
    std::cout << "=====================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
