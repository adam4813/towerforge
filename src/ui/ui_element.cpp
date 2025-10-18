#include "ui/ui_element.h"
#include "ui/ui_theme.h"
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
          , border_color_(border_color)
          , is_visible_(true)
          , is_animating_(false)
          , animation_progress_(1.0f)
          , animation_speed_(3.0f) {  // Default animation speed
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

    void Panel::Render() const {
        // Skip rendering if completely hidden
        if (animation_progress_ <= 0.0f) {
            return;
        }

        const Rectangle bounds = GetAbsoluteBounds();
        
        // Apply alpha based on animation progress
        Color bg_color = background_color_;
        Color border_col = border_color_;
        
        if (animation_progress_ < 1.0f) {
            bg_color = ColorAlpha(bg_color, (bg_color.a / 255.0f) * animation_progress_);
            border_col = ColorAlpha(border_col, (border_col.a / 255.0f) * animation_progress_);
        }
        
        // Draw background
        DrawRectangleRec(bounds, bg_color);
        
        // Draw border if not transparent
        if (border_col.a > 0) {
            DrawRectangleLinesEx(bounds, 2, border_col);
        }
        
        // Render all children (they will also be affected by parent alpha)
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
        
        // Get accessibility settings
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const bool high_contrast = accessibility.IsHighContrastEnabled();
        const float font_scale = accessibility.GetFontScale();
        
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
        
        // Draw border (thicker in high-contrast mode or when focused)
        const float border_thickness = (high_contrast || is_focused_) ? 3.0f : 2.0f;
        if (border_col.a > 0) {
            DrawRectangleLinesEx(draw_bounds, border_thickness, border_col);
        }
        
        // Draw label text centered with font scaling
        if (!label_.empty()) {
            const int scaled_font_size = static_cast<int>(font_size_ * font_scale);
            const int text_width = MeasureText(label_.c_str(), scaled_font_size);
            const int text_x = draw_bounds.x + (draw_bounds.width - text_width) / 2;
            const int text_y = draw_bounds.y + (draw_bounds.height - scaled_font_size) / 2;
            DrawText(label_.c_str(), text_x, text_y, scaled_font_size, text_col);
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
        : Panel(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, ColorAlpha(UITheme::BACKGROUND_MODAL, 0.95f), UITheme::PRIMARY)
          , title_(title)
          , message_(message)
          , confirm_button_(nullptr)
          , cancel_button_(nullptr)
          , confirm_callback_(nullptr)
          , cancel_callback_(nullptr)
          , is_visible_(false)
          , animation_time_(0.0f) {
        
        // Create confirm button
        confirm_button_ = std::make_unique<Button>(
            DIALOG_WIDTH / 2 - BUTTON_WIDTH - UITheme::PADDING_SMALL,
            DIALOG_HEIGHT - BUTTON_HEIGHT - UITheme::PADDING_LARGE,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            confirm_text,
            ColorAlpha(UITheme::SUCCESS, 0.5f),
            UITheme::SUCCESS
        );
        
        // Create cancel button
        cancel_button_ = std::make_unique<Button>(
            DIALOG_WIDTH / 2 + UITheme::PADDING_SMALL,
            DIALOG_HEIGHT - BUTTON_HEIGHT - UITheme::PADDING_LARGE,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            cancel_text,
            ColorAlpha(UITheme::ERROR, 0.5f),
            UITheme::ERROR
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
        
        // Calculate centered dialog position
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int dialog_x = (screen_width - DIALOG_WIDTH) / 2;
        const int dialog_y = (screen_height - DIALOG_HEIGHT) / 2;
        
        // Check if click is on any button and handle it directly
        if (event.left_pressed) {
            for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
                if (auto* btn = dynamic_cast<Button*>(it->get())) {
                    const Rectangle child_bounds = btn->GetRelativeBounds();
                    const int btn_x = dialog_x + static_cast<int>(child_bounds.x);
                    const int btn_y = dialog_y + static_cast<int>(child_bounds.y);
                    
                    // Check if click is within button bounds
                    if (event.x >= btn_x && event.x <= btn_x + child_bounds.width &&
                        event.y >= btn_y && event.y <= btn_y + child_bounds.height) {
                        
                        if (btn->IsEnabled()) {
                            // Trigger the button's OnClick handler
                            MouseEvent btn_event = event;
                            btn->OnClick(btn_event);
                            return true;
                        }
                    }
                }
            }
        }
        
        // Consume all events if dialog is visible (modal behavior)
        // Check if click is within dialog bounds
        return (event.x >= dialog_x && event.x <= dialog_x + DIALOG_WIDTH &&
                event.y >= dialog_y && event.y <= dialog_y + DIALOG_HEIGHT);
    }

    void ConfirmationDialog::Render() const {
        if (!is_visible_) return;
        
        // Draw semi-transparent overlay
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        DrawRectangle(0, 0, screen_width, screen_height, UITheme::OVERLAY_DARK);
        
        // Center the dialog on screen
        const int dialog_x = (screen_width - DIALOG_WIDTH) / 2;
        const int dialog_y = (screen_height - DIALOG_HEIGHT) / 2;
        
        // Draw dialog background and border (at centered position)
        DrawRectangle(dialog_x, dialog_y, DIALOG_WIDTH, DIALOG_HEIGHT, GetBackgroundColor());
        if (GetBorderColor().a > 0) {
            DrawRectangleLinesEx(Rectangle{static_cast<float>(dialog_x), static_cast<float>(dialog_y), 
                                          static_cast<float>(DIALOG_WIDTH), static_cast<float>(DIALOG_HEIGHT)}, 
                                UITheme::BORDER_NORMAL, GetBorderColor());
        }
        
        // Draw title
        const int title_width = MeasureText(title_.c_str(), UITheme::FONT_SIZE_LARGE);
        DrawText(title_.c_str(), 
                dialog_x + (DIALOG_WIDTH - title_width) / 2,
                dialog_y + UITheme::PADDING_LARGE,
                UITheme::FONT_SIZE_LARGE,
                UITheme::PRIMARY);
        
        // Draw separator
        DrawLine(dialog_x + UITheme::PADDING_LARGE, dialog_y + 55, 
                dialog_x + DIALOG_WIDTH - UITheme::PADDING_LARGE, dialog_y + 55,
                UITheme::BORDER_SUBTLE);
        
        // Draw message (word-wrapped)
        constexpr int message_y = 75;
        const int max_line_width = DIALOG_WIDTH - (UITheme::PADDING_MEDIUM * 2);
        int current_y = dialog_y + message_y;
        
        // Simple word wrapping
        std::string remaining = message_;
        while (!remaining.empty()) {
            int chars_fit = 0;
            int last_space = -1;
            
            for (size_t i = 0; i < remaining.length(); ++i) {
                std::string test_str = remaining.substr(0, i + 1);
                if (MeasureText(test_str.c_str(), UITheme::FONT_SIZE_NORMAL) > max_line_width) {
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
            DrawText(line.c_str(), dialog_x + UITheme::PADDING_LARGE, current_y, UITheme::FONT_SIZE_NORMAL, UITheme::TEXT_PRIMARY);
            
            current_y += UITheme::PADDING_LARGE;
            remaining = remaining.substr(chars_fit);
            
            // Trim leading spaces from next line
            while (!remaining.empty() && remaining[0] == ' ') {
                remaining = remaining.substr(1);
            }
        }
        
        // Render buttons by temporarily adjusting their parent's position in the rendering context
        // Since buttons are children, we need to render them with the dialog's centered position
        // We'll do this by directly drawing them at the correct absolute positions
        for (const auto& child : GetChildren()) {
            if (const auto* btn = dynamic_cast<const Button*>(child.get())) {
                // Calculate absolute button position
                const Rectangle child_bounds = child->GetRelativeBounds();
                const int btn_x = dialog_x + static_cast<int>(child_bounds.x);
                const int btn_y = dialog_y + static_cast<int>(child_bounds.y);
                
                // Draw button background
                Color bg_color = btn->GetBackgroundColor();
                Color border_color = btn->GetBorderColor();
                Color text_color = btn->GetTextColor();
                
                if (!btn->IsEnabled()) {
                    bg_color = ColorAlpha(bg_color, 0.5f);
                    border_color = ColorAlpha(border_color, 0.5f);
                    text_color = ColorAlpha(text_color, 0.5f);
                } else if (btn->IsHovered()) {
                    bg_color = ColorBrightness(bg_color, 0.2f);
                    border_color = ColorBrightness(border_color, 0.2f);
                }
                
                const Rectangle btn_rect{static_cast<float>(btn_x), static_cast<float>(btn_y), 
                                        child_bounds.width, child_bounds.height};
                
                DrawRectangleRec(btn_rect, bg_color);
                if (border_color.a > 0) {
                    DrawRectangleLinesEx(btn_rect, UITheme::BORDER_NORMAL, border_color);
                }
                
                // Draw button text
                const std::string& label = btn->GetLabel();
                if (!label.empty()) {
                    const int text_width = MeasureText(label.c_str(), btn->GetFontSize());
                    const int text_x = btn_x + (static_cast<int>(child_bounds.width) - text_width) / 2;
                    const int text_y = btn_y + (static_cast<int>(child_bounds.height) - btn->GetFontSize()) / 2;
                    DrawText(label.c_str(), text_x, text_y, btn->GetFontSize(), text_color);
                }
            }
        }
    }

}
