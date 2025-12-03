#pragma once

#include "ui/ui_element.h"
#include "ui/icon_button.h"
#include <raylib.h>
#include <functional>
#include <algorithm>

namespace towerforge::ui {

    struct GameState;

    /**
     * @brief Speed control panel for game simulation speed
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Uses proper Composite pattern with IconButton children.
     */
    class SpeedControlPanel : public Panel {
    public:
        using SpeedCallback = std::function<void(int speed, bool paused)>;

        SpeedControlPanel(float x, float y, float width, float height);
        ~SpeedControlPanel() override = default;

        /**
         * @brief Update panel state and position
         */
        void Update(float delta_time);

        /**
         * @brief Set the game state reference for reading speed/pause state
         */
        void SetGameState(const GameState* state) { game_state_ = state; }

        /**
         * @brief Set callback for speed changes
         */
        void SetSpeedCallback(SpeedCallback callback) { speed_callback_ = callback; }

    private:
        const GameState* game_state_;

        // Button children (ownership transferred to Panel via AddChild)
        IconButton* pause_button_;
        IconButton* speed_1x_button_;
        IconButton* speed_2x_button_;
        IconButton* speed_4x_button_;
        
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
        
        void BuildButtons();
        void UpdateButtonStates();
        void OnPauseClick();
        void OnSpeedClick(int speed);
        
    public:
        // Calculate responsive width
        static int CalculateWidth() {
            const int screen_width = GetScreenWidth();
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
