/**
 * @file hud_demo.cpp
 * @brief Demo application showcasing the HUD and Information Display System
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - HUD System Demo" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(1200, 800, "TowerForge - HUD System Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create HUD and build menu
    HUD hud;
    BuildMenu build_menu;
    
    // Set initial game state
    GameState game_state;
    game_state.funds = 25000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 125;
    game_state.current_day = 5;
    game_state.current_time = 8.5f;  // 8:30 AM
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    
    hud.SetGameState(game_state);
    
    // Add notifications
    hud.AddNotification(Notification::Type::Warning, "Low satisfaction on Floor 3", 20.0f);
    hud.AddNotification(Notification::Type::Success, "Milestone: 100 population", 20.0f);
    hud.AddNotification(Notification::Type::Success, "Income milestone: $50K", 20.0f);
    
    // Show facility info panel by default
    FacilityInfo facility_info;
    facility_info.type = "OFFICE";
    facility_info.floor = 5;
    facility_info.occupancy = 8;
    facility_info.max_occupancy = 10;
    facility_info.revenue = 80.0f;
    facility_info.satisfaction = 85.0f;
    facility_info.tenant_count = 8;
    hud.ShowFacilityInfo(facility_info);
    
    // Get the tower grid
    auto& grid = ecs_world.GetTowerGrid();
    
    // Place facilities on the grid
    grid.PlaceFacility(0, 0, 10, 1);   // Lobby on floor 0
    grid.PlaceFacility(1, 2, 8, 2);    // Office on floor 1
    grid.PlaceFacility(2, 5, 6, 3);    // Restaurant on floor 2
    grid.PlaceFacility(3, 1, 4, 4);    // Shop on floor 3
    grid.PlaceFacility(4, 8, 10, 5);   // Hotel on floor 4
    
    std::cout << "HUD Demo initialized. Press ESC or close window to exit." << std::endl;
    std::cout << "Click on elements to interact:" << std::endl;
    std::cout << "  - Build menu: Select facility types" << std::endl;
    std::cout << "  - Speed controls: Change simulation speed" << std::endl;
    std::cout << "  - Grid cells: View facility info" << std::endl;
    std::cout << "  - Press 1: Show Facility Info" << std::endl;
    std::cout << "  - Press 2: Show Person Info" << std::endl;
    std::cout << "  - Press 3: Show Elevator Info" << std::endl;
    std::cout << "  - Press 4: Hide all info panels" << std::endl;
    std::cout << "  - Press N: Add notification" << std::endl;
    std::cout << "  - Press SPACE: Toggle pause" << std::endl;
    
    const float time_step = 1.0f / 60.0f;  // 60 FPS
    float elapsed_time = 0.0f;
    float sim_time = 0.0f;
    int notification_counter = 0;
    
    while (!renderer.ShouldClose()) {
        if (!game_state.paused) {
            if (!ecs_world.Update(time_step)) {
                break;
            }
            sim_time += time_step * game_state.speed_multiplier;
        }
        elapsed_time += time_step;
        
        // Update game state for HUD
        game_state.current_time = 8.5f + (sim_time / 3600.0f);
        if (game_state.current_time >= 24.0f) {
            game_state.current_time -= 24.0f;
            game_state.current_day++;
        }
        game_state.funds += (game_state.income_rate / 3600.0f) * time_step;
        
        hud.SetGameState(game_state);
        hud.Update(time_step);
        
        // Handle keyboard input
        if (IsKeyPressed(KEY_ONE)) {
            hud.ShowFacilityInfo(facility_info);
            std::cout << "Showing Facility Info Panel" << std::endl;
        }
        if (IsKeyPressed(KEY_TWO)) {
            PersonInfo person_info;
            person_info.id = 42;
            person_info.state = "WaitingElevator";
            person_info.current_floor = 1;
            person_info.destination_floor = 8;
            person_info.wait_time = 45.0f;
            person_info.needs = "Work";
            person_info.satisfaction = 60.0f;
            hud.ShowPersonInfo(person_info);
            std::cout << "Showing Person Info Panel" << std::endl;
        }
        if (IsKeyPressed(KEY_THREE)) {
            ElevatorInfo elevator_info;
            elevator_info.id = 1;
            elevator_info.current_floor = 5;
            elevator_info.direction = "UP";
            elevator_info.occupancy = 6;
            elevator_info.max_occupancy = 8;
            elevator_info.next_stop = 8;
            elevator_info.queue.push_back({1, 2});
            elevator_info.queue.push_back({3, 1});
            hud.ShowElevatorInfo(elevator_info);
            std::cout << "Showing Elevator Info Panel" << std::endl;
        }
        if (IsKeyPressed(KEY_FOUR)) {
            hud.HideInfoPanels();
            std::cout << "Hiding all info panels" << std::endl;
        }
        if (IsKeyPressed(KEY_N)) {
            notification_counter++;
            hud.AddNotification(Notification::Type::Info, 
                              "Test notification #" + std::to_string(notification_counter), 5.0f);
            std::cout << "Added notification" << std::endl;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            game_state.paused = !game_state.paused;
            std::cout << (game_state.paused ? "Paused" : "Resumed") << std::endl;
        }
        
        // Handle mouse clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            
            // Check if click is on build menu
            int selected = build_menu.HandleClick(mouse_x, mouse_y);
            if (selected >= 0) {
                const auto& types = build_menu.GetFacilityTypes();
                hud.AddNotification(Notification::Type::Info, 
                                  "Selected: " + types[selected].name, 3.0f);
                std::cout << "Selected facility: " << types[selected].name << std::endl;
            }
            // Check if click is on HUD (speed controls)
            else if (!hud.HandleClick(mouse_x, mouse_y)) {
                // Click in game area
                std::cout << "Click at (" << mouse_x << ", " << mouse_y << ")" << std::endl;
            }
        }
        
        renderer.BeginFrame();
        
        // Clear background to dark gray
        renderer.Clear(DARKGRAY);
        
        // Draw grid representation
        const int grid_offset_x = 250;
        const int grid_offset_y = 100;
        const int cell_width = 40;
        const int cell_height = 50;
        
        // Draw grid lines
        for (int floor = 0; floor < 6; floor++) {
            int y = grid_offset_y + floor * cell_height;
            DrawLine(grid_offset_x, y, grid_offset_x + 800, y, GRAY);
        }
        
        for (int col = 0; col <= 20; col++) {
            int x = grid_offset_x + col * cell_width;
            DrawLine(x, grid_offset_y, x, grid_offset_y + 5 * cell_height, GRAY);
        }
        
        // Draw facilities
        for (int floor = 0; floor < 5; floor++) {
            for (int col = 0; col < 20; col++) {
                int facility_id = grid.GetFacilityAt(floor, col);
                if (facility_id > 0) {
                    int x = grid_offset_x + col * cell_width + 2;
                    int y = grid_offset_y + floor * cell_height + 2;
                    
                    Color color;
                    switch (facility_id) {
                        case 1: color = GOLD; break;        // Lobby
                        case 2: color = SKYBLUE; break;     // Office
                        case 3: color = RED; break;         // Restaurant
                        case 4: color = GREEN; break;       // Shop
                        case 5: color = PURPLE; break;      // Hotel
                        default: color = WHITE; break;
                    }
                    
                    DrawRectangle(x, y, cell_width - 4, cell_height - 4, color);
                }
            }
        }
        
        // Draw some people (circles)
        DrawCircle(grid_offset_x + 100, grid_offset_y + 25, 8, YELLOW);
        DrawCircle(grid_offset_x + 200, grid_offset_y + 75, 8, YELLOW);
        DrawCircle(grid_offset_x + 350, grid_offset_y + 125, 8, YELLOW);
        
        // Draw title
        DrawText("TowerForge - HUD System Demo", 250, 20, 24, WHITE);
        DrawText("Press 1-4 to show/hide panels | N for notifications | SPACE to pause", 
                 250, 50, 14, LIGHTGRAY);
        
        // Render HUD and build menu
        hud.Render();
        build_menu.Render();
        
        renderer.EndFrame();
    }
    
    // Cleanup
    renderer.Shutdown();
    
    std::cout << std::endl << "HUD Demo completed" << std::endl;
    return 0;
}
