#include "rendering/camera.h"
#include <raylib.h>
#include <algorithm>
#include <cmath>

namespace towerforge::rendering {

    Camera::Camera()
        : target_position_{0.0f, 0.0f}
          , target_zoom_(1.0f)
          , current_zoom_(1.0f)
          , screen_width_(800)
          , screen_height_(600)
          , tower_width_(1000.0f)
          , tower_height_(1000.0f)
          , is_panning_(false)
          , pan_start_mouse_{0.0f, 0.0f}
          , pan_start_camera_{0.0f, 0.0f}
          , following_(false)
          , followed_entity_id_(-1)
          , followed_position_{0.0f, 0.0f} {

        camera_.target = {0.0f, 0.0f};
        camera_.offset = {0.0f, 0.0f};
        camera_.rotation = 0.0f;
        camera_.zoom = 1.0f;
    }

    Camera::~Camera() = default;

    void Camera::Initialize(const int screen_width, const int screen_height, const float tower_width, const float tower_height) {
        screen_width_ = screen_width;
        screen_height_ = screen_height;
        tower_width_ = tower_width;
        tower_height_ = tower_height;

        // Set camera offset to center of screen
        camera_.offset = {screen_width / 2.0f, screen_height / 2.0f};

        // Start centered on tower
        target_position_ = {tower_width / 2.0f, tower_height / 2.0f};
        camera_.target = target_position_;
    }

    void Camera::UpdateScreenSize(const int screen_width, const int screen_height) {
        screen_width_ = screen_width;
        screen_height_ = screen_height;

        // Update camera offset to new screen center
        camera_.offset = {screen_width / 2.0f, screen_height / 2.0f};
    }

    void Camera::Update(const float delta_time) {
        // Update follow mode
        if (following_) {
            target_position_ = followed_position_;
        }

        // Smooth movement
        SmoothMove(delta_time);

        // Apply bounds
        ApplyBounds();

        // Update camera
        camera_.target = target_position_;
        camera_.zoom = current_zoom_;
    }

    void Camera::HandleInput(const bool hud_handled) {
        // Check if mouse is over camera controls - if so, don't handle camera input
        const Vector2 mouse_pos = GetMousePosition();
        if (IsMouseOverControls(static_cast<int>(mouse_pos.x), static_cast<int>(mouse_pos.y))) {
            is_panning_ = false;
            return;
        }

        // Don't handle input if HUD consumed it
        if (hud_handled) {
            is_panning_ = false;
            return;
        }

        // Pan with mouse drag
        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
            is_panning_ = true;
            pan_start_mouse_ = GetMousePosition();
            pan_start_camera_ = target_position_;
            following_ = false;  // Stop following when panning
        }

        if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
            is_panning_ = false;
        }

        if (is_panning_) {
            const Vector2 current_mouse = GetMousePosition();
            const float dx = (pan_start_mouse_.x - current_mouse.x) / current_zoom_;
            const float dy = (pan_start_mouse_.y - current_mouse.y) / current_zoom_;
            target_position_ = {pan_start_camera_.x + dx, pan_start_camera_.y + dy};
            following_ = false;
        }

        // Pan with keyboard
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            target_position_.x -= KEYBOARD_PAN_SPEED / current_zoom_ * GetFrameTime();
            following_ = false;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            target_position_.x += KEYBOARD_PAN_SPEED / current_zoom_ * GetFrameTime();
            following_ = false;
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            target_position_.y -= KEYBOARD_PAN_SPEED / current_zoom_ * GetFrameTime();
            following_ = false;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            target_position_.y += KEYBOARD_PAN_SPEED / current_zoom_ * GetFrameTime();
            following_ = false;
        }

        // Zoom with mouse wheel
        const float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            target_zoom_ += wheel * ZOOM_INCREMENT;
            target_zoom_ = std::clamp(target_zoom_, MIN_ZOOM, MAX_ZOOM);
        }

        // Zoom with keyboard
        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
            target_zoom_ += ZOOM_INCREMENT;
            target_zoom_ = std::clamp(target_zoom_, MIN_ZOOM, MAX_ZOOM);
        }
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            target_zoom_ -= ZOOM_INCREMENT;
            target_zoom_ = std::clamp(target_zoom_, MIN_ZOOM, MAX_ZOOM);
        }

        // Reset camera
        if (IsKeyPressed(KEY_HOME)) {
            Reset();
        }

        // Toggle follow mode with F key
        if (IsKeyPressed(KEY_F)) {
            if (following_) {
                StopFollowing();
            }
        }
    }

    void Camera::BeginMode() const {
        BeginMode2D(camera_);
    }

    void Camera::EndMode() {
        EndMode2D();
    }

    void Camera::Reset() {
        target_position_ = {tower_width_ / 2.0f, tower_height_ / 2.0f};
        target_zoom_ = 1.0f;
        following_ = false;
        followed_entity_id_ = -1;
    }

    void Camera::FollowEntity(const float entity_x, const float entity_y, const int entity_id) {
        following_ = true;
        followed_entity_id_ = entity_id;
        followed_position_ = {entity_x, entity_y};
    }

    void Camera::StopFollowing() {
        following_ = false;
        followed_entity_id_ = -1;
    }

    void Camera::ScreenToWorld(const int screen_x, const int screen_y, float& world_x, float& world_y) const {
        const Vector2 world_pos = GetScreenToWorld2D({static_cast<float>(screen_x), static_cast<float>(screen_y)}, camera_);
        world_x = world_pos.x;
        world_y = world_pos.y;
    }

    void Camera::WorldToScreen(const float world_x, const float world_y, int& screen_x, int& screen_y) const {
        const Vector2 screen_pos = GetWorldToScreen2D({world_x, world_y}, camera_);
        screen_x = static_cast<int>(screen_pos.x);
        screen_y = static_cast<int>(screen_pos.y);
    }

    void Camera::RenderControlsOverlay() const {
        // Position in lower-left corner (Sims-style)
        constexpr int x = 10;
        const int y = screen_height_ - 180 - 60;  // Above speed controls (60 = speed control height + gap)
        constexpr int width = 220;
        constexpr int height = 170;

        // Background
        DrawRectangle(x, y, width, height, Fade(BLACK, 0.7f));
        DrawRectangleLines(x, y, width, height, LIGHTGRAY);

        // Title
        DrawText("CAMERA", x + 10, y + 5, 14, YELLOW);

        // Instructions
        DrawText("Pan: Mid-Click+Drag", x + 10, y + 25, 12, LIGHTGRAY);
        DrawText("     or Arrow Keys", x + 10, y + 40, 12, LIGHTGRAY);
        DrawText("Zoom: Mouse Wheel", x + 10, y + 55, 12, LIGHTGRAY);
        DrawText("      or +/- keys", x + 10, y + 70, 12, LIGHTGRAY);
        DrawText("Reset: Home", x + 10, y + 85, 12, LIGHTGRAY);
        DrawText("Follow: F", x + 10, y + 100, 12, LIGHTGRAY);

        // Zoom slider
        const int slider_x = x + 10;
        const int slider_y = y + 120;
        constexpr int slider_width = 200;
        constexpr int slider_height = 20;
        
        DrawText("Zoom:", slider_x, slider_y - 15, 12, LIGHTGRAY);
        
        // Slider track
        DrawRectangle(slider_x, slider_y, slider_width, slider_height, DARKGRAY);
        DrawRectangleLines(slider_x, slider_y, slider_width, slider_height, LIGHTGRAY);
        
        // Slider handle position
        const float zoom_normalized = (target_zoom_ - MIN_ZOOM) / (MAX_ZOOM - MIN_ZOOM);
        const int handle_x = slider_x + static_cast<int>(zoom_normalized * (slider_width - 10));
        
        // Slider handle
        DrawRectangle(handle_x, slider_y - 2, 10, slider_height + 4, GREEN);
        DrawRectangleLines(handle_x, slider_y - 2, 10, slider_height + 4, LIGHTGRAY);
        
        // Zoom percentage
        const char* zoom_text = TextFormat("%.0f%%", target_zoom_ * 100);
        DrawText(zoom_text, slider_x + slider_width + 10, slider_y + 3, 12, GREEN);
    }

    bool Camera::IsMouseOverControls(const int mouse_x, const int mouse_y) const {
        constexpr int x = 10;
        const int y = screen_height_ - 180 - 60;
        constexpr int width = 220;
        constexpr int height = 170;
        
        return mouse_x >= x && mouse_x <= x + width && 
               mouse_y >= y && mouse_y <= y + height;
    }

    bool Camera::HandleControlsClick(const int mouse_x, const int mouse_y) {
        // Check if clicking on zoom slider
        constexpr int panel_x = 10;
        const int panel_y = screen_height_ - 180 - 60;
        const int slider_x = panel_x + 10;
        const int slider_y = panel_y + 120;
        constexpr int slider_width = 200;
        constexpr int slider_height = 20;
        
        if (mouse_x >= slider_x && mouse_x <= slider_x + slider_width &&
            mouse_y >= slider_y && mouse_y <= slider_y + slider_height) {
            // Calculate new zoom based on click position
            const float normalized = static_cast<float>(mouse_x - slider_x) / static_cast<float>(slider_width);
            float new_zoom = MIN_ZOOM + normalized * (MAX_ZOOM - MIN_ZOOM);
            
            // Round to nearest increment
            new_zoom = std::round(new_zoom / ZOOM_INCREMENT) * ZOOM_INCREMENT;
            
            target_zoom_ = std::clamp(new_zoom, MIN_ZOOM, MAX_ZOOM);
            return true;
        }
        
        return false;
    }

    void Camera::RenderFollowIndicator() const {
        if (!following_) {
            return;
        }

        const int x = screen_width_ / 2 - 150;
        constexpr int y = 50;
        constexpr int width = 300;
        constexpr int height = 60;

        // Background
        DrawRectangle(x, y, width, height, Fade(BLACK, 0.8f));
        DrawRectangleLines(x, y, width, height, YELLOW);

        // Icon and text
        DrawText("📍", x + 10, y + 5, 20, YELLOW);

        const char* follow_text = TextFormat("Following: Entity #%d", followed_entity_id_);
        DrawText(follow_text, x + 40, y + 10, 14, WHITE);

        const char* pos_text = TextFormat("Position: (%.0f, %.0f)", followed_position_.x, followed_position_.y);
        DrawText(pos_text, x + 40, y + 30, 12, LIGHTGRAY);

        // Stop button
        DrawRectangle(x + width - 120, y + 10, 110, 25, DARKGRAY);
        DrawText("[Stop Follow]", x + width - 115, y + 15, 12, RED);
    }

    void Camera::SetTowerBounds(const float width, const float height) {
        tower_width_ = width;
        tower_height_ = height;
    }

    void Camera::UpdatePan(float delta_time) {
        // Pan updates are handled in HandleInput
    }

    void Camera::UpdateZoom(float delta_time) {
        // Zoom updates are handled in HandleInput
    }

    void Camera::ApplyBounds() {
        // Allow camera to pan freely across the entire tower
        // Bounds are fixed to tower size, not dependent on zoom level
        
        // Simple bounds - allow some margin beyond tower edges
        constexpr float margin = 200.0f;  // Fixed margin in world units
        
        const float min_x = -margin;
        const float max_x = tower_width_ + margin;
        const float min_y = -margin;
        const float max_y = tower_height_ + margin;
        
        target_position_.x = std::clamp(target_position_.x, min_x, max_x);
        target_position_.y = std::clamp(target_position_.y, min_y, max_y);
    }

    void Camera::SmoothMove(const float delta_time) {
        // Smooth zoom
        const float zoom_diff = target_zoom_ - current_zoom_;
        current_zoom_ += zoom_diff * ZOOM_SMOOTHING * delta_time;

        // Clamp to prevent overshoot
        if (std::abs(zoom_diff) < 0.001f) {
            current_zoom_ = target_zoom_;
        }
    }

}
