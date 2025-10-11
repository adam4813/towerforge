/**
 * @file ui_windows_screenshot.cpp
 * @brief Screenshot generation tool for TowerForge UI Window Manager
 */

#include <iostream>
#include "rendering/renderer.h"
#include "ui/hud.h"
#include "ui/ui_window_manager.h"
#include <raylib.h>

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - UI Window Manager Screenshot Generator" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(1200, 800, "TowerForge - UI Window Manager Demo");
    
    // Create HUD with window manager
    towerforge::ui::HUD hud;
    
    // Create sample facility info
    towerforge::ui::FacilityInfo facility1;
    facility1.type = "Office";
    facility1.floor = 3;
    facility1.occupancy = 15;
    facility1.max_occupancy = 20;
    facility1.revenue = 500.0f;
    facility1.satisfaction = 85.0f;
    facility1.tenant_count = 15;
    
    towerforge::ui::FacilityInfo facility2;
    facility2.type = "Restaurant";
    facility2.floor = 1;
    facility2.occupancy = 30;
    facility2.max_occupancy = 40;
    facility2.revenue = 800.0f;
    facility2.satisfaction = 92.0f;
    facility2.tenant_count = 8;
    
    // Create sample person info
    towerforge::ui::PersonInfo person1;
    person1.id = 42;
    person1.name = "John Visitor";
    person1.npc_type = "Visitor";
    person1.state = "WaitingElevator";
    person1.status = "Shopping";
    person1.current_floor = 2;
    person1.destination_floor = 5;
    person1.wait_time = 15.0f;
    person1.needs = "Food";
    person1.satisfaction = 78.0f;
    
    // Create sample elevator info
    towerforge::ui::ElevatorInfo elevator1;
    elevator1.id = 1;
    elevator1.current_floor = 4;
    elevator1.direction = "↑";
    elevator1.occupancy = 6;
    elevator1.max_occupancy = 8;
    elevator1.next_stop = 7;
    elevator1.queue = {{2, 3}, {5, 1}, {7, 2}};
    
    // Set game state
    towerforge::ui::GameState game_state;
    game_state.funds = 50000.0f;
    game_state.income_rate = 1200.0f;
    game_state.population = 150;
    game_state.current_day = 5;
    game_state.current_time = 14.5f;
    game_state.speed_multiplier = 2;
    game_state.paused = false;
    game_state.rating.stars = 3;
    game_state.rating.average_satisfaction = 82.0f;
    game_state.rating.total_tenants = 150;
    game_state.rating.total_floors = 10;
    game_state.rating.hourly_income = 1200.0f;
    game_state.rating.next_star_tenants = 200;
    game_state.rating.next_star_satisfaction = 85.0f;
    
    hud.SetGameState(game_state);
    
    // Add notifications
    hud.AddNotification(towerforge::ui::Notification::Type::Info, 
                       "Multiple windows can be open at once!", 8.0f);
    hud.AddNotification(towerforge::ui::Notification::Type::Success, 
                       "Click X button to close windows", 7.0f);
    
    // Simulate opening multiple windows
    bool windows_added = false;
    int frame_count = 0;
    
    std::cout << "Rendering UI with multiple windows..." << std::endl;
    
    while (!WindowShouldClose() && frame_count < 150) {
        // Add windows after certain frames to show them appearing
        if (frame_count == 30 && !windows_added) {
            std::cout << "  Adding facility window 1..." << std::endl;
            hud.ShowFacilityInfo(facility1);
        }
        if (frame_count == 45 && !windows_added) {
            std::cout << "  Adding person window..." << std::endl;
            hud.ShowPersonInfo(person1);
        }
        if (frame_count == 60 && !windows_added) {
            std::cout << "  Adding elevator window..." << std::endl;
            hud.ShowElevatorInfo(elevator1);
        }
        if (frame_count == 75 && !windows_added) {
            std::cout << "  Adding facility window 2..." << std::endl;
            hud.ShowFacilityInfo(facility2);
            windows_added = true;
        }
        
        hud.Update(1.0f / 60.0f);
        
        renderer.BeginFrame();
        renderer.Clear(DARKGRAY);
        
        // Draw a simple background grid to show the game area
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 20; x++) {
                int cell_x = 50 + x * 40;
                int cell_y = 100 + y * 50;
                DrawRectangleLines(cell_x, cell_y, 40, 50, ColorAlpha(WHITE, 0.2f));
            }
        }
        
        // Draw some facility rectangles to show the game  
        DrawRectangle(50 + 2 * 40 + 2, 100 + 3 * 50 + 2, 40 - 4, 50 - 4, SKYBLUE);
        DrawText("Office\nFloor 3", 50 + 2 * 40 + 5, 100 + 3 * 50 + 12, 8, WHITE);
        
        DrawRectangle(50 + 5 * 40 + 2, 100 + 1 * 50 + 2, 40 - 4, 50 - 4, GREEN);
        DrawText("Food\nFloor 1", 50 + 5 * 40 + 5, 100 + 1 * 50 + 12, 8, WHITE);
        
        // Draw a person
        DrawCircle(50 + 8 * 40 + 20, 100 + 2 * 50 + 25, 8, YELLOW);
        
        // Draw elevator shaft
        for (int f = 0; f < 8; f++) {
            DrawRectangle(50 + 12 * 40 + 10, 100 + f * 50 + 5, 20, 40, ColorAlpha(PURPLE, 0.3f));
        }
        // Draw elevator car at floor 4
        DrawRectangle(50 + 12 * 40 + 10, 100 + 4 * 50 + 5, 20, 40, PURPLE);
        
        // Render HUD with all windows
        hud.Render();
        
        // Draw instruction text
        if (frame_count < 120) {
            DrawText("Multiple info windows open simultaneously!", 10, 700, 20, YELLOW);
            DrawText("Windows cascade and support z-ordering", 10, 725, 16, LIGHTGRAY);
            DrawText("Click X to close • Click to bring to front", 10, 745, 16, LIGHTGRAY);
        }
        
        renderer.EndFrame();
        frame_count++;
        
        // Take screenshot at frame 120
        if (frame_count == 120) {
            const char* screenshot_path = "ui_window_manager_demo.png";
            TakeScreenshot(screenshot_path);
            std::cout << "Screenshot saved: " << screenshot_path << std::endl;
        }
    }
    
    std::cout << "Window manager test completed!" << std::endl;
    std::cout << "Total windows: " << hud.GetWindowManager()->GetWindowCount() << std::endl;
    
    renderer.Shutdown();
    return 0;
}
