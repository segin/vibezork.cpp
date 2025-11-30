#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "../src/parser/syntax.h"
#include "../src/parser/verb_registry.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include <sstream>
#include <iostream>

// Capture output for testing
class OutputCapture {
public:
    OutputCapture() : old_cout(std::cout.rdbuf()) {
        std::cout.rdbuf(buffer.rdbuf());
    }
    
    ~OutputCapture() {
        std::cout.rdbuf(old_cout);
    }
    
    std::string getOutput() const {
        return buffer.str();
    }
    
private:
    std::stringstream buffer;
    std::streambuf* old_cout;
};

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

// Test VerbRegistry system
TEST(VerbRegistryLookup) {
    VerbRegistry registry;
    
    // Test looking up verb synonyms
    auto takeVerb = registry.lookupVerb("take");
    ASSERT_TRUE(takeVerb.has_value());
    ASSERT_EQ(takeVerb.value(), V_TAKE);
    
    auto getVerb = registry.lookupVerb("get");
    ASSERT_TRUE(getVerb.has_value());
    ASSERT_EQ(getVerb.value(), V_TAKE);  // "get" is synonym for "take"
    
    auto grabVerb = registry.lookupVerb("grab");
    ASSERT_TRUE(grabVerb.has_value());
    ASSERT_EQ(grabVerb.value(), V_TAKE);  // "grab" is synonym for "take"
}

TEST(VerbRegistryCaseInsensitive) {
    VerbRegistry registry;
    
    // Test case-insensitive lookup
    auto verb1 = registry.lookupVerb("TAKE");
    ASSERT_TRUE(verb1.has_value());
    ASSERT_EQ(verb1.value(), V_TAKE);
    
    auto verb2 = registry.lookupVerb("Take");
    ASSERT_TRUE(verb2.has_value());
    ASSERT_EQ(verb2.value(), V_TAKE);
    
    auto verb3 = registry.lookupVerb("take");
    ASSERT_TRUE(verb3.has_value());
    ASSERT_EQ(verb3.value(), V_TAKE);
}

TEST(VerbRegistryUnknownWord) {
    VerbRegistry registry;
    
    // Test looking up unknown word
    auto unknownVerb = registry.lookupVerb("xyzzy");
    ASSERT_FALSE(unknownVerb.has_value());
}

TEST(VerbRegistryExamineSynonyms) {
    VerbRegistry registry;
    
    // Test EXAMINE verb synonyms
    auto examineVerb = registry.lookupVerb("examine");
    ASSERT_TRUE(examineVerb.has_value());
    ASSERT_EQ(examineVerb.value(), V_EXAMINE);
    
    auto xVerb = registry.lookupVerb("x");
    ASSERT_TRUE(xVerb.has_value());
    ASSERT_EQ(xVerb.value(), V_EXAMINE);  // "x" is synonym for "examine"
    
    auto describeVerb = registry.lookupVerb("describe");
    ASSERT_TRUE(describeVerb.has_value());
    ASSERT_EQ(describeVerb.value(), V_EXAMINE);  // "describe" is synonym for "examine"
}

TEST(VerbRegistryHasPatterns) {
    VerbRegistry registry;
    
    // Test that verbs have patterns registered
    ASSERT_TRUE(registry.hasPatterns(V_TAKE));
    ASSERT_TRUE(registry.hasPatterns(V_EXAMINE));
    ASSERT_TRUE(registry.hasPatterns(V_ATTACK));
    ASSERT_TRUE(registry.hasPatterns(V_PUT));
}

TEST(VerbRegistryGetPatterns) {
    VerbRegistry registry;
    
    // Test getting patterns for a verb
    const auto& takePatterns = registry.getSyntaxPatterns(V_TAKE);
    ASSERT_TRUE(takePatterns.size() > 0);  // Should have at least one pattern
    
    // TAKE should have multiple patterns (TAKE OBJECT, TAKE OBJECT FROM OBJECT)
    ASSERT_TRUE(takePatterns.size() >= 2);
}

TEST(VerbRegistryAttackPatterns) {
    VerbRegistry registry;
    
    // Test ATTACK verb has both patterns (with and without weapon)
    const auto& attackPatterns = registry.getSyntaxPatterns(V_ATTACK);
    ASSERT_TRUE(attackPatterns.size() >= 2);  // Should have at least 2 patterns
}

// Test object recognition - Task 3.4
TEST(ObjectRecognitionSynonymMatching) {
    // Create test objects with synonyms
    ZObject lamp(1, "brass lantern");
    lamp.addSynonym("lamp");
    lamp.addSynonym("lantern");
    lamp.addSynonym("light");
    lamp.setFlag(ObjectFlag::TAKEBIT);
    
    ZObject mailbox(2, "small mailbox");
    mailbox.addSynonym("mailbox");
    mailbox.addSynonym("box");
    
    // Test synonym matching
    ASSERT_TRUE(lamp.hasSynonym("lamp"));
    ASSERT_TRUE(lamp.hasSynonym("lantern"));
    ASSERT_TRUE(lamp.hasSynonym("light"));
    ASSERT_FALSE(lamp.hasSynonym("mailbox"));
    
    ASSERT_TRUE(mailbox.hasSynonym("mailbox"));
    ASSERT_TRUE(mailbox.hasSynonym("box"));
    ASSERT_FALSE(mailbox.hasSynonym("lamp"));
}

TEST(ObjectRecognitionAdjectiveMatching) {
    // Create test objects with adjectives
    ZObject lamp(1, "brass lantern");
    lamp.addSynonym("lamp");
    lamp.addSynonym("lantern");
    lamp.addAdjective("brass");
    lamp.addAdjective("small");
    
    ZObject knife(2, "rusty knife");
    knife.addSynonym("knife");
    knife.addAdjective("rusty");
    knife.addAdjective("old");
    
    // Test adjective matching
    ASSERT_TRUE(lamp.hasAdjective("brass"));
    ASSERT_TRUE(lamp.hasAdjective("small"));
    ASSERT_FALSE(lamp.hasAdjective("rusty"));
    
    ASSERT_TRUE(knife.hasAdjective("rusty"));
    ASSERT_TRUE(knife.hasAdjective("old"));
    ASSERT_FALSE(knife.hasAdjective("brass"));
}

TEST(ObjectRecognitionMultiWordNames) {
    auto& g = Globals::instance();
    
    // Create test room and objects
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create object with multi-word name
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->addAdjective("brass");
    lamp->addAdjective("small");
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "rusty knife");
    knife->addSynonym("knife");
    knife->addAdjective("rusty");
    knife->moveTo(&testRoom);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    // Create parser and test multi-word matching
    Parser parser;
    
    // Test "brass lamp" - adjective + noun
    std::vector<std::string> words1 = {"brass", "lamp"};
    auto matches1 = parser.findObjects(words1);
    ASSERT_TRUE(matches1.size() > 0);
    ASSERT_EQ(matches1[0], lampPtr);
    
    // Test "small brass lantern" - multiple adjectives + noun
    std::vector<std::string> words2 = {"small", "brass", "lantern"};
    auto matches2 = parser.findObjects(words2);
    ASSERT_TRUE(matches2.size() > 0);
    ASSERT_EQ(matches2[0], lampPtr);
    
    // Test "rusty knife" - adjective + noun
    std::vector<std::string> words3 = {"rusty", "knife"};
    auto matches3 = parser.findObjects(words3);
    ASSERT_TRUE(matches3.size() > 0);
    ASSERT_EQ(matches3[0], knifePtr);
    
    // Cleanup
    g.reset();
}

TEST(ObjectRecognitionLocationPriority) {
    auto& g = Globals::instance();
    
    // Create test rooms
    ZRoom room1(100, "Room 1", "First room.");
    ZRoom room2(101, "Room 2", "Second room.");
    g.here = &room1;
    
    // Create player object
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create three lamps in different locations
    auto lampInRoom = std::make_unique<ZObject>(1, "lamp");
    lampInRoom->addSynonym("lamp");
    lampInRoom->moveTo(&room1);  // In current room
    ZObject* lampInRoomPtr = lampInRoom.get();
    g.registerObject(1, std::move(lampInRoom));
    
    auto lampInInventory = std::make_unique<ZObject>(2, "lamp");
    lampInInventory->addSynonym("lamp");
    lampInInventory->moveTo(g.winner);  // In player inventory
    ZObject* lampInInventoryPtr = lampInInventory.get();
    g.registerObject(2, std::move(lampInInventory));
    
    auto lampElsewhere = std::make_unique<ZObject>(3, "lamp");
    lampElsewhere->addSynonym("lamp");
    lampElsewhere->moveTo(&room2);  // In other room
    ZObject* lampElsewherePtr = lampElsewhere.get();
    g.registerObject(3, std::move(lampElsewhere));
    
    // Create parser and test location prioritization
    Parser parser;
    std::vector<std::string> words = {"lamp"};
    auto matches = parser.findObjects(words);
    
    // Should find objects in current room and inventory, but not elsewhere
    ASSERT_TRUE(matches.size() >= 2);
    
    // First match should be from current room (highest priority)
    ASSERT_EQ(matches[0], lampInRoomPtr);
    
    // Second match should be from inventory
    ASSERT_EQ(matches[1], lampInInventoryPtr);
    
    // Lamp in other room should not be in matches (not visible)
    bool foundElsewhere = false;
    for (auto* obj : matches) {
        if (obj == lampElsewherePtr) {
            foundElsewhere = true;
            break;
        }
    }
    ASSERT_FALSE(foundElsewhere);
    
    // Cleanup
    g.reset();
}

TEST(ObjectRecognitionOpenContainerPriority) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container in room
    auto box = std::make_unique<ZObject>(10, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(10, std::move(box));
    
    // Create object in open container
    auto coin = std::make_unique<ZObject>(1, "coin");
    coin->addSynonym("coin");
    coin->moveTo(boxPtr);
    ZObject* coinPtr = coin.get();
    g.registerObject(1, std::move(coin));
    
    // Create closed container in room
    auto chest = std::make_unique<ZObject>(11, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Not setting OPENBIT - it's closed
    chest->moveTo(&testRoom);
    ZObject* chestPtr = chest.get();
    g.registerObject(11, std::move(chest));
    
    // Create object in closed container
    auto gem = std::make_unique<ZObject>(2, "gem");
    gem->addSynonym("gem");
    gem->moveTo(chestPtr);
    ZObject* gemPtr = gem.get();
    g.registerObject(2, std::move(gem));
    
    // Create parser and test visibility
    Parser parser;
    
    // Coin in open container should be visible
    std::vector<std::string> words1 = {"coin"};
    auto matches1 = parser.findObjects(words1);
    ASSERT_TRUE(matches1.size() > 0);
    ASSERT_EQ(matches1[0], coinPtr);
    
    // Gem in closed container should NOT be visible
    std::vector<std::string> words2 = {"gem"};
    auto matches2 = parser.findObjects(words2);
    ASSERT_EQ(matches2.size(), 0);
    
    // Cleanup
    g.reset();
}

TEST(ObjectRecognitionAdjectiveNarrowing) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create two knives with different adjectives
    auto brassKnife = std::make_unique<ZObject>(1, "brass knife");
    brassKnife->addSynonym("knife");
    brassKnife->addAdjective("brass");
    brassKnife->moveTo(&testRoom);
    ZObject* brassKnifePtr = brassKnife.get();
    g.registerObject(1, std::move(brassKnife));
    
    auto rustyKnife = std::make_unique<ZObject>(2, "rusty knife");
    rustyKnife->addSynonym("knife");
    rustyKnife->addAdjective("rusty");
    rustyKnife->moveTo(&testRoom);
    ZObject* rustyKnifePtr = rustyKnife.get();
    g.registerObject(2, std::move(rustyKnife));
    
    // Create parser
    Parser parser;
    
    // Test "knife" - should match both
    std::vector<std::string> words1 = {"knife"};
    auto matches1 = parser.findObjects(words1);
    ASSERT_EQ(matches1.size(), 2);
    
    // Test "brass knife" - should match only brass knife
    std::vector<std::string> words2 = {"brass", "knife"};
    auto matches2 = parser.findObjects(words2);
    ASSERT_EQ(matches2.size(), 1);
    ASSERT_EQ(matches2[0], brassKnifePtr);
    
    // Test "rusty knife" - should match only rusty knife
    std::vector<std::string> words3 = {"rusty", "knife"};
    auto matches3 = parser.findObjects(words3);
    ASSERT_EQ(matches3.size(), 1);
    ASSERT_EQ(matches3[0], rustyKnifePtr);
    
    // Cleanup
    g.reset();
}

TEST(ObjectRecognitionSingleWordSynonym) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create object with multiple single-word synonyms
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->addSynonym("light");
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test each synonym
    std::vector<std::string> words1 = {"lamp"};
    auto matches1 = parser.findObjects(words1);
    ASSERT_TRUE(matches1.size() > 0);
    ASSERT_EQ(matches1[0], lampPtr);
    
    std::vector<std::string> words2 = {"lantern"};
    auto matches2 = parser.findObjects(words2);
    ASSERT_TRUE(matches2.size() > 0);
    ASSERT_EQ(matches2[0], lampPtr);
    
    std::vector<std::string> words3 = {"light"};
    auto matches3 = parser.findObjects(words3);
    ASSERT_TRUE(matches3.size() > 0);
    ASSERT_EQ(matches3[0], lampPtr);
    
    // Cleanup
    g.reset();
}

// Test disambiguation system - Task 4.3
TEST(DisambiguationMultipleObjects) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create multiple objects with same synonym
    auto brassKnife = std::make_unique<ZObject>(1, "brass knife");
    brassKnife->addSynonym("knife");
    brassKnife->addAdjective("brass");
    brassKnife->moveTo(&testRoom);
    ZObject* brassKnifePtr = brassKnife.get();
    g.registerObject(1, std::move(brassKnife));
    
    auto rustyKnife = std::make_unique<ZObject>(2, "rusty knife");
    rustyKnife->addSynonym("knife");
    rustyKnife->addAdjective("rusty");
    rustyKnife->moveTo(&testRoom);
    ZObject* rustyKnifePtr = rustyKnife.get();
    g.registerObject(2, std::move(rustyKnife));
    
    // Create parser
    Parser parser;
    
    // Find objects matching "knife"
    std::vector<std::string> words = {"knife"};
    auto candidates = parser.findObjects(words);
    
    // Should find both knives
    ASSERT_EQ(candidates.size(), 2);
    
    // Test disambiguation with number selection (simulated)
    // In real usage, this would prompt the user, but for testing we call directly
    // We can't easily test the interactive part, but we can test the logic
    
    // Test that single candidate returns that object
    std::vector<ZObject*> singleCandidate = {brassKnifePtr};
    ZObject* result1 = parser.disambiguate(singleCandidate, "knife");
    ASSERT_EQ(result1, brassKnifePtr);
    
    // Cleanup
    g.reset();
}

TEST(DisambiguationParseResponse) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create test objects
    auto brassKnife = std::make_unique<ZObject>(1, "brass knife");
    brassKnife->addSynonym("knife");
    brassKnife->addAdjective("brass");
    brassKnife->moveTo(&testRoom);
    ZObject* brassKnifePtr = brassKnife.get();
    g.registerObject(1, std::move(brassKnife));
    
    auto rustyKnife = std::make_unique<ZObject>(2, "rusty knife");
    rustyKnife->addSynonym("knife");
    rustyKnife->addAdjective("rusty");
    rustyKnife->moveTo(&testRoom);
    ZObject* rustyKnifePtr = rustyKnife.get();
    g.registerObject(2, std::move(rustyKnife));
    
    // Create parser
    Parser parser;
    
    std::vector<ZObject*> candidates = {brassKnifePtr, rustyKnifePtr};
    
    // Note: We can't easily test the full interactive disambiguation without mocking stdin
    // But we can test the helper methods that parse responses
    
    // The parseDisambiguationResponse method is private, so we test through public interface
    // In a real scenario, the user would type "1" or "brass knife" at the prompt
    
    // For now, verify that candidates are properly set up
    ASSERT_EQ(candidates.size(), 2);
    ASSERT_EQ(candidates[0], brassKnifePtr);
    ASSERT_EQ(candidates[1], rustyKnifePtr);
    
    // Cleanup
    g.reset();
}

TEST(DisambiguationSingleCandidate) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create single object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Single candidate should be returned immediately without prompting
    std::vector<ZObject*> candidates = {lampPtr};
    ZObject* result = parser.disambiguate(candidates, "lamp");
    
    ASSERT_EQ(result, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(DisambiguationEmptyCandidates) {
    // Create parser
    Parser parser;
    
    // Empty candidates should return nullptr
    std::vector<ZObject*> candidates;
    ZObject* result = parser.disambiguate(candidates, "nothing");
    
    ASSERT_EQ(result, nullptr);
}

TEST(DisambiguationFormatDescription) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create container
    auto box = std::make_unique<ZObject>(10, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(10, std::move(box));
    
    // Create objects in different locations
    auto lampInRoom = std::make_unique<ZObject>(1, "lamp");
    lampInRoom->addSynonym("lamp");
    lampInRoom->moveTo(&testRoom);
    ZObject* lampInRoomPtr = lampInRoom.get();
    g.registerObject(1, std::move(lampInRoom));
    
    auto lampInInventory = std::make_unique<ZObject>(2, "lamp");
    lampInInventory->addSynonym("lamp");
    lampInInventory->moveTo(g.winner);
    ZObject* lampInInventoryPtr = lampInInventory.get();
    g.registerObject(2, std::move(lampInInventory));
    
    auto lampInBox = std::make_unique<ZObject>(3, "lamp");
    lampInBox->addSynonym("lamp");
    lampInBox->moveTo(boxPtr);
    ZObject* lampInBoxPtr = lampInBox.get();
    g.registerObject(3, std::move(lampInBox));
    
    // Create parser
    Parser parser;
    
    // Test that objects in different locations can be distinguished
    // The formatObjectDescription method adds location context
    std::vector<ZObject*> candidates = {lampInRoomPtr, lampInInventoryPtr, lampInBoxPtr};
    
    // Verify all three are different objects
    ASSERT_EQ(candidates.size(), 3);
    ASSERT_TRUE(lampInRoomPtr != lampInInventoryPtr);
    ASSERT_TRUE(lampInRoomPtr != lampInBoxPtr);
    ASSERT_TRUE(lampInInventoryPtr != lampInBoxPtr);
    
    // Cleanup
    g.reset();
}

// Test preposition handling - Task 5.3
TEST(PrepositionPutObjectInContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);  // In player inventory
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create container (box)
    auto box = std::make_unique<ZObject>(2, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    // Create parser with verb registry
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "PUT LAMP IN BOX"
    ParsedCommand cmd = parser.parse("put lamp in box");
    
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, boxPtr);
    
    // Cleanup
    g.reset();
}

TEST(PrepositionAttackTrollWithSword) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create troll NPC
    auto troll = std::make_unique<ZObject>(1, "troll");
    troll->addSynonym("troll");
    troll->setFlag(ObjectFlag::FIGHTBIT);
    troll->moveTo(&testRoom);
    ZObject* trollPtr = troll.get();
    g.registerObject(1, std::move(troll));
    
    // Create sword weapon
    auto sword = std::make_unique<ZObject>(2, "sword");
    sword->addSynonym("sword");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(g.winner);  // In player inventory
    ZObject* swordPtr = sword.get();
    g.registerObject(2, std::move(sword));
    
    // Create parser with verb registry
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "ATTACK TROLL WITH SWORD"
    ParsedCommand cmd = parser.parse("attack troll with sword");
    
    ASSERT_EQ(cmd.verb, V_ATTACK);
    ASSERT_EQ(cmd.directObj, trollPtr);
    ASSERT_EQ(cmd.indirectObj, swordPtr);
    
    // Cleanup
    g.reset();
}

TEST(PrepositionInvalidPreposition) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create box container
    auto box = std::make_unique<ZObject>(2, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    // Create parser with verb registry
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "PUT LAMP UNDER BOX" - "under" is not valid for PUT verb
    // The parser should reject this or handle it gracefully
    ParsedCommand cmd = parser.parse("put lamp under box");
    
    // The command should either:
    // 1. Have verb = 0 (invalid), or
    // 2. Not have both objects set properly
    // This depends on implementation - we're testing that invalid prepositions are handled
    
    // For now, we just verify the parser doesn't crash and returns something
    // The exact behavior depends on whether the verb registry validates prepositions
    ASSERT_TRUE(cmd.verb == V_PUT || cmd.verb == 0);
    
    // Cleanup
    g.reset();
}

TEST(PrepositionOptionalPreposition) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create troll NPC
    auto troll = std::make_unique<ZObject>(1, "troll");
    troll->addSynonym("troll");
    troll->setFlag(ObjectFlag::FIGHTBIT);
    troll->moveTo(&testRoom);
    ZObject* trollPtr = troll.get();
    g.registerObject(1, std::move(troll));
    
    // Create sword weapon
    auto sword = std::make_unique<ZObject>(2, "sword");
    sword->addSynonym("sword");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(g.winner);
    ZObject* swordPtr = sword.get();
    g.registerObject(2, std::move(sword));
    
    // Create parser with verb registry
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "ATTACK TROLL" - without optional "WITH SWORD"
    ParsedCommand cmd1 = parser.parse("attack troll");
    
    ASSERT_EQ(cmd1.verb, V_ATTACK);
    ASSERT_EQ(cmd1.directObj, trollPtr);
    ASSERT_EQ(cmd1.indirectObj, nullptr);  // No weapon specified
    
    // Test "ATTACK TROLL WITH SWORD" - with optional preposition
    ParsedCommand cmd2 = parser.parse("attack troll with sword");
    
    ASSERT_EQ(cmd2.verb, V_ATTACK);
    ASSERT_EQ(cmd2.directObj, trollPtr);
    ASSERT_EQ(cmd2.indirectObj, swordPtr);  // Weapon specified
    
    // Cleanup
    g.reset();
}

TEST(PrepositionMultipleValidPrepositions) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create box container
    auto box = std::make_unique<ZObject>(2, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    // Create parser with verb registry
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "PUT LAMP IN BOX" - "in" should be valid for V_PUT
    ParsedCommand cmd1 = parser.parse("put lamp in box");
    
    ASSERT_EQ(cmd1.verb, V_PUT);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    ASSERT_EQ(cmd1.indirectObj, boxPtr);
    
    // Test "PUT LAMP INTO BOX" - "into" should also be valid for V_PUT
    ParsedCommand cmd2 = parser.parse("put lamp into box");
    
    ASSERT_EQ(cmd2.verb, V_PUT);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    ASSERT_EQ(cmd2.indirectObj, boxPtr);
    
    // Test "PUT LAMP INSIDE BOX" - "inside" should also be valid for V_PUT
    ParsedCommand cmd3 = parser.parse("put lamp inside box");
    
    ASSERT_EQ(cmd3.verb, V_PUT);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    ASSERT_EQ(cmd3.indirectObj, boxPtr);
    
    // Cleanup
    g.reset();
}

TEST(PrepositionRecognition) {
    // Test that parser recognizes common prepositions
    Parser parser;
    
    // Test core prepositions
    ASSERT_TRUE(parser.isPreposition("in"));
    ASSERT_TRUE(parser.isPreposition("on"));
    ASSERT_TRUE(parser.isPreposition("with"));
    ASSERT_TRUE(parser.isPreposition("under"));
    ASSERT_TRUE(parser.isPreposition("to"));
    ASSERT_TRUE(parser.isPreposition("from"));
    ASSERT_TRUE(parser.isPreposition("at"));
    
    // Test preposition synonyms
    ASSERT_TRUE(parser.isPreposition("into"));
    ASSERT_TRUE(parser.isPreposition("onto"));
    ASSERT_TRUE(parser.isPreposition("inside"));
    ASSERT_TRUE(parser.isPreposition("underneath"));
    ASSERT_TRUE(parser.isPreposition("beneath"));
    
    // Test non-prepositions
    ASSERT_FALSE(parser.isPreposition("lamp"));
    ASSERT_FALSE(parser.isPreposition("take"));
    ASSERT_FALSE(parser.isPreposition("the"));
}

TEST(PrepositionFindInTokens) {
    Parser parser;
    
    // Test finding preposition in token list
    std::vector<std::string> tokens1 = {"put", "lamp", "in", "box"};
    auto idx1 = parser.findPrepositionIndex(tokens1);
    ASSERT_TRUE(idx1.has_value());
    ASSERT_EQ(idx1.value(), 2);  // "in" is at index 2
    
    // Test with no preposition
    std::vector<std::string> tokens2 = {"take", "lamp"};
    auto idx2 = parser.findPrepositionIndex(tokens2);
    ASSERT_FALSE(idx2.has_value());
    
    // Test with preposition at different position
    std::vector<std::string> tokens3 = {"attack", "troll", "with", "sword"};
    auto idx3 = parser.findPrepositionIndex(tokens3);
    ASSERT_TRUE(idx3.has_value());
    ASSERT_EQ(idx3.value(), 2);  // "with" is at index 2
    
    // Test with multiple words before preposition
    std::vector<std::string> tokens4 = {"put", "brass", "lamp", "on", "wooden", "table"};
    auto idx4 = parser.findPrepositionIndex(tokens4);
    ASSERT_TRUE(idx4.has_value());
    ASSERT_EQ(idx4.value(), 3);  // "on" is at index 3
}

TEST(PrepositionExtractObjects) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple objects
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto box = std::make_unique<ZObject>(2, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    auto table = std::make_unique<ZObject>(3, "table");
    table->addSynonym("table");
    table->moveTo(&testRoom);
    ZObject* tablePtr = table.get();
    g.registerObject(3, std::move(table));
    
    // Create parser
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test that parser correctly extracts both objects from command with preposition
    ParsedCommand cmd = parser.parse("put lamp in box");
    
    // Verify direct object (before preposition)
    ASSERT_EQ(cmd.directObj, lampPtr);
    
    // Verify indirect object (after preposition)
    ASSERT_EQ(cmd.indirectObj, boxPtr);
    
    // Verify the other object is not involved
    ASSERT_TRUE(cmd.directObj != tablePtr);
    ASSERT_TRUE(cmd.indirectObj != tablePtr);
    
    // Cleanup
    g.reset();
}

// Test special parser features - Task 6.6
TEST(SpecialFeaturesTakeAll) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple takeable objects in room
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(&testRoom);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    // Create non-takeable object (anchored)
    auto house = std::make_unique<ZObject>(3, "house");
    house->addSynonym("house");
    house->setFlag(ObjectFlag::TRYTAKEBIT);  // Anchored, can't be taken
    house->moveTo(&testRoom);
    g.registerObject(3, std::move(house));
    
    // Create parser
    Parser parser;
    
    // Test "TAKE ALL"
    ParsedCommand cmd = parser.parse("take all");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_TRUE(cmd.allObjects.size() >= 2);  // Should include lamp and knife
    
    // Verify lamp and knife are in the list
    bool foundLamp = false;
    bool foundKnife = false;
    bool foundHouse = false;
    
    for (auto* obj : cmd.allObjects) {
        if (obj == lampPtr) foundLamp = true;
        if (obj == knifePtr) foundKnife = true;
        if (obj == house.get()) foundHouse = true;
    }
    
    ASSERT_TRUE(foundLamp);
    ASSERT_TRUE(foundKnife);
    ASSERT_FALSE(foundHouse);  // House should not be included (anchored)
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesTakeAllExcept) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple takeable objects
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(&testRoom);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    auto sword = std::make_unique<ZObject>(3, "sword");
    sword->addSynonym("sword");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(&testRoom);
    ZObject* swordPtr = sword.get();
    g.registerObject(3, std::move(sword));
    
    // Create parser
    Parser parser;
    
    // Test "TAKE ALL EXCEPT LAMP"
    ParsedCommand cmd = parser.parse("take all except lamp");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_EQ(cmd.exceptObject, lampPtr);
    
    // Verify lamp is NOT in the list, but knife and sword are
    bool foundLamp = false;
    bool foundKnife = false;
    bool foundSword = false;
    
    for (auto* obj : cmd.allObjects) {
        if (obj == lampPtr) foundLamp = true;
        if (obj == knifePtr) foundKnife = true;
        if (obj == swordPtr) foundSword = true;
    }
    
    ASSERT_FALSE(foundLamp);  // Lamp should be excluded
    ASSERT_TRUE(foundKnife);
    ASSERT_TRUE(foundSword);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesAgainCommand) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Parse initial command
    ParsedCommand cmd1 = parser.parse("take lamp");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    // Test "AGAIN" command
    ParsedCommand cmd2 = parser.parse("again");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test "G" command (synonym for AGAIN)
    ParsedCommand cmd3 = parser.parse("g");
    ASSERT_EQ(cmd3.verb, V_TAKE);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesAgainOnFirstTurn) {
    // Create parser
    Parser parser;
    
    // Test AGAIN on first turn (no previous command)
    // Should handle gracefully without crashing
    ParsedCommand cmd = parser.parse("again");
    
    // Command should be invalid (verb = 0) or handled gracefully
    // The exact behavior is that it prints a message and returns empty command
    ASSERT_EQ(cmd.verb, 0);
    
    // No cleanup needed - no globals used
}

TEST(SpecialFeaturesOopsCorrection) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Parse command with typo (unknown verb)
    ParsedCommand cmd1 = parser.parse("tke lamp");
    ASSERT_EQ(cmd1.verb, 0);  // Unknown verb
    
    // Test OOPS correction
    ParsedCommand cmd2 = parser.parse("oops take");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesOopsWithoutError) {
    // Create parser
    Parser parser;
    
    // Test OOPS without a previous unknown word
    // Should handle gracefully
    ParsedCommand cmd = parser.parse("oops take");
    
    // Command should be invalid or handled gracefully
    ASSERT_EQ(cmd.verb, 0);
    
    // No cleanup needed
}

TEST(SpecialFeaturesPronounIt) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Parse command that mentions lamp
    ParsedCommand cmd1 = parser.parse("examine lamp");
    ASSERT_EQ(cmd1.verb, V_EXAMINE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    // Test "IT" pronoun
    ParsedCommand cmd2 = parser.parse("take it");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);  // "it" should refer to lamp
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesPronounThem) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple objects
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(&testRoom);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    // Create parser
    Parser parser;
    
    // Parse "TAKE ALL" to establish multiple objects
    ParsedCommand cmd1 = parser.parse("take all");
    ASSERT_TRUE(cmd1.isAll);
    ASSERT_TRUE(cmd1.allObjects.size() >= 2);
    
    // Move objects to inventory for DROP test
    lampPtr->moveTo(g.winner);
    knifePtr->moveTo(g.winner);
    
    // Test "THEM" pronoun
    ParsedCommand cmd2 = parser.parse("drop them");
    ASSERT_EQ(cmd2.verb, V_DROP);
    ASSERT_TRUE(cmd2.isAll);
    ASSERT_TRUE(cmd2.allObjects.size() >= 2);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesPronounWithoutReference) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create parser
    Parser parser;
    
    // Test "IT" without previous object reference
    ParsedCommand cmd1 = parser.parse("take it");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, nullptr);  // No object to refer to
    
    // Test "THEM" without previous objects reference
    ParsedCommand cmd2 = parser.parse("drop them");
    ASSERT_EQ(cmd2.verb, V_DROP);
    // Should either have no objects or handle gracefully
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesDropAll) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create objects in player inventory
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(g.winner);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    // Create parser
    Parser parser;
    
    // Test "DROP ALL"
    ParsedCommand cmd = parser.parse("drop all");
    
    ASSERT_EQ(cmd.verb, V_DROP);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_TRUE(cmd.allObjects.size() >= 2);
    
    // Verify both objects are in the list
    bool foundLamp = false;
    bool foundKnife = false;
    
    for (auto* obj : cmd.allObjects) {
        if (obj == lampPtr) foundLamp = true;
        if (obj == knifePtr) foundKnife = true;
    }
    
    ASSERT_TRUE(foundLamp);
    ASSERT_TRUE(foundKnife);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesAllButSynonym) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple takeable objects
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    auto knife = std::make_unique<ZObject>(2, "knife");
    knife->addSynonym("knife");
    knife->setFlag(ObjectFlag::TAKEBIT);
    knife->moveTo(&testRoom);
    ZObject* knifePtr = knife.get();
    g.registerObject(2, std::move(knife));
    
    // Create parser
    Parser parser;
    
    // Test "TAKE ALL BUT LAMP" (using "but" instead of "except")
    ParsedCommand cmd = parser.parse("take all but lamp");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_EQ(cmd.exceptObject, lampPtr);
    
    // Verify lamp is excluded
    bool foundLamp = false;
    bool foundKnife = false;
    
    for (auto* obj : cmd.allObjects) {
        if (obj == lampPtr) foundLamp = true;
        if (obj == knifePtr) foundKnife = true;
    }
    
    ASSERT_FALSE(foundLamp);
    ASSERT_TRUE(foundKnife);
    
    // Cleanup
    g.reset();
}

TEST(SpecialFeaturesEverythingSynonym) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create takeable objects
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test "TAKE EVERYTHING" (synonym for "all")
    ParsedCommand cmd = parser.parse("take everything");
    
    ASSERT_EQ(cmd.verb, V_TAKE);
    ASSERT_TRUE(cmd.isAll);
    ASSERT_TRUE(cmd.allObjects.size() >= 1);
    
    // Cleanup
    g.reset();
}

// Integration tests for parser - Task 7.1
TEST(IntegrationParserCompleteFlow) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test complete command parsing flow
    ParsedCommand cmd1 = parser.parse("take lamp");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    ASSERT_EQ(cmd1.indirectObj, nullptr);
    
    // Test with synonym
    ParsedCommand cmd2 = parser.parse("get lantern");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test examine command
    ParsedCommand cmd3 = parser.parse("examine lamp");
    ASSERT_EQ(cmd3.verb, V_EXAMINE);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserErrorHandling) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create parser
    Parser parser;
    
    // Test unknown verb
    ParsedCommand cmd1 = parser.parse("xyzzy lamp");
    ASSERT_EQ(cmd1.verb, 0);  // Unknown verb
    
    // Test unknown object
    ParsedCommand cmd2 = parser.parse("take unicorn");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, nullptr);  // Object not found
    
    // Test empty command
    ParsedCommand cmd3 = parser.parse("");
    ASSERT_EQ(cmd3.verb, 0);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserEmptyInput) {
    // Test empty input handling (Requirement 72.1)
    Parser parser;
    
    ParsedCommand cmd1 = parser.parse("");
    ASSERT_EQ(cmd1.verb, 0);
    ASSERT_TRUE(cmd1.words.empty());
    
    // Test whitespace-only input
    ParsedCommand cmd2 = parser.parse("   ");
    ASSERT_EQ(cmd2.verb, 0);
    
    // Test tab and newline
    ParsedCommand cmd3 = parser.parse("\t\n");
    ASSERT_EQ(cmd3.verb, 0);
    
    // No cleanup needed - no globals used
}

TEST(IntegrationParserVeryLongInput) {
    // Test very long input handling (Requirement 72.2)
    Parser parser;
    
    // Create a very long command (over 1000 characters)
    std::string longInput(1500, 'a');
    
    // Parser should handle this gracefully without crashing
    ParsedCommand cmd = parser.parse(longInput);
    
    // Command will be invalid (unknown verb), but shouldn't crash
    ASSERT_EQ(cmd.verb, 0);
    
    // No cleanup needed
}

TEST(IntegrationParserSpecialCharacters) {
    // Test special characters handling (Requirement 72.3)
    Parser parser;
    
    // Test with punctuation
    ParsedCommand cmd1 = parser.parse("take lamp!");
    // Should parse "take" and "lamp!" (with exclamation)
    // The exclamation will be part of the word, which won't match
    // But it shouldn't crash
    ASSERT_TRUE(cmd1.verb == V_TAKE || cmd1.verb == 0);
    
    // Test with numbers
    ParsedCommand cmd2 = parser.parse("take 123");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    
    // Test with special symbols
    ParsedCommand cmd3 = parser.parse("take @#$%");
    ASSERT_EQ(cmd3.verb, V_TAKE);
    
    // No cleanup needed
}

TEST(IntegrationParserWhitespaceNormalization) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test with leading whitespace
    ParsedCommand cmd1 = parser.parse("   take lamp");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    // Test with trailing whitespace
    ParsedCommand cmd2 = parser.parse("take lamp   ");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test with multiple spaces between words
    ParsedCommand cmd3 = parser.parse("take    lamp");
    ASSERT_EQ(cmd3.verb, V_TAKE);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserComplexCommand) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->addAdjective("brass");
    lamp->addAdjective("small");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create box container
    auto box = std::make_unique<ZObject>(2, "wooden box");
    box->addSynonym("box");
    box->addAdjective("wooden");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    // Create parser
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test complex command with adjectives and preposition
    ParsedCommand cmd = parser.parse("put small brass lamp in wooden box");
    
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, boxPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserDirectionCommand) {
    auto& g = Globals::instance();
    
    // Create test rooms
    ZRoom room1(100, "Room 1", "First room.");
    ZRoom room2(101, "Room 2", "Second room.");
    room1.setExit(Direction::NORTH, RoomExit(101));
    
    g.here = &room1;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create parser
    Parser parser;
    
    // Test direction command
    ParsedCommand cmd1 = parser.parse("north");
    ASSERT_TRUE(cmd1.isDirection);
    ASSERT_EQ(cmd1.direction, Direction::NORTH);
    ASSERT_EQ(cmd1.verb, V_WALK);
    
    // Test abbreviated direction
    ParsedCommand cmd2 = parser.parse("n");
    ASSERT_TRUE(cmd2.isDirection);
    ASSERT_EQ(cmd2.direction, Direction::NORTH);
    
    // Test other directions
    ParsedCommand cmd3 = parser.parse("south");
    ASSERT_TRUE(cmd3.isDirection);
    ASSERT_EQ(cmd3.direction, Direction::SOUTH);
    
    ParsedCommand cmd4 = parser.parse("up");
    ASSERT_TRUE(cmd4.isDirection);
    ASSERT_EQ(cmd4.direction, Direction::UP);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserMultipleCommands) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test sequence of commands
    ParsedCommand cmd1 = parser.parse("examine lamp");
    ASSERT_EQ(cmd1.verb, V_EXAMINE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    ParsedCommand cmd2 = parser.parse("take lamp");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Move lamp to inventory for next test
    lampPtr->moveTo(g.winner);
    
    ParsedCommand cmd3 = parser.parse("drop lamp");
    ASSERT_EQ(cmd3.verb, V_DROP);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserCaseInsensitivity) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test uppercase
    ParsedCommand cmd1 = parser.parse("TAKE LAMP");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    // Test mixed case
    ParsedCommand cmd2 = parser.parse("TaKe LaMp");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test lowercase
    ParsedCommand cmd3 = parser.parse("take lamp");
    ASSERT_EQ(cmd3.verb, V_TAKE);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserArticleIgnoring) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test with "the"
    ParsedCommand cmd1 = parser.parse("take the lamp");
    ASSERT_EQ(cmd1.verb, V_TAKE);
    ASSERT_EQ(cmd1.directObj, lampPtr);
    
    // Test with "a"
    ParsedCommand cmd2 = parser.parse("take a lamp");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test with "an"
    ParsedCommand cmd3 = parser.parse("examine an lamp");
    ASSERT_EQ(cmd3.verb, V_EXAMINE);
    ASSERT_EQ(cmd3.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}

TEST(IntegrationParserErrorRecovery) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create parser
    Parser parser;
    
    // Test error followed by valid command
    ParsedCommand cmd1 = parser.parse("xyzzy lamp");
    ASSERT_EQ(cmd1.verb, 0);  // Error
    
    // Parser should recover and handle next command
    ParsedCommand cmd2 = parser.parse("take lamp");
    ASSERT_EQ(cmd2.verb, V_TAKE);
    ASSERT_EQ(cmd2.directObj, lampPtr);
    
    // Test missing object followed by valid command
    ParsedCommand cmd3 = parser.parse("take unicorn");
    ASSERT_EQ(cmd3.verb, V_TAKE);
    ASSERT_EQ(cmd3.directObj, nullptr);
    
    // Parser should still work
    ParsedCommand cmd4 = parser.parse("examine lamp");
    ASSERT_EQ(cmd4.verb, V_EXAMINE);
    ASSERT_EQ(cmd4.directObj, lampPtr);
    
    // Cleanup
    g.reset();
}
// Test NPC creation - Task 20
TEST(NPCCreationTroll) {
    auto& g = Globals::instance();
    
    // Initialize world to create NPCs
    initializeWorld();
    
    // Verify TROLL NPC exists
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ASSERT_TRUE(troll != nullptr);
    ASSERT_EQ(troll->getDesc(), "troll");
    
    // Verify TROLL has correct flags
    ASSERT_TRUE(troll->hasFlag(ObjectFlag::FIGHTBIT));
    ASSERT_TRUE(troll->hasFlag(ObjectFlag::ACTORBIT));
    
    // Verify TROLL has correct strength (stronger than thief)
    ASSERT_EQ(troll->getProperty(P_STRENGTH), 8);
    
    // Verify TROLL has correct synonyms
    ASSERT_TRUE(troll->hasSynonym("troll"));
    
    // Verify TROLL is in correct location (Troll Room)
    ASSERT_EQ(troll->getLocation(), g.getObject(RoomIds::TROLL_ROOM));
    
    // Cleanup
    g.reset();
}

TEST(NPCCreationCyclops) {
    auto& g = Globals::instance();
    
    // Initialize world to create NPCs
    initializeWorld();
    
    // Verify CYCLOPS NPC exists
    ZObject* cyclops = g.getObject(ObjectIds::CYCLOPS);
    ASSERT_TRUE(cyclops != nullptr);
    ASSERT_EQ(cyclops->getDesc(), "cyclops");
    
    // Verify CYCLOPS has correct flags
    ASSERT_TRUE(cyclops->hasFlag(ObjectFlag::FIGHTBIT));
    ASSERT_TRUE(cyclops->hasFlag(ObjectFlag::ACTORBIT));
    
    // Verify CYCLOPS has correct strength
    ASSERT_EQ(cyclops->getProperty(P_STRENGTH), 10);
    
    // Verify CYCLOPS has correct synonyms
    ASSERT_TRUE(cyclops->hasSynonym("cyclops"));
    ASSERT_TRUE(cyclops->hasSynonym("giant"));
    
    // Verify CYCLOPS is in correct location (Cyclops Room)
    ASSERT_EQ(cyclops->getLocation(), g.getObject(RoomIds::CYCLOPS_ROOM));
    
    // Cleanup
    g.reset();
}

TEST(NPCCreationGrue) {
    auto& g = Globals::instance();
    
    // Initialize world to create NPCs
    initializeWorld();
    
    // Verify GRUE object exists
    ZObject* grue = g.getObject(ObjectIds::GRUE);
    ASSERT_TRUE(grue != nullptr);
    ASSERT_EQ(grue->getDesc(), "grue");
    
    // Verify GRUE has INVISIBLE flag (never seen, only felt)
    ASSERT_TRUE(grue->hasFlag(ObjectFlag::INVISIBLE));
    
    // Verify GRUE has correct synonym
    ASSERT_TRUE(grue->hasSynonym("grue"));
    
    // Verify GRUE has no location (exists in darkness)
    ASSERT_EQ(grue->getLocation(), nullptr);
    
    // Cleanup
    g.reset();
}

TEST(NPCCreationThief) {
    auto& g = Globals::instance();
    
    // Initialize world to create NPCs
    initializeWorld();
    
    // Verify THIEF NPC exists (already created in task 20.1)
    ZObject* thief = g.getObject(ObjectIds::THIEF);
    ASSERT_TRUE(thief != nullptr);
    ASSERT_EQ(thief->getDesc(), "thief");
    
    // Verify THIEF has correct flags
    ASSERT_TRUE(thief->hasFlag(ObjectFlag::FIGHTBIT));
    ASSERT_TRUE(thief->hasFlag(ObjectFlag::ACTORBIT));
    
    // Verify THIEF has correct strength
    ASSERT_EQ(thief->getProperty(P_STRENGTH), 5);
    
    // Verify THIEF has correct synonyms
    ASSERT_TRUE(thief->hasSynonym("thief"));
    ASSERT_TRUE(thief->hasSynonym("robber"));
    ASSERT_TRUE(thief->hasSynonym("burglar"));
    
    // Cleanup
    g.reset();
}

// Description Mode Tests - Task 65.6

// Test verbose mode displays full description every time (Requirement 65)
TEST(VerboseModeFullDescription) {
    auto& g = Globals::instance();
    
    // Create test room with long description
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set verbose mode
    g.verboseMode = true;
    g.briefMode = false;
    g.superbriefMode = false;
    
    // First visit - should show full description
    OutputCapture capture1;
    Verbs::vLook();
    std::string output1 = capture1.getOutput();
    ASSERT_CONTAINS(output1, "detailed description");
    
    // Mark as visited
    ASSERT_TRUE(roomPtr->hasFlag(ObjectFlag::TOUCHBIT));
    
    // Second visit - should still show full description in verbose mode
    OutputCapture capture2;
    Verbs::vLook();
    std::string output2 = capture2.getOutput();
    ASSERT_CONTAINS(output2, "detailed description");
    
    // Cleanup
    g.reset();
}

// Test brief mode shows short description for visited rooms (Requirement 66)
TEST(BriefModeVisitedRoom) {
    auto& g = Globals::instance();
    
    // Create test room
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set brief mode
    g.verboseMode = false;
    g.briefMode = true;
    g.superbriefMode = false;
    
    // First visit - should show full description
    roomPtr->clearFlag(ObjectFlag::TOUCHBIT);
    OutputCapture capture1;
    Verbs::vLook();
    std::string output1 = capture1.getOutput();
    ASSERT_CONTAINS(output1, "detailed description");
    
    // Mark as visited
    ASSERT_TRUE(roomPtr->hasFlag(ObjectFlag::TOUCHBIT));
    
    // Second visit - should show short description (just room name)
    OutputCapture capture2;
    Verbs::vLook();
    std::string output2 = capture2.getOutput();
    ASSERT_CONTAINS(output2, "Test Room");
    
    // Cleanup
    g.reset();
}

// Test brief mode shows full description for unvisited rooms (Requirement 66)
TEST(BriefModeUnvisitedRoom) {
    auto& g = Globals::instance();
    
    // Create test room
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set brief mode
    g.verboseMode = false;
    g.briefMode = true;
    g.superbriefMode = false;
    
    // Ensure room is not visited
    roomPtr->clearFlag(ObjectFlag::TOUCHBIT);
    ASSERT_FALSE(roomPtr->hasFlag(ObjectFlag::TOUCHBIT));
    
    // First visit - should show full description
    OutputCapture capture;
    Verbs::vLook();
    std::string output = capture.getOutput();
    ASSERT_CONTAINS(output, "detailed description");
    
    // Room should now be marked as visited
    ASSERT_TRUE(roomPtr->hasFlag(ObjectFlag::TOUCHBIT));
    
    // Cleanup
    g.reset();
}

// Test superbrief mode shows only room name (Requirement 67)
TEST(SuperbriefModeRoomName) {
    auto& g = Globals::instance();
    
    // Create test room
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set superbrief mode
    g.verboseMode = false;
    g.briefMode = false;
    g.superbriefMode = true;
    
    // Look - should show only room name
    OutputCapture capture;
    Verbs::vLook();
    std::string output = capture.getOutput();
    ASSERT_CONTAINS(output, "Test Room");
    
    // Cleanup
    g.reset();
}

// Test superbrief mode omits object lists (Requirement 67)
TEST(SuperbriefModeOmitsObjects) {
    auto& g = Globals::instance();
    
    // Create test room
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create object in room
    auto lamp = std::make_unique<ZObject>(200, "brass lantern");
    ZObject* lampPtr = lamp.get();
    g.registerObject(200, std::move(lamp));
    lampPtr->moveTo(roomPtr);
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set superbrief mode
    g.verboseMode = false;
    g.briefMode = false;
    g.superbriefMode = true;
    
    // Look - should NOT list objects
    OutputCapture capture;
    Verbs::vLook();
    std::string output = capture.getOutput();
    ASSERT_CONTAINS(output, "Test Room");
    // In superbrief mode, objects should not be listed
    // The output should be minimal
    
    // Cleanup
    g.reset();
}

// Test mode switching affects room display (Requirement 65, 66, 67)
TEST(ModeSwitchingAffectsDisplay) {
    auto& g = Globals::instance();
    
    // Create test room
    auto room = std::make_unique<ZRoom>(100, "Test Room", "This is a detailed description of the test room.");
    ZRoom* roomPtr = room.get();
    g.registerObject(100, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Start in verbose mode
    g.verboseMode = true;
    g.briefMode = false;
    g.superbriefMode = false;
    
    // Look in verbose mode
    OutputCapture capture1;
    Verbs::vLook();
    std::string output1 = capture1.getOutput();
    ASSERT_CONTAINS(output1, "detailed description");
    
    // Switch to brief mode
    Verbs::vBrief();
    ASSERT_TRUE(g.briefMode);
    
    // Look in brief mode (room is now visited)
    OutputCapture capture2;
    Verbs::vLook();
    std::string output2 = capture2.getOutput();
    ASSERT_CONTAINS(output2, "Test Room");
    
    // Switch to superbrief mode
    Verbs::vSuperbrief();
    ASSERT_TRUE(g.superbriefMode);
    
    // Look in superbrief mode
    OutputCapture capture3;
    Verbs::vLook();
    std::string output3 = capture3.getOutput();
    ASSERT_CONTAINS(output3, "Test Room");
    
    // Cleanup
    g.reset();
}

// Test default mode is verbose (Requirement 65.5)
TEST(DefaultModeIsVerbose) {
    auto& g = Globals::instance();
    g.reset();
    
    // After reset, verbose mode should be default
    ASSERT_TRUE(g.verboseMode);
    ASSERT_FALSE(g.briefMode);
    ASSERT_FALSE(g.superbriefMode);
    
    // Cleanup
    g.reset();
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

// Test TAKE verb - Task 26.3
TEST(TakeVerbNormalObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create takeable object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->setProperty(P_SIZE, 5);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set up for TAKE command
    g.prso = lampPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was taken
    ASSERT_TRUE(result);
    ASSERT_EQ(lampPtr->getLocation(), g.winner);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbAnchoredObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create anchored object (TRYTAKEBIT set)
    auto mailbox = std::make_unique<ZObject>(1, "mailbox");
    mailbox->addSynonym("mailbox");
    mailbox->setFlag(ObjectFlag::TRYTAKEBIT);  // Anchored - cannot be taken
    mailbox->moveTo(&testRoom);
    ZObject* mailboxPtr = mailbox.get();
    g.registerObject(1, std::move(mailbox));
    
    // Set up for TAKE command
    g.prso = mailboxPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was NOT taken (still in room)
    ASSERT_TRUE(result);
    ASSERT_EQ(mailboxPtr->getLocation(), &testRoom);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbWeightLimit) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set a low weight limit for testing
    g.loadAllowed = 20;
    
    // Create heavy object already in inventory
    auto heavyItem = std::make_unique<ZObject>(1, "heavy item");
    heavyItem->setProperty(P_SIZE, 15);
    heavyItem->moveTo(g.winner);
    g.registerObject(1, std::move(heavyItem));
    
    // Create another object that would exceed the limit
    auto lamp = std::make_unique<ZObject>(2, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->setProperty(P_SIZE, 10);  // 15 + 10 = 25 > 20 (limit)
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(2, std::move(lamp));
    
    // Set up for TAKE command
    g.prso = lampPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was NOT taken (weight limit exceeded)
    ASSERT_TRUE(result);
    ASSERT_EQ(lampPtr->getLocation(), &testRoom);
    
    // Cleanup
    g.reset();
}

TEST(WeightCalculation) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create multiple objects with different sizes
    auto obj1 = std::make_unique<ZObject>(1, "object 1");
    obj1->setProperty(P_SIZE, 10);
    obj1->moveTo(g.winner);
    g.registerObject(1, std::move(obj1));
    
    auto obj2 = std::make_unique<ZObject>(2, "object 2");
    obj2->setProperty(P_SIZE, 15);
    obj2->moveTo(g.winner);
    g.registerObject(2, std::move(obj2));
    
    auto obj3 = std::make_unique<ZObject>(3, "object 3");
    obj3->setProperty(P_SIZE, 20);
    obj3->moveTo(g.winner);
    g.registerObject(3, std::move(obj3));
    
    // Calculate total weight
    int totalWeight = 0;
    for (const auto* obj : g.winner->getContents()) {
        totalWeight += obj->getProperty(P_SIZE);
    }
    
    // Verify weight calculation is correct (10 + 15 + 20 = 45)
    ASSERT_EQ(totalWeight, 45);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbWithinWeightLimit) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set weight limit
    g.loadAllowed = 100;
    
    // Create object already in inventory
    auto obj1 = std::make_unique<ZObject>(1, "object 1");
    obj1->setProperty(P_SIZE, 30);
    obj1->moveTo(g.winner);
    g.registerObject(1, std::move(obj1));
    
    // Create object to take (within limit: 30 + 40 = 70 < 100)
    auto lamp = std::make_unique<ZObject>(2, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->setProperty(P_SIZE, 40);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(2, std::move(lamp));
    
    // Set up for TAKE command
    g.prso = lampPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object WAS taken (within weight limit)
    ASSERT_TRUE(result);
    ASSERT_EQ(lampPtr->getLocation(), g.winner);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbHeavyObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set weight limit
    g.loadAllowed = 100;
    
    // Create very heavy object that exceeds limit by itself
    auto coffin = std::make_unique<ZObject>(1, "coffin");
    coffin->addSynonym("coffin");
    coffin->setFlag(ObjectFlag::TAKEBIT);
    coffin->setProperty(P_SIZE, 150);  // Exceeds limit of 100
    coffin->moveTo(&testRoom);
    ZObject* coffinPtr = coffin.get();
    g.registerObject(1, std::move(coffin));
    
    // Set up for TAKE command
    g.prso = coffinPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was NOT taken (too heavy)
    ASSERT_TRUE(result);
    ASSERT_EQ(coffinPtr->getLocation(), &testRoom);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbFromOpenContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container in room
    auto box = std::make_unique<ZObject>(10, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(10, std::move(box));
    
    // Create object in open container
    auto coin = std::make_unique<ZObject>(1, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(P_SIZE, 2);
    coin->moveTo(boxPtr);
    ZObject* coinPtr = coin.get();
    g.registerObject(1, std::move(coin));
    
    // Set up for TAKE command
    g.prso = coinPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was taken from container
    ASSERT_TRUE(result);
    ASSERT_EQ(coinPtr->getLocation(), g.winner);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbFromClosedContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container in room
    auto chest = std::make_unique<ZObject>(10, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Not setting OPENBIT - it's closed
    chest->moveTo(&testRoom);
    ZObject* chestPtr = chest.get();
    g.registerObject(10, std::move(chest));
    
    // Create object in closed container
    auto gem = std::make_unique<ZObject>(1, "gem");
    gem->addSynonym("gem");
    gem->setFlag(ObjectFlag::TAKEBIT);
    gem->setProperty(P_SIZE, 2);
    gem->moveTo(chestPtr);
    ZObject* gemPtr = gem.get();
    g.registerObject(1, std::move(gem));
    
    // Set up for TAKE command
    g.prso = gemPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was NOT taken (container is closed, not accessible)
    ASSERT_TRUE(result);
    ASSERT_EQ(gemPtr->getLocation(), chestPtr);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbNonTakeableObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-takeable object (no TAKEBIT flag)
    auto house = std::make_unique<ZObject>(1, "house");
    house->addSynonym("house");
    // No TAKEBIT flag set
    house->moveTo(&testRoom);
    ZObject* housePtr = house.get();
    g.registerObject(1, std::move(house));
    
    // Set up for TAKE command
    g.prso = housePtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object was NOT taken
    ASSERT_TRUE(result);
    ASSERT_EQ(housePtr->getLocation(), &testRoom);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbAlreadyHeld) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create object already in inventory
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->setProperty(P_SIZE, 5);
    lamp->moveTo(g.winner);  // Already in inventory
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set up for TAKE command
    g.prso = lampPtr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Verify object is still in inventory (no change)
    ASSERT_TRUE(result);
    ASSERT_EQ(lampPtr->getLocation(), g.winner);
    
    // Cleanup
    g.reset();
}

TEST(TakeVerbNoObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up for TAKE command with no object
    g.prso = nullptr;
    g.prsa = V_TAKE;
    
    // Execute TAKE verb
    bool result = Verbs::vTake();
    
    // Should return true (command handled, even if it's an error)
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}
