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
        Rectangle GetAbsoluteBounds() const;

        /**
         * @brief Get the relative bounding rectangle (relative to parent)
         * @return Relative rectangle
         */
        Rectangle GetRelativeBounds() const;

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
        bool ProcessMouseEvent(const MouseEvent& event);

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

        UIElement* parent_;     // Parent element (nullptr for root elements)
        std::vector<std::unique_ptr<UIElement>> children_;  // Child elements
    };

    /**
     * @brief Panel class - a basic rectangular container for UI elements
     * 
     * Represents a panel that can contain other UI elements and provides
     * basic rendering with background and optional border.
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
         * @brief Render the panel
         */
        void Render() const override;

        /**
         * @brief Set background color
         */
        void SetBackgroundColor(Color color) { background_color_ = color; }

        /**
         * @brief Set border color
         */
        void SetBorderColor(Color color) { border_color_ = color; }

        /**
         * @brief Get background color
         */
        Color GetBackgroundColor() const { return background_color_; }

        /**
         * @brief Get border color
         */
        Color GetBorderColor() const { return border_color_; }

    private:
        Color background_color_;
        Color border_color_;
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
        void SetClickCallback(ClickCallback callback) { click_callback_ = callback; }

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
        void SetBackgroundColor(Color color) { background_color_ = color; }

        /**
         * @brief Set border color
         */
        void SetBorderColor(Color color) { border_color_ = color; }

        /**
         * @brief Set text color
         */
        void SetTextColor(Color color) { text_color_ = color; }

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
        void SetFontSize(int size) { font_size_ = size; }

        /**
         * @brief Get font size
         */
        int GetFontSize() const { return font_size_; }

        /**
         * @brief Set enabled state
         */
        void SetEnabled(bool enabled) { enabled_ = enabled; }

        /**
         * @brief Get enabled state
         */
        bool IsEnabled() const { return enabled_; }

    private:
        std::string label_;
        Color background_color_;
        Color border_color_;
        Color text_color_;
        int font_size_;
        bool enabled_;
        ClickCallback click_callback_;
    };

}
