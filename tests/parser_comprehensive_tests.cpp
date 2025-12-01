#include "test_framework.h"
#include "../src/parser/parser.h"
#include "../src/parser/verb_registry.h"
#include "../src/parser/syntax.h"
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
    ASSERT_TRUE(cmd.words.size() >= 100);
}

// Test verb recognition edge cases
TEST(VerbRecognitionUnknownVerb) {
    VerbRegistry registry;
    auto verb = registry.lookupVerb("xyzzy");
    ASSERT_FALSE(verb.has_value());
}

TEST(VerbRecognitionEmptyString) {
    VerbRegistry registry;
    auto verb = registry.lookupVerb("");
    ASSERT_FALSE(verb.has_value());
}

TEST(VerbRecognitionAllSynonyms) {
    VerbRegistry registry;
    
    // Test all TAKE synonyms
    ASSERT_TRUE(registry.lookupVerb("take").has_value());
    ASSERT_TRUE(registry.lookupVerb("get").has_value());
    ASSERT_TRUE(registry.lookupVerb("grab").has_value());
    ASSERT_TRUE(registry.lookupVerb("carry").has_value());
    
    // All should map to same verb
    ASSERT_EQ(registry.lookupVerb("take").value(), registry.lookupVerb("get").value());
    ASSERT_EQ(registry.lookupVerb("take").value(), registry.lookupVerb("grab").value());
}

TEST(VerbRecognitionDirections) {
    VerbRegistry registry;
    
    // Test that direction words are registered as verbs (V_WALK)
    ASSERT_TRUE(registry.lookupVerb("north").has_value());
    ASSERT_TRUE(registry.lookupVerb("n").has_value());
    ASSERT_TRUE(registry.lookupVerb("south").has_value());
    ASSERT_TRUE(registry.lookupVerb("s").has_value());
    ASSERT_TRUE(registry.lookupVerb("east").has_value());
    ASSERT_TRUE(registry.lookupVerb("e").has_value());
    ASSERT_TRUE(registry.lookupVerb("west").has_value());
    ASSERT_TRUE(registry.lookupVerb("w").has_value());
    ASSERT_TRUE(registry.lookupVerb("up").has_value());
    ASSERT_TRUE(registry.lookupVerb("u").has_value());
    ASSERT_TRUE(registry.lookupVerb("down").has_value());
    ASSERT_TRUE(registry.lookupVerb("d").has_value());
}

TEST(VerbRecognitionCaseInsensitivity) {
    VerbRegistry registry;
    
    // Test case variations
    ASSERT_TRUE(registry.lookupVerb("take").has_value());
    ASSERT_TRUE(registry.lookupVerb("TAKE").has_value());
    ASSERT_TRUE(registry.lookupVerb("Take").has_value());
    ASSERT_TRUE(registry.lookupVerb("TaKe").has_value());
    
    // All should return same verb ID
    auto v1 = registry.lookupVerb("take").value();
    auto v2 = registry.lookupVerb("TAKE").value();
    auto v3 = registry.lookupVerb("Take").value();
    ASSERT_EQ(v1, v2);
    ASSERT_EQ(v1, v3);
}

// Test object recognition edge cases
TEST(ObjectRecognitionNoObjects) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    Parser parser;
    std::vector<std::string> words = {"lamp"};
    auto matches = parser.findObjects(words);
    
    ASSERT_EQ(matches.size(), 0);
    
    g.reset();
}

TEST(ObjectRecognitionEmptyWord) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    Parser parser;
    std::vector<std::string> words = {""};
    auto matches = parser.findObjects(words);
    
    ASSERT_EQ(matches.size(), 0);
    
    g.reset();
}

TEST(ObjectRecognitionMultipleMatches) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple objects with same synonym
    auto lamp1 = std::make_unique<ZObject>(1, "lamp");
    lamp1->addSynonym("lamp");
    lamp1->moveTo(&testRoom);
    g.registerObject(1, std::move(lamp1));
    
    auto lamp2 = std::make_unique<ZObject>(2, "lamp");
    lamp2->addSynonym("lamp");
    lamp2->moveTo(&testRoom);
    g.registerObject(2, std::move(lamp2));
    
    Parser parser;
    std::vector<std::string> words = {"lamp"};
    auto matches = parser.findObjects(words);
    
    ASSERT_EQ(matches.size(), 2);
    
    g.reset();
}

TEST(ObjectRecognitionInvisibleObjects) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create invisible object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::INVISIBLE);
    lamp->moveTo(&testRoom);
    g.registerObject(1, std::move(lamp));
    
    Parser parser;
    std::vector<std::string> words = {"lamp"};
    auto matches = parser.findObjects(words);
    
    // Invisible objects should not be found
    ASSERT_EQ(matches.size(), 0);
    
    g.reset();
}

TEST(ObjectRecognitionClosedContainerContents) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container
    auto box = std::make_unique<ZObject>(10, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    // Not setting OPENBIT - it's closed
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(10, std::move(box));
    
    // Create object in closed container
    auto coin = std::make_unique<ZObject>(1, "coin");
    coin->addSynonym("coin");
    coin->moveTo(boxPtr);
    g.registerObject(1, std::move(coin));
    
    Parser parser;
    std::vector<std::string> words = {"coin"};
    auto matches = parser.findObjects(words);
    
    // Object in closed container should not be visible
    ASSERT_EQ(matches.size(), 0);
    
    g.reset();
}

TEST(ObjectRecognitionTransparentContainerContents) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create transparent container (closed but transparent)
    auto jar = std::make_unique<ZObject>(10, "jar");
    jar->addSynonym("jar");
    jar->setFlag(ObjectFlag::CONTBIT);
    jar->setFlag(ObjectFlag::TRANSBIT);  // Transparent
    // Not setting OPENBIT - it's closed
    jar->moveTo(&testRoom);
    ZObject* jarPtr = jar.get();
    g.registerObject(10, std::move(jar));
    
    // Create object in transparent container
    auto coin = std::make_unique<ZObject>(1, "coin");
    coin->addSynonym("coin");
    coin->moveTo(jarPtr);
    ZObject* coinPtr = coin.get();
    g.registerObject(1, std::move(coin));
    
    Parser parser;
    std::vector<std::string> words = {"coin"};
    auto matches = parser.findObjects(words);
    
    // Object in transparent container should be visible
    ASSERT_EQ(matches.size(), 1);
    ASSERT_EQ(matches[0], coinPtr);
    
    g.reset();
}

// Test syntax pattern matching edge cases
TEST(SyntaxPatternEmptyPattern) {
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;  // Empty pattern
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    std::vector<std::string> tokens = {"take", "lamp"};
    // Empty pattern should not match anything
    ASSERT_FALSE(syntaxPattern.matches(tokens));
}

TEST(SyntaxPatternTooFewTokens) {
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    // Only one token, but pattern requires two
    std::vector<std::string> tokens = {"take"};
    ASSERT_FALSE(syntaxPattern.matches(tokens));
}

TEST(SyntaxPatternTooManyTokens) {
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    // Too many tokens
    std::vector<std::string> tokens = {"take", "lamp", "extra", "words"};
    ASSERT_FALSE(syntaxPattern.matches(tokens));
}

TEST(SyntaxPatternAllOptional) {
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    Element verb(ElementType::VERB);
    verb.optional = true;
    pattern.push_back(verb);
    
    Element obj(ElementType::OBJECT);
    obj.optional = true;
    pattern.push_back(obj);
    
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    // Empty tokens should match all-optional pattern
    std::vector<std::string> tokens;
    ASSERT_TRUE(syntaxPattern.matches(tokens));
}

TEST(SyntaxPatternMultiplePrepositions) {
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    
    Element prep(ElementType::PREPOSITION, std::vector<std::string>{"in", "on", "under", "behind"});
    pattern.push_back(prep);
    pattern.push_back(Element(ElementType::OBJECT));
    
    SyntaxPattern syntaxPattern(V_PUT, pattern);
    
    // Test each preposition
    ASSERT_TRUE(syntaxPattern.matches({"put", "lamp", "in", "box"}));
    ASSERT_TRUE(syntaxPattern.matches({"put", "lamp", "on", "table"}));
    ASSERT_TRUE(syntaxPattern.matches({"put", "lamp", "under", "rug"}));
    ASSERT_TRUE(syntaxPattern.matches({"put", "lamp", "behind", "door"}));
    
    // Test invalid preposition
    ASSERT_FALSE(syntaxPattern.matches({"put", "lamp", "with", "box"}));
}

// Test complete parsing flow
TEST(ParsingFlowSimpleCommand) {
    auto& g = Globals::instance();
    
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
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
    
    VerbRegistry registry;
    Parser parser(&registry);
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
