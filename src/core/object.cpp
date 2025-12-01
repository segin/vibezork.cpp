#include "object.h"
#include <algorithm>

ZObject::ZObject(ObjectId id, std::string_view desc)
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

void ZObject::addSynonym(std::string_view syn) {
    synonyms_.emplace_back(syn);
    synonymSet_.emplace(syn);  // O(1) lookup cache
}

void ZObject::addAdjective(std::string_view adj) {
    adjectives_.emplace_back(adj);
    adjectiveSet_.emplace(adj);  // O(1) lookup cache
}

bool ZObject::hasSynonym(std::string_view word) const {
    // O(1) lookup using hash set instead of O(n) linear search
    return synonymSet_.find(std::string(word)) != synonymSet_.end();
}

bool ZObject::hasAdjective(std::string_view word) const {
    // O(1) lookup using hash set instead of O(n) linear search
    return adjectiveSet_.find(std::string(word)) != adjectiveSet_.end();
}

void ZObject::setText(std::string_view text) {
    text_ = text;
}

const std::string& ZObject::getText() const {
    return text_;
}

bool ZObject::hasText() const {
    return !text_.empty();
}
