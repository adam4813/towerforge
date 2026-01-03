#pragma once

namespace towerforge::ui {
    /**
     * @brief Mouse event data structure
     * 
     * Contains information about a mouse event (position, button state, etc.)
     */
    struct MouseEvent {
        float x; // Mouse X position in screen coordinates
        float y; // Mouse Y position in screen coordinates
        bool left_down; // Left mouse button is down
        bool right_down; // Right mouse button is down
        bool left_pressed; // Left mouse button was just pressed this frame
        bool right_pressed; // Right mouse button was just pressed this frame

        MouseEvent(const float x = 0.0f, const float y = 0.0f,
                   const bool left_down = false, const bool right_down = false,
                   const bool left_pressed = false, const bool right_pressed = false)
            : x(x), y(y), left_down(left_down), right_down(right_down),
              left_pressed(left_pressed), right_pressed(right_pressed) {
        }
    };

    /**
     * @brief Interface for objects that can receive mouse events
     * 
     * This interface provides a unified way to handle mouse interactions
     * across UI elements and game objects. Events use a bubble-down approach
     * where they propagate to children and can be marked as consumed.
     */
    class IMouseInteractive {
    public:
        virtual ~IMouseInteractive() = default;

        /**
         * @brief Handle mouse hover event
         * 
         * Called when the mouse is over the element or its bounds.
         * Events bubble down to children first.
         * 
         * @param event Mouse event data
         * @return true if the event was consumed (stops propagation to siblings), false otherwise
         */
        virtual bool OnHover(const MouseEvent &event) { return false; }

        /**
         * @brief Handle mouse click event
         * 
         * Called when a mouse button is clicked while over the element.
         * Events bubble down to children first.
         * 
         * @param event Mouse event data
         * @return true if the event was consumed (stops propagation to siblings), false otherwise
         */
        virtual bool OnClick(const MouseEvent &event) { return false; }

        /**
         * @brief Check if the element contains a point
         * 
         * Used for hit testing to determine if mouse events should be sent to this element.
         * 
         * @param x Screen X coordinate
         * @param y Screen Y coordinate
         * @return true if the point is inside the element's bounds
         */
        virtual bool Contains(float x, float y) const = 0;

        /**
         * @brief Get whether the element is currently being hovered
         * 
         * @return true if mouse is currently over the element
         */
        virtual bool IsHovered() const { return is_hovered_; }

    protected:
        bool is_hovered_ = false; // Tracks current hover state
    };
}
