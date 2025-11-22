#include "parser.h"
#include "verb_registry.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

Parser::Parser() : verbRegistry_(nullptr) {
    // Initialize verb synonyms (from gsyntax.zil)
    verbSynonyms_["take"] = V_TAKE;
    verbSynonyms_["get"] = V_TAKE;
    verbSynonyms_["drop"] = V_DROP;
    verbSynonyms_["put"] = V_PUT;
    verbSynonyms_["look"] = V_LOOK;
    verbSynonyms_["l"] = V_LOOK;
    verbSynonyms_["examine"] = V_EXAMINE;
    verbSynonyms_["read"] = V_READ;
    verbSynonyms_["open"] = V_OPEN;
    verbSynonyms_["close"] = V_CLOSE;
    verbSynonyms_["inventory"] = V_INVENTORY;
    verbSynonyms_["i"] = V_INVENTORY;
    verbSynonyms_["go"] = V_WALK;
    verbSynonyms_["attack"] = V_ATTACK;
    verbSynonyms_["kill"] = V_KILL;
    verbSynonyms_["quit"] = V_QUIT;
    verbSynonyms_["q"] = V_QUIT;
    
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
    
    // Prepositions (from gsyntax.zil)
    // Core prepositions
    prepositions_.insert("with");
    prepositions_.insert("using");
    prepositions_.insert("through");
    prepositions_.insert("thru");
    
    prepositions_.insert("in");
    prepositions_.insert("inside");
    prepositions_.insert("into");
    
    prepositions_.insert("on");
    prepositions_.insert("onto");
    
    prepositions_.insert("under");
    prepositions_.insert("underneath");
    prepositions_.insert("beneath");
    prepositions_.insert("below");
    
    // Additional prepositions used in syntax patterns
    prepositions_.insert("to");
    prepositions_.insert("at");
    prepositions_.insert("from");
    prepositions_.insert("for");
    prepositions_.insert("about");
    prepositions_.insert("off");
    prepositions_.insert("out");
    prepositions_.insert("over");
    prepositions_.insert("across");
    prepositions_.insert("behind");
    prepositions_.insert("around");
    prepositions_.insert("down");
    prepositions_.insert("up");
}

Parser::Parser(VerbRegistry* registry) : verbRegistry_(registry) {
    // Initialize verb synonyms (from gsyntax.zil)
    verbSynonyms_["take"] = V_TAKE;
    verbSynonyms_["get"] = V_TAKE;
    verbSynonyms_["drop"] = V_DROP;
    verbSynonyms_["put"] = V_PUT;
    verbSynonyms_["look"] = V_LOOK;
    verbSynonyms_["l"] = V_LOOK;
    verbSynonyms_["examine"] = V_EXAMINE;
    verbSynonyms_["read"] = V_READ;
    verbSynonyms_["open"] = V_OPEN;
    verbSynonyms_["close"] = V_CLOSE;
    verbSynonyms_["inventory"] = V_INVENTORY;
    verbSynonyms_["i"] = V_INVENTORY;
    verbSynonyms_["go"] = V_WALK;
    verbSynonyms_["attack"] = V_ATTACK;
    verbSynonyms_["kill"] = V_KILL;
    verbSynonyms_["quit"] = V_QUIT;
    verbSynonyms_["q"] = V_QUIT;
    
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
    
    // Prepositions (from gsyntax.zil)
    // Core prepositions
    prepositions_.insert("with");
    prepositions_.insert("using");
    prepositions_.insert("through");
    prepositions_.insert("thru");
    
    prepositions_.insert("in");
    prepositions_.insert("inside");
    prepositions_.insert("into");
    
    prepositions_.insert("on");
    prepositions_.insert("onto");
    
    prepositions_.insert("under");
    prepositions_.insert("underneath");
    prepositions_.insert("beneath");
    prepositions_.insert("below");
    
    // Additional prepositions used in syntax patterns
    prepositions_.insert("to");
    prepositions_.insert("at");
    prepositions_.insert("from");
    prepositions_.insert("for");
    prepositions_.insert("about");
    prepositions_.insert("off");
    prepositions_.insert("out");
    prepositions_.insert("over");
    prepositions_.insert("across");
    prepositions_.insert("behind");
    prepositions_.insert("around");
    prepositions_.insert("down");
    prepositions_.insert("up");
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

bool Parser::matchesSynonym(ZObject* obj, const std::string& word) const {
    // Check if the word matches any of the object's synonyms
    return obj->hasSynonym(word);
}

bool Parser::matchesAdjectives(ZObject* obj, const std::vector<std::string>& adjectives) const {
    // Check if all provided adjectives match the object
    for (const auto& adj : adjectives) {
        if (!obj->hasAdjective(adj)) {
            return false;
        }
    }
    return true;
}

int Parser::getLocationPriority(ZObject* obj) const {
    auto& g = Globals::instance();
    
    // Priority 1: Objects in current room
    if (obj->getLocation() == g.here) {
        return 3;
    }
    
    // Priority 2: Objects in player inventory
    if (obj->getLocation() == g.winner) {
        return 2;
    }
    
    // Priority 3: Objects in open containers in room or inventory
    ZObject* loc = obj->getLocation();
    if (loc && loc->hasFlag(ObjectFlag::CONTBIT) && loc->hasFlag(ObjectFlag::OPENBIT)) {
        if (loc->getLocation() == g.here || loc->getLocation() == g.winner) {
            return 1;
        }
    }
    
    // Priority 0: Objects elsewhere (not visible)
    return 0;
}

bool Parser::isObjectVisible(ZObject* obj) const {
    // Objects with priority > 0 are visible
    return getLocationPriority(obj) > 0;
}

std::vector<ZObject*> Parser::findObjects(const std::vector<std::string>& words, size_t startIdx) {
    std::vector<ZObject*> matches;
    
    if (startIdx >= words.size()) {
        return matches;
    }
    
    auto& g = Globals::instance();
    
    // Separate adjectives from nouns
    std::vector<std::string> adjectives;
    std::vector<std::string> nouns;
    
    for (size_t i = startIdx; i < words.size(); ++i) {
        const auto& word = words[i];
        
        // Skip articles and prepositions
        if (word == "the" || word == "a" || word == "an" || 
            prepositions_.find(word) != prepositions_.end()) {
            continue;
        }
        
        // For now, treat all words as potential nouns or adjectives
        // We'll check both during matching
        nouns.push_back(word);
    }
    
    if (nouns.empty()) {
        return matches;
    }
    
    // Search through all objects
    for (const auto& [id, objPtr] : g.getAllObjects()) {
        ZObject* obj = objPtr.get();
        
        // Skip invisible objects
        if (!isObjectVisible(obj)) {
            continue;
        }
        
        // Try to match the object
        bool matched = false;
        
        // Strategy 1: Try matching last word as noun with earlier words as adjectives
        if (nouns.size() > 1) {
            const std::string& noun = nouns.back();
            std::vector<std::string> potentialAdjectives(nouns.begin(), nouns.end() - 1);
            
            if (matchesSynonym(obj, noun) && matchesAdjectives(obj, potentialAdjectives)) {
                matched = true;
            }
        }
        
        // Strategy 2: Try matching any single word as a synonym
        if (!matched) {
            for (const auto& word : nouns) {
                if (matchesSynonym(obj, word)) {
                    // If we have multiple words, check if the others are adjectives
                    if (nouns.size() > 1) {
                        std::vector<std::string> otherWords;
                        for (const auto& w : nouns) {
                            if (w != word) {
                                otherWords.push_back(w);
                            }
                        }
                        // Only match if other words are adjectives or if object has no adjectives
                        if (matchesAdjectives(obj, otherWords) || obj->getAdjectives().empty()) {
                            matched = true;
                            break;
                        }
                    } else {
                        matched = true;
                        break;
                    }
                }
            }
        }
        
        if (matched) {
            matches.push_back(obj);
        }
    }
    
    // Sort by location priority (highest first)
    std::sort(matches.begin(), matches.end(), [this](ZObject* a, ZObject* b) {
        return getLocationPriority(a) > getLocationPriority(b);
    });
    
    return matches;
}

std::string Parser::formatObjectDescription(ZObject* obj) const {
    // Format object for disambiguation list
    // Show description and location for clarity
    std::string result = obj->getDesc();
    
    // Add location context if helpful
    auto& g = Globals::instance();
    ZObject* loc = obj->getLocation();
    
    if (loc == g.winner) {
        result += " (in your inventory)";
    } else if (loc && loc->hasFlag(ObjectFlag::CONTBIT)) {
        result += " (in the " + std::string(loc->getDesc()) + ")";
    } else if (loc == g.here) {
        result += " (here)";
    }
    
    return result;
}

ZObject* Parser::parseDisambiguationResponse(const std::string& response, 
                                             const std::vector<ZObject*>& candidates) {
    if (candidates.empty()) {
        return nullptr;
    }
    
    // Tokenize the response
    std::vector<std::string> tokens;
    tokenize(response, tokens);
    
    if (tokens.empty()) {
        return nullptr;
    }
    
    // Try to parse as a number (1-based index)
    try {
        int choice = std::stoi(tokens[0]);
        if (choice >= 1 && choice <= static_cast<int>(candidates.size())) {
            return candidates[choice - 1];
        }
    } catch (...) {
        // Not a number, try matching by name
    }
    
    // Try to match by object name/synonym
    // Look for objects that match the response words
    for (auto* candidate : candidates) {
        // Check if any word in response matches a synonym
        for (const auto& word : tokens) {
            if (matchesSynonym(candidate, word)) {
                return candidate;
            }
        }
        
        // Check if response matches adjectives + synonym
        if (tokens.size() > 1) {
            std::vector<std::string> adjectives(tokens.begin(), tokens.end() - 1);
            const std::string& noun = tokens.back();
            
            if (matchesSynonym(candidate, noun) && matchesAdjectives(candidate, adjectives)) {
                return candidate;
            }
        }
    }
    
    return nullptr;
}

ZObject* Parser::disambiguate(const std::vector<ZObject*>& candidates, const std::string& noun) {
    if (candidates.empty()) {
        return nullptr;
    }
    
    if (candidates.size() == 1) {
        return candidates[0];
    }
    
    // Display disambiguation prompt
    print("Which " + noun + " do you mean?\n");
    
    // List the candidates with numbers
    for (size_t i = 0; i < candidates.size(); ++i) {
        print("  " + std::to_string(i + 1) + ". " + formatObjectDescription(candidates[i]) + "\n");
    }
    
    // Read player's choice
    print("> ");
    std::string response = readLine();
    
    // Parse the response
    ZObject* selected = parseDisambiguationResponse(response, candidates);
    
    if (!selected) {
        printLine("I don't understand that choice.");
    }
    
    return selected;
}

bool Parser::isPreposition(const std::string& word) const {
    return prepositions_.find(word) != prepositions_.end();
}

std::optional<size_t> Parser::findPrepositionIndex(const std::vector<std::string>& tokens) const {
    for (size_t i = 1; i < tokens.size(); ++i) {  // Start at 1 to skip verb
        if (isPreposition(tokens[i])) {
            return i;
        }
    }
    return std::nullopt;
}

bool Parser::validatePreposition(VerbId verb, const std::string& preposition) const {
    // If we have a verb registry, use it for validation
    if (verbRegistry_) {
        return verbRegistry_->isPrepositionValidForVerb(verb, preposition);
    }
    
    // Otherwise, just check if it's a known preposition
    return isPreposition(preposition);
}

// Special command support methods
void Parser::setLastCommand(const std::string& cmd) {
    lastCommand_ = cmd;
}

std::string_view Parser::getLastCommand() const {
    return lastCommand_;
}

void Parser::setLastObject(ZObject* obj) {
    lastObject_ = obj;
}

ZObject* Parser::getLastObject() const {
    return lastObject_;
}

void Parser::setLastObjects(const std::vector<ZObject*>& objs) {
    lastObjects_ = objs;
}

const std::vector<ZObject*>& Parser::getLastObjects() const {
    return lastObjects_;
}

void Parser::setLastUnknownWord(const std::string& word) {
    lastUnknownWord_ = word;
    hadUnknownWordLastTurn_ = true;
}

std::string_view Parser::getLastUnknownWord() const {
    return lastUnknownWord_;
}

void Parser::clearLastUnknownWord() {
    lastUnknownWord_.clear();
    hadUnknownWordLastTurn_ = false;
}

bool Parser::isAllKeyword(const std::string& word) const {
    return word == "all" || word == "everything";
}

bool Parser::isExceptKeyword(const std::string& word) const {
    return word == "except" || word == "but";
}

bool Parser::isAgainCommand(const std::vector<std::string>& tokens) const {
    return !tokens.empty() && (tokens[0] == "again" || tokens[0] == "g");
}

bool Parser::isOopsCommand(const std::vector<std::string>& tokens) const {
    return !tokens.empty() && tokens[0] == "oops";
}

bool Parser::isPronoun(const std::string& word) const {
    return word == "it" || word == "them";
}

std::vector<ZObject*> Parser::findAllApplicableObjects(VerbId verb) const {
    std::vector<ZObject*> applicable;
    auto& g = Globals::instance();
    
    // Determine which objects are applicable based on the verb
    for (const auto& [id, objPtr] : g.getAllObjects()) {
        ZObject* obj = objPtr.get();
        
        // Skip invisible objects
        if (!isObjectVisible(obj)) {
            continue;
        }
        
        // For TAKE: objects in room that can be taken
        if (verb == V_TAKE) {
            if (obj->getLocation() == g.here && 
                obj->hasFlag(ObjectFlag::TAKEBIT) &&
                !obj->hasFlag(ObjectFlag::TRYTAKEBIT)) {
                applicable.push_back(obj);
            }
        }
        // For DROP: objects in inventory
        else if (verb == V_DROP) {
            if (obj->getLocation() == g.winner) {
                applicable.push_back(obj);
            }
        }
        // For other verbs, include visible objects
        else {
            applicable.push_back(obj);
        }
    }
    
    return applicable;
}

std::string Parser::replaceOopsWord(const std::string& original, const std::string& replacement) {
    if (lastUnknownWord_.empty()) {
        return original;
    }
    
    std::string result = original;
    size_t pos = result.find(lastUnknownWord_);
    if (pos != std::string::npos) {
        result.replace(pos, lastUnknownWord_.length(), replacement);
    }
    
    return result;
}

ParsedCommand Parser::parse(const std::string& input) {
    ParsedCommand cmd;
    
    // Handle AGAIN command
    tokenize(input, cmd.words);
    if (isAgainCommand(cmd.words)) {
        if (lastCommand_.empty()) {
            printLine("You haven't entered a command yet.");
            return cmd;
        }
        // Re-parse the last command
        return parse(lastCommand_);
    }
    
    // Handle OOPS command
    if (isOopsCommand(cmd.words)) {
        if (!hadUnknownWordLastTurn_ || lastUnknownWord_.empty()) {
            printLine("There was no word to correct.");
            return cmd;
        }
        
        if (cmd.words.size() < 2) {
            printLine("Oops what?");
            return cmd;
        }
        
        // Replace the unknown word and retry
        std::string corrected = replaceOopsWord(lastCommand_, cmd.words[1]);
        clearLastUnknownWord();
        return parse(corrected);
    }
    
    // Clear unknown word flag for normal commands
    if (!cmd.words.empty()) {
        hadUnknownWordLastTurn_ = false;
    }
    
    if (cmd.words.empty()) {
        return cmd;
    }
    
    // Save this command for AGAIN (but not if it's AGAIN itself)
    if (!isAgainCommand(cmd.words) && !isOopsCommand(cmd.words)) {
        lastCommand_ = input;
    }
    
    // Check if first word is a direction
    Direction* dir = findDirection(cmd.words[0]);
    if (dir) {
        cmd.isDirection = true;
        cmd.direction = *dir;
        cmd.verb = V_WALK;
        return cmd;
    }
    
    // Check for verb
    cmd.verb = findVerb(cmd.words[0]);
    if (cmd.verb == 0) {
        // Unknown verb - save for OOPS
        setLastUnknownWord(cmd.words[0]);
        printLine("I don't know the word \"" + cmd.words[0] + "\".");
        return cmd;
    }
    
    // Check for "all" keyword
    if (cmd.words.size() > 1 && isAllKeyword(cmd.words[1])) {
        cmd.isAll = true;
        
        // Check for "all except [object]"
        if (cmd.words.size() > 2 && isExceptKeyword(cmd.words[2])) {
            // Find the exception object
            if (cmd.words.size() > 3) {
                std::vector<std::string> exceptWords(cmd.words.begin() + 3, cmd.words.end());
                auto exceptMatches = findObjects(exceptWords, 0);
                if (!exceptMatches.empty()) {
                    cmd.exceptObject = exceptMatches.size() == 1
                        ? exceptMatches[0]
                        : disambiguate(exceptMatches, exceptWords.back());
                }
            }
        }
        
        // Find all applicable objects for this verb
        cmd.allObjects = findAllApplicableObjects(cmd.verb);
        
        // Remove the exception object if specified
        if (cmd.exceptObject) {
            cmd.allObjects.erase(
                std::remove(cmd.allObjects.begin(), cmd.allObjects.end(), cmd.exceptObject),
                cmd.allObjects.end()
            );
        }
        
        // Update pronoun tracking
        if (!cmd.allObjects.empty()) {
            setLastObjects(cmd.allObjects);
            if (cmd.allObjects.size() == 1) {
                setLastObject(cmd.allObjects[0]);
            }
        }
        
        return cmd;
    }
    
    // Handle pronoun substitution
    if (cmd.words.size() > 1) {
        if (isPronoun(cmd.words[1])) {
            if (cmd.words[1] == "it") {
                if (lastObject_) {
                    cmd.directObj = lastObject_;
                } else {
                    printLine("I don't know what \"it\" refers to.");
                    return cmd;
                }
            } else if (cmd.words[1] == "them") {
                if (!lastObjects_.empty()) {
                    // For "them", treat as "all" with the last objects
                    cmd.isAll = true;
                    cmd.allObjects = lastObjects_;
                } else {
                    printLine("I don't know what \"them\" refers to.");
                    return cmd;
                }
            }
            
            // If we substituted a pronoun, we're done with object parsing
            if (cmd.directObj || cmd.isAll) {
                return cmd;
            }
        }
    }
    
    // Find preposition and extract indirect object (PRSI)
    if (cmd.verb != 0 && !cmd.isDirection) {
        auto prepIdx = findPrepositionIndex(cmd.words);
        if (prepIdx.has_value() && prepIdx.value() + 1 < cmd.words.size()) {
            const std::string& preposition = cmd.words[prepIdx.value()];
            
            // Validate preposition for this verb
            if (!validatePreposition(cmd.verb, preposition)) {
                // Invalid preposition for this verb
                printLine("I don't understand that.");
                cmd.verb = 0;  // Mark command as invalid
                return cmd;
            }
            
            // Extract direct object (words between verb and preposition)
            std::vector<std::string> directObjWords(
                cmd.words.begin() + 1, 
                cmd.words.begin() + prepIdx.value()
            );
            
            // Extract indirect object (words after preposition)
            std::vector<std::string> indirectObjWords(
                cmd.words.begin() + prepIdx.value() + 1,
                cmd.words.end()
            );
            
            // Find objects
            if (!directObjWords.empty()) {
                auto directMatches = findObjects(directObjWords, 0);
                if (!directMatches.empty()) {
                    cmd.directObj = directMatches.size() == 1 
                        ? directMatches[0]
                        : disambiguate(directMatches, directObjWords.back());
                    
                    // Update pronoun tracking
                    if (cmd.directObj) {
                        setLastObject(cmd.directObj);
                    }
                }
            }
            
            if (!indirectObjWords.empty()) {
                auto indirectMatches = findObjects(indirectObjWords, 0);
                if (!indirectMatches.empty()) {
                    cmd.indirectObj = indirectMatches.size() == 1
                        ? indirectMatches[0]
                        : disambiguate(indirectMatches, indirectObjWords.back());
                }
            }
        } else if (cmd.verb != 0) {
            // No preposition, just try to find direct object
            if (cmd.words.size() > 1) {
                std::vector<std::string> objWords(cmd.words.begin() + 1, cmd.words.end());
                auto matches = findObjects(objWords, 0);
                if (!matches.empty()) {
                    cmd.directObj = matches.size() == 1
                        ? matches[0]
                        : disambiguate(matches, objWords.back());
                    
                    // Update pronoun tracking
                    if (cmd.directObj) {
                        setLastObject(cmd.directObj);
                    }
                } else if (!objWords.empty()) {
                    // Check if any word is unknown
                    for (const auto& word : objWords) {
                        if (word != "the" && word != "a" && word != "an" && 
                            !isPreposition(word)) {
                            // This might be an unknown word
                            setLastUnknownWord(word);
                        }
                    }
                }
            }
        }
    }
    
    return cmd;
}
