#include "ui/minimap.h"
#include "rendering/camera.h"
#include <algorithm>

namespace towerforge::ui {

    Minimap::Minimap()
        : screen_width_(800)
          , screen_height_(600)
          , tower_width_(1000.0f)
          , tower_height_(1000.0f)
          , x_(0)
          , y_(0)
          , width_(DEFAULT_WIDTH)
          , height_(DEFAULT_HEIGHT)
          , visible_(true)
          , dragging_viewport_(false)
          , drag_start_mouse_{0.0f, 0.0f}
          , drag_start_camera_{0.0f, 0.0f} {
    }

    Minimap::~Minimap() = default;

    void Minimap::Initialize(const int screen_width, const int screen_height,
                              const float tower_width, const float tower_height) {
        screen_width_ = screen_width;
        screen_height_ = screen_height;
        tower_width_ = tower_width;
        tower_height_ = tower_height;

        // Position in top-right corner by default
        x_ = screen_width - width_ - PADDING;
        y_ = PADDING;
    }

    void Minimap::Update(const float delta_time) {
        // Update logic if needed (e.g., animations)
    }

    void Minimap::Render(const rendering::Camera& camera) {
        if (!visible_) {
            return;
        }

        // Draw background
        DrawRectangle(x_, y_, width_, height_, ColorAlpha(BLACK, BACKGROUND_ALPHA));

        // Draw border
        DrawRectangleLinesEx({static_cast<float>(x_), static_cast<float>(y_),
                              static_cast<float>(width_), static_cast<float>(height_)},
                             BORDER_THICKNESS, DARKGRAY);

        // Draw tower outline (simplified representation)
        const float scale_x = static_cast<float>(width_) / tower_width_;
        const float scale_y = static_cast<float>(height_) / tower_height_;
        const float scale = std::min(scale_x, scale_y);

        const int tower_render_width = static_cast<int>(tower_width_ * scale);
        const int tower_render_height = static_cast<int>(tower_height_ * scale);

        // Center the tower representation in the minimap
        const int tower_x = x_ + (width_ - tower_render_width) / 2;
        const int tower_y = y_ + (height_ - tower_render_height) / 2;

        // Draw tower bounds
        DrawRectangleLines(tower_x, tower_y, tower_render_width, tower_render_height, GRAY);

        // Calculate and draw viewport indicator
        const Rectangle viewport_rect = CalculateViewportRect(camera);

        // Draw viewport indicator with transparency
        DrawRectangleRec(viewport_rect, ColorAlpha(YELLOW, 0.3f));
        DrawRectangleLinesEx(viewport_rect, 2.0f, YELLOW);

        // Draw title
        DrawText("MINIMAP", x_ + 5, y_ + 5, 10, LIGHTGRAY);

        // Optional: Draw 'M' hint to toggle
        const char* toggle_hint = "[M]";
        const int hint_width = MeasureText(toggle_hint, 10);
        DrawText(toggle_hint, x_ + width_ - hint_width - 5, y_ + 5, 10, DARKGRAY);
    }

    bool Minimap::HandleInput(rendering::Camera& camera) {
        if (!visible_) {
            return false;
        }

        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();

        // Check for dragging viewport indicator
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (IsMouseOverViewport(mouse_x, mouse_y, camera)) {
                dragging_viewport_ = true;
                drag_start_mouse_ = {static_cast<float>(mouse_x), static_cast<float>(mouse_y)};
                camera.GetPosition(drag_start_camera_.x, drag_start_camera_.y);
                return true;
            } else if (IsMouseOver(mouse_x, mouse_y)) {
                // Click on minimap but not on viewport - jump camera to that location
                float world_x, world_y;
                MinimapToWorld(mouse_x, mouse_y, world_x, world_y);

                // Set camera to center on clicked location
                camera.SetTargetPosition(world_x, world_y);

                return true;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragging_viewport_ = false;
        }

        if (dragging_viewport_) {
            // Calculate delta in minimap space
            const float delta_x = static_cast<float>(mouse_x) - drag_start_mouse_.x;
            const float delta_y = static_cast<float>(mouse_y) - drag_start_mouse_.y;

            // Convert delta to world space
            const float scale_x = static_cast<float>(width_) / tower_width_;
            const float scale_y = static_cast<float>(height_) / tower_height_;
            const float scale = std::min(scale_x, scale_y);

            const float world_delta_x = delta_x / scale;
            const float world_delta_y = delta_y / scale;

            // Update camera position
            camera.SetTargetPosition(drag_start_camera_.x + world_delta_x,
                                      drag_start_camera_.y + world_delta_y);

            return true;
        }

        return false;
    }

    void Minimap::Toggle() {
        visible_ = !visible_;
    }

    void Minimap::SetVisible(const bool visible) {
        visible_ = visible;
    }

    void Minimap::SetPosition(const int x, const int y) {
        x_ = x;
        y_ = y;
    }

    void Minimap::SetSize(const int width, const int height) {
        width_ = width;
        height_ = height;
    }

    void Minimap::SetTowerBounds(const float tower_width, const float tower_height) {
        tower_width_ = tower_width;
        tower_height_ = tower_height;
    }

    bool Minimap::IsMouseOver(const int mouse_x, const int mouse_y) const {
        return mouse_x >= x_ && mouse_x <= x_ + width_ &&
               mouse_y >= y_ && mouse_y <= y_ + height_;
    }

    bool Minimap::IsMouseOverViewport(const int mouse_x, const int mouse_y,
                                       const rendering::Camera& camera) const {
        const Rectangle viewport_rect = CalculateViewportRect(camera);
        return CheckCollisionPointRec({static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, viewport_rect);
    }

    void Minimap::MinimapToWorld(const int minimap_x, const int minimap_y,
                                  float& world_x, float& world_y) const {
        // Calculate scale
        const float scale_x = static_cast<float>(width_) / tower_width_;
        const float scale_y = static_cast<float>(height_) / tower_height_;
        const float scale = std::min(scale_x, scale_y);

        const int tower_render_width = static_cast<int>(tower_width_ * scale);
        const int tower_render_height = static_cast<int>(tower_height_ * scale);

        // Calculate tower position in minimap
        const int tower_x = x_ + (width_ - tower_render_width) / 2;
        const int tower_y = y_ + (height_ - tower_render_height) / 2;

        // Convert minimap coordinates to world coordinates
        const int relative_x = minimap_x - tower_x;
        const int relative_y = minimap_y - tower_y;

        world_x = static_cast<float>(relative_x) / scale;
        world_y = static_cast<float>(relative_y) / scale;
    }

    void Minimap::WorldToMinimap(const float world_x, const float world_y,
                                  int& minimap_x, int& minimap_y) const {
        // Calculate scale
        const float scale_x = static_cast<float>(width_) / tower_width_;
        const float scale_y = static_cast<float>(height_) / tower_height_;
        const float scale = std::min(scale_x, scale_y);

        const int tower_render_width = static_cast<int>(tower_width_ * scale);
        const int tower_render_height = static_cast<int>(tower_height_ * scale);

        // Calculate tower position in minimap
        const int tower_x = x_ + (width_ - tower_render_width) / 2;
        const int tower_y = y_ + (height_ - tower_render_height) / 2;

        // Convert world coordinates to minimap coordinates
        minimap_x = tower_x + static_cast<int>(world_x * scale);
        minimap_y = tower_y + static_cast<int>(world_y * scale);
    }

    Rectangle Minimap::CalculateViewportRect(const rendering::Camera& camera) const {
        // Get camera zoom and calculate visible area
        const float zoom = camera.GetZoom();

        // Get camera position
        float camera_x, camera_y;
        camera.GetPosition(camera_x, camera_y);

        // Get screen dimensions
        int screen_width, screen_height;
        camera.GetScreenDimensions(screen_width, screen_height);

        // Calculate visible dimensions in world units
        const float visible_width = static_cast<float>(screen_width) / zoom;
        const float visible_height = static_cast<float>(screen_height) / zoom;

        // Calculate camera's top-left position in world space
        const float camera_top_left_x = camera_x - visible_width / 2.0f;
        const float camera_top_left_y = camera_y - visible_height / 2.0f;

        // Calculate scale
        const float scale_x = static_cast<float>(width_) / tower_width_;
        const float scale_y = static_cast<float>(height_) / tower_height_;
        const float scale = std::min(scale_x, scale_y);

        const int tower_render_width = static_cast<int>(tower_width_ * scale);
        const int tower_render_height = static_cast<int>(tower_height_ * scale);
        const int tower_x = x_ + (width_ - tower_render_width) / 2;
        const int tower_y = y_ + (height_ - tower_render_height) / 2;

        // Calculate viewport dimensions in minimap space
        const float viewport_width = visible_width * scale;
        const float viewport_height = visible_height * scale;

        // Calculate viewport position in minimap space
        const float viewport_x = static_cast<float>(tower_x) + camera_top_left_x * scale;
        const float viewport_y = static_cast<float>(tower_y) + camera_top_left_y * scale;

        return {viewport_x, viewport_y, viewport_width, viewport_height};
    }

}
