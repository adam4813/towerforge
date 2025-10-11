#pragma once

#include <raylib.h>
#include <string>
#include <functional>
#include <memory>

namespace towerforge {
namespace ui {

/**
 * @brief Tooltip system for providing contextual information
 * 
 * Provides hover and keyboard-accessible tooltips across all UI elements.
 * Supports dynamic content based on game state.
 */
class Tooltip {
public:
    /**
     * @brief Create a tooltip with static text
     * @param text The tooltip text
     */
    explicit Tooltip(const std::string& text);
    
    /**
     * @brief Create a tooltip with dynamic text
     * @param text_generator Function that generates tooltip text based on current state
     */
    explicit Tooltip(std::function<std::string()> text_generator);
    
    /**
     * @brief Get the current tooltip text
     */
    std::string GetText() const;
    
    /**
     * @brief Check if tooltip should be shown
     */
    bool ShouldShow() const;
    
    /**
     * @brief Set visibility
     */
    void SetVisible(bool visible) { visible_ = visible; }
    
private:
    std::string static_text_;
    std::function<std::string()> text_generator_;
    bool is_dynamic_;
    bool visible_;
};

/**
 * @brief Tooltip manager for handling tooltip display and keyboard navigation
 */
class TooltipManager {
public:
    TooltipManager();
    ~TooltipManager();
    
    /**
     * @brief Update tooltip state based on mouse position
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     */
    void Update(int mouse_x, int mouse_y);
    
    /**
     * @brief Render active tooltip
     */
    void Render();
    
    /**
     * @brief Show tooltip at specific position
     * @param tooltip Tooltip to show
     * @param x X position
     * @param y Y position
     * @param width Width of the element (for positioning)
     * @param height Height of the element (for positioning)
     */
    void ShowTooltip(const Tooltip& tooltip, int x, int y, int width, int height);
    
    /**
     * @brief Hide current tooltip
     */
    void HideTooltip();
    
    /**
     * @brief Check if mouse is hovering over a region
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param x Region X position
     * @param y Region Y position
     * @param width Region width
     * @param height Region height
     * @return true if hovering
     */
    bool IsHovering(int mouse_x, int mouse_y, int x, int y, int width, int height) const;
    
    /**
     * @brief Set keyboard focus for tooltip navigation
     * @param element_id ID of focused element
     */
    void SetKeyboardFocus(int element_id) { keyboard_focus_id_ = element_id; }
    
    /**
     * @brief Get current keyboard focus
     */
    int GetKeyboardFocus() const { return keyboard_focus_id_; }
    
    /**
     * @brief Clear keyboard focus
     */
    void ClearKeyboardFocus() { keyboard_focus_id_ = -1; }
    
private:
    void RenderTooltipBox(const std::string& text, int x, int y);
    
    std::unique_ptr<Tooltip> current_tooltip_;
    int tooltip_x_;
    int tooltip_y_;
    int element_width_;
    int element_height_;
    float hover_time_;
    bool is_visible_;
    int keyboard_focus_id_;
    
    static constexpr float HOVER_DELAY = 0.5f;  // Delay before showing tooltip (seconds)
    static constexpr int TOOLTIP_PADDING = 8;
    static constexpr int TOOLTIP_FONT_SIZE = 12;
    static constexpr int TOOLTIP_MAX_WIDTH = 300;
    static constexpr int TOOLTIP_OFFSET_Y = 10;  // Offset below element
};

} // namespace ui
} // namespace towerforge
