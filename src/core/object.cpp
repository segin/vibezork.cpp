#include "object.h"
#include <algorithm>

ZObject::ZObject(ObjectId id, const std::string& desc)
    : id_(id), desc_(desc) {}

void ZObject::setProperty(PropertyId prop, int value) {
    properties_[prop] = value;
}

int ZObject::getProperty(PropertyId prop) const {
    auto it = properties_.find(prop);
    return it != properties_.end() ? it->second : 0;
}

void ZObject::setFlag(ObjectFlag flag) {
    flags_ |= static_cast<uint32_t>(flag);
}

void ZObject::clearFlag(ObjectFlag flag) {
    flags_ &= ~static_cast<uint32_t>(flag);
}

bool ZObject::hasFlag(ObjectFlag flag) const {
    return (flags_ & static_cast<uint32_t>(flag)) != 0;
}

void ZObject::moveTo(ZObject* location) {
    // Remove from current location
    if (location_) {
        auto& contents = location_->contents_;
        contents.erase(std::remove(contents.begin(), contents.end(), this), contents.end());
    }
    
    // Add to new location
    location_ = location;
    if (location_) {
        location_->contents_.push_back(this);
    }
}

bool ZObject::hasSynonym(const std::string& word) const {
    return std::find(synonyms_.begin(), synonyms_.end(), word) != synonyms_.end();
}

bool ZObject::hasAdjective(const std::string& word) const {
    return std::find(adjectives_.begin(), adjectives_.end(), word) != adjectives_.end();
}
