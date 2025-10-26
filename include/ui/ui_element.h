#pragma once

#include <raylib.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "mouse_interface.h"

namespace towerforge::ui {

    /**
     * @brief Base class for all UI elements with parent/child relationship support
     * 
     * This class provides a unified concept for UI elements throughout the codebase.
     * Each UI element maintains its bounding rectangle relative to its parent's position.
     * Implements IMouseInteractive for unified mouse event handling.
     */
    class UIElement : public IMouseInteractive {
    public:
        /**
         * @brief Construct a UI element with relative positioning
         * @param relative_x X position relative to parent (or screen if no parent)
         * @param relative_y Y position relative to parent (or screen if no parent)
         * @param width Width of the element
         * @param height Height of the element
         */
        UIElement(float relative_x, float relative_y, float width, float height);
        
        virtual ~UIElement() = default;

        /**
         * @brief Get the absolute bounding rectangle (accounting for parent position)
         * @return Absolute screen-space rectangle
         */
        virtual Rectangle GetAbsoluteBounds() const;

        /**
         * @brief Get the relative bounding rectangle (relative to parent)
         * @return Relative rectangle
         */
        virtual Rectangle GetRelativeBounds() const;

        /**
         * @brief Set the parent element
         * @param parent Pointer to parent element (nullptr for no parent)
         */
        void SetParent(UIElement* parent);

        /**
         * @brief Get the parent element
         * @return Pointer to parent element (nullptr if no parent)
         */
        UIElement* GetParent() const { return parent_; }

        /**
         * @brief Add a child element
         * @param child Unique pointer to child element
         */
        void AddChild(std::unique_ptr<UIElement> child);

        /**
         * @brief Remove a child element
         * @param child Pointer to child element to remove
         */
        void RemoveChild(UIElement* child);

        /**
         * @brief Get all children
         * @return Reference to vector of child elements
         */
        const std::vector<std::unique_ptr<UIElement>>& GetChildren() const { return children_; }

        /**
         * @brief Set the relative position
         * @param x X position relative to parent
         * @param y Y position relative to parent
         */
        void SetRelativePosition(float x, float y);

        /**
         * @brief Set the size
         * @param width Width of the element
         * @param height Height of the element
         */
        void SetSize(float width, float height);

        /**
         * @brief Check if a point is inside the element
         * @param x Screen X coordinate
         * @param y Screen Y coordinate
         * @return True if point is inside element
         */
        bool Contains(float x, float y) const override;

        /**
         * @brief Set keyboard focus state
         * @param focused Whether this element has keyboard focus
         */
        void SetFocused(const bool focused) { is_focused_ = focused; }

        /**
         * @brief Check if element has keyboard focus
         * @return True if element has keyboard focus
         */
        bool IsFocused() const { return is_focused_; }

        /**
         * @brief Render the element (can be overridden by derived classes)
         */
        virtual void Render() const {}

        /**
         * @brief Process mouse events with bubble-down propagation
         * 
         * This method handles mouse events by first propagating them to children
         * (bubble-down), then calling the element's own event handlers if not consumed.
         * 
         * @param event Mouse event data
         * @return true if the event was handled and should not propagate further
         */
        virtual bool ProcessMouseEvent(const MouseEvent& event);

        /**
         * @brief Handle mouse hover event (override in derived classes)
         * 
         * @param event Mouse event data
         * @return true if the event was consumed
         */
        bool OnHover(const MouseEvent& event) override { return false; }

        /**
         * @brief Handle mouse click event (override in derived classes)
         * 
         * @param event Mouse event data
         * @return true if the event was consumed
         */
        bool OnClick(const MouseEvent& event) override { return false; }

    protected:
        float relative_x_;      // X position relative to parent
        float relative_y_;      // Y position relative to parent
        float width_;           // Width of the element
        float height_;          // Height of the element
        bool is_focused_;       // Whether element has keyboard focus

        UIElement* parent_;     // Parent element (nullptr for root elements)
        std::vector<std::unique_ptr<UIElement>> children_;  // Child elements
    };

    /**
     * @brief Panel class - a basic rectangular container for UI elements
     * 
     * Represents a panel that can contain other UI elements and provides
     * basic rendering with background and optional border.
     * Supports animated show/hide transitions and padding for children.
     */
    class Panel : public UIElement {
    public:
        /**
         * @brief Construct a panel
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the panel
         * @param height Height of the panel
         * @param background_color Background color
         * @param border_color Border color (pass BLANK for no border)
         */
        Panel(float relative_x, float relative_y, float width, float height,
              Color background_color = ColorAlpha(BLACK, 0.8f),
              Color border_color = BLANK);

        /**
         * @brief Update panel state for animations
         * @param delta_time Time elapsed since last frame
         */
        virtual void Update(float delta_time);

        /**
         * @brief Render the panel
         */
        void Render() const override;
        
        /**
         * @brief Get absolute bounds - overridden to account for padding on children
         * When getting absolute bounds for positioning children, padding is added
         * @return Absolute screen-space rectangle with padding applied
         */
        Rectangle GetAbsoluteBounds() const override;

        /**
         * @brief Set background color
         */
        void SetBackgroundColor(const Color color) { background_color_ = color; }

        /**
         * @brief Set border color
         */
        void SetBorderColor(const Color color) { border_color_ = color; }

        /**
         * @brief Get background color
         */
        Color GetBackgroundColor() const { return background_color_; }

        /**
         * @brief Get border color
         */
        Color GetBorderColor() const { return border_color_; }
        
        /**
         * @brief Set padding for children
         * @param padding Padding in pixels applied to all sides
         */
        void SetPadding(const float padding) { padding_ = padding; }
        
        /**
         * @brief Get padding
         */
        float GetPadding() const { return padding_; }
        
        /**
         * @brief Get the panel's own bounds (without padding applied)
         * Use this to render the panel background/border
         * @return Panel's full rectangle
         */
        Rectangle GetPanelBounds() const;

        /**
         * @brief Show panel with animation
         * @param animate Whether to animate the transition
         */
        void Show(bool animate = true);

        /**
         * @brief Hide panel with animation
         * @param animate Whether to animate the transition
         */
        void Hide(bool animate = true);

        /**
         * @brief Check if panel is visible
         */
        bool IsVisible() const { return is_visible_; }

        /**
         * @brief Check if panel animation is complete
         */
        bool IsAnimationComplete() const { return animation_progress_ >= 1.0f || animation_progress_ <= 0.0f; }

    private:
        Color background_color_;
        Color border_color_;
        float padding_;
        bool is_visible_;
        bool is_animating_;
        float animation_progress_;  // 0.0 = fully hidden, 1.0 = fully visible
        float animation_speed_;
    };

    /**
     * @brief Button class - an interactive UI element for user actions
     * 
     * Represents a clickable button with text label, background, border,
     * and support for different states (normal, hovered, pressed, disabled).
     */
    class Button : public UIElement {
    public:
        /**
         * @brief Callback type for button click events
         */
        using ClickCallback = std::function<void()>;

        /**
         * @brief Construct a button
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the button
         * @param height Height of the button
         * @param label Text label for the button
         * @param background_color Background color (normal state)
         * @param border_color Border color (normal state)
         */
        Button(float relative_x, float relative_y, float width, float height,
               const std::string& label = "",
               Color background_color = ColorAlpha(DARKGRAY, 0.3f),
               Color border_color = GRAY);

        /**
         * @brief Render the button
         */
        void Render() const override;

        /**
         * @brief Update button state for animations
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Handle hover event
         */
        bool OnHover(const MouseEvent& event) override;

        /**
         * @brief Handle click event
         */
        bool OnClick(const MouseEvent& event) override;

        /**
         * @brief Set click callback
         * @param callback Function to call when button is clicked
         */
        void SetClickCallback(const ClickCallback &callback) { click_callback_ = callback; }

        /**
         * @brief Set the button label
         */
        void SetLabel(const std::string& label) { label_ = label; }

        /**
         * @brief Get the button label
         */
        const std::string& GetLabel() const { return label_; }

        /**
         * @brief Set background color
         */
        void SetBackgroundColor(const Color color) { background_color_ = color; }

        /**
         * @brief Set border color
         */
        void SetBorderColor(const Color color) { border_color_ = color; }

        /**
         * @brief Set text color
         */
        void SetTextColor(const Color color) { text_color_ = color; }

        /**
         * @brief Get background color
         */
        Color GetBackgroundColor() const { return background_color_; }

        /**
         * @brief Get border color
         */
        Color GetBorderColor() const { return border_color_; }

        /**
         * @brief Get text color
         */
        Color GetTextColor() const { return text_color_; }

        /**
         * @brief Set font size
         */
        void SetFontSize(const int size) { font_size_ = size; }

        /**
         * @brief Get font size
         */
        int GetFontSize() const { return font_size_; }

        /**
         * @brief Set enabled state
         */
        void SetEnabled(const bool enabled) { enabled_ = enabled; }

        /**
         * @brief Get enabled state
         */
        bool IsEnabled() const { return enabled_; }

        /**
         * @brief Check if button is currently pressed
         */
        bool IsPressed() const { return is_pressed_; }

    private:
        std::string label_;
        Color background_color_;
        Color border_color_;
        Color text_color_;
        int font_size_;
        bool enabled_;
        bool is_pressed_;           // Visual pressed state for feedback
        float press_animation_;     // Animation timer for press feedback (0.0 - 1.0)
        ClickCallback click_callback_;
    };

    /**
     * @brief ConfirmationDialog class - a modal dialog for confirming actions
     * 
     * Displays a message with "Confirm" and "Cancel" buttons.
     * Used for destructive or expensive actions.
     */
    class ConfirmationDialog : public Panel {
    public:
        /**
         * @brief Callback types for dialog buttons
         */
        using ConfirmCallback = std::function<void()>;
        using CancelCallback = std::function<void()>;

        /**
         * @brief Construct a confirmation dialog
         * @param title Dialog title
         * @param message Dialog message
         * @param confirm_text Text for confirm button (default: "Confirm")
         * @param cancel_text Text for cancel button (default: "Cancel")
         */
        ConfirmationDialog(const std::string& title,
                          const std::string& message,
                          const std::string& confirm_text = "Confirm",
                          const std::string& cancel_text = "Cancel");

        /**
         * @brief Render the dialog (centered on screen)
         */
        void Render() const override;

        /**
         * @brief Update dialog state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time) override;

        /**
         * @brief Handle mouse event
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event);

        /**
         * @brief Set confirm callback
         */
        void SetConfirmCallback(const ConfirmCallback &callback) { confirm_callback_ = callback; }

        /**
         * @brief Set cancel callback
         */
        void SetCancelCallback(const CancelCallback &callback) { cancel_callback_ = callback; }

        /**
         * @brief Check if dialog is visible
         */
        bool IsVisible() const { return is_visible_; }

        /**
         * @brief Show the dialog
         */
        void Show() { is_visible_ = true; }

        /**
         * @brief Hide the dialog
         */
        void Hide() { is_visible_ = false; }

    private:
        std::string title_;
        std::string message_;
        std::unique_ptr<Button> confirm_button_;
        std::unique_ptr<Button> cancel_button_;
        ConfirmCallback confirm_callback_;
        CancelCallback cancel_callback_;
        bool is_visible_;
        float animation_time_;
        
        static constexpr int DIALOG_WIDTH = 400;
        static constexpr int DIALOG_HEIGHT = 200;
        static constexpr int BUTTON_WIDTH = 120;
        static constexpr int BUTTON_HEIGHT = 40;
    };

    /**
     * @brief Slider class - interactive horizontal slider for value adjustment
     * 
     * Provides a draggable slider for adjusting numeric values within a range.
     * Supports mouse drag and keyboard adjustment.
     * Implements Observer pattern via value change callback.
     */
    class Slider : public UIElement {
    public:
        /**
         * @brief Callback type for value changes
         */
        using ValueChangedCallback = std::function<void(float)>;

        /**
         * @brief Construct a slider
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the slider
         * @param height Height of the slider (label + bar)
         * @param min_value Minimum value
         * @param max_value Maximum value
         * @param label Label text displayed above slider
         */
        Slider(float relative_x, float relative_y, float width, float height,
               float min_value, float max_value, const std::string& label = "");

        /**
         * @brief Render the slider
         */
        void Render() const override;

        /**
         * @brief Update slider state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Handle hover event
         */
        bool OnHover(const MouseEvent& event) override;

        /**
         * @brief Handle click/drag event
         */
        bool OnClick(const MouseEvent& event) override;

        /**
         * @brief Set value change callback
         */
        void SetValueChangedCallback(const ValueChangedCallback &callback) { value_changed_callback_ = callback; }

        /**
         * @brief Set current value (normalized 0.0-1.0)
         */
        void SetValue(float value);

        /**
         * @brief Get current value (normalized 0.0-1.0)
         */
        float GetValue() const { return value_; }

        /**
         * @brief Set label text
         */
        void SetLabel(const std::string& label) { label_ = label; }

        /**
         * @brief Get label text
         */
        const std::string& GetLabel() const { return label_; }

        /**
         * @brief Handle keyboard input for value adjustment
         * @return true if input was handled
         */
        bool HandleKeyboard();

    private:
        std::string label_;
        float min_value_;
        float max_value_;
        float value_;               // Normalized value (0.0 - 1.0)
        bool is_dragging_;
        ValueChangedCallback value_changed_callback_;

        static constexpr int LABEL_HEIGHT = 20;
        static constexpr int BAR_HEIGHT = 10;
        static constexpr int THUMB_RADIUS = 8;
    };

    /**
     * @brief Checkbox class - interactive checkbox for boolean values
     * 
     * Provides a clickable checkbox with label for toggling boolean states.
     * Supports keyboard toggle.
     * Implements Observer pattern via toggle callback.
     */
    class Checkbox : public UIElement {
    public:
        /**
         * @brief Callback type for toggle events
         */
        using ToggleCallback = std::function<void(bool)>;

        /**
         * @brief Construct a checkbox
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param label Label text displayed next to checkbox
         */
        Checkbox(float relative_x, float relative_y, const std::string& label = "");

        /**
         * @brief Render the checkbox
         */
        void Render() const override;

        /**
         * @brief Handle click event
         */
        bool OnClick(const MouseEvent& event) override;

        /**
         * @brief Set toggle callback
         */
        void SetToggleCallback(const ToggleCallback &callback) { toggle_callback_ = callback; }

        /**
         * @brief Set checked state
         */
        void SetChecked(bool checked);

        /**
         * @brief Get checked state
         */
        bool IsChecked() const { return checked_; }

        /**
         * @brief Set label text
         */
        void SetLabel(const std::string& label) { label_ = label; }

        /**
         * @brief Get label text
         */
        const std::string& GetLabel() const { return label_; }

        /**
         * @brief Handle keyboard input for toggle
         * @return true if input was handled
         */
        bool HandleKeyboard();

    private:
        std::string label_;
        bool checked_;
        ToggleCallback toggle_callback_;

        static constexpr int BOX_SIZE = 20;
        static constexpr int LABEL_SPACING = 10;
        static constexpr int HEIGHT = 30;
    };

    /**
     * @brief Label class - text display element
     * 
     * Provides a simple text label with configurable font size, color, and alignment.
     * Useful as a building block for more complex components.
     */
    class Label : public UIElement {
    public:
        /**
         * @brief Text alignment options
         */
        enum class Alignment {
            Left,
            Center,
            Right
        };

        /**
         * @brief Construct a label
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param text Label text
         * @param font_size Font size in pixels
         * @param color Text color
         * @param alignment Text alignment (default: Left)
         */
        Label(float relative_x, float relative_y, const std::string& text,
              int font_size = 14, Color color = LIGHTGRAY,
              Alignment alignment = Alignment::Left);

        /**
         * @brief Render the label
         */
        void Render() const override;

        /**
         * @brief Set label text (reactive update)
         */
        void SetText(const std::string& text);

        /**
         * @brief Get label text
         */
        const std::string& GetText() const { return text_; }

        /**
         * @brief Set text color (reactive update)
         */
        void SetColor(Color color) { color_ = color; }

        /**
         * @brief Get text color
         */
        Color GetColor() const { return color_; }

        /**
         * @brief Set font size (reactive update)
         */
        void SetFontSize(int size) { font_size_ = size; }

        /**
         * @brief Get font size
         */
        int GetFontSize() const { return font_size_; }

        /**
         * @brief Set text alignment (reactive update)
         */
        void SetAlignment(Alignment alignment) { alignment_ = alignment; }

        /**
         * @brief Get text alignment
         */
        Alignment GetAlignment() const { return alignment_; }

    private:
        std::string text_;
        int font_size_;
        Color color_;
        Alignment alignment_;
    };

}
