#pragma once

#include <memory>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/save_load_manager.hpp"
#include "core/achievement_manager.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"
#include "ui/history_panel.h"
#include "ui/main_menu.h"
#include "ui/pause_menu.h"
#include "ui/save_load_menu.h"
#include "ui/research_tree_menu.h"
#include "ui/achievements_menu.h"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"
#include "ui/accessibility_settings_menu.h"
#include "ui/tutorial_manager.h"
#include "ui/mods_menu.h"
#include "ui/help_system.h"
#include "ui/analytics_overlay.h"
#include "audio/audio_manager.h"

// Forward declare scene classes
namespace towerforge::core {
    class GameScene;
    class TitleScene;
    class AchievementsScene;
    class SettingsScene;
    class CreditsScene;
	class InGameScene;
	class TutorialScene;
}

namespace towerforge::core {

    /**
 * @brief Game state enumeration
 */
    enum class GameState {
        TitleScreen,
        Tutorial,
        InGame,
        Settings,
        LoadGame,
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

        // Public accessors for scenes
        rendering::Renderer& GetRenderer() { return renderer_; }
        audio::AudioManager* GetAudioManager() { return audio_manager_; }
        ui::AchievementsMenu& GetAchievementsMenu() { return achievements_menu_; }
        ui::MainMenu& GetMainMenu() { return main_menu_; }
        void SetGameState(GameState state);
        GameState GetGameState() const { return current_state_; }
        towerforge::core::AchievementManager* GetAchievementManager() { return achievement_manager_.get(); }

        // Temporary: InGame scene helpers (will be moved into scene)
        void InitializeGameSystems();
        void CleanupGameSystems();
        void UpdateInGame(float delta_time);
        void RenderInGame();
        void HandleInGameInput();
        void UpdateTutorial(float delta_time);
        void RenderTutorial();
        void HandleTutorialInput();

    private:
        void TransitionToState(GameState new_state);
        void CreateStarterTower() const;
        void CalculateTowerRating();
        void UpdateCameraBounds();
        ui::IncomeBreakdown CollectIncomeAnalytics() const;
        ui::PopulationBreakdown CollectPopulationAnalytics() const;

        // Game state
        GameState current_state_;
        GameState previous_state_;

        // Core systems (always active)
        rendering::Renderer renderer_;
        audio::AudioManager* audio_manager_;

        // Scene management
        GameScene* active_scene_;
        std::unique_ptr<TitleScene> title_scene_;
        std::unique_ptr<AchievementsScene> achievements_scene_;
        std::unique_ptr<SettingsScene> settings_scene_;
        std::unique_ptr<CreditsScene> credits_scene_;
        std::unique_ptr<InGameScene> ingame_scene_;
        std::unique_ptr<TutorialScene> tutorial_scene_;

        // UI for title screen (kept for backward compatibility during transition)
        ui::MainMenu main_menu_;
        ui::AchievementsMenu achievements_menu_;
        ui::GeneralSettingsMenu general_settings_menu_;
        ui::AudioSettingsMenu audio_settings_menu_;
        ui::AccessibilitySettingsMenu accessibility_settings_menu_;

        // Achievement manager (shared across scenes)
        std::unique_ptr<towerforge::core::AchievementManager> achievement_manager_;

        // Temporary: InGame state (will be moved into InGameScene)
        std::unique_ptr<towerforge::core::ECSWorld> ecs_world_;
        std::unique_ptr<towerforge::core::SaveLoadManager> save_load_manager_;
        std::unique_ptr<ui::HUD> hud_;
        std::unique_ptr<ui::BuildMenu> build_menu_;
        std::unique_ptr<ui::PauseMenu> pause_menu_;
        std::unique_ptr<ui::SaveLoadMenu> save_load_menu_;
        std::unique_ptr<ui::ResearchTreeMenu> research_menu_;
        std::unique_ptr<ui::ModsMenu> mods_menu_;
        std::unique_ptr<rendering::Camera> camera_;
        std::unique_ptr<ui::PlacementSystem> placement_system_;
        std::unique_ptr<ui::HistoryPanel> history_panel_;
        std::unique_ptr<ui::TutorialManager> tutorial_manager_;
        std::unique_ptr<ui::HelpSystem> help_system_;
        
        ui::GameState game_state_;
        bool is_paused_;
        bool in_settings_from_pause_;
        bool in_audio_settings_from_pause_;
        bool in_accessibility_settings_from_pause_;
        ui::GeneralSettingsMenu pause_general_settings_menu_;
        ui::AudioSettingsMenu pause_audio_settings_menu_;
        ui::AccessibilitySettingsMenu pause_accessibility_settings_menu_;
        bool tutorial_active_;
        bool game_initialized_;
        int grid_offset_x_;
        int grid_offset_y_;
        int cell_width_;
        int cell_height_;

        // Timing
        float elapsed_time_;
        float sim_time_;
        const float time_step_;
        const float total_time_;
    };

}
