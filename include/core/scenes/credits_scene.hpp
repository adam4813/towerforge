#pragma once

#include "core/scenes/game_scene.hpp"

namespace towerforge::core {

    class CreditsScene : public GameScene {
    public:
        explicit CreditsScene(Game* game);
        ~CreditsScene() override = default;

        void Initialize() override;
        void Shutdown() override;
        void Update(float delta_time) override;
        void Render() override;

    private:
        void HandleInput();
    };

}
