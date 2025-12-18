#pragma once
#include "flags.h"
#include "types.h"
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

/**
 * @brief Core game object class (mirrors ZIL <OBJECT> definition)
 *
 * ZObject represents all game entities including items, NPCs, and scenery.
 * Each object has:
 * - Unique ID and description
 * - Synonyms and adjectives for parser matching
 * - Flags (TAKEBIT, OPENBIT, etc.) for state and capabilities
 * - Properties (SIZE, CAPACITY, VALUE, etc.) for numeric attributes
 * - Location tracking for containment hierarchy
 * - Optional action handler for object-specific behavior
 *
 * ZRoom extends this class for room-specific functionality.
 *
 * @see ZIL equivalent: <OBJECT> definitions in 1DUNGEON.ZIL
 */
class ZObject {
public:
  ZObject(ObjectId id, std::string_view desc);
  virtual ~ZObject() = default;

  // Property accessors
  void setProperty(PropertyId prop, int value);
  int getProperty(PropertyId prop) const;

  // Text property accessors
  void setText(std::string_view text);
  const std::string &getText() const;
  bool hasText() const;

  // Long description (for room display)
  void setLongDesc(std::string_view ldesc) { longDesc_ = ldesc; }
  const std::string &getLongDesc() const { return longDesc_; }
  bool hasLongDesc() const { return !longDesc_.empty(); }

  // Flag operations
  void setFlag(ObjectFlag flag);
  void clearFlag(ObjectFlag flag);
  bool hasFlag(ObjectFlag flag) const;

  // Location/containment
  void moveTo(ZObject *location);
  ZObject *getLocation() const { return location_; }
  const std::vector<ZObject *> &getContents() const { return contents_; }

  // Identification
  ObjectId getId() const { return id_; }
  const std::string &getDesc() const { return desc_; }
  void addSynonym(std::string_view syn);
  void addAdjective(std::string_view adj);
  const std::vector<std::string> &getSynonyms() const { return synonyms_; }
  const std::vector<std::string> &getAdjectives() const { return adjectives_; }
  bool hasSynonym(std::string_view word) const;
  bool hasAdjective(std::string_view word) const;

  // Action handler
  using ActionFunc = std::function<bool()>;
  void setAction(ActionFunc func) { action_ = func; }
  bool performAction() { return action_ ? action_() : false; }

  // Serialization support (for save/restore system)
  uint32_t getAllFlags() const { return flags_; }
  void setAllFlags(uint32_t flags) { flags_ = flags; }
  const std::map<PropertyId, int> &getAllProperties() const {
    return properties_;
  }

private:
  ObjectId id_;
  std::string desc_;
  std::vector<std::string> synonyms_;
  std::vector<std::string> adjectives_;
  std::unordered_set<std::string> synonymSet_;   // O(1) lookup cache
  std::unordered_set<std::string> adjectiveSet_; // O(1) lookup cache
  uint64_t flags_ = 0;
  std::map<PropertyId, int> properties_;
  std::string text_;     // For readable objects
  std::string longDesc_; // Long description for room display
  ZObject *location_ = nullptr;
  std::vector<ZObject *> contents_;
  ActionFunc action_;
};
