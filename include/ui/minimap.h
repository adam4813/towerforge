#pragma once

#include <raylib.h>

namespace towerforge::rendering {
    class Camera;
}

namespace towerforge::ui {

    /**
     * @brief Minimap widget for displaying an overview of the entire tower
     * 
     * Displays a scaled representation of the full tower and allows navigation
     * by dragging a viewport indicator or clicking to jump to a location.
     */
    class Minimap {
    public:
        Minimap();
        ~Minimap();

        /**
         * @brief Initialize the minimap
         * @param screen_width Screen width in pixels
         * @param screen_height Screen height in pixels
         * @param tower_width Tower width in world units
         * @param tower_height Tower height in world units
         */
        void Initialize(int screen_width, int screen_height, float tower_width, float tower_height);

        /**
         * @brief Update minimap state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Render the minimap
         * @param camera Reference to the camera for position and zoom
         */
        void Render(const rendering::Camera& camera);

        /**
         * @brief Handle mouse input for minimap interaction
         * @param camera Reference to the camera to update
         * @return true if minimap consumed the input
         */
        bool HandleInput(rendering::Camera& camera);

        /**
         * @brief Toggle minimap visibility
         */
        void Toggle();

        /**
         * @brief Set minimap visibility
         * @param visible Visibility state
         */
        void SetVisible(bool visible);

        /**
         * @brief Check if minimap is visible
         * @return true if visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Set minimap position
         * @param x X position in screen coordinates
         * @param y Y position in screen coordinates
         */
        void SetPosition(int x, int y);

        /**
         * @brief Set minimap size
         * @param width Width in pixels
         * @param height Height in pixels
         */
        void SetSize(int width, int height);

        /**
         * @brief Update tower dimensions
         * @param tower_width Tower width in world units
         * @param tower_height Tower height in world units
         */
        void SetTowerBounds(float tower_width, float tower_height);

    private:
        /**
         * @brief Check if mouse is over minimap area
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @return true if mouse is over minimap
         */
        bool IsMouseOver(int mouse_x, int mouse_y) const;

        /**
         * @brief Check if mouse is over viewport indicator
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param camera Reference to camera for viewport calculation
         * @return true if mouse is over viewport indicator
         */
        bool IsMouseOverViewport(int mouse_x, int mouse_y, const rendering::Camera& camera) const;

        /**
         * @brief Convert minimap coordinates to world coordinates
         * @param minimap_x Minimap X position
         * @param minimap_y Minimap Y position
         * @param world_x Output world X position
         * @param world_y Output world Y position
         */
        void MinimapToWorld(int minimap_x, int minimap_y, float& world_x, float& world_y) const;

        /**
         * @brief Convert world coordinates to minimap coordinates
         * @param world_x World X position
         * @param world_y World Y position
         * @param minimap_x Output minimap X position
         * @param minimap_y Output minimap Y position
         */
        void WorldToMinimap(float world_x, float world_y, int& minimap_x, int& minimap_y) const;

        /**
         * @brief Calculate viewport indicator rectangle
         * @param camera Reference to camera
         * @return Rectangle representing viewport in minimap coordinates
         */
        Rectangle CalculateViewportRect(const rendering::Camera& camera) const;

        // Screen dimensions
        int screen_width_;
        int screen_height_;

        // Tower dimensions
        float tower_width_;
        float tower_height_;

        // Minimap position and size
        int x_;
        int y_;
        int width_;
        int height_;

        // Visibility
        bool visible_;

        // Interaction state
        bool dragging_viewport_;
        Vector2 drag_start_mouse_;
        Vector2 drag_start_camera_;

        // Style constants
        static constexpr int DEFAULT_WIDTH = 200;
        static constexpr int DEFAULT_HEIGHT = 150;
        static constexpr int PADDING = 10;
        static constexpr float BACKGROUND_ALPHA = 0.85f;
        static constexpr float BORDER_THICKNESS = 2.0f;
    };

}
