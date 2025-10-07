# Core library - headless simulation module
add_library(towerforge_core
    ecs_world.cpp
    tower_grid.cpp
)

# Find flecs package
find_package(flecs CONFIG REQUIRED)

# Link flecs to the core library
target_link_libraries(towerforge_core 
    PUBLIC flecs::flecs_static
)

# Include directories
target_include_directories(towerforge_core
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Set C++20 standard for the library
target_compile_features(towerforge_core PUBLIC cxx_std_20)

# Test executable
add_executable(tower_grid_test tower_grid_test.cpp)
target_link_libraries(tower_grid_test PRIVATE towerforge_core)
target_compile_features(tower_grid_test PUBLIC cxx_std_20)

# Demo executable
add_executable(grid_demo grid_demo.cpp)
target_link_libraries(grid_demo PRIVATE towerforge_core)
target_compile_features(grid_demo PUBLIC cxx_std_20)
