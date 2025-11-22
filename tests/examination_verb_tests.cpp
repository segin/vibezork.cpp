#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/verbs/verbs.h"

// Test EXAMINE verb - Task 25.5
TEST(ExamineVerbBasic) {
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
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set PRSO to lamp
    g.prso = lampPtr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExamineVerbNoObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set PRSO to nullptr (no object specified)
    g.prso = nullptr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb without object
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExamineVerbContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create item in container
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->moveTo(boxPtr);
    g.registerObject(2, std::move(coin));
    
    // Set PRSO to box
    g.prso = boxPtr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb on open container
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExamineVerbClosedContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Not setting OPENBIT - it's closed
    chest->moveTo(&testRoom);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set PRSO to chest
    g.prso = chestPtr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb on closed container
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExamineVerbLightSource) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp (light source, on)
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set PRSO to lamp
    g.prso = lampPtr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb on light source
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ExamineVerbLockedContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container
    auto safe = std::make_unique<ZObject>(1, "safe");
    safe->addSynonym("safe");
    safe->setFlag(ObjectFlag::CONTBIT);
    safe->setFlag(ObjectFlag::LOCKEDBIT);
    safe->moveTo(&testRoom);
    ZObject* safePtr = safe.get();
    g.registerObject(1, std::move(safe));
    
    // Set PRSO to safe
    g.prso = safePtr;
    g.prsa = V_EXAMINE;
    
    // Test EXAMINE verb on locked container
    bool result = Verbs::vExamine();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test LOOK-INSIDE verb - Task 25.5
TEST(LookInsideVerbBasic) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create item in container
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->moveTo(boxPtr);
    g.registerObject(2, std::move(coin));
    
    // Set PRSO to box
    g.prso = boxPtr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(LookInsideVerbNoObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set PRSO to nullptr
    g.prso = nullptr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb without object
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(LookInsideVerbNotContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-container object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set PRSO to lamp
    g.prso = lampPtr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb on non-container
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(LookInsideVerbClosedContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Not setting OPENBIT - it's closed
    chest->moveTo(&testRoom);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set PRSO to chest
    g.prso = chestPtr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb on closed container
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(LookInsideVerbTransparentContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create transparent container (closed but can see through)
    auto bottle = std::make_unique<ZObject>(1, "bottle");
    bottle->addSynonym("bottle");
    bottle->setFlag(ObjectFlag::CONTBIT);
    bottle->setFlag(ObjectFlag::TRANSBIT);
    // Not setting OPENBIT - it's closed but transparent
    bottle->moveTo(&testRoom);
    ZObject* bottlePtr = bottle.get();
    g.registerObject(1, std::move(bottle));
    
    // Create item in bottle
    auto water = std::make_unique<ZObject>(2, "water");
    water->addSynonym("water");
    water->moveTo(bottlePtr);
    g.registerObject(2, std::move(water));
    
    // Set PRSO to bottle
    g.prso = bottlePtr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb on transparent container
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(LookInsideVerbEmptyContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create empty open container
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Set PRSO to box
    g.prso = boxPtr;
    g.prsa = V_LOOK_INSIDE;
    
    // Test LOOK-INSIDE verb on empty container
    bool result = Verbs::vLookInside();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test SEARCH verb - Task 25.5
TEST(SearchVerbBasic) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create container
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->moveTo(&testRoom);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create item in container
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->moveTo(boxPtr);
    g.registerObject(2, std::move(coin));
    
    // Set PRSO to box
    g.prso = boxPtr;
    g.prsa = V_SEARCH;
    
    // Test SEARCH verb
    bool result = Verbs::vSearch();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(SearchVerbNoObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set PRSO to nullptr
    g.prso = nullptr;
    g.prsa = V_SEARCH;
    
    // Test SEARCH verb without object
    bool result = Verbs::vSearch();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(SearchVerbNonContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-container object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set PRSO to lamp
    g.prso = lampPtr;
    g.prsa = V_SEARCH;
    
    // Test SEARCH verb on non-container
    bool result = Verbs::vSearch();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(SearchVerbLockedContainer) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container
    auto safe = std::make_unique<ZObject>(1, "safe");
    safe->addSynonym("safe");
    safe->setFlag(ObjectFlag::CONTBIT);
    safe->setFlag(ObjectFlag::LOCKEDBIT);
    safe->moveTo(&testRoom);
    ZObject* safePtr = safe.get();
    g.registerObject(1, std::move(safe));
    
    // Set PRSO to safe
    g.prso = safePtr;
    g.prsa = V_SEARCH;
    
    // Test SEARCH verb on locked container
    bool result = Verbs::vSearch();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

// Test READ verb - Task 25.5
TEST(ReadVerbBasic) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create readable object
    auto leaflet = std::make_unique<ZObject>(1, "leaflet");
    leaflet->addSynonym("leaflet");
    leaflet->setFlag(ObjectFlag::READBIT);
    leaflet->setText("WELCOME TO ZORK!");
    leaflet->moveTo(&testRoom);
    ZObject* leafletPtr = leaflet.get();
    g.registerObject(1, std::move(leaflet));
    
    // Set PRSO to leaflet
    g.prso = leafletPtr;
    g.prsa = V_READ;
    
    // Test READ verb
    bool result = Verbs::vRead();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ReadVerbNoObject) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set PRSO to nullptr
    g.prso = nullptr;
    g.prsa = V_READ;
    
    // Test READ verb without object
    bool result = Verbs::vRead();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ReadVerbNotReadable) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-readable object
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    // Not setting READBIT flag
    lamp->moveTo(&testRoom);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set PRSO to lamp
    g.prso = lampPtr;
    g.prsa = V_READ;
    
    // Test READ verb on non-readable object
    bool result = Verbs::vRead();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

TEST(ReadVerbNoText) {
    auto& g = Globals::instance();
    
    // Create test room
    ZRoom testRoom(100, "Test Room", "A test room.");
    g.here = &testRoom;
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create readable object without text
    auto book = std::make_unique<ZObject>(1, "book");
    book->addSynonym("book");
    book->setFlag(ObjectFlag::READBIT);
    // Not setting text
    book->moveTo(&testRoom);
    ZObject* bookPtr = book.get();
    g.registerObject(1, std::move(book));
    
    // Set PRSO to book
    g.prso = bookPtr;
    g.prsa = V_READ;
    
    // Test READ verb on readable object without text
    bool result = Verbs::vRead();
    ASSERT_TRUE(result);
    
    // Cleanup
    g.reset();
}

int main() {
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
    
    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
