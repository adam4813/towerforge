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
      static void Initialize(int width, int height, const char *title);

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
   };
}
