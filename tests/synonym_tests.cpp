#include "test_framework.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"

TEST(DirectionSynonyms) {
    Parser parser;
    
    // North
    auto cmd = parser.parse("north");
    ASSERT_EQ(cmd.verb, V_WALK);
    ASSERT_TRUE(cmd.isDirection);
    ASSERT_EQ(cmd.direction, Direction::NORTH);
    
    cmd = parser.parse("n");
    ASSERT_EQ(cmd.direction, Direction::NORTH);
    
    // South
    cmd = parser.parse("south");
    ASSERT_EQ(cmd.direction, Direction::SOUTH);
    cmd = parser.parse("s");
    ASSERT_EQ(cmd.direction, Direction::SOUTH);
    
    // East
    cmd = parser.parse("east");
    ASSERT_EQ(cmd.direction, Direction::EAST);
    cmd = parser.parse("e");
    ASSERT_EQ(cmd.direction, Direction::EAST);
    
    // West
    cmd = parser.parse("west");
    ASSERT_EQ(cmd.direction, Direction::WEST);
    cmd = parser.parse("w");
    ASSERT_EQ(cmd.direction, Direction::WEST);
    
    // NE
    cmd = parser.parse("ne");
    ASSERT_EQ(cmd.direction, Direction::NE);
    cmd = parser.parse("northeast");
    ASSERT_EQ(cmd.direction, Direction::NE);
    
    // NW
    cmd = parser.parse("nw");
    ASSERT_EQ(cmd.direction, Direction::NW);
    cmd = parser.parse("northwest");
    ASSERT_EQ(cmd.direction, Direction::NW);
    
    // SE
    cmd = parser.parse("se");
    ASSERT_EQ(cmd.direction, Direction::SE);
    cmd = parser.parse("southeast");
    ASSERT_EQ(cmd.direction, Direction::SE);
    
    // SW
    cmd = parser.parse("sw");
    ASSERT_EQ(cmd.direction, Direction::SW);
    cmd = parser.parse("southwest");
    ASSERT_EQ(cmd.direction, Direction::SW);
    
    // Up/Down
    cmd = parser.parse("up");
    ASSERT_EQ(cmd.direction, Direction::UP);
    cmd = parser.parse("u");
    ASSERT_EQ(cmd.direction, Direction::UP);
    
    cmd = parser.parse("down");
    ASSERT_EQ(cmd.direction, Direction::DOWN);
    cmd = parser.parse("d");
    ASSERT_EQ(cmd.direction, Direction::DOWN);
    
    // In/Out
    cmd = parser.parse("in");
    ASSERT_EQ(cmd.direction, Direction::IN);
    cmd = parser.parse("inside");
    ASSERT_EQ(cmd.direction, Direction::IN);
    
    cmd = parser.parse("out");
    ASSERT_EQ(cmd.direction, Direction::OUT);
    cmd = parser.parse("outside");
    ASSERT_EQ(cmd.direction, Direction::OUT);
}

TEST(VerbSynonymsTake) {
    Parser parser;
    auto& g = Globals::instance();
    g.reset();
    
    // TAKE requires an object. Without one, it should orphan.
    auto cmd = parser.parse("take");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
    
    // Clear orphan for next test
    parser.clearOrphan();
    
    cmd = parser.parse("get");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
}

TEST(VerbSynonymsLook) {
    Parser parser;
    auto cmd = parser.parse("look");
    ASSERT_EQ(cmd.verb, V_LOOK);
    
    cmd = parser.parse("l");
    ASSERT_EQ(cmd.verb, V_LOOK);
}

TEST(VerbSynonymsAttack) {
    Parser parser;
    auto& g = Globals::instance();
    g.reset();
    
    auto cmd = parser.parse("attack");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
    
    parser.clearOrphan();
    
    cmd = parser.parse("kill");
    ASSERT_EQ(cmd.verb, 0);
    ASSERT_TRUE(parser.isOrphaned());
}


#include "../src/parser/verb_registry.h"

TEST(RegistrySynonyms) {
    VerbRegistry registry;
    
    // Helper to check synonyms
    auto check = [&](VerbId id, const std::vector<std::string>& synonyms) {
        for (const auto& syn : synonyms) {
            auto result = registry.lookupVerb(syn);
            if (!result.has_value()) {
                 std::cout << "Missing synonym: '" << syn << "'" << std::endl;
                 ASSERT_TRUE(false);
            }
            if (result.value() != id) {
                 std::cout << "Mismatch for '" << syn << "': Expected " << id << ", Actual " << result.value() << std::endl;
                 ASSERT_EQ(result.value(), id);
            }
        }
    };

    // Game Commands
    check(V_VERBOSE, {"verbose"});
    check(V_BRIEF, {"brief"});
    check(V_SUPERBRIEF, {"superbrief", "super"});
    check(V_DIAGNOSE, {"diagnose"});
    check(V_INVENTORY, {"inventory", "i"});
    check(V_QUIT, {"quit", "q"});
    check(V_RESTART, {"restart"});
    check(V_RESTORE, {"restore"});
    check(V_SAVE, {"save"});
    check(V_SCORE, {"score"});
    check(V_VERSION, {"version"});

    // Manipulation
    check(V_TAKE, {"take", "get", "hold", "carry", "remove", "grab", "catch"});
    check(V_DROP, {"drop", "leave"});
    check(V_PUT, {"put", "stuff", "insert", "place", "hide"});
    check(V_GIVE, {"give", "donate", "offer", "feed"});
    
    // Examination
    check(V_LOOK, {"look", "l", "stare", "gaze"});
    check(V_EXAMINE, {"examine", "describe", "what", "whats", "x"});
    check(V_READ, {"read", "skim"});
    check(V_SEARCH, {"search"});

    // Container
    check(V_OPEN, {"open"});
    check(V_CLOSE, {"close"});
    check(V_LOCK, {"lock"});
    check(V_UNLOCK, {"unlock"});

    // Movement
    check(V_WALK, {"walk", "go", "run", "proceed", "step"});
    check(V_ENTER, {"enter"});
    check(V_EXIT, {"exit"});
    check(V_BOARD, {"board"});
    check(V_DISEMBARK, {"disembark"});
    
    // Combat
    check(V_ATTACK, {"attack", "fight", "hurt", "injure", "hit", "strike"});
    check(V_KILL, {"kill", "murder", "slay", "dispatch"});
    check(V_THROW, {"throw", "hurl", "chuck", "toss"});
    check(V_SWING, {"swing", "thrust"});

    // Light
    check(V_LAMP_ON, {"light", "activate"});
    check(V_LAMP_OFF, {"extinguish", "douse"});

    // Manipulation (Misc)
    check(V_TURN, {"turn", "set", "flip", "shut"});
    check(V_PUSH, {"push", "press"});
    check(V_PULL, {"pull", "tug", "yank"});
    check(V_MOVE, {"move"});

    // Interaction
    check(V_TIE, {"tie", "fasten", "secure", "attach"});
    check(V_UNTIE, {"untie", "free", "release", "unfasten"});
    check(V_LISTEN, {"listen"});
    check(V_SMELL, {"smell", "sniff"});
    check(V_TOUCH, {"touch"});
    check(V_RUB, {"rub", "feel", "pat", "pet"});
    check(V_YELL, {"yell", "scream", "shout", "holler"});

    // Consumption
    check(V_EAT, {"eat", "consume", "taste", "bite"});
    check(V_DRINK, {"drink", "imbibe", "swallow"});

    // Special
    check(V_INFLATE, {"inflate"});
    check(V_DEFLATE, {"deflate"});
    check(V_PRAY, {"pray"});
    check(V_EXORCISE, {"exorcise", "banish", "cast", "drive", "begone"});
    check(V_WAVE, {"wave", "brandish"});
    check(V_RING, {"ring", "peal"});
    check(V_BURN, {"burn", "incinerate", "ignite"});
    check(V_DIG, {"dig"});
    check(V_FILL, {"fill"});

    // Communication
    check(V_TALK, {"talk", "speak"});
    check(V_ASK, {"ask"});
    check(V_TELL, {"tell"});
    check(V_TELL, {"tell"});
    check(V_ODYSSEUS, {"odysseus", "ulysses"});

    // ZIL Audit High Priority
    check(V_MUNG, {"destroy", "damage", "break", "block", "smash"});
    check(V_WEAR, {"wear"});
    check(V_FIND, {"find", "where", "seek"});
    check(V_LEAP, {"leap", "dive"});
    check(V_SAY, {"say"});
    check(V_KICK, {"kick"});
    check(V_BREATHE, {"breathe"});
    check(V_RAPE, {"rape", "molest"});
    
    // Movement Batch Verification
    check(V_STAND, {"stand"});
    check(V_ALARM, {"wake", "awake", "surprise", "startle"});
    check(V_LAUNCH, {"launch"});

    // Manipulation Batch Verification
    check(V_CUT, {"cut", "slice", "pierce"});
    check(V_LOWER, {"lower"});
    check(V_RAISE, {"raise", "lift"});
    check(V_MAKE, {"make"});
    check(V_MELT, {"melt", "liquefy"});
    check(V_PLAY, {"play"});
    check(V_PLUG, {"plug", "glue", "patch", "repair", "fix"});
    check(V_POUR_ON, {"pour", "spill"});
    check(V_SHAKE, {"shake"});
    check(V_SPIN, {"spin"});
    check(V_SQUEEZE, {"squeeze"});
    check(V_WIND, {"wind"});

    // Interaction Batch Verification
    check(V_ANSWER, {"answer"});
    check(V_REPLY, {"reply"});
    check(V_COMMAND, {"command"});
    check(V_ECHO, {"echo"});
    check(V_FOLLOW, {"follow", "pursue", "chase", "come with"});
    check(V_KISS, {"kiss"});
    check(V_MUMBLE, {"mumble", "sigh"});
    check(V_REPENT, {"repent"});
    check(V_SEND, {"send"});
    check(V_WISH, {"wish"});
    check(V_SPRAY, {"spray"});
    
    // Easter Eggs / Misc
    check(V_WAIT, {"wait"}); // zswait
    check(V_SWIM, {"swim"});
    // check(V_BACK, {"back"}); // Not in registry explicitly?? Let's check listing.
    // V_BACK is id 142.
    // Checking previous VIEW of verb_registry.cpp... I don't recall seeing V_BACK registered.
    // Wait, let me check the file content I viewed in step 986...
    // It stopped at line 150. I need to check further down or assume.
    // Wait, I saw V_ODYSSEUS at line 104 in step 986. That was the END of initializeVerbSynonyms function.
    // So V_WAIT, V_SWIM, V_BACK might be missing from registerVerb?
    // Let's verify if they ARE missing.
}
int main() {
    std::cout << "Running Synonym Tests\n";
    std::cout << "=====================\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    std::cout << "\n" << (results.size() - failed) << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
