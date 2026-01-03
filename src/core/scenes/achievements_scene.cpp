#include "core/scenes/achievements_scene.hpp"
#include "core/game.h"
#include "ui/achievements_menu.h"
#include "audio/audio_manager.h"

using namespace towerforge::ui;
using namespace towerforge::audio;

namespace towerforge::core {
    AchievementsScene::AchievementsScene(Game *game, ui::AchievementsMenu &achievements_menu)
        : GameScene(game), achievements_menu_(achievements_menu) {
    }

    void AchievementsScene::Initialize() {
        engine::ui::text_renderer::FontManager::Initialize("fonts/Kenney Future.ttf", 16);
        engine::ui::BatchRenderer::Initialize();

        achievements_menu_.Initialize();
        achievements_menu_.SetCloseCallback([this]() {
            AudioManager::GetInstance().PlaySFX(AudioCue::MenuClose);
            game_->SetGameState(GameState::TitleScreen);
        });
    }

    void AchievementsScene::Shutdown() {
        achievements_menu_.Shutdown();
        engine::ui::BatchRenderer::Shutdown();
        engine::ui::text_renderer::FontManager::Shutdown();
    }

    void AchievementsScene::Update(const float delta_time) {
        achievements_menu_.Update(delta_time);
    }

    void AchievementsScene::Render() {
        ClearBackground(Color{20, 20, 30, 255});

        engine::ui::BatchRenderer::BeginFrame();
        achievements_menu_.Render();
        engine::ui::BatchRenderer::EndFrame();
    }

    void AchievementsScene::HandleMouseEvent(const engine::ui::MouseEvent &event) {
        achievements_menu_.HandleKeyboard();
        achievements_menu_.ProcessMouseEvent(event);
    }
}
