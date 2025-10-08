#pragma once

#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/save_load_manager.hpp"
#include "core/achievement_manager.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"
#include "ui/main_menu.h"
#include "ui/pause_menu.h"
#include "ui/save_load_menu.h"
#include "ui/research_tree_menu.h"
#include "ui/achievements_menu.h"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"
#include "ui/tutorial_manager.h"
#include "audio/audio_manager.h"

namespace towerforge {
namespace core {

/**
 * @brief Game state enumeration
 */
enum class GameState {
    TitleScreen,
    Tutorial,
    InGame,
    Settings,
    Credits,
    Achievements,
    Quit
};

/**
 * @brief Main game class that manages game states and logic
 */
class Game {
public:
    Game();
    ~Game();

    /**
     * @brief Initialize the game
     * @return true if initialization succeeded, false otherwise
     */
    bool Initialize();

    /**
     * @brief Run the main game loop
     */
    void Run();

    /**
     * @brief Shutdown and cleanup
     */
    void Shutdown();

private:
    // State management
    void UpdateTitleScreen(float delta_time);
    void RenderTitleScreen();
    void HandleTitleScreenInput();

    void UpdateAchievementsScreen(float delta_time);
    void RenderAchievementsScreen();
    void HandleAchievementsInput();

    void UpdateSettingsScreen(float delta_time);
    void RenderSettingsScreen();
    void HandleSettingsInput();

    void UpdateCreditsScreen(float delta_time);
    void RenderCreditsScreen();
    void HandleCreditsInput();

    void UpdateInGame(float delta_time);
    void RenderInGame();
    void HandleInGameInput();

    void UpdateTutorial(float delta_time);
    void RenderTutorial();
    void HandleTutorialInput();

    // Game initialization for InGame state
    void InitializeGameSystems();
    void CleanupGameSystems();
    void CreateStarterTower();

    // Helper methods
    void CalculateTowerRating();

    // Game state
    GameState current_state_;
    GameState previous_state_;

    // Core systems (always active)
    towerforge::rendering::Renderer renderer_;
    towerforge::audio::AudioManager* audio_manager_;

    // UI for title screen
    towerforge::ui::MainMenu main_menu_;
    towerforge::ui::AchievementsMenu achievements_menu_;
    towerforge::ui::GeneralSettingsMenu general_settings_menu_;
    towerforge::ui::AudioSettingsMenu audio_settings_menu_;

    // Tutorial system
    towerforge::ui::TutorialManager* tutorial_manager_;
    bool tutorial_active_;

    // Settings screen state
    bool in_audio_settings_;

    // InGame systems (initialized on demand)
    TowerForge::Core::ECSWorld* ecs_world_;
    TowerForge::Core::SaveLoadManager* save_load_manager_;
    TowerForge::Core::AchievementManager* achievement_manager_;
    towerforge::ui::HUD* hud_;
    towerforge::ui::BuildMenu* build_menu_;
    towerforge::ui::PauseMenu* pause_menu_;
    towerforge::ui::SaveLoadMenu* save_load_menu_;
    towerforge::ui::ResearchTreeMenu* research_menu_;
    towerforge::rendering::Camera* camera_;
    towerforge::ui::PlacementSystem* placement_system_;

    // InGame state
    towerforge::ui::GameState game_state_;
    bool is_paused_;
    bool in_settings_from_pause_;
    bool in_audio_settings_from_pause_;
    towerforge::ui::GeneralSettingsMenu pause_general_settings_menu_;
    towerforge::ui::AudioSettingsMenu pause_audio_settings_menu_;

    // Timing
    float elapsed_time_;
    float sim_time_;
    const float time_step_;
    const float total_time_;

    // Grid rendering constants
    int grid_offset_x_;
    int grid_offset_y_;
    int cell_width_;
    int cell_height_;

    // Flags
    bool game_initialized_;
};

} // namespace core
} // namespace towerforge
