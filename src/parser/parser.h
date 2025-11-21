#pragma once
#include "core/types.h"
#include "world/rooms.h"
#include <string>
#include <vector>
#include <map>

struct ParsedCommand {
    VerbId verb = 0;
    ZObject* directObj = nullptr;
    ZObject* indirectObj = nullptr;
    std::vector<std::string> words;
    Direction direction = Direction::NORTH;
    bool isDirection = false;
};

class Parser {
public:
    Parser();
    
    ParsedCommand parse(const std::string& input);
    
    // Public for testing
    std::vector<ZObject*> findObjects(const std::vector<std::string>& words, size_t startIdx = 0);
    
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
    
    std::map<std::string, VerbId> verbSynonyms_;
    std::map<std::string, std::string> prepositions_;
    std::map<std::string, Direction> directions_;
};
