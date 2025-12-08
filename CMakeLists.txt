cmake_minimum_required(VERSION 3.10)
project(Zork1)

set(CMAKE_CXX_STANDARD 20)
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
    
    # Interaction verb tests
    add_executable(interaction_verb_tests tests/interaction_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME InteractionVerbTests COMMAND interaction_verb_tests)
    
    # Consumption verb tests
    add_executable(consumption_verb_tests tests/consumption_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME ConsumptionVerbTests COMMAND consumption_verb_tests)
    
    # Light verb tests
    add_executable(light_verb_tests tests/light_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME LightVerbTests COMMAND light_verb_tests)
    
    # Special action verb tests
    add_executable(special_action_verb_tests tests/special_action_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME SpecialActionVerbTests COMMAND special_action_verb_tests)
    
    # Combat verb tests
    add_executable(combat_verb_tests tests/combat_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME CombatVerbTests COMMAND combat_verb_tests)
    
    # Meta-game verb tests
    add_executable(meta_game_verb_tests tests/meta_game_verb_tests.cpp ${LIB_SOURCES})
    add_test(NAME MetaGameVerbTests COMMAND meta_game_verb_tests)
    
    # Timer system tests
    add_executable(timer_system_tests tests/timer_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME TimerSystemTests COMMAND timer_system_tests)
    
    # Sword glow tests
    add_executable(sword_glow_tests tests/sword_glow_tests.cpp ${LIB_SOURCES})
    add_test(NAME SwordGlowTests COMMAND sword_glow_tests)
    
    # Light system tests
    add_executable(light_system_tests tests/light_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME LightSystemTests COMMAND light_system_tests)
    
    # Score system tests
    add_executable(score_system_tests tests/score_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME ScoreSystemTests COMMAND score_system_tests)
    
    # Combat system tests
    add_executable(combat_system_tests tests/combat_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME CombatSystemTests COMMAND combat_system_tests)
    
    # Death system tests
    add_executable(death_system_tests tests/death_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME DeathSystemTests COMMAND death_system_tests)
    
    # Save system tests
    add_executable(save_system_tests tests/save_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME SaveSystemTests COMMAND save_system_tests)
    
    # Object system tests
    add_executable(object_system_tests tests/object_system_tests.cpp ${LIB_SOURCES})
    add_test(NAME ObjectSystemTests COMMAND object_system_tests)
    
    # Parser comprehensive tests
    add_executable(parser_comprehensive_tests tests/parser_comprehensive_tests.cpp ${LIB_SOURCES})
    add_test(NAME ParserComprehensiveTests COMMAND parser_comprehensive_tests)
    
    # Verb edge case tests
    add_executable(verb_edge_case_tests tests/verb_edge_case_tests.cpp ${LIB_SOURCES})
    add_test(NAME VerbEdgeCaseTests COMMAND verb_edge_case_tests)
    
    # Integration tests
    add_executable(integration_tests tests/integration_tests.cpp ${LIB_SOURCES})
    add_test(NAME IntegrationTests COMMAND integration_tests)
    
    # Puzzle solvability tests (Task 70.1)
    add_executable(puzzle_solvability_tests tests/puzzle_solvability_tests.cpp ${LIB_SOURCES})
    add_test(NAME PuzzleSolvabilityTests COMMAND puzzle_solvability_tests)
    
    # Treasure collection tests (Task 70.2)
    add_executable(treasure_collection_tests tests/treasure_collection_tests.cpp ${LIB_SOURCES})
    add_test(NAME TreasureCollectionTests COMMAND treasure_collection_tests)
    
    # Unwinnable state tests (Task 70.3)
    add_executable(unwinnable_state_tests tests/unwinnable_state_tests.cpp ${LIB_SOURCES})
    add_test(NAME UnwinnableStateTests COMMAND unwinnable_state_tests)
    
    # Performance tests (Task 71)
    add_executable(performance_tests tests/performance_tests.cpp ${LIB_SOURCES})
    add_test(NAME PerformanceTests COMMAND performance_tests)
endif()
