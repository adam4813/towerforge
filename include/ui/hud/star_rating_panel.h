#pragma once

#include "ui/ui_element.h"
#include <raylib.h>
#include <string>

namespace towerforge::ui {

    struct GameState;

    /**
     * @brief Star rating panel showing tower progression
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Displays current star rating and progress toward next milestone.
     */
    class StarRatingPanel : public Panel {
    public:
        StarRatingPanel();
        ~StarRatingPanel() override = default;

        /**
         * @brief Update panel position on resize
         */
        void Update(float delta_time) override;

        /**
         * @brief Render the star rating panel
         */
        void Render() const override;

        /**
         * @brief Set the game state reference for display
         */
        void SetGameState(const GameState* state) { game_state_ = state; }

        /**
         * @brief Check if max rating achieved
         */
        bool IsMaxRating() const;

        static constexpr int WIDTH = 230;
        static constexpr int HEIGHT = 180;
        static constexpr int PADDING = 10;

    private:
        void RenderContent() const;

        const GameState* game_state_;
    };

} // namespace towerforge::ui
