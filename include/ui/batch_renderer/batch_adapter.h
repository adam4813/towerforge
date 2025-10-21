#pragma once

#include "batch_renderer.h"
#include <raylib.h>

namespace towerforge::ui::batch_renderer {

    /**
     * @brief Adapter for easy migration from direct Raylib calls
     * 
     * This adapter provides drop-in replacements for common Raylib
     * draw functions that route through the batch renderer.
     * 
     * Migration example:
     * 
     * OLD:
     *   DrawRectangle(x, y, w, h, color);
     *   DrawText("Hello", x, y, 20, WHITE);
     * 
     * NEW (Option 1 - Direct):
     *   BatchRenderer::SubmitQuad({x, y, w, h}, color);
     *   BatchRenderer::SubmitText("Hello", x, y, 20, WHITE);
     * 
     * NEW (Option 2 - Adapter):
     *   using namespace towerforge::ui::batch_renderer::adapter;
     *   DrawRectangle(x, y, w, h, color);  // Automatically batched
     *   DrawText("Hello", x, y, 20, WHITE); // Automatically batched
     * 
     * To use adapter globally:
     *   #include "ui/batch_renderer/batch_adapter.h"
     *   using namespace towerforge::ui::batch_renderer::adapter;
     */
    namespace adapter {
        
        /**
         * @brief Batched version of DrawRectangle
         */
        inline void DrawRectangle(int x, int y, int width, int height, Color color) {
            BatchRenderer::SubmitQuad(
                Rectangle{static_cast<float>(x), static_cast<float>(y), 
                         static_cast<float>(width), static_cast<float>(height)},
                color
            );
        }
        
        /**
         * @brief Batched version of DrawRectangleRec
         */
        inline void DrawRectangleRec(Rectangle rect, Color color) {
            BatchRenderer::SubmitQuad(rect, color);
        }
        
        /**
         * @brief Batched version of DrawRectangleLines
         * 
         * Note: Draws 4 lines to form rectangle outline.
         * Thickness is fixed at 1 pixel.
         */
        inline void DrawRectangleLines(int x, int y, int width, int height, Color color) {
            const float fx = static_cast<float>(x);
            const float fy = static_cast<float>(y);
            const float fw = static_cast<float>(width);
            const float fh = static_cast<float>(height);
            
            // Top
            BatchRenderer::SubmitLine(fx, fy, fx + fw, fy, 1.0f, color);
            // Right
            BatchRenderer::SubmitLine(fx + fw, fy, fx + fw, fy + fh, 1.0f, color);
            // Bottom
            BatchRenderer::SubmitLine(fx + fw, fy + fh, fx, fy + fh, 1.0f, color);
            // Left
            BatchRenderer::SubmitLine(fx, fy + fh, fx, fy, 1.0f, color);
        }
        
        /**
         * @brief Batched version of DrawRectangleLinesEx
         */
        inline void DrawRectangleLinesEx(Rectangle rect, float thickness, Color color) {
            // Top
            BatchRenderer::SubmitLine(rect.x, rect.y, 
                                     rect.x + rect.width, rect.y, 
                                     thickness, color);
            // Right
            BatchRenderer::SubmitLine(rect.x + rect.width, rect.y, 
                                     rect.x + rect.width, rect.y + rect.height, 
                                     thickness, color);
            // Bottom
            BatchRenderer::SubmitLine(rect.x + rect.width, rect.y + rect.height, 
                                     rect.x, rect.y + rect.height, 
                                     thickness, color);
            // Left
            BatchRenderer::SubmitLine(rect.x, rect.y + rect.height, 
                                     rect.x, rect.y, 
                                     thickness, color);
        }
        
        /**
         * @brief Batched version of DrawCircle
         */
        inline void DrawCircle(int center_x, int center_y, float radius, Color color) {
            BatchRenderer::SubmitCircle(
                static_cast<float>(center_x),
                static_cast<float>(center_y),
                radius,
                color
            );
        }
        
        /**
         * @brief Batched version of DrawCircleV
         */
        inline void DrawCircleV(Vector2 center, float radius, Color color) {
            BatchRenderer::SubmitCircle(center.x, center.y, radius, color);
        }
        
        /**
         * @brief Batched version of DrawText
         */
        inline void DrawText(const char* text, int x, int y, int font_size, Color color) {
            BatchRenderer::SubmitText(
                std::string(text),
                static_cast<float>(x),
                static_cast<float>(y),
                font_size,
                color
            );
        }
        
        /**
         * @brief Batched version of DrawLine
         */
        inline void DrawLine(int start_x, int start_y, int end_x, int end_y, Color color) {
            BatchRenderer::SubmitLine(
                static_cast<float>(start_x),
                static_cast<float>(start_y),
                static_cast<float>(end_x),
                static_cast<float>(end_y),
                1.0f,
                color
            );
        }
        
        /**
         * @brief Batched version of DrawLineEx
         */
        inline void DrawLineEx(Vector2 start, Vector2 end, float thickness, Color color) {
            BatchRenderer::SubmitLine(start.x, start.y, end.x, end.y, thickness, color);
        }
        
        /**
         * @brief Batched version of DrawRectangleRounded
         */
        inline void DrawRectangleRounded(Rectangle rect, float roundness, 
                                        int segments, Color color) {
            const float corner_radius = (rect.width < rect.height ? rect.width : rect.height) 
                                       * roundness * 0.5f;
            BatchRenderer::SubmitRoundedRect(rect, corner_radius, color, segments);
        }
        
    } // namespace adapter

} // namespace towerforge::ui::batch_renderer
