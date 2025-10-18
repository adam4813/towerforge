#pragma once

#include "mouse_interface.h"
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

namespace towerforge::ui {

    /**
     * @brief Callback type for mouse hover events
     * 
     * @param event Mouse event data
     * @return true if event was consumed, false otherwise
     */
    using MouseHoverCallback = std::function<bool(const MouseEvent& event)>;

    /**
     * @brief Callback type for mouse click events
     * 
     * @param event Mouse event data
     * @return true if event was consumed, false otherwise
     */
    using MouseClickCallback = std::function<bool(const MouseEvent& event)>;

    /**
     * @brief Represents a rectangular area that can receive mouse events
     */
    struct MouseEventRegion {
        Rectangle bounds;            // Screen-space bounds
        MouseHoverCallback on_hover; // Hover callback (optional)
        MouseClickCallback on_click; // Click callback (optional)
        int priority;                // Higher priority regions receive events first
        void* user_data;             // Optional user data pointer

        MouseEventRegion(Rectangle bounds, 
                        MouseHoverCallback hover = nullptr,
                        MouseClickCallback click = nullptr,
                        int priority = 0,
                        void* user_data = nullptr)
            : bounds(bounds), on_hover(hover), on_click(click), 
              priority(priority), user_data(user_data) {}
    };

    /**
     * @brief Manages mouse events for game objects and non-UI elements
     * 
     * This manager provides a subscription-based system where game objects
     * (facilities, world objects, etc.) can register for mouse event notifications.
     * It handles spatial queries to determine which objects should receive events.
     * 
     * Unlike UI elements which handle events hierarchically, the manager processes
     * regions based on priority and spatial overlap.
     */
    class MouseEventManager {
    public:
        MouseEventManager() = default;
        ~MouseEventManager() = default;

        // Prevent copying
        MouseEventManager(const MouseEventManager&) = delete;
        MouseEventManager& operator=(const MouseEventManager&) = delete;

        /**
         * @brief Register a region for mouse events
         * 
         * @param region The region to register
         * @return Handle to the registered region (for unregistration)
         */
        int RegisterRegion(const MouseEventRegion& region);

        /**
         * @brief Unregister a region by its handle
         * 
         * @param handle The handle returned from RegisterRegion
         */
        void UnregisterRegion(int handle);

        /**
         * @brief Update a region's bounds
         * 
         * Useful for moving objects or dynamic UI
         * 
         * @param handle The handle of the region to update
         * @param new_bounds The new bounds
         */
        void UpdateRegionBounds(int handle, Rectangle new_bounds);

        /**
         * @brief Process mouse events for all registered regions
         * 
         * Should be called once per frame with current mouse state.
         * Processes hover and click events in priority order.
         * 
         * @param event Current mouse event data
         */
        void ProcessMouseEvents(const MouseEvent& event);

        /**
         * @brief Clear all registered regions
         */
        void ClearAllRegions();

        /**
         * @brief Get the number of registered regions
         * 
         * @return Number of active regions
         */
        size_t GetRegionCount() const { return regions_.size(); }

    private:
        struct Region {
            int handle;
            MouseEventRegion data;
            bool is_hovered;

            Region(int h, const MouseEventRegion& d)
                : handle(h), data(d), is_hovered(false) {}
        };

        std::vector<Region> regions_;
        int next_handle_ = 1;

        /**
         * @brief Find regions that contain the given point, sorted by priority
         * 
         * @param x Screen X coordinate
         * @param y Screen Y coordinate
         * @return Vector of regions containing the point, highest priority first
         */
        std::vector<Region*> FindRegionsAtPoint(float x, float y);
    };

}
