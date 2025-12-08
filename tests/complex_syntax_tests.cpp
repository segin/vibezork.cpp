#include "test_framework.h"
#include "../src/parser/parser.h"
#include "../src/parser/verb_registry.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/objects.h"

TEST(ComplexSyntaxPutIn) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create container and object
    auto container = std::make_unique<ZObject>(100, "box");
    container->addSynonym("box");
    container->setFlag(ObjectFlag::CONTBIT); 
    container->setFlag(ObjectFlag::OPENBIT); // Open so we can put things in
    
    auto obj = std::make_unique<ZObject>(101, "ball");
    obj->addSynonym("ball");
    obj->setFlag(ObjectFlag::TAKEBIT);
    
    // Put object in player's inventory
    obj->moveTo(g.winner);
    // Put container in room
    container->moveTo(g.here);
    
    ZObject* containerPtr = container.get();
    ZObject* objPtr = obj.get();
    
    g.registerObject(100, std::move(container));
    g.registerObject(101, std::move(obj));
    
    Parser parser;
    
    // Test "put ball in box"
    auto cmd = parser.parse("put ball in box");
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, objPtr);
    ASSERT_EQ(cmd.indirectObj, containerPtr);
    
    // Test "put ball inside box"
    cmd = parser.parse("put ball inside box");
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, objPtr);
    ASSERT_EQ(cmd.indirectObj, containerPtr);
    
    // Test "put ball into box"
    cmd = parser.parse("put ball into box");
    ASSERT_EQ(cmd.verb, V_PUT);
    ASSERT_EQ(cmd.directObj, objPtr);
    ASSERT_EQ(cmd.indirectObj, containerPtr);
}

TEST(ComplexSyntaxPutOn) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create surface (table) and object (lamp)
    auto table = std::make_unique<ZObject>(200, "table");
    table->addSynonym("table");
    table->setFlag(ObjectFlag::SURFACEBIT);
    // Note: In Zorky behavior, surface doesn't explicitly need OPENBIT usually, 
    // but parser might check if it's a container-like thing.
    // Zork I tables accept things on them.
    
    auto lamp = std::make_unique<ZObject>(201, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    
    // Lamp in inventory
    lamp->moveTo(g.winner);
    // Table in room
    table->moveTo(g.here);
    
    ZObject* tablePtr = table.get();
    ZObject* lampPtr = lamp.get();
    
    g.registerObject(200, std::move(table));
    g.registerObject(201, std::move(lamp));
    
    VerbRegistry registry;
    Parser parser(&registry);
    
    // Test "put lamp on table"
    auto cmd = parser.parse("put lamp on table");
    ASSERT_EQ(cmd.verb, V_PUT_ON);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, tablePtr);
    
    // Test "put lamp onto table"
    cmd = parser.parse("put lamp onto table");
    ASSERT_EQ(cmd.verb, V_PUT_ON);
    ASSERT_EQ(cmd.directObj, lampPtr);
    ASSERT_EQ(cmd.indirectObj, tablePtr);
}

int main() {
    std::cout << "Running Complex Syntax Tests\n";
    std::cout << "============================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
