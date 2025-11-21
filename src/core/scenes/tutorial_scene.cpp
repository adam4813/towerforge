#include "core/scenes/tutorial_scene.hpp"
#include "core/scenes/ingame_scene.hpp"
#include "core/game.h"
#include <raylib.h>

namespace towerforge::core {

    TutorialScene::TutorialScene(Game* game)
        : GameScene(game)
        , tutorial_active_(false) {
    }

    TutorialScene::~TutorialScene() = default;

    void TutorialScene::Initialize() {
        // Delegate to Game class helper which handles tutorial setup
        game_->InitializeGameSystems();
        tutorial_active_ = true;
    }

    void TutorialScene::Shutdown() {
        // Delegate to Game class helper
        game_->CleanupGameSystems();
        tutorial_active_ = false;
    }

    void TutorialScene::Update(const float delta_time) {
        // Delegate to Game class helper
        game_->UpdateTutorial(delta_time);
        HandleInput();
    }

    void TutorialScene::Render() {
        // Delegate to Game class helper
        game_->RenderTutorial();
    }

    void TutorialScene::HandleInput() {
        // Delegate to Game class helper
        game_->HandleTutorialInput();
    }

}
