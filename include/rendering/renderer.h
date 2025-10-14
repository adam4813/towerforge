#pragma once

#include <raylib.h>

namespace towerforge::rendering {

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
        static void Initialize(int width, int height, const char* title);
    
        /**
     * @brief Shutdown the renderer and close the window
     */
        static void Shutdown();
    
        /**
     * @brief Check if the window should close
     * @return true if the window should close, false otherwise
     */
        static bool ShouldClose();
    
        /**
     * @brief Begin drawing frame
     */
        static void BeginFrame();
    
        /**
     * @brief End drawing frame
     */
        static void EndFrame();
    
        /**
     * @brief Clear the screen with a color
     * @param color Color to clear the screen with
     */
        static void Clear(Color color);
    
        /**
     * @brief Draw a rectangle
     * @param x X position
     * @param y Y position
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Rectangle color
     */
        static void DrawRectangle(int x, int y, int width, int height, Color color);
    
        /**
     * @brief Draw a circle
     * @param centerX Circle center X position
     * @param centerY Circle center Y position
     * @param radius Circle radius
     * @param color Circle color
     */
        static void DrawCircle(int centerX, int centerY, float radius, Color color);
    
        /**
     * @brief Draw text on the screen
     * @param text Text to draw
     * @param x X position
     * @param y Y position
     * @param fontSize Font size
     * @param color Text color
     */
        static void DrawText(const char* text, int x, int y, int fontSize, Color color);
    };

}
