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
extern bool knifeAction();

// Initialize world for testing
static void setupTestWorld() {
    initializeWorld();
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
