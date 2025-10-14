#pragma once

#include <raylib.h>

namespace towerforge::rendering {

    /**
 * @brief Camera class for managing viewport, pan, zoom, and entity following
 * 
 * This class provides camera controls for navigating the tower view,
 * including pan, zoom, entity following, and bounds checking.
 */
    class Camera {
    public:
        Camera();
        ~Camera();

        /**
     * @brief Initialize the camera with screen dimensions and tower bounds
     * @param screen_width Screen width in pixels
     * @param screen_height Screen height in pixels
     * @param tower_width Tower width in world units
     * @param tower_height Tower height in world units
     */
        void Initialize(int screen_width, int screen_height, float tower_width, float tower_height);

        /**
     * @brief Update camera state (called every frame)
     * @param delta_time Time elapsed since last frame
     */
        void Update(float delta_time);

        /**
     * @brief Handle input for camera controls
     * @param hud_handled True if HUD already handled this input
     */
        void HandleInput(bool hud_handled);

        /**
     * @brief Begin camera transform (call before drawing game world)
     */
        void BeginMode() const;

        /**
     * @brief End camera transform (call after drawing game world)
     */
        static void EndMode();

        /**
     * @brief Reset camera to default position and zoom
     */
        void Reset();

        /**
     * @brief Set camera to follow an entity
     * @param entity_x Entity X position in world space
     * @param entity_y Entity Y position in world space
     * @param entity_id Entity ID for display
     */
        void FollowEntity(float entity_x, float entity_y, int entity_id);

        /**
     * @brief Stop following entity
     */
        void StopFollowing();

        /**
     * @brief Check if camera is following an entity
     * @return true if following
     */
        bool IsFollowing() const { return following_; }

        /**
     * @brief Get the entity ID being followed
     * @return Entity ID or -1 if not following
     */
        int GetFollowedEntityId() const { return followed_entity_id_; }

        /**
     * @brief Get current zoom level
     * @return Zoom level (1.0 = 100%)
     */
        float GetZoom() const { return target_zoom_; }

        /**
     * @brief Convert screen coordinates to world coordinates
     * @param screen_x Screen X position
     * @param screen_y Screen Y position
     * @param world_x Output world X position
     * @param world_y Output world Y position
     */
        void ScreenToWorld(int screen_x, int screen_y, float& world_x, float& world_y) const;

        /**
     * @brief Convert world coordinates to screen coordinates
     * @param world_x World X position
     * @param world_y World Y position
     * @param screen_x Output screen X position
     * @param screen_y Output screen Y position
     */
        void WorldToScreen(float world_x, float world_y, int& screen_x, int& screen_y) const;

        /**
     * @brief Render camera controls overlay (bottom right)
     */
        void RenderControlsOverlay() const;

        /**
     * @brief Render follow mode indicator
     */
        void RenderFollowIndicator() const;

        /**
     * @brief Set tower bounds for camera limiting
     * @param width Tower width in world units
     * @param height Tower height in world units
     */
        void SetTowerBounds(float width, float height);

    private:
        static void UpdatePan(float delta_time);

        static void UpdateZoom(float delta_time);
        void ApplyBounds();
        void SmoothMove(float delta_time);

        // Raylib camera
        Camera2D camera_;

        // Camera position and zoom
        Vector2 target_position_;   // Target position for smooth movement
        float target_zoom_;         // Target zoom for smooth zooming
        float current_zoom_;        // Current interpolated zoom

        // Screen and tower dimensions
        int screen_width_;
        int screen_height_;
        float tower_width_;
        float tower_height_;

        // Pan controls
        bool is_panning_;
        Vector2 pan_start_mouse_;
        Vector2 pan_start_camera_;

        // Follow mode
        bool following_;
        int followed_entity_id_;
        Vector2 followed_position_;

        // Zoom levels
        static constexpr float MIN_ZOOM = 0.25f;
        static constexpr float MAX_ZOOM = 2.0f;
        static constexpr float ZOOM_INCREMENT = 0.1f;

        // Movement smoothing
        static constexpr float PAN_SMOOTHING = 8.0f;
        static constexpr float ZOOM_SMOOTHING = 6.0f;

        // Pan speed
        static constexpr float KEYBOARD_PAN_SPEED = 300.0f;
    };

}
