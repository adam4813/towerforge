#pragma once

#include "ui/ui_element.h"
#include <raylib.h>
#include <memory>
#include <functional>
#include <string>

namespace towerforge::ui {

    class Button;
    class NotificationCenter;
    struct GameState;

    /**
     * @brief Top bar panel displaying funds, population, time, and speed
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Handles interactive buttons for income/population analytics.
     */
    class TopBar : public Panel {
    public:
        using IncomeClickCallback = std::function<void()>;
        using PopulationClickCallback = std::function<void()>;
        using NotificationClickCallback = std::function<void()>;

        TopBar();
        ~TopBar() override = default;

        /**
         * @brief Update top bar state
         */
        void Update(float delta_time) override;

        /**
         * @brief Render the top bar
         */
        void Render() const override;

        /**
         * @brief Set the game state reference for display
         */
        void SetGameState(const GameState* state) { game_state_ = state; }

        /**
         * @brief Set notification center for unread count display
         */
        void SetNotificationCenter(NotificationCenter* center) { notification_center_ = center; }

        /**
         * @brief Set income button click callback
         */
        void SetIncomeClickCallback(IncomeClickCallback callback) { income_click_callback_ = std::move(callback); }

        /**
         * @brief Set population button click callback
         */
        void SetPopulationClickCallback(PopulationClickCallback callback) { population_click_callback_ = std::move(callback); }

        /**
         * @brief Set notification button click callback
         */
        void SetNotificationClickCallback(NotificationClickCallback callback) { notification_click_callback_ = std::move(callback); }

        static constexpr int HEIGHT = 40;

    private:
        void BuildButtons();
        void RenderContent() const;
        static std::string FormatTime(float time);

        const GameState* game_state_;
        NotificationCenter* notification_center_;

        // Interactive buttons (ownership via Panel::children_)
        Button* income_button_;
        Button* population_button_;
        Button* notification_button_;

        // Callbacks
        IncomeClickCallback income_click_callback_;
        PopulationClickCallback population_click_callback_;
        NotificationClickCallback notification_click_callback_;
    };

} // namespace towerforge::ui
