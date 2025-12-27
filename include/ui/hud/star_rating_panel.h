#pragma once

#include <memory>

import engine;

namespace towerforge::ui {
    struct GameState;

    /**
     * @brief Star rating panel showing tower progression
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Displays current star rating and progress toward next milestone.
     * Uses citrus engine Panel for consistent styling.
     */
    class StarRatingPanel {
    public:
        StarRatingPanel();

        ~StarRatingPanel() = default;

        /**
         * @brief Initialize the panel UI elements
         */
        void Initialize();

        /**
         * @brief Update panel position on resize
         */
        void Update(float delta_time);

        /**
         * @brief Render the star rating panel
         */
        void Render() const;

        /**
         * @brief Set the game state reference for display
         */
        void SetGameState(const GameState *state) { game_state_ = state; }

        /**
         * @brief Check if max rating achieved
         */
        bool IsMaxRating() const;

        static constexpr int WIDTH = 230;
        static constexpr int HEIGHT = 180;
        static constexpr int PADDING = 10;

    private:
        void UpdateLayout();

        void RenderContent() const;

        const GameState *game_state_;
        std::unique_ptr<engine::ui::elements::Panel> panel_;
        int last_screen_width_;
        int last_screen_height_;
    };
} // namespace towerforge::ui
