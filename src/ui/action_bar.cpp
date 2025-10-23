#include "ui/action_bar.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"

namespace towerforge::ui {

    ActionBar::ActionBar(const float relative_x, const float relative_y, const float width, const float height)
        : Panel(relative_x, relative_y, width, height,
                ColorAlpha(UITheme::BACKGROUND_PANEL, 0.9f),
                UITheme::BORDER_ACCENT)
        , action_callback_(nullptr)
        , active_action_index_(-1) {
        
        SetPadding(5);

        // Create action buttons - positions are relative to panel
        CreateActionButton(Action::Build, "Build", 5);
        CreateActionButton(Action::FacilityInfo, "Facilities", 5 + (BUTTON_WIDTH + BUTTON_SPACING) * 1);
        CreateActionButton(Action::VisitorInfo, "Visitors", 5 + (BUTTON_WIDTH + BUTTON_SPACING) * 2);
        CreateActionButton(Action::StaffManagement, "Staff", 5 + (BUTTON_WIDTH + BUTTON_SPACING) * 3);
        CreateActionButton(Action::Research, "Research", 5 + (BUTTON_WIDTH + BUTTON_SPACING) * 4);
        CreateActionButton(Action::Settings, "Settings", 5 + (BUTTON_WIDTH + BUTTON_SPACING) * 5);
    }

    void ActionBar::CreateActionButton(Action action, const std::string& label, float x) {
        auto button = std::make_unique<Button>(
            x, 5,
            BUTTON_WIDTH, height_ - 10,
            label,
            UITheme::BUTTON_BACKGROUND,
            UITheme::BORDER_DEFAULT
        );
        button->SetFontSize(14);
        
        button->SetClickCallback([this, action]() {
            if (action_callback_) {
                action_callback_(action);
            }
        });
        
        action_buttons_.push_back(button.get());
        AddChild(std::move(button));
    }

    void ActionBar::Render() const {
        Panel::Render();
    }

    void ActionBar::Update(const float delta_time) {
        Panel::Update(delta_time);
        
        for (auto* button : action_buttons_) {
            if (auto* btn = dynamic_cast<Button*>(button)) {
                btn->Update(delta_time);
            }
        }
    }

    void ActionBar::SetActiveAction(Action action) {
        active_action_index_ = static_cast<int>(action);
        
        // Update button visuals
        for (size_t i = 0; i < action_buttons_.size(); ++i) {
            const bool is_active = (static_cast<int>(i) == active_action_index_);
            action_buttons_[i]->SetBackgroundColor(
                is_active ? UITheme::BUTTON_PRESSED : UITheme::BUTTON_BACKGROUND
            );
            action_buttons_[i]->SetBorderColor(
                is_active ? GOLD : UITheme::BORDER_DEFAULT
            );
        }
    }

    void ActionBar::ClearActiveAction() {
        active_action_index_ = -1;
        
        for (auto* button : action_buttons_) {
            button->SetBackgroundColor(UITheme::BUTTON_BACKGROUND);
            button->SetBorderColor(UITheme::BORDER_DEFAULT);
        }
    }

    bool ActionBar::ProcessMouseEvent(const MouseEvent& event) {
        // Forward event to children (buttons)
        for (const auto& child : children_) {
            if (child->ProcessMouseEvent(event)) {
                return true;  // Event consumed
            }
        }
        return false;
    }

}
