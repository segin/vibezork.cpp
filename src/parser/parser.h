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
    
private:
    void tokenize(const std::string& input, std::vector<std::string>& tokens);
    VerbId findVerb(const std::string& word);
    ZObject* findObject(const std::string& word);
    Direction* findDirection(const std::string& word);
    
    std::map<std::string, VerbId> verbSynonyms_;
    std::map<std::string, std::string> prepositions_;
    std::map<std::string, Direction> directions_;
};
