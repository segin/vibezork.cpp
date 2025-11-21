#pragma once
#include "core/types.h"
#include "world/rooms.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

struct ParsedCommand {
    VerbId verb = 0;
    ZObject* directObj = nullptr;
    ZObject* indirectObj = nullptr;
    std::vector<std::string> words;
    Direction direction = Direction::NORTH;
    bool isDirection = false;
};

// Forward declaration
class VerbRegistry;

class Parser {
public:
    Parser();
    Parser(VerbRegistry* registry);  // Constructor with registry
    
    ParsedCommand parse(const std::string& input);
    
    // Public for testing
    std::vector<ZObject*> findObjects(const std::vector<std::string>& words, size_t startIdx = 0);
    ZObject* disambiguate(const std::vector<ZObject*>& candidates, const std::string& noun);
    
private:
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
    bool isPreposition(const std::string& word) const;
    std::optional<size_t> findPrepositionIndex(const std::vector<std::string>& tokens) const;
    bool validatePreposition(VerbId verb, const std::string& preposition) const;
    
    std::map<std::string, VerbId> verbSynonyms_;
    std::set<std::string> prepositions_;
    std::map<std::string, Direction> directions_;
    VerbRegistry* verbRegistry_;  // Optional registry for advanced validation
};
