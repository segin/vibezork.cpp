#include "test_framework.h"
#include "../src/parser/parser.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/verbs/verbs.h"

// Comprehensive Parser Tests - Task 66.2

// Test tokenization through parsing (tokenize is private)
TEST(TokenizationEmptyString) {
    Parser parser;
    ParsedCommand cmd = parser.parse("");
    ASSERT_EQ(cmd.words.size(), 0);
}

TEST(TokenizationSingleWord) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take");
    ASSERT_TRUE(cmd.words.size() >= 1);
    ASSERT_EQ(cmd.words[0], "take");
}

TEST(TokenizationMultipleSpaces) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take    lamp");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationLeadingSpaces) {
    Parser parser;
    ParsedCommand cmd = parser.parse("    take lamp");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationTrailingSpaces) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take lamp    ");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationTabsAndNewlines) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take\tlamp\n");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationPunctuation) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take lamp!");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationNumbers) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take 123");
    ASSERT_TRUE(cmd.words.size() >= 2);
}

TEST(TokenizationMixedCase) {
    Parser parser;
    ParsedCommand cmd = parser.parse("TAKE Lamp");
    ASSERT_TRUE(cmd.words.size() >= 2);
    // Parser should normalize to lowercase
    ASSERT_EQ(cmd.words[0], "take");
}

TEST(TokenizationVeryLongWord) {
    Parser parser;
    std::string longWord(1000, 'a');
    ParsedCommand cmd = parser.parse(longWord);
    ASSERT_TRUE(cmd.words.size() >= 1);
}

TEST(TokenizationManyWords) {
    Parser parser;
    std::string manyWords;
    for (int i = 0; i < 100; i++) {
        manyWords += "word" + std::to_string(i) + " ";
    }
    ParsedCommand cmd = parser.parse(manyWords);
    // P-INBUF keeps 120 characters and P-LEXV 59 words (gparser.zil:34-47)
    ASSERT_TRUE(cmd.words.size() > 10 && cmd.words.size() <= 59);
}

















// Test complete parsing flow
TEST(ParsingFlowSimpleCommand) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.player = g.winner;
    g.lit = true; // PARSER recomputes LIT from HERE, which it derives from the player's location
    if (g.player && g.here) { g.player->moveTo(g.here); g.here->setFlag(ObjectFlag::ONBIT); }
    g.registerObject(999, std::move(player));
    
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    Parser parser;
    ParsedCommand cmd = parser.parse("take lamp");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, nullptr);
    ASSERT_FALSE(cmd.isAll);
    ASSERT_FALSE(cmd.isDirection);
    
    g.reset();
}

TEST(ParsingFlowComplexCommand) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.player = g.winner;
    g.lit = true; // PARSER recomputes LIT from HERE, which it derives from the player's location
    if (g.player && g.here) { g.player->moveTo(g.here); g.here->setFlag(ObjectFlag::ONBIT); }
    g.registerObject(999, std::move(player));
    
    auto lamp = std::make_unique<ZObject>(1, "brass lamp");
    lamp->addSynonym("lamp");
    lamp->addAdjective("brass");
    lamp->addAdjective("small");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto box = std::make_unique<ZObject>(2, "wooden box");
    box->addSynonym("box");
    box->addAdjective("wooden");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    Parser parser;
    ParsedCommand cmd = parser.parse("put small brass lamp in wooden box");
    
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, boxPtr);
    
    g.reset();
}

TEST(ParsingFlowInvalidCommand) {
    Parser parser;
    ParsedCommand cmd = parser.parse("xyzzy plugh");
    
    ASSERT_EQ(cmd.verb, 0);  // Unknown verb
}

TEST(ParsingFlowDirectionCommand) {
    auto& g = Globals::instance();
    
    ZRoom room1(100, "Room 1", "First room.");
    ZRoom room2(101, "Room 2", "Second room.");
    room1.setExit(Direction::NORTH, RoomExit(101));
    
    g.here = &room1;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.player = g.winner;
    g.lit = true; // PARSER recomputes LIT from HERE, which it derives from the player's location
    if (g.player && g.here) { g.player->moveTo(g.here); g.here->setFlag(ObjectFlag::ONBIT); }
    g.registerObject(999, std::move(player));
    
    Parser parser;
    ParsedCommand cmd = parser.parse("north");
    
    ASSERT_TRUE(cmd.isDirection);
    ASSERT_EQ(cmd.direction, Direction::NORTH);
    ASSERT_EQ(cmd.verb, V_WALK);
    
    g.reset();
}

TEST(ParsingFlowAllCommand) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.player = g.winner;
    g.lit = true; // PARSER recomputes LIT from HERE, which it derives from the player's location
    if (g.player && g.here) { g.player->moveTo(g.here); g.here->setFlag(ObjectFlag::ONBIT); }
    g.registerObject(999, std::move(player));
    
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(&testRoom);
    g.registerObject(2, std::move(knife));
    
    Parser parser;
    ParsedCommand cmd = parser.parse("take all");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_TRUE(cmd.allObjects.size() >= 2);
    
    g.reset();
}

// Test error handling
TEST(ParsingErrorUnknownWord) {
    Parser parser;
    // Use a truly unknown word (not xyzzy which is a known verb synonym for PLUGH)
    ParsedCommand cmd = parser.parse("blargblarg");
    
    ASSERT_EQ(cmd.verb, 0);
}

TEST(ParsingErrorMissingObject) {
    Parser parser;
    ParsedCommand cmd = parser.parse("take");
    
    // Parser asks "What do you want to take?" and sets verb to 0 (orphaned command)
    // The command is incomplete, waiting for the object to be specified
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_EQ(cmd.directObj, nullptr);
}

// ZIL: SYNTAX-CHECK with no noun clause (gparser.zil:707-775): a verb whose
// SYNTAX takes no objects is performed with P-SONUMS = 0; otherwise the
// player is asked "What do you want to <verb> [<prep>]?".
TEST(ParsingObjectlessSyntax) {
    auto& g = Globals::instance();
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.player = g.winner;
    g.lit = true;
    g.registerObject(999, std::move(player));

    Parser parser;

    // JUMP = V-LEAP takes no object
    ParsedCommand cmd = parser.parse("jump");
    ASSERT_EQ(cmd.verb, V_LEAP); // <SYNTAX JUMP = V-LEAP> (gsyntax.zil:249)
    ASSERT_EQ(cmd.objectsExpected, 0);

    // "put" has no objectless syntax -> orphan question
    {
        std::stringstream buf;
        auto* old = std::cout.rdbuf(buf.rdbuf());
        cmd = parser.parse("put");
        std::cout.rdbuf(old);
        ASSERT_EQ(cmd.verb, 0);
        ASSERT_TRUE(buf.str().find("What do you want to put?") != std::string::npos);
    }
    parser.clearOrphan();

    // A trailing preposition is echoed: "turn on" -> "What do you want to turn on?"
    {
        std::stringstream buf;
        auto* old = std::cout.rdbuf(buf.rdbuf());
        cmd = parser.parse("turn on");
        std::cout.rdbuf(old);
        ASSERT_EQ(cmd.verb, 0);
        ASSERT_TRUE(buf.str().find("What do you want to turn on?") != std::string::npos);
    }
    parser.clearOrphan();

    g.reset();
}

TEST(ParsingErrorInvalidSyntax) {
    Parser parser;
    ParsedCommand cmd = parser.parse("the lamp take");
    
    // Invalid syntax should be handled
    ASSERT_TRUE(cmd.verb == 0 || cmd.directObj == nullptr);
}

int main() {
    std::cout << "Running Comprehensive Parser Tests...\n\n";
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "\n" << passed << " tests passed, " << failed << " tests failed\n";
    
    return failed > 0 ? 1 : 0;
}
