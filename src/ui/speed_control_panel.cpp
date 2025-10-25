#include "ui/speed_control_panel.h"
#include "ui/mouse_interface.h"
#include <raylib.h>

namespace towerforge::ui {

    SpeedControlPanel::SpeedControlPanel(float x, float y, float width, float height)
        : UIElement(x, y, width, height)
        , current_speed_(1)
        , is_paused_(false)
        , speed_callback_(nullptr) {

        // Build button structure declaratively (once, in constructor)
        // Use relative positions within the panel
        buttons_.clear();
        
        float button_x = PADDING;
        const float button_y = PADDING;

        // Pause button
        buttons_.push_back({
            Rectangle{button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT},
            0,
            true
        });
        button_x += BUTTON_WIDTH + BUTTON_SPACING;

        // 1x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT},
            1,
            false
        });
        button_x += BUTTON_WIDTH + BUTTON_SPACING;

        // 2x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT},
            2,
            false
        });
        button_x += BUTTON_WIDTH + BUTTON_SPACING;

        // 4x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT},
            4,
            false
        });
    }

    void SpeedControlPanel::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();

        // Background panel
        DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            static_cast<int>(bounds.height),
            ColorAlpha(BLACK, 0.7f)
        );
        DrawRectangleLines(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            static_cast<int>(bounds.height),
            LIGHTGRAY
        );

        // Render buttons with reactive state colors
        // Buttons are stored with relative positions, need to offset by panel position
        for (const auto& btn : buttons_) {
            Color button_color;
            if (btn.is_pause) {
                button_color = is_paused_ ? RED : DARKGRAY;
            } else {
                button_color = (!is_paused_ && current_speed_ == btn.speed) ? GREEN : DARKGRAY;
            }

            // Offset button position by panel bounds
            const int abs_x = static_cast<int>(bounds.x + btn.rect.x);
            const int abs_y = static_cast<int>(bounds.y + btn.rect.y);

            DrawRectangle(
                abs_x,
                abs_y,
                static_cast<int>(btn.rect.width),
                static_cast<int>(btn.rect.height),
                button_color
            );
            DrawRectangleLines(
                abs_x,
                abs_y,
                static_cast<int>(btn.rect.width),
                static_cast<int>(btn.rect.height),
                LIGHTGRAY
            );

            // Button labels
            const char* label;
            int label_offset_x;
            if (btn.is_pause) {
                label = "||";
                label_offset_x = 15;
            } else if (btn.speed == 1) {
                label = "1x";
                label_offset_x = 12;
            } else if (btn.speed == 2) {
                label = "2x";
                label_offset_x = 12;
            } else {
                label = "4x";
                label_offset_x = 12;
            }

            DrawText(
                label,
                abs_x + label_offset_x,
                abs_y + 7,
                16,
                WHITE
            );
        }
    }

    bool SpeedControlPanel::ProcessMouseEvent(const MouseEvent& event) {
        if (event.left_pressed) {
            const Rectangle bounds = GetAbsoluteBounds();
            
            // Check which button was clicked
            // Buttons are stored with relative positions, need to offset by panel position
            for (const auto& btn : buttons_) {
                Rectangle abs_button_rect = {
                    bounds.x + btn.rect.x,
                    bounds.y + btn.rect.y,
                    btn.rect.width,
                    btn.rect.height
                };
                
                if (CheckCollisionPointRec(Vector2{event.x, event.y}, abs_button_rect)) {
                    // Notify observer of speed change
                    if (speed_callback_) {
                        if (btn.is_pause) {
                            speed_callback_(current_speed_, !is_paused_);
                        } else {
                            speed_callback_(btn.speed, false);
                        }
                    }
                    return true;
                }
            }
        }

        return false;
    }

    void SpeedControlPanel::SetSpeedState(const int speed, const bool paused) {
        // Reactive update - only change when state actually changes
        if (current_speed_ != speed || is_paused_ != paused) {
            current_speed_ = speed;
            is_paused_ = paused;
            // Render() will reflect new state on next frame
        }
    }

} // namespace towerforge::ui
