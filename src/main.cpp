#include <iostream>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/save_load_manager.hpp"
#include "core/achievement_manager.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"
#include "ui/main_menu.h"
#include "ui/pause_menu.h"
#include "ui/save_load_menu.h"
#include "ui/achievements_menu.h"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"

using namespace TowerForge::Core;
using namespace towerforge::ui;
using namespace towerforge::rendering;

// Helper function to calculate tower rating based on statistics
void CalculateTowerRating(TowerRating& rating, ECSWorld& ecs_world, float income_rate) {
    // Collect statistics from ECS world
    int total_tenants = 0;
    float total_satisfaction = 0.0f;
    int satisfaction_count = 0;
    
    // Count tenants and satisfaction from facilities
    ecs_world.GetWorld().each([&](const FacilityEconomics& econ, const Satisfaction& sat) {
        total_tenants += econ.current_tenants;
        total_satisfaction += sat.satisfaction_score;
        satisfaction_count++;
    });
    
    // Calculate average satisfaction
    float avg_satisfaction = (satisfaction_count > 0) ? (total_satisfaction / satisfaction_count) : 0.0f;
    
    // Get floor count from tower grid
    int floor_count = ecs_world.GetTowerGrid().GetFloorCount();
    
    // Update rating structure
    rating.total_tenants = total_tenants;
    rating.average_satisfaction = avg_satisfaction;
    rating.total_floors = floor_count;
    rating.hourly_income = income_rate;
    
    // Calculate star rating based on thresholds
    int new_stars = 1;  // Start with 1 star
    
    // 2 stars: 25+ tenants
    if (total_tenants >= 25) {
        new_stars = 2;
        rating.next_star_tenants = 50;
        rating.next_star_satisfaction = 70.0f;
        rating.next_star_floors = 0;
        rating.next_star_income = 0.0f;
    } else {
        rating.next_star_tenants = 25;
        rating.next_star_satisfaction = 0.0f;
        rating.next_star_floors = 0;
        rating.next_star_income = 0.0f;
    }
    
    // 3 stars: 50+ tenants AND 70%+ satisfaction
    if (total_tenants >= 50 && avg_satisfaction >= 70.0f) {
        new_stars = 3;
        rating.next_star_tenants = 100;
        rating.next_star_satisfaction = 75.0f;
        rating.next_star_floors = 20;
        rating.next_star_income = 0.0f;
    } else if (new_stars >= 2) {
        rating.next_star_tenants = 50;
        rating.next_star_satisfaction = 70.0f;
        rating.next_star_floors = 0;
        rating.next_star_income = 0.0f;
    }
    
    // 4 stars: 100+ tenants AND 75%+ satisfaction AND 20+ floors
    if (total_tenants >= 100 && avg_satisfaction >= 75.0f && floor_count >= 20) {
        new_stars = 4;
        rating.next_star_tenants = 200;
        rating.next_star_satisfaction = 80.0f;
        rating.next_star_floors = 40;
        rating.next_star_income = 10000.0f;
    } else if (new_stars >= 3) {
        rating.next_star_tenants = 100;
        rating.next_star_satisfaction = 75.0f;
        rating.next_star_floors = 20;
        rating.next_star_income = 0.0f;
    }
    
    // 5 stars: 200+ tenants AND 80%+ satisfaction AND 40+ floors AND $10k+/hr income
    if (total_tenants >= 200 && avg_satisfaction >= 80.0f && 
        floor_count >= 40 && income_rate >= 10000.0f) {
        new_stars = 5;
        rating.next_star_tenants = 0;
        rating.next_star_satisfaction = 0.0f;
        rating.next_star_floors = 0;
        rating.next_star_income = 0.0f;
    } else if (new_stars >= 4) {
        rating.next_star_tenants = 200;
        rating.next_star_satisfaction = 80.0f;
        rating.next_star_floors = 40;
        rating.next_star_income = 10000.0f;
    }
    
    rating.stars = new_stars;
}

// Game modes
enum class GameMode {
    TitleScreen,
    InGame,
    Settings,
    Credits,
    Achievements,
    Quit
};

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Tower Simulation Game" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Initializing Raylib renderer..." << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge");
    
    // Game mode management
    GameMode current_mode = GameMode::TitleScreen;
    bool game_initialized = false;
    
    // Create main menu
    MainMenu main_menu;
    
    // Create achievement manager for persistent achievements
    AchievementManager achievement_manager;
    achievement_manager.Initialize();
    
    // Create achievements menu
    AchievementsMenu achievements_menu;
    achievements_menu.SetAchievementManager(&achievement_manager);
    
    // Title screen loop
    while (current_mode == GameMode::TitleScreen && !renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        
        // Update menu
        main_menu.Update(delta_time);
        
        // Handle input
        int keyboard_selection = main_menu.HandleKeyboard();
        int mouse_selection = main_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                     IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
        
        int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
        
        if (selected >= 0) {
            MenuOption option = static_cast<MenuOption>(selected);
            switch (option) {
                case MenuOption::NewGame:
                    current_mode = GameMode::InGame;
                    std::cout << "Starting new game..." << std::endl;
                    break;
                case MenuOption::LoadGame:
                    std::cout << "Load game not yet implemented" << std::endl;
                    // For now, just start a new game
                    current_mode = GameMode::InGame;
                    break;
                case MenuOption::Achievements:
                    current_mode = GameMode::Achievements;
                    break;
                case MenuOption::Settings:
                    current_mode = GameMode::Settings;
                    break;
                case MenuOption::Credits:
                    current_mode = GameMode::Credits;
                    break;
                case MenuOption::Quit:
                    current_mode = GameMode::Quit;
                    break;
            }
        }
        
        // Render
        renderer.BeginFrame();
        main_menu.Render();
        renderer.EndFrame();
    }
    
    // Handle achievements screen
    if (current_mode == GameMode::Achievements) {
        while (current_mode == GameMode::Achievements && !renderer.ShouldClose()) {
            float delta_time = GetFrameTime();
            
            // Update achievements menu
            achievements_menu.Update(delta_time);
            
            // Handle input
            if (achievements_menu.HandleKeyboard()) {
                current_mode = GameMode::TitleScreen;
            }
            achievements_menu.HandleMouse(GetMouseX(), GetMouseY(), GetMouseWheelMove());
            
            // Render
            renderer.BeginFrame();
            ClearBackground(Color{20, 20, 30, 255});
            achievements_menu.Render();
            renderer.EndFrame();
        }
        
        // Return to title screen if not quitting
        if (current_mode != GameMode::Quit && !renderer.ShouldClose()) {
            current_mode = GameMode::TitleScreen;
            // Restart title screen loop (code will continue below)
        }
    }
    
    // Handle settings screen
    if (current_mode == GameMode::Settings) {
        GeneralSettingsMenu general_settings_menu;
        AudioSettingsMenu audio_settings_menu;
        bool in_audio_settings = false;
        
        while (current_mode == GameMode::Settings && !renderer.ShouldClose()) {
            float delta_time = GetFrameTime();
            
            if (in_audio_settings) {
                // Update audio settings menu
                audio_settings_menu.Update(delta_time);
                
                // Handle input
                if (audio_settings_menu.HandleKeyboard()) {
                    in_audio_settings = false;  // Go back to general settings
                }
                bool back_clicked = audio_settings_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                                     IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                if (back_clicked) {
                    in_audio_settings = false;  // Go back to general settings
                }
                
                // Render
                renderer.BeginFrame();
                ClearBackground(Color{20, 20, 30, 255});
                audio_settings_menu.Render();
                renderer.EndFrame();
            } else {
                // Update general settings menu
                general_settings_menu.Update(delta_time);
                
                // Handle input
                int keyboard_selection = general_settings_menu.HandleKeyboard();
                int mouse_selection = general_settings_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                
                int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
                
                if (selected >= 0) {
                    SettingsOption option = static_cast<SettingsOption>(selected);
                    switch (option) {
                        case SettingsOption::Audio:
                            in_audio_settings = true;
                            break;
                        case SettingsOption::Controls:
                        case SettingsOption::Display:
                        case SettingsOption::Accessibility:
                        case SettingsOption::Gameplay:
                            // Placeholder for future settings
                            std::cout << "Settings option not yet implemented" << std::endl;
                            break;
                        case SettingsOption::Back:
                            current_mode = GameMode::TitleScreen;
                            break;
                    }
                }
                
                // Render
                renderer.BeginFrame();
                ClearBackground(Color{20, 20, 30, 255});
                general_settings_menu.Render();
                renderer.EndFrame();
            }
        }
        
        // Return to title screen if not quitting
        if (current_mode != GameMode::Quit && !renderer.ShouldClose()) {
            current_mode = GameMode::TitleScreen;
            // Restart title screen loop (code will continue below)
        }
    }
    
    // Handle credits screen
    if (current_mode == GameMode::Credits) {
        while (current_mode == GameMode::Credits && !renderer.ShouldClose()) {
            // Simple credits screen
            renderer.BeginFrame();
            ClearBackground(Color{20, 20, 30, 255});
            
            int screen_width = GetScreenWidth();
            int y = 100;
            
            DrawText("CREDITS", (screen_width - MeasureText("CREDITS", 40)) / 2, y, 40, GOLD);
            y += 80;
            
            DrawText("TowerForge v0.1.0", (screen_width - MeasureText("TowerForge v0.1.0", 24)) / 2, y, 24, WHITE);
            y += 50;
            
            DrawText("A modern SimTower-inspired skyscraper simulation", 
                     (screen_width - MeasureText("A modern SimTower-inspired skyscraper simulation", 18)) / 2, 
                     y, 18, LIGHTGRAY);
            y += 60;
            
            DrawText("Built with:", (screen_width - MeasureText("Built with:", 20)) / 2, y, 20, LIGHTGRAY);
            y += 40;
            DrawText("- C++20", (screen_width - MeasureText("- C++20", 18)) / 2, y, 18, WHITE);
            y += 30;
            DrawText("- Raylib (rendering)", (screen_width - MeasureText("- Raylib (rendering)", 18)) / 2, y, 18, WHITE);
            y += 30;
            DrawText("- Flecs (ECS framework)", (screen_width - MeasureText("- Flecs (ECS framework)", 18)) / 2, y, 18, WHITE);
            y += 60;
            
            DrawText("Press ESC or ENTER to return to menu", 
                     (screen_width - MeasureText("Press ESC or ENTER to return to menu", 16)) / 2, 
                     y, 16, GRAY);
            
            renderer.EndFrame();
            
            // Check for return to menu
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                current_mode = GameMode::TitleScreen;
            }
        }
        
        // Return to title screen if not quitting
        if (current_mode != GameMode::Quit && !renderer.ShouldClose()) {
            current_mode = GameMode::TitleScreen;
            // Restart title screen loop
            while (current_mode == GameMode::TitleScreen && !renderer.ShouldClose()) {
                float delta_time = GetFrameTime();
                main_menu.Update(delta_time);
                
                int keyboard_selection = main_menu.HandleKeyboard();
                int mouse_selection = main_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                             IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                
                int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
                
                if (selected >= 0) {
                    MenuOption option = static_cast<MenuOption>(selected);
                    switch (option) {
                        case MenuOption::NewGame:
                            current_mode = GameMode::InGame;
                            break;
                        case MenuOption::LoadGame:
                            current_mode = GameMode::InGame;
                            break;
                        case MenuOption::Achievements:
                            current_mode = GameMode::Achievements;
                            break;
                        case MenuOption::Settings:
                            current_mode = GameMode::TitleScreen;
                            break;
                        case MenuOption::Credits:
                            current_mode = GameMode::Credits;
                            break;
                        case MenuOption::Quit:
                            current_mode = GameMode::Quit;
                            break;
                    }
                }
                
                renderer.BeginFrame();
                main_menu.Render();
                renderer.EndFrame();
            }
        }
    }
    
    // Exit if user chose quit or closed window
    if (current_mode == GameMode::Quit || renderer.ShouldClose()) {
        renderer.Shutdown();
        std::cout << "Exiting TowerForge..." << std::endl;
        return 0;
    }
    
    // Initialize game (only when entering InGame mode)
    std::cout << "Initializing game..." << std::endl;
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create and initialize save/load manager
    TowerForge::Core::SaveLoadManager save_load_manager;
    save_load_manager.Initialize();
    save_load_manager.SetAutosaveEnabled(true);
    save_load_manager.SetAutosaveInterval(120.0f);  // Auto-save every 2 minutes
    save_load_manager.SetAchievementManager(&achievement_manager);  // Link achievement manager for persistence
    
    // Create save/load menu
    SaveLoadMenu save_load_menu;
    save_load_menu.SetSaveLoadManager(&save_load_manager);
    
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
    auto lobby_entity = ecs_world.CreateEntity("Lobby");
    lobby_entity.set<Position>({0.0f, 0.0f});
    lobby_entity.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    lobby_entity.set<Satisfaction>({85.0f});  // Lobbies generally have good satisfaction
    lobby_entity.set<FacilityEconomics>({50.0f, 10.0f, 50});  // Low rent, low cost, high capacity
    
    auto office1_entity = ecs_world.CreateEntity("Office_Floor_5");
    office1_entity.set<Position>({0.0f, 50.0f});
    office1_entity.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 8, 20});
    office1_entity.set<Satisfaction>({70.0f});
    office1_entity.set<FacilityEconomics>({150.0f, 30.0f, 20});  // Medium rent, medium cost
    
    auto restaurant_entity = ecs_world.CreateEntity("Restaurant_Floor_3");
    restaurant_entity.set<Position>({0.0f, 30.0f});
    restaurant_entity.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 6, 30});
    restaurant_entity.set<Satisfaction>({65.0f});
    restaurant_entity.set<FacilityEconomics>({200.0f, 60.0f, 30});  // High rent, high cost
    
    std::cout << "  Created 2 actors and 3 building components with satisfaction and economics" << std::endl;
    
    // Create HUD and build menu
    HUD hud;
    BuildMenu build_menu;
    PauseMenu pause_menu;
    
    // Create and initialize camera
    towerforge::rendering::Camera camera;
    camera.Initialize(800, 600, 1200.0f, 800.0f);  // Tower is 1200x800 world units
    
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
    
    // Create elevator system
    std::cout << "  Creating elevator system..." << std::endl;
    auto elevator_shaft = ecs_world.CreateEntity("MainElevatorShaft");
    elevator_shaft.set<ElevatorShaft>({10, 0, 5, 1});  // Column 10, floors 0-5, 1 car
    
    auto elevator_car = ecs_world.CreateEntity("Elevator1");
    elevator_car.set<ElevatorCar>({static_cast<int>(elevator_shaft.id()), 0, 8});  // Start at floor 0, capacity 8
    
    std::cout << "  Created elevator shaft at column 10 serving floors 0-5" << std::endl;
    
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
    bool is_paused = false;  // Track pause state
    bool in_settings_from_pause = false;  // Track if we're in settings from pause menu
    bool in_audio_settings_from_pause = false;  // Track if we're in audio settings submenu
    
    // Create settings menus for pause menu usage
    GeneralSettingsMenu pause_general_settings_menu;
    AudioSettingsMenu pause_audio_settings_menu;
    
    while (elapsed_time < total_time && !renderer.ShouldClose()) {
        // Handle ESC key to toggle pause menu (unless we're in settings)
        if (IsKeyPressed(KEY_ESCAPE) && !in_settings_from_pause && !in_audio_settings_from_pause) {
            is_paused = !is_paused;
            if (is_paused) {
                game_state.paused = true;
            }
        }
        
        // Only update simulation if not paused
        if (!is_paused) {
            // Update autosave
            save_load_manager.UpdateAutosave(time_step, ecs_world);
            
            if (!ecs_world.Update(time_step)) {
                break;
            }
            elapsed_time += time_step;
            sim_time += time_step * game_state.speed_multiplier;
        }
        
        // Update game state for HUD
        game_state.current_time = 8.5f + (sim_time / 3600.0f);  // Increment time
        if (game_state.current_time >= 24.0f) {
            game_state.current_time -= 24.0f;
            game_state.current_day++;
        }
        
        // Only update game state if not paused
        if (!is_paused) {
            game_state.funds += (game_state.income_rate / 3600.0f) * time_step;
        }
        
        // Calculate tower rating
        CalculateTowerRating(game_state.rating, ecs_world, game_state.income_rate);
        
        hud.SetGameState(game_state);
        hud.Update(time_step);
        
        // Check for achievements (only if not paused)
        if (!is_paused) {
            // Get economy singleton for total income
            flecs::world& world = ecs_world.GetWorld();
            float total_income = 0.0f;
            if (world.has<TowerEconomy>()) {
                const auto& economy = world.get<TowerEconomy>();
                total_income = economy.total_revenue;
            }
            
            // Get floor count
            int floor_count = ecs_world.GetTowerGrid().GetFloorCount();
            
            // Calculate average satisfaction (simplified - just use game state population for now)
            float avg_satisfaction = 75.0f;  // Default to 75% for now
            
            // Check achievements
            achievement_manager.CheckAchievements(game_state.population, total_income, floor_count, avg_satisfaction);
            
            // Display notifications for newly unlocked achievements
            if (achievement_manager.HasNewAchievements()) {
                auto newly_unlocked = achievement_manager.PopNewlyUnlocked();
                for (const auto& achievement_id : newly_unlocked) {
                    // Find achievement to get its name
                    for (const auto& achievement : achievement_manager.GetAllAchievements()) {
                        if (achievement.id == achievement_id) {
                            std::string message = "Achievement Unlocked: " + achievement.name;
                            hud.AddNotification(Notification::Type::Success, message, 5.0f);
                            break;
                        }
                    }
                }
            }
            
            // Update achievements menu with current stats
            achievements_menu.SetGameStats(game_state.population, total_income, floor_count, avg_satisfaction);
        }
        
        // Handle pause menu input
        if (is_paused) {
            // Handle settings menu input if in settings
            if (in_audio_settings_from_pause) {
                pause_audio_settings_menu.Update(time_step);
                
                // Handle input
                if (pause_audio_settings_menu.HandleKeyboard()) {
                    in_audio_settings_from_pause = false;  // Go back to general settings
                }
                bool back_clicked = pause_audio_settings_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                                           IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                if (back_clicked) {
                    in_audio_settings_from_pause = false;  // Go back to general settings
                }
            } else if (in_settings_from_pause) {
                pause_general_settings_menu.Update(time_step);
                
                // Handle input
                int keyboard_selection = pause_general_settings_menu.HandleKeyboard();
                int mouse_selection = pause_general_settings_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                                               IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                
                int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
                
                if (selected >= 0) {
                    SettingsOption option = static_cast<SettingsOption>(selected);
                    switch (option) {
                        case SettingsOption::Audio:
                            in_audio_settings_from_pause = true;
                            break;
                        case SettingsOption::Controls:
                        case SettingsOption::Display:
                        case SettingsOption::Accessibility:
                        case SettingsOption::Gameplay:
                            // Placeholder for future settings
                            hud.AddNotification(Notification::Type::Info, "Settings option not yet implemented", 3.0f);
                            break;
                        case SettingsOption::Back:
                            in_settings_from_pause = false;  // Go back to pause menu
                            break;
                    }
                }
            } else {
                // Regular pause menu input
                pause_menu.Update(time_step);
                
                // Handle quit confirmation if showing
                int quit_result = pause_menu.HandleQuitConfirmation();
                if (quit_result == 1) {
                    // User confirmed quit to title
                    current_mode = GameMode::TitleScreen;
                    break;  // Exit game loop
                } else if (quit_result == 0) {
                    // User cancelled quit - just stay in pause menu
                } else {
                    // No quit confirmation showing - handle regular menu input
                    int keyboard_selection = pause_menu.HandleKeyboard();
                    int mouse_selection = pause_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                                 IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                    
                    int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
                    
                    if (selected >= 0) {
                        PauseMenuOption option = static_cast<PauseMenuOption>(selected);
                        switch (option) {
                            case PauseMenuOption::Resume:
                                is_paused = false;
                                game_state.paused = false;
                                break;
                            case PauseMenuOption::SaveGame:
                                hud.AddNotification(Notification::Type::Info, "Save game not yet implemented", 3.0f);
                                break;
                            case PauseMenuOption::LoadGame:
                                hud.AddNotification(Notification::Type::Info, "Load game not yet implemented", 3.0f);
                                break;
                            case PauseMenuOption::Settings:
                                in_settings_from_pause = true;
                                break;
                            case PauseMenuOption::QuitToTitle:
                                // Show confirmation dialog
                                pause_menu.ShowQuitConfirmation(true);
                                break;
                        }
                    }
                }
            }
        }
        
        // Update placement system
        placement_system.Update(time_step);
        
        // Handle keyboard shortcuts (only if not paused)
        if (!is_paused) {
            placement_system.HandleKeyboard();
        }
        // Update camera
        camera.Update(time_step);
        
        // Handle mouse clicks for demo (only if not paused)
        if (!is_paused && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            
            bool hud_handled = false;
            
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
                // Click is in game area - try placement/demolition first
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
                } else {
                    // No placement/demolition occurred - check if user clicked on a facility or person to view info
                    // Convert mouse position to grid coordinates
                    int rel_x = mouse_x - grid_offset_x;
                    int rel_y = mouse_y - grid_offset_y;
                    
                    if (rel_x >= 0 && rel_y >= 0) {
                        int clicked_floor = rel_y / cell_height;
                        int clicked_column = rel_x / cell_width;
                        
                        // Check if clicked on a facility
                        if (clicked_floor >= 0 && clicked_floor < grid.GetFloorCount() &&
                            clicked_column >= 0 && clicked_column < grid.GetColumnCount()) {
                            
                            if (grid.IsOccupied(clicked_floor, clicked_column)) {
                                // Clicked on a facility - show info
                                FacilityInfo info;
                                info.type = "FACILITY";
                                info.floor = clicked_floor;
                                info.occupancy = 0;
                                info.max_occupancy = 10;
                                info.revenue = 100.0f;
                                info.satisfaction = 75.0f;
                                info.tenant_count = 0;
                                hud.ShowFacilityInfo(info);
                            }
                        }
                    }
                }
            }
        }
        
        // Handle camera input
        bool hud_handled_input = false;  // Could be updated based on mouse position over HUD
        camera.HandleInput(hud_handled_input);
        
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
        
        // Draw elevator shafts
        auto shaft_query = ecs_world.GetWorld().query<const ElevatorShaft>();
        shaft_query.each([&](flecs::entity e, const ElevatorShaft& shaft) {
            // Draw shaft as vertical column
            for (int floor = shaft.bottom_floor; floor <= shaft.top_floor; ++floor) {
                int x = grid_offset_x + shaft.column * cell_width;
                int y = grid_offset_y + floor * cell_height;
                
                // Draw shaft background
                DrawRectangle(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{60, 60, 70, 255});
                DrawRectangleLines(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{100, 100, 120, 255});
            }
        });
        
        // Draw elevator cars
        auto car_query = ecs_world.GetWorld().query<const ElevatorCar>();
        car_query.each([&](flecs::entity e, const ElevatorCar& car) {
            // Find the shaft for this car
            auto shaft_entity = ecs_world.GetWorld().entity(car.shaft_entity_id);
            if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                const ElevatorShaft& shaft = shaft_entity.ensure<ElevatorShaft>();
                
                int x = grid_offset_x + shaft.column * cell_width;
                int y = grid_offset_y + static_cast<int>(car.current_floor * cell_height);
                
                // Color based on state
                Color car_color;
                switch (car.state) {
                    case ElevatorState::Idle:
                        car_color = GRAY;
                        break;
                    case ElevatorState::MovingUp:
                        car_color = SKYBLUE;
                        break;
                    case ElevatorState::MovingDown:
                        car_color = PURPLE;
                        break;
                    case ElevatorState::DoorsOpening:
                    case ElevatorState::DoorsClosing:
                        car_color = YELLOW;
                        break;
                    case ElevatorState::DoorsOpen:
                        car_color = GREEN;
                        break;
                }
                
                // Draw car as rectangle
                DrawRectangle(x + 6, y + 6, cell_width - 12, cell_height - 12, car_color);
                
                // Draw occupancy indicator
                if (car.current_occupancy > 0) {
                    DrawText(TextFormat("%d", car.current_occupancy), x + 16, y + 18, 14, BLACK);
                }
            }
        });
        
        // Begin camera mode for world rendering
        camera.BeginMode();
        
        // Render placement system (preview and construction)
        placement_system.Render(grid_offset_x, grid_offset_y, cell_width, cell_height);
        
        // End camera mode
        camera.EndMode();
        
        // Render HUD and build menu (these are in screen space, not world space)
        hud.Render();
        build_menu.Render(placement_system.CanUndo(), placement_system.CanRedo(), 
                         placement_system.IsDemolishMode());
        
        // Render camera controls overlay
        camera.RenderControlsOverlay();
        camera.RenderFollowIndicator();

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
        
        // Render pause menu overlay if paused
        if (is_paused) {
            if (in_audio_settings_from_pause) {
                pause_audio_settings_menu.Render();
            } else if (in_settings_from_pause) {
                pause_general_settings_menu.Render();
            } else {
                pause_menu.Render();
            }
        }
        
        renderer.EndFrame();
    }
    
    // Check if we should return to title screen
    if (current_mode == GameMode::TitleScreen && !renderer.ShouldClose()) {
        // Reset and restart title screen loop
        while (current_mode == GameMode::TitleScreen && !renderer.ShouldClose()) {
            float delta_time = GetFrameTime();
            main_menu.Update(delta_time);
            
            int keyboard_selection = main_menu.HandleKeyboard();
            int mouse_selection = main_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                                         IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            
            int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;
            
            if (selected >= 0) {
                MenuOption option = static_cast<MenuOption>(selected);
                switch (option) {
                    case MenuOption::NewGame:
                        std::cout << "New Game from pause menu not yet implemented" << std::endl;
                        current_mode = GameMode::TitleScreen;  // Stay on title for now
                        break;
                    case MenuOption::LoadGame:
                        std::cout << "Load Game from pause menu not yet implemented" << std::endl;
                        current_mode = GameMode::TitleScreen;  // Stay on title for now
                        break;
                    case MenuOption::Settings:
                        current_mode = GameMode::TitleScreen;
                        break;
                    case MenuOption::Credits:
                        current_mode = GameMode::Credits;
                        break;
                    case MenuOption::Quit:
                        current_mode = GameMode::Quit;
                        break;
                }
            }
            
            renderer.BeginFrame();
            main_menu.Render();
            renderer.EndFrame();
        }
    }
    
    // Cleanup
    // Perform final autosave before exiting
    if (save_load_manager.IsAutosaveEnabled()) {
        std::cout << "Performing final autosave before exit..." << std::endl;
        auto result = save_load_manager.Autosave(ecs_world);
        if (result.success) {
            std::cout << "Final autosave completed successfully" << std::endl;
        }
    }
    
    renderer.Shutdown();
    
    std::cout << std::endl << "Simulation completed after " << elapsed_time << " seconds" << std::endl;
    std::cout << "ECS test successful!" << std::endl;
    std::cout << "Renderer shut down. Exiting." << std::endl;
    return 0;
}

