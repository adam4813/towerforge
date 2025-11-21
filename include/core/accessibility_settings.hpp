#pragma once

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

namespace towerforge::core {

    /**
     * @brief Accessibility settings manager
     * 
     * Manages accessibility settings like high-contrast mode, font scaling,
     * and keyboard navigation preferences. Settings are persisted to disk.
     */
    class AccessibilitySettings {
    public:
        static AccessibilitySettings& GetInstance() {
            static AccessibilitySettings instance;
            return instance;
        }

        // Delete copy/move constructors for singleton
        AccessibilitySettings(const AccessibilitySettings&) = delete;
        AccessibilitySettings& operator=(const AccessibilitySettings&) = delete;
        AccessibilitySettings(AccessibilitySettings&&) = delete;
        AccessibilitySettings& operator=(AccessibilitySettings&&) = delete;

        // High-contrast mode
        bool IsHighContrastEnabled() const { return high_contrast_enabled_; }
        void SetHighContrastEnabled(const bool enabled) { 
            high_contrast_enabled_ = enabled;
            SaveSettings();
        }

        // Font scaling (1.0 = normal, 1.5 = 150%, 2.0 = 200%)
        float GetFontScale() const { return font_scale_; }
        void SetFontScale(const float scale) { 
            font_scale_ = std::clamp(scale, 0.5f, 3.0f);
            SaveSettings();
        }

        // Keyboard navigation enabled (always enabled for accessibility)
        bool IsKeyboardNavigationEnabled() const { return keyboard_navigation_enabled_; }
        void SetKeyboardNavigationEnabled(const bool enabled) { 
            keyboard_navigation_enabled_ = enabled;
            SaveSettings();
        }

        // Load and save settings
        void LoadSettings();
        void SaveSettings() const;

    private:
        AccessibilitySettings() 
            : high_contrast_enabled_(false)
            , font_scale_(1.0f)
            , keyboard_navigation_enabled_(true) {
            LoadSettings();
        }

        ~AccessibilitySettings() = default;

        bool high_contrast_enabled_;
        float font_scale_;
        bool keyboard_navigation_enabled_;

        static constexpr const char* SETTINGS_FILE = "accessibility_settings.json";
    };

} // namespace towerforge::core
