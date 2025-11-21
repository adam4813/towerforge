#include "core/scenes/ingame_scene.hpp"
#include "core/game.h"
#include <raylib.h>

namespace towerforge::core {

    InGameScene::InGameScene(Game* game)
        : GameScene(game)
        , game_state_(nullptr)
        , is_paused_(false)
        , in_settings_from_pause_(false)
        , in_audio_settings_from_pause_(false)
        , in_accessibility_settings_from_pause_(false)
        , grid_offset_x_(300)
        , grid_offset_y_(100)
        , cell_width_(40)
        , cell_height_(50)
        , game_initialized_(false) {
    }

    InGameScene::~InGameScene() {
        // Cleanup handled by Game class for now
    }

    void InGameScene::Initialize() {
        // Delegate to Game class helper
        game_->InitializeGameSystems();
    }

    void InGameScene::Shutdown() {
        // Delegate to Game class helper
        game_->CleanupGameSystems();
    }

    void InGameScene::Update(const float delta_time) {
        // Delegate to Game class helper
        game_->UpdateInGame(delta_time);
    }

    void InGameScene::Render() {
        // Delegate to Game class helper
        game_->RenderInGame();
    }

    void InGameScene::HandleInput() {
        // Delegate to Game class helper
        game_->HandleInGameInput();
    }

    void InGameScene::InitializeGameSystems() {
        // Implemented in Game class for now
    }

    void InGameScene::CleanupGameSystems() {
        // Implemented in Game class for now
    }

    void InGameScene::CreateStarterTower() const {
        // Implemented in Game class for now
    }

    void InGameScene::CalculateTowerRating() {
        // Implemented in Game class for now
    }

    void InGameScene::UpdateCameraBounds() {
        // Implemented in Game class for now
    }

}
