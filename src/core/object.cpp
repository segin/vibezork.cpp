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

std::optional<int> ZObject::getPropertyOpt(PropertyId prop) const {
    if (auto it = properties_.find(prop); it != properties_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ZObject::setFlag(ObjectFlag flag) {
    flags_ |= static_cast<uint64_t>(flag);
}

void ZObject::clearFlag(ObjectFlag flag) {
    flags_ &= ~static_cast<uint64_t>(flag);
}

bool ZObject::hasFlag(ObjectFlag flag) const {
    return (flags_ & static_cast<uint64_t>(flag)) != 0;
}

void ZObject::moveTo(ZObject* location) {
    // Prevent moving object to itself
    if (location == this) {
        return;
    }
    
    // Remove from current location using C++20/23 std::erase
    if (location_) {
        std::erase(location_->contents_, this);
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
    // O(1) lookup using hash set, case-insensitive with C++20/23 .contains()
    std::string lowerWord(word);
    std::ranges::transform(lowerWord, lowerWord.begin(), ::tolower);
    return synonymSet_.contains(lowerWord);
}

bool ZObject::hasAdjective(std::string_view word) const {
    // O(1) lookup using hash set, case-insensitive with C++20/23 .contains()
    std::string lowerWord(word);
    std::ranges::transform(lowerWord, lowerWord.begin(), ::tolower);
    return adjectiveSet_.contains(lowerWord);
}

void ZObject::setText(std::string_view text) {
    text_ = text;
}

const std::string& ZObject::getText() const {
    return text_;
}

std::optional<std::string_view> ZObject::getTextOpt() const {
    if (!text_.empty()) {
        return std::string_view(text_);
    }
    return std::nullopt;
}

bool ZObject::hasText() const {
    return !text_.empty();
}
