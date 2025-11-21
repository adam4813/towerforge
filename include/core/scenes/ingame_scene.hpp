#pragma once

#include "core/scenes/game_scene.hpp"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"
#include "ui/accessibility_settings_menu.h"
#include <memory>

namespace towerforge::ui {
    class HUD;
    class BuildMenu;
    class PauseMenu;
    class SaveLoadMenu;
    class ResearchTreeMenu;
    class ModsMenu;
    class PlacementSystem;
    class HistoryPanel;
    class TutorialManager;
    class HelpSystem;
}

namespace towerforge::rendering {
    class Camera;
}

namespace towerforge::core {

    // Forward declarations
    class ECSWorld;
    class SaveLoadManager;
    class AchievementManager;

    struct UIGameState;

    class InGameScene : public GameScene {
    public:
        explicit InGameScene(Game* game);
        ~InGameScene() override;

        void Initialize() override;
        void Shutdown() override;
        void Update(float delta_time) override;
        void Render() override;

    private:
        void HandleInput();
        void InitializeGameSystems();
        void CleanupGameSystems();
        void CreateStarterTower() const;
        void CalculateTowerRating();
        void UpdateCameraBounds();

        // InGame systems
        std::unique_ptr<ECSWorld> ecs_world_;
        std::unique_ptr<SaveLoadManager> save_load_manager_;
        std::unique_ptr<AchievementManager> achievement_manager_;
        std::unique_ptr<ui::HUD> hud_;
        std::unique_ptr<ui::BuildMenu> build_menu_;
        std::unique_ptr<ui::PauseMenu> pause_menu_;
        std::unique_ptr<ui::SaveLoadMenu> save_load_menu_;
        std::unique_ptr<ui::ResearchTreeMenu> research_menu_;
        std::unique_ptr<ui::ModsMenu> mods_menu_;
        std::unique_ptr<rendering::Camera> camera_;
        std::unique_ptr<ui::PlacementSystem> placement_system_;
        std::unique_ptr<ui::HistoryPanel> history_panel_;
        std::unique_ptr<ui::HelpSystem> help_system_;

        // InGame state
        UIGameState* game_state_;
        bool is_paused_;
        bool in_settings_from_pause_;
        bool in_audio_settings_from_pause_;
        bool in_accessibility_settings_from_pause_;
        ui::GeneralSettingsMenu pause_general_settings_menu_;
        ui::AudioSettingsMenu pause_audio_settings_menu_;
        ui::AccessibilitySettingsMenu pause_accessibility_settings_menu_;

        // Grid rendering constants
        int grid_offset_x_;
        int grid_offset_y_;
        int cell_width_;
        int cell_height_;

        bool game_initialized_;
    };

}
