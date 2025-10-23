#include "ui/grid_panel.h"
#include "ui/ui_theme.h"
#include <algorithm>

namespace towerforge::ui {

    GridPanel::GridPanel(float relative_x, float relative_y, float width, float height,
                         const int columns, const float item_size, const float spacing)
        : Panel(relative_x, relative_y, width, height,
                ColorAlpha(UITheme::BACKGROUND_DARK, 0.7f),
                UITheme::BORDER_DEFAULT)
        , columns_(columns)
        , item_size_(item_size)
        , spacing_(spacing)
        , scroll_offset_(0.0f)
        , max_scroll_(0.0f)
        , selected_item_index_(-1)
        , item_selected_callback_(nullptr) {
        
        SetPadding(spacing);
    }

    Button* GridPanel::AddItem(const std::string& label, int data_index, Color background_color) {
        const int row = static_cast<int>(items_.size()) / columns_;
        const int col = static_cast<int>(items_.size()) % columns_;
        
        const float x = col * (item_size_ + spacing_) + spacing_;
        const float y = row * (item_size_ + spacing_) - scroll_offset_ + spacing_;
        
        auto button = std::make_unique<Button>(
            x, y,
            item_size_, item_size_,
            label,
            ColorAlpha(background_color, 0.3f),
            data_index == selected_item_index_ ? GOLD : GRAY
        );
        button->SetFontSize(12);
        
        button->SetClickCallback([this, data_index]() {
            SetSelectedItem(data_index);
            if (item_selected_callback_) {
                item_selected_callback_(data_index);
            }
        });
        
        Button* button_ptr = button.get();
        
        // Add to children for rendering and event handling
        AddChild(std::move(button));
        
        // Store reference in items (button now owned by children_)
        items_.push_back({nullptr, data_index});
        
        UpdateScrollBounds();
        
        return button_ptr;
    }

    void GridPanel::ClearItems() {
        items_.clear();
        children_.clear();  // Clear all child buttons
        scroll_offset_ = 0.0f;
        max_scroll_ = 0.0f;
        selected_item_index_ = -1;
    }

    void GridPanel::UpdateScrollBounds() {
        const int total_rows = (static_cast<int>(items_.size()) + columns_ - 1) / columns_;
        const float total_height = total_rows * (item_size_ + spacing_);
        const float visible_height = height_ - GetPadding() * 2;
        
        max_scroll_ = std::max(0.0f, total_height - visible_height);
    }

    void GridPanel::Render() const {
        // Render panel background and border
        Panel::Render();
        
        // Render all children (buttons)
        for (const auto& child : children_) {
            child->Render();
        }
    }

    void GridPanel::Update(float delta_time) {
        Panel::Update(delta_time);
        
        // Handle mouse wheel scrolling
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scroll_offset_ -= wheel * 30.0f;
            scroll_offset_ = std::clamp(scroll_offset_, 0.0f, max_scroll_);
            
            // Reposition all buttons based on new scroll offset
            for (size_t i = 0; i < children_.size(); ++i) {
                const int row = static_cast<int>(i) / columns_;
                const int col = static_cast<int>(i) % columns_;
                
                const float x = col * (item_size_ + spacing_) + spacing_;
                const float y = row * (item_size_ + spacing_) - scroll_offset_ + spacing_;
                
                children_[i]->SetRelativePosition(x, y);
            }
        }
        
        // Update all child buttons
        for (auto& child : children_) {
            if (auto* btn = dynamic_cast<Button*>(child.get())) {
                btn->Update(delta_time);
            }
        }
    }

    void GridPanel::SetSelectedItem(int data_index) {
        selected_item_index_ = data_index;
        
        // Update button borders
        for (size_t i = 0; i < items_.size(); ++i) {
            if (i < children_.size()) {
                if (auto* btn = dynamic_cast<Button*>(children_[i].get())) {
                    btn->SetBorderColor(items_[i].data_index == data_index ? GOLD : GRAY);
                }
            }
        }
    }

}
