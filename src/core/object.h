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
    
    // Action handler
    using ActionFunc = std::function<bool()>;
    void setAction(ActionFunc func) { action_ = func; }
    bool performAction() { return action_ ? action_() : false; }
    
private:
    ObjectId id_;
    std::string desc_;
    std::vector<std::string> synonyms_;
    std::vector<std::string> adjectives_;
    uint32_t flags_ = 0;
    std::map<PropertyId, int> properties_;
    ZObject* location_ = nullptr;
    std::vector<ZObject*> contents_;
    ActionFunc action_;
};
