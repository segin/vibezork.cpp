#include "syntax.h"
#include "core/globals.h"
#include <algorithm>

SyntaxPattern::SyntaxPattern(VerbId verbId, std::vector<Element> pattern)
    : verbId_(verbId), pattern_(std::move(pattern)) {
}

bool SyntaxPattern::matchElement(const Element& element, std::string_view token) const {
    switch (element.type) {
        case ElementType::VERB:
            // Verb matching is handled separately
            return true;
            
        case ElementType::PREPOSITION:
            // Check if token matches any of the allowed prepositions
            return std::any_of(element.values.begin(), element.values.end(),
                [token](const std::string& prep) { return prep == token; });
            
        case ElementType::OBJECT:
            // Object matching requires actual object lookup (handled in apply)
            return true;
            
        case ElementType::DIRECTION:
            // Direction matching (handled in apply)
            return true;
    }
    return false;
}

std::optional<size_t> SyntaxPattern::findPrepositionIndex(
    const std::vector<std::string>& tokens) const {
    
    // Find preposition elements in pattern
    for (const auto& element : pattern_) {
        if (element.type == ElementType::PREPOSITION) {
            // Search for any of the prepositions in tokens
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (std::any_of(element.values.begin(), element.values.end(),
                    [&tokens, i](const std::string& prep) { 
                        return prep == tokens[i]; 
                    })) {
                    return i;
                }
            }
        }
    }
    return std::nullopt;
}

bool SyntaxPattern::matches(const std::vector<std::string>& tokens) const {
    if (tokens.empty()) {
        return false;
    }
    
    // Track position in tokens and pattern
    size_t tokenIdx = 1;  // Skip verb (first token)
    size_t patternIdx = 1;  // Skip verb element in pattern
    
    while (patternIdx < pattern_.size()) {
        const auto& element = pattern_[patternIdx];
        
        // If we've run out of tokens
        if (tokenIdx >= tokens.size()) {
            // Check if remaining elements are optional
            if (element.optional) {
                patternIdx++;
                continue;
            }
            // If we've consumed all tokens and only optional elements remain
            bool allOptional = true;
            for (size_t i = patternIdx; i < pattern_.size(); ++i) {
                if (!pattern_[i].optional) {
                    allOptional = false;
                    break;
                }
            }
            return allOptional;
        }
        
        // Handle different element types
        switch (element.type) {
            case ElementType::PREPOSITION: {
                // Check if current token matches any of the allowed prepositions
                if (matchElement(element, tokens[tokenIdx])) {
                    tokenIdx++;
                    patternIdx++;
                } else if (element.optional) {
                    // Optional preposition not found, skip pattern element
                    patternIdx++;
                } else {
                    // Required preposition not found
                    return false;
                }
                break;
            }
                
            case ElementType::OBJECT: {
                // Objects consume tokens until we hit a preposition or end
                // Look ahead for prepositions
                bool foundPrep = false;
                for (size_t i = patternIdx + 1; i < pattern_.size(); ++i) {
                    if (pattern_[i].type == ElementType::PREPOSITION) {
                        // Check if any upcoming token is this preposition
                        for (size_t j = tokenIdx; j < tokens.size(); ++j) {
                            if (matchElement(pattern_[i], tokens[j])) {
                                foundPrep = true;
                                break;
                            }
                        }
                        break;
                    }
                }
                
                // Consume at least one token for the object
                if (tokenIdx < tokens.size()) {
                    tokenIdx++;
                    // Consume additional tokens until preposition or end
                    while (tokenIdx < tokens.size() && !foundPrep) {
                        // Check if current token is a preposition
                        bool isPrep = false;
                        for (size_t i = patternIdx + 1; i < pattern_.size(); ++i) {
                            if (pattern_[i].type == ElementType::PREPOSITION &&
                                matchElement(pattern_[i], tokens[tokenIdx])) {
                                isPrep = true;
                                break;
                            }
                        }
                        if (isPrep) break;
                        tokenIdx++;
                    }
                }
                patternIdx++;
                break;
            }
                
            case ElementType::DIRECTION: {
                // Directions are single tokens
                tokenIdx++;
                patternIdx++;
                break;
            }
                
            case ElementType::VERB: {
                // Should not happen (verb is first)
                patternIdx++;
                break;
            }
        }
    }
    
    // Successfully matched if we've processed all required pattern elements
    return true;
}

ParseResult SyntaxPattern::apply(const std::vector<std::string>& tokens) const {
    ParseResult result;
    result.verb = verbId_;
    result.success = false;
    
    if (tokens.empty()) {
        result.error = "Empty command";
        return result;
    }
    
    // Find preposition position if any
    auto prepIdx = findPrepositionIndex(tokens);
    
    // Track which pattern elements need objects
    bool needsPrso = false;
    bool needsPrsi = false;
    std::optional<ObjectFlag> prsoRequiredFlag;
    std::optional<ObjectFlag> prsiRequiredFlag;
    
    // Analyze pattern to determine what objects are needed
    size_t objectCount = 0;
    for (const auto& element : pattern_) {
        if (element.type == ElementType::OBJECT) {
            if (objectCount == 0) {
                needsPrso = true;
                prsoRequiredFlag = element.requiredFlag;
            } else if (objectCount == 1) {
                needsPrsi = true;
                prsiRequiredFlag = element.requiredFlag;
            }
            objectCount++;
        }
    }
    
    // Extract direct object (PRSO) - words between verb and preposition (or end)
    if (needsPrso) {
        size_t objStartIdx = 1;  // After verb
        size_t objEndIdx = prepIdx.value_or(tokens.size());
        
        if (objStartIdx < objEndIdx) {
            // Build object name from tokens
            std::string objName;
            for (size_t i = objStartIdx; i < objEndIdx; ++i) {
                if (!objName.empty()) objName += " ";
                objName += tokens[i];
            }
            
            // Note: Actual object lookup will be done by the parser
            // This method just identifies what needs to be looked up
            // The parser will need to call a separate object finder
            // For now, we store the information in the result
            
            // In a complete implementation, this would be:
            // result.prso = findObjectByName(objName, prsoRequiredFlag);
            // if (!result.prso) {
            //     result.error = "You can't see any " + objName + " here.";
            //     return result;
            // }
        }
    }
    
    // Extract indirect object (PRSI) - words after preposition
    if (needsPrsi && prepIdx.has_value() && prepIdx.value() + 1 < tokens.size()) {
        size_t indirStartIdx = prepIdx.value() + 1;
        size_t indirEndIdx = tokens.size();
        
        // Build object name from tokens
        std::string objName;
        for (size_t i = indirStartIdx; i < indirEndIdx; ++i) {
            if (!objName.empty()) objName += " ";
            objName += tokens[i];
        }
        
        // Note: Actual object lookup will be done by the parser
        // In a complete implementation:
        // result.prsi = findObjectByName(objName, prsiRequiredFlag);
        // if (!result.prsi) {
        //     result.error = "You can't see any " + objName + " here.";
        //     return result;
        // }
    }
    
    // Validate pattern requirements
    // Check if PRSO has required flag (if specified)
    if (result.prso && prsoRequiredFlag.has_value()) {
        // In full implementation:
        // if (!result.prso->hasFlag(prsoRequiredFlag.value())) {
        //     result.error = "You can't do that with the " + result.prso->getDesc();
        //     return result;
        // }
    }
    
    // Check if PRSI has required flag (if specified)
    if (result.prsi && prsiRequiredFlag.has_value()) {
        // In full implementation:
        // if (!result.prsi->hasFlag(prsiRequiredFlag.value())) {
        //     result.error = "You can't do that with the " + result.prsi->getDesc();
        //     return result;
        // }
    }
    
    result.success = true;
    return result;
}
