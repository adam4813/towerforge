#include "core/scenes/title_scene.hpp"
#include "core/game.h"
#include "ui/main_menu.h"
#include "ui/mouse_interface.h"

import engine;

using namespace towerforge::ui;

namespace towerforge::core {
    TitleScene::TitleScene(Game *game, ui::MainMenu &main_menu)
        : GameScene(game)
          , main_menu_(main_menu) {
    }

    void TitleScene::Initialize() {
        engine::ui::text_renderer::FontManager::Initialize("fonts/Kenney Future.ttf", 16);
        engine::ui::BatchRenderer::Initialize();

        main_menu_.SetStateChangeCallback([this](const GameState new_state) {
            game_->SetGameState(new_state);
        });

        main_menu_.Initialize();
    }

    void TitleScene::Shutdown() {
        main_menu_.Shutdown();
        engine::ui::BatchRenderer::Shutdown();
        engine::ui::text_renderer::FontManager::Shutdown();
    }

    void TitleScene::Update(const float delta_time) {
        main_menu_.Update(delta_time);
    }

    void TitleScene::Render() {
        engine::ui::BatchRenderer::BeginFrame();
        main_menu_.Render();
        engine::ui::BatchRenderer::EndFrame();
    }

    void TitleScene::HandleMouseEvent(const engine::ui::MouseEvent &event) {
        main_menu_.HandleKeyboard();
        main_menu_.ProcessMouseEvent(event);
    }
}
