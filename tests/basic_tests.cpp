#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/parser/syntax.h"
#include "../src/verbs/verbs.h"

// Test object system
TEST(ObjectCreation) {
    ZObject obj(1, "test object");
    ASSERT_EQ(obj.getId(), 1);
    ASSERT_EQ(obj.getDesc(), "test object");
}

TEST(ObjectFlags) {
    ZObject obj(1, "test");
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
    
    obj.setFlag(ObjectFlag::TAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    
    obj.clearFlag(ObjectFlag::TAKEBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
}

TEST(ObjectProperties) {
    ZObject obj(1, "test");
    constexpr PropertyId P_SIZE = 1;
    ASSERT_EQ(obj.getProperty(P_SIZE), 0);
    
    obj.setProperty(P_SIZE, 10);
    ASSERT_EQ(obj.getProperty(P_SIZE), 10);
}

TEST(ObjectContainment) {
    ZObject container(1, "container");
    ZObject item(2, "item");
    
    item.moveTo(&container);
    ASSERT_EQ(item.getLocation(), &container);
    ASSERT_EQ(container.getContents().size(), 1);
    ASSERT_EQ(container.getContents()[0], &item);
}

// Test room system
TEST(RoomCreation) {
    ZRoom room(1, "Test Room", "A test room.");
    ASSERT_EQ(room.getId(), 1);
    ASSERT_EQ(room.getDesc(), "Test Room");
}

TEST(RoomExits) {
    ZRoom room1(1, "Room 1", "");
    ZRoom room2(2, "Room 2", "");
    
    room1.setExit(Direction::NORTH, RoomExit(2));
    
    RoomExit* exit = room1.getExit(Direction::NORTH);
    ASSERT_TRUE(exit != nullptr);
    ASSERT_EQ(exit->targetRoom, 2);
}

TEST(BlockedExits) {
    ZRoom room(1, "Room", "");
    room.setExit(Direction::EAST, RoomExit("The door is locked."));
    
    RoomExit* exit = room.getExit(Direction::EAST);
    ASSERT_TRUE(exit != nullptr);
    ASSERT_EQ(exit->targetRoom, 0);
    ASSERT_EQ(exit->message, "The door is locked.");
}

// Test SyntaxPattern system
TEST(SyntaxPatternSimple) {
    // Test simple VERB OBJECT pattern (e.g., "TAKE LAMP")
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    // Test matching
    std::vector<std::string> tokens = {"take", "lamp"};
    ASSERT_TRUE(syntaxPattern.matches(tokens));
    
    // Test non-matching (missing object)
    std::vector<std::string> tokens2 = {"take"};
    ASSERT_FALSE(syntaxPattern.matches(tokens2));
}

TEST(SyntaxPatternWithPreposition) {
    // Test VERB OBJECT PREP OBJECT pattern (e.g., "PUT LAMP IN BOX")
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    Element prepElement(ElementType::PREPOSITION, std::vector<std::string>{"in", "on"});
    pattern.push_back(prepElement);
    pattern.push_back(Element(ElementType::OBJECT));
    
    SyntaxPattern syntaxPattern(V_PUT, pattern);
    
    // Test matching with "in"
    std::vector<std::string> tokens1 = {"put", "lamp", "in", "box"};
    ASSERT_TRUE(syntaxPattern.matches(tokens1));
    
    // Test matching with "on"
    std::vector<std::string> tokens2 = {"put", "lamp", "on", "table"};
    ASSERT_TRUE(syntaxPattern.matches(tokens2));
    
    // Test non-matching (wrong preposition)
    std::vector<std::string> tokens3 = {"put", "lamp", "with", "box"};
    ASSERT_FALSE(syntaxPattern.matches(tokens3));
}

TEST(SyntaxPatternOptionalElement) {
    // Test pattern with optional preposition
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    pattern.push_back(Element(ElementType::OBJECT));
    Element prepElement(ElementType::PREPOSITION, std::vector<std::string>{"with"});
    prepElement.optional = true;
    pattern.push_back(prepElement);
    Element obj2Element(ElementType::OBJECT);
    obj2Element.optional = true;  // Second object is also optional when preposition is optional
    pattern.push_back(obj2Element);
    
    SyntaxPattern syntaxPattern(V_ATTACK, pattern);
    
    // Test matching with optional preposition present
    std::vector<std::string> tokens1 = {"attack", "troll", "with", "sword"};
    ASSERT_TRUE(syntaxPattern.matches(tokens1));
    
    // Test matching with optional preposition absent
    std::vector<std::string> tokens2 = {"attack", "troll"};
    ASSERT_TRUE(syntaxPattern.matches(tokens2));
}

TEST(SyntaxPatternFlagRequirement) {
    // Test pattern with object flag requirement
    using Element = SyntaxPattern::Element;
    using ElementType = SyntaxPattern::ElementType;
    
    std::vector<Element> pattern;
    pattern.push_back(Element(ElementType::VERB));
    Element objElement(ElementType::OBJECT, ObjectFlag::TAKEBIT);
    pattern.push_back(objElement);
    
    SyntaxPattern syntaxPattern(V_TAKE, pattern);
    
    // Create test objects
    ZObject takeableObj(1, "lamp");
    takeableObj.setFlag(ObjectFlag::TAKEBIT);
    
    ZObject nonTakeableObj(2, "house");
    // No TAKEBIT flag
    
    // Test apply with object finder
    auto objectFinder = [&](const std::string& name, std::optional<ObjectFlag> flag) -> ZObject* {
        if (name == "lamp") return &takeableObj;
        if (name == "house") return &nonTakeableObj;
        return nullptr;
    };
    
    // Test with takeable object
    std::vector<std::string> tokens1 = {"take", "lamp"};
    ParseResult result1 = syntaxPattern.apply(tokens1, objectFinder);
    ASSERT_TRUE(result1.success);
    ASSERT_EQ(result1.prso, &takeableObj);
    
    // Test with non-takeable object (should fail flag check)
    std::vector<std::string> tokens2 = {"take", "house"};
    ParseResult result2 = syntaxPattern.apply(tokens2, objectFinder);
    ASSERT_FALSE(result2.success);
}

// Main test runner
int main() {
    std::cout << "Running Zork C++ Tests\n";
    std::cout << "======================\n\n";
    
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
    
    std::cout << "\n======================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
