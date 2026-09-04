#pragma once

#include "core/types.h"
#include "core/flags.h"
#include "parser/gparser.h"
#include "verbs/verbs.h"

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <span>

class VerbRegistry;

/**
 * @brief Complete C++23 port of ZIL syntax declarations from zil/gsyntax.zil.
 * 
 * Contains:
 * - 20 Buzzwords (BUZZ)
 * - 4 Preposition synonym groups (WITH, IN, ON, UNDER)
 * - 10 Direction synonym groups (NORTH, SOUTH, EAST, WEST, UP, DOWN, NW, NE, SW, SE)
 * - 61 Verb synonym groups (SUPER, INVENTORY, QUIT, TAKE, etc.)
 * - 267 Full ZIL SYNTAX declarations with element definitions, object search scopes,
 *   find flags, requirements (HAVE, TAKE, MANY), actions, and pre-actions.
 */
namespace GSyntax {

// Object scope search bitmasks
constexpr uint8_t SH_HELD = 1 << 0;
constexpr uint8_t SH_CARRIED = 1 << 1;
constexpr uint8_t SH_ON_GROUND = 1 << 2;
constexpr uint8_t SH_IN_ROOM = 1 << 3;

struct SyntaxElement {
    enum class Type { VERB, OBJECT, PREPOSITION };
    Type type = Type::VERB;
    std::string value;                       // Word for verb or preposition
    std::vector<std::string> synonyms;       // Preposition synonyms (e.g. "inside", "into")
    std::optional<ObjectFlag> findFlag;      // (FIND ...) flag
    uint8_t scopeFlags = 0;                  // Bitwise OR of SH_HELD, SH_CARRIED, SH_ON_GROUND, SH_IN_ROOM
    bool have = false;                       // (HAVE) - must possess object
    bool take = false;                       // (TAKE) - try to take object
    bool many = false;                       // (MANY) - allows multiple objects
};

struct ZilSyntax {
    std::string zilSource;                   // e.g. "zil/gsyntax.zil:40"
    std::string patternString;               // Full ZIL syntax string
    std::string verb;                        // Initial verb keyword
    VerbId verbId = 0;                       // Canonical VerbId
    std::string actionName;                  // Action routine name (e.g. "V-ATTACK")
    VerbId actionId = 0;                     // Action VerbId
    std::string preactionName;               // Preaction routine name (or empty)
    std::optional<VerbId> preactionId;       // Preaction VerbId (if any)
    std::vector<SyntaxElement> elements;     // Parsed pattern elements
};

// Initialization and queries
void initialize();

// Buzzwords (BUZZ)
bool isBuzzWord(std::string_view word);
const std::vector<std::string>& getBuzzWords();

// Prepositions
bool isPreposition(std::string_view word);
std::string canonicalPreposition(std::string_view word);
const std::vector<std::string>& getPrepositionSynonyms(std::string_view canonicalPrep);

// Directions
bool isDirection(std::string_view word);
std::string canonicalDirection(std::string_view word);
const std::vector<std::string>& getDirectionSynonyms(std::string_view canonicalDir);

// Verb Synonyms
std::optional<VerbId> lookupVerb(std::string_view word);
std::string canonicalVerb(std::string_view word);
const std::vector<std::string>& getVerbSynonyms(VerbId verbId);

// Syntaxes
const std::vector<ZilSyntax>& getAllSyntaxes();
std::vector<const ZilSyntax*> getSyntaxesForVerb(VerbId verbId);
std::vector<const ZilSyntax*> getSyntaxesForVerb(std::string_view verbWord);
const ZilSyntax* matchSyntax(std::string_view verbWord,
                             const std::vector<std::string>& prepositions,
                             size_t objectCount);

// Populate VerbRegistry
void populateVerbRegistry(VerbRegistry& registry);

} // namespace GSyntax
