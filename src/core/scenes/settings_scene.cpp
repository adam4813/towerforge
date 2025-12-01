#include "core/scenes/settings_scene.hpp"
#include "core/game.h"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"
#include "ui/accessibility_settings_menu.h"
#include "ui/mouse_interface.h"
#include <raylib.h>

using namespace towerforge::ui;

namespace towerforge::core {
    SettingsScene::SettingsScene(Game *game, ui::GeneralSettingsMenu &general_settings_menu,
                                 ui::AudioSettingsMenu &audio_settings_menu,
                                 ui::AccessibilitySettingsMenu &accessibility_settings_menu)
        : GameScene(game)
          , general_settings_menu_(general_settings_menu)
          , audio_settings_menu_(audio_settings_menu)
          , accessibility_settings_menu_(accessibility_settings_menu) {
    }

    void SettingsScene::Initialize() {
        general_settings_menu_.Initialize();

        general_settings_menu_.SetOptionCallback([this](const SettingsOption option) {
            switch (option) {
                case SettingsOption::Audio:
                    in_audio_settings_ = true;
                    break;
                case SettingsOption::Accessibility:
                    in_accessibility_settings_ = true;
                    break;
                case SettingsOption::Back:
                    game_->SetGameState(GameState::TitleScreen);
                    break;
                default:
                    break;
            }
        });

        audio_settings_menu_.Initialize();
        audio_settings_menu_.SetBackCallback([this]() {
            in_audio_settings_ = false;
        });

        accessibility_settings_menu_.Initialize();
        accessibility_settings_menu_.SetBackCallback([this]() {
            in_accessibility_settings_ = false;
        });
    }

    void SettingsScene::Shutdown() {
        general_settings_menu_.Shutdown();
        accessibility_settings_menu_.Shutdown();
        audio_settings_menu_.Shutdown();
        in_audio_settings_ = false;
        in_accessibility_settings_ = false;
    }

    void SettingsScene::Update(const float delta_time) {
        if (in_accessibility_settings_) {
            accessibility_settings_menu_.Update(delta_time);
        } else if (in_audio_settings_) {
            audio_settings_menu_.Update(delta_time);
        } else {
            general_settings_menu_.Update(delta_time);
        }
        HandleInput();
    }

    void SettingsScene::Render() {
        ClearBackground(Color{20, 20, 30, 255});

        engine::ui::BatchRenderer::BeginFrame();
        if (in_accessibility_settings_) {
            accessibility_settings_menu_.Render();
        } else if (in_audio_settings_) {
            audio_settings_menu_.Render();
        } else {
            general_settings_menu_.Render();
        }
        engine::ui::BatchRenderer::EndFrame();
    }

    void SettingsScene::HandleInput() {
        const MouseEvent mouse_event(
            static_cast<float>(GetMouseX()),
            static_cast<float>(GetMouseY()),
            IsMouseButtonDown(MOUSE_LEFT_BUTTON),
            IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON),
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
        );

        if (in_accessibility_settings_) {
            accessibility_settings_menu_.HandleKeyboard();
            accessibility_settings_menu_.ProcessMouseEvent(mouse_event);
        } else if (in_audio_settings_) {
            audio_settings_menu_.HandleKeyboard();
            audio_settings_menu_.ProcessMouseEvent(mouse_event);
        } else {
            general_settings_menu_.HandleKeyboard();
            general_settings_menu_.ProcessMouseEvent(mouse_event);
        }
    }
}
