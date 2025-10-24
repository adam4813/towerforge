#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "audio/audio_manager.h"
#include "ui/ui_element.h"

namespace towerforge::ui {

    /**
     * @brief Audio settings menu for adjusting volume levels
     * 
     * Declarative, event-driven UI using Slider and Checkbox components.
     * All changes apply immediately via callbacks and persist automatically.
     */
    class AudioSettingsMenu {
    public:
        using BackCallback = std::function<void()>;

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
         */
        void HandleKeyboard();
    
        /**
         * @brief Handle mouse input for menu interaction
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         */
        void HandleMouse(int mouse_x, int mouse_y, bool clicked);

        /**
         * @brief Set callback for back button
         */
        void SetBackCallback(const BackCallback &callback) { back_callback_ = callback; }
    
    private:
        void UpdateLayout();
        void UpdateSelection(int new_selection);
        void LoadSettings();
        void SaveSettings() const;
        void ApplyAudioSettings();
        void RenderHeader() const;
    
        BackCallback back_callback_;
        std::unique_ptr<Panel> settings_panel_;
        std::unique_ptr<class PanelHeaderOverlay> header_overlay_;
        std::unique_ptr<class DimOverlay> dim_overlay_;
        std::vector<UIElement*> interactive_elements_;  // Pointers to focusable elements
        
        Slider* master_slider_;
        Slider* music_slider_;
        Slider* sfx_slider_;
        Checkbox* mute_all_checkbox_;
        Checkbox* mute_music_checkbox_;
        Checkbox* mute_sfx_checkbox_;
        Checkbox* enable_ambient_checkbox_;
        Button* back_button_;
        
        int selected_index_;
        float animation_time_;
        int last_screen_width_;
        int last_screen_height_;
        
        // Volume levels (0.0 to 1.0)
        float master_volume_;
        float music_volume_;
        float sfx_volume_;
    
        // Mute states
        bool mute_all_;
        bool mute_music_;
        bool mute_sfx_;
        bool enable_ambient_;
    
        // Menu layout constants
        static constexpr int MENU_WIDTH = 600;
        static constexpr int MENU_HEIGHT = 650;  // Increased to prevent overlap
        static constexpr int SLIDER_START_Y = 120;
        static constexpr int SLIDER_HEIGHT = 70;
        static constexpr int SLIDER_SPACING = 20;
        static constexpr int CHECKBOX_START_Y = 380;
        static constexpr int CHECKBOX_HEIGHT = 40;
        static constexpr int CHECKBOX_SPACING = 10;
        static constexpr int BACK_BUTTON_Y = 580;  // Moved down to avoid overlap with checkboxes
    };

}
