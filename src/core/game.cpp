#include "core/game.h"
#include "core/components.hpp"
#include "core/user_preferences.hpp"
#include "ui/notification_center.h"
#include <iostream>

using namespace TowerForge::Core;
using namespace towerforge::ui;
using namespace towerforge::rendering;

namespace towerforge::core {
    // Helper function to calculate tower rating based on statistics
    static void CalculateTowerRatingHelper(TowerRating &rating, ECSWorld &ecs_world, const float income_rate) {
        // Collect statistics from ECS world
        int total_tenants = 0;
        float total_satisfaction = 0.0f;
        int satisfaction_count = 0;

        // Count tenants and satisfaction from facilities
        ecs_world.GetWorld().each([&](const FacilityEconomics &econ, const Satisfaction &sat) {
            total_tenants += econ.current_tenants;
            total_satisfaction += sat.satisfaction_score;
            satisfaction_count++;
        });

        // Calculate average satisfaction
        const float avg_satisfaction = (satisfaction_count > 0) ? (total_satisfaction / satisfaction_count) : 0.0f;

        // Get floor count from tower grid
        const int floor_count = ecs_world.GetTowerGrid().GetFloorCount();

        // Update rating structure
        rating.total_tenants = total_tenants;
        rating.average_satisfaction = avg_satisfaction;
        rating.total_floors = floor_count;
        rating.hourly_income = income_rate;

        // Calculate star rating based on thresholds
        int new_stars = 1; // Start with 1 star

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

    Game::Game()
        : current_state_(GameState::TitleScreen)
          , previous_state_(GameState::TitleScreen)
          , audio_manager_(nullptr)
          , tutorial_manager_(nullptr)
          , tutorial_active_(false)
          , help_system_(nullptr)
          , in_audio_settings_(false)
          , in_accessibility_settings_(false)
          , ecs_world_(nullptr)
          , save_load_manager_(nullptr)
          , achievement_manager_(nullptr)
          , hud_(nullptr)
          , build_menu_(nullptr)
          , pause_menu_(nullptr)
          , save_load_menu_(nullptr)
          , research_menu_(nullptr)
          , camera_(nullptr)
          , placement_system_(nullptr)
          , history_panel_(nullptr)  // std::unique_ptr initialized to nullptr
          , is_paused_(false)
          , in_settings_from_pause_(false)
          , in_audio_settings_from_pause_(false)
          , in_accessibility_settings_from_pause_(false)
          , elapsed_time_(0.0f)
          , sim_time_(0.0f)
          , time_step_(1.0f / 60.0f)
          , total_time_(30.0f)
          , grid_offset_x_(300)
          , grid_offset_y_(100)
          , cell_width_(40)
          , cell_height_(50)
          , game_initialized_(false) {
        game_state_.funds = 25000.0f;
        game_state_.income_rate = 500.0f;
        game_state_.population = 2;
        game_state_.current_day = 1;
        game_state_.current_time = 8.5f;
        game_state_.speed_multiplier = 1;
        game_state_.paused = false;
    }

    Game::~Game() {
        CleanupGameSystems();
    }

    bool Game::Initialize() {
        std::cout << "TowerForge - Tower Simulation Game" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Initializing Raylib renderer..." << std::endl;

        // Load user preferences first (this happens in the singleton constructor)
        auto& preferences = UserPreferences::GetInstance();
        std::cout << "User preferences loaded" << std::endl;

        // Initialize renderer
        renderer_.Initialize(800, 600, "TowerForge");

        // Initialize audio system
        audio_manager_ = &audio::AudioManager::GetInstance();
        audio_manager_->Initialize();

        // Apply audio preferences from loaded settings
        audio_manager_->SetMasterVolume(preferences.GetMasterVolume());
        audio_manager_->SetVolume(audio::AudioType::Music, preferences.GetMusicVolume());
        audio_manager_->SetVolume(audio::AudioType::SFX, preferences.GetSFXVolume());

        // Create achievement manager for persistent achievements
        achievement_manager_ = new AchievementManager();
        achievement_manager_->Initialize();

        // Set achievement manager for achievements menu
        achievements_menu_.SetAchievementManager(achievement_manager_);

        // Play main theme music (volume already set from preferences)
        audio_manager_->PlayMusic(audio::AudioCue::MainTheme, true, 1.0f);

        std::cout << "User preferences applied to all systems" << std::endl;

        return true;
    }

    void Game::Run() {
        while (current_state_ != GameState::Quit && !renderer_.ShouldClose()) {
            const float delta_time = GetFrameTime();

            // Update audio system
            audio_manager_->Update(delta_time);

            switch (current_state_) {
                case GameState::TitleScreen:
                    UpdateTitleScreen(delta_time);
                    RenderTitleScreen();
                    break;

                case GameState::Tutorial:
                    if (!game_initialized_) {
                        InitializeGameSystems();
                        game_initialized_ = true;
                        tutorial_active_ = true;
                    }
                    UpdateTutorial(delta_time);
                    RenderTutorial();
                    break;

                case GameState::Achievements:
                    UpdateAchievementsScreen(delta_time);
                    RenderAchievementsScreen();
                    break;

                case GameState::Settings:
                    UpdateSettingsScreen(delta_time);
                    RenderSettingsScreen();
                    break;

                case GameState::Credits:
                    UpdateCreditsScreen(delta_time);
                    RenderCreditsScreen();
                    break;

                case GameState::InGame:
                    if (!game_initialized_) {
                        InitializeGameSystems();
                        game_initialized_ = true;
                    }
                    UpdateInGame(delta_time);
                    if (current_state_ == GameState::InGame) {
                        RenderInGame();
                    }
                    break;

                case GameState::Quit:
                    break;
            }
        }
    }

    void Game::Shutdown() {
        // Cleanup game systems if initialized
        CleanupGameSystems();

        renderer_.Shutdown();
        std::cout << "Exiting TowerForge..." << std::endl;
    }

    void Game::UpdateTitleScreen(const float delta_time) {
        main_menu_.Update(delta_time);
        HandleTitleScreenInput();
    }

    void Game::RenderTitleScreen() {
        renderer_.BeginFrame();
        main_menu_.Render();
        renderer_.EndFrame();
    }

    void Game::HandleTitleScreenInput() {
        const int keyboard_selection = main_menu_.HandleKeyboard();
        const int mouse_selection = main_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                                                           IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

        int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;

        if (selected >= 0) {
            audio_manager_->PlaySFX(audio::AudioCue::MenuConfirm);
            const auto option = static_cast<MenuOption>(selected);
            switch (option) {
                case MenuOption::NewGame:
                    current_state_ = GameState::InGame;
                    std::cout << "Starting new game..." << std::endl;
                    break;
                case MenuOption::Tutorial:
                    current_state_ = GameState::Tutorial;
                    std::cout << "Starting tutorial..." << std::endl;
                    break;
                case MenuOption::LoadGame:
                    std::cout << "Load game not yet implemented" << std::endl;
                    current_state_ = GameState::InGame;
                    break;
                case MenuOption::Achievements:
                    current_state_ = GameState::Achievements;
                    break;
                case MenuOption::Settings:
                    current_state_ = GameState::Settings;
                    break;
                case MenuOption::Credits:
                    current_state_ = GameState::Credits;
                    break;
                case MenuOption::Quit:
                    current_state_ = GameState::Quit;
                    break;
            }
        }
    }

    void Game::UpdateAchievementsScreen(const float delta_time) {
        achievements_menu_.Update(delta_time);
        HandleAchievementsInput();
    }

    void Game::RenderAchievementsScreen() {
        renderer_.BeginFrame();
        ClearBackground(Color{20, 20, 30, 255});
        achievements_menu_.Render();
        renderer_.EndFrame();
    }

    void Game::HandleAchievementsInput() {
        if (achievements_menu_.HandleKeyboard()) {
            audio_manager_->PlaySFX(audio::AudioCue::MenuClose);
            current_state_ = GameState::TitleScreen;
        }
        achievements_menu_.HandleMouse(GetMouseX(), GetMouseY(), GetMouseWheelMove());
    }

    void Game::UpdateSettingsScreen(const float delta_time) {
        if (in_accessibility_settings_) {
            accessibility_settings_menu_.Update(delta_time);
            HandleSettingsInput();
        } else if (in_audio_settings_) {
            audio_settings_menu_.Update(delta_time);
            HandleSettingsInput();
        } else {
            general_settings_menu_.Update(delta_time);
            HandleSettingsInput();
        }
    }

    void Game::RenderSettingsScreen() {
        renderer_.BeginFrame();
        ClearBackground(Color{20, 20, 30, 255});

        if (in_accessibility_settings_) {
            accessibility_settings_menu_.Render();
        } else if (in_audio_settings_) {
            audio_settings_menu_.Render();
        } else {
            general_settings_menu_.Render();
        }

        renderer_.EndFrame();
    }

    void Game::HandleSettingsInput() {
        if (in_accessibility_settings_) {
            if (accessibility_settings_menu_.HandleKeyboard()) {
                in_accessibility_settings_ = false;
            }
            const bool back_clicked = accessibility_settings_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                                                                               IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            if (back_clicked) {
                in_accessibility_settings_ = false;
            }
        } else if (in_audio_settings_) {
            if (audio_settings_menu_.HandleKeyboard()) {
                in_audio_settings_ = false;
            }
            const bool back_clicked = audio_settings_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                                                                       IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            if (back_clicked) {
                in_audio_settings_ = false;
            }
        } else {
            const int keyboard_selection = general_settings_menu_.HandleKeyboard();
            const int mouse_selection = general_settings_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                                                                           IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

            int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;

            if (selected >= 0) {
                const auto option = static_cast<SettingsOption>(selected);
                switch (option) {
                    case SettingsOption::Audio:
                        in_audio_settings_ = true;
                        break;
                    case SettingsOption::Accessibility:
                        in_accessibility_settings_ = true;
                        break;
                    case SettingsOption::Controls:
                    case SettingsOption::Display:
                    case SettingsOption::Gameplay:
                        std::cout << "Settings option not yet implemented" << std::endl;
                        break;
                    case SettingsOption::Back:
                        current_state_ = GameState::TitleScreen;
                        in_audio_settings_ = false;
                        in_accessibility_settings_ = false;
                        break;
                }
            }
        }
    }

    void Game::UpdateCreditsScreen(float delta_time) {
        HandleCreditsInput();
    }

    void Game::RenderCreditsScreen() const {
        renderer_.BeginFrame();
        ClearBackground(Color{20, 20, 30, 255});

        const int screen_width = GetScreenWidth();
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
        DrawText("- Flecs (ECS framework)", (screen_width - MeasureText("- Flecs (ECS framework)", 18)) / 2, y, 18,
                 WHITE);
        y += 60;

        DrawText("Press ESC or ENTER to return to menu",
                 (screen_width - MeasureText("Press ESC or ENTER to return to menu", 16)) / 2,
                 y, 16, GRAY);

        renderer_.EndFrame();
    }

    void Game::HandleCreditsInput() {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            current_state_ = GameState::TitleScreen;
        }
    }

    void Game::InitializeGameSystems() {
        std::cout << "Initializing game..." << std::endl;

        // Change music to gameplay theme
        audio_manager_->StopMusic(1.0f);
        audio_manager_->PlayMusic(audio::AudioCue::GameplayLoop, true, 2.0f);

        // Create and initialize the ECS world
        ecs_world_ = new ECSWorld();
        ecs_world_->Initialize();

        // Create and initialize save/load manager
        save_load_manager_ = new SaveLoadManager();
        save_load_manager_->Initialize();
        save_load_manager_->SetAutosaveEnabled(true);
        save_load_manager_->SetAutosaveInterval(120.0f);
        save_load_manager_->SetAchievementManager(achievement_manager_);

        // Create save/load menu
        save_load_menu_ = new SaveLoadMenu();
        save_load_menu_->SetSaveLoadManager(save_load_manager_);

        // Create the global TimeManager as a singleton
        ecs_world_->GetWorld().set<TimeManager>({60.0f});

        // Create the global TowerEconomy as a singleton
        ecs_world_->GetWorld().set<TowerEconomy>({10000.0f});

        // Create the global ResearchTree as a singleton
        ResearchTree research_tree;
        research_tree.InitializeDefaultTree();
        research_tree.AwardPoints(50);
        ecs_world_->GetWorld().set<ResearchTree>(research_tree);

        // Create the global NPCSpawner as a singleton
        ecs_world_->GetWorld().set<NPCSpawner>({30.0f}); // Spawn visitors every 30 seconds base rate

        // Create the global StaffManager as a singleton
        ecs_world_->GetWorld().set<StaffManager>({});

        std::cout << std::endl << "Creating example entities..." << std::endl;
        std::cout << "Renderer initialized. Window opened." << std::endl;
        std::cout << "Press ESC or close window to exit." << std::endl;
        std::cout << std::endl;

        // Create some example actors (people)
        // Create one employee to demonstrate the system (Alice will be hired for an existing job)
        const auto employee1 = ecs_world_->CreateEntity("Alice");
        employee1.set<Person>({"Alice", 0, 5.0f, 2.0f, NPCType::Employee});
        employee1.set<EmploymentInfo>({"Office Worker", 1, 5, 9.0f, 17.0f});
        employee1.set<Satisfaction>({80.0f});

        std::cout << "  Created 1 initial employee (Alice - Office Worker)" << std::endl;
        std::cout << "  Additional visitors and employees will be spawned dynamically" << std::endl;

        // Keep the old actors for compatibility
        const auto actor1 = ecs_world_->CreateEntity("John");
        actor1.set<Position>({10.0f, 0.0f});
        actor1.set<Velocity>({0.5f, 0.0f});
        actor1.set<Actor>({"John", 5, 1.0f});
        actor1.set<Satisfaction>({80.0f});

        DailySchedule john_schedule;
        john_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 9.0f);
        john_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.0f);
        john_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 17.0f);
        john_schedule.AddWeekendAction(ScheduledAction::Type::Idle, 10.0f);
        actor1.set<DailySchedule>(john_schedule);

        const auto actor2 = ecs_world_->CreateEntity("Sarah");
        actor2.set<Position>({20.0f, 0.0f});
        actor2.set<Velocity>({-0.3f, 0.0f});
        actor2.set<Actor>({"Sarah", 3, 0.8f});
        actor2.set<Satisfaction>({75.0f});

        DailySchedule sarah_schedule;
        sarah_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 8.5f);
        sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.5f);
        sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 16.5f);
        sarah_schedule.AddWeekendAction(ScheduledAction::Type::Idle, 11.0f);
        actor2.set<DailySchedule>(sarah_schedule);

        std::cout << "  Created 2 legacy actors for compatibility" << std::endl;

        // Create building components
        const auto lobby_entity = ecs_world_->CreateEntity("Lobby");
        lobby_entity.set<Position>({0.0f, 0.0f});
        lobby_entity.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 0, 10, 50});
        lobby_entity.set<Satisfaction>({85.0f});
        lobby_entity.set<FacilityEconomics>({50.0f, 10.0f, 50});

        const auto office1_entity = ecs_world_->CreateEntity("Office_Floor_5");
        office1_entity.set<Position>({0.0f, 50.0f});
        office1_entity.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 0, 8, 20});
        office1_entity.set<Satisfaction>({70.0f});
        office1_entity.set<FacilityEconomics>({150.0f, 30.0f, 20});

        const auto restaurant_entity = ecs_world_->CreateEntity("Restaurant_Floor_3");
        restaurant_entity.set<Position>({0.0f, 30.0f});
        restaurant_entity.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 0, 6, 30});
        restaurant_entity.set<Satisfaction>({65.0f});
        restaurant_entity.set<FacilityEconomics>({200.0f, 60.0f, 30});

        std::cout << "  Created 2 actors and 3 building components with satisfaction and economics" << std::endl;

        // Create HUD and build menu
        hud_ = new HUD();
        build_menu_ = new BuildMenu();
        pause_menu_ = new PauseMenu();
        research_menu_ = new ResearchTreeMenu();
        mods_menu_ = new ModsMenu();
        mods_menu_->SetModManager(&ecs_world_->GetModManager());

        // Connect tooltip manager from HUD to other UI components
        build_menu_->SetTooltipManager(hud_->GetTooltipManager());
        
        // Connect notification center to research menu
        research_menu_->SetNotificationCenter(hud_->GetNotificationCenter());

        // Create and initialize camera
        camera_ = new rendering::Camera();
        camera_->Initialize(800, 600, 1200.0f, 800.0f);

        hud_->SetGameState(game_state_);

        // Add example notifications to showcase notification center
        hud_->AddNotification(Notification::Type::Success, "Welcome to TowerForge!", 10.0f);
        hud_->AddNotification(Notification::Type::Info, "Click entities to view details", 8.0f);
    
        // Add notifications directly to notification center with clickable callbacks
        auto* nc = hud_->GetNotificationCenter();
        if (nc) {
            // Welcome notification
            nc->AddNotification(
                "Welcome to TowerForge",
                "Start building your tower empire! Press N to toggle the notification center.",
                NotificationType::Info,
                NotificationPriority::Medium,
                15.0f
            );
        
            // Tutorial notification
            nc->AddNotification(
                "Getting Started",
                "Use the build menu on the left to place facilities. Click the notification button (top right) or press N to view all notifications.",
                NotificationType::Info,
                NotificationPriority::Medium,
                -1.0f  // Pin important tutorials
            );
        
            // Feature showcase
            nc->AddNotification(
                "Notification Center Features",
                "Pin important notifications, filter by type, and click to trigger actions!",
                NotificationType::Event,
                NotificationPriority::Low,
                20.0f
            );
        }

        // Setup tower grid and facilities
        std::cout << std::endl << "Demonstrating Tower Grid System and Facility Manager..." << std::endl;
        auto &grid = ecs_world_->GetTowerGrid();
        auto &facility_mgr = ecs_world_->GetFacilityManager();

        placement_system_ = new PlacementSystem(grid, facility_mgr, *build_menu_);
        placement_system_->SetCamera(camera_);
        placement_system_->SetTooltipManager(hud_->GetTooltipManager());

        // Create history panel
        history_panel_ = std::make_unique<ui::HistoryPanel>();
        history_panel_->SetVisible(false);  // Hidden by default

        std::cout << "  Initial grid: " << grid.GetFloorCount() << " floors x "
                << grid.GetColumnCount() << " columns" << std::endl;

        // Create facilities
        std::cout << "  Creating facilities..." << std::endl;
        auto grid_lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0, 0, "MainLobby");
        auto grid_office1 = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 2, 0, "Office_Floor_1");
        auto residential1 = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 5, 0, "Condo_Floor_2");
        auto shop1 = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 1, 0, "Shop_Floor_3");

        std::cout << "  Created 4 facilities (Lobby, Office, Residential, RetailShop)" << std::endl;

        // Create elevator system
        std::cout << "  Creating elevator system..." << std::endl;
        const auto elevator_shaft = ecs_world_->CreateEntity("MainElevatorShaft");
        elevator_shaft.set<ElevatorShaft>({10, 0, 5, 1});

        const auto elevator_car = ecs_world_->CreateEntity("Elevator1");
        elevator_car.set<ElevatorCar>({static_cast<int>(elevator_shaft.id()), 0, 8});

        std::cout << "  Created elevator shaft at column 10 serving floors 0-5" << std::endl;

        std::cout << "  Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
        std::cout << "  Facility at (0, 0): " << grid.GetFacilityAt(0, 0) << std::endl;
        std::cout << "  Facility at (1, 5): " << grid.GetFacilityAt(1, 5) << std::endl;

        // Add more floors
        std::cout << "  Adding 5 more floors..." << std::endl;
        grid.AddFloors(5);
        std::cout << "  New grid size: " << grid.GetFloorCount() << " floors x "
                << grid.GetColumnCount() << " columns" << std::endl;

        std::cout << std::endl << "Running simulation..." << std::endl;
        std::cout << std::endl;

        // If this is a new game (not tutorial), create the starter tower
        if (current_state_ == GameState::InGame && !tutorial_active_) {
            CreateStarterTower();
        }

        // If this is tutorial mode, initialize the tutorial manager
        if (current_state_ == GameState::Tutorial) {
            tutorial_manager_ = new TutorialManager();
            tutorial_manager_->Initialize();
            hud_->AddNotification(Notification::Type::Info, "Welcome to the tutorial!", 5.0f);
        }

        // Initialize help system
        help_system_ = new HelpSystem();
        help_system_->Initialize();

        // Reset timing
        elapsed_time_ = 0.0f;
        sim_time_ = 0.0f;
        is_paused_ = false;
        in_settings_from_pause_ = false;
        in_audio_settings_from_pause_ = false;
        in_accessibility_settings_from_pause_ = false;
    }

    void Game::UpdateInGame(float delta_time) {
        // Handle F1 key to toggle help system
        if (help_system_ != nullptr && IsKeyPressed(KEY_F1)) {
            if (help_system_->IsVisible()) {
                help_system_->Hide();
            } else {
                // Determine current context based on active UI
                ui::HelpContext context = ui::HelpContext::MainGame;
                if (is_paused_) {
                    context = ui::HelpContext::PauseMenu;
                } else if (research_menu_ != nullptr && research_menu_->IsVisible()) {
                    context = ui::HelpContext::ResearchTree;
                } else if (mods_menu_ != nullptr && mods_menu_->IsVisible()) {
                    context = ui::HelpContext::ModsMenu;
                }
                help_system_->Show(context);
            }
            return;
        }

        // Handle ESC key to close help, pause menu, or research menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (help_system_ != nullptr && help_system_->IsVisible()) {
                help_system_->Hide();
            } else if (save_load_menu_ != nullptr &&save_load_menu_->IsOpen()) {
                save_load_menu_->Close();
            } else if (research_menu_ != nullptr && research_menu_->IsVisible()) {
                research_menu_->SetVisible(false);
            } else if (!in_settings_from_pause_ && !in_audio_settings_from_pause_ && 
                       !in_accessibility_settings_from_pause_ && !is_paused_) {
                is_paused_ = true;
                audio_manager_->PlaySFX(audio::AudioCue::MenuOpen);
                game_state_.paused = true;
            }
            return;
        }

        // Handle R key to toggle research menu (only if not paused)
        if (research_menu_ != nullptr && !is_paused_ && IsKeyPressed(KEY_R)) {
            research_menu_->Toggle();
        }
    
        // Handle N key to toggle notification center
        if (IsKeyPressed(KEY_N)) {
            hud_->ToggleNotificationCenter();
        }

        // Handle H key to toggle history panel (only if not paused)
        if (history_panel_ != nullptr && !is_paused_ && IsKeyPressed(KEY_H)) {
            history_panel_->ToggleVisible();
        }

        // Only update simulation if not paused
        if (!is_paused_) {
            save_load_manager_->UpdateAutosave(time_step_, *ecs_world_);

            if (!ecs_world_->Update(time_step_)) {
                current_state_ = GameState::Quit;
                return;
            }
            elapsed_time_ += time_step_;
            sim_time_ += time_step_ * game_state_.speed_multiplier;
        }

        // Update game state for HUD
        game_state_.current_time = 8.5f + (sim_time_ / 3600.0f);
        if (game_state_.current_time >= 24.0f) {
            game_state_.current_time -= 24.0f;
            game_state_.current_day++;
        }

        // Only update funds if not paused
        if (!is_paused_) {
            game_state_.funds += (game_state_.income_rate / 3600.0f) * time_step_;
        }

        CalculateTowerRating();

        hud_->SetGameState(game_state_);
        hud_->Update(time_step_);

        // Update help system
        if (help_system_ != nullptr) {
            help_system_->Update(delta_time);
        }

        // Handle research menu
        if (research_menu_->IsVisible()) {
            research_menu_->Update(time_step_);
            
            // Handle mouse events for confirmation dialogs first
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                const ui::MouseEvent mouse_event{
                    static_cast<float>(GetMouseX()),
                    static_cast<float>(GetMouseY()),
                    false, // left_down
                    false, // right_down
                    true,  // left_pressed
                    false  // right_pressed
                };
                
                // Check research menu confirmation dialogs
                if (research_menu_->ProcessMouseEvent(mouse_event)) {
                    // Dialog consumed the event, don't process node clicks
                } else {
                    // Normal node click handling
                    ResearchTree &research_tree_ref = ecs_world_->GetWorld().get_mut<ResearchTree>();
                    const bool unlocked = research_menu_->HandleMouse(GetMouseX(), GetMouseY(),
                                                                      true,  // clicked
                                                                      research_tree_ref);
                    // Note: unlock notification is now handled in ResearchTreeMenu via notification center
                }
            }
        }

        // Check for achievements (only if not paused)
        if (!is_paused_) {
            const flecs::world &world = ecs_world_->GetWorld();
            float total_income = 0.0f;
            if (world.has<TowerEconomy>()) {
                const auto &economy = world.get<TowerEconomy>();
                total_income = economy.total_revenue;
            }

            const int floor_count = ecs_world_->GetTowerGrid().GetFloorCount();
            constexpr float avg_satisfaction = 75.0f;

            achievement_manager_->
                    CheckAchievements(game_state_.population, total_income, floor_count, avg_satisfaction);

            if (achievement_manager_->HasNewAchievements()) {
                const auto newly_unlocked = achievement_manager_->PopNewlyUnlocked();
                for (const auto &achievement_id: newly_unlocked) {
                    audio_manager_->PlaySFX(audio::AudioCue::Achievement);

                    for (const auto &achievement: achievement_manager_->GetAllAchievements()) {
                        if (achievement.id == achievement_id) {
                            std::string message = "Achievement Unlocked: " + achievement.name;
                            hud_->AddNotification(Notification::Type::Success, message, 5.0f);
                        
                            // Also add to notification center with clickable callback
                            auto* nc = hud_->GetNotificationCenter();
                            if (nc) {
                                nc->AddNotification(
                                    achievement.name,
                                    achievement.description,
                                    NotificationType::Achievement,
                                    NotificationPriority::High,
                                    -1.0f,  // Don't auto-dismiss achievements
                                    [this]() {
                                        // When clicked, switch to achievements screen
                                        previous_state_ = current_state_;
                                        current_state_ = GameState::Achievements;
                                    }
                                );
                            }
                            break;
                        }
                    }
                }
            }

            achievements_menu_.SetGameStats(game_state_.population, total_income, floor_count, avg_satisfaction);
        }

        // Handle pause menu input
        if (is_paused_) {
            if (save_load_menu_ != nullptr && save_load_menu_->IsOpen()) {
                save_load_menu_->Update(time_step_);
                save_load_menu_->HandleMouse(GetMouseX(), GetMouseY(),
                                             IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                save_load_menu_->HandleKeyboard();
            } else if (in_audio_settings_from_pause_) {
                pause_audio_settings_menu_.Update(time_step_);

                if (pause_audio_settings_menu_.HandleKeyboard()) {
                    in_audio_settings_from_pause_ = false;
                }
                const bool back_clicked = pause_audio_settings_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
                if (back_clicked) {
                    in_audio_settings_from_pause_ = false;
                }
            } else if (in_settings_from_pause_) {
                pause_general_settings_menu_.Update(time_step_);

                const int keyboard_selection = pause_general_settings_menu_.HandleKeyboard();
                const int mouse_selection = pause_general_settings_menu_.HandleMouse(GetMouseX(), GetMouseY(),
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

                int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;

                if (selected >= 0) {
                    const auto option = static_cast<SettingsOption>(selected);
                    switch (option) {
                        case SettingsOption::Audio:
                            in_audio_settings_from_pause_ = true;
                            break;
                        case SettingsOption::Accessibility:
                            in_accessibility_settings_from_pause_ = true;
                            break;
                        case SettingsOption::Controls:
                        case SettingsOption::Display:
                        case SettingsOption::Gameplay:
                            hud_->AddNotification(Notification::Type::Info, "Settings option not yet implemented",
                                                  3.0f);
                            break;
                        case SettingsOption::Back:
                            in_settings_from_pause_ = false;
                            break;
                    }
                }
            } else {
                pause_menu_->Update(time_step_);

                const int quit_result = pause_menu_->HandleQuitConfirmation();
                if (quit_result == 1) {
                    current_state_ = GameState::TitleScreen;
                    CleanupGameSystems();
                    game_initialized_ = false;
                    return;
                } else if (quit_result == 0) {
                    // User cancelled quit
                } else {
                    const int keyboard_selection = pause_menu_->HandleKeyboard();
                    const int mouse_selection = pause_menu_->HandleMouse(GetMouseX(), GetMouseY(),
                                                                         IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

                    int selected = (keyboard_selection >= 0) ? keyboard_selection : mouse_selection;

                    if (selected >= 0) {
                        const auto option = static_cast<PauseMenuOption>(selected);
                        switch (option) {
                            case PauseMenuOption::Resume:
                                is_paused_ = false;
                                game_state_.paused = false;
                                break;
                            case PauseMenuOption::SaveGame:
                                save_load_menu_->Open(true);
                                //hud_->AddNotification(Notification::Type::Info, "Save game not yet implemented", 3.0f);
                                break;
                            case PauseMenuOption::LoadGame:
                                save_load_menu_->Open(false);
                                break;
                            case PauseMenuOption::Settings:
                                in_settings_from_pause_ = true;
                                break;
                            case PauseMenuOption::Mods:
                                mods_menu_->Show();
                                break;
                            case PauseMenuOption::QuitToTitle:
                                pause_menu_->ShowQuitConfirmation(true);
                                break;
                        }
                    }
                }
            }
        }

        // Update placement system

        // Handle keyboard shortcuts (only if not paused)
        if (!is_paused_) {
            placement_system_->Update(time_step_);
            placement_system_->HandleKeyboard();
            
            // Check for pending demolish from confirmation dialog
            const int pending_change = placement_system_->GetPendingFundsChange();
            if (pending_change != 0) {
                game_state_.funds += pending_change;
                audio_manager_->PlaySFX(audio::AudioCue::FacilityDemolish);
                hud_->AddNotification(Notification::Type::Info,
                    TextFormat("Facility demolished! Refund: $%d", pending_change), 3.0f);
            }
            
            // Update history panel with current command history
            if (history_panel_ != nullptr && history_panel_->IsVisible()) {
                history_panel_->UpdateFromHistory(placement_system_->GetCommandHistory());
            }
            
            camera_->Update(time_step_);
        }

        HandleInGameInput();
    }

    void Game::HandleInGameInput() {
        // Update tooltips for mouse position (even when paused, for UI tooltips)
        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();

        // Handle help system mouse input first (if visible)
        if (help_system_ != nullptr && help_system_->IsVisible()) {
            help_system_->HandleMouse(mouse_x, mouse_y, IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
            return;  // Help system consumes all input when visible
        }

        // Update HUD tooltips
        hud_->UpdateTooltips(mouse_x, mouse_y);

        // Update build menu tooltips
        build_menu_->UpdateTooltips(mouse_x, mouse_y, game_state_.funds);

        // Update placement tooltips (if not paused and not in research menu)
        if (!is_paused_ && !research_menu_->IsVisible()) {
            float world_x, world_y;
            camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);
            placement_system_->UpdateTooltips(static_cast<int>(world_x), static_cast<int>(world_y),
                                              grid_offset_x_, grid_offset_y_,
                                              cell_width_, cell_height_, game_state_.funds);
        }

        // Handle mouse clicks (only if not paused)
        if (!is_paused_ && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Create mouse event for UI system
            const ui::MouseEvent mouse_event{
                static_cast<float>(mouse_x),
                static_cast<float>(mouse_y),
                false, // left_down
                false, // right_down
                true,  // left_pressed
                false  // right_pressed
            };
            
            // Check placement system confirmation dialogs first
            if (placement_system_->ProcessMouseEvent(mouse_event)) {
                return;  // Dialog consumed the event
            }
            
            auto &grid = ecs_world_->GetTowerGrid();

            // Check history panel first (if visible)
            if (history_panel_ != nullptr && history_panel_->IsVisible() && 
                history_panel_->IsMouseOver(mouse_x, mouse_y)) {
                const int steps = history_panel_->HandleClick(mouse_x, mouse_y);
                if (steps > 0) {
                    // Undo 'steps' times
                    int success_count = 0;
                    for (int i = 0; i < steps; i++) {
                        if (placement_system_->Undo(game_state_.funds)) {
                            success_count++;
                        } else {
                            break;
                        }
                    }
                    if (success_count > 0) {
                        hud_->AddNotification(Notification::Type::Info, 
                            TextFormat("Undid %d action(s)", success_count), 2.0f);
                    }
                } else if (steps < 0) {
                    // Redo 'steps' times
                    int success_count = 0;
                    for (int i = 0; i < -steps; i++) {
                        if (placement_system_->Redo(game_state_.funds)) {
                            success_count++;
                        } else {
                            break;
                        }
                    }
                    if (success_count > 0) {
                        hud_->AddNotification(Notification::Type::Info, 
                            TextFormat("Redid %d action(s)", success_count), 2.0f);
                    }
                }
                return;  // Don't process other clicks
            }

            const int menu_result = build_menu_->HandleClick(mouse_x, mouse_y,
                                                             placement_system_->CanUndo(),
                                                             placement_system_->CanRedo());
            if (menu_result >= 0) {
                hud_->AddNotification(Notification::Type::Info, "Facility selected from menu", 3.0f);
            } else if (menu_result == -2) {
                placement_system_->SetDemolishMode(!placement_system_->IsDemolishMode());
                hud_->AddNotification(Notification::Type::Info,
                                      placement_system_->IsDemolishMode() ? "Demolish mode ON" : "Demolish mode OFF",
                                      3.0f);
            } else if (menu_result == -3) {
                // Undo button clicked
                if (placement_system_->Undo(game_state_.funds)) {
                    hud_->AddNotification(Notification::Type::Info, "Undid last action", 2.0f);
                } else {
                    hud_->AddNotification(Notification::Type::Warning, "Cannot undo (insufficient funds or nothing to undo)", 2.0f);
                }
            } else if (menu_result == -4) {
                // Redo button clicked
                if (placement_system_->Redo(game_state_.funds)) {
                    hud_->AddNotification(Notification::Type::Info, "Redid action", 2.0f);
                } else {
                    hud_->AddNotification(Notification::Type::Warning, "Cannot redo (insufficient funds or nothing to redo)", 2.0f);
                }
            } else if (menu_result == -5) {
                // Add floor
                const int floor_cost = TowerGrid::GetFloorBuildCost() * grid.GetColumnCount();
                if (game_state_.funds >= floor_cost) {
                    grid.AddFloor();
                    game_state_.funds -= floor_cost;
                    hud_->AddNotification(Notification::Type::Success,
                                          TextFormat("Floor added! Cost: $%d", floor_cost), 3.0f);
                } else {
                    hud_->AddNotification(Notification::Type::Error,
                                          TextFormat("Not enough funds! Need $%d", floor_cost), 3.0f);
                }
            } else if (menu_result == -6) {
                // Add basement
                const int basement_cost = TowerGrid::GetFloorBuildCost() * grid.GetColumnCount();
                if (game_state_.funds >= basement_cost) {
                    grid.AddBasementFloor();
                    game_state_.funds -= basement_cost;
                    hud_->AddNotification(Notification::Type::Success,
                                          TextFormat("Basement added! Cost: $%d", basement_cost), 3.0f);
                } else {
                    hud_->AddNotification(Notification::Type::Error,
                                          TextFormat("Not enough funds! Need $%d", basement_cost), 3.0f);
                }
            } else if (!hud_->HandleClick(mouse_x, mouse_y)) {
                // Convert screen coordinates to world coordinates for camera-transformed clicks
                float world_x, world_y;
                camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);

                const int cost_change = placement_system_->HandleClick(static_cast<int>(world_x),
                                                                       static_cast<int>(world_y),
                                                                       grid_offset_x_, grid_offset_y_, cell_width_,
                                                                       cell_height_, game_state_.funds);

                if (cost_change != 0) {
                    game_state_.funds += cost_change;
                    if (cost_change < 0) {
                        audio_manager_->PlaySFX(audio::AudioCue::FacilityPlace);
                        hud_->AddNotification(Notification::Type::Success,
                                              TextFormat("Facility placed! Cost: $%d", -cost_change), 3.0f);

                        // Notify tutorial if active
                        if (tutorial_active_ && tutorial_manager_) {
                            const int selected = build_menu_->GetSelectedFacility();
                            if (selected >= 0) {
                                const auto &facility_types = build_menu_->GetFacilityTypes();
                                if (selected < static_cast<int>(facility_types.size())) {
                                    tutorial_manager_->OnFacilityPlaced(facility_types[selected].name);
                                }
                            }
                        }
                    } else {
                        audio_manager_->PlaySFX(audio::AudioCue::FacilityDemolish);
                        hud_->AddNotification(Notification::Type::Info,
                                              TextFormat("Facility demolished! Refund: $%d", cost_change), 3.0f);
                    }
                } else {
                    // Check if placement was attempted but failed
                    const int selected = build_menu_->GetSelectedFacility();
                    if (selected >= 0 && !placement_system_->IsDemolishMode()) {
                        // Placement was attempted but failed - provide feedback
                        const auto &facility_types = build_menu_->GetFacilityTypes();
                        if (selected < static_cast<int>(facility_types.size())) {
                            const auto& facility_type = facility_types[selected];
                            
                            // Check specific reason for failure
                            const int rel_x = static_cast<int>(world_x) - grid_offset_x_;
                            const int rel_y = static_cast<int>(world_y) - grid_offset_y_;
                            
                            if (rel_x >= 0 && rel_y >= 0) {
                                const int clicked_floor = rel_y / cell_height_;
                                const int clicked_column = rel_x / cell_width_;
                                
                                if (clicked_floor >= 0 && clicked_floor < grid.GetFloorCount() &&
                                    clicked_column >= 0 && clicked_column < grid.GetColumnCount()) {
                                    
                                    const int floor_build_cost = ecs_world_->GetFacilityManager().CalculateFloorBuildCost(
                                        clicked_floor, clicked_column, facility_type.width);
                                    const int total_cost = facility_type.cost + floor_build_cost;
                                    
                                    if (game_state_.funds < total_cost) {
                                        hud_->AddNotification(Notification::Type::Error,
                                            TextFormat("Insufficient funds! Need $%d (have $%.0f)", 
                                                total_cost, game_state_.funds), 3.0f);
                                    } else if (!grid.IsSpaceAvailable(clicked_floor, clicked_column, facility_type.width)) {
                                        hud_->AddNotification(Notification::Type::Warning,
                                            "Cannot place facility here - space not available", 3.0f);
                                    }
                                }
                            }
                        }
                    }
                    
                    // Original code for entity selection continues below...
                    const int rel_x = static_cast<int>(world_x) - grid_offset_x_;
                    const int rel_y = static_cast<int>(world_y) - grid_offset_y_;

                    if (rel_x >= 0 && rel_y >= 0) {
                        const int clicked_floor = rel_y / cell_height_;
                        const int clicked_column = rel_x / cell_width_;

                        if (clicked_floor >= 0 && clicked_floor < grid.GetFloorCount() &&
                            clicked_column >= 0 && clicked_column < grid.GetColumnCount()) {
                            // Check if click is on a Person entity
                            bool person_clicked = false;
                            const auto person_query = ecs_world_->GetWorld().query<const Person>();
                            person_query.each([&](const flecs::entity e, const Person &person) {
                                // Calculate person position on screen
                                const int person_x =
                                        grid_offset_x_ + static_cast<int>(person.current_column * cell_width_);
                                const int person_y = grid_offset_y_ + person.current_floor * cell_height_;

                                // Check if click is within person's bounds (circle with radius 10)
                                const int dx = static_cast<int>(world_x) - (person_x + cell_width_ / 2);
                                const int dy = static_cast<int>(world_y) - (person_y + cell_height_ / 2);
                                if (dx * dx + dy * dy <= 100) {
                                    // radius of 10 pixels squared
                                    // Create PersonInfo and show in HUD
                                    PersonInfo info;
                                    info.id = static_cast<int>(e.id());
                                    info.name = person.name;
                                    info.npc_type = (person.npc_type == NPCType::Visitor) ? "Visitor" : "Employee";
                                    info.state = person.GetStateString();
                                    info.current_floor = person.current_floor;
                                    info.destination_floor = person.destination_floor;
                                    info.wait_time = person.wait_time;
                                    info.needs = person.current_need;
                                    info.is_staff = false;
                                    info.staff_role = "";
                                    info.on_duty = false;
                                    info.shift_hours = "";

                                    // Check if this is staff
                                    if (e.has<StaffAssignment>()) {
                                        const StaffAssignment& assignment = e.ensure<StaffAssignment>();
                                        info.is_staff = true;
                                        info.npc_type = "Staff";
                                        info.staff_role = assignment.GetRoleName();
                                        info.on_duty = assignment.is_active;
                                        
                                        // Format shift hours
                                        char shift_buffer[32];
                                        snprintf(shift_buffer, sizeof(shift_buffer), "%.0f:00 - %.0f:00",
                                                assignment.shift_start_time, assignment.shift_end_time);
                                        info.shift_hours = shift_buffer;
                                        
                                        info.status = info.on_duty ? 
                                            (std::string("On duty: ") + info.staff_role) : 
                                            (std::string("Off duty: ") + info.staff_role);
                                    }
                                    // Get status based on NPC type
                                    else if (person.npc_type == NPCType::Employee && e.has<EmploymentInfo>()) {
                                        const EmploymentInfo &emp = e.ensure<EmploymentInfo>();
                                        info.status = emp.GetStatusString();
                                    } else if (person.npc_type == NPCType::Visitor && e.has<VisitorInfo>()) {
                                        const VisitorInfo &visitor = e.ensure<VisitorInfo>();
                                        info.status = visitor.GetActivityString();
                                    } else {
                                        info.status = person.current_need;
                                    }

                                    // Get satisfaction if available
                                    if (e.has<Satisfaction>()) {
                                        const Satisfaction &sat = e.ensure<Satisfaction>();
                                        info.satisfaction = sat.satisfaction_score;
                                    } else {
                                        info.satisfaction = 75.0f;
                                    }

                                    hud_->ShowPersonInfo(info);
                                    person_clicked = true;
                                }
                            });

                            // If no person was clicked, check for facility
                            if (!person_clicked && grid.IsOccupied(clicked_floor, clicked_column)) {
                                // Get facility entity at this location
                                const int facility_id = grid.GetFacilityAt(clicked_floor, clicked_column);
                                if (facility_id >= 0) {
                                    const flecs::entity facility_entity = ecs_world_->GetWorld().entity(
                                        static_cast<flecs::entity_t>(facility_id));
                                    
                                    FacilityInfo info;
                                    info.type = "FACILITY";
                                    info.floor = clicked_floor;
                                    info.occupancy = 0;
                                    info.max_occupancy = 10;
                                    info.revenue = 100.0f;
                                    info.satisfaction = 75.0f;
                                    info.tenant_count = 0;
                                    
                                    // Get actual facility data if available
                                    if (facility_entity.is_alive() && facility_entity.has<BuildingComponent>()) {
                                        const BuildingComponent& facility = facility_entity.ensure<BuildingComponent>();
                                        info.occupancy = facility.current_occupancy;
                                        info.max_occupancy = facility.capacity;
                                        
                                        // Get facility type name
                                        switch (facility.type) {
                                            case BuildingComponent::Type::Office:      info.type = "Office"; break;
                                            case BuildingComponent::Type::Residential: info.type = "Residential"; break;
                                            case BuildingComponent::Type::RetailShop:  info.type = "Retail Shop"; break;
                                            case BuildingComponent::Type::Lobby:       info.type = "Lobby"; break;
                                            case BuildingComponent::Type::Restaurant:  info.type = "Restaurant"; break;
                                            case BuildingComponent::Type::Hotel:       info.type = "Hotel"; break;
                                            case BuildingComponent::Type::Elevator:    info.type = "Elevator"; break;
                                            case BuildingComponent::Type::Gym:         info.type = "Gym"; break;
                                            case BuildingComponent::Type::Arcade:      info.type = "Arcade"; break;
                                            default: info.type = "Facility"; break;
                                        }
                                    }
                                    
                                    // Get economics data
                                    if (facility_entity.is_alive() && facility_entity.has<FacilityEconomics>()) {
                                        const FacilityEconomics& econ = facility_entity.ensure<FacilityEconomics>();
                                        info.revenue = econ.CalculateDailyRevenue();
                                        info.tenant_count = econ.current_tenants;
                                    }
                                    
                                    // Get satisfaction
                                    if (facility_entity.is_alive() && facility_entity.has<Satisfaction>()) {
                                        const Satisfaction& sat = facility_entity.ensure<Satisfaction>();
                                        info.satisfaction = sat.satisfaction_score;
                                    }
                                    
                                    // Get facility status (cleanliness and maintenance)
                                    if (facility_entity.is_alive() && facility_entity.has<FacilityStatus>()) {
                                        const FacilityStatus& status = facility_entity.ensure<FacilityStatus>();
                                        info.cleanliness = status.cleanliness;
                                        info.maintenance_level = status.maintenance_level;
                                        info.cleanliness_rating = status.GetCleanlinessRating();
                                        info.maintenance_rating = status.GetMaintenanceRating();
                                        info.has_fire = status.has_fire;
                                        info.has_security_issue = status.has_security_issue;
                                    } else {
                                        info.cleanliness = 100.0f;
                                        info.maintenance_level = 100.0f;
                                        info.cleanliness_rating = "Spotless";
                                        info.maintenance_rating = "Excellent";
                                        info.has_fire = false;
                                        info.has_security_issue = false;
                                    }
                                    
                                    // Get CleanlinessStatus state information
                                    if (facility_entity.is_alive() && facility_entity.has<CleanlinessStatus>()) {
                                        const CleanlinessStatus& cleanliness = facility_entity.ensure<CleanlinessStatus>();
                                        info.cleanliness_state = cleanliness.GetStateString();
                                        info.needs_cleaning = cleanliness.NeedsCleaning();
                                        // Override cleanliness percentage with state-based value if available
                                        info.cleanliness = cleanliness.GetCleanlinessPercent();
                                    } else {
                                        info.cleanliness_state = "";
                                        info.needs_cleaning = false;
                                    }
                                    
                                    hud_->ShowFacilityInfo(info);
                                }
                            }
                        }
                    }
                }
            }
        }

        // Handle camera input
        constexpr bool hud_handled_input = false;
        camera_->HandleInput(hud_handled_input);
    }

    void Game::RenderInGame() {
        const auto &grid = ecs_world_->GetTowerGrid();

        renderer_.BeginFrame();
        renderer_.Clear(DARKGRAY);

        // Begin camera mode for all game world rendering
        camera_->BeginMode();

        // Draw grid
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            for (int col = 0; col < grid.GetColumnCount(); ++col) {
                const int x = grid_offset_x_ + col * cell_width_;
                const int y = grid_offset_y_ + floor * cell_height_;

                // Show built floors with solid outline, unbuilt with dashed/faded outline
                if (grid.IsFloorBuilt(floor, col)) {
                    DrawRectangleLines(x, y, cell_width_, cell_height_, ColorAlpha(WHITE, 0.2f));
                } else {
                    // Draw faded outline for unbuilt floors
                    DrawRectangleLines(x, y, cell_width_, cell_height_, ColorAlpha(DARKGRAY, 0.1f));
                }

                if (grid.IsOccupied(floor, col)) {
                    const int facility_id = grid.GetFacilityAt(floor, col);
                    auto facility_color = SKYBLUE;
                    if (facility_id % 3 == 0) facility_color = PURPLE;
                    else if (facility_id % 3 == 1) facility_color = GREEN;

                    DrawRectangle(x + 2, y + 2, cell_width_ - 4, cell_height_ - 4, facility_color);
                }
            }
        }

        // Draw floor labels
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            const int y = grid_offset_y_ + floor * cell_height_;
            DrawText(TextFormat("F%d", floor), grid_offset_x_ - 30, y + 15, 12, LIGHTGRAY);
        }

        // Draw elevator shafts
        const auto shaft_query = ecs_world_->GetWorld().query<const ElevatorShaft>();
        shaft_query.each([&](flecs::entity e, const ElevatorShaft &shaft) {
            for (int floor = shaft.bottom_floor; floor <= shaft.top_floor; ++floor) {
                const int x = grid_offset_x_ + shaft.column * cell_width_;
                const int y = grid_offset_y_ + floor * cell_height_;

                DrawRectangle(x + 4, y + 4, cell_width_ - 8, cell_height_ - 8, Color{60, 60, 70, 255});
                DrawRectangleLines(x + 4, y + 4, cell_width_ - 8, cell_height_ - 8, Color{100, 100, 120, 255});
            }
        });

        // Draw elevator cars
        const auto car_query = ecs_world_->GetWorld().query<const ElevatorCar>();
        car_query.each([&](flecs::entity e, const ElevatorCar &car) {
            const auto shaft_entity = ecs_world_->GetWorld().entity(car.shaft_entity_id);
            if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                const ElevatorShaft &shaft = shaft_entity.ensure<ElevatorShaft>();

                const int x = grid_offset_x_ + shaft.column * cell_width_;
                const int y = grid_offset_y_ + static_cast<int>(car.current_floor * cell_height_);

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

                DrawRectangle(x + 6, y + 6, cell_width_ - 12, cell_height_ - 12, car_color);

                if (car.current_occupancy > 0) {
                    DrawText(TextFormat("%d", car.current_occupancy), x + 16, y + 18, 14, BLACK);
                }
            }
        });

        // Draw Person entities
        const auto person_query = ecs_world_->GetWorld().query<const Person>();
        person_query.each([&](const flecs::entity e, const Person &person) {
            const int x = grid_offset_x_ + static_cast<int>(person.current_column * cell_width_);
            const int y = grid_offset_y_ + person.current_floor * cell_height_;

            // Draw person as a circle
            Color person_color;
            if (person.npc_type == NPCType::Employee) {
                // Employees are blue
                if (e.has<EmploymentInfo>()) {
                    const EmploymentInfo &emp = e.ensure<EmploymentInfo>();
                    person_color = emp.currently_on_shift ? BLUE : SKYBLUE;
                } else {
                    person_color = BLUE;
                }
            } else {
                // Visitors are orange/yellow
                if (e.has<VisitorInfo>()) {
                    const VisitorInfo &visitor = e.ensure<VisitorInfo>();
                    if (visitor.activity == VisitorActivity::Leaving) {
                        person_color = GRAY;
                    } else if (visitor.activity == VisitorActivity::Shopping) {
                        person_color = GOLD;
                    } else {
                        person_color = ORANGE;
                    }
                } else {
                    person_color = ORANGE;
                }
            }

            DrawCircle(x + cell_width_ / 2, y + cell_height_ / 2, 8, person_color);

            // Draw outline to make them more visible
            DrawCircleLines(x + cell_width_ / 2, y + cell_height_ / 2, 8, BLACK);
        });

        // Render placement system preview
        placement_system_->Render(grid_offset_x_, grid_offset_y_, cell_width_, cell_height_);

        // End camera mode
        camera_->EndMode();

        // Render HUD and menus
        hud_->Render();
        build_menu_->Render(placement_system_->CanUndo(), placement_system_->CanRedo(),
                            placement_system_->IsDemolishMode());

        // Render history panel (if visible)
        if (history_panel_ != nullptr) {
            history_panel_->Render();
        }

        camera_->RenderControlsOverlay();
        camera_->RenderFollowIndicator();

        // Display tower economy status
        /*const auto& tower_economy = ecs_world_->GetWorld().get<TowerEconomy>();
    renderer_.DrawRectangle(10, 140, 280, 100, Color{0, 0, 0, 180});
    renderer_.DrawText("Tower Economics", 20, 145, 18, GOLD);

    std::string balance_str = "Balance: $" + std::to_string(static_cast<int>(tower_economy.total_balance));
    std::string revenue_str = "Revenue: $" + std::to_string(static_cast<int>(tower_economy.daily_revenue));
    std::string expense_str = "Expenses: $" + std::to_string(static_cast<int>(tower_economy.daily_expenses));

    renderer_.DrawText(balance_str.c_str(), 20, 170, 16, GREEN);
    renderer_.DrawText(revenue_str.c_str(), 20, 195, 16, SKYBLUE);
    renderer_.DrawText(expense_str.c_str(), 20, 220, 16, ORANGE);*/

        // Display satisfaction indicators
        /*int y_offset = 250;
    auto actor_query = ecs_world_->GetWorld().query<const Actor, const Satisfaction>();
    actor_query.each([&](flecs::entity e, const Actor& actor, const Satisfaction& sat) {
        if (y_offset < 520) {
            renderer_.DrawRectangle(10, y_offset, 280, 50, Color{0, 0, 0, 180});

            std::string name_str = actor.name + " Satisfaction";
            renderer_.DrawText(name_str.c_str(), 20, y_offset + 5, 16, WHITE);

            std::string score_str = std::to_string(static_cast<int>(sat.satisfaction_score)) + "% - " + sat.GetLevelString();

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

            renderer_.DrawText(score_str.c_str(), 20, y_offset + 25, 16, sat_color);
            y_offset += 55;
        }
    });*/

        // Render pause menu overlay if paused
        if (is_paused_) {
            if (save_load_menu_->IsOpen()) {
                save_load_menu_->Render();
            } else if (in_accessibility_settings_from_pause_) {
                pause_accessibility_settings_menu_.Render();
            } else if (in_audio_settings_from_pause_) {
                pause_audio_settings_menu_.Render();
            } else if (in_settings_from_pause_) {
                pause_general_settings_menu_.Render();
            } else if (mods_menu_->IsVisible()) {
                mods_menu_->Render();
            } else {
                pause_menu_->Render();
            }
        }

        // Render research menu overlay if visible
        if (research_menu_->IsVisible()) {
            ResearchTree &research_tree_ref = ecs_world_->GetWorld().get_mut<ResearchTree>();
            research_menu_->Render(research_tree_ref);
        }

        // Render help system overlay if visible (render last, on top of everything)
        if (help_system_ != nullptr && help_system_->IsVisible()) {
            help_system_->Render();
        }

        renderer_.EndFrame();
    }

    void Game::CleanupGameSystems() {
        if (!game_initialized_) {
            return;
        }

        // Perform final autosave before cleanup
        if (save_load_manager_ && save_load_manager_->IsAutosaveEnabled() && ecs_world_) {
            std::cout << "Performing final autosave before exit..." << std::endl;
            const auto result = save_load_manager_->Autosave(*ecs_world_);
            if (result.success) {
                std::cout << "Final autosave completed successfully" << std::endl;
            }
        }

        delete placement_system_;
        delete camera_;
        delete research_menu_;
        delete save_load_menu_;
        delete mods_menu_;
        delete pause_menu_;
        delete build_menu_;
        delete hud_;
        delete save_load_manager_;
        delete ecs_world_;
        delete tutorial_manager_;
        delete help_system_;

        placement_system_ = nullptr;
        history_panel_.reset();  // Explicit reset for clarity, though automatic
        camera_ = nullptr;
        research_menu_ = nullptr;
        save_load_menu_ = nullptr;
        mods_menu_ = nullptr;
        pause_menu_ = nullptr;
        build_menu_ = nullptr;
        hud_ = nullptr;
        save_load_manager_ = nullptr;
        ecs_world_ = nullptr;
        tutorial_manager_ = nullptr;
        help_system_ = nullptr;
    }

    void Game::CalculateTowerRating() {
        if (ecs_world_) {
            CalculateTowerRatingHelper(game_state_.rating, *ecs_world_, game_state_.income_rate);
        }
    }

    void Game::UpdateTutorial(const float delta_time) {
        // Initialize tutorial manager if needed
        if (!tutorial_manager_) {
            tutorial_manager_ = new TutorialManager();
            tutorial_manager_->Initialize();
        }

        tutorial_manager_->Update(delta_time);

        // Update build menu tutorial mode
        if (build_menu_) {
            const std::string highlighted = tutorial_manager_->GetHighlightedFacility();
            build_menu_->SetTutorialMode(true, highlighted);
        }

        // Update game systems (same as InGame)
        UpdateInGame(delta_time);

        // Handle tutorial-specific input
        HandleTutorialInput();
    }

    void Game::RenderTutorial() {
        // Render game (same as InGame)
        RenderInGame();

        // Render tutorial overlay on top
        if (tutorial_manager_) {
            tutorial_manager_->Render();
        }
    }

    void Game::HandleTutorialInput() {
        if (tutorial_manager_) {
            // Check if tutorial should be skipped/exited
            if (tutorial_manager_->HandleInput()) {
                // Tutorial skipped - transition to normal game with starter tower
                tutorial_active_ = false;
                build_menu_->SetTutorialMode(false);
                delete tutorial_manager_;
                tutorial_manager_ = nullptr;
                CreateStarterTower();
                current_state_ = GameState::InGame;
                hud_->AddNotification(Notification::Type::Info, "Tutorial skipped - Good luck!", 5.0f);
                return;
            }

            // Check if tutorial is complete
            if (tutorial_manager_->IsComplete()) {
                tutorial_active_ = false;
                build_menu_->SetTutorialMode(false);
                delete tutorial_manager_;
                tutorial_manager_ = nullptr;
                current_state_ = GameState::InGame;
                hud_->AddNotification(Notification::Type::Success, "Tutorial complete! Keep building!", 5.0f);
                return;
            }
        }
    }

    void Game::CreateStarterTower() const {
        if (!ecs_world_) return;

        auto &grid = ecs_world_->GetTowerGrid();
        auto &facility_mgr = ecs_world_->GetFacilityManager();

        std::cout << "Creating starter tower setup..." << std::endl;

        // Ensure we have enough floors for the starter tower
        // We need floors 0-2 (ground, 1st, 2nd)
        const int current_floors = grid.GetFloorCount();
        if (current_floors < 3) {
            const int floors_to_add = 3 - current_floors;
            grid.AddFloors(floors_to_add);
            std::cout << "  Added " << floors_to_add << " floors to tower" << std::endl;
        }

        // Build the starter tower as specified:
        // Floor 0: Lobby (already built by default)
        // Floor 1: Business (Office)
        // Floor 1 (different position): Shop (RetailShop)
        // Floors 0-2: Stair (not implemented as separate type yet)
        // Floor 2: Condo (Residential)
        // Floors 0-2: Elevator

        try {
            // Place Lobby at floor 0, column 0
            const auto lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0);
            if (lobby) {
                std::cout << "  Placed Lobby at (0,0)" << std::endl;
            }

            // Place Office at floor 1, column 0
            const auto office = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 0);
            if (office) {
                std::cout << "  Placed Office at (1,0)" << std::endl;
            }

            // Place RetailShop at floor 1, column 5
            const auto retail = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 1, 5);
            if (retail) {
                std::cout << "  Placed Retail at (1,5)" << std::endl;
            }

            // Place Residential at floor 2, column 0
            const auto residential = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 0);
            if (residential) {
                std::cout << "  Placed Residential at (2,0)" << std::endl;
            }

            // Place Elevator at floor 0, column 8
            const auto elevator = facility_mgr.CreateFacility(BuildingComponent::Type::Elevator, 0, 8);
            if (elevator) {
                std::cout << "  Placed Elevator at (0,8)" << std::endl;
            }

            std::cout << "Starter tower created successfully" << std::endl;
            hud_->AddNotification(Notification::Type::Info, "Starter tower ready!", 5.0f);
        
            // Create initial staff for the tower
            std::cout << "Hiring initial staff..." << std::endl;
        
            // Hire a janitor for general cleaning (tower-wide)
            const auto janitor = ecs_world_->CreateEntity("Bob the Janitor");
            janitor.set<Person>({"Bob", 0, 3.0f, 2.0f, NPCType::Employee});
            janitor.set<StaffAssignment>({StaffRole::Janitor, -1, 6.0f, 18.0f});
            std::cout << "  Hired janitor: Bob (6:00 AM - 6:00 PM, tower-wide)" << std::endl;
        
            // Hire a maintenance technician (tower-wide)
            const auto maintenance = ecs_world_->CreateEntity("Carlos the Maintenance Tech");
            maintenance.set<Person>({"Carlos", 0, 4.0f, 2.0f, NPCType::Employee});
            maintenance.set<StaffAssignment>({StaffRole::Maintenance, -1, 8.0f, 17.0f});
            std::cout << "  Hired maintenance tech: Carlos (8:00 AM - 5:00 PM, tower-wide)" << std::endl;
        
            // Hire a firefighter (tower-wide, 24-hour shift)
            const auto firefighter = ecs_world_->CreateEntity("Dana the Firefighter");
            firefighter.set<Person>({"Dana", 0, 2.0f, 2.0f, NPCType::Employee});
            firefighter.set<StaffAssignment>({StaffRole::Firefighter, -1, 0.0f, 24.0f});
            std::cout << "  Hired firefighter: Dana (24/7, tower-wide)" << std::endl;
        
            // Add FacilityStatus to all facilities
            ecs_world_->GetWorld().each<BuildingComponent>([&](flecs::entity facility_entity, BuildingComponent& facility) {
                if (!facility_entity.has<FacilityStatus>()) {
                    facility_entity.set<FacilityStatus>({});
                }
            });
        
            std::cout << "Initial staff hired" << std::endl;
            hud_->AddNotification(Notification::Type::Info, "Staff hired!", 3.0f);
        
            // Create staff from custom Lua roles (demo purposes - one of each type)
            const auto& custom_roles = ecs_world_->GetModManager().GetCustomStaffRoles();
            int custom_staff_count = 0;
            for (const auto& [role_id, role_data] : custom_roles) {
                // Create one staff member for each custom role
                const std::string staff_name = role_data.name + " #" + std::to_string(custom_staff_count + 1);
                const auto custom_staff = ecs_world_->CreateEntity(staff_name.c_str());
                custom_staff.set<Person>({staff_name, 0, 3.0f, 2.0f, NPCType::Employee});
            
                // Create custom staff assignment
                StaffAssignment assignment(StaffRole::Janitor, -1, role_data.shift_start_hour, role_data.shift_end_hour);
                assignment.custom_role_id = role_id;
                assignment.work_type = role_data.work_type;
                assignment.work_efficiency = role_data.work_efficiency;
                custom_staff.set<StaffAssignment>(assignment);
            
                std::cout << "  Hired custom staff: " << staff_name << " (" << role_data.name 
                        << ", " << role_data.shift_start_hour << ":00 - " << role_data.shift_end_hour 
                        << ":00, work type: " << role_data.work_type << ")" << std::endl;
                custom_staff_count++;
            }
        
            if (custom_staff_count > 0) {
                std::cout << "Hired " << custom_staff_count << " custom staff from mods" << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error creating starter tower: " << e.what() << std::endl;
        }
    }
}
