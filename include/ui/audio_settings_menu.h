#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "audio/audio_manager.h"

import engine;

namespace towerforge::ui {

    /**
     * @brief Audio settings menu for adjusting volume levels
     * 
     * Declarative, event-driven UI using engine Slider and Checkbox components.
     * All changes apply immediately via callbacks and persist automatically.
     */
    class AudioSettingsMenu {
    public:
        using BackCallback = std::function<void()>;

        AudioSettingsMenu();

        ~AudioSettingsMenu();

        /**
         * @brief Initialize UI components (must be called after graphics context is ready)
         */
        void Initialize();

        /**
         * @brief Clean up UI components
         */
        void Shutdown();

        /**
         * @brief Render the audio settings menu
         */
        void Render() const;

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
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Handle keyboard input for menu navigation
         */
        void HandleKeyboard();

        /**
         * @brief Set callback for back button
         */
        void SetBackCallback(const BackCallback &callback) { back_callback_ = callback; }

    private:
        void UpdateLayout();

        void LoadSettings();

        void SaveSettings() const;

        void ApplyAudioSettings();

        static void RenderDimOverlay();

        BackCallback back_callback_;
        std::unique_ptr<engine::ui::elements::Panel> settings_panel_;

        engine::ui::elements::Slider *master_slider_;
        engine::ui::elements::Slider *music_slider_;
        engine::ui::elements::Slider *sfx_slider_;
        engine::ui::elements::Checkbox *mute_all_checkbox_;
        engine::ui::elements::Checkbox *mute_music_checkbox_;
        engine::ui::elements::Checkbox *mute_sfx_checkbox_;
        engine::ui::elements::Checkbox *enable_ambient_checkbox_;
        engine::ui::elements::Button *back_button_;

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
        static constexpr int MENU_HEIGHT = 450;
        static constexpr int SLIDER_HEIGHT = 30;
        static constexpr int SLIDER_SPACING = 20;
        static constexpr int CHECKBOX_HEIGHT = 30;
        static constexpr int CHECKBOX_SPACING = 10;
    };
}
