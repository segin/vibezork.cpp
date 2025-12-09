// Action unit tests for ZIL action routines
#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "../src/verbs/verbs.h"
#include <sstream>

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

// Forward declarations for action functions
extern bool axeAction();
extern bool knifeAction();

// Initialize world for testing
static void setupTestWorld() {
    initializeWorld();
}

// =============================================================================
// AXE-F Tests (1actions.zil lines 622-638)
// ZIL Logic: If troll alive in room, prevents taking axe via WEAPON-FUNCTION
// =============================================================================

TEST(AxeF_TrollInRoomBlocksTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Move player and troll to same room
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !trollRoom) {
        return; // Skip if objects not defined
    }
    
    // Setup: troll and player in troll room, axe on ground
    g.here = trollRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(trollRoom);
    troll->clearFlag(ObjectFlag::NDESCBIT); // Troll is alive
    
    // Try to take axe
    g.prsa = V_TAKE;
    g.prso = axe;
    
    OutputCapture cap;
    bool result = axeAction();
    
    // Should block take and print message
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("swings") != std::string::npos || 
                output.find("white-hot") != std::string::npos);
}

TEST(AxeF_DeadTrollAllowsTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !trollRoom) {
        return;
    }
    
    // Setup: dead troll (NDESCBIT set)
    g.here = trollRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(trollRoom);
    troll->setFlag(ObjectFlag::NDESCBIT); // Troll is dead
    
    g.prsa = V_TAKE;
    g.prso = axe;
    
    bool result = axeAction();
    
    // Should return false, allowing normal take
    ASSERT_FALSE(result);
}

TEST(AxeF_TrollNotInRoomAllowsTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* livingRoom = g.getObject(RoomIds::LIVING_ROOM);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !livingRoom || !trollRoom) {
        return;
    }
    
    // Player in living room, troll in troll room
    g.here = livingRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(livingRoom);
    
    g.prsa = V_TAKE;
    g.prso = axe;
    
    bool result = axeAction();
    
    // Troll not here, should allow take
    ASSERT_FALSE(result);
}

// =============================================================================
// BAG-OF-COINS-F Tests (1actions.zil lines 4137-4152)
// ZIL Logic: STUPID-CONTAINER for coins - blocks OPEN/CLOSE, handles EXAMINE
// =============================================================================

extern bool bagOfCoinsAction();

TEST(BagOfCoinsF_OpenReturnsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    OutputCapture cap;
    bool result = bagOfCoinsAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("coins") != std::string::npos);
    ASSERT_TRUE(output.find("safely inside") != std::string::npos);
}

TEST(BagOfCoinsF_ExamineShowsContents) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    OutputCapture cap;
    bool result = bagOfCoinsAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("lots of") != std::string::npos);
    ASSERT_TRUE(output.find("coins") != std::string::npos);
}

TEST(BagOfCoinsF_TakeReturnsFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    bool result = bagOfCoinsAction();
    
    // TAKE not handled by stupid container, returns false
    ASSERT_FALSE(result);
}

// =============================================================================
// BARROW-DOOR-FCN Tests (1actions.zil lines 432-434)
// ZIL Logic: OPEN/CLOSE prints "The door is too heavy."
// =============================================================================

extern bool barrowDoorAction();

TEST(BarrowDoorFcn_OpenPrintsTooHeavy) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    
    OutputCapture cap;
    bool result = barrowDoorAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("too heavy") != std::string::npos);
}

TEST(BarrowDoorFcn_ClosePrintsTooHeavy) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_CLOSE;
    
    OutputCapture cap;
    bool result = barrowDoorAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("too heavy") != std::string::npos);
}

TEST(BarrowDoorFcn_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = barrowDoorAction();
    
    // Other verbs not handled
    ASSERT_FALSE(result);
}

// =============================================================================
// KNIFE-F Tests (1actions.zil lines 926-929)
// ZIL Logic: On TAKE, clears ATTIC-TABLE NDESCBIT
// =============================================================================

TEST(KnifeF_TakeKnifeClearsAtticTableNdescbit) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Get attic table and set NDESCBIT
    ZObject* atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (!atticTable) {
        // Skip test if ATTIC_TABLE not yet defined
        return;
    }
    
    atticTable->setFlag(ObjectFlag::NDESCBIT);
    ASSERT_TRUE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
    
    // Simulate V_TAKE action
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    // Call knife action
    knifeAction();
    
    // Verify NDESCBIT was cleared
    ASSERT_FALSE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
}

TEST(KnifeF_NonTakeVerbDoesNotAffectTable) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (!atticTable) {
        return;
    }
    
    atticTable->setFlag(ObjectFlag::NDESCBIT);
    
    // Try EXAMINE instead of TAKE
    g.prsa = V_EXAMINE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    knifeAction();
    
    // NDESCBIT should still be set (not cleared for non-TAKE)
    ASSERT_TRUE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
}

TEST(KnifeF_ReturnsFalseToAllowNormalHandling) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    bool result = knifeAction();
    
    // Should return false to allow normal TAKE to proceed
    ASSERT_FALSE(result);
}

// Main test runner
int main(int argc, char* argv[]) {
    std::cout << "Running Action Tests" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << std::endl;
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0, failed = 0;
    for (const auto& r : results) {
        if (r.passed) passed++; else failed++;
    }
    
    std::cout << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    
    return failed > 0 ? 1 : 0;
}
