#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"

// Property ID constants for testing
constexpr PropertyId P_SIZE = 1;
constexpr PropertyId P_CAPACITY = 2;
constexpr PropertyId P_VALUE = 3;
constexpr PropertyId P_STRENGTH = 4;
constexpr PropertyId P_TVALUE = 5;

// Comprehensive Object System Tests - Task 66.1

// Test all flag operations
TEST(ObjectFlagsSetAndClear) {
    ZObject obj(1, "test");
    
    // Test setting multiple flags
    obj.setFlag(ObjectFlag::TAKEBIT);
    obj.setFlag(ObjectFlag::CONTBIT);
    obj.setFlag(ObjectFlag::OPENBIT);
    
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::CONTBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::OPENBIT));
    
    // Test clearing individual flags
    obj.clearFlag(ObjectFlag::TAKEBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::CONTBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::OPENBIT));
    
    obj.clearFlag(ObjectFlag::CONTBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::CONTBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::OPENBIT));
    
    obj.clearFlag(ObjectFlag::OPENBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::OPENBIT));
}

TEST(ObjectFlagsMultipleOperations) {
    ZObject obj(1, "test");
    
    // Test setting same flag multiple times
    obj.setFlag(ObjectFlag::TAKEBIT);
    obj.setFlag(ObjectFlag::TAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    
    // Test clearing non-existent flag
    obj.clearFlag(ObjectFlag::CONTBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::CONTBIT));
    
    // Test clearing already cleared flag
    obj.clearFlag(ObjectFlag::TAKEBIT);
    obj.clearFlag(ObjectFlag::TAKEBIT);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
}

TEST(ObjectFlagsAllTypes) {
    ZObject obj(1, "test");
    
    // Test all flag types
    obj.setFlag(ObjectFlag::INVISIBLE);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::INVISIBLE));
    
    obj.setFlag(ObjectFlag::TOUCHBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TOUCHBIT));
    
    obj.setFlag(ObjectFlag::TRYTAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TRYTAKEBIT));
    
    obj.setFlag(ObjectFlag::TRANSBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TRANSBIT));
    
    obj.setFlag(ObjectFlag::ONBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::ONBIT));
    
    obj.setFlag(ObjectFlag::LIGHTBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::LIGHTBIT));
    
    obj.setFlag(ObjectFlag::LOCKEDBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::LOCKEDBIT));
    
    obj.setFlag(ObjectFlag::DEADBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::DEADBIT));
    
    obj.setFlag(ObjectFlag::FIGHTBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::FIGHTBIT));
    
    obj.setFlag(ObjectFlag::FOODBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::FOODBIT));
    
    obj.setFlag(ObjectFlag::DRINKBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::DRINKBIT));
    
    obj.setFlag(ObjectFlag::BURNBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::BURNBIT));
    
    obj.setFlag(ObjectFlag::FLAMEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::FLAMEBIT));
}

TEST(ObjectFlagsSerialization) {
    ZObject obj(1, "test");
    
    // Set some flags
    obj.setFlag(ObjectFlag::TAKEBIT);
    obj.setFlag(ObjectFlag::CONTBIT);
    obj.setFlag(ObjectFlag::OPENBIT);
    
    // Get all flags
    uint32_t flags = obj.getAllFlags();
    
    // Create new object and set flags
    ZObject obj2(2, "test2");
    obj2.setAllFlags(flags);
    
    // Verify flags were copied
    ASSERT_TRUE(obj2.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_TRUE(obj2.hasFlag(ObjectFlag::CONTBIT));
    ASSERT_TRUE(obj2.hasFlag(ObjectFlag::OPENBIT));
    ASSERT_FALSE(obj2.hasFlag(ObjectFlag::LIGHTBIT));
}

// Test all property operations
TEST(ObjectPropertiesBasic) {
    ZObject obj(1, "test");
    
    // Test setting and getting properties
    obj.setProperty(P_SIZE, 10);
    ASSERT_EQ(obj.getProperty(P_SIZE), 10);
    
    obj.setProperty(P_CAPACITY, 100);
    ASSERT_EQ(obj.getProperty(P_CAPACITY), 100);
    
    obj.setProperty(P_VALUE, 50);
    ASSERT_EQ(obj.getProperty(P_VALUE), 50);
}

TEST(ObjectPropertiesDefaultValue) {
    ZObject obj(1, "test");
    
    // Test getting unset property returns 0
    ASSERT_EQ(obj.getProperty(P_SIZE), 0);
    ASSERT_EQ(obj.getProperty(P_CAPACITY), 0);
    ASSERT_EQ(obj.getProperty(P_VALUE), 0);
}

TEST(ObjectPropertiesOverwrite) {
    ZObject obj(1, "test");
    
    // Test overwriting property value
    obj.setProperty(P_SIZE, 10);
    ASSERT_EQ(obj.getProperty(P_SIZE), 10);
    
    obj.setProperty(P_SIZE, 20);
    ASSERT_EQ(obj.getProperty(P_SIZE), 20);
    
    obj.setProperty(P_SIZE, 0);
    ASSERT_EQ(obj.getProperty(P_SIZE), 0);
}

TEST(ObjectPropertiesNegativeValues) {
    ZObject obj(1, "test");
    
    // Test negative property values
    obj.setProperty(P_SIZE, -5);
    ASSERT_EQ(obj.getProperty(P_SIZE), -5);
    
    obj.setProperty(P_VALUE, -100);
    ASSERT_EQ(obj.getProperty(P_VALUE), -100);
}

TEST(ObjectPropertiesMultiple) {
    ZObject obj(1, "test");
    
    // Test setting multiple properties
    obj.setProperty(P_SIZE, 10);
    obj.setProperty(P_CAPACITY, 100);
    obj.setProperty(P_VALUE, 50);
    obj.setProperty(P_STRENGTH, 25);
    
    // Verify all properties are independent
    ASSERT_EQ(obj.getProperty(P_SIZE), 10);
    ASSERT_EQ(obj.getProperty(P_CAPACITY), 100);
    ASSERT_EQ(obj.getProperty(P_VALUE), 50);
    ASSERT_EQ(obj.getProperty(P_STRENGTH), 25);
}

TEST(ObjectPropertiesSerialization) {
    ZObject obj(1, "test");
    
    // Set some properties
    obj.setProperty(P_SIZE, 10);
    obj.setProperty(P_CAPACITY, 100);
    obj.setProperty(P_VALUE, 50);
    
    // Get all properties
    const auto& props = obj.getAllProperties();
    
    // Verify properties are in the map
    ASSERT_EQ(props.size(), 3);
    ASSERT_EQ(props.at(P_SIZE), 10);
    ASSERT_EQ(props.at(P_CAPACITY), 100);
    ASSERT_EQ(props.at(P_VALUE), 50);
}

// Test text property operations
TEST(ObjectTextProperty) {
    ZObject obj(1, "test");
    
    // Test setting and getting text
    ASSERT_FALSE(obj.hasText());
    
    obj.setText("This is readable text.");
    ASSERT_TRUE(obj.hasText());
    ASSERT_EQ(obj.getText(), "This is readable text.");
}

TEST(ObjectTextPropertyEmpty) {
    ZObject obj(1, "test");
    
    // Test empty text - hasText returns false for empty string
    // (this is a reasonable design: no readable content = no text)
    obj.setText("");
    ASSERT_FALSE(obj.hasText());
    ASSERT_EQ(obj.getText(), "");
}

TEST(ObjectTextPropertyOverwrite) {
    ZObject obj(1, "test");
    
    // Test overwriting text
    obj.setText("First text");
    ASSERT_EQ(obj.getText(), "First text");
    
    obj.setText("Second text");
    ASSERT_EQ(obj.getText(), "Second text");
}

TEST(ObjectTextPropertyLong) {
    ZObject obj(1, "test");
    
    // Test long text
    std::string longText(1000, 'a');
    obj.setText(longText);
    ASSERT_TRUE(obj.hasText());
    ASSERT_EQ(obj.getText(), longText);
}

// Test containment operations
TEST(ObjectContainmentBasic) {
    ZObject container(1, "container");
    ZObject item(2, "item");
    
    // Test moving item to container
    item.moveTo(&container);
    
    ASSERT_EQ(item.getLocation(), &container);
    ASSERT_EQ(container.getContents().size(), 1);
    ASSERT_EQ(container.getContents()[0], &item);
}

TEST(ObjectContainmentMultipleItems) {
    ZObject container(1, "container");
    ZObject item1(2, "item1");
    ZObject item2(3, "item2");
    ZObject item3(4, "item3");
    
    // Test moving multiple items to container
    item1.moveTo(&container);
    item2.moveTo(&container);
    item3.moveTo(&container);
    
    ASSERT_EQ(container.getContents().size(), 3);
    ASSERT_EQ(item1.getLocation(), &container);
    ASSERT_EQ(item2.getLocation(), &container);
    ASSERT_EQ(item3.getLocation(), &container);
}

TEST(ObjectContainmentMoveBetweenContainers) {
    ZObject container1(1, "container1");
    ZObject container2(2, "container2");
    ZObject item(3, "item");
    
    // Move item to first container
    item.moveTo(&container1);
    ASSERT_EQ(item.getLocation(), &container1);
    ASSERT_EQ(container1.getContents().size(), 1);
    ASSERT_EQ(container2.getContents().size(), 0);
    
    // Move item to second container
    item.moveTo(&container2);
    ASSERT_EQ(item.getLocation(), &container2);
    ASSERT_EQ(container1.getContents().size(), 0);
    ASSERT_EQ(container2.getContents().size(), 1);
}

TEST(ObjectContainmentNested) {
    ZObject room(1, "room");
    ZObject box(2, "box");
    ZObject item(3, "item");
    
    // Test nested containment
    box.moveTo(&room);
    item.moveTo(&box);
    
    ASSERT_EQ(box.getLocation(), &room);
    ASSERT_EQ(item.getLocation(), &box);
    ASSERT_EQ(room.getContents().size(), 1);
    ASSERT_EQ(box.getContents().size(), 1);
}

TEST(ObjectContainmentRemoveFromContainer) {
    ZObject container(1, "container");
    ZObject item(2, "item");
    
    // Add item to container
    item.moveTo(&container);
    ASSERT_EQ(container.getContents().size(), 1);
    
    // Move item to null (remove from container)
    item.moveTo(nullptr);
    ASSERT_EQ(item.getLocation(), nullptr);
    ASSERT_EQ(container.getContents().size(), 0);
}

TEST(ObjectContainmentMoveToSelf) {
    ZObject obj(1, "obj");
    
    // Test moving object to itself (should handle gracefully)
    obj.moveTo(&obj);
    
    // Object should not be in its own contents
    ASSERT_EQ(obj.getContents().size(), 0);
}

TEST(ObjectContainmentOrder) {
    ZObject container(1, "container");
    ZObject item1(2, "item1");
    ZObject item2(3, "item2");
    ZObject item3(4, "item3");
    
    // Add items in specific order
    item1.moveTo(&container);
    item2.moveTo(&container);
    item3.moveTo(&container);
    
    // Verify order is preserved
    const auto& contents = container.getContents();
    ASSERT_EQ(contents[0], &item1);
    ASSERT_EQ(contents[1], &item2);
    ASSERT_EQ(contents[2], &item3);
}

// Test object creation and destruction
TEST(ObjectCreationBasic) {
    ZObject obj(42, "test object");
    
    ASSERT_EQ(obj.getId(), 42);
    ASSERT_EQ(obj.getDesc(), "test object");
}

TEST(ObjectCreationEmptyDescription) {
    ZObject obj(1, "");
    
    ASSERT_EQ(obj.getId(), 1);
    ASSERT_EQ(obj.getDesc(), "");
}

TEST(ObjectCreationLongDescription) {
    std::string longDesc(1000, 'x');
    ZObject obj(1, longDesc);
    
    ASSERT_EQ(obj.getDesc(), longDesc);
}

TEST(ObjectCreationInitialState) {
    ZObject obj(1, "test");
    
    // Verify initial state
    ASSERT_EQ(obj.getLocation(), nullptr);
    ASSERT_EQ(obj.getContents().size(), 0);
    ASSERT_FALSE(obj.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_EQ(obj.getProperty(P_SIZE), 0);
    ASSERT_FALSE(obj.hasText());
    ASSERT_EQ(obj.getSynonyms().size(), 0);
    ASSERT_EQ(obj.getAdjectives().size(), 0);
}

TEST(ObjectDestructionWithContents) {
    auto container = std::make_unique<ZObject>(1, "container");
    ZObject item(2, "item");
    
    // Add item to container
    item.moveTo(container.get());
    ASSERT_EQ(item.getLocation(), container.get());
    
    // Destroy container
    container.reset();
    
    // Item's location pointer is now dangling, but shouldn't crash
    // In real usage, items should be moved out before container destruction
}

// Test synonym operations
TEST(ObjectSynonymsBasic) {
    ZObject obj(1, "lamp");
    
    obj.addSynonym("lamp");
    obj.addSynonym("lantern");
    obj.addSynonym("light");
    
    ASSERT_TRUE(obj.hasSynonym("lamp"));
    ASSERT_TRUE(obj.hasSynonym("lantern"));
    ASSERT_TRUE(obj.hasSynonym("light"));
    ASSERT_FALSE(obj.hasSynonym("box"));
}

TEST(ObjectSynonymsCaseInsensitive) {
    ZObject obj(1, "lamp");
    
    obj.addSynonym("lamp");
    
    // hasSynonym should be case-insensitive
    ASSERT_TRUE(obj.hasSynonym("lamp"));
    ASSERT_TRUE(obj.hasSynonym("LAMP"));
    ASSERT_TRUE(obj.hasSynonym("Lamp"));
}

TEST(ObjectSynonymsMultiple) {
    ZObject obj(1, "object");
    
    // Add many synonyms
    for (int i = 0; i < 10; i++) {
        obj.addSynonym("synonym" + std::to_string(i));
    }
    
    ASSERT_EQ(obj.getSynonyms().size(), 10);
    
    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(obj.hasSynonym("synonym" + std::to_string(i)));
    }
}

TEST(ObjectSynonymsDuplicate) {
    ZObject obj(1, "lamp");
    
    // Add same synonym multiple times
    obj.addSynonym("lamp");
    obj.addSynonym("lamp");
    obj.addSynonym("lamp");
    
    // All three should be in the list (no deduplication)
    ASSERT_EQ(obj.getSynonyms().size(), 3);
    ASSERT_TRUE(obj.hasSynonym("lamp"));
}

// Test adjective operations
TEST(ObjectAdjectivesBasic) {
    ZObject obj(1, "lamp");
    
    obj.addAdjective("brass");
    obj.addAdjective("small");
    
    ASSERT_TRUE(obj.hasAdjective("brass"));
    ASSERT_TRUE(obj.hasAdjective("small"));
    ASSERT_FALSE(obj.hasAdjective("large"));
}

TEST(ObjectAdjectivesCaseInsensitive) {
    ZObject obj(1, "lamp");
    
    obj.addAdjective("brass");
    
    // hasAdjective should be case-insensitive
    ASSERT_TRUE(obj.hasAdjective("brass"));
    ASSERT_TRUE(obj.hasAdjective("BRASS"));
    ASSERT_TRUE(obj.hasAdjective("Brass"));
}

TEST(ObjectAdjectivesMultiple) {
    ZObject obj(1, "object");
    
    // Add many adjectives
    obj.addAdjective("small");
    obj.addAdjective("brass");
    obj.addAdjective("shiny");
    obj.addAdjective("old");
    obj.addAdjective("rusty");
    
    ASSERT_EQ(obj.getAdjectives().size(), 5);
    ASSERT_TRUE(obj.hasAdjective("small"));
    ASSERT_TRUE(obj.hasAdjective("brass"));
    ASSERT_TRUE(obj.hasAdjective("shiny"));
    ASSERT_TRUE(obj.hasAdjective("old"));
    ASSERT_TRUE(obj.hasAdjective("rusty"));
}

// Test action handler
TEST(ObjectActionHandlerBasic) {
    ZObject obj(1, "test");
    
    bool actionCalled = false;
    obj.setAction([&actionCalled]() {
        actionCalled = true;
        return true;
    });
    
    ASSERT_TRUE(obj.performAction());
    ASSERT_TRUE(actionCalled);
}

TEST(ObjectActionHandlerNoAction) {
    ZObject obj(1, "test");
    
    // No action set, should return false
    ASSERT_FALSE(obj.performAction());
}

TEST(ObjectActionHandlerReturnValue) {
    ZObject obj(1, "test");
    
    // Test action that returns true
    obj.setAction([]() { return true; });
    ASSERT_TRUE(obj.performAction());
    
    // Test action that returns false
    obj.setAction([]() { return false; });
    ASSERT_FALSE(obj.performAction());
}

TEST(ObjectActionHandlerWithState) {
    ZObject obj(1, "test");
    
    int counter = 0;
    obj.setAction([&counter]() {
        counter++;
        return true;
    });
    
    obj.performAction();
    ASSERT_EQ(counter, 1);
    
    obj.performAction();
    ASSERT_EQ(counter, 2);
    
    obj.performAction();
    ASSERT_EQ(counter, 3);
}

TEST(ObjectActionHandlerReplacement) {
    ZObject obj(1, "test");
    
    int counter1 = 0;
    int counter2 = 0;
    
    // Set first action
    obj.setAction([&counter1]() {
        counter1++;
        return true;
    });
    
    obj.performAction();
    ASSERT_EQ(counter1, 1);
    ASSERT_EQ(counter2, 0);
    
    // Replace with second action
    obj.setAction([&counter2]() {
        counter2++;
        return true;
    });
    
    obj.performAction();
    ASSERT_EQ(counter1, 1);  // First action not called
    ASSERT_EQ(counter2, 1);  // Second action called
}

// Test combined operations
TEST(ObjectCombinedFlagsAndProperties) {
    ZObject obj(1, "lamp");
    
    // Set flags
    obj.setFlag(ObjectFlag::TAKEBIT);
    obj.setFlag(ObjectFlag::LIGHTBIT);
    
    // Set properties
    obj.setProperty(P_SIZE, 5);
    obj.setProperty(P_VALUE, 10);
    
    // Verify both work together
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::LIGHTBIT));
    ASSERT_EQ(obj.getProperty(P_SIZE), 5);
    ASSERT_EQ(obj.getProperty(P_VALUE), 10);
}

TEST(ObjectCombinedAllFeatures) {
    ZObject obj(1, "brass lamp");
    
    // Set everything
    obj.addSynonym("lamp");
    obj.addSynonym("lantern");
    obj.addAdjective("brass");
    obj.addAdjective("small");
    obj.setFlag(ObjectFlag::TAKEBIT);
    obj.setFlag(ObjectFlag::LIGHTBIT);
    obj.setProperty(P_SIZE, 5);
    obj.setProperty(P_VALUE, 10);
    obj.setText("A small brass lamp.");
    
    // Verify all features
    ASSERT_TRUE(obj.hasSynonym("lamp"));
    ASSERT_TRUE(obj.hasSynonym("lantern"));
    ASSERT_TRUE(obj.hasAdjective("brass"));
    ASSERT_TRUE(obj.hasAdjective("small"));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::LIGHTBIT));
    ASSERT_EQ(obj.getProperty(P_SIZE), 5);
    ASSERT_EQ(obj.getProperty(P_VALUE), 10);
    ASSERT_TRUE(obj.hasText());
    ASSERT_EQ(obj.getText(), "A small brass lamp.");
}

int main() {
    std::cout << "Running Object System Tests...\n\n";
    
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
