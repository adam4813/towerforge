#pragma once

#include <memory>
#include <functional>
#include <string>

import engine;

namespace towerforge::ui {
    class NotificationCenter;
    struct GameState;

    /**
     * @brief Top bar panel displaying funds, population, time, and speed
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Uses citrus engine Panel and Button for consistent styling.
     * Handles interactive buttons for income/population analytics.
     */
    class TopBar {
    public:
        using IncomeClickCallback = std::function<void()>;
        using PopulationClickCallback = std::function<void()>;
        using NotificationClickCallback = std::function<void()>;

        TopBar();

        ~TopBar() = default;

        /**
         * @brief Initialize UI elements
         */
        void Initialize();

        /**
         * @brief Update top bar state
         */
        void Update(float delta_time);

        /**
         * @brief Render the top bar
         */
        void Render() const;

        /**
         * @brief Process mouse events
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Set the game state reference for display
         */
        void SetGameState(const GameState *state) { game_state_ = state; }

        /**
         * @brief Set notification center for unread count display
         */
        void SetNotificationCenter(NotificationCenter *center) { notification_center_ = center; }

        /**
         * @brief Set income button click callback
         */
        void SetIncomeClickCallback(IncomeClickCallback callback) { income_click_callback_ = std::move(callback); }

        /**
         * @brief Set population button click callback
         */
        void SetPopulationClickCallback(PopulationClickCallback callback) {
            population_click_callback_ = std::move(callback);
        }

        /**
         * @brief Set notification button click callback
         */
        void SetNotificationClickCallback(NotificationClickCallback callback) {
            notification_click_callback_ = std::move(callback);
        }

        static constexpr int HEIGHT = 40;

    private:
        void BuildPanel();

        void UpdateLayout();

        void UpdateTextElements() const;

        static std::string FormatTime(float time);

        const GameState *game_state_;
        NotificationCenter *notification_center_;

        // Engine UI elements
        std::unique_ptr<engine::ui::elements::Panel> panel_;
        engine::ui::elements::Button *income_button_;
        engine::ui::elements::Button *population_button_;
        engine::ui::elements::Button *notification_button_;

        // Text elements for dynamic content
        engine::ui::elements::Text *funds_text_;
        engine::ui::elements::Text *population_text_;
        engine::ui::elements::Text *time_text_;
        engine::ui::elements::Text *speed_text_;
        engine::ui::elements::Text *badge_text_;

        int last_screen_width_;

        // Callbacks
        IncomeClickCallback income_click_callback_;
        PopulationClickCallback population_click_callback_;
        NotificationClickCallback notification_click_callback_;
    };
} // namespace towerforge::ui
