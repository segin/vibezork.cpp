#include "parser.h"
#include "core/globals.h"
#include "verbs/verbs.h"
#include <algorithm>
#include <cctype>
#include <sstream>

Parser::Parser() {
    // Initialize verb synonyms (from gsyntax.zil)
    verbSynonyms_["take"] = V_TAKE;
    verbSynonyms_["get"] = V_TAKE;
    verbSynonyms_["drop"] = V_DROP;
    verbSynonyms_["look"] = V_LOOK;
    verbSynonyms_["l"] = V_LOOK;
    verbSynonyms_["examine"] = V_EXAMINE;
    verbSynonyms_["open"] = V_OPEN;
    verbSynonyms_["close"] = V_CLOSE;
    verbSynonyms_["inventory"] = V_INVENTORY;
    verbSynonyms_["i"] = V_INVENTORY;
    verbSynonyms_["go"] = V_WALK;
    
    // Directions
    directions_["north"] = Direction::NORTH;
    directions_["n"] = Direction::NORTH;
    directions_["south"] = Direction::SOUTH;
    directions_["s"] = Direction::SOUTH;
    directions_["east"] = Direction::EAST;
    directions_["e"] = Direction::EAST;
    directions_["west"] = Direction::WEST;
    directions_["w"] = Direction::WEST;
    directions_["ne"] = Direction::NE;
    directions_["northeast"] = Direction::NE;
    directions_["nw"] = Direction::NW;
    directions_["northwest"] = Direction::NW;
    directions_["se"] = Direction::SE;
    directions_["southeast"] = Direction::SE;
    directions_["sw"] = Direction::SW;
    directions_["southwest"] = Direction::SW;
    directions_["up"] = Direction::UP;
    directions_["u"] = Direction::UP;
    directions_["down"] = Direction::DOWN;
    directions_["d"] = Direction::DOWN;
    verbSynonyms_["quit"] = V_QUIT;
    verbSynonyms_["q"] = V_QUIT;
    
    // Prepositions
    prepositions_["with"] = "with";
    prepositions_["in"] = "in";
    prepositions_["on"] = "on";
    prepositions_["to"] = "to";
}

void Parser::tokenize(const std::string& input, std::vector<std::string>& tokens) {
    std::istringstream iss(input);
    std::string word;
    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        tokens.push_back(word);
    }
}

VerbId Parser::findVerb(const std::string& word) {
    auto it = verbSynonyms_.find(word);
    return it != verbSynonyms_.end() ? it->second : 0;
}

ZObject* Parser::findObject(const std::string& word) {
    // Simplified object lookup - would need full implementation
    return nullptr;
}

Direction* Parser::findDirection(const std::string& word) {
    auto it = directions_.find(word);
    return it != directions_.end() ? &it->second : nullptr;
}

ParsedCommand Parser::parse(const std::string& input) {
    ParsedCommand cmd;
    tokenize(input, cmd.words);
    
    if (cmd.words.empty()) {
        return cmd;
    }
    
    // Check if first word is a direction
    Direction* dir = findDirection(cmd.words[0]);
    if (dir) {
        cmd.isDirection = true;
        cmd.direction = *dir;
        cmd.verb = V_WALK;
    } else {
        cmd.verb = findVerb(cmd.words[0]);
    }
    
    return cmd;
}
