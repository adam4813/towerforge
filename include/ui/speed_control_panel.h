#pragma once

#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include <functional>
#include <algorithm>
#include <memory>

import engine;

namespace towerforge::ui {
    struct GameState;

    /**
     * @brief Speed control panel for game simulation speed
     * 
     * Declarative UI component using citrus engine Panel and Button elements.
     * Provides pause/play and speed multiplier controls.
     */
    class SpeedControlPanel {
    public:
        using SpeedCallback = std::function<void(int speed, bool paused)>;

        SpeedControlPanel(float x, float y, float width, float height);

        ~SpeedControlPanel() = default;

        /**
         * @brief Update panel state and position
         */
        void Update(float delta_time);

        /**
         * @brief Render the panel
         */
        void Render() const;

        /**
         * @brief Process mouse events (towerforge event type)
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Set the game state reference for reading speed/pause state
         */
        void SetGameState(const GameState *state) { game_state_ = state; }

        /**
         * @brief Set callback for speed changes
         */
        void SetSpeedCallback(SpeedCallback callback) { speed_callback_ = callback; }

    private:
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        const GameState *game_state_;

        // Button references (ownership in main_panel_)
        engine::ui::elements::Button *pause_button_;
        engine::ui::elements::Button *speed_1x_button_;
        engine::ui::elements::Button *speed_2x_button_;
        engine::ui::elements::Button *speed_4x_button_;

        int current_speed_;
        bool is_paused_;
        SpeedCallback speed_callback_;

        // Responsive sizing constants
        static constexpr int BASE_WIDTH = 150;
        static constexpr int BASE_HEIGHT = 38;
        static constexpr float MAX_WIDTH_PERCENT = 0.20f;
        static constexpr int MIN_WIDTH = 100;
        static constexpr int BUTTON_SPACING = 5;
        static constexpr int PADDING = 5;

        void BuildComponents();

        void UpdateButtonStates() const;

        void OnPauseClick();

        void OnSpeedClick(int speed);

    public:
        // Calculate responsive width
        static int CalculateWidth() {
            std::uint32_t screen_width;
            std::uint32_t screen_height;
            engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);
            const int max_width = static_cast<int>(screen_width * MAX_WIDTH_PERCENT);
            return std::clamp(std::min(BASE_WIDTH, max_width), MIN_WIDTH, BASE_WIDTH);
        }

        // Calculate responsive height based on width
        static int CalculateHeight() {
            const int width = CalculateWidth();
            return static_cast<int>(BASE_HEIGHT * (static_cast<float>(width) / BASE_WIDTH));
        }

        // Calculate button width based on panel width
        static int CalculateButtonWidth() {
            const int panel_width = CalculateWidth();
            constexpr int num_buttons = 4;
            const int available_width = panel_width - PADDING * 2 - BUTTON_SPACING * (num_buttons - 1);
            return std::max(20, available_width / num_buttons);
        }
    };
} // namespace towerforge::ui
