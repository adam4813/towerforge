cmake_minimum_required(VERSION 3.20)

project(towerforge VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify the vcpkg toolchain if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Subdirectories for modules
add_subdirectory(src/core)

# Main executable
set(SOURCE_FILES
    src/rendering/renderer.cpp
    src/rendering/camera.cpp
    src/ui/hud.cpp
    src/ui/build_menu.cpp
    src/ui/placement_system.cpp
    src/ui/main_menu.cpp
    src/ui/pause_menu.cpp
    src/ui/save_load_menu.cpp
    src/ui/research_tree_menu.cpp
    src/ui/achievements_menu.cpp
    src/ui/general_settings_menu.cpp
    src/ui/audio_settings_menu.cpp
    src/ui/tutorial_manager.cpp
    src/ui/mods_menu.cpp
    src/ui/ui_window.cpp
    src/ui/ui_window_manager.cpp
    src/ui/info_windows.cpp
    src/ui/tooltip.cpp
    src/ui/ui_element.cpp
    src/ui/mouse_event_manager.cpp
    src/audio/audio_manager.cpp
)

add_executable(towerforge src/main.cpp ${SOURCE_FILES})

# Find and link dependencies
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(towerforge PRIVATE raylib glfw towerforge_core)

# Screenshot application
add_executable(screenshot_app src/screenshot_app.cpp ${SOURCE_FILES})
target_link_libraries(screenshot_app PRIVATE raylib glfw towerforge_core)

# Test mods application
add_executable(test_mods src/test_mods.cpp)
target_link_libraries(test_mods PRIVATE towerforge_core)

# Future modules (uncomment as needed)
# add_subdirectory(src/rendering)
# add_subdirectory(src/simulation)
# add_subdirectory(third_party)
