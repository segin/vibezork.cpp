#pragma once
#include "core/types.h"
#include "world/rooms.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

// Forward declaration
class Parser;

// Global parser accessor (defined in main.cpp)
Parser& getGlobalParser();

struct ParsedCommand {
    VerbId verb = 0;
    ZObject* directObj = nullptr;
    ZObject* indirectObj = nullptr;
    std::vector<std::string> words;
    Direction direction = Direction::NORTH;
    bool isDirection = false;
    bool isAll = false;  // "all" keyword used
    std::vector<ZObject*> allObjects;  // Objects for "all" command
    ZObject* exceptObject = nullptr;  // Object to exclude in "all except"
};

// Forward declaration
class VerbRegistry;

class Parser {
public:
    Parser();
    Parser(VerbRegistry* registry);  // Constructor with registry
    
    ParsedCommand parse(const std::string& input);
    
    // Special command support
    void setLastCommand(const std::string& cmd);
    std::string_view getLastCommand() const;
    void setLastObject(ZObject* obj);
    ZObject* getLastObject() const;
    void setLastObjects(const std::vector<ZObject*>& objs);
    const std::vector<ZObject*>& getLastObjects() const;
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
    void tokenize(const std::string& input, std::vector<std::string>& tokens);
    VerbId findVerb(const std::string& word);
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
    bool isPronoun(const std::string& word) const;
    bool isKnownObjectWord(const std::string& word) const;
    std::vector<ZObject*> findAllApplicableObjects(VerbId verb) const;
    std::string replaceOopsWord(const std::string& original, const std::string& replacement);
    
    std::map<std::string, VerbId> verbSynonyms_;
    std::set<std::string> prepositions_;
    std::map<std::string, Direction> directions_;
    VerbRegistry* verbRegistry_;  // Optional registry for advanced validation
    
    // Special command state
    std::string lastCommand_;
    ZObject* lastObject_ = nullptr;
    std::vector<ZObject*> lastObjects_;
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
