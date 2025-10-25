#include "ui/speed_control_panel.h"
#include "ui/mouse_interface.h"
#include <raylib.h>

namespace towerforge::ui {

    SpeedControlPanel::SpeedControlPanel(float x, float y, float width, float height)
        : UIElement(x, y, width, height)
        , current_speed_(1)
        , is_paused_(false)
        , speed_callback_(nullptr) {

        // Build button structure declaratively with responsive sizing
        buttons_.clear();
        
        const int button_width = CalculateButtonWidth();
        const int button_height = height - PADDING * 2;
        
        float button_x = PADDING;
        const float button_y = PADDING;

        // Pause button
        buttons_.push_back({
            Rectangle{button_x, button_y, static_cast<float>(button_width), static_cast<float>(button_height)},
            0,
            true
        });
        button_x += button_width + BUTTON_SPACING;

        // 1x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, static_cast<float>(button_width), static_cast<float>(button_height)},
            1,
            false
        });
        button_x += button_width + BUTTON_SPACING;

        // 2x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, static_cast<float>(button_width), static_cast<float>(button_height)},
            2,
            false
        });
        button_x += button_width + BUTTON_SPACING;

        // 4x speed button
        buttons_.push_back({
            Rectangle{button_x, button_y, static_cast<float>(button_width), static_cast<float>(button_height)},
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

        // Render buttons with reactive state colors and responsive sizing
        const int button_width = CalculateButtonWidth();
        const int font_size = std::max(10, button_width / 4);
        
        for (size_t i = 0; i < buttons_.size(); ++i) {
            const auto& btn = buttons_[i];
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

            // Button labels with responsive font size
            const char* label;
            if (btn.is_pause) {
                label = "||";
            } else if (btn.speed == 1) {
                label = "1x";
            } else if (btn.speed == 2) {
                label = "2x";
            } else {
                label = "4x";
            }

            const int text_width = MeasureText(label, font_size);
            const int label_x = abs_x + (static_cast<int>(btn.rect.width) - text_width) / 2;
            const int label_y = abs_y + (static_cast<int>(btn.rect.height) - font_size) / 2;
            
            DrawText(
                label,
                label_x,
                label_y,
                font_size,
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
