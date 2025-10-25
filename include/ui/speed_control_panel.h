#pragma once

#include "ui/ui_element.h"
#include <raylib.h>
#include <functional>
#include <algorithm>

namespace towerforge::ui {

    /**
     * @brief Speed control panel for game simulation speed
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Displays pause and speed multiplier buttons.
     */
    class SpeedControlPanel : public UIElement {
    public:
        using SpeedCallback = std::function<void(int speed, bool paused)>;

        SpeedControlPanel(float x, float y, float width, float height);
        ~SpeedControlPanel() override = default;

        void Render() const override;
        bool ProcessMouseEvent(const MouseEvent& event);

        /**
         * @brief Set current speed state (reactive update)
         * @param speed Speed multiplier (1, 2, 4)
         * @param paused Whether simulation is paused
         */
        void SetSpeedState(int speed, bool paused);

        /**
         * @brief Set callback for speed changes
         */
        void SetSpeedCallback(SpeedCallback callback) { speed_callback_ = callback; }

    private:
        struct ButtonBounds {
            Rectangle rect;
            int speed;  // 0 = pause, 1/2/4 = speed multiplier
            bool is_pause;
        };

        std::vector<ButtonBounds> buttons_;
        int current_speed_;
        bool is_paused_;
        SpeedCallback speed_callback_;

        // Responsive sizing constants
        static constexpr int BASE_WIDTH = 150;  // 75% of original 200
        static constexpr int BASE_HEIGHT = 38;  // 75% of original 50 (rounded)
        static constexpr float MAX_WIDTH_PERCENT = 0.20f;  // Max 20% of screen width
        static constexpr int MIN_WIDTH = 100;  // Minimum usable width
        
        static constexpr int BUTTON_SPACING = 5;
        static constexpr int PADDING = 5;
        
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
