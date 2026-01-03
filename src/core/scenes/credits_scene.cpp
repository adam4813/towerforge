#include "core/scenes/credits_scene.hpp"
#include "core/game.h"
#include <raylib.h>

namespace towerforge::core {
    CreditsScene::CreditsScene(Game *game)
        : GameScene(game) {
    }

    void CreditsScene::Initialize() {
        // Nothing to initialize
    }

    void CreditsScene::Shutdown() {
        // Nothing to clean up
    }

    void CreditsScene::Update(const float delta_time) {
    }

    void CreditsScene::Render() {
        ClearBackground(Color{20, 20, 30, 255});

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

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
    }

    void CreditsScene::HandleMouseEvent(const engine::ui::MouseEvent &event) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            game_->SetGameState(GameState::TitleScreen);
        }
    }
}
