/**
 * @file demo_screenshot.cpp
 * @brief Screenshot tool for TowerForge satisfaction and economy demo
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include <raylib.h>

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Demo Screenshot Generator" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Satisfaction & Economy Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Set up global time manager (60x speed)
    ecs_world.GetWorld().set<TimeManager>({60.0f});
    
    // Set up global tower economy
    ecs_world.GetWorld().set<TowerEconomy>({10000.0f});
    
    // Create some example actors with satisfaction
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    actor1.set<Satisfaction>({85.0f});  // Excellent satisfaction
    
    DailySchedule john_schedule;
    john_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 9.0f);
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.0f);
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 17.0f);
    actor1.set<DailySchedule>(john_schedule);
    
    auto actor2 = ecs_world.CreateEntity("Sarah");
    actor2.set<Position>({20.0f, 0.0f});
    actor2.set<Velocity>({-0.3f, 0.0f});
    actor2.set<Actor>({"Sarah", 3, 0.8f});
    actor2.set<Satisfaction>({55.0f});  // Average satisfaction
    
    DailySchedule sarah_schedule;
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 8.5f);
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.5f);
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 16.5f);
    actor2.set<DailySchedule>(sarah_schedule);
    
    // Create facilities with economics
    auto lobby = ecs_world.CreateEntity("Lobby");
    lobby.set<Position>({0.0f, 0.0f});
    lobby.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    lobby.set<Satisfaction>({90.0f});
    lobby.set<FacilityEconomics>({50.0f, 10.0f, 50});
    
    auto office = ecs_world.CreateEntity("Office");
    office.set<Position>({0.0f, 50.0f});
    office.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 8, 20});
    office.set<Satisfaction>({75.0f});
    office.set<FacilityEconomics>({150.0f, 30.0f, 20});
    
    auto restaurant = ecs_world.CreateEntity("Restaurant");
    restaurant.set<Position>({0.0f, 30.0f});
    restaurant.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 6, 30});
    restaurant.set<Satisfaction>({65.0f});
    restaurant.set<FacilityEconomics>({200.0f, 60.0f, 30});
    
    // Run simulation for a few frames to populate data
    const float time_step = 1.0f / 60.0f;
    for (int i = 0; i < 120; i++) {  // 2 seconds of simulation
        ecs_world.Update(time_step);
    }
    
    // Manually add some revenue for demonstration
    auto& economy = ecs_world.GetWorld().ensure<TowerEconomy>();
    economy.daily_revenue = 450.0f;
    economy.daily_expenses = 100.0f;
    economy.total_balance = 10350.0f;
    
    // Render one frame
    renderer.BeginFrame();
    renderer.Clear(DARKGRAY);
    
    // Draw a test rectangle (representing a building floor)
    renderer.DrawRectangle(250, 200, 300, 80, SKYBLUE);
    
    // Draw a test circle (representing a person or elevator)
    renderer.DrawCircle(400, 400, 30.0f, RED);
    
    // Display current simulation time
    const auto& time_mgr = ecs_world.GetWorld().get<TimeManager>();
    std::string time_str = "Time: " + time_mgr.GetTimeString();
    std::string day_str = std::string("Day: ") + time_mgr.GetDayName();
    std::string week_str = "Week: " + std::to_string(time_mgr.current_week);
    std::string speed_str = "Speed: " + std::to_string(static_cast<int>(time_mgr.simulation_speed)) + "x";
    
    // Background panel for better readability
    renderer.DrawRectangle(10, 10, 250, 120, Color{0, 0, 0, 180});
    
    // Draw time text
    renderer.DrawText(time_str.c_str(), 20, 20, 20, WHITE);
    renderer.DrawText(day_str.c_str(), 20, 45, 20, WHITE);
    renderer.DrawText(week_str.c_str(), 20, 70, 20, WHITE);
    renderer.DrawText(speed_str.c_str(), 20, 95, 20, YELLOW);
    
    // Day/night cycle indicator
    Color cycle_color = time_mgr.IsBusinessHours() ? YELLOW : DARKBLUE;
    const char* cycle_text = time_mgr.IsBusinessHours() ? "DAY" : "NIGHT";
    renderer.DrawRectangle(680, 10, 110, 40, Color{0, 0, 0, 180});
    renderer.DrawText(cycle_text, 690, 20, 20, cycle_color);
    
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
        if (y_offset < 520) {
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
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("satisfaction_economy_demo.png");
    std::cout << "Screenshot saved to satisfaction_economy_demo.png" << std::endl;
    
    // Cleanup
    renderer.Shutdown();
    
    return 0;
}
