/**
 * @file star_rating_demo.cpp
 * @brief Demonstration of the star rating system
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Star Rating Demo" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Star Rating System Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    ecs_world.GetWorld().set<TimeManager>({1.0f});
    ecs_world.GetWorld().set<TowerEconomy>({10000.0f});
    
    // Get the facility manager
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create some facilities with satisfaction and economics
    auto lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0);
    lobby.set<Satisfaction>({88.0f});
    lobby.set<FacilityEconomics>({500.0f, 50.0f, 50});
    
    auto office1 = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 2);
    office1.set<Satisfaction>({85.0f});
    office1.set<FacilityEconomics>({1200.0f, 200.0f, 100});
    
    auto residential1 = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 5);
    residential1.set<Satisfaction>({90.0f});
    residential1.set<FacilityEconomics>({800.0f, 100.0f, 80});
    
    // Create HUD
    HUD hud;
    
    // Set initial game state with demo values
    GameState game_state;
    game_state.funds = 45000.0f;
    game_state.income_rate = 12000.0f;
    game_state.population = 230;
    game_state.current_day = 15;
    game_state.current_time = 14.5f;
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    
    // Manually set rating for demo (3-star tower)
    game_state.rating.stars = 3;
    game_state.rating.average_satisfaction = 88.0f;
    game_state.rating.total_tenants = 230;
    game_state.rating.total_floors = 15;
    game_state.rating.hourly_income = 12000.0f;
    game_state.rating.next_star_tenants = 100;
    game_state.rating.next_star_satisfaction = 75.0f;
    game_state.rating.next_star_floors = 20;
    game_state.rating.next_star_income = 0.0f;
    
    hud.SetGameState(game_state);
    
    // Add notification
    hud.AddNotification(Notification::Type::Success, "Star rating system active!", 10.0f);
    
    // Render a few frames and take screenshot
    for (int frame = 0; frame < 5; frame++) {
        renderer.BeginFrame();
        ClearBackground(Color{30, 30, 40, 255});
        
        // Draw title
        DrawText("STAR RATING SYSTEM DEMO", 20, 250, 30, WHITE);
        DrawText("Tower Progression & Rating", 20, 290, 20, LIGHTGRAY);
        
        // Render HUD (includes star rating panel)
        hud.Render();
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    TakeScreenshot("star_rating_demo.png");
    std::cout << "Screenshot saved to: star_rating_demo.png" << std::endl;
    
    renderer.Shutdown();
    
    return 0;
}
