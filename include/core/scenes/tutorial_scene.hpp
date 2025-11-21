#pragma once

#include "core/scenes/ingame_scene.hpp"
#include <memory>


namespace towerforge::ui {
    class TutorialManager;
    class BuildMenu;
}

namespace towerforge::core {

    class TutorialScene : public InGameScene {
    public:
        explicit TutorialScene(Game* game);
        ~TutorialScene() override;

        void Initialize() override;
        void Shutdown() override;
        void Update(float delta_time) override;
        void Render() override;

    private:
        void HandleTutorialInput();
        void CreateStarterTower() const;

        std::unique_ptr<ui::TutorialManager> tutorial_manager_;
        bool tutorial_active_;
    };

}
