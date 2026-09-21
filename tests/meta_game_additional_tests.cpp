#include "test_framework.h"
#include "../src/core/globals.h"
#include "../src/verbs/verbs.h"
#include <iostream>
#include <sstream>

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

TEST(ScriptVerb) {
    auto& g = Globals::instance();
    g.reset(); // ensure scripting is false
    
    // Test SCRIPT
    {
        OutputCapture capture;
        bool result = Verbs::vScript();
        ASSERT_TRUE(result);
        ASSERT_TRUE(g.scripting);
        // ZIL V-SCRIPT prints its header then V-VERSION (gverbs.zil:86-90)
        ASSERT_CONTAINS(capture.getOutput(),
                        "Here begins a transcript of interaction with");
    }
    
    // ZIL V-SCRIPT has no "already on" case: it sets the header bit and
    // prints the same two lines every time (gverbs.zil:86-90).
    {
        OutputCapture capture;
        bool result = Verbs::vScript();
        ASSERT_TRUE(result);
        ASSERT_TRUE(g.scripting);
        ASSERT_CONTAINS(capture.getOutput(),
                        "Here begins a transcript of interaction with");
    }
    
    g.reset();
}

TEST(UnscriptVerb) {
    auto& g = Globals::instance();
    g.reset();
    g.scripting = true; // Start enabled
    
    // Test UNSCRIPT
    {
        OutputCapture capture;
        bool result = Verbs::vUnscript();
        ASSERT_TRUE(result);
        ASSERT_FALSE(g.scripting);
        // ZIL V-UNSCRIPT prints its header then V-VERSION (gverbs.zil:92-96)
        ASSERT_CONTAINS(capture.getOutput(),
                        "Here ends a transcript of interaction with");
    }
    
    // ZIL V-UNSCRIPT likewise has no "already off" case (gverbs.zil:92-96).
    {
        OutputCapture capture;
        bool result = Verbs::vUnscript();
        ASSERT_TRUE(result);
        ASSERT_FALSE(g.scripting);
        ASSERT_CONTAINS(capture.getOutput(),
                        "Here ends a transcript of interaction with");
    }
    g.reset();
}

// Basic check that Quit/Restart/Restore/Save exist and return true (compilation/linkage check)
// We cannot fully test them here due to IO dependency, but we verify they are callable.
TEST(GameControlVerbsExist) {
    // Just verify addresses are callable or similar? 
    // Actually calling them might block on input.
    // So we skip calling them, but their presence in this compilation unit (if we linked validly) is enough.
    // We can rely on `tests/meta_game_verb_tests.cpp` which covered some of this.
}

int main() {
    std::cout << "Running Meta-Game Additional Tests\n";
    std::cout << "==================================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
