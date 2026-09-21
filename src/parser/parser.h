#pragma once
#include "core/types.h"
#include "world/rooms.h"
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Forward declaration
class Parser;

// Global parser accessor (defined in main.cpp)
Parser& getGlobalParser();

struct ParsedCommand {
    VerbId verb = 0;
    ZObject* directObj = nullptr;    // first entry of prsoTable, if any
    ZObject* indirectObj = nullptr;  // first entry of prsiTable, if any
    std::vector<std::string> words;
    Direction direction = Direction::NORTH;
    bool isDirection = false;

    // ZIL: P-PRSO / P-PRSI match tables (gparser.zil:964-965); the size is
    // P-MATCHLEN. The IT object (gglobals.zil:42-46) may appear in either
    // table and is substituted by MAIN-LOOP-1 / PERFORM (gmain.zil:45-64,
    // 194-203).
    std::vector<ZObject*> prsoTable;
    std::vector<ZObject*> prsiTable;
    // ZIL: ,P-GETFLAGS after SNARFEM: 0, P-ALL, P-ONE or P-INHIBIT
    // (gparser.zil:970-973).
    int getFlags = 0;
    // ZIL: <EQUAL? <GET <GET ,P-ITBL ,P-NC1> 0> ,W?ALL> -- the first noun
    // clause began with the word ALL (gmain.zil:122, 129).
    bool nc1IsAll = false;
    // ZIL: <BAND <GETB ,P-SYNTAX ,P-SBITS> ,P-SONUMS> -- number of objects
    // the matched syntax expects (gmain.zil:83); -1 when unknown.
    int objectsExpected = -1;

    // Legacy mirrors kept for existing tests: true / the P-PRSO table when
    // the first noun clause was ALL.
    bool isAll = false;
    std::vector<ZObject*> allObjects;
    ZObject* exceptObject = nullptr;
};

// C++23 std::expected error types
enum class ParseErrorCode {
    EMPTY_INPUT,
    UNKNOWN_WORD,
    SYNTAX_ERROR,
    AMBIGUOUS,
    OBJECT_NOT_FOUND
};

struct ParseError {
    ParseErrorCode code = ParseErrorCode::EMPTY_INPUT;
    std::string message;
};

using ParseExpected = std::expected<ParsedCommand, ParseError>;

// Forward declaration
class VerbRegistry;

class Parser {
public:
    Parser();
    Parser(VerbRegistry* registry);  // Constructor with registry
    
    ParsedCommand parse(const std::string& input);
    ParseExpected tryParse(std::string_view input);
    
    // Special command support
    void setLastCommand(const std::string& cmd);
    std::string_view getLastCommand() const;
    void setLastUnknownWord(const std::string& word);
    std::string_view getLastUnknownWord() const;
    void clearLastUnknownWord();
    
    // Orphan (incomplete command) support
    void setOrphanDirect(VerbId verb, const std::string& verbWord);
    void setOrphanIndirect(VerbId verb, ZObject* directObj, const std::string& prep);
    void clearOrphan();
    bool isOrphaned() const { return orphanFlag_; }
    
    // Public for testing
    std::vector<ZObject*> findObjects(const std::vector<std::string>& words, size_t startIdx = 0);
    ZObject* disambiguate(const std::vector<ZObject*>& candidates, const std::string& noun);
    bool isPreposition(const std::string& word) const;
    std::optional<size_t> findPrepositionIndex(const std::vector<std::string>& tokens) const;
    
private:
    void initializeVerbsAndDirections();
    void finishTables(ParsedCommand& cmd);
    bool resolveObjectlessSyntax(ParsedCommand& cmd, const std::string& trailingPrep);

    // Interim stand-ins for SNARFEM / GET-OBJECT (gparser.zil:978-1140)
    // until Phase B ports the parser: resolve one noun clause (which may be
    // an AND/comma list, ALL, ALL EXCEPT ..., or a pronoun) into a match
    // table. Returns false when the parser must give up (message printed).
    bool snarfPhrase(ParsedCommand& cmd, const std::vector<std::string>& phrase,
                     bool directSlot, std::vector<ZObject*>& table);
    // ZIL: GET-OBJECT with P-GETFLAGS = P-ALL: DO-SL over HERE and the
    // player with the syntax's search bits, then GLOBAL-CHECK.
    bool collectAll(ParsedCommand& cmd, bool directSlot, std::vector<ZObject*>& table);
    int syntaxScopeBits(const ParsedCommand& cmd, bool directSlot) const;
    void searchList(ZObject* obj, std::vector<ZObject*>& table, int level) const;
    void doSl(ZObject* obj, int bit1, int bit2, std::vector<ZObject*>& table) const;
    void globalCheck(const ParsedCommand& cmd, std::vector<ZObject*>& table) const;
    mutable int pSlocbits_ = 0;  // ZIL: ,P-SLOCBITS during a GET-OBJECT
    void tokenize(const std::string& input, std::vector<std::string>& tokens);
    VerbId findVerb(const std::string& word) const;
    ZObject* findObject(const std::string& word);
    Direction* findDirection(const std::string& word);
    
    // Helper methods for object matching
    bool matchesSynonym(ZObject* obj, const std::string& word) const;
    bool matchesAdjectives(ZObject* obj, const std::vector<std::string>& adjectives) const;
    int getLocationPriority(ZObject* obj) const;
    bool isObjectVisible(ZObject* obj) const;
    
    // Disambiguation helpers
    std::string formatObjectDescription(ZObject* obj) const;
    ZObject* parseDisambiguationResponse(const std::string& response, 
                                         const std::vector<ZObject*>& candidates);
    
    // Preposition handling
    bool validatePreposition(VerbId verb, const std::string& preposition) const;
    
    // Special command helpers
    bool isAllKeyword(const std::string& word) const;
    bool isExceptKeyword(const std::string& word) const;
    bool isAgainCommand(const std::vector<std::string>& tokens) const;
    bool isOopsCommand(const std::vector<std::string>& tokens) const;
    // ZIL: IT / THEM / HER / HIM are synonyms of the IT object
    // (gglobals.zil:42-46); findObjects yields that object for them.
    bool isPronoun(const std::string& word) const;
    bool isKnownObjectWord(const std::string& word) const;
    std::string replaceOopsWord(const std::string& original, const std::string& replacement);
    
    // Using unordered containers for O(1) lookup
    std::unordered_map<std::string, VerbId> verbSynonyms_;
    std::unordered_set<std::string> prepositions_;
    std::unordered_map<std::string, Direction> directions_;
    VerbRegistry* verbRegistry_;  // Optional registry for advanced validation
    
    // Special command state
    std::string lastCommand_;
    std::string lastUnknownWord_;
    bool hadUnknownWordLastTurn_ = false;
    
    // Orphan (incomplete command) state - for continuation parsing
    bool orphanFlag_ = false;           // P-OFLAG: true if last command was incomplete
    VerbId orphanVerb_ = 0;             // The verb from the incomplete command
    std::string orphanPreposition_;     // Preposition if any
    bool orphanNeedsDirect_ = true;     // True if missing direct object
    bool orphanNeedsIndirect_ = false;  // True if missing indirect object
    ZObject* orphanDirectObj_ = nullptr; // Direct object if already specified
};
