#include "ui/speed_control_panel.h"
#include "ui/hud/hud.h"

namespace towerforge::ui {
    SpeedControlPanel::SpeedControlPanel(float /*x*/, float /*y*/, float /*width*/, float /*height*/)
        : game_state_(nullptr)
          , pause_button_(nullptr)
          , speed_1x_button_(nullptr)
          , speed_2x_button_(nullptr)
          , speed_4x_button_(nullptr)
          , current_speed_(1)
          , is_paused_(false)
          , speed_callback_(nullptr) {
        BuildComponents();
        UpdateButtonStates();
    }

    void SpeedControlPanel::BuildComponents() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int speed_width = CalculateWidth();
        const int speed_height = CalculateHeight();
        const int screen_height = GetScreenHeight();

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetSize(static_cast<float>(speed_width), static_cast<float>(speed_height));
        main_panel_->SetRelativePosition(10, static_cast<float>(screen_height - speed_height - 10));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->SetBorderWidth(1.0f);
        main_panel_->SetPadding(static_cast<float>(PADDING));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<HorizontalLayout>(static_cast<float>(BUTTON_SPACING), Alignment::Center)
        );

        const int button_width = CalculateButtonWidth();
        const int button_height = speed_height - PADDING * 2;

        // Pause button
        auto pause = std::make_unique<engine::ui::elements::Button>(
            button_width, button_height, "||", 12
        );
        pause->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        pause->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(DARKGRAY, 0.8f)));
        pause->SetTextColor(UITheme::ToEngineColor(WHITE));
        pause->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                OnPauseClick();
                return true;
            }
            return false;
        });
        pause_button_ = pause.get();
        main_panel_->AddChild(std::move(pause));

        // 1x speed button
        auto speed_1x = std::make_unique<engine::ui::elements::Button>(
            button_width, button_height, "1x", 12
        );
        speed_1x->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        speed_1x->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(DARKGRAY, 0.8f)));
        speed_1x->SetTextColor(UITheme::ToEngineColor(WHITE));
        speed_1x->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                OnSpeedClick(1);
                return true;
            }
            return false;
        });
        speed_1x_button_ = speed_1x.get();
        main_panel_->AddChild(std::move(speed_1x));

        // 2x speed button
        auto speed_2x = std::make_unique<engine::ui::elements::Button>(
            button_width, button_height, "2x", 12
        );
        speed_2x->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        speed_2x->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(DARKGRAY, 0.8f)));
        speed_2x->SetTextColor(UITheme::ToEngineColor(WHITE));
        speed_2x->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                OnSpeedClick(2);
                return true;
            }
            return false;
        });
        speed_2x_button_ = speed_2x.get();
        main_panel_->AddChild(std::move(speed_2x));

        // 4x speed button
        auto speed_4x = std::make_unique<engine::ui::elements::Button>(
            button_width, button_height, "4x", 12
        );
        speed_4x->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        speed_4x->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(DARKGRAY, 0.8f)));
        speed_4x->SetTextColor(UITheme::ToEngineColor(WHITE));
        speed_4x->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                OnSpeedClick(4);
                return true;
            }
            return false;
        });
        speed_4x_button_ = speed_4x.get();
        main_panel_->AddChild(std::move(speed_4x));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void SpeedControlPanel::Update(float /*delta_time*/) {
        if (!main_panel_) return;

        // Update position and size on resize
        const int screen_height = GetScreenHeight();
        const int speed_width = CalculateWidth();
        const int speed_height = CalculateHeight();
        main_panel_->SetRelativePosition(10, static_cast<float>(screen_height - speed_height - 10));
        main_panel_->SetSize(static_cast<float>(speed_width), static_cast<float>(speed_height));

        // Update speed state from game state
        if (game_state_) {
            if (current_speed_ != game_state_->speed_multiplier || is_paused_ != game_state_->paused) {
                current_speed_ = game_state_->speed_multiplier;
                is_paused_ = game_state_->paused;
                UpdateButtonStates();
            }
        }
    }

    void SpeedControlPanel::Render() const {
        if (!main_panel_) return;

        main_panel_->Render();
    }

    bool SpeedControlPanel::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        return main_panel_ ? main_panel_->ProcessMouseEvent(event) : false;
    }

    void SpeedControlPanel::UpdateButtonStates() const {
        if (!pause_button_) return;

        // Update pause button color
        pause_button_->SetNormalColor(UITheme::ToEngineColor(is_paused_ ? RED : DARKGRAY));

        // Update speed button colors
        speed_1x_button_->SetNormalColor(UITheme::ToEngineColor(
            (!is_paused_ && current_speed_ == 1) ? GREEN : DARKGRAY
        ));
        speed_2x_button_->SetNormalColor(UITheme::ToEngineColor(
            (!is_paused_ && current_speed_ == 2) ? GREEN : DARKGRAY
        ));
        speed_4x_button_->SetNormalColor(UITheme::ToEngineColor(
            (!is_paused_ && current_speed_ == 4) ? GREEN : DARKGRAY
        ));
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
