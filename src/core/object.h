#pragma once
#include "types.h"
#include "flags.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

class ZObject {
public:
    ZObject(ObjectId id, const std::string& desc);
    virtual ~ZObject() = default;
    
    // Property accessors
    void setProperty(PropertyId prop, int value);
    int getProperty(PropertyId prop) const;
    
    // Text property accessors
    void setText(const std::string& text);
    const std::string& getText() const;
    bool hasText() const;
    
    // Flag operations
    void setFlag(ObjectFlag flag);
    void clearFlag(ObjectFlag flag);
    bool hasFlag(ObjectFlag flag) const;
    
    // Location/containment
    void moveTo(ZObject* location);
    ZObject* getLocation() const { return location_; }
    const std::vector<ZObject*>& getContents() const { return contents_; }
    
    // Identification
    ObjectId getId() const { return id_; }
    const std::string& getDesc() const { return desc_; }
    void addSynonym(const std::string& syn) { synonyms_.push_back(syn); }
    void addAdjective(const std::string& adj) { adjectives_.push_back(adj); }
    const std::vector<std::string>& getSynonyms() const { return synonyms_; }
    const std::vector<std::string>& getAdjectives() const { return adjectives_; }
    bool hasSynonym(const std::string& word) const;
    bool hasAdjective(const std::string& word) const;
    
    // Action handler
    using ActionFunc = std::function<bool()>;
    void setAction(ActionFunc func) { action_ = func; }
    bool performAction() { return action_ ? action_() : false; }
    
    // Serialization support (for save/restore system)
    uint32_t getAllFlags() const { return flags_; }
    void setAllFlags(uint32_t flags) { flags_ = flags; }
    const std::map<PropertyId, int>& getAllProperties() const { return properties_; }
    
private:
    ObjectId id_;
    std::string desc_;
    std::vector<std::string> synonyms_;
    std::vector<std::string> adjectives_;
    uint32_t flags_ = 0;
    std::map<PropertyId, int> properties_;
    std::string text_;  // For readable objects
    ZObject* location_ = nullptr;
    std::vector<ZObject*> contents_;
    ActionFunc action_;
};
