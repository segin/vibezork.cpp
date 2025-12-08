// Object builder - fluent API for creating ZObjects with correct initial values
// Eliminates boilerplate and ensures objects are created correctly from the start
#pragma once

#include "objects.h"
#include "rooms.h"
#include "core/object.h"
#include "core/globals.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <initializer_list>
#include <functional>

// Object configuration using designated initializers (C++20)
struct ObjectDef {
    ObjectId id;
    std::string_view desc;
    std::vector<std::string_view> synonyms = {};
    std::vector<std::string_view> adjectives = {};
    std::vector<ObjectFlag> flags = {};
    int size = 0;
    int capacity = 0;
    int value = 0;
    int tvalue = 0;
    int strength = 0;
    std::string_view longDesc = "";
    std::string_view text = "";
    ObjectId location = 0;  // 0 = nowhere
    std::function<bool()> action = nullptr;
};

// Create and register an object from definition
inline ZObject* createObject(const ObjectDef& def) {
    auto& g = Globals::instance();
    
    auto obj = std::make_unique<ZObject>(def.id, std::string(def.desc));
    
    for (auto syn : def.synonyms) {
        obj->addSynonym(std::string(syn));
    }
    for (auto adj : def.adjectives) {
        obj->addAdjective(std::string(adj));
    }
    for (auto flag : def.flags) {
        obj->setFlag(flag);
    }
    
    if (def.size > 0) obj->setProperty(P_SIZE, def.size);
    if (def.capacity > 0) obj->setProperty(P_CAPACITY, def.capacity);
    if (def.value > 0) obj->setProperty(P_VALUE, def.value);
    if (def.tvalue > 0) obj->setProperty(P_TVALUE, def.tvalue);
    if (def.strength > 0) obj->setProperty(P_STRENGTH, def.strength);
    
    if (!def.longDesc.empty()) {
        obj->setLongDesc(std::string(def.longDesc));
    }
    if (!def.text.empty()) {
        obj->setText(std::string(def.text));
    }
    if (def.action) {
        obj->setAction(def.action);
    }
    
    ZObject* ptr = obj.get();
    g.registerObject(def.id, std::move(obj));
    
    // Set location after registration
    if (def.location != 0) {
        if (auto* loc = g.getObject(def.location)) {
            ptr->moveTo(loc);
        }
    }
    
    return ptr;
}
