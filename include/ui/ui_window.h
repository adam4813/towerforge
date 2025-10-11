#pragma once

#include <raylib.h>
#include <string>
#include <memory>

namespace towerforge {
namespace ui {

/**
 * @brief Base class for all UI windows
 */
class UIWindow {
public:
    UIWindow(const std::string& title, int width, int height);
    virtual ~UIWindow() = default;
    
    /**
     * @brief Render the window
     */
    virtual void Render() = 0;
    
    /**
     * @brief Check if a point is inside the window
     */
    bool Contains(int x, int y) const;
    
    /**
     * @brief Check if the close button was clicked
     */
    bool IsCloseButtonClicked(int mouse_x, int mouse_y) const;
    
    /**
     * @brief Get window bounds
     */
    Rectangle GetBounds() const;
    
    /**
     * @brief Set window position
     */
    void SetPosition(int x, int y);
    
    /**
     * @brief Get z-order (higher values are rendered on top)
     */
    int GetZOrder() const { return z_order_; }
    
    /**
     * @brief Set z-order
     */
    void SetZOrder(int z_order) { z_order_ = z_order; }
    
    /**
     * @brief Get window ID
     */
    int GetId() const { return id_; }
    
protected:
    void RenderFrame(Color border_color);
    void RenderCloseButton();
    
    int id_;
    std::string title_;
    int x_;
    int y_;
    int width_;
    int height_;
    int z_order_;
    
    static constexpr int TITLE_BAR_HEIGHT = 25;
    static constexpr int CLOSE_BUTTON_SIZE = 15;
    static constexpr int PADDING = 10;
    
private:
    static int next_id_;
};

} // namespace ui
} // namespace towerforge
