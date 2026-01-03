#include <algorithm>
#include "core/accessibility_settings.hpp"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/batch_renderer/batch_adapter.h"

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
        Rectangle bounds = GetRelativeBounds();

        // Walk up the parent chain to calculate absolute position
        const UIElement *current_parent = parent_;
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

    void UIElement::SetParent(UIElement *parent) {
        parent_ = parent;
    }

    void UIElement::AddChild(std::unique_ptr<UIElement> child) {
        if (child) {
            child->SetParent(this);
            children_.push_back(std::move(child));
        }
    }

    void UIElement::RemoveChild(UIElement *child) {
        children_.erase(
            std::remove_if(children_.begin(), children_.end(),
                           [child](const std::unique_ptr<UIElement> &c) { return c.get() == child; }),
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

    bool UIElement::ProcessMouseEvent(const MouseEvent &event) {
        // Update hover state
        const bool was_hovered = is_hovered_;
        is_hovered_ = Contains(event.x, event.y);

        // Bubble-down: Process children first (reverse order so top children get events first)
        // Always process children even if parent isn't hovered, so they can clear their hover state
        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            if ((*it)->ProcessMouseEvent(event)) {
                // Event was consumed by a child
                return true;
            }
        }

        // If mouse is not over this element, we're done (children already processed)
        if (!is_hovered_) {
            return false;
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
          , border_color_(border_color)
          , padding_(0.0f)
          , is_visible_(true)
          , is_animating_(false)
          , animation_progress_(1.0f)
          , animation_speed_(3.0f) {
        // Default animation speed
    }

    void Panel::Update(const float delta_time) {
        if (is_animating_) {
            if (is_visible_) {
                // Fading in
                animation_progress_ += delta_time * animation_speed_;
                if (animation_progress_ >= 1.0f) {
                    animation_progress_ = 1.0f;
                    is_animating_ = false;
                }
            } else {
                // Fading out
                animation_progress_ -= delta_time * animation_speed_;
                if (animation_progress_ <= 0.0f) {
                    animation_progress_ = 0.0f;
                    is_animating_ = false;
                }
            }
        }
    }

    void Panel::Show(const bool animate) {
        is_visible_ = true;
        if (animate) {
            is_animating_ = true;
            if (animation_progress_ == 1.0f) {
                animation_progress_ = 0.0f;
            }
        } else {
            animation_progress_ = 1.0f;
            is_animating_ = false;
        }
    }

    void Panel::Hide(const bool animate) {
        is_visible_ = false;
        if (animate) {
            is_animating_ = true;
            if (animation_progress_ == 0.0f) {
                animation_progress_ = 1.0f;
            }
        } else {
            animation_progress_ = 0.0f;
            is_animating_ = false;
        }
    }

    Rectangle Panel::GetAbsoluteBounds() const {
        // For panels, GetAbsoluteBounds returns the content area (with padding applied)
        // This ensures children are automatically positioned within the padded area
        Rectangle bounds = GetRelativeBounds();

        // Apply padding to the bounds
        bounds.x += padding_;
        bounds.y += padding_;
        bounds.width -= padding_ * 2;
        bounds.height -= padding_ * 2;

        // Walk up the parent chain to calculate absolute position
        const UIElement *current_parent = parent_;
        while (current_parent != nullptr) {
            const Rectangle parent_bounds = current_parent->GetRelativeBounds();
            bounds.x += parent_bounds.x;
            bounds.y += parent_bounds.y;
            current_parent = current_parent->GetParent();
        }

        return bounds;
    }

    Rectangle Panel::GetPanelBounds() const {
        // Get the panel's own bounds without padding (for rendering background/border)
        Rectangle bounds = GetRelativeBounds();

        // Walk up the parent chain to calculate absolute position
        const UIElement *current_parent = parent_;
        while (current_parent != nullptr) {
            const Rectangle parent_bounds = current_parent->GetRelativeBounds();
            bounds.x += parent_bounds.x;
            bounds.y += parent_bounds.y;
            current_parent = current_parent->GetParent();
        }

        return bounds;
    }

    void Panel::Render() const {
        // Skip rendering if completely hidden
        if (animation_progress_ <= 0.0f) {
            return;
        }

        // Get panel's own bounds (not content bounds) for rendering background/border
        const Rectangle bounds = GetPanelBounds();

        // Apply alpha based on animation progress
        Color bg_color = background_color_;
        Color border_col = border_color_;

        if (animation_progress_ < 1.0f) {
            bg_color = ColorAlpha(bg_color, (bg_color.a / 255.0f) * animation_progress_);
            border_col = ColorAlpha(border_col, (border_col.a / 255.0f) * animation_progress_);
        }

        // Draw background
        batch_renderer::adapter::DrawRectangleRec(bounds, bg_color);

        // Draw border if not transparent
        if (border_col.a > 0) {
            batch_renderer::adapter::DrawRectangleLinesEx(bounds, 2, border_col);
        }

        // Render all children (they use GetAbsoluteBounds which applies padding automatically)
        for (const auto &child: children_) {
            child->Render();
        }
    }
}
