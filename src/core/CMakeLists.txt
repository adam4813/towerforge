# Core library - headless simulation module
add_library(towerforge_core
    ecs_world.cpp
    tower_grid.cpp
    facility_manager.cpp
    save_load_manager.cpp
    achievement_manager.cpp
    accessibility_settings.cpp
    user_preferences.cpp
    lua_mod_manager.cpp
    game.cpp
    command.cpp
    command_history.cpp
    scenes/title_scene.cpp
    scenes/achievements_scene.cpp
    scenes/settings_scene.cpp
    scenes/credits_scene.cpp
    scenes/ingame_scene.cpp
    scenes/tutorial_scene.cpp
    systems/time_systems.cpp
    systems/movement_systems.cpp
    systems/economy_systems.cpp
    systems/person_elevator_systems.cpp
    systems/visitor_employee_systems.cpp
    systems/facility_systems.cpp
    systems/staff_systems.cpp
)

find_package(game-engine CONFIG REQUIRED)

# Find flecs package
find_package(flecs CONFIG REQUIRED)

# Find nlohmann-json package
find_package(nlohmann_json CONFIG REQUIRED)

# Find Lua package
find_package(Lua REQUIRED)

# Find raylib package (needed for game.cpp which includes UI dependencies)
find_package(raylib CONFIG REQUIRED)

# Find glfw3 package (needed for rendering dependencies)
find_package(glfw3 CONFIG REQUIRED)

# Link flecs, nlohmann-json, Lua, raylib, and glfw to the core library
target_link_libraries(towerforge_core 
    PUBLIC game-engine::engine-core
    PUBLIC flecs::flecs_static
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC ${LUA_LIBRARIES}
    PUBLIC raylib
)

# Add Lua include directories
target_include_directories(towerforge_core
    PUBLIC ${LUA_INCLUDE_DIR}
)

# Include directories
target_include_directories(towerforge_core
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Set C++20 standard for the library
target_compile_features(towerforge_core PUBLIC cxx_std_20)
