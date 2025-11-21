#pragma once
#include <string>
#include <vector>
#include <functional>
#include <iostream>

// Simple test framework for Zork
class TestFramework {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
    };
    
    static TestFramework& instance() {
        static TestFramework inst;
        return inst;
    }
    
    void addTest(const std::string& name, std::function<void()> test) {
        tests_.push_back({name, test});
    }
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        
        for (const auto& test : tests_) {
            try {
                test.func();
                results.push_back({test.name, true, ""});
                std::cout << "✓ " << test.name << std::endl;
            } catch (const std::exception& e) {
                results.push_back({test.name, false, e.what()});
                std::cout << "✗ " << test.name << ": " << e.what() << std::endl;
            }
        }
        
        return results;
    }
    
private:
    struct Test {
        std::string name;
        std::function<void()> func;
    };
    
    std::vector<Test> tests_;
};

// Test assertion macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) throw std::runtime_error("Assertion failed: " #condition)

#define ASSERT_FALSE(condition) \
    if (condition) throw std::runtime_error("Assertion failed: !" #condition)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " == " #b)

#define ASSERT_CONTAINS(str, substr) \
    if ((str).find(substr) == std::string::npos) \
        throw std::runtime_error("String does not contain: " + std::string(substr))

// Test registration macro
#define TEST(name) \
    void test_##name(); \
    struct TestRegistrar_##name { \
        TestRegistrar_##name() { \
            TestFramework::instance().addTest(#name, test_##name); \
        } \
    } registrar_##name; \
    void test_##name()
