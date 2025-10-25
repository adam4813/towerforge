#pragma once

#include "ui/ui_element.h"
#include <raylib.h>
#include <functional>

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

        static constexpr int BUTTON_WIDTH = 45;
        static constexpr int BUTTON_HEIGHT = 30;
        static constexpr int BUTTON_SPACING = 5;
        static constexpr int PADDING = 5;
    };

} // namespace towerforge::ui
