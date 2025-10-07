/**
 * @file screenshot_app.cpp
 * @brief Screenshot generation tool for TowerForge Tower Grid System documentation
 * 
 * USAGE INSTRUCTIONS FOR FUTURE MAINTENANCE:
 * ==========================================
 * 
 * WHEN TO UPDATE THIS TOOL:
 * ------------------------
 * - When the Tower Grid System API changes (new methods, different behavior)
 * - When adding new facility types to the system
 * - When the grid dimensions or layout need to be visualized differently
 * - When the visual style or color scheme needs updating
 * - When documentation screenshots need to be regenerated
 * 
 * HOW TO USE THIS TOOL:
 * --------------------
 * 1. Build the project:
 *    ```bash
 *    mkdir build && cd build
 *    cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
 *    cmake --build .
 *    ```
 * 
 * 2. Run with Xvfb (headless X server) to generate screenshots:
 *    ```bash
 *    xvfb-run -a ./bin/screenshot_app
 *    ```
 *    This creates: towerforge_screenshot.png in the current directory
 * 
 * 3. Copy the screenshot to the docs folder:
 *    ```bash
 *    cp towerforge_screenshot.png ../docs/grid_demo_screenshot.png
 *    ```
 * 
 * 4. Update the PR/README with the new screenshot if needed
 * 
 * HOW TO UPDATE THE VISUALIZATION:
 * --------------------------------
 * - Facility placement (lines ~90-100): Use facility_mgr.CreateFacility() calls
 *   to change which facilities are displayed
 * 
 * - Colors (lines ~140-165): Update the switch statement to change facility colors
 *   or add new facility types
 * 
 * - Grid layout (lines ~117-120): Modify grid_offset_x/y, cell_width/height to
 *   change the visual grid dimensions
 * 
 * - Legend (lines ~175-190): Update to reflect new facility types or colors
 * 
 * - Text/Labels (lines 157-173): Modify to update title, descriptions, or stats
 * 
 * TROUBLESHOOTING:
 * ---------------
 * - If screenshot is blank: Increase the frame count (line 110) to allow more
 *   rendering time
 * 
 * - If colors don't match: Verify the facility IDs in the switch statement
 *   match the IDs used in PlaceFacility() calls
 * 
 * - If running without Xvfb fails: Install xvfb-run or run on a system with
 *   an X server (display)
 * 
 * - If screenshot path fails: Check write permissions for /tmp or modify the
 *   path in line 180
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include <raylib.h>

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Screenshot Generator" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Facility System Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Get the tower grid and facility manager
    auto& grid = ecs_world.GetTowerGrid();
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create facilities using the FacilityManager
    auto lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0);
    auto office = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 2);
    auto residential = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 5);
    auto shop = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 1);
    auto restaurant = facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 4, 8);
    
    // Create some example actors (people)
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    
    auto actor2 = ecs_world.CreateEntity("Sarah");
    actor2.set<Position>({20.0f, 0.0f});
    actor2.set<Velocity>({-0.3f, 0.0f});
    actor2.set<Actor>({"Sarah", 3, 0.8f});
    
    // Render a few frames to ensure everything is drawn
    for (int i = 0; i < 5; i++) {
        ecs_world.Update(1.0f / 60.0f);
        
        renderer.BeginFrame();
        renderer.Clear(DARKGRAY);
        
        // Draw grid representation
        const int grid_offset_x = 50;
        const int grid_offset_y = 50;
        const int cell_width = 30;
        const int cell_height = 40;
        
        // Draw grid lines
        for (int floor = 0; floor < 6; floor++) {
            int y = grid_offset_y + floor * cell_height;
            DrawLine(grid_offset_x, y, grid_offset_x + 600, y, GRAY);
        }
        
        for (int col = 0; col <= 20; col++) {
            int x = grid_offset_x + col * cell_width;
            DrawLine(x, grid_offset_y, x, grid_offset_y + 5 * cell_height, GRAY);
        }
        
        // Draw occupied cells (facilities)
        for (int floor = 0; floor < 5; floor++) {
            for (int col = 0; col < 20; col++) {
                if (grid.IsOccupied(floor, col)) {
                    int x = grid_offset_x + col * cell_width + 2;
                    int y = grid_offset_y + floor * cell_height + 2;
                    
                    // Get facility type and color from facility manager
                    int facilityId = grid.GetFacilityAt(floor, col);
                    auto facilityType = facility_mgr.GetFacilityType(facilityId);
                    
                    Color color;
                    switch (facilityType) {
                        case BuildingComponent::Type::Lobby:
                            color = GOLD; break;
                        case BuildingComponent::Type::Office:
                            color = SKYBLUE; break;
                        case BuildingComponent::Type::Residential:
                            color = PURPLE; break;
                        case BuildingComponent::Type::RetailShop:
                            color = GREEN; break;
                        case BuildingComponent::Type::Restaurant:
                            color = RED; break;
                        case BuildingComponent::Type::Hotel:
                            color = DARKBLUE; break;
                        case BuildingComponent::Type::Elevator:
                            color = GRAY; break;
                        default:
                            color = SKYBLUE; break;
                    }
                    
                    DrawRectangle(x, y, cell_width - 4, cell_height - 4, color);
                }
            }
        }
        
        // Draw title and legend
        DrawText("TowerForge - Facility System", 50, 10, 24, WHITE);
        DrawText("Grid: 10 floors x 20 columns", 50, 280, 16, LIGHTGRAY);
        DrawText("Legend:", 50, 310, 16, WHITE);
        DrawRectangle(50, 335, 20, 15, GOLD);
        DrawText("Lobby", 80, 335, 14, WHITE);
        DrawRectangle(50, 355, 20, 15, SKYBLUE);
        DrawText("Office", 80, 355, 14, WHITE);
        DrawRectangle(50, 375, 20, 15, PURPLE);
        DrawText("Residential", 80, 375, 14, WHITE);
        DrawRectangle(50, 395, 20, 15, GREEN);
        DrawText("RetailShop", 80, 395, 14, WHITE);
        DrawRectangle(50, 415, 20, 15, RED);
        DrawText("Restaurant", 80, 415, 14, WHITE);
        
        // Draw info panel
        DrawText(TextFormat("Occupied cells: %d", grid.GetOccupiedCellCount()), 50, 450, 16, LIGHTGRAY);
        DrawText(TextFormat("Floors: %d | Columns: %d", grid.GetFloorCount(), grid.GetColumnCount()), 50, 470, 16, LIGHTGRAY);
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("/tmp/towerforge_screenshot.png");
    std::cout << "Screenshot saved to /tmp/towerforge_screenshot.png" << std::endl;
    
    // Cleanup
    renderer.Shutdown();
    
    return 0;
}
