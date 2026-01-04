#pragma once

#include "core/scenes/game_scene.hpp"
#include <memory>

import engine;

namespace towerforge::core {
    class CreditsScene : public GameScene {
    public:
        explicit CreditsScene(Game *game);

        ~CreditsScene() override = default;

        void Initialize() override;

        void Shutdown() override;

        void Update(float delta_time) override;

        void Render() override;

        void HandleMouseEvent(const engine::ui::MouseEvent &event) override;

    private:
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        std::uint32_t last_screen_width_ = 0;
        std::uint32_t last_screen_height_ = 0;

        void BuildUI();
        void UpdateLayout();
    };
}
