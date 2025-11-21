#include "core/scenes/title_scene.hpp"
#include "core/game.h"
#include "ui/main_menu.h"
#include "ui/mouse_interface.h"
#include <raylib.h>

using namespace towerforge::ui;

namespace towerforge::core {

    TitleScene::TitleScene(Game* game, ui::MainMenu& main_menu)
        : GameScene(game)
        , main_menu_(main_menu) {
    }

    void TitleScene::Initialize() {
        main_menu_.SetStateChangeCallback([this](const GameState new_state) {
            game_->SetGameState(new_state);
        });
    }

    void TitleScene::Shutdown() {
        // Nothing to clean up
    }

    void TitleScene::Update(const float delta_time) {
        main_menu_.Update(delta_time);
        HandleInput();
    }

    void TitleScene::Render() {
        main_menu_.Render();
    }

    void TitleScene::HandleInput() {
        main_menu_.HandleKeyboard();
        
        const MouseEvent mouse_event(
            static_cast<float>(GetMouseX()),
            static_cast<float>(GetMouseY()),
            IsMouseButtonDown(MOUSE_LEFT_BUTTON),
            IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON),
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)
        );
        main_menu_.ProcessMouseEvent(mouse_event);
    }

}
