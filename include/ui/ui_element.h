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
        void SetParent(UIElement *parent);

        /**
         * @brief Get the parent element
         * @return Pointer to parent element (nullptr if no parent)
         */
        UIElement *GetParent() const { return parent_; }

        /**
         * @brief Add a child element
         * @param child Unique pointer to child element
         */
        void AddChild(std::unique_ptr<UIElement> child);

        /**
         * @brief Remove a child element
         * @param child Pointer to child element to remove
         */
        void RemoveChild(UIElement *child);

        /**
         * @brief Get all children
         * @return Reference to vector of child elements
         */
        const std::vector<std::unique_ptr<UIElement> > &GetChildren() const { return children_; }

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
        virtual void Render() const {
        }

        /**
         * @brief Process mouse events with bubble-down propagation
         *
         * This method handles mouse events by first propagating them to children
         * (bubble-down), then calling the element's own event handlers if not consumed.
         *
         * @param event Mouse event data
         * @return true if the event was handled and should not propagate further
         */
        virtual bool ProcessMouseEvent(const MouseEvent &event);

        /**
         * @brief Handle mouse hover event (override in derived classes)
         *
         * @param event Mouse event data
         * @return true if the event was consumed
         */
        bool OnHover(const MouseEvent &event) override { return false; }

        /**
         * @brief Handle mouse click event (override in derived classes)
         *
         * @param event Mouse event data
         * @return true if the event was consumed
         */
        bool OnClick(const MouseEvent &event) override { return false; }

    protected:
        float relative_x_; // X position relative to parent
        float relative_y_; // Y position relative to parent
        float width_; // Width of the element
        float height_; // Height of the element
        bool is_focused_; // Whether element has keyboard focus

        UIElement *parent_; // Parent element (nullptr for root elements)
        std::vector<std::unique_ptr<UIElement> > children_; // Child elements
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
        float animation_progress_; // 0.0 = fully hidden, 1.0 = fully visible
        float animation_speed_;
    };
}
