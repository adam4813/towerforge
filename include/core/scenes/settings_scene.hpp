#pragma once

#include "core/scenes/game_scene.hpp"

namespace towerforge::ui {
    class GeneralSettingsMenu;
    class AudioSettingsMenu;
    class AccessibilitySettingsMenu;
}

namespace towerforge::core {
    class SettingsScene : public GameScene {
    public:
        SettingsScene(Game *game, ui::GeneralSettingsMenu &general_settings_menu,
                      ui::AudioSettingsMenu &audio_settings_menu,
                      ui::AccessibilitySettingsMenu &accessibility_settings_menu);

        ~SettingsScene() override = default;

        void Initialize() override;

        void Shutdown() override;

        void Update(float delta_time) override;

        void Render() override;

        void HandleMouseEvent(const engine::ui::MouseEvent &event) override;

    private:
        ui::GeneralSettingsMenu &general_settings_menu_;
        ui::AudioSettingsMenu &audio_settings_menu_;
        ui::AccessibilitySettingsMenu &accessibility_settings_menu_;

        bool in_audio_settings_ = false;
        bool in_accessibility_settings_ = false;
    };
}
