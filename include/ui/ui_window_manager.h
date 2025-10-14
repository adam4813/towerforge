#pragma once

#include "ui/ui_window.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace towerforge::ui {

    /**
 * @brief Manages multiple UI windows with stacking and positioning
 */
    class UIWindowManager {
    public:
        UIWindowManager();
        ~UIWindowManager() = default;
    
        /**
     * @brief Add a new window to the manager
     * @return Window ID
     */
        int AddWindow(std::unique_ptr<UIWindow> window);
    
        /**
     * @brief Remove a window by ID
     */
        void RemoveWindow(int window_id);
    
        /**
     * @brief Remove all windows
     */
        void Clear();
    
        /**
     * @brief Render all windows in z-order
     */
        void Render() const;
    
        /**
     * @brief Handle click events
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @return true if click was handled by a window
     */
        bool HandleClick(int mouse_x, int mouse_y);
    
        /**
     * @brief Get window by ID
     */
        UIWindow* GetWindow(int window_id) const;
    
        /**
     * @brief Bring window to front
     */
        void BringToFront(int window_id);
    
        /**
     * @brief Get number of active windows
     */
        size_t GetWindowCount() const { return windows_.size(); }
    
    private:
        /**
     * @brief Calculate position for a new window to avoid overlap
     */
        void CalculateWindowPosition(UIWindow* window) const;
    
        /**
     * @brief Update z-orders after changes
     */
        void UpdateZOrders();
    
        std::vector<std::unique_ptr<UIWindow>> windows_;
        int next_z_order_;
    
        // Layout constants
        static constexpr int INITIAL_X_OFFSET = 10;
        static constexpr int INITIAL_Y_OFFSET = 60;
        static constexpr int WINDOW_SPACING = 20;
    };

}
