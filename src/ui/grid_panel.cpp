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

    Button* GridPanel::AddItem(const std::string& label, int data_index, const Color background_color) {
        auto button = std::make_unique<Button>(
            0, 0,  // Position calculated in RepositionItems
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
        items_.push_back({std::move(button), data_index});
        
        RepositionItems();
        UpdateScrollBounds();
        
        return button_ptr;
    }

    void GridPanel::ClearItems() {
        items_.clear();
        scroll_offset_ = 0.0f;
        max_scroll_ = 0.0f;
        selected_item_index_ = -1;
        
        // Clear children from Panel
        // Note: This is a simplified approach - in production we'd have better child management
        children_.clear();
    }

    void GridPanel::RepositionItems() {
        children_.clear();
        
        for (size_t i = 0; i < items_.size(); ++i) {
            const int row = static_cast<int>(i) / columns_;
            const int col = static_cast<int>(i) % columns_;
            
            const float x = col * (item_size_ + spacing_);
            const float y = row * (item_size_ + spacing_) - scroll_offset_;
            
            items_[i].button->SetRelativePosition(x, y);
            
            // Only add to children if visible (optimization)
            const float panel_height = height_ - GetPadding() * 2;
            if (y + item_size_ >= -scroll_offset_ && y < panel_height) {
                AddChild(std::move(items_[i].button));
                // Re-wrap the button for next iteration
                items_[i].button = std::unique_ptr<Button>(static_cast<Button*>(children_.back().release()));
                children_.pop_back();
            }
        }
        
        // Re-add all items to children for rendering
        for (auto& item : items_) {
            // This is a bit awkward - we need a better child management system
            // For now, we'll just leave them positioned
        }
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
        
        // Render visible items
        for (const auto& item : items_) {
            const Rectangle bounds = item.button->GetAbsoluteBounds();
            const Rectangle panel_bounds = GetAbsoluteBounds();
            
            // Clip to panel bounds
            if (bounds.y + bounds.height >= panel_bounds.y && 
                bounds.y <= panel_bounds.y + panel_bounds.height) {
                item.button->Render();
            }
        }
    }

    void GridPanel::Update(const float delta_time) {
        Panel::Update(delta_time);
        
        // Handle mouse wheel scrolling
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scroll_offset_ -= wheel * 30.0f;
            scroll_offset_ = std::clamp(scroll_offset_, 0.0f, max_scroll_);
            RepositionItems();
        }
        
        // Update visible items
        for (auto& item : items_) {
            item.button->Update(delta_time);
        }
    }

    void GridPanel::SetSelectedItem(const int data_index) {
        selected_item_index_ = data_index;
        
        // Update button borders
        for (auto& item : items_) {
            item.button->SetBorderColor(item.data_index == data_index ? GOLD : GRAY);
        }
    }

}
