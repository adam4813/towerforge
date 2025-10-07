#pragma once

#include <raylib.h>

namespace towerforge {
namespace rendering {

/**
 * @brief Renderer class for managing 2D vector graphics rendering
 * 
 * This class provides a modular interface for rendering operations,
 * designed to be easily integrated with ECS systems for GUI, actors,
 * and building visuals.
 */
class Renderer {
public:
    /**
     * @brief Initialize the renderer and create a window
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title
     */
    void Initialize(int width, int height, const char* title);
    
    /**
     * @brief Shutdown the renderer and close the window
     */
    void Shutdown();
    
    /**
     * @brief Check if the window should close
     * @return true if the window should close, false otherwise
     */
    bool ShouldClose() const;
    
    /**
     * @brief Begin drawing frame
     */
    void BeginFrame();
    
    /**
     * @brief End drawing frame
     */
    void EndFrame();
    
    /**
     * @brief Clear the screen with a color
     * @param color Color to clear the screen with
     */
    void Clear(Color color);
    
    /**
     * @brief Draw a rectangle
     * @param x X position
     * @param y Y position
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Rectangle color
     */
    void DrawRectangle(int x, int y, int width, int height, Color color);
    
    /**
     * @brief Draw a circle
     * @param centerX Circle center X position
     * @param centerY Circle center Y position
     * @param radius Circle radius
     * @param color Circle color
     */
    void DrawCircle(int centerX, int centerY, float radius, Color color);
};

} // namespace rendering
} // namespace towerforge
