---
inclusion: always
---

# Modern C++ Implementation Guidelines

## Philosophy

Use the latest C++17/20/23 features available to create idiomatic, clean, and maintainable code. Do not shy away from modern language features when they make the code more expressive, safer, or easier to understand.

## Encouraged Modern C++ Features

### C++17 Features (Primary Target)
- **Structured bindings**: `auto [key, value] = map.find(...)`
- **`if` with initializer**: `if (auto it = map.find(key); it != map.end())`
- **`std::optional`**: For values that may or may not exist
- **`std::variant`**: Type-safe unions
- **`std::string_view`**: Non-owning string references
- **Fold expressions**: Template parameter pack expansion
- **Class template argument deduction**: `std::pair(1, 2.0)` instead of `std::pair<int, double>`
- **Inline variables**: For header-only constants
- **`constexpr if`**: Compile-time conditionals

### C++20 Features (When Available)
- **Concepts**: For template constraints
- **Ranges**: Modern iteration and algorithms
- **Coroutines**: For async operations if needed
- **Three-way comparison (`<=>`)**
- **Designated initializers**: `Point{.x = 10, .y = 20}`
- **`std::span`**: Non-owning array views

### C++23 Features (When Available)
- **`std::expected`**: For error handling
- **`std::mdspan`**: Multi-dimensional array views
- **Deducing `this`**: Simplified CRTP patterns

## Specific Use Cases for Zork Port

### 1. ZIL Conditionals → Modern C++

**ZIL Pattern:**
```lisp
<COND (<AND <VERB? TAKE> <EQUAL? ,PRSO ,MAILBOX>>
       <TELL "It is securely anchored." CR>)>
```

**Modern C++ (with structured binding and early return):**
```cpp
bool mailboxAction() {
    auto& g = Globals::instance();
    
    if (auto [verb, obj] = std::tuple{g.prsa, g.prso}; 
        verb == V_TAKE && obj && obj->getId() == OBJ_MAILBOX) {
        printLine("It is securely anchored.");
        return true;
    }
    return false;
}
```

### 2. Object Lookup → `std::optional`

**Instead of:**
```cpp
ZObject* obj = findObject(name);
if (obj) {
    // use obj
}
```

**Use:**
```cpp
if (auto obj = findObject(name)) {
    // use *obj
}
```

### 3. Parser State → `std::variant`

**For multi-type parser results:**
```cpp
using ParseResult = std::variant<
    VerbCommand,
    DirectionCommand,
    InvalidCommand
>;

ParseResult parse(const std::string& input);
```

### 4. Property Access → `std::optional`

**Instead of returning 0 for missing properties:**
```cpp
std::optional<int> getProperty(PropertyId prop) const {
    if (auto it = properties_.find(prop); it != properties_.end()) {
        return it->second;
    }
    return std::nullopt;
}
```

### 5. Room Exits → Structured Bindings

**When iterating exits:**
```cpp
for (const auto& [direction, exit] : room->getExits()) {
    if (exit.isAccessible()) {
        // process exit
    }
}
```

### 6. Flag Checking → Concepts (C++20)

**For type-safe flag operations:**
```cpp
template<typename T>
concept ObjectFlag = std::is_same_v<T, ObjectFlag>;

template<ObjectFlag... Flags>
bool hasAllFlags(Flags... flags) const {
    return (hasFlag(flags) && ...);  // fold expression
}
```

### 7. String Handling → `std::string_view`

**For non-owning string parameters:**
```cpp
void print(std::string_view text);  // No copy, works with literals and strings
```

### 8. Error Handling → `std::expected` (C++23)

**For operations that can fail:**
```cpp
std::expected<ZObject*, ParseError> parseObject(std::string_view input);

if (auto result = parseObject(input)) {
    ZObject* obj = *result;
    // use obj
} else {
    handleError(result.error());
}
```

## Code Style Guidelines

### Prefer Modern Idioms

1. **Range-based for loops** over index loops
2. **Lambdas** for callbacks and small functions
3. **Smart pointers** over raw pointers
4. **`auto`** for obvious types
5. **Structured bindings** for multiple return values
6. **`constexpr`** for compile-time constants
7. **`if constexpr`** for compile-time branches

### Example: ZIL TELL Macro → Modern C++

**ZIL:**
```lisp
<TELL "You see " D ,OBJ " here." CR>
```

**Modern C++ with fold expressions:**
```cpp
template<typename... Args>
void tell(Args&&... args) {
    (print(std::forward<Args>(args)), ...);
    crlf();
}

// Usage:
tell("You see ", obj->getDesc(), " here.");
```

## When to Use Advanced Features

### DO Use When:
- It makes the code **clearer** and more **expressive**
- It improves **type safety**
- It eliminates **boilerplate**
- It prevents **common errors**
- It matches the **semantic intent** of the ZIL code

### DON'T Use When:
- It obscures the **logic**
- It requires **excessive template metaprogramming**
- It makes debugging **significantly harder**
- The simpler approach is **equally clear**

## Practical Examples for Zork

### Object Creation with Designated Initializers

```cpp
struct ObjectConfig {
    ObjectId id;
    std::string desc;
    std::vector<std::string> synonyms;
    std::vector<ObjectFlag> flags;
};

auto mailbox = createObject(ObjectConfig{
    .id = OBJ_MAILBOX,
    .desc = "small mailbox",
    .synonyms = {"mailbox", "box"},
    .flags = {ObjectFlag::CONTBIT, ObjectFlag::TRYTAKEBIT}
});
```

### Parser with `std::optional` and Structured Bindings

```cpp
std::optional<ParsedCommand> Parser::tryParse(std::string_view input) {
    if (auto [verb, rest] = extractVerb(input); verb) {
        if (auto [obj1, obj2] = extractObjects(rest); obj1) {
            return ParsedCommand{*verb, *obj1, obj2};
        }
    }
    return std::nullopt;
}
```

### Room Navigation with Monadic Operations (C++23)

```cpp
auto destination = currentRoom
    ->getExit(direction)
    .and_then([](auto& exit) { return exit.checkCondition(); })
    .transform([](auto& exit) { return getRoom(exit.targetId); });

if (destination) {
    moveTo(*destination);
}
```

## Summary

Modern C++ provides powerful tools for creating clean, safe, and expressive code. When converting ZIL to C++, leverage these features to create code that is:

1. **More readable** than direct translations
2. **Safer** with compile-time checks
3. **More maintainable** with clear intent
4. **More efficient** with zero-cost abstractions

The goal is not to use features for their own sake, but to write the best possible C++ code that faithfully represents the original game logic while being idiomatic and maintainable.
