#pragma once
#include "core/types.h"
#include "syntax.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>

/**
 * VerbRegistry manages verb synonyms and their associated syntax patterns.
 * 
 * This class provides:
 * - Mapping from verb synonyms to canonical verb IDs
 * - Association of syntax patterns with verbs
 * - Lookup of verbs by synonym
 * - Retrieval of syntax patterns for a given verb
 * 
 * Example usage:
 *   VerbRegistry registry;
 *   registry.registerVerb(V_TAKE, {"take", "get", "grab", "carry"});
 *   registry.registerSyntax(V_TAKE, pattern);
 *   
 *   auto verbId = registry.lookupVerb("get");  // Returns V_TAKE
 *   auto patterns = registry.getSyntaxPatterns(V_TAKE);
 */
class VerbRegistry {
public:
    VerbRegistry();
    
    /**
     * Register a verb with its synonyms.
     * Maps all synonyms to the canonical verb ID.
     * 
     * @param verbId The canonical verb identifier
     * @param synonyms List of synonym words for this verb
     */
    void registerVerb(VerbId verbId, std::vector<std::string> synonyms);
    
    /**
     * Register a syntax pattern for a verb.
     * A verb can have multiple syntax patterns (e.g., "TAKE OBJECT" and "TAKE OBJECT FROM OBJECT").
     * 
     * @param verbId The verb identifier
     * @param pattern The syntax pattern to associate with this verb
     */
    void registerSyntax(VerbId verbId, SyntaxPattern pattern);
    
    /**
     * Look up a verb by synonym.
     * 
     * @param word The word to look up
     * @return The verb ID if found, std::nullopt otherwise
     */
    std::optional<VerbId> lookupVerb(std::string_view word) const;
    
    /**
     * Get all syntax patterns for a verb.
     * 
     * @param verbId The verb identifier
     * @return Vector of syntax patterns for this verb (may be empty)
     */
    const std::vector<SyntaxPattern>& getSyntaxPatterns(VerbId verbId) const;
    
    /**
     * Check if a verb has any registered patterns.
     * 
     * @param verbId The verb identifier
     * @return true if the verb has at least one pattern
     */
    bool hasPatterns(VerbId verbId) const;
    
    /**
     * Get all registered verb IDs.
     * Useful for iteration or debugging.
     * 
     * @return Vector of all registered verb IDs
     */
    std::vector<VerbId> getAllVerbs() const;
    
private:
    // Map from synonym word to verb ID
    std::unordered_map<std::string, VerbId> verbMap_;
    
    // Map from verb ID to its syntax patterns
    std::unordered_map<VerbId, std::vector<SyntaxPattern>> syntaxMap_;
    
    // Empty pattern vector for verbs with no patterns
    static const std::vector<SyntaxPattern> emptyPatterns_;
    
    // Initialize all verb synonyms from gsyntax.zil
    void initializeVerbSynonyms();
    
    // Initialize all syntax patterns from gsyntax.zil
    void initializeSyntaxPatterns();
};

