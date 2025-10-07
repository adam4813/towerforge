#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"

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
  
    std::cout << "  Created 2 actors" << std::endl;
    
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
    
    // Demonstrate Tower Grid System and Facility Manager
    std::cout << std::endl << "Demonstrating Tower Grid System and Facility Manager..." << std::endl;
    auto& grid = ecs_world.GetTowerGrid();
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create placement system now that we have grid and facility_mgr
    PlacementSystem placement_system(grid, facility_mgr, build_menu);
    
    std::cout << "  Initial grid: " << grid.GetFloorCount() << " floors x " 
              << grid.GetColumnCount() << " columns" << std::endl;
    
    // Create and place facilities using FacilityManager
    std::cout << "  Creating facilities..." << std::endl;
    auto grid_lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0, 0, "MainLobby");
    auto grid_office1 = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 2, 0, "Office_Floor_1");
    auto residential1 = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 5, 0, "Condo_Floor_2");
    auto shop1 = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 1, 0, "Shop_Floor_3");
    
    std::cout << "  Created 4 facilities (Lobby, Office, Residential, RetailShop)" << std::endl;
    
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
    
    // Grid rendering constants
    const int grid_offset_x = 300;
    const int grid_offset_y = 100;
    const int cell_width = 40;
    const int cell_height = 50;
    
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
        
        // Update placement system
        placement_system.Update(time_step);
        
        // Handle keyboard shortcuts
        placement_system.HandleKeyboard();
        
        // Handle mouse clicks for demo
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            
            // Check if click is on build menu
            int menu_result = build_menu.HandleClick(mouse_x, mouse_y, 
                                                     placement_system.CanUndo(), 
                                                     placement_system.CanRedo());
            if (menu_result >= 0) {
                hud.AddNotification(Notification::Type::Info, "Facility selected from menu", 3.0f);
            } else if (menu_result == -2) {
                // Demolish mode
                placement_system.SetDemolishMode(!placement_system.IsDemolishMode());
                hud.AddNotification(Notification::Type::Info, 
                    placement_system.IsDemolishMode() ? "Demolish mode ON" : "Demolish mode OFF", 3.0f);
            } else if (menu_result == -3) {
                // Undo
                placement_system.Undo();
                hud.AddNotification(Notification::Type::Info, "Undid last action", 2.0f);
            } else if (menu_result == -4) {
                // Redo
                placement_system.Redo();
                hud.AddNotification(Notification::Type::Info, "Redid action", 2.0f);
            }
            // Check if click is on HUD
            else if (!hud.HandleClick(mouse_x, mouse_y)) {
                // Click is in game area - try placement/demolition
                int cost_change = placement_system.HandleClick(mouse_x, mouse_y,
                    grid_offset_x, grid_offset_y, cell_width, cell_height, game_state.funds);
                
                if (cost_change != 0) {
                    game_state.funds += cost_change;
                    if (cost_change < 0) {
                        hud.AddNotification(Notification::Type::Success, 
                            TextFormat("Facility placed! Cost: $%d", -cost_change), 3.0f);
                    } else {
                        hud.AddNotification(Notification::Type::Info, 
                            TextFormat("Facility demolished! Refund: $%d", cost_change), 3.0f);
                    }
                }
            }
        }
        
        renderer.BeginFrame();
        
        // Clear background to dark gray
        renderer.Clear(DARKGRAY);
        
        // Draw grid
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            for (int col = 0; col < grid.GetColumnCount(); ++col) {
                int x = grid_offset_x + col * cell_width;
                int y = grid_offset_y + floor * cell_height;
                
                // Draw grid cell outline
                DrawRectangleLines(x, y, cell_width, cell_height, ColorAlpha(WHITE, 0.2f));
                
                // Draw facility if present
                if (grid.IsOccupied(floor, col)) {
                    int facility_id = grid.GetFacilityAt(floor, col);
                    // Color based on facility type (simplified)
                    Color facility_color = SKYBLUE;
                    if (facility_id % 3 == 0) facility_color = PURPLE;
                    else if (facility_id % 3 == 1) facility_color = GREEN;
                    
                    DrawRectangle(x + 2, y + 2, cell_width - 4, cell_height - 4, facility_color);
                }
            }
        }
        
        // Draw floor labels
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            int y = grid_offset_y + floor * cell_height;
            DrawText(TextFormat("F%d", floor), grid_offset_x - 30, y + 15, 12, LIGHTGRAY);
        }
        
        // Render placement system (preview and construction)
        placement_system.Render(grid_offset_x, grid_offset_y, cell_width, cell_height);
        
        // Render HUD and build menu
        hud.Render();
        build_menu.Render(placement_system.CanUndo(), placement_system.CanRedo(), 
                         placement_system.IsDemolishMode());
        
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

