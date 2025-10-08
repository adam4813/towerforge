#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "audio/audio_manager.h"

namespace towerforge {
namespace ui {

/**
 * @brief Audio settings menu for adjusting volume levels
 * 
 * Provides controls for master volume, music volume, and sound effects volume.
 * All changes apply immediately and persist.
 */
class AudioSettingsMenu {
public:
    AudioSettingsMenu();
    ~AudioSettingsMenu();
    
    /**
     * @brief Render the audio settings menu
     */
    void Render();
    
    /**
     * @brief Update menu state (called every frame)
     * @param delta_time Time elapsed since last frame
     */
    void Update(float delta_time);
    
    /**
     * @brief Sync local volume values with AudioManager
     */
    void SyncWithAudioManager();
    
    /**
     * @brief Handle keyboard input for menu navigation
     * @return True if ESC was pressed to go back
     */
    bool HandleKeyboard();
    
    /**
     * @brief Handle mouse input for menu interaction
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param clicked Whether mouse was clicked
     * @return True if back button was clicked
     */
    bool HandleMouse(int mouse_x, int mouse_y, bool clicked);
    
private:
    void RenderBackground();
    void RenderHeader();
    void RenderVolumeControls();
    void RenderBackButton();
    void RenderVolumeSlider(const char* label, float value, int y_pos, bool is_selected);
    
    int selected_option_;  // Currently highlighted menu option (0=master, 1=music, 2=sfx, 3=back)
    float animation_time_; // For animations
    
    // Volume levels (0.0 to 1.0)
    float master_volume_;
    float music_volume_;
    float sfx_volume_;
    
    // Menu layout constants
    static constexpr int MENU_WIDTH = 500;
    static constexpr int MENU_HEIGHT = 400;
    static constexpr int HEADER_HEIGHT = 80;
    static constexpr int SLIDER_HEIGHT = 60;
    static constexpr int SLIDER_SPACING = 20;
    static constexpr int SLIDER_START_Y = 150;
    static constexpr int BACK_BUTTON_Y = 450;
    static constexpr int BACK_BUTTON_WIDTH = 150;
    static constexpr int BACK_BUTTON_HEIGHT = 50;
};

} // namespace ui
} // namespace towerforge
