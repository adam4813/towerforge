#pragma once

#include "core/scenes/game_scene.hpp"

namespace towerforge::ui {
    class MainMenu;
}

namespace towerforge::core {
    class TitleScene : public GameScene {
    public:
        TitleScene(Game *game, ui::MainMenu &main_menu);

        ~TitleScene() override = default;

        void Initialize() override;

        void Shutdown() override;

        void Update(float delta_time) override;

        void Render() override;

        void HandleMouseEvent(const engine::ui::MouseEvent &event) override;

    private:
        ui::MainMenu &main_menu_;
    };
}
