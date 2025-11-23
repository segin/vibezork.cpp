#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/verbs/verbs.h"

// Test INFLATE verb (Requirement 31)
TEST(InflateWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to inflate without specifying an object
    g.prsa = V_INFLATE;
    g.prso = nullptr;
    
    bool result = Verbs::vInflate();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(InflateNonInflatableObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to inflate a non-inflatable object
    g.prsa = V_INFLATE;
    g.prso = lampPtr;
    
    bool result = Verbs::vInflate();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test DEFLATE verb (Requirement 31)
TEST(DeflateWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to deflate without specifying an object
    g.prsa = V_DEFLATE;
    g.prso = nullptr;
    
    bool result = Verbs::vDeflate();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(DeflateNonDeflatableObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to deflate a non-deflatable object
    g.prsa = V_DEFLATE;
    g.prso = lampPtr;
    
    bool result = Verbs::vDeflate();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test PRAY verb (Requirement 31)
TEST(PrayDefaultBehavior) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to pray in a normal room
    g.prsa = V_PRAY;
    
    bool result = Verbs::vPray();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test EXORCISE verb (Requirement 31)
TEST(ExorciseWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to exorcise without specifying an object
    g.prsa = V_EXORCISE;
    g.prso = nullptr;
    
    bool result = Verbs::vExorcise();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExorciseDefaultBehavior) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to exorcise a normal object
    g.prsa = V_EXORCISE;
    g.prso = lampPtr;
    
    bool result = Verbs::vExorcise();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test WAVE verb (Requirement 31)
TEST(WaveWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to wave without specifying an object
    g.prsa = V_WAVE;
    g.prso = nullptr;
    
    bool result = Verbs::vWave();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(WaveDefaultBehavior) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to wave an object
    g.prsa = V_WAVE;
    g.prso = lampPtr;
    
    bool result = Verbs::vWave();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test RUB verb (Requirement 31)
TEST(RubWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to rub without specifying an object
    g.prsa = V_RUB;
    g.prso = nullptr;
    
    bool result = Verbs::vRub();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(RubDefaultBehavior) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to rub an object
    g.prsa = V_RUB;
    g.prso = lampPtr;
    
    bool result = Verbs::vRub();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test RING verb (Requirement 31)
TEST(RingWithoutObject) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Try to ring without specifying an object
    g.prsa = V_RING;
    g.prso = nullptr;
    
    bool result = Verbs::vRing();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(RingDefaultBehavior) {
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Try to ring a non-ringable object
    g.prsa = V_RING;
    g.prso = lampPtr;
    
    bool result = Verbs::vRing();
    
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

int main() {
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
