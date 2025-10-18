#pragma once

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ui/notification_center.h"

namespace TowerForge::Core {

    /**
     * @brief Color/theme mode options
     */
    enum class ColorMode {
        Light,
        Dark,
        HighContrast
    };

    /**
     * @brief Unified user preferences manager
     * 
     * Manages all user preferences including audio, UI scaling, color mode,
     * notification preferences, and accessibility settings. Settings are persisted
     * to disk and loaded on startup.
     */
    class UserPreferences {
    public:
        static UserPreferences& GetInstance() {
            static UserPreferences instance;
            return instance;
        }

        // Delete copy/move constructors for singleton
        UserPreferences(const UserPreferences&) = delete;
        UserPreferences& operator=(const UserPreferences&) = delete;
        UserPreferences(UserPreferences&&) = delete;
        UserPreferences& operator=(UserPreferences&&) = delete;

        // Audio settings
        float GetMasterVolume() const { return master_volume_; }
        void SetMasterVolume(float volume) {
            master_volume_ = std::clamp(volume, 0.0f, 1.0f);
            SaveSettings();
        }

        float GetMusicVolume() const { return music_volume_; }
        void SetMusicVolume(float volume) {
            music_volume_ = std::clamp(volume, 0.0f, 1.0f);
            SaveSettings();
        }

        float GetSFXVolume() const { return sfx_volume_; }
        void SetSFXVolume(float volume) {
            sfx_volume_ = std::clamp(volume, 0.0f, 1.0f);
            SaveSettings();
        }

        bool GetMuteAll() const { return mute_all_; }
        void SetMuteAll(bool mute) {
            mute_all_ = mute;
            SaveSettings();
        }

        bool GetMuteMusic() const { return mute_music_; }
        void SetMuteMusic(bool mute) {
            mute_music_ = mute;
            SaveSettings();
        }

        bool GetMuteSFX() const { return mute_sfx_; }
        void SetMuteSFX(bool mute) {
            mute_sfx_ = mute;
            SaveSettings();
        }

        bool GetEnableAmbient() const { return enable_ambient_; }
        void SetEnableAmbient(bool enable) {
            enable_ambient_ = enable;
            SaveSettings();
        }

        // UI Scaling (1.0 = 100%, 1.5 = 150%, etc.)
        float GetUIScale() const { return ui_scale_; }
        void SetUIScale(float scale) {
            ui_scale_ = std::clamp(scale, 0.5f, 2.0f);
            SaveSettings();
        }

        // Color mode
        ColorMode GetColorMode() const { return color_mode_; }
        void SetColorMode(ColorMode mode) {
            color_mode_ = mode;
            SaveSettings();
        }

        // Notification preferences
        towerforge::ui::NotificationFilter& GetNotificationFilter() { return notification_filter_; }
        const towerforge::ui::NotificationFilter& GetNotificationFilter() const { return notification_filter_; }
        void SetNotificationFilter(const towerforge::ui::NotificationFilter& filter) {
            notification_filter_ = filter;
            SaveSettings();
        }

        // Accessibility settings
        bool IsHighContrastEnabled() const { return high_contrast_enabled_; }
        void SetHighContrastEnabled(bool enabled) {
            high_contrast_enabled_ = enabled;
            SaveSettings();
        }

        float GetFontScale() const { return font_scale_; }
        void SetFontScale(float scale) {
            font_scale_ = std::clamp(scale, 0.5f, 3.0f);
            SaveSettings();
        }

        bool IsKeyboardNavigationEnabled() const { return keyboard_navigation_enabled_; }
        void SetKeyboardNavigationEnabled(bool enabled) {
            keyboard_navigation_enabled_ = enabled;
            SaveSettings();
        }

        // Load and save settings
        void LoadSettings();
        void SaveSettings() const;

    private:
        UserPreferences()
            : master_volume_(0.7f)
            , music_volume_(0.5f)
            , sfx_volume_(0.6f)
            , mute_all_(false)
            , mute_music_(false)
            , mute_sfx_(false)
            , enable_ambient_(true)
            , ui_scale_(1.0f)
            , color_mode_(ColorMode::Dark)
            , high_contrast_enabled_(false)
            , font_scale_(1.0f)
            , keyboard_navigation_enabled_(true) {
            LoadSettings();
        }

        ~UserPreferences() = default;

        // Audio settings
        float master_volume_;
        float music_volume_;
        float sfx_volume_;
        bool mute_all_;
        bool mute_music_;
        bool mute_sfx_;
        bool enable_ambient_;

        // UI settings
        float ui_scale_;
        ColorMode color_mode_;

        // Notification settings
        towerforge::ui::NotificationFilter notification_filter_;

        // Accessibility settings
        bool high_contrast_enabled_;
        float font_scale_;
        bool keyboard_navigation_enabled_;

        static constexpr const char* SETTINGS_FILE = "user_preferences.json";
    };

} // namespace TowerForge::Core
