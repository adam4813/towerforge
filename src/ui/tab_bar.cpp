#include "ui/tab_bar.h"
#include "ui/ui_theme.h"

namespace towerforge::ui {

    TabBar::TabBar(const float relative_x, const float relative_y, const float width, float height,
                   const std::vector<std::string>& tab_labels)
        : UIElement(relative_x, relative_y, width, height)
        , active_tab_index_(0)
        , tab_selected_callback_(nullptr) {
        
        if (tab_labels.empty()) return;

        const float tab_width = width / static_cast<float>(tab_labels.size());
        
        for (size_t i = 0; i < tab_labels.size(); ++i) {
            const bool is_active = (i == 0);
            auto button = std::make_unique<Button>(
                i * tab_width, 0,
                tab_width, height,
                tab_labels[i],
                is_active ? UITheme::BUTTON_PRESSED : UITheme::BUTTON_BACKGROUND,
                is_active ? UITheme::BORDER_ACCENT : UITheme::BORDER_DEFAULT
            );
            
            const int tab_index = static_cast<int>(i);
            button->SetClickCallback([this, tab_index]() {
                SetActiveTab(tab_index);
                if (tab_selected_callback_) {
                    tab_selected_callback_(tab_index);
                }
            });
            
            tab_buttons_.push_back(button.get());
            AddChild(std::move(button));
        }
    }

    void TabBar::Render() const {
        for (const auto& child : children_) {
            child->Render();
        }
    }

    void TabBar::Update(const float delta_time) const {
        for (auto* button : tab_buttons_) {
            if (auto* btn = dynamic_cast<Button*>(button)) {
                btn->Update(delta_time);
            }
        }
    }

    void TabBar::SetActiveTab(const int index) {
        if (index < 0 || index >= static_cast<int>(tab_buttons_.size())) return;
        
        active_tab_index_ = index;
        
        // Update button visuals
        for (size_t i = 0; i < tab_buttons_.size(); ++i) {
            const bool is_active = (static_cast<int>(i) == active_tab_index_);
            tab_buttons_[i]->SetBackgroundColor(
                is_active ? UITheme::BUTTON_PRESSED : UITheme::BUTTON_BACKGROUND
            );
            tab_buttons_[i]->SetBorderColor(
                is_active ? UITheme::BORDER_ACCENT : UITheme::BORDER_DEFAULT
            );
        }
    }

}
