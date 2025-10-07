#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Tower Simulation Game" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Initializing Raylib renderer..." << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - 2D Vector Rendering Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create the global TimeManager as a singleton
    // Start at 8:00 AM on Monday, Week 1
    // Simulation runs at 60x speed (60 in-game hours per real-time second)
    ecs_world.GetWorld().set<TimeManager>({60.0f});
    
    std::cout << std::endl << "Creating example entities..." << std::endl;    
    std::cout << "Renderer initialized. Window opened." << std::endl;
    std::cout << "Press ESC or close window to exit." << std::endl;
    std::cout << std::endl;
    
    // Create some example actors (people)
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    
    // Add a daily schedule for John
    DailySchedule john_schedule;
    john_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 9.0f);   // 9:00 AM
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.0f);  // 12:00 PM
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 17.0f);   // 5:00 PM
    john_schedule.AddWeekendAction(ScheduledAction::Type::Idle, 10.0f);        // Idle on weekends
    actor1.set<DailySchedule>(john_schedule);
    
    auto actor2 = ecs_world.CreateEntity("Sarah");
    actor2.set<Position>({20.0f, 0.0f});
    actor2.set<Velocity>({-0.3f, 0.0f});
    actor2.set<Actor>({"Sarah", 3, 0.8f});
    
    // Add a daily schedule for Sarah
    DailySchedule sarah_schedule;
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 8.5f);  // 8:30 AM
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.5f); // 12:30 PM
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 16.5f);  // 4:30 PM
    sarah_schedule.AddWeekendAction(ScheduledAction::Type::Idle, 11.0f);       // Idle on weekends
    actor2.set<DailySchedule>(sarah_schedule);
    
    // Create some example building components
    auto lobby = ecs_world.CreateEntity("Lobby");
    lobby.set<Position>({0.0f, 0.0f});
    lobby.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    
    auto office1 = ecs_world.CreateEntity("Office_Floor_5");
    office1.set<Position>({0.0f, 50.0f});
    office1.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 8, 20});
    
    auto restaurant = ecs_world.CreateEntity("Restaurant_Floor_3");
    restaurant.set<Position>({0.0f, 30.0f});
    restaurant.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 6, 30});
    
    std::cout << "  Created 2 actors and 3 building components" << std::endl;
    
    // Demonstrate Tower Grid System
    std::cout << std::endl << "Demonstrating Tower Grid System..." << std::endl;
    auto& grid = ecs_world.GetTowerGrid();
    
    std::cout << "  Initial grid: " << grid.GetFloorCount() << " floors x " 
              << grid.GetColumnCount() << " columns" << std::endl;
    
    // Place facilities on the grid
    std::cout << "  Placing facilities on grid..." << std::endl;
    grid.PlaceFacility(0, 0, 10, 1);   // Lobby on floor 0
    grid.PlaceFacility(1, 2, 8, 2);    // Office on floor 1
    grid.PlaceFacility(2, 5, 6, 3);    // Restaurant on floor 2
    grid.PlaceFacility(3, 1, 4, 4);    // Shop on floor 3
    
    std::cout << "  Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << "  Facility at (0, 0): " << grid.GetFacilityAt(0, 0) << std::endl;
    std::cout << "  Facility at (1, 5): " << grid.GetFacilityAt(1, 5) << std::endl;
    std::cout << "  Space available at floor 1, column 12 for width 5: " 
              << (grid.IsSpaceAvailable(1, 12, 5) ? "Yes" : "No") << std::endl;
    
    // Add more floors
    std::cout << "  Adding 5 more floors..." << std::endl;
    grid.AddFloors(5);
    std::cout << "  New grid size: " << grid.GetFloorCount() << " floors x " 
              << grid.GetColumnCount() << " columns" << std::endl;
    
    std::cout << std::endl << "Running simulation..." << std::endl;
    std::cout << std::endl;
    
    // Run the simulation for 30 seconds (simulated time)
    const float time_step = 1.0f / 60.0f;  // 60 FPS
    const float total_time = 30.0f;
    float elapsed_time = 0.0f;
    
    while (elapsed_time < total_time && !renderer.ShouldClose()) {
        if (!ecs_world.Update(time_step)) {
            break;
        }
        elapsed_time += time_step;
        renderer.BeginFrame();
        
        // Clear background to dark gray
        renderer.Clear(DARKGRAY);
        
        // Draw a test rectangle (representing a building floor)
        renderer.DrawRectangle(250, 200, 300, 80, SKYBLUE);
        
        // Draw a test circle (representing a person or elevator)
        renderer.DrawCircle(400, 400, 30.0f, RED);
        
        // Display current simulation time
        const auto& time_mgr = ecs_world.GetWorld().get<TimeManager>();
        // Draw time display in top-left corner
        std::string time_str = "Time: " + time_mgr.GetTimeString();
        std::string day_str = std::string("Day: ") + time_mgr.GetDayName();
        std::string week_str = "Week: " + std::to_string(time_mgr.current_week);
        std::string speed_str = "Speed: " + std::to_string(static_cast<int>(time_mgr.simulation_speed)) + "x";
        
        // Background panel for better readability
        renderer.DrawRectangle(10, 10, 250, 120, Color{0, 0, 0, 180});
        
        // Draw text
        renderer.DrawText(time_str.c_str(), 20, 20, 20, WHITE);
        renderer.DrawText(day_str.c_str(), 20, 45, 20, WHITE);
        renderer.DrawText(week_str.c_str(), 20, 70, 20, WHITE);
        renderer.DrawText(speed_str.c_str(), 20, 95, 20, YELLOW);
        
        // Day/night cycle indicator
        Color cycle_color = time_mgr.IsBusinessHours() ? YELLOW : DARKBLUE;
        const char* cycle_text = time_mgr.IsBusinessHours() ? "DAY" : "NIGHT";
        renderer.DrawRectangle(680, 10, 110, 40, Color{0, 0, 0, 180});
        renderer.DrawText(cycle_text, 690, 20, 20, cycle_color);
        
        renderer.EndFrame();
    }
    
    // Cleanup
    renderer.Shutdown();
    
    std::cout << std::endl << "Simulation completed after " << elapsed_time << " seconds" << std::endl;
    std::cout << "ECS test successful!" << std::endl;
    std::cout << "Renderer shut down. Exiting." << std::endl;
    return 0;
}

