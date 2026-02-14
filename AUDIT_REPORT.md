# Comprehensive Codebase Audit Report: Zork I C++ Port

This report details security vulnerabilities, implementation issues, and potential improvements identified during a review of the Zork I C++ Port codebase.

## 1. Security Audit

### 1.1 Critical Vulnerabilities

#### **Arbitrary File Read/Write (Path Traversal)**
- **Location**: `src/verbs/verbs.cpp` in `vSave()` and `vRestore()`.
- **Description**: The `vSave` and `vRestore` functions take user input directly for filenames without any sanitization or validation.
- **Impact**: A malicious user could supply a path like `../../../../etc/passwd` (or Windows equivalent) to read arbitrary files on the system or overwrite critical system files if the game is run with sufficient privileges.
- **Remediation**:
  - Sanitize filenames to allow only alphanumeric characters, underscores, and dots.
  - Restrict file operations to a specific `save/` directory.
  - Use `std::filesystem::path` to validate and resolve paths safely.

### 1.2 Medium Vulnerabilities

#### **Predictable Random Number Generation (RNG)**
- **Location**: `src/systems/combat.cpp`, `src/systems/death.cpp`.
- **Description**: These systems use `std::rand()` for combat outcomes and item scattering. However, `std::srand()` is never called in `main.cpp` or `initializeGame()`.
- **Impact**: The sequence of random numbers will be identical every time the game is run. This makes combat deterministic and exploitable by players who memorize the sequence.
- **Remediation**:
  - Call `std::srand(std::time(nullptr))` once at startup (though `std::rand` is generally discouraged).
  - Ideally, replace all `std::rand()` usage with `std::mt19937` seeded with `std::random_device` (as used in `src/systems/npc.cpp`).

#### **Save File Integrity**
- **Location**: `src/systems/save.cpp`.
- **Description**: The save file format is plain text (`ZORK_SAVE_V1`) and lacks any integrity check (checksum or hash).
- **Impact**: Players can easily modify save files to cheat (e.g., change location, score, flags) or cause the game to crash by introducing invalid data.
- **Remediation**:
  - Implement a checksum (e.g., CRC32 or SHA-256) at the end of the file.
  - Verify the checksum upon loading.
  - Consider using a binary format or encrypted format if tamper-resistance is desired.

### 1.3 Low Vulnerabilities

#### **Unchecked Input Length**
- **Location**: `src/core/io.cpp` in `readLine()`.
- **Description**: While `std::getline` handles memory allocation, extremely long inputs could theoretically cause a denial of service (DoS) via memory exhaustion, although this is unlikely in a local console game.
- **Remediation**: Limit input length to a reasonable maximum (e.g., 1024 characters).

## 2. Implementation Audit

### 2.1 Code Quality Issues

#### **Inconsistent RNG Usage**
- **Description**: `src/systems/npc.cpp` uses the modern and robust `std::mt19937`, while `src/systems/combat.cpp` and `src/systems/death.cpp` use the legacy and flawed `std::rand()`.
- **Impact**: Inconsistent code style and potential statistical weaknesses in combat logic.
- **Recommendation**: Standardize on `std::mt19937` and `std::uniform_int_distribution` across the entire codebase. Create a central `Random` utility class.

#### **Raw Pointer Usage**
- **Location**: `src/core/object.h`, `src/parser/parser.h`, and throughout the codebase.
- **Description**: Extensive use of raw pointers (`ZObject*`) for object relationships (`location_`, `contents_`).
- **Impact**: Increases the risk of memory leaks, dangling pointers, and double-free errors. Ownership semantics are unclear.
- **Recommendation**:
  - Use `std::unique_ptr` for owning references (e.g., the global object list).
  - Use `std::weak_ptr` or raw pointers (non-owning) for relationships like location, but ensure proper cleanup.

#### **Error Handling in Parser**
- **Location**: `src/parser/parser.cpp`.
- **Description**: Uses `std::stoi` and `std::stoul` which can throw exceptions if the input is not a valid number. While some `try-catch` blocks exist, relying on exceptions for normal control flow or missing them can lead to crashes.
- **Recommendation**: Use `std::from_chars` (C++17) or verify string content before conversion.

#### **Unimplemented/Placeholder Code**
- **Location**: `src/verbs/verbs.cpp`.
- **Description**: Several verbs like `vRandom`, `vBug`, `vRecord`, `vUnrecord` are placeholders or print simple messages without functionality.
- **Recommendation**: Either implement the functionality or remove the commands if they are not intended for the final release.

## 3. Suggestions for Improvement

### 3.1 Architecture
- **Centralized RNG**: Create a `Random` singleton or static class that wraps `std::mt19937` and provides helper methods (e.g., `Random::get(min, max)`).
- **Smart Pointers**: Refactor `ZObject` management to use smart pointers for automatic memory management.
- **Configuration**: Move hardcoded values (e.g., max inventory weight, initial stats) to a configuration file or constants header.

### 3.2 Features
- **Autosave**: Implement an autosave feature that saves the game state periodically or upon entering specific rooms.
- **Robust Save Format**: Switch to a structured format like JSON or a binary format with versioning and checksums for better forward compatibility and integrity.
- **Command History**: Implement command history (up/down arrow keys) using a library like `readline` or `linenoise` for better user experience.
- **Scripting Support**: Expand the `vScript` functionality to allow loading and executing batch commands from a file for testing or automation.

### 3.3 Testing
- **Unit Tests**: Add unit tests for `Parser` edge cases, especially around numeric inputs and special characters.
- **Fuzz Testing**: Implement fuzz testing for the parser to identify inputs that could cause crashes or unexpected behavior.

## 4. Conclusion
The Zork I C++ Port is a functional implementation but suffers from significant security vulnerabilities related to file I/O and potential stability issues due to raw pointer usage and legacy RNG. Addressing the arbitrary file read/write vulnerability and standardizing the RNG should be the immediate priorities.
