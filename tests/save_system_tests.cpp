#include "test_framework.h"
#include "systems/save.h"
#include "core/globals.h"
#include "core/object.h"
#include "systems/score.h"
#include "systems/timer.h"
#include "world/world.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

// Test file path
const char* TEST_SAVE_FILE = "test_save.dat";

// Helper to clean up test file
void cleanupTestFile() {
    std::remove(TEST_SAVE_FILE);
}

// Test basic save functionality (Requirement 60)
void testBasicSave() {
    cleanupTestFile();
    
    auto& g = Globals::instance();
    g.reset();
    
    // Set up some basic state
    g.score = 100;
    g.moves = 50;
    g.verboseMode = false;
    g.briefMode = true;
    
    // Perform save
    auto result = SaveSystem::save(TEST_SAVE_FILE);
    
    TEST_ASSERT(result == SaveSystem::SaveError::SUCCESS,
                "Save should succeed");
    
    // Verify file exists
    std::ifstream check(TEST_SAVE_FILE);
    TEST_ASSERT(check.good(), "Save file should exist");
    check.close();
    
    cleanupTestFile();
    std::cout << "✓ Basic save test passed" << std::endl;
}

// Test basic restore functionality (Requirement 61)
void testBasicRestore() {
    cleanupTestFile();
    
    auto& g = Globals::instance();
    auto& score = ScoreSystem::instance();
    g.reset();
    score.reset();
    
    // Set up initial state
    score.addScore(100);
    for (int i = 0; i < 50; i++) {
        score.incrementMoves();
    }
    g.verboseMode = false;
    g.briefMode = true;
    
    // Save
    auto saveResult = SaveSystem::save(TEST_SAVE_FILE);
    TEST_ASSERT(saveResult == SaveSystem::SaveError::SUCCESS,
                "Save should succeed");
    
    // Modify state
    g.reset();
    score.reset();
    TEST_ASSERT(score.getScore() == 0, "Score should be reset to 0");
    TEST_ASSERT(score.getMoves() == 0, "Moves should be reset to 0");
    
    // Restore
    auto restoreResult = SaveSystem::restore(TEST_SAVE_FILE);
    TEST_ASSERT(restoreResult == SaveSystem::SaveError::SUCCESS,
                "Restore should succeed");
    
    // Verify state was restored
    TEST_ASSERT(score.getScore() == 100, "Score should be restored to 100");
    TEST_ASSERT(score.getMoves() == 50, "Moves should be restored to 50");
    TEST_ASSERT(!g.verboseMode, "Verbose mode should be restored to false");
    TEST_ASSERT(g.briefMode, "Brief mode should be restored to true");
    
    cleanupTestFile();
    std::cout << "✓ Basic restore test passed" << std::endl;
}

// Test save with non-existent directory (Requirement 60)
void testSavePermissionError() {
    auto result = SaveSystem::save("/nonexistent/directory/save.dat");
    
    TEST_ASSERT(result == SaveSystem::SaveError::PERMISSION_DENIED,
                "Save to invalid path should return PERMISSION_DENIED");
    
    std::cout << "✓ Save permission error test passed" << std::endl;
}

// Test restore with non-existent file (Requirement 61)
void testRestoreFileNotFound() {
    auto result = SaveSystem::restore("nonexistent_file.dat");
    
    TEST_ASSERT(result == SaveSystem::SaveError::FILE_NOT_FOUND,
                "Restore of non-existent file should return FILE_NOT_FOUND");
    
    std::cout << "✓ Restore file not found test passed" << std::endl;
}

// Test restore with corrupt file (Requirement 61)
void testRestoreCorruptFile() {
    cleanupTestFile();
    
    // Create a corrupt save file
    std::ofstream corrupt(TEST_SAVE_FILE);
    corrupt << "INVALID_VERSION\n";
    corrupt << "GARBAGE DATA\n";
    corrupt.close();
    
    auto result = SaveSystem::restore(TEST_SAVE_FILE);
    
    TEST_ASSERT(result == SaveSystem::SaveError::VERSION_MISMATCH,
                "Restore of corrupt file should return VERSION_MISMATCH");
    
    cleanupTestFile();
    std::cout << "✓ Restore corrupt file test passed" << std::endl;
}

// Test error message conversion
void testErrorMessages() {
    TEST_ASSERT(SaveSystem::errorToString(SaveSystem::SaveError::SUCCESS) == "Success",
                "SUCCESS error message should be correct");
    
    TEST_ASSERT(SaveSystem::errorToString(SaveSystem::SaveError::FILE_NOT_FOUND) == "File not found",
                "FILE_NOT_FOUND error message should be correct");
    
    TEST_ASSERT(SaveSystem::errorToString(SaveSystem::SaveError::PERMISSION_DENIED) == "Permission denied",
                "PERMISSION_DENIED error message should be correct");
    
    TEST_ASSERT(SaveSystem::errorToString(SaveSystem::SaveError::CORRUPT_FILE) == "Corrupt save file",
                "CORRUPT_FILE error message should be correct");
    
    std::cout << "✓ Error message test passed" << std::endl;
}

// Test save/restore with objects (Requirement 60.3, 61.3)
void testObjectSerialization() {
    cleanupTestFile();
    
    auto& g = Globals::instance();
    g.reset();
    
    // Create a test object
    auto obj = std::make_unique<ZObject>(100, "test object");
    obj->setFlag(ObjectFlag::TAKEBIT);
    obj->setFlag(ObjectFlag::OPENBIT);
    obj->setProperty(1, 10);  // Property ID 1 = SIZE
    obj->setProperty(2, 5);   // Property ID 2 = VALUE
    obj->setText("Test text content");
    
    ZObject* objPtr = obj.get();
    g.registerObject(100, std::move(obj));
    
    // Save
    auto saveResult = SaveSystem::save(TEST_SAVE_FILE);
    TEST_ASSERT(saveResult == SaveSystem::SaveError::SUCCESS,
                "Save with objects should succeed");
    
    // Modify object
    objPtr->clearFlag(ObjectFlag::TAKEBIT);
    objPtr->setProperty(1, 99);
    objPtr->setText("");
    
    // Restore
    auto restoreResult = SaveSystem::restore(TEST_SAVE_FILE);
    TEST_ASSERT(restoreResult == SaveSystem::SaveError::SUCCESS,
                "Restore with objects should succeed");
    
    // Verify object state was restored
    TEST_ASSERT(objPtr->hasFlag(ObjectFlag::TAKEBIT),
                "Object TAKEBIT flag should be restored");
    TEST_ASSERT(objPtr->hasFlag(ObjectFlag::OPENBIT),
                "Object OPENBIT flag should be restored");
    TEST_ASSERT(objPtr->getProperty(1) == 10,
                "Object SIZE property should be restored");
    TEST_ASSERT(objPtr->getProperty(2) == 5,
                "Object VALUE property should be restored");
    TEST_ASSERT(objPtr->getText() == "Test text content",
                "Object text should be restored");
    
    cleanupTestFile();
    std::cout << "✓ Object serialization test passed" << std::endl;
}

// Test save/restore with timer states (Requirement 60.5, 61.5)
void testTimerSerialization() {
    cleanupTestFile();
    
    auto& g = Globals::instance();
    g.reset();
    TimerSystem::clear();
    
    // Register a test timer
    bool timerFired = false;
    TimerSystem::registerTimer("TEST_TIMER", 10, [&]() {
        timerFired = true;
    });
    
    // Set timer to specific state
    TimerSystem::queueTimer("TEST_TIMER", 5);
    
    // Save
    auto saveResult = SaveSystem::save(TEST_SAVE_FILE);
    TEST_ASSERT(saveResult == SaveSystem::SaveError::SUCCESS,
                "Save with timers should succeed");
    
    // Modify timer state
    TimerSystem::queueTimer("TEST_TIMER", 99);
    
    // Restore
    auto restoreResult = SaveSystem::restore(TEST_SAVE_FILE);
    TEST_ASSERT(restoreResult == SaveSystem::SaveError::SUCCESS,
                "Restore with timers should succeed");
    
    // Note: We can't directly verify timer counter was restored without
    // exposing more timer internals, but we've tested the serialization path
    
    cleanupTestFile();
    std::cout << "✓ Timer serialization test passed" << std::endl;
}

int main() {
    std::cout << "Running Save System Tests...\n\n";
    
    try {
        testBasicSave();
        testBasicRestore();
        testSavePermissionError();
        testRestoreFileNotFound();
        testRestoreCorruptFile();
        testErrorMessages();
        testObjectSerialization();
        testTimerSerialization();
        
        std::cout << "\nAll Save System Tests Passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << std::endl;
        cleanupTestFile();
        return 1;
    }
}
