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
          , is_pressed_(false)
          , press_animation_(0.0f)
          , click_callback_(nullptr) {
    }

    void Button::Update(const float delta_time) {
        // Animate press feedback
        if (press_animation_ > 0.0f) {
            press_animation_ -= delta_time * 5.0f; // Fade out over ~0.2 seconds
            if (press_animation_ <= 0.0f) {
                press_animation_ = 0.0f;
                is_pressed_ = false;  // Reset pressed state when animation completes
            }
        }
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
            // Trigger press animation
            is_pressed_ = true;
            press_animation_ = 1.0f;
            
            if (click_callback_) {
                click_callback_();
            }
            return true; // Consume the event
        }

        return false;
    }

    void Button::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();
        
        // Determine colors based on state
        Color bg_color = background_color_;
        Color border_col = border_color_;
        Color text_col = text_color_;

        // Scale factor for press animation (button "pushes in" when clicked)
        float scale = 1.0f;
        float offset = 0.0f;

        if (!enabled_) {
            bg_color = ColorAlpha(bg_color, 0.5f);
            border_col = ColorAlpha(border_col, 0.5f);
            text_col = ColorAlpha(text_col, 0.5f);
        } else if (press_animation_ > 0.0f) {
            // Press animation - brighten and slightly shrink
            const float intensity = press_animation_;
            bg_color = ColorBrightness(bg_color, 0.3f * intensity);
            border_col = ColorBrightness(border_col, 0.3f * intensity);
            scale = 1.0f - (0.05f * intensity); // Shrink by 5% at peak
            offset = 2.0f * intensity; // Offset down slightly
        } else if (is_hovered_) {
            // Brighten colors when hovered
            bg_color = ColorBrightness(bg_color, 0.2f);
            border_col = ColorBrightness(border_col, 0.2f);
        }
        
        // Apply scale and offset for press animation
        Rectangle draw_bounds = bounds;
        if (scale != 1.0f) {
            const float width_diff = bounds.width * (1.0f - scale);
            const float height_diff = bounds.height * (1.0f - scale);
            draw_bounds.x += width_diff / 2.0f;
            draw_bounds.y += height_diff / 2.0f + offset;
            draw_bounds.width *= scale;
            draw_bounds.height *= scale;
        } else {
            draw_bounds.y += offset;
        }
        
        // Draw background
        DrawRectangleRec(draw_bounds, bg_color);
        
        // Draw border if not transparent
        if (border_col.a > 0) {
            DrawRectangleLinesEx(draw_bounds, 2, border_col);
        }
        
        // Draw label text centered
        if (!label_.empty()) {
            const int text_width = MeasureText(label_.c_str(), font_size_);
            const int text_x = draw_bounds.x + (draw_bounds.width - text_width) / 2;
            const int text_y = draw_bounds.y + (draw_bounds.height - font_size_) / 2;
            DrawText(label_.c_str(), text_x, text_y, font_size_, text_col);
        }
        
        // Render all children
        for (const auto& child : children_) {
            child->Render();
        }
    }

    // ConfirmationDialog implementation
    ConfirmationDialog::ConfirmationDialog(const std::string& title,
                                          const std::string& message,
                                          const std::string& confirm_text,
                                          const std::string& cancel_text)
        : Panel(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, ColorAlpha(Color{30, 30, 40, 255}, 0.95f), GOLD)
          , title_(title)
          , message_(message)
          , confirm_callback_(nullptr)
          , cancel_callback_(nullptr)
          , is_visible_(false)
          , animation_time_(0.0f) {
        
        // Create confirm button
        confirm_button_ = std::make_unique<Button>(
            DIALOG_WIDTH / 2 - BUTTON_WIDTH - 10,
            DIALOG_HEIGHT - BUTTON_HEIGHT - 20,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            confirm_text,
            ColorAlpha(GREEN, 0.5f),
            LIME
        );
        
        // Create cancel button
        cancel_button_ = std::make_unique<Button>(
            DIALOG_WIDTH / 2 + 10,
            DIALOG_HEIGHT - BUTTON_HEIGHT - 20,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            cancel_text,
            ColorAlpha(RED, 0.5f),
            MAROON
        );
        
        // Set button callbacks
        confirm_button_->SetClickCallback([this]() {
            if (confirm_callback_) {
                confirm_callback_();
            }
            Hide();
        });
        
        cancel_button_->SetClickCallback([this]() {
            if (cancel_callback_) {
                cancel_callback_();
            }
            Hide();
        });
        
        // Add buttons as children
        AddChild(std::move(confirm_button_));
        AddChild(std::move(cancel_button_));
    }

    void ConfirmationDialog::Update(const float delta_time) {
        if (!is_visible_) return;
        
        animation_time_ += delta_time;
        
        // Update buttons
        for (const auto& child : GetChildren()) {
            if (auto* btn = dynamic_cast<Button*>(child.get())) {
                btn->Update(delta_time);
            }
        }
    }

    bool ConfirmationDialog::ProcessMouseEvent(const MouseEvent& event) {
        if (!is_visible_) return false;
        
        // Process children (buttons)
        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            if ((*it)->ProcessMouseEvent(event)) {
                return true;
            }
        }
        
        // Consume all events if dialog is visible (modal behavior)
        return Contains(event.x, event.y);
    }

    void ConfirmationDialog::Render() const {
        if (!is_visible_) return;
        
        // Draw semi-transparent overlay
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.5f));
        
        // Center the dialog on screen
        const int dialog_x = (screen_width - DIALOG_WIDTH) / 2;
        const int dialog_y = (screen_height - DIALOG_HEIGHT) / 2;
        
        // Temporarily set position to centered
        const_cast<ConfirmationDialog*>(this)->SetRelativePosition(dialog_x, dialog_y);
        
        // Draw dialog background and border
        Panel::Render();
        
        // Draw title
        const int title_width = MeasureText(title_.c_str(), 24);
        DrawText(title_.c_str(), 
                dialog_x + (DIALOG_WIDTH - title_width) / 2,
                dialog_y + 20,
                24,
                GOLD);
        
        // Draw separator
        DrawLine(dialog_x + 20, dialog_y + 55, 
                dialog_x + DIALOG_WIDTH - 20, dialog_y + 55,
                GRAY);
        
        // Draw message (word-wrapped)
        constexpr int message_y = 75;
        constexpr int max_line_width = DIALOG_WIDTH - 40;
        int current_y = dialog_y + message_y;
        
        // Simple word wrapping
        std::string remaining = message_;
        while (!remaining.empty()) {
            int chars_fit = 0;
            int last_space = -1;
            
            for (size_t i = 0; i < remaining.length(); ++i) {
                std::string test_str = remaining.substr(0, i + 1);
                if (MeasureText(test_str.c_str(), 16) > max_line_width) {
                    break;
                }
                chars_fit = i + 1;
                if (remaining[i] == ' ') {
                    last_space = i;
                }
            }
            
            // If we didn't fit the whole string, break at last space
            if (chars_fit < static_cast<int>(remaining.length()) && last_space > 0) {
                chars_fit = last_space;
            }
            
            std::string line = remaining.substr(0, chars_fit);
            DrawText(line.c_str(), dialog_x + 20, current_y, 16, WHITE);
            
            current_y += 20;
            remaining = remaining.substr(chars_fit);
            
            // Trim leading spaces from next line
            while (!remaining.empty() && remaining[0] == ' ') {
                remaining = remaining.substr(1);
            }
        }
    }

}
