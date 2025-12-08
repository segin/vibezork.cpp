#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/verbs/verbs.h"
#include <iostream>
#include <sstream>
#include <vector>

// Helper to capture output
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

// Test empty inventory
TEST(InventoryEmpty) {
    auto& g = Globals::instance();
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Test
    OutputCapture capture;
    bool result = Verbs::vInventory();
    
    ASSERT_TRUE(result);
    // Should say "You are empty-handed." or similar
    ASSERT_CONTAINS(capture.getOutput(), "empty-handed");
    
    g.reset();
}

// Test single item
TEST(InventorySingleItem) {
    auto& g = Globals::instance();
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create item in inventory with correct description
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->moveTo(g.winner); // Put in inventory
    g.registerObject(1, std::move(lamp));
    
    // Test
    OutputCapture capture;
    bool result = Verbs::vInventory();
    
    ASSERT_TRUE(result);
    std::string out = capture.getOutput();
    ASSERT_CONTAINS(out, "You are carrying:");
    ASSERT_CONTAINS(out, "brass lantern");
    
    g.reset();
}

// Test multiple items
TEST(InventoryMultipleItems) {
    auto& g = Globals::instance();
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create items
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->moveTo(g.winner);
    g.registerObject(1, std::move(lamp));
    
    auto sword = std::make_unique<ZObject>(2, "elvish sword");
    sword->moveTo(g.winner);
    g.registerObject(2, std::move(sword));
    
    // Test
    OutputCapture capture;
    bool result = Verbs::vInventory();
    
    ASSERT_TRUE(result);
    std::string out = capture.getOutput();
    ASSERT_CONTAINS(out, "You are carrying:");
    ASSERT_CONTAINS(out, "brass lantern");
    ASSERT_CONTAINS(out, "elvish sword");
    
    g.reset();
}

int main() {
    std::cout << "Running Inventory Tests\n";
    std::cout << "======================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
