#include "test_framework.h"
#include "../src/parser/parser.h"
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
