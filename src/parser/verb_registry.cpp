#include "verb_registry.h"
#include "verbs/verbs.h"
#include <algorithm>

// Static member initialization
const std::vector<SyntaxPattern> VerbRegistry::emptyPatterns_;

VerbRegistry::VerbRegistry() {
    // Initialize all verb synonyms from gsyntax.zil
    initializeVerbSynonyms();
    
    // Initialize all syntax patterns from gsyntax.zil
    initializeSyntaxPatterns();
}

void VerbRegistry::initializeVerbSynonyms() {
    // Game commands
    registerVerb(V_VERBOSE, {"verbose"});
    registerVerb(V_BRIEF, {"brief"});
    registerVerb(V_SUPERBRIEF, {"superbrief", "super"});
    registerVerb(V_DIAGNOSE, {"diagnose"});
    registerVerb(V_INVENTORY, {"inventory", "i"});
    registerVerb(V_QUIT, {"quit", "q"});
    registerVerb(V_RESTART, {"restart"});
    registerVerb(V_RESTORE, {"restore"});
    registerVerb(V_SAVE, {"save"});
    registerVerb(V_SCORE, {"score"});
    registerVerb(V_VERSION, {"version"});
    
    // Manipulation verbs
    registerVerb(V_TAKE, {"take", "get", "hold", "carry", "remove", "grab", "catch"});
    registerVerb(V_DROP, {"drop", "leave"});
    registerVerb(V_PUT, {"put", "stuff", "insert", "place", "hide"});
    // registerVerb(V_PUT_ON, {"put"});  // "put on" handled by syntax pattern
    registerVerb(V_GIVE, {"give", "donate", "offer", "feed"});
    
    // Examination verbs
    registerVerb(V_LOOK, {"look", "l", "stare", "gaze"});
    registerVerb(V_EXAMINE, {"examine", "describe", "what", "whats", "x"});
    registerVerb(V_READ, {"read", "skim"});
    // registerVerb(V_LOOK_INSIDE, {"look"});  // "look in/inside" handled by syntax under V_LOOK
    registerVerb(V_SEARCH, {"search"});
    
    // Container operations
    registerVerb(V_JUMP, {"jump"});
    registerVerb(V_CURSE, {"curse", "damn"});
    
    // ZIL Audit Verbs
    registerVerb(V_MUNG, {"destroy", "damage", "break", "block", "smash"});
    registerVerb(V_WEAR, {"wear"});
    registerVerb(V_FIND, {"find", "where", "seek"});
    registerVerb(V_LEAP, {"leap", "dive"});
    registerVerb(V_SAY, {"say"});
    registerVerb(V_KICK, {"kick"});
    // V_BREATHE is usually "blow in", handled by syntax. Register "breathe" as synonym just in case.
    registerVerb(V_BREATHE, {"breathe"});
    registerVerb(V_RAPE, {"rape", "molest"});
    registerVerb(V_OPEN, {"open"});
    registerVerb(V_CLOSE, {"close"});
    registerVerb(V_LOCK, {"lock"});
    registerVerb(V_UNLOCK, {"unlock"});
    
    // Movement verbs
    registerVerb(V_WALK, {"walk", "go", "run", "proceed", "step"});
    registerVerb(V_ENTER, {"enter"});
    registerVerb(V_EXIT, {"exit"});
    // registerVerb(V_CLIMB_UP, {"climb"});  // "climb up" handled by syntax
    // registerVerb(V_CLIMB_DOWN, {"climb"});  // "climb down" handled by syntax
    // registerVerb(V_CLIMB_ON, {"climb", "sit"});  // "climb on" handled by syntax
    registerVerb(V_CLIMB_UP, {"climb"}); // Default climb -> V_CLIMB_UP
    registerVerb(V_CLIMB_ON, {"sit"});   // Sit -> V_CLIMB_ON (or V_SIT if exists, but ZIL maps sit to climb-on)
    registerVerb(V_BOARD, {"board"});
    registerVerb(V_DISEMBARK, {"disembark"});
    
    // Combat verbs
    registerVerb(V_ATTACK, {"attack", "fight", "hurt", "injure", "hit", "strike"});
    registerVerb(V_KILL, {"kill", "murder", "slay", "dispatch"});
    registerVerb(V_THROW, {"throw", "hurl", "chuck", "toss"});
    registerVerb(V_SWING, {"swing", "thrust"});
    
    // Light verbs
    registerVerb(V_LAMP_ON, {"light", "activate"});
    registerVerb(V_LAMP_OFF, {"extinguish", "douse"});
    
    // Manipulation verbs
    registerVerb(V_TURN, {"turn", "set", "flip", "shut"});
    registerVerb(V_PUSH, {"push", "press"});
    registerVerb(V_PULL, {"pull", "tug", "yank"});
    registerVerb(V_MOVE, {"move"});
    
    // Interaction verbs
    registerVerb(V_TIE, {"tie", "fasten", "secure", "attach"});
    registerVerb(V_UNTIE, {"untie", "free", "release", "unfasten", "unattach", "unhook"});
    registerVerb(V_LISTEN, {"listen"});
    registerVerb(V_SMELL, {"smell", "sniff"});
    registerVerb(V_TOUCH, {"touch"});
    registerVerb(V_RUB, {"rub", "feel", "pat", "pet"});
    registerVerb(V_YELL, {"yell", "scream", "shout", "holler"});
    
    // Consumption verbs
    registerVerb(V_EAT, {"eat", "consume", "taste", "bite"});
    registerVerb(V_DRINK, {"drink", "imbibe", "swallow"});
    
    // Special action verbs
    registerVerb(V_INFLATE, {"inflate"});
    registerVerb(V_DEFLATE, {"deflate"});
    registerVerb(V_PRAY, {"pray"});
    registerVerb(V_EXORCISE, {"exorcise", "banish", "cast", "drive", "begone"});
    registerVerb(V_WAVE, {"wave", "brandish"});
    registerVerb(V_RING, {"ring", "peal"});
    registerVerb(V_BURN, {"burn", "incinerate", "ignite"});
    registerVerb(V_DIG, {"dig"});
    registerVerb(V_FILL, {"fill"});
    
    // Communication verbs
    registerVerb(V_TALK, {"talk", "speak", "say"});
    registerVerb(V_ASK, {"ask"});
    registerVerb(V_TELL, {"tell"});
    registerVerb(V_ODYSSEUS, {"odysseus", "ulysses"});  // Special verb for cyclops puzzle
    
    // Easter eggs / Misc / Additional common verbs
    registerVerb(V_HELLO, {"hello", "hi"});
    registerVerb(V_ZORK, {"zork"});
    registerVerb(V_PLUGH, {"plugh", "xyzzy"});
    registerVerb(V_FROBOZZ, {"frobozz"});
    
    registerVerb(V_WAIT, {"wait", "z"});
    registerVerb(V_SWIM, {"swim"});
    registerVerb(V_BACK, {"back"});
    registerVerb(V_JUMP, {"jump"});
    registerVerb(V_CURSE, {"curse", "damn", "shit", "fuck"}); // Combined
    
    // ZIL Audit Verbs
    registerVerb(V_MUNG, {"destroy", "damage", "break", "block", "smash"});
    registerVerb(V_WEAR, {"wear"});
    registerVerb(V_FIND, {"find", "where", "seek"});
    registerVerb(V_LEAP, {"leap", "dive", "skip", "hop"}); // Moved skip/hop here
    registerVerb(V_SAY, {"say"});
    registerVerb(V_KICK, {"kick"});
    registerVerb(V_BREATHE, {"breathe"});
    registerVerb(V_RAPE, {"rape", "molest"});
    
    // Movement Batch
    registerVerb(V_STAND, {"stand"});
    registerVerb(V_ALARM, {"wake", "awake", "surprise", "startle"});
    registerVerb(V_LAUNCH, {"launch"});
    // V_WALK_AROUND and V_WALK_TO are handled by syntax under V_WALK ("walk")
    // See initializeSyntaxPatterns mappings

    // Manipulation Batch for Phase 10.3
    registerVerb(V_CUT, {"cut", "slice", "pierce"});
    registerVerb(V_LOWER, {"lower"});
    registerVerb(V_RAISE, {"raise", "lift"});
    registerVerb(V_MAKE, {"make"});
    registerVerb(V_MELT, {"melt", "liquefy"});
    registerVerb(V_PLAY, {"play"});
    registerVerb(V_PLUG, {"plug", "glue", "patch", "repair", "fix"});
    registerVerb(V_POUR_ON, {"pour", "spill"}); // Syntax will map specific uses
    registerVerb(V_SHAKE, {"shake"});
    registerVerb(V_SPIN, {"spin"});
    registerVerb(V_SQUEEZE, {"squeeze"});
    registerVerb(V_WIND, {"wind"});
    // V_TIE exists? If not, need to check. Grep said V_TIE = 90.
    // registerVerb(V_TIE, {"tie", "fasten"}); // likely already exists in main block
    
    // TIE UP is likely handled by "tie" + "up" preposition in syntax.

}

void VerbRegistry::registerVerb(VerbId verbId, std::vector<std::string> synonyms) {
    // Register each synonym to map to this verb ID
    for (const auto& synonym : synonyms) {
        // Convert to lowercase for case-insensitive matching
        std::string lowerSynonym = synonym;
        std::transform(lowerSynonym.begin(), lowerSynonym.end(), 
                      lowerSynonym.begin(), ::tolower);
        
        verbMap_[lowerSynonym] = verbId;
    }
}

void VerbRegistry::registerSyntax(VerbId verbId, SyntaxPattern pattern) {
    // Add the pattern to the verb's pattern list
    syntaxMap_[verbId].push_back(std::move(pattern));
}

std::optional<VerbId> VerbRegistry::lookupVerb(std::string_view word) const {
    // Convert to lowercase for case-insensitive lookup
    std::string lowerWord(word);
    std::transform(lowerWord.begin(), lowerWord.end(), 
                  lowerWord.begin(), ::tolower);
    
    // C++17 if with initializer
    if (auto it = verbMap_.find(lowerWord); it != verbMap_.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::vector<SyntaxPattern>& VerbRegistry::getSyntaxPatterns(VerbId verbId) const {
    // C++17 if with initializer
    if (auto it = syntaxMap_.find(verbId); it != syntaxMap_.end()) {
        return it->second;
    }
    return emptyPatterns_;
}

bool VerbRegistry::hasPatterns(VerbId verbId) const {
    // C++17 if with initializer
    if (auto it = syntaxMap_.find(verbId); it != syntaxMap_.end()) {
        return !it->second.empty();
    }
    return false;
}

std::vector<VerbId> VerbRegistry::getAllVerbs() const {
    std::vector<VerbId> verbs;
    verbs.reserve(syntaxMap_.size());
    
    for (const auto& [verbId, patterns] : syntaxMap_) {
        verbs.push_back(verbId);
    }
    
    // Sort for consistent ordering
    std::sort(verbs.begin(), verbs.end());
    
    return verbs;
}

bool VerbRegistry::isPrepositionValidForVerb(VerbId verbId, const std::string& preposition) const {
    const auto& patterns = getSyntaxPatterns(verbId);
    
    // Check each pattern to see if it contains this preposition
    for (const auto& pattern : patterns) {
        for (const auto& element : pattern.getPattern()) {
            if (element.type == SyntaxPattern::ElementType::PREPOSITION) {
                // Check if this preposition is in the element's values
                for (const auto& prep : element.values) {
                    if (prep == preposition) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

std::set<std::string> VerbRegistry::getValidPrepositions(VerbId verbId) const {
    std::set<std::string> validPreps;
    const auto& patterns = getSyntaxPatterns(verbId);
    
    // Collect all prepositions from all patterns
    for (const auto& pattern : patterns) {
        for (const auto& element : pattern.getPattern()) {
            if (element.type == SyntaxPattern::ElementType::PREPOSITION) {
                for (const auto& prep : element.values) {
                    validPreps.insert(prep);
                }
            }
        }
    }
    
    return validPreps;
}

std::optional<VerbId> VerbRegistry::getVerbIdForSyntax(VerbId baseVerb, const std::string& preposition) const {
    const auto& patterns = getSyntaxPatterns(baseVerb);
    
    for (const auto& pattern : patterns) {
        // Check pattern elements for this preposition
        for (const auto& element : pattern.getPattern()) {
            if (element.type == SyntaxPattern::ElementType::PREPOSITION) {
                for (const auto& prep : element.values) {
                    if (prep == preposition) {
                        return pattern.getVerbId();
                    }
                }
            }
        }
    }
    
    return std::nullopt;
}



void VerbRegistry::initializeSyntaxPatterns() {
    using ET = SyntaxPattern::ElementType;
    using Elem = SyntaxPattern::Element;
    
    // Game commands (no objects needed)
    registerSyntax(V_VERBOSE, SyntaxPattern(V_VERBOSE, {Elem(ET::VERB)}));
    registerSyntax(V_BRIEF, SyntaxPattern(V_BRIEF, {Elem(ET::VERB)}));
    registerSyntax(V_SUPERBRIEF, SyntaxPattern(V_SUPERBRIEF, {Elem(ET::VERB)}));
    registerSyntax(V_DIAGNOSE, SyntaxPattern(V_DIAGNOSE, {Elem(ET::VERB)}));
    registerSyntax(V_INVENTORY, SyntaxPattern(V_INVENTORY, {Elem(ET::VERB)}));
    registerSyntax(V_QUIT, SyntaxPattern(V_QUIT, {Elem(ET::VERB)}));
    registerSyntax(V_RESTART, SyntaxPattern(V_RESTART, {Elem(ET::VERB)}));
    registerSyntax(V_RESTORE, SyntaxPattern(V_RESTORE, {Elem(ET::VERB)}));
    registerSyntax(V_SAVE, SyntaxPattern(V_SAVE, {Elem(ET::VERB)}));
    registerSyntax(V_SCORE, SyntaxPattern(V_SCORE, {Elem(ET::VERB)}));
    registerSyntax(V_VERSION, SyntaxPattern(V_VERSION, {Elem(ET::VERB)}));
    
    // TAKE verb patterns
    // TAKE OBJECT (FIND TAKEBIT) (ON-GROUND IN-ROOM MANY)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::TAKEBIT);
        registerSyntax(V_TAKE, SyntaxPattern(V_TAKE, {Elem(ET::VERB), objElem}));
    }
    
    // TAKE OBJECT FROM OBJECT
    {
        Elem obj1(ET::OBJECT, ObjectFlag::TAKEBIT);
        Elem prep(ET::PREPOSITION, {"from", "out", "off"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_TAKE, SyntaxPattern(V_TAKE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // DROP verb patterns
    // DROP OBJECT (HELD MANY HAVE)
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_DROP, SyntaxPattern(V_DROP, {Elem(ET::VERB), objElem}));
    }
    
    // PUT verb patterns
    // PUT OBJECT IN OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"in", "inside", "into"});
        Elem obj2(ET::OBJECT, ObjectFlag::CONTBIT);
        registerSyntax(V_PUT, SyntaxPattern(V_PUT, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // PUT OBJECT ON OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"on", "onto"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_PUT, SyntaxPattern(V_PUT_ON, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // EXAMINE verb patterns
    // EXAMINE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_EXAMINE, SyntaxPattern(V_EXAMINE, {Elem(ET::VERB), objElem}));
    }
    
    // LOOK AT OBJECT
    {
        Elem prep(ET::PREPOSITION, {"at"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_EXAMINE, SyntaxPattern(V_EXAMINE, {Elem(ET::VERB), prep, objElem}));
    }
    
    // LOOK verb patterns
    // LOOK (no object)
    registerSyntax(V_LOOK, SyntaxPattern(V_LOOK, {Elem(ET::VERB)}));
    
    // LOOK IN/INSIDE OBJECT
    {
        Elem prep(ET::PREPOSITION, {"in", "inside"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_LOOK_INSIDE, SyntaxPattern(V_LOOK_INSIDE, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_LOOK, SyntaxPattern(V_LOOK_INSIDE, {Elem(ET::VERB), prep, objElem})); // Look In support
    }
    
    // READ verb patterns
    // READ OBJECT (FIND READBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::READBIT);
        registerSyntax(V_READ, SyntaxPattern(V_READ, {Elem(ET::VERB), objElem}));
    }
    
    // OPEN verb patterns
    // OPEN OBJECT (FIND DOORBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::CONTBIT);
        registerSyntax(V_OPEN, SyntaxPattern(V_OPEN, {Elem(ET::VERB), objElem}));
    }
    
    // OPEN OBJECT WITH OBJECT (for locked doors)
    {
        Elem obj1(ET::OBJECT, ObjectFlag::CONTBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_OPEN, SyntaxPattern(V_OPEN, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // CLOSE verb patterns
    // CLOSE OBJECT (FIND DOORBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::CONTBIT);
        registerSyntax(V_CLOSE, SyntaxPattern(V_CLOSE, {Elem(ET::VERB), objElem}));
    }
    
    // LOCK verb patterns
    // LOCK OBJECT WITH OBJECT (FIND TOOLBIT)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_LOCK, SyntaxPattern(V_LOCK, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // UNLOCK verb patterns
    // UNLOCK OBJECT WITH OBJECT (FIND TOOLBIT)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_UNLOCK, SyntaxPattern(V_UNLOCK, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // ATTACK verb patterns
    // ATTACK OBJECT (FIND ACTORBIT) WITH OBJECT (FIND WEAPONBIT)
    {
        Elem obj1(ET::OBJECT, ObjectFlag::ACTORBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::WEAPONBIT);
        registerSyntax(V_ATTACK, SyntaxPattern(V_ATTACK, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // ATTACK OBJECT (bare-handed)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_ATTACK, SyntaxPattern(V_ATTACK, {Elem(ET::VERB), objElem}));
    }
    
    // KILL verb patterns (same as ATTACK)
    {
        Elem obj1(ET::OBJECT, ObjectFlag::ACTORBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::WEAPONBIT);
        registerSyntax(V_KILL, SyntaxPattern(V_KILL, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    {
        Elem objElem(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_KILL, SyntaxPattern(V_KILL, {Elem(ET::VERB), objElem}));
    }
    
    // THROW verb patterns
    // THROW OBJECT AT OBJECT (FIND ACTORBIT)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"at"});
        Elem obj2(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_THROW, SyntaxPattern(V_THROW, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // THROW OBJECT IN OBJECT (put in container)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"in", "into"});
        Elem obj2(ET::OBJECT, ObjectFlag::CONTBIT);
        registerSyntax(V_THROW, SyntaxPattern(V_THROW, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // LAMP-ON verb patterns
    // LIGHT OBJECT (FIND LIGHTBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::LIGHTBIT);
        registerSyntax(V_LAMP_ON, SyntaxPattern(V_LAMP_ON, {Elem(ET::VERB), objElem}));
    }
    
    // TURN ON OBJECT
    {
        Elem prep(ET::PREPOSITION, {"on"});
        Elem objElem(ET::OBJECT, ObjectFlag::LIGHTBIT);
        registerSyntax(V_LAMP_ON, SyntaxPattern(V_LAMP_ON, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_TURN, SyntaxPattern(V_LAMP_ON, {Elem(ET::VERB), prep, objElem})); // Turn On support
    }
    
    // LAMP-OFF verb patterns
    // EXTINGUISH OBJECT (FIND ONBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::ONBIT);
        registerSyntax(V_LAMP_OFF, SyntaxPattern(V_LAMP_OFF, {Elem(ET::VERB), objElem}));
    }
    
    // TURN OFF OBJECT
    {
        Elem prep(ET::PREPOSITION, {"off"});
        Elem objElem(ET::OBJECT, ObjectFlag::ONBIT);
        registerSyntax(V_LAMP_OFF, SyntaxPattern(V_LAMP_OFF, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_TURN, SyntaxPattern(V_LAMP_OFF, {Elem(ET::VERB), prep, objElem})); // Turn Off support
    }
    
    // TURN verb patterns
    // TURN OBJECT (FIND TURNBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::TURNBIT);
        registerSyntax(V_TURN, SyntaxPattern(V_TURN, {Elem(ET::VERB), objElem}));
    }
    
    // TURN OBJECT WITH OBJECT
    {
        Elem obj1(ET::OBJECT, ObjectFlag::TURNBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_TURN, SyntaxPattern(V_TURN, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // PUSH verb patterns
    // PUSH OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_PUSH, SyntaxPattern(V_PUSH, {Elem(ET::VERB), objElem}));
    }
    
    // PULL verb patterns
    // PULL OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_PULL, SyntaxPattern(V_PULL, {Elem(ET::VERB), objElem}));
    }
    
    // MOVE verb patterns
    // MOVE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_MOVE, SyntaxPattern(V_MOVE, {Elem(ET::VERB), objElem}));
    }
    
    // TIE verb patterns
    // TIE OBJECT TO OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"to"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_TIE, SyntaxPattern(V_TIE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // UNTIE verb patterns
    // UNTIE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_UNTIE, SyntaxPattern(V_UNTIE, {Elem(ET::VERB), objElem}));
    }
    
    // UNTIE OBJECT FROM OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"from"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_UNTIE, SyntaxPattern(V_UNTIE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // LISTEN verb patterns
    // LISTEN (no object)
    registerSyntax(V_LISTEN, SyntaxPattern(V_LISTEN, {Elem(ET::VERB)}));
    
    // LISTEN TO OBJECT
    {
        Elem prep(ET::PREPOSITION, {"to", "for"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_LISTEN, SyntaxPattern(V_LISTEN, {Elem(ET::VERB), prep, objElem}));
    }
    
    // YELL verb patterns
    // YELL (no object)
    registerSyntax(V_YELL, SyntaxPattern(V_YELL, {Elem(ET::VERB)}));
    
    // SMELL verb patterns
    // SMELL (no object)
    registerSyntax(V_SMELL, SyntaxPattern(V_SMELL, {Elem(ET::VERB)}));
    
    // SMELL OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_SMELL, SyntaxPattern(V_SMELL, {Elem(ET::VERB), objElem}));
    }
    
    // TOUCH/RUB verb patterns
    // RUB OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_RUB, SyntaxPattern(V_RUB, {Elem(ET::VERB), objElem}));
    }
    
    // RUB OBJECT WITH OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_RUB, SyntaxPattern(V_RUB, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // EAT verb patterns
    // EAT OBJECT (FIND FOODBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::FOODBIT);
        registerSyntax(V_EAT, SyntaxPattern(V_EAT, {Elem(ET::VERB), objElem}));
    }
    
    // DRINK verb patterns
    // DRINK OBJECT (FIND DRINKBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::DRINKBIT);
        registerSyntax(V_DRINK, SyntaxPattern(V_DRINK, {Elem(ET::VERB), objElem}));
    }
    
    // DRINK FROM OBJECT
    {
        Elem prep(ET::PREPOSITION, {"from"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_DRINK, SyntaxPattern(V_DRINK, {Elem(ET::VERB), prep, objElem}));
    }
    
    // INFLATE verb patterns
    // INFLATE OBJECT WITH OBJECT (FIND TOOLBIT)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_INFLATE, SyntaxPattern(V_INFLATE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // DEFLATE verb patterns
    // DEFLATE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_DEFLATE, SyntaxPattern(V_DEFLATE, {Elem(ET::VERB), objElem}));
    }
    
    // PRAY verb patterns
    // PRAY (no object)
    registerSyntax(V_PRAY, SyntaxPattern(V_PRAY, {Elem(ET::VERB)}));
    
    // EXORCISE verb patterns
    // EXORCISE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_EXORCISE, SyntaxPattern(V_EXORCISE, {Elem(ET::VERB), objElem}));
    }
    
    // WAVE verb patterns
    // WAVE OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_WAVE, SyntaxPattern(V_WAVE, {Elem(ET::VERB), objElem}));
    }
    
    // WAVE OBJECT AT OBJECT
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"at"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_WAVE, SyntaxPattern(V_WAVE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // RING verb patterns
    // RING OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_RING, SyntaxPattern(V_RING, {Elem(ET::VERB), objElem}));
    }
    
    // BURN verb patterns
    // BURN OBJECT WITH OBJECT (FIND FLAMEBIT)
    {
        Elem obj1(ET::OBJECT, ObjectFlag::BURNBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::FLAMEBIT);
        registerSyntax(V_BURN, SyntaxPattern(V_BURN, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // DIG verb patterns
    // DIG IN OBJECT
    {
        Elem prep(ET::PREPOSITION, {"in"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_DIG, SyntaxPattern(V_DIG, {Elem(ET::VERB), prep, objElem}));
    }
    
    // DIG IN OBJECT WITH OBJECT (FIND TOOLBIT)
    {
        Elem prep1(ET::PREPOSITION, {"in"});
        Elem obj1(ET::OBJECT);
        Elem prep2(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT, ObjectFlag::TOOLBIT);
        registerSyntax(V_DIG, SyntaxPattern(V_DIG, {Elem(ET::VERB), prep1, obj1, prep2, obj2}));
    }
    
    // FILL verb patterns
    // FILL OBJECT (FIND CONTBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::CONTBIT);
        registerSyntax(V_FILL, SyntaxPattern(V_FILL, {Elem(ET::VERB), objElem}));
    }
    
    // FILL OBJECT WITH OBJECT
    {
        Elem obj1(ET::OBJECT, ObjectFlag::CONTBIT);
        Elem prep(ET::PREPOSITION, {"with"});
        Elem obj2(ET::OBJECT);
        registerSyntax(V_FILL, SyntaxPattern(V_FILL, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // GIVE verb patterns
    // GIVE OBJECT TO OBJECT (FIND ACTORBIT)
    {
        Elem obj1(ET::OBJECT);
        Elem prep(ET::PREPOSITION, {"to"});
        Elem obj2(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_GIVE, SyntaxPattern(V_GIVE, {Elem(ET::VERB), obj1, prep, obj2}));
    }
    
    // SEARCH verb patterns
    // SEARCH OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_SEARCH, SyntaxPattern(V_SEARCH, {Elem(ET::VERB), objElem}));
    }
    
    // SEARCH IN OBJECT
    {
        Elem prep(ET::PREPOSITION, {"in"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_SEARCH, SyntaxPattern(V_SEARCH, {Elem(ET::VERB), prep, objElem}));
    }
    
    // Movement verbs
    // ENTER (no object)
    registerSyntax(V_ENTER, SyntaxPattern(V_ENTER, {Elem(ET::VERB)}));
    
    // ENTER OBJECT
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_ENTER, SyntaxPattern(V_ENTER, {Elem(ET::VERB), objElem}));
    }
    
    // GO IN / GO INSIDE / GO INTO -> ENTER (ZIL compatibility)
    {
        Elem goVerb(ET::VERB);  // matches "go"
        Elem inPrep(ET::PREPOSITION, {"in", "inside", "into"});
        registerSyntax(V_ENTER, SyntaxPattern(V_ENTER, {goVerb, inPrep}));
        registerSyntax(V_WALK, SyntaxPattern(V_ENTER, {goVerb, inPrep})); // Go In support
        registerSyntax(V_TAKE, SyntaxPattern(V_ENTER, {goVerb, inPrep})); // Get In support
    }
    
    // GO IN OBJECT -> ENTER OBJECT
    {
        Elem goVerb(ET::VERB);
        Elem inPrep(ET::PREPOSITION, {"in", "inside", "into"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_ENTER, SyntaxPattern(V_ENTER, {goVerb, inPrep, objElem}));
        registerSyntax(V_WALK, SyntaxPattern(V_ENTER, {goVerb, inPrep, objElem})); // Go In Object support
        registerSyntax(V_TAKE, SyntaxPattern(V_ENTER, {goVerb, inPrep, objElem})); // Get In Object support
    }
    
    // EXIT (no object)
    registerSyntax(V_EXIT, SyntaxPattern(V_EXIT, {Elem(ET::VERB)}));
    
    // GO OUT / GO OUTSIDE -> EXIT (ZIL compatibility)
    {
        Elem goVerb(ET::VERB);  // matches "go"
        Elem outPrep(ET::PREPOSITION, {"out", "outside"});
        registerSyntax(V_EXIT, SyntaxPattern(V_EXIT, {goVerb, outPrep}));
        registerSyntax(V_WALK, SyntaxPattern(V_EXIT, {goVerb, outPrep})); // Go Out support
        registerSyntax(V_TAKE, SyntaxPattern(V_EXIT, {goVerb, outPrep})); // Get Out support
    }
    
    // CLIMB UP OBJECT
    {
        Elem prep(ET::PREPOSITION, {"up"});
        Elem objElem(ET::OBJECT, ObjectFlag::CLIMBBIT);
        registerSyntax(V_CLIMB_UP, SyntaxPattern(V_CLIMB_UP, {Elem(ET::VERB), prep, objElem}));
    }
    
    // CLIMB DOWN OBJECT
    {
        Elem prep(ET::PREPOSITION, {"down"});
        Elem objElem(ET::OBJECT, ObjectFlag::CLIMBBIT);
        registerSyntax(V_CLIMB_DOWN, SyntaxPattern(V_CLIMB_DOWN, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_CLIMB_UP, SyntaxPattern(V_CLIMB_DOWN, {Elem(ET::VERB), prep, objElem})); // Climb Down support
    }
    
    // CLIMB ON OBJECT
    {
        Elem prep(ET::PREPOSITION, {"on"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_CLIMB_ON, SyntaxPattern(V_CLIMB_ON, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_CLIMB_UP, SyntaxPattern(V_CLIMB_ON, {Elem(ET::VERB), prep, objElem})); // Climb On support
    }
    
    // BOARD OBJECT (FIND VEHBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::VEHBIT);
        registerSyntax(V_BOARD, SyntaxPattern(V_BOARD, {Elem(ET::VERB), objElem}));
    }
    
    // DISEMBARK OBJECT
    {
        Elem objElem(ET::OBJECT, ObjectFlag::VEHBIT);
        registerSyntax(V_DISEMBARK, SyntaxPattern(V_DISEMBARK, {Elem(ET::VERB), objElem}));
    }
    
    // SWING verb patterns
    // SWING OBJECT (FIND WEAPONBIT)
    {
        Elem objElem(ET::OBJECT, ObjectFlag::WEAPONBIT);
        registerSyntax(V_SWING, SyntaxPattern(V_SWING, {Elem(ET::VERB), objElem}));
    }
    
    // SWING OBJECT AT OBJECT (FIND ACTORBIT)
    {
        Elem obj1(ET::OBJECT, ObjectFlag::WEAPONBIT);
        Elem prep(ET::PREPOSITION, {"at"});
        Elem obj2(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_SWING, SyntaxPattern(V_SWING, {Elem(ET::VERB), obj1, prep, obj2}));
    }

    // ZIL Audit Syntax Patterns
    
    // DESTROY / MUNG
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_MUNG, SyntaxPattern(V_MUNG, {Elem(ET::VERB), objElem}));
    }
    
    // WEAR
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_WEAR, SyntaxPattern(V_WEAR, {Elem(ET::VERB), objElem}));
    }
    
    // FIND
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_FIND, SyntaxPattern(V_FIND, {Elem(ET::VERB), objElem}));
    }
    
    // LEAP (no object)
    registerSyntax(V_LEAP, SyntaxPattern(V_LEAP, {Elem(ET::VERB)}));
    
    // SAY (no object - special handling usually, assumes text follows? Parser handles quotes separately?)
    // For now, SAY -> V_SAY
    registerSyntax(V_SAY, SyntaxPattern(V_SAY, {Elem(ET::VERB)}));
    
    // KICK
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_KICK, SyntaxPattern(V_KICK, {Elem(ET::VERB), objElem}));
    }
    
    // BREATHE / BLOW IN
    {
        Elem blow(ET::VERB); // blow
        Elem in(ET::PREPOSITION, {"in"});
        Elem obj(ET::OBJECT);
        registerSyntax(V_BREATHE, SyntaxPattern(V_BREATHE, {blow, in, obj}));
    }
    
    // RAPE
    {
        Elem objElem(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_RAPE, SyntaxPattern(V_RAPE, {Elem(ET::VERB), objElem}));
    }

    // Movement Batch Syntaxes
    
    // CLIMB OBJECT -> V_CLIMB_FOO
    {
        Elem objElem(ET::OBJECT, ObjectFlag::CLIMBBIT);
        registerSyntax(V_CLIMB_UP, SyntaxPattern(V_CLIMB_FOO, {Elem(ET::VERB), objElem}));
        registerSyntax(V_CLIMB_DOWN, SyntaxPattern(V_CLIMB_FOO, {Elem(ET::VERB), objElem})); // Map synonyms
        registerSyntax(V_CLIMB_ON, SyntaxPattern(V_CLIMB_FOO, {Elem(ET::VERB), objElem}));   // Map synonyms
    }
    
    // CLIMB WITH/THROUGH -> V_THROUGH
    {
        Elem prep(ET::PREPOSITION, {"with", "through", "thru"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_CLIMB_UP, SyntaxPattern(V_THROUGH, {Elem(ET::VERB), prep, objElem}));
    }
    
    // ENTER OBJECT -> V_THROUGH
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_ENTER, SyntaxPattern(V_THROUGH, {Elem(ET::VERB), objElem}));
    }
    
    // STAND [UP] / STAND ON OBJECT (V-STAND not ideal for ON, but ZIL says STAND UP OBJ = V-STAND)
    // ZIL: STAND = V-STAND. STAND UP OBJ = V-STAND.
    registerSyntax(V_STAND, SyntaxPattern(V_STAND, {Elem(ET::VERB)}));
    {
        Elem prep(ET::PREPOSITION, {"up"});
        Elem objElem(ET::OBJECT); // FIND RMUNGBIT in ZIL?
        registerSyntax(V_STAND, SyntaxPattern(V_STAND, {Elem(ET::VERB), prep, objElem}));
        registerSyntax(V_STAND, SyntaxPattern(V_STAND, {Elem(ET::VERB), prep})); // STAND UP
    }
    
    // ALARM / WAKE
    {
        Elem objElem(ET::OBJECT, ObjectFlag::ACTORBIT);
        registerSyntax(V_ALARM, SyntaxPattern(V_ALARM, {Elem(ET::VERB), objElem}));
        
        Elem prep(ET::PREPOSITION, {"up"});
        registerSyntax(V_ALARM, SyntaxPattern(V_ALARM, {Elem(ET::VERB), prep, objElem})); // WAKE UP OBJ
    }
    
    // WALK AROUND
    {
        Elem prep(ET::PREPOSITION, {"around"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_WALK, SyntaxPattern(V_WALK_AROUND, {Elem(ET::VERB), prep, objElem}));
    }
    
    // WALK TO
    {
        Elem prep(ET::PREPOSITION, {"to"});
        Elem objElem(ET::OBJECT);
        registerSyntax(V_WALK, SyntaxPattern(V_WALK_TO, {Elem(ET::VERB), prep, objElem}));
    }
    
    // WALK THROUGH/WITH/ON -> V_THROUGH
    {
        Elem prep(ET::PREPOSITION, {"through", "thru", "with", "in", "on"}); // ZIL: WALK IN/WITH/ON = V-THROUGH
        Elem objElem(ET::OBJECT);
        registerSyntax(V_WALK, SyntaxPattern(V_THROUGH, {Elem(ET::VERB), prep, objElem}));
    }
    
    // LAUNCH
    {
        Elem objElem(ET::OBJECT, ObjectFlag::VEHBIT);
        registerSyntax(V_LAUNCH, SyntaxPattern(V_LAUNCH, {Elem(ET::VERB), objElem}));
    }

    // Manipulation Batch Syntaxes (Phase 10.3)
    
    // CUT
    {
        Elem objElem(ET::OBJECT);
        Elem withPrep(ET::PREPOSITION, {"with"});
        Elem weaponElem(ET::OBJECT, ObjectFlag::WEAPONBIT);
        Elem toolElem(ET::OBJECT, ObjectFlag::TOOLBIT);
        
        registerSyntax(V_CUT, SyntaxPattern(V_CUT, {Elem(ET::VERB), objElem}));
        registerSyntax(V_CUT, SyntaxPattern(V_CUT, {Elem(ET::VERB), objElem, withPrep, weaponElem}));
        registerSyntax(V_CUT, SyntaxPattern(V_CUT, {Elem(ET::VERB), objElem, withPrep, toolElem}));
    }
    
    // SHAKE/LOWER/RAISE/SPIN/SQUEEZE/WIND/PLAY
    {
        Elem objElem(ET::OBJECT);
        registerSyntax(V_SHAKE, SyntaxPattern(V_SHAKE, {Elem(ET::VERB), objElem}));
        registerSyntax(V_LOWER, SyntaxPattern(V_LOWER, {Elem(ET::VERB), objElem}));
        registerSyntax(V_RAISE, SyntaxPattern(V_RAISE, {Elem(ET::VERB), objElem}));
        registerSyntax(V_SPIN, SyntaxPattern(V_SPIN, {Elem(ET::VERB), objElem}));
        registerSyntax(V_SQUEEZE, SyntaxPattern(V_SQUEEZE, {Elem(ET::VERB), objElem}));
        registerSyntax(V_WIND, SyntaxPattern(V_WIND, {Elem(ET::VERB), objElem}));
        registerSyntax(V_PLAY, SyntaxPattern(V_PLAY, {Elem(ET::VERB), objElem}));
        registerSyntax(V_MAKE, SyntaxPattern(V_MAKE, {Elem(ET::VERB), objElem})); // Make obj?
        
        // PLUG WITH
        Elem withPrep(ET::PREPOSITION, {"with"});
        Elem tool(ET::OBJECT);
        registerSyntax(V_PLUG, SyntaxPattern(V_PLUG, {Elem(ET::VERB), objElem, withPrep, tool}));
        // ZIL says PLUG can be bare? "PLUG HOLE"
        registerSyntax(V_PLUG, SyntaxPattern(V_PLUG, {Elem(ET::VERB), objElem}));
    }
    
    // MELT
    {
        Elem objElem(ET::OBJECT);
        Elem withPrep(ET::PREPOSITION, {"with"});
        Elem tool(ET::OBJECT);
        registerSyntax(V_MELT, SyntaxPattern(V_MELT, {Elem(ET::VERB), objElem, withPrep, tool}));
    }
    
    // POUR Handling
    {
        // POUR OBJ -> DROP
        Elem objElem(ET::OBJECT);
        registerSyntax(V_POUR_ON, SyntaxPattern(V_DROP, {Elem(ET::VERB), objElem}));
        
        // POUR OBJ IN OBJ -> PUT
        Elem inPrep(ET::PREPOSITION, {"in", "into"});
        Elem container(ET::OBJECT);
        registerSyntax(V_POUR_ON, SyntaxPattern(V_PUT, {Elem(ET::VERB), objElem, inPrep, container}));
        
        // POUR OBJ ON OBJ -> POUR_ON
        Elem onPrep(ET::PREPOSITION, {"on"});
        Elem surface(ET::OBJECT);
        registerSyntax(V_POUR_ON, SyntaxPattern(V_POUR_ON, {Elem(ET::VERB), objElem, onPrep, surface}));
    }
    
    // PUSH / MOVE Handling
    {
        Elem objElem(ET::OBJECT);
        
        // PUSH OBJ TO OBJ
        Elem toPrep(ET::PREPOSITION, {"to"});
        Elem dest(ET::OBJECT);
        registerSyntax(V_PUSH, SyntaxPattern(V_PUSH_TO, {Elem(ET::VERB), objElem, toPrep, dest}));
        // Also MOVE OBJ TO OBJ -> PUSH_TO (ZIL: MOVE ... TO = V-PUSH-TO)
        // Need to check if MOVE is alias for PUSH or separate verb?
        // Usually MOVE is synonym for PUSH in registry.
        
        // PUSH OBJ UNDER OBJ
        Elem underPrep(ET::PREPOSITION, {"under"});
        registerSyntax(V_PUSH, SyntaxPattern(V_PUT_UNDER, {Elem(ET::VERB), objElem, underPrep, dest}));
        
        // PUT OBJ UNDER OBJ
        registerSyntax(V_PUT, SyntaxPattern(V_PUT_UNDER, {Elem(ET::VERB), objElem, underPrep, dest}));
        
        // PUT OBJ BEHIND OBJ
        Elem behindPrep(ET::PREPOSITION, {"behind"});
        registerSyntax(V_PUT, SyntaxPattern(V_PUT_BEHIND, {Elem(ET::VERB), objElem, behindPrep, dest}));
    }
    
    // TIE UP
    {
        // TIE OBJ UP -> V_TIE_UP
        Elem objElem(ET::OBJECT);
        Elem upPrep(ET::PREPOSITION, {"up"});
        // Need to check if TIE verb is registered. Assuming V_TIE (90) exists.
        registerSyntax(VerbId(90), SyntaxPattern(V_TIE_UP, {Elem(ET::VERB), objElem, upPrep}));
    }
}
