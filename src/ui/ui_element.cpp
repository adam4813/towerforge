#include "ui/ui_element.h"
#include <algorithm>

namespace towerforge::ui {

    UIElement::UIElement(const float relative_x, const float relative_y, const float width, const float height)
        : relative_x_(relative_x)
          , relative_y_(relative_y)
          , width_(width)
          , height_(height)
          , parent_(nullptr) {
    }

    Rectangle UIElement::GetAbsoluteBounds() const {
        Rectangle bounds = {relative_x_, relative_y_, width_, height_};
        
        // Walk up the parent chain to calculate absolute position
        UIElement* current_parent = parent_;
        while (current_parent != nullptr) {
            const Rectangle parent_bounds = current_parent->GetRelativeBounds();
            bounds.x += parent_bounds.x;
            bounds.y += parent_bounds.y;
            current_parent = current_parent->parent_;
        }
        
        return bounds;
    }

    Rectangle UIElement::GetRelativeBounds() const {
        return Rectangle{relative_x_, relative_y_, width_, height_};
    }

    void UIElement::SetParent(UIElement* parent) {
        parent_ = parent;
    }

    void UIElement::AddChild(std::unique_ptr<UIElement> child) {
        if (child) {
            child->SetParent(this);
            children_.push_back(std::move(child));
        }
    }

    void UIElement::RemoveChild(UIElement* child) {
        children_.erase(
            std::remove_if(children_.begin(), children_.end(),
                           [child](const std::unique_ptr<UIElement>& c) { return c.get() == child; }),
            children_.end()
        );
    }

    void UIElement::SetRelativePosition(const float x, const float y) {
        relative_x_ = x;
        relative_y_ = y;
    }

    void UIElement::SetSize(const float width, const float height) {
        width_ = width;
        height_ = height;
    }

    bool UIElement::Contains(const float x, const float y) const {
        const Rectangle bounds = GetAbsoluteBounds();
        return x >= bounds.x && x <= bounds.x + bounds.width &&
               y >= bounds.y && y <= bounds.y + bounds.height;
    }

    // Panel implementation
    Panel::Panel(const float relative_x, const float relative_y, const float width, const float height,
                 const Color background_color, const Color border_color)
        : UIElement(relative_x, relative_y, width, height)
          , background_color_(background_color)
          , border_color_(border_color) {
    }

    void Panel::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();
        
        // Draw background
        DrawRectangleRec(bounds, background_color_);
        
        // Draw border if not transparent
        if (border_color_.a > 0) {
            DrawRectangleLinesEx(bounds, 2, border_color_);
        }
        
        // Render all children
        for (const auto& child : children_) {
            child->Render();
        }
    }

    // Button implementation
    Button::Button(const float relative_x, const float relative_y, const float width, const float height,
                   const std::string& label, const Color background_color, const Color border_color)
        : UIElement(relative_x, relative_y, width, height)
          , label_(label)
          , background_color_(background_color)
          , border_color_(border_color)
          , text_color_(WHITE)
          , font_size_(20) {
    }

    void Button::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();
        
        // Draw background
        DrawRectangleRec(bounds, background_color_);
        
        // Draw border if not transparent
        if (border_color_.a > 0) {
            DrawRectangleLinesEx(bounds, 2, border_color_);
        }
        
        // Draw label text centered
        if (!label_.empty()) {
            const int text_width = MeasureText(label_.c_str(), font_size_);
            const int text_x = bounds.x + (bounds.width - text_width) / 2;
            const int text_y = bounds.y + (bounds.height - font_size_) / 2;
            DrawText(label_.c_str(), text_x, text_y, font_size_, text_color_);
        }
        
        // Render all children
        for (const auto& child : children_) {
            child->Render();
        }
    }

}
