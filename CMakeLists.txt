cmake_minimum_required(VERSION 3.10)
project(Zork1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test suite" OFF)

include_directories(src)

# Main game executable
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(zork1 ${SOURCES})

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    # Collect library sources (everything except main.cpp)
    file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
    list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    
    # Basic tests
    add_executable(basic_tests tests/basic_tests.cpp ${LIB_SOURCES})
    add_test(NAME BasicTests COMMAND basic_tests)
    
    # Transcript tests
    add_executable(transcript_tests tests/transcript_tests.cpp ${LIB_SOURCES})
    add_test(NAME TranscriptTests COMMAND transcript_tests)
    
    # Room navigation tests
    add_executable(room_navigation_tests tests/room_navigation_tests.cpp ${LIB_SOURCES})
    add_test(NAME RoomNavigationTests COMMAND room_navigation_tests)
    
    # World initialization tests
    add_executable(world_initialization_tests tests/world_initialization_tests.cpp ${LIB_SOURCES})
    add_test(NAME WorldInitializationTests COMMAND world_initialization_tests)
    
    # Examination verb tests
    add_executable(examination_verb_tests tests/examination_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME ExaminationVerbTests COMMAND examination_verb_tests)
    
    # Manipulation verb tests
    add_executable(manipulation_verb_tests tests/manipulation_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME ManipulationVerbTests COMMAND manipulation_verb_tests)
endif()
