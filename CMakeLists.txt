cmake_minimum_required(VERSION 3.10)
project(Zork1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test suite" OFF)

include_directories(src)

# Collect library sources (everything except main.cpp)
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Library target for game engine (compiled once, linked by all targets)
add_library(zork_core STATIC ${LIB_SOURCES})
target_include_directories(zork_core PUBLIC src)

# Main game executable
add_executable(zork1 src/main.cpp)
target_link_libraries(zork1 PRIVATE zork_core)

# Tests
if(BUILD_TESTS)
    enable_testing()
    
    macro(add_zork_test test_name target_name source_file)
        add_executable(${target_name} ${source_file})
        target_link_libraries(${target_name} PRIVATE zork_core)
        add_test(NAME ${test_name} COMMAND ${target_name})
    endmacro()

    # Basic tests
    add_zork_test(BasicTests basic_tests tests/basic_tests.cpp)
    
    # Transcript tests
    add_zork_test(TranscriptTests transcript_tests tests/transcript_tests.cpp)
    
    # Room navigation tests
    add_zork_test(RoomNavigationTests room_navigation_tests tests/room_navigation_tests.cpp)
    
    # World initialization tests
    add_zork_test(WorldInitializationTests world_initialization_tests tests/world_initialization_tests.cpp)
    
    # Examination verb tests
    add_zork_test(ExaminationVerbTests examination_verb_tests tests/examination_verb_tests.cpp)
    
    # Manipulation verb tests
    add_zork_test(ManipulationVerbTests manipulation_verb_tests tests/manipulation_verb_tests.cpp)
    
    # Interaction verb tests
    add_zork_test(InteractionVerbTests interaction_verb_tests tests/interaction_verb_tests.cpp)
    
    # Consumption verb tests
    add_zork_test(ConsumptionVerbTests consumption_verb_tests tests/consumption_verb_tests.cpp)
    
    # Light verb tests
    add_zork_test(LightVerbTests light_verb_tests tests/light_verb_tests.cpp)
    
    # Special action verb tests
    add_zork_test(SpecialActionVerbTests special_action_verb_tests tests/special_action_verb_tests.cpp)
    
    # Combat verb tests
    add_zork_test(CombatVerbTests combat_verb_tests tests/combat_verb_tests.cpp)
    
    # Meta-game verb tests
    add_zork_test(MetaGameVerbTests meta_game_verb_tests tests/meta_game_verb_tests.cpp)
    
    # Timer system tests
    add_zork_test(TimerSystemTests timer_system_tests tests/timer_system_tests.cpp)
    
    # Sword glow tests
    add_zork_test(SwordGlowTests sword_glow_tests tests/sword_glow_tests.cpp)
    
    # Light system tests
    add_zork_test(LightSystemTests light_system_tests tests/light_system_tests.cpp)
    
    # Score system tests
    add_zork_test(ScoreSystemTests score_system_tests tests/score_system_tests.cpp)
    
    # Combat system tests
    add_zork_test(CombatSystemTests combat_system_tests tests/combat_system_tests.cpp)
    
    # Death system tests
    add_zork_test(DeathSystemTests death_system_tests tests/death_system_tests.cpp)
    
    # Save system tests
    add_zork_test(SaveSystemTests save_system_tests tests/save_system_tests.cpp)
    
    # Object system tests
    add_zork_test(ObjectSystemTests object_system_tests tests/object_system_tests.cpp)
    
    # Parser comprehensive tests
    add_zork_test(ParserComprehensiveTests parser_comprehensive_tests tests/parser_comprehensive_tests.cpp)
    
    # Verb edge case tests
    add_zork_test(VerbEdgeCaseTests verb_edge_case_tests tests/verb_edge_case_tests.cpp)
    
    # Inventory tests
    add_zork_test(InventoryTests inventory_tests tests/inventory_tests.cpp)

    # Meta-game additional tests
    add_zork_test(MetaGameAdditionalTests meta_game_additional_tests tests/meta_game_additional_tests.cpp)
    
    # Synonym tests
    add_zork_test(SynonymTests synonym_tests tests/synonym_tests.cpp)

    # Complex syntax tests
    add_zork_test(ComplexSyntaxTests complex_syntax_tests tests/complex_syntax_tests.cpp)

    # Game Command Tests
    add_zork_test(GameCommandTests game_command_tests tests/game_command_tests.cpp)

    # Complex Syntax Take Tests
    add_zork_test(ComplexSyntaxTakeTests complex_syntax_take_tests tests/complex_syntax_take_tests.cpp)

    # Complex Syntax Give Tests
    add_zork_test(ComplexSyntaxGiveTests complex_syntax_give_tests tests/complex_syntax_give_tests.cpp)

    # Complex Syntax Attack Tests
    add_zork_test(ComplexSyntaxAttackTests complex_syntax_attack_tests tests/complex_syntax_attack_tests.cpp)
    
    # Complex Syntax Unlock Tests
    add_zork_test(ComplexSyntaxUnlockTests complex_syntax_unlock_tests tests/complex_syntax_unlock_tests.cpp)
    
    # Integration tests
    add_zork_test(IntegrationTests integration_tests tests/integration_tests.cpp)
    
    # Puzzle solvability tests (Task 70.1)
    add_zork_test(PuzzleSolvabilityTests puzzle_solvability_tests tests/puzzle_solvability_tests.cpp)
    
    # Treasure collection tests (Task 70.2)
    add_zork_test(TreasureCollectionTests treasure_collection_tests tests/treasure_collection_tests.cpp)
    
    # Unwinnable state tests (Task 70.3)
    add_zork_test(UnwinnableStateTests unwinnable_state_tests tests/unwinnable_state_tests.cpp)
    
    # Performance tests (Task 71)
    add_zork_test(PerformanceTests performance_tests tests/performance_tests.cpp)

    # Trophy Case tests
    add_zork_test(TrophyCaseTest trophy_case_test tests/trophy_case_test.cpp)

    # Diagnose tests
    add_zork_test(DiagnoseTest diagnose_test tests/diagnose_test.cpp)

    # Critical Puzzles Tests (Egg, Canary, Basket)
    add_zork_test(CriticalPuzzlesTests critical_puzzles_tests tests/critical_puzzles_tests.cpp)

    # Action Tests (ZIL action routines)
    add_zork_test(ActionTests action_tests tests/action_tests.cpp)
    
    # PAINTING-FCN Tests (ZIL action verification)
    add_zork_test(PaintingTests painting_tests tests/painting_tests.cpp)
    
    # PUTTY-FCN Tests (ZIL action verification)
    add_zork_test(PuttyTests putty_tests tests/putty_tests.cpp)
    
    # RAINBOW-FCN Tests (ZIL action verification)
    add_zork_test(RainbowTests rainbow_tests tests/rainbow_tests.cpp)

    # RBOAT-FUNCTION comprehensive tests
    add_zork_test(RBoatTests rboat_tests tests/rboat_tests.cpp)

    # Parser Flag Tests (All ZIL bits verification)
    add_zork_test(ParserFlagTests parser_flag_tests tests/parser_flag_tests.cpp)

    # RIVER-FUNCTION Tests (ZIL action verification)
    add_zork_test(RiverTests river_tests tests/river_tests.cpp)

    # NAILS-PSEUDO Tests (ZIL action verification)
    add_zork_test(NailsPseudoTests nails_pseudo_tests tests/nails_pseudo_tests.cpp)

    # LAKE-PSEUDO Tests (ZIL action verification)
    add_zork_test(LakePseudoTests lake_pseudo_tests tests/lake_pseudo_tests.cpp)

    # GMAIN Tests (gmain.zil routines verification)
    add_zork_test(GMainTests gmain_tests tests/gmain_tests.cpp)

    # GCLOCK Tests (gclock.zil routines verification)
    add_zork_test(GClockTests gclock_tests tests/gclock_tests.cpp)

    # GVERBS Tests (gverbs.zil routines and preactions verification)
    add_zork_test(GVerbsTests gverbs_tests tests/gverbs_tests.cpp)

    # GMACROS Tests (gmacros.zil macros and routines verification)
    add_zork_test(GMacrosTests gmacros_tests tests/gmacros_tests.cpp)

    # GPARSER Tests (gparser.zil routines and tables verification)
    add_zork_test(GParserTests gparser_tests tests/gparser_tests.cpp)

    # GSYNTAX Tests (gsyntax.zil syntax declarations, synonyms, buzzwords)
    add_zork_test(GSyntaxTests gsyntax_tests tests/gsyntax_tests.cpp)

    # GGLOBALS Tests (gglobals.zil routines, variables, and global objects)
    add_zork_test(GGlobalsTests gglobals_tests tests/gglobals_tests.cpp)

    # DUNGEON Tests (1dungeon.zil rooms, objects, exits, tables, and routines)
    add_zork_test(DungeonTests dungeon_tests tests/dungeon_tests.cpp)
endif()

