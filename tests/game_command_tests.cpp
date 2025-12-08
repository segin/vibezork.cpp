#include "test_framework.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"

#include <sstream>

// Output capture helpers
std::streambuf* oldCout;
std::stringstream captureSS;

void startOutputCapture() {
    oldCout = std::cout.rdbuf();
    captureSS.str("");
    captureSS.clear();
    std::cout.rdbuf(captureSS.rdbuf());
}

std::string stopOutputCapture() {
    std::cout.rdbuf(oldCout);
    return captureSS.str();
}

// Helper to reset and setup globals for testing
void setupGameCommandTest() {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a test room
    auto room = std::make_unique<ZRoom>(100, "TEST_ROOM", "Test Room");
    // Description is now passed in constructor or managed by ZRoom internals if separate
    // ZRoom constructor: ZRoom(ObjectId id, string_view desc, string_view longDesc)
    // Wait, let's verify ZRoom constructor again. 
    // From previous error: ZRoom(ObjectId id, string_view desc, string_view longDesc)
    // So "TEST_ROOM" is desc (name), "Test Room" is longDesc? 
    // Usually name is internal ID string, desc is printed name.
    // Let's assume (100, "TEST_ROOM", "This is a test room description.")
    
    // Re-creating room with correct constructor
    room = std::make_unique<ZRoom>(100, "Test Room", "This is a test room description.");
    
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    // Ensure we start with standard defaults
    g.verboseMode = true;
    g.briefMode = false;
    g.superbriefMode = false;
}

TEST(VerboseCommand) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Set to brief first to verify change
    g.briefMode = true;
    g.verboseMode = false;
    
    startOutputCapture();
    Verbs::vVerbose();
    std::string output = stopOutputCapture();
    
    ASSERT_TRUE(g.verboseMode);
    ASSERT_FALSE(g.briefMode);
    ASSERT_FALSE(g.superbriefMode);
    ASSERT_CONTAINS(output, "Maximum verbosity");
}

TEST(BriefCommand) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Set to verbose first
    g.verboseMode = true;
    g.briefMode = false;
    
    startOutputCapture();
    Verbs::vBrief();
    std::string output = stopOutputCapture();
    
    ASSERT_FALSE(g.verboseMode);
    ASSERT_TRUE(g.briefMode);
    ASSERT_FALSE(g.superbriefMode);
    ASSERT_CONTAINS(output, "Brief descriptions");
}

TEST(SuperbriefCommand) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Set to verbose first
    g.verboseMode = true;
    g.superbriefMode = false;
    
    startOutputCapture();
    Verbs::vSuperbrief();
    std::string output = stopOutputCapture();
    
    ASSERT_FALSE(g.verboseMode);
    ASSERT_FALSE(g.briefMode);
    ASSERT_TRUE(g.superbriefMode);
    ASSERT_CONTAINS(output, "Superbrief descriptions");
}

TEST(LookVerbose) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Explicitly set verbose
    Verbs::vVerbose();
    stopOutputCapture(); // clear buffer
    
    // Mark room as visited to ensure verbose ignores it
    g.here->setFlag(ObjectFlag::TOUCHBIT);
    
    startOutputCapture();
    Verbs::vLook();
    std::string output = stopOutputCapture();
    
    ASSERT_CONTAINS(output, "Test Room");
    ASSERT_CONTAINS(output, "This is a test room description");
}

TEST(LookBrief) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Set to Brief
    Verbs::vBrief();
    stopOutputCapture(); // clear buffer
    
    // 1. Not visited -> Show full description
    g.here->clearFlag(ObjectFlag::TOUCHBIT);
    
    startOutputCapture();
    Verbs::vLook();
    std::string output1 = stopOutputCapture();
    
    ASSERT_CONTAINS(output1, "Test Room");
    ASSERT_CONTAINS(output1, "This is a test room description");
    
    // 2. Visited -> Show ONLY room name (no description)
    g.here->setFlag(ObjectFlag::TOUCHBIT);
    
    startOutputCapture();
    Verbs::vLook();
    std::string output2 = stopOutputCapture();
    
    ASSERT_CONTAINS(output2, "Test Room");
    // Ensure description is NOT present
    bool hasDesc = output2.find("This is a test room description") != std::string::npos;
    ASSERT_FALSE(hasDesc);
}

TEST(LookSuperbrief) {
    setupGameCommandTest();
    auto& g = Globals::instance();
    
    // Set to Superbrief
    Verbs::vSuperbrief();
    stopOutputCapture();
    
    // 1. Not visited -> Show ONLY room name
    g.here->clearFlag(ObjectFlag::TOUCHBIT);
    
    startOutputCapture();
    Verbs::vLook();
    std::string output1 = stopOutputCapture();
    
    ASSERT_CONTAINS(output1, "Test Room");
    bool hasDesc1 = output1.find("This is a test room description") != std::string::npos;
    ASSERT_FALSE(hasDesc1);
    
    // 2. Visited -> Show ONLY room name
    g.here->setFlag(ObjectFlag::TOUCHBIT);
    
    startOutputCapture();
    Verbs::vLook();
    std::string output2 = stopOutputCapture();
    
    ASSERT_CONTAINS(output2, "Test Room");
    bool hasDesc2 = output2.find("This is a test room description") != std::string::npos;
    ASSERT_FALSE(hasDesc2);
}

TEST(DiagnoseCommand) {
    setupGameCommandTest();
    
    startOutputCapture();
    Verbs::vDiagnose();
    std::string output = stopOutputCapture();
    
    ASSERT_CONTAINS(output, "perfect health");
}

int main() {
    std::cout << "Running Game Command Tests (VERBOSE, BRIEF, SUPERBRIEF, DIAGNOSE)\n";
    std::cout << "================================================================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
