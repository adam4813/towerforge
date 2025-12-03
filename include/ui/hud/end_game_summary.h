#pragma once

#include "ui/ui_element.h"
#include <raylib.h>
#include <string>

namespace towerforge::ui {

    struct GameState;

    /**
     * @brief End game summary overlay shown when max stars achieved
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Modal overlay that displays final statistics.
     */
    class EndGameSummary : public Panel {
    public:
        EndGameSummary();
        ~EndGameSummary() override = default;

        /**
         * @brief Update overlay position on resize
         */
        void Update(float delta_time) override;

        /**
         * @brief Render the end game summary
         */
        void Render() const override;

        /**
         * @brief Set the game state reference for display
         */
        void SetGameState(const GameState* state) { game_state_ = state; }

        /**
         * @brief Check if should be visible (max stars achieved)
         */
        bool ShouldShow() const;

        static constexpr int BOX_WIDTH = 400;
        static constexpr int BOX_HEIGHT = 300;

    private:
        void RenderOverlay() const;
        void RenderContent() const;
        void UpdatePosition();

        const GameState* game_state_;
    };

} // namespace towerforge::ui
