#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"

using namespace TowerForge::Core;
using namespace towerforge::ui;

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
    
    // Create the global TowerEconomy as a singleton
    // Start with $10,000 balance
    ecs_world.GetWorld().set<TowerEconomy>({10000.0f});
    
    std::cout << std::endl << "Creating example entities..." << std::endl;    
    std::cout << "Renderer initialized. Window opened." << std::endl;
    std::cout << "Press ESC or close window to exit." << std::endl;
    std::cout << std::endl;
    
    // Create some example actors (people)
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    actor1.set<Satisfaction>({80.0f});  // Start with good satisfaction
    
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
    actor2.set<Satisfaction>({75.0f});  // Start with good satisfaction
    
    // Add a daily schedule for Sarah
    DailySchedule sarah_schedule;
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 8.5f);  // 8:30 AM
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.5f); // 12:30 PM
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 16.5f);  // 4:30 PM
    sarah_schedule.AddWeekendAction(ScheduledAction::Type::Idle, 11.0f);       // Idle on weekends
    actor2.set<DailySchedule>(sarah_schedule);
    
    // Create some example building components with economics
    auto lobby = ecs_world.CreateEntity("Lobby");
    lobby.set<Position>({0.0f, 0.0f});
    lobby.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    lobby.set<Satisfaction>({85.0f});  // Lobbies generally have good satisfaction
    lobby.set<FacilityEconomics>({50.0f, 10.0f, 50});  // Low rent, low cost, high capacity
    
    auto office1 = ecs_world.CreateEntity("Office_Floor_5");
    office1.set<Position>({0.0f, 50.0f});
    office1.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 8, 20});
    office1.set<Satisfaction>({70.0f});
    office1.set<FacilityEconomics>({150.0f, 30.0f, 20});  // Medium rent, medium cost
    
    auto restaurant = ecs_world.CreateEntity("Restaurant_Floor_3");
    restaurant.set<Position>({0.0f, 30.0f});
    restaurant.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 6, 30});
    restaurant.set<Satisfaction>({65.0f});
    restaurant.set<FacilityEconomics>({200.0f, 60.0f, 30});  // High rent, high cost
    
    std::cout << "  Created 2 actors and 3 building components with satisfaction and economics" << std::endl;
    
    // Create HUD and build menu
    HUD hud;
    BuildMenu build_menu;
    
    // Set initial game state
    GameState game_state;
    game_state.funds = 25000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 2;
    game_state.current_day = 1;
    game_state.current_time = 8.5f;  // 8:30 AM
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    
    hud.SetGameState(game_state);
    
    // Add some example notifications
    hud.AddNotification(Notification::Type::Success, "Welcome to TowerForge!", 10.0f);
    hud.AddNotification(Notification::Type::Info, "Click entities to view details", 8.0f);
    
    
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
    float sim_time = 0.0f;
    
    while (elapsed_time < total_time && !renderer.ShouldClose()) {
        if (!ecs_world.Update(time_step)) {
            break;
        }
        elapsed_time += time_step;
        sim_time += time_step * game_state.speed_multiplier;
        
        // Update game state for HUD
        game_state.current_time = 8.5f + (sim_time / 3600.0f);  // Increment time
        if (game_state.current_time >= 24.0f) {
            game_state.current_time -= 24.0f;
            game_state.current_day++;
        }
        game_state.funds += (game_state.income_rate / 3600.0f) * time_step;
        
        hud.SetGameState(game_state);
        hud.Update(time_step);
        
        // Handle mouse clicks for demo
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            
            // Check if click is on build menu
            if (build_menu.HandleClick(mouse_x, mouse_y) >= 0) {
                hud.AddNotification(Notification::Type::Info, "Facility selected from menu", 3.0f);
            }
            // Check if click is on HUD
            else if (!hud.HandleClick(mouse_x, mouse_y)) {
                // Click is in game area - show example info panels
                if (mouse_x > 250 && mouse_x < 550 && mouse_y > 200 && mouse_y < 280) {
                    // Clicked on building
                    FacilityInfo info;
                    info.type = "OFFICE";
                    info.floor = 5;
                    info.occupancy = 8;
                    info.max_occupancy = 10;
                    info.revenue = 80.0f;
                    info.satisfaction = 85.0f;
                    info.tenant_count = 8;
                    hud.ShowFacilityInfo(info);
                } else if (mouse_x > 370 && mouse_x < 430 && mouse_y > 370 && mouse_y < 430) {
                    // Clicked on person
                    PersonInfo info;
                    info.id = 42;
                    info.state = "WaitingElevator";
                    info.current_floor = 1;
                    info.destination_floor = 8;
                    info.wait_time = 45.0f;
                    info.needs = "Work";
                    info.satisfaction = 60.0f;
                    hud.ShowPersonInfo(info);
                }
            }
        }
        
        renderer.BeginFrame();
        
        // Clear background to dark gray
        renderer.Clear(DARKGRAY);
        
        // Draw a test rectangle (representing a building floor)
        renderer.DrawRectangle(250, 200, 300, 80, SKYBLUE);
        
        // Draw a test circle (representing a person or elevator)
        renderer.DrawCircle(400, 400, 30.0f, RED);
        
        // Render HUD and build menu
        hud.Render();
        build_menu.Render();
        
        // Display tower economy status
        const auto& tower_economy = ecs_world.GetWorld().get<TowerEconomy>();
        renderer.DrawRectangle(10, 140, 280, 100, Color{0, 0, 0, 180});
        renderer.DrawText("Tower Economics", 20, 145, 18, GOLD);
        
        std::string balance_str = "Balance: $" + std::to_string(static_cast<int>(tower_economy.total_balance));
        std::string revenue_str = "Revenue: $" + std::to_string(static_cast<int>(tower_economy.daily_revenue));
        std::string expense_str = "Expenses: $" + std::to_string(static_cast<int>(tower_economy.daily_expenses));
        
        renderer.DrawText(balance_str.c_str(), 20, 170, 16, GREEN);
        renderer.DrawText(revenue_str.c_str(), 20, 195, 16, SKYBLUE);
        renderer.DrawText(expense_str.c_str(), 20, 220, 16, ORANGE);
        
        // Display satisfaction indicators for actors
        int y_offset = 250;
        auto actor_query = ecs_world.GetWorld().query<const Actor, const Satisfaction>();
        actor_query.each([&](flecs::entity e, const Actor& actor, const Satisfaction& sat) {
            if (y_offset < 520) {  // Don't overflow screen
                renderer.DrawRectangle(10, y_offset, 280, 50, Color{0, 0, 0, 180});
                
                std::string name_str = actor.name + " Satisfaction";
                renderer.DrawText(name_str.c_str(), 20, y_offset + 5, 16, WHITE);
                
                std::string score_str = std::to_string(static_cast<int>(sat.satisfaction_score)) + "% - " + sat.GetLevelString();
                
                // Color based on satisfaction level
                Color sat_color;
                switch (sat.GetLevel()) {
                    case Satisfaction::Level::VeryPoor:
                        sat_color = RED;
                        break;
                    case Satisfaction::Level::Poor:
                        sat_color = ORANGE;
                        break;
                    case Satisfaction::Level::Average:
                        sat_color = YELLOW;
                        break;
                    case Satisfaction::Level::Good:
                        sat_color = LIME;
                        break;
                    case Satisfaction::Level::Excellent:
                        sat_color = GREEN;
                        break;
                    default:
                        sat_color = WHITE;
                }
                
                renderer.DrawText(score_str.c_str(), 20, y_offset + 25, 16, sat_color);
                y_offset += 55;
            }
        });
        
        renderer.EndFrame();
    }
    
    // Cleanup
    renderer.Shutdown();
    
    std::cout << std::endl << "Simulation completed after " << elapsed_time << " seconds" << std::endl;
    std::cout << "ECS test successful!" << std::endl;
    std::cout << "Renderer shut down. Exiting." << std::endl;
    return 0;
}

