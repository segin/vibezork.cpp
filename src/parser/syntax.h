#pragma once
#include "core/types.h"
#include "core/flags.h"
#include "world/rooms.h"
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <functional>

// Forward declaration
class ZObject;

// Parse result structure
struct ParseResult {
    VerbId verb = 0;
    ZObject* prso = nullptr;  // Direct object
    ZObject* prsi = nullptr;  // Indirect object
    std::string error;
    bool success = false;
};

// Syntax pattern system for verb command parsing
class SyntaxPattern {
public:
    // Element types in a syntax pattern
    enum class ElementType {
        VERB,
        OBJECT,
        PREPOSITION,
        DIRECTION
    };
    
    // Individual element in a pattern
    struct Element {
        ElementType type;
        std::vector<std::string> values;  // For prepositions (e.g., "with", "in", "on")
        std::optional<ObjectFlag> requiredFlag;  // For objects (e.g., TAKEBIT)
        bool optional = false;
        
        Element(ElementType t) : type(t) {}
        Element(ElementType t, std::vector<std::string> vals) 
            : type(t), values(std::move(vals)) {}
        Element(ElementType t, ObjectFlag flag) 
            : type(t), requiredFlag(flag) {}
    };
    
    // Constructor
    SyntaxPattern(VerbId verbId, std::vector<Element> pattern);
    
    // Check if tokens match this pattern
    bool matches(const std::vector<std::string>& tokens) const;
    
    // Apply pattern to extract objects from matched tokens
    // objectFinder: function to find objects by name, returns nullptr if not found
    using ObjectFinder = std::function<ZObject*(const std::string&, std::optional<ObjectFlag>)>;
    ParseResult apply(const std::vector<std::string>& tokens, 
                     const ObjectFinder& objectFinder) const;
    
    // Getters
    VerbId getVerbId() const { return verbId_; }
    const std::vector<Element>& getPattern() const { return pattern_; }
    
private:
    VerbId verbId_;
    std::vector<Element> pattern_;
    
    // Helper methods
    bool matchElement(const Element& element, std::string_view token) const;
    std::optional<size_t> findPrepositionIndex(const std::vector<std::string>& tokens) const;
};
