#include "core/scenes/achievements_scene.hpp"
#include "core/game.h"
#include "ui/achievements_menu.h"
#include "ui/mouse_interface.h"
#include "audio/audio_manager.h"
#include <raylib.h>

using namespace towerforge::ui;
using namespace towerforge::audio;

namespace towerforge::core {

    AchievementsScene::AchievementsScene(Game* game, ui::AchievementsMenu& achievements_menu)
        : GameScene(game)
        , achievements_menu_(achievements_menu) {
    }

    void AchievementsScene::Initialize() {
        // Achievement manager will be set by Game class
    }

    void AchievementsScene::Shutdown() {
        // Nothing to clean up
    }

    void AchievementsScene::Update(const float delta_time) {
        achievements_menu_.Update(delta_time);
        HandleInput();
    }

    void AchievementsScene::Render() {
        ClearBackground(Color{20, 20, 30, 255});
        achievements_menu_.Render();
    }

    void AchievementsScene::HandleInput() {
        if (achievements_menu_.HandleKeyboard()) {
            AudioManager::GetInstance().PlaySFX(AudioCue::MenuClose);
            game_->SetGameState(GameState::TitleScreen);
        }

        const MouseEvent mouse_event(
            static_cast<float>(GetMouseX()),
            static_cast<float>(GetMouseY()),
            IsMouseButtonDown(MOUSE_LEFT_BUTTON),
            IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON),
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
        );
        achievements_menu_.ProcessMouseEvent(mouse_event);
    }

}
