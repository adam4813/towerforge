#include "ui/speed_control_panel.h"
#include "ui/hud/hud.h"
#include "ui/mouse_interface.h"
#include <raylib.h>

namespace towerforge::ui {

    SpeedControlPanel::SpeedControlPanel(float x, float y, float width, float height)
        : Panel(x, y, width, height, ColorAlpha(BLACK, 0.7f), LIGHTGRAY)
        , game_state_(nullptr)
        , pause_button_(nullptr)
        , speed_1x_button_(nullptr)
        , speed_2x_button_(nullptr)
        , speed_4x_button_(nullptr)
        , current_speed_(1)
        , is_paused_(false)
        , speed_callback_(nullptr) {
        
        SetPadding(PADDING);
        BuildButtons();
        UpdateButtonStates();
    }

    void SpeedControlPanel::Update(float delta_time) {
        Panel::Update(delta_time);

        // Update position and size on resize
        const int screen_height = GetScreenHeight();
        const int speed_width = CalculateWidth();
        const int speed_height = CalculateHeight();
        SetRelativePosition(10, screen_height - speed_height - 10);
        SetSize(speed_width, speed_height);

        // Update speed state from game state
        if (game_state_) {
            if (current_speed_ != game_state_->speed_multiplier || is_paused_ != game_state_->paused) {
                current_speed_ = game_state_->speed_multiplier;
                is_paused_ = game_state_->paused;
                UpdateButtonStates();
            }
        }
    }

    void SpeedControlPanel::BuildButtons() {
        const int button_width = CalculateButtonWidth();
        const int button_height = static_cast<int>(GetRelativeBounds().height) - PADDING * 2;
        float button_x = static_cast<float>(PADDING);
        const float button_y = static_cast<float>(PADDING);

        // Pause button
        auto pause = std::make_unique<IconButton>(
            button_x, button_y, button_width, button_height, "||", DARKGRAY, WHITE
        );
        pause->SetClickCallback([this]() { OnPauseClick(); });
        pause_button_ = pause.get();
        AddChild(std::move(pause));
        button_x += button_width + BUTTON_SPACING;

        // 1x speed button
        auto speed_1x = std::make_unique<IconButton>(
            button_x, button_y, button_width, button_height, "1x", DARKGRAY, WHITE
        );
        speed_1x->SetClickCallback([this]() { OnSpeedClick(1); });
        speed_1x_button_ = speed_1x.get();
        AddChild(std::move(speed_1x));
        button_x += button_width + BUTTON_SPACING;

        // 2x speed button
        auto speed_2x = std::make_unique<IconButton>(
            button_x, button_y, button_width, button_height, "2x", DARKGRAY, WHITE
        );
        speed_2x->SetClickCallback([this]() { OnSpeedClick(2); });
        speed_2x_button_ = speed_2x.get();
        AddChild(std::move(speed_2x));
        button_x += button_width + BUTTON_SPACING;

        // 4x speed button
        auto speed_4x = std::make_unique<IconButton>(
            button_x, button_y, button_width, button_height, "4x", DARKGRAY, WHITE
        );
        speed_4x->SetClickCallback([this]() { OnSpeedClick(4); });
        speed_4x_button_ = speed_4x.get();
        AddChild(std::move(speed_4x));
    }

    void SpeedControlPanel::UpdateButtonStates() {
        if (!pause_button_) return;
        
        // Update pause button color
        pause_button_->SetBackgroundColor(is_paused_ ? RED : DARKGRAY);
        
        // Update speed button colors
        speed_1x_button_->SetBackgroundColor(
            (!is_paused_ && current_speed_ == 1) ? GREEN : DARKGRAY
        );
        speed_2x_button_->SetBackgroundColor(
            (!is_paused_ && current_speed_ == 2) ? GREEN : DARKGRAY
        );
        speed_4x_button_->SetBackgroundColor(
            (!is_paused_ && current_speed_ == 4) ? GREEN : DARKGRAY
        );
    }

    void SpeedControlPanel::OnPauseClick() {
        if (speed_callback_) {
            speed_callback_(current_speed_, !is_paused_);
        }
    }

    void SpeedControlPanel::OnSpeedClick(int speed) {
        if (speed_callback_) {
            speed_callback_(speed, false);
        }
    }

} // namespace towerforge::ui
