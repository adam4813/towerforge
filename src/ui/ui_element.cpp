#include "ui/ui_element.h"
#include "core/accessibility_settings.hpp"
#include <algorithm>

namespace towerforge::ui {

    UIElement::UIElement(const float relative_x, const float relative_y, const float width, const float height)
        : relative_x_(relative_x)
          , relative_y_(relative_y)
          , width_(width)
          , height_(height)
          , is_focused_(false)
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

    bool UIElement::ProcessMouseEvent(const MouseEvent& event) {
        // Update hover state
        const bool was_hovered = is_hovered_;
        is_hovered_ = Contains(event.x, event.y);

        // If mouse is not over this element or any children, nothing to do
        if (!is_hovered_) {
            return false;
        }

        // Bubble-down: Process children first (reverse order so top children get events first)
        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            if ((*it)->ProcessMouseEvent(event)) {
                // Event was consumed by a child
                return true;
            }
        }

        // Process hover event for this element
        bool consumed = false;
        if (is_hovered_) {
            consumed = OnHover(event);
        }

        // Process click event if a button was pressed
        if (!consumed && (event.left_pressed || event.right_pressed)) {
            if (is_hovered_) {
                consumed = OnClick(event);
            }
        }

        return consumed;
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
          , font_size_(20)
          , enabled_(true)
          , click_callback_(nullptr) {
    }

    bool Button::OnHover(const MouseEvent& event) {
        // Hover handling can be used for visual feedback
        // For now, just return false to allow event propagation
        return false;
    }

    bool Button::OnClick(const MouseEvent& event) {
        if (!enabled_) {
            return false;
        }

        // Only handle left clicks
        if (event.left_pressed) {
            if (click_callback_) {
                click_callback_();
            }
            return true; // Consume the event
        }

        return false;
    }

    void Button::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();
        
        // Get accessibility settings
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const bool high_contrast = accessibility.IsHighContrastEnabled();
        const float font_scale = accessibility.GetFontScale();
        
        // Determine colors based on state
        Color bg_color = background_color_;
        Color border_col = border_color_;
        Color text_col = text_color_;

        if (!enabled_) {
            bg_color = ColorAlpha(bg_color, 0.5f);
            border_col = ColorAlpha(border_col, 0.5f);
            text_col = ColorAlpha(text_col, 0.5f);
        } else if (is_hovered_ || is_focused_) {
            // Brighten colors when hovered or focused
            if (high_contrast) {
                // In high-contrast mode, use more distinct colors
                bg_color = is_focused_ ? ColorAlpha(YELLOW, 0.5f) : ColorAlpha(bg_color, bg_color.a / 255.0f * 1.5f);
                border_col = is_focused_ ? YELLOW : ColorBrightness(border_col, 0.2f);
                text_col = is_focused_ ? BLACK : text_col;
            } else {
                bg_color = ColorAlpha(bg_color, bg_color.a / 255.0f * 1.5f);
                border_col = ColorBrightness(border_col, 0.2f);
            }
        }
        
        // Apply high-contrast mode adjustments
        if (high_contrast && enabled_) {
            // Increase contrast for better visibility
            border_col = ColorBrightness(border_col, 0.3f);
        }
        
        // Draw background
        DrawRectangleRec(bounds, bg_color);
        
        // Draw border (thicker in high-contrast mode or when focused)
        const float border_thickness = (high_contrast || is_focused_) ? 3.0f : 2.0f;
        if (border_col.a > 0) {
            DrawRectangleLinesEx(bounds, border_thickness, border_col);
        }
        
        // Draw label text centered with font scaling
        if (!label_.empty()) {
            const int scaled_font_size = static_cast<int>(font_size_ * font_scale);
            const int text_width = MeasureText(label_.c_str(), scaled_font_size);
            const int text_x = bounds.x + (bounds.width - text_width) / 2;
            const int text_y = bounds.y + (bounds.height - scaled_font_size) / 2;
            DrawText(label_.c_str(), text_x, text_y, scaled_font_size, text_col);
        }
        
        // Render all children
        for (const auto& child : children_) {
            child->Render();
        }
    }

}
