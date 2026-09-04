#include "parser/gsyntax.h"
#include "parser/verb_registry.h"
#include "verbs/verbs.h"
#include "core/flags.h"

#include <cassert>
#include <iostream>
#include <print>
#include <string>
#include <vector>

// ZIL: Test suite for gsyntax.zil declarations, tables, and patterns
// Source: zil/gsyntax.zil:1-562

void testBuzzwords() {
    std::println("Testing Buzzwords (BUZZ)...");
    
    // Command buzzwords (zil/gsyntax.zil:9)
    assert(GSyntax::isBuzzWord("again"));
    assert(GSyntax::isBuzzWord("AGAIN"));
    assert(GSyntax::isBuzzWord("g"));
    assert(GSyntax::isBuzzWord("G"));
    assert(GSyntax::isBuzzWord("oops"));
    assert(GSyntax::isBuzzWord("OOPS"));

    // General buzzwords (zil/gsyntax.zil:11)
    assert(GSyntax::isBuzzWord("a"));
    assert(GSyntax::isBuzzWord("an"));
    assert(GSyntax::isBuzzWord("the"));
    assert(GSyntax::isBuzzWord("is"));
    assert(GSyntax::isBuzzWord("and"));
    assert(GSyntax::isBuzzWord("of"));
    assert(GSyntax::isBuzzWord("then"));
    assert(GSyntax::isBuzzWord("all"));
    assert(GSyntax::isBuzzWord("one"));
    assert(GSyntax::isBuzzWord("but"));
    assert(GSyntax::isBuzzWord("except"));
    assert(GSyntax::isBuzzWord("."));
    assert(GSyntax::isBuzzWord(","));
    assert(GSyntax::isBuzzWord("\""));
    assert(GSyntax::isBuzzWord("yes"));
    assert(GSyntax::isBuzzWord("no"));
    assert(GSyntax::isBuzzWord("y"));
    assert(GSyntax::isBuzzWord("here"));

    // Non-buzzwords
    assert(!GSyntax::isBuzzWord("sword"));
    assert(!GSyntax::isBuzzWord("troll"));
    assert(!GSyntax::isBuzzWord("take"));

    assert(GSyntax::getBuzzWords().size() >= 20);
    std::println("✓ Buzzwords verified against gsyntax.zil:9-12");
}

void testPrepositions() {
    std::println("Testing Preposition Synonyms (SYNONYM)...");

    // WITH USING THROUGH THRU (zil/gsyntax.zil:20)
    assert(GSyntax::isPreposition("with"));
    assert(GSyntax::isPreposition("using"));
    assert(GSyntax::isPreposition("through"));
    assert(GSyntax::isPreposition("thru"));
    assert(GSyntax::canonicalPreposition("with") == "with");
    assert(GSyntax::canonicalPreposition("using") == "with");
    assert(GSyntax::canonicalPreposition("through") == "with");
    assert(GSyntax::canonicalPreposition("thru") == "with");

    // IN INSIDE INTO (zil/gsyntax.zil:21)
    assert(GSyntax::isPreposition("in"));
    assert(GSyntax::isPreposition("inside"));
    assert(GSyntax::isPreposition("into"));
    assert(GSyntax::canonicalPreposition("inside") == "in");
    assert(GSyntax::canonicalPreposition("into") == "in");

    // ON ONTO (zil/gsyntax.zil:22)
    assert(GSyntax::isPreposition("on"));
    assert(GSyntax::isPreposition("onto"));
    assert(GSyntax::canonicalPreposition("onto") == "on");

    // UNDER UNDERNEATH BENEATH BELOW (zil/gsyntax.zil:23)
    assert(GSyntax::isPreposition("under"));
    assert(GSyntax::isPreposition("underneath"));
    assert(GSyntax::isPreposition("beneath"));
    assert(GSyntax::isPreposition("below"));
    assert(GSyntax::canonicalPreposition("underneath") == "under");
    assert(GSyntax::canonicalPreposition("beneath") == "under");
    assert(GSyntax::canonicalPreposition("below") == "under");

    // Check synonyms list
    const auto& withSyns = GSyntax::getPrepositionSynonyms("with");
    assert(!withSyns.empty());

    std::println("✓ Prepositions verified against gsyntax.zil:20-23");
}

void testDirections() {
    std::println("Testing Direction Synonyms (SYNONYM)...");

    // Cardinal directions (zil/gsyntax.zil:25-30)
    assert(GSyntax::isDirection("north"));
    assert(GSyntax::isDirection("n"));
    assert(GSyntax::canonicalDirection("n") == "north");
    assert(GSyntax::canonicalDirection("s") == "south");
    assert(GSyntax::canonicalDirection("e") == "east");
    assert(GSyntax::canonicalDirection("w") == "west");
    assert(GSyntax::canonicalDirection("d") == "down");
    assert(GSyntax::canonicalDirection("u") == "up");

    // Diagonal directions (zil/gsyntax.zil:31-34)
    assert(GSyntax::isDirection("nw"));
    assert(GSyntax::isDirection("northwest"));
    assert(GSyntax::canonicalDirection("northwest") == "nw");
    assert(GSyntax::canonicalDirection("northe") == "ne");
    assert(GSyntax::canonicalDirection("southwest") == "sw");
    assert(GSyntax::canonicalDirection("southe") == "se");

    std::println("✓ Directions verified against gsyntax.zil:25-34");
}

void testVerbSynonyms() {
    std::println("Testing Verb Synonyms (SYNONYM)...");

    // Command synonyms
    assert(GSyntax::lookupVerb("superbrief") == V_SUPERBRIEF);
    assert(GSyntax::lookupVerb("super") == V_SUPERBRIEF);
    assert(GSyntax::lookupVerb("i") == V_INVENTORY);
    assert(GSyntax::lookupVerb("inventory") == V_INVENTORY);
    assert(GSyntax::lookupVerb("q") == V_QUIT);
    assert(GSyntax::lookupVerb("quit") == V_QUIT);

    // Manipulation verb synonyms
    assert(GSyntax::lookupVerb("get") == V_TAKE);
    assert(GSyntax::lookupVerb("hold") == V_TAKE);
    assert(GSyntax::lookupVerb("carry") == V_TAKE);
    assert(GSyntax::lookupVerb("remove") == V_TAKE);
    assert(GSyntax::lookupVerb("grab") == V_TAKE);
    assert(GSyntax::lookupVerb("catch") == V_TAKE);
    assert(GSyntax::lookupVerb("take") == V_TAKE);

    // Give synonyms including Zork 1 condition (GIVE HAND)
    assert(GSyntax::lookupVerb("give") == V_GIVE);
    assert(GSyntax::lookupVerb("donate") == V_GIVE);
    assert(GSyntax::lookupVerb("offer") == V_GIVE);
    assert(GSyntax::lookupVerb("feed") == V_GIVE);
    assert(GSyntax::lookupVerb("hand") == V_GIVE);

    // Combat synonyms
    assert(GSyntax::lookupVerb("attack") == V_ATTACK);
    assert(GSyntax::lookupVerb("fight") == V_ATTACK);
    assert(GSyntax::lookupVerb("hurt") == V_ATTACK);
    assert(GSyntax::lookupVerb("injure") == V_ATTACK);
    assert(GSyntax::lookupVerb("hit") == V_ATTACK);
    assert(GSyntax::lookupVerb("kill") == V_KILL);
    assert(GSyntax::lookupVerb("murder") == V_KILL);
    assert(GSyntax::lookupVerb("slay") == V_KILL);
    assert(GSyntax::lookupVerb("dispatch") == V_KILL);

    // Look synonyms
    assert(GSyntax::lookupVerb("look") == V_LOOK);
    assert(GSyntax::lookupVerb("l") == V_LOOK);
    assert(GSyntax::lookupVerb("stare") == V_LOOK);
    assert(GSyntax::lookupVerb("gaze") == V_LOOK);

    // Light synonyms
    assert(GSyntax::lookupVerb("extinguish") == V_LAMP_OFF);
    assert(GSyntax::lookupVerb("douse") == V_LAMP_OFF);

    // Plugh / Xyzzy
    assert(GSyntax::lookupVerb("plugh") == V_PLUGH);
    assert(GSyntax::lookupVerb("xyzzy") == V_PLUGH);

    // Curiosities
    assert(GSyntax::lookupVerb("lose") == V_CHOMP);
    assert(GSyntax::lookupVerb("barf") == V_CHOMP);
    assert(GSyntax::lookupVerb("sit") == V_CLIMB);
    assert(GSyntax::lookupVerb("z") == V_WAIT);

    std::println("✓ Verb synonyms verified against gsyntax.zil");
}

void testAllSyntaxesCompleteness() {
    std::println("Testing All 267 Syntaxes Completeness...");
    const auto& all = GSyntax::getAllSyntaxes();
    assert(all.size() == 267);

    for (const auto& syn : all) {
        assert(!syn.zilSource.empty());
        assert(!syn.patternString.empty());
        assert(!syn.verb.empty());
        assert(syn.verbId > 0);
        assert(!syn.actionName.empty());
        assert(syn.actionId > 0);
        assert(!syn.elements.empty());
        assert(syn.elements[0].type == GSyntax::SyntaxElement::Type::VERB);
    }

    std::println("✓ Verified 267 syntaxes parsed with full metadata");
}

void testSpecificSyntaxPatterns() {
    std::println("Testing Specific Syntax Patterns against ZIL source...");

    // 1. VERBOSE = V-VERBOSE (zil/gsyntax.zil:40)
    auto verboseList = GSyntax::getSyntaxesForVerb("verbose");
    assert(!verboseList.empty());
    assert(verboseList[0]->actionId == V_VERBOSE);
    assert(verboseList[0]->elements.size() == 1);

    // 2. TAKE OBJECT (FIND TAKEBIT) (ON-GROUND IN-ROOM MANY) = V-TAKE PRE-TAKE (zil/gsyntax.zil:469)
    auto takeList = GSyntax::getSyntaxesForVerb("take");
    assert(!takeList.empty());
    bool foundSimpleTake = false;
    for (const auto* s : takeList) {
        if (s->elements.size() == 2 && s->elements[1].type == GSyntax::SyntaxElement::Type::OBJECT) {
            foundSimpleTake = true;
            assert(s->actionId == V_TAKE);
            assert(s->preactionId == V_TAKE);
            assert(s->elements[1].findFlag == ObjectFlag::TAKEBIT);
            assert((s->elements[1].scopeFlags & GSyntax::SH_ON_GROUND) != 0);
            assert((s->elements[1].scopeFlags & GSyntax::SH_IN_ROOM) != 0);
            assert(s->elements[1].many == true);
            break;
        }
    }
    assert(foundSimpleTake);

    // 3. ATTACK OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE) = V-ATTACK (zil/gsyntax.zil:96)
    auto attackList = GSyntax::getSyntaxesForVerb("attack");
    assert(!attackList.empty());
    bool foundAttackWith = false;
    for (const auto* s : attackList) {
        if (s->elements.size() == 4) {
            foundAttackWith = true;
            assert(s->actionId == V_ATTACK);
            assert(s->elements[1].type == GSyntax::SyntaxElement::Type::OBJECT);
            assert(s->elements[1].findFlag == ObjectFlag::ACTORBIT);
            assert(s->elements[2].type == GSyntax::SyntaxElement::Type::PREPOSITION);
            assert(s->elements[2].value == "with");
            assert(s->elements[3].type == GSyntax::SyntaxElement::Type::OBJECT);
            assert(s->elements[3].findFlag == ObjectFlag::WEAPONBIT);
            assert(s->elements[3].have == true);
            break;
        }
    }
    assert(foundAttackWith);

    // 4. PUT OBJECT (HELD MANY HAVE) IN OBJECT = V-PUT PRE-PUT (zil/gsyntax.zil:384)
    auto putList = GSyntax::getSyntaxesForVerb("put");
    assert(!putList.empty());
    bool foundPutIn = false;
    bool foundPutOn = false;
    for (const auto* s : putList) {
        if (s->elements.size() == 4 && s->elements[2].value == "in") {
            foundPutIn = true;
            assert(s->actionId == V_PUT);
            assert(s->preactionId == V_PUT);
        }
        if (s->elements.size() == 4 && s->elements[2].value == "on") {
            foundPutOn = true;
            assert(s->actionId == V_PUT_ON);
            assert(s->preactionId == V_PUT);
        }
    }
    assert(foundPutIn);
    assert(foundPutOn);

    // 5. BLOW OUT OBJECT = V-LAMP-OFF (zil/gsyntax.zil:104)
    //    BLOW UP OBJECT WITH OBJECT = V-INFLATE (zil/gsyntax.zil:105)
    //    BLOW UP OBJECT = V-BLAST (zil/gsyntax.zil:107)
    //    BLOW IN OBJECT = V-BREATHE (zil/gsyntax.zil:108)
    auto blowList = GSyntax::getSyntaxesForVerb("blow");
    assert(blowList.size() == 4);

    std::println("✓ Specific syntax patterns verified");
}

void testSyntaxMatching() {
    std::println("Testing Syntax Pattern Matching...");

    // Simple verb
    const auto* syn1 = GSyntax::matchSyntax("verbose", {}, 0);
    assert(syn1 != nullptr);
    assert(syn1->actionId == V_VERBOSE);

    // Take object
    const auto* syn2 = GSyntax::matchSyntax("take", {}, 1);
    assert(syn2 != nullptr);
    assert(syn2->actionId == V_TAKE);

    // Take object from object (from / out / off)
    const auto* syn3 = GSyntax::matchSyntax("take", {"from"}, 2);
    assert(syn3 != nullptr);
    assert(syn3->actionId == V_TAKE);

    // Put object in object
    const auto* syn4 = GSyntax::matchSyntax("put", {"in"}, 2);
    assert(syn4 != nullptr);
    assert(syn4->actionId == V_PUT);

    // Put object on object -> V_PUT_ON
    const auto* syn5 = GSyntax::matchSyntax("put", {"on"}, 2);
    assert(syn5 != nullptr);
    assert(syn5->actionId == V_PUT_ON);

    // Attack object with object
    const auto* syn6 = GSyntax::matchSyntax("attack", {"with"}, 2);
    assert(syn6 != nullptr);
    assert(syn6->actionId == V_ATTACK);

    // Synonyms in matching (e.g. inside -> in)
    const auto* syn7 = GSyntax::matchSyntax("put", {"inside"}, 2);
    assert(syn7 != nullptr);
    assert(syn7->actionId == V_PUT);

    std::println("✓ Syntax matching verified");
}

void testVerbRegistryPopulation() {
    std::println("Testing VerbRegistry Integration...");
    VerbRegistry registry;
    GSyntax::populateVerbRegistry(registry);

    // Check registered patterns
    assert(registry.hasPatterns(V_TAKE));
    assert(registry.hasPatterns(V_PUT));
    assert(registry.hasPatterns(V_ATTACK));
    assert(registry.hasPatterns(V_OPEN));
    assert(registry.hasPatterns(V_CLOSE));

    // Check prepositions
    assert(registry.isPrepositionValidForVerb(V_TAKE, "from"));
    assert(registry.isPrepositionValidForVerb(V_PUT, "in"));
    assert(registry.isPrepositionValidForVerb(V_PUT, "on"));
    assert(registry.isPrepositionValidForVerb(V_ATTACK, "with"));

    // Check syntax disambiguation (e.g. PUT ON)
    auto putOnId = registry.getVerbIdForSyntax(V_PUT, "on");
    assert(putOnId.has_value());
    assert(*putOnId == V_PUT_ON);

    std::println("✓ VerbRegistry integration verified");
}

int main() {
    std::println("========================================");
    std::println("Running GSYNTAX (zil/gsyntax.zil) Tests");
    std::println("========================================");

    testBuzzwords();
    testPrepositions();
    testDirections();
    testVerbSynonyms();
    testAllSyntaxesCompleteness();
    testSpecificSyntaxPatterns();
    testSyntaxMatching();
    testVerbRegistryPopulation();

    std::println("========================================");
    std::println("All GSYNTAX tests passed successfully! ✓");
    std::println("========================================");
    return 0;
}
