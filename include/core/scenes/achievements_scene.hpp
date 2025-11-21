#pragma once

#include "core/scenes/game_scene.hpp"

// Forward declaration
namespace towerforge::ui {
    class AchievementsMenu;
}

namespace towerforge::core {

    class AchievementsScene : public GameScene {
    public:
        AchievementsScene(Game* game, ui::AchievementsMenu& achievements_menu);
        ~AchievementsScene() override = default;

        void Initialize() override;
        void Shutdown() override;
        void Update(float delta_time) override;
        void Render() override;

    private:
        void HandleInput();

        ui::AchievementsMenu& achievements_menu_;
    };

}
