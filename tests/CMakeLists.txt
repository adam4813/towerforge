# Test configuration for TowerForge
cmake_minimum_required(VERSION 3.20)

# Enable testing
enable_testing()

# Find GTest package
find_package(GTest CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create a test-specific core library without game.cpp and UI dependencies
add_library(towerforge_core_test
    ${CMAKE_SOURCE_DIR}/src/core/ecs_world.cpp
    ${CMAKE_SOURCE_DIR}/src/core/tower_grid.cpp
    ${CMAKE_SOURCE_DIR}/src/core/facility_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/save_load_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/achievement_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/accessibility_settings.cpp
    ${CMAKE_SOURCE_DIR}/src/core/user_preferences.cpp
    ${CMAKE_SOURCE_DIR}/src/core/lua_mod_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/command.cpp
    ${CMAKE_SOURCE_DIR}/src/core/command_history.cpp
    # Explicitly exclude game.cpp which has UI dependencies
)

# Find required packages for test core library
find_package(flecs CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Lua REQUIRED)

# Link dependencies to test core library
target_link_libraries(towerforge_core_test 
    PUBLIC flecs::flecs_static
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC ${LUA_LIBRARIES}
)

# Include directories for test core library
target_include_directories(towerforge_core_test
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PUBLIC ${LUA_INCLUDE_DIR}
)

# Set C++20 standard for test core library
target_compile_features(towerforge_core_test PUBLIC cxx_std_20)

# Helper function to add a test executable
function(add_test_executable test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} PRIVATE 
        GTest::gtest 
        GTest::gtest_main
        towerforge_core_test
    )
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Integration tests
add_test_executable(test_tower_grid_integration integration/test_tower_grid_integration.cpp)
add_test_executable(test_facility_manager_integration integration/test_facility_manager_integration.cpp)
add_test_executable(test_ecs_world_integration integration/test_ecs_world_integration.cpp)
add_test_executable(test_save_load_integration integration/test_save_load_integration.cpp)
add_test_executable(test_achievement_manager_integration integration/test_achievement_manager_integration.cpp)
add_test_executable(test_lua_mod_manager_integration integration/test_lua_mod_manager_integration.cpp)

# E2E tests
add_test_executable(test_game_initialization_e2e e2e/test_game_initialization_e2e.cpp)
add_test_executable(test_facility_placement_workflow_e2e e2e/test_facility_placement_workflow_e2e.cpp)
add_test_executable(test_save_load_workflow_e2e e2e/test_save_load_workflow_e2e.cpp)

# Unit tests
add_test_executable(test_user_preferences_unit unit/test_user_preferences_unit.cpp)
add_test_executable(test_command_history_unit unit/test_command_history_unit.cpp)
add_test_executable(test_accessibility_settings_unit unit/test_accessibility_settings_unit.cpp)
