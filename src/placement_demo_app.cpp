/**
 * @file placement_demo_app.cpp
 * @brief Demonstration application for the Interactive Building and Placement System
 * 
 * This app demonstrates:
 * - Mouse-driven facility placement
 * - Placement preview with green/red validation
 * - Build menu with facility selection
 * - Demolish mode
 * - Undo/redo functionality
 * - Construction progress visualization
 * - Keyboard shortcuts
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Placement System Demo" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(1200, 700, "TowerForge - Interactive Building & Placement Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    ecs_world.GetWorld().set<TimeManager>({1.0f});
    ecs_world.GetWorld().set<TowerEconomy>({50000.0f});
    
    // Get the tower grid and facility manager
    auto& grid = ecs_world.GetTowerGrid();
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create HUD and build menu
    HUD hud;
    BuildMenu build_menu;
    PlacementSystem placement_system(grid, facility_mgr, build_menu);
    
    // Set initial game state
    GameState game_state;
    game_state.funds = 50000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 0;
    game_state.current_day = 1;
    game_state.current_time = 9.0f;
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    
    hud.SetGameState(game_state);
    hud.AddNotification(Notification::Type::Success, "Welcome! Click facilities to build", 5.0f);
    hud.AddNotification(Notification::Type::Info, "Press D for demolish mode", 5.0f);
    
    // Place a few example facilities to start
    facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 5);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 3);
    
    // Grid rendering constants
    const int grid_offset_x = 300;
    const int grid_offset_y = 100;
    const int cell_width = 40;
    const int cell_height = 50;
    
    const float time_step = 1.0f / 60.0f;
    int frame_count = 0;
    const int max_frames = 600; // 10 seconds at 60 FPS
    
    std::cout << "Starting render loop..." << std::endl;
    
    while (frame_count < max_frames && !renderer.ShouldClose()) {
        ecs_world.Update(time_step);
        
        // Update placement system
        placement_system.Update(time_step);
        
        // Update HUD
        game_state.current_time += time_step / 3600.0f;
        if (game_state.current_time >= 24.0f) {
            game_state.current_time -= 24.0f;
            game_state.current_day++;
        }
        hud.SetGameState(game_state);
        hud.Update(time_step);
        
        // Handle keyboard
        placement_system.HandleKeyboard();
        
        // Handle mouse clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            
            int menu_result = build_menu.HandleClick(mouse_x, mouse_y, 
                                                     placement_system.CanUndo(), 
                                                     placement_system.CanRedo());
            if (menu_result >= 0) {
                hud.AddNotification(Notification::Type::Info, "Facility selected", 2.0f);
            } else if (menu_result == -2) {
                placement_system.SetDemolishMode(!placement_system.IsDemolishMode());
                hud.AddNotification(Notification::Type::Info, 
                    placement_system.IsDemolishMode() ? "Demolish ON" : "Demolish OFF", 2.0f);
            } else if (menu_result == -3) {
                placement_system.Undo();
                hud.AddNotification(Notification::Type::Info, "Undo", 1.0f);
            } else if (menu_result == -4) {
                placement_system.Redo();
                hud.AddNotification(Notification::Type::Info, "Redo", 1.0f);
            } else if (!hud.HandleClick(mouse_x, mouse_y)) {
                int cost_change = placement_system.HandleClick(mouse_x, mouse_y,
                    grid_offset_x, grid_offset_y, cell_width, cell_height, game_state.funds);
                
                if (cost_change != 0) {
                    game_state.funds += cost_change;
                    if (cost_change < 0) {
                        hud.AddNotification(Notification::Type::Success, 
                            TextFormat("Placed! -$%d", -cost_change), 2.0f);
                    } else {
                        hud.AddNotification(Notification::Type::Info, 
                            TextFormat("Demolished! +$%d", cost_change), 2.0f);
                    }
                }
            }
        }
        
        // Render
        renderer.BeginFrame();
        renderer.Clear(DARKGRAY);
        
        // Draw grid
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            for (int col = 0; col < grid.GetColumnCount(); ++col) {
                int x = grid_offset_x + col * cell_width;
                int y = grid_offset_y + floor * cell_height;
                
                DrawRectangleLines(x, y, cell_width, cell_height, ColorAlpha(WHITE, 0.2f));
                
                if (grid.IsOccupied(floor, col)) {
                    int facility_id = grid.GetFacilityAt(floor, col);
                    Color facility_color = SKYBLUE;
                    if (facility_id % 4 == 0) facility_color = GOLD;
                    else if (facility_id % 4 == 1) facility_color = SKYBLUE;
                    else if (facility_id % 4 == 2) facility_color = PURPLE;
                    else facility_color = GREEN;
                    
                    DrawRectangle(x + 2, y + 2, cell_width - 4, cell_height - 4, facility_color);
                }
            }
        }
        
        // Draw floor labels
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            int y = grid_offset_y + floor * cell_height;
            DrawText(TextFormat("F%d", floor), grid_offset_x - 30, y + 15, 12, LIGHTGRAY);
        }
        
        // Render placement system
        placement_system.Render(grid_offset_x, grid_offset_y, cell_width, cell_height);
        
        // Render UI
        hud.Render();
        build_menu.Render(placement_system.CanUndo(), placement_system.CanRedo(), 
                         placement_system.IsDemolishMode());
        
        // Draw title
        DrawText("INTERACTIVE BUILDING & PLACEMENT SYSTEM", 220, 10, 20, GOLD);
        DrawText("Click facilities to build | Press D for demolish | Ctrl+Z/Y for undo/redo", 
                 220, 35, 12, LIGHTGRAY);
        
        renderer.EndFrame();
        
        // Take screenshot at frame 300 (5 seconds in)
        if (frame_count == 300) {
            std::cout << "Taking screenshot..." << std::endl;
            TakeScreenshot("/tmp/placement_system_demo.png");
            std::cout << "Screenshot saved to /tmp/placement_system_demo.png" << std::endl;
        }
        
        frame_count++;
    }
    
    renderer.Shutdown();
    
    std::cout << "Demo complete! Screenshot saved." << std::endl;
    return 0;
}
