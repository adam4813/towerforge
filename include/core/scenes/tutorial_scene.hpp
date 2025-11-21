#pragma once

#include "core/scenes/game_scene.hpp"
#include <memory>


namespace towerforge::ui {
    class TutorialManager;
    class BuildMenu;
}

namespace towerforge::core {

    // Forward declarations
    class ECSWorld;

    class InGameScene;

    class TutorialScene : public GameScene {
    public:
        explicit TutorialScene(Game* game);
        ~TutorialScene() override;

        void Initialize() override;
        void Shutdown() override;
        void Update(float delta_time) override;
        void Render() override;

    private:
        void HandleInput();

        std::unique_ptr<ui::TutorialManager> tutorial_manager_;
        std::unique_ptr<InGameScene> ingame_scene_;
        bool tutorial_active_;
    };

}
