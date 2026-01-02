cmake_minimum_required(VERSION 3.28)

project(towerforge VERSION 0.1.0 LANGUAGES CXX)

# Enable testing at the root level for CTest to work with presets
enable_testing()


# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)


# Subdirectories for modules
add_subdirectory(src/core)

# Main executable
set(SOURCE_FILES
    src/rendering/renderer.cpp
    src/rendering/camera.cpp
    src/ui/action_bar.cpp
    src/ui/hud/hud.cpp
    src/ui/hud/top_bar.cpp
    src/ui/hud/star_rating_panel.cpp
    src/ui/hud/end_game_summary.cpp
    src/ui/speed_control_panel.cpp
    src/ui/camera_controls_panel.cpp
    src/ui/build_menu.cpp
    src/ui/placement_system.cpp
    src/ui/history_panel.cpp
    src/ui/main_menu.cpp
    src/ui/pause_menu.cpp
    src/ui/save_load_menu.cpp
    src/ui/research_tree_menu.cpp
    src/ui/achievements_menu.cpp
    src/ui/general_settings_menu.cpp
    src/ui/audio_settings_menu.cpp
    src/ui/accessibility_settings_menu.cpp
    src/ui/tutorial_manager.cpp
    src/ui/mods_menu.cpp
    src/ui/ui_window.cpp
    src/ui/ui_window_manager.cpp
    src/ui/window_chrome.cpp
    src/ui/info_windows.cpp
    src/ui/section_header.cpp
    src/ui/analytics_overlay.cpp
    src/ui/tooltip.cpp
    src/ui/notification_center.cpp
    src/ui/ui_element.cpp
    src/ui/mouse_event_manager.cpp
    src/ui/help_system.cpp
    src/ui/batch_renderer/batch_renderer.cpp
    src/audio/audio_manager.cpp
)

add_executable(towerforge src/main.cpp ${SOURCE_FILES})

# Find and link dependencies
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(citrus-engine CONFIG REQUIRED)
target_link_libraries(towerforge PRIVATE raylib glfw towerforge_core citrus-engine::engine-core)

# Setup asset handling
setup_asset_preload(towerforge "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# Install target
install(TARGETS towerforge RUNTIME DESTINATION bin)

if (NOT EMSCRIPTEN)
    install(
        DIRECTORY assets/
        DESTINATION bin/assets
        PATTERN "*")
endif ()

# Screenshot application
add_executable(screenshot_app src/screenshot_app.cpp ${SOURCE_FILES})
target_link_libraries(screenshot_app PRIVATE raylib towerforge_core citrus-engine::engine-core)

# Test mods application
add_executable(test_mods src/test_mods.cpp)
target_link_libraries(test_mods PRIVATE towerforge_core citrus-engine::engine-core)

# Tests subdirectory
add_subdirectory(tests)

# Future modules (uncomment as needed)
# add_subdirectory(src/rendering)
# add_subdirectory(src/simulation)
# add_subdirectory(third_party)
