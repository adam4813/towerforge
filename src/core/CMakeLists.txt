# Core library - headless simulation module
add_library(towerforge_core
    ecs_world.cpp
    tower_grid.cpp
    facility_manager.cpp
    save_load_manager.cpp
    achievement_manager.cpp
    lua_mod_manager.cpp
    game.cpp
    command.cpp
    command_history.cpp
)

# Find flecs package
find_package(flecs CONFIG REQUIRED)

# Find nlohmann-json package
find_package(nlohmann_json CONFIG REQUIRED)

# Find Lua package
find_package(Lua REQUIRED)

# Link flecs, nlohmann-json, and Lua to the core library
target_link_libraries(towerforge_core 
    PUBLIC flecs::flecs_static
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC ${LUA_LIBRARIES}
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
