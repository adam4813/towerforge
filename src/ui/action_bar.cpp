#include "ui/action_bar.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "audio/audio_manager.h"
#include <algorithm>

import engine;

namespace towerforge::ui {
    void ActionBar::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int bar_width = CalculateBarWidth();
        const int bar_x = (screen_width - bar_width) / 2;

        // Create main panel with horizontal layout
        main_panel_ = std::make_unique<engine::ui::elements::Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(bar_x), static_cast<float>(screen_height - HEIGHT));
        main_panel_->SetSize(static_cast<float>(bar_width), static_cast<float>(HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.9f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        main_panel_->SetPadding(5.0f);
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<HorizontalLayout>(BUTTON_SPACING, Alignment::Center)
        );

        // Create action buttons
        const std::vector<std::pair<Action, std::string> > actions = {
            {Action::Build, "Build"},
            {Action::FacilityInfo, "Facilities"},
            {Action::VisitorInfo, "Visitors"},
            {Action::StaffManagement, "Staff"},
            {Action::Research, "Research"},
            {Action::Settings, "Settings"}
        };

        const int button_width = CalculateButtonWidth();

        for (const auto &[action, label]: actions) {
            auto button = std::make_unique<engine::ui::elements::Button>(
                button_width, HEIGHT - 10,
                label,
                UITheme::FONT_SIZE_NORMAL
            );

            button->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_DEFAULT));
            button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
            button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
            button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
            button->SetPressedColor(UITheme::ToEngineColor(UITheme::BUTTON_PRESSED));

            button->SetClickCallback([this, action](const engine::ui::MouseEvent &event) {
                if (event.left_pressed) {
                    audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                    if (action_callback_) {
                        action_callback_(action);
                    }
                    return true;
                }
                return false;
            });

            action_buttons_.push_back(button.get());
            main_panel_->AddChild(std::move(button));
        }

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void ActionBar::Render() const {
        main_panel_->Render();
    }

    void ActionBar::Update(const float delta_time) {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Check for window resize
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        main_panel_->UpdateComponentsRecursive(delta_time);
    }

    void ActionBar::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int bar_width = CalculateBarWidth();
        const int bar_x = (screen_width - bar_width) / 2;

        main_panel_->SetRelativePosition(static_cast<float>(bar_x), static_cast<float>(screen_height - HEIGHT));
        main_panel_->SetSize(static_cast<float>(bar_width), static_cast<float>(HEIGHT));

        // Update button widths
        const int button_width = CalculateButtonWidth();
        for (auto *button: action_buttons_) {
            button->SetSize(static_cast<float>(button_width), static_cast<float>(HEIGHT - 10));
        }

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void ActionBar::SetActiveAction(Action action) {
        active_action_index_ = static_cast<int>(action);

        // Update button visuals
        for (size_t i = 0; i < action_buttons_.size(); ++i) {
            const bool is_active = (static_cast<int>(i) == active_action_index_);
            action_buttons_[i]->SetNormalColor(
                UITheme::ToEngineColor(is_active ? UITheme::BUTTON_PRESSED : UITheme::BUTTON_BACKGROUND)
            );
            action_buttons_[i]->SetBorderColor(
                UITheme::ToEngineColor(is_active ? GOLD : UITheme::BORDER_DEFAULT)
            );
        }
    }

    void ActionBar::ClearActiveAction() {
        active_action_index_ = -1;

        for (auto *button: action_buttons_) {
            button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
            button->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_DEFAULT));
        }
    }

    bool ActionBar::ProcessMouseEvent(const MouseEvent &event) {
        return main_panel_->ProcessMouseEvent({
            event.x,
            event.y,
            event.left_down,
            event.right_down,
            event.left_pressed,
            event.right_pressed
        });
    }
}
