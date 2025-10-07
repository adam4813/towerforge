/**
 * @file click_test_app.cpp
 * @brief Test application to demonstrate clicking on facilities to view info
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
    std::cout << "TowerForge - Click Test Demo" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(1200, 700, "TowerForge - Click to View Info Test");
    
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
    hud.AddNotification(Notification::Type::Success, "Click on facilities to view info!", 10.0f);
    
    // Place some example facilities
    facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 5);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 3);
    facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 7);
    
    // Grid rendering constants
    const int grid_offset_x = 300;
    const int grid_offset_y = 100;
    const int cell_width = 40;
    const int cell_height = 50;
    
    const float time_step = 1.0f / 60.0f;
    int frame_count = 0;
    const int max_frames = 300; // 5 seconds
    
    std::cout << "Starting render loop..." << std::endl;
    
    while (frame_count < max_frames && !renderer.ShouldClose()) {
        ecs_world.Update(time_step);
        placement_system.Update(time_step);
        
        game_state.current_time += time_step / 3600.0f;
        if (game_state.current_time >= 24.0f) {
            game_state.current_time -= 24.0f;
            game_state.current_day++;
        }
        hud.SetGameState(game_state);
        hud.Update(time_step);
        
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
            } else if (menu_result == -3) {
                placement_system.Undo();
            } else if (menu_result == -4) {
                placement_system.Redo();
            } else if (!hud.HandleClick(mouse_x, mouse_y)) {
                int cost_change = placement_system.HandleClick(mouse_x, mouse_y,
                    grid_offset_x, grid_offset_y, cell_width, cell_height, game_state.funds);
                
                if (cost_change != 0) {
                    game_state.funds += cost_change;
                } else {
                    // No placement - check if clicked on facility
                    int rel_x = mouse_x - grid_offset_x;
                    int rel_y = mouse_y - grid_offset_y;
                    
                    if (rel_x >= 0 && rel_y >= 0) {
                        int clicked_floor = rel_y / cell_height;
                        int clicked_column = rel_x / cell_width;
                        
                        if (clicked_floor >= 0 && clicked_floor < grid.GetFloorCount() &&
                            clicked_column >= 0 && clicked_column < grid.GetColumnCount()) {
                            
                            if (grid.IsOccupied(clicked_floor, clicked_column)) {
                                FacilityInfo info;
                                info.type = "FACILITY";
                                info.floor = clicked_floor;
                                info.occupancy = 5;
                                info.max_occupancy = 10;
                                info.revenue = 150.0f;
                                info.satisfaction = 85.0f;
                                info.tenant_count = 5;
                                hud.ShowFacilityInfo(info);
                                hud.AddNotification(Notification::Type::Success, 
                                    TextFormat("Viewing facility on floor %d", clicked_floor), 2.0f);
                            }
                        }
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
        
        placement_system.Render(grid_offset_x, grid_offset_y, cell_width, cell_height);
        
        hud.Render();
        build_menu.Render(placement_system.CanUndo(), placement_system.CanRedo(), 
                         placement_system.IsDemolishMode());
        
        DrawText("CLICK ON FACILITIES TO VIEW INFO", 300, 10, 20, GOLD);
        DrawText("Click placed facilities (colored blocks) to see their details", 
                 300, 35, 12, LIGHTGRAY);
        
        renderer.EndFrame();
        
        // Take screenshot after a short delay
        if (frame_count == 150) {
            std::cout << "Taking screenshot..." << std::endl;
            TakeScreenshot("/tmp/click_facility_demo.png");
            std::cout << "Screenshot saved!" << std::endl;
        }
        
        frame_count++;
    }
    
    renderer.Shutdown();
    
    std::cout << "Demo complete!" << std::endl;
    return 0;
}
