#include "core/user_preferences.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace TowerForge::Core {

    void UserPreferences::LoadSettings() {
        try {
            // Determine config file path
            std::string config_path;
            std::string config_dir;

#ifdef _WIN32
            if (const char* appdata = std::getenv("APPDATA")) {
                config_dir = std::string(appdata) + "/TowerForge";
                config_path = config_dir + "/" + SETTINGS_FILE;
            } else {
                config_path = SETTINGS_FILE;
            }
#else
            const char* home = std::getenv("HOME");
            if (home) {
                config_dir = std::string(home) + "/.towerforge";
                config_path = config_dir + "/" + SETTINGS_FILE;
            } else {
                config_path = SETTINGS_FILE;
            }
#endif

            std::ifstream file(config_path);
            if (!file.is_open()) {
                std::cout << "User preferences file not found, using defaults" << std::endl;
                return;
            }

            nlohmann::json j;
            file >> j;

            // Load audio settings
            if (j.contains("audio")) {
                const auto& audio = j["audio"];
                if (audio.contains("master_volume")) master_volume_ = audio["master_volume"];
                if (audio.contains("music_volume")) music_volume_ = audio["music_volume"];
                if (audio.contains("sfx_volume")) sfx_volume_ = audio["sfx_volume"];
                if (audio.contains("mute_all")) mute_all_ = audio["mute_all"];
                if (audio.contains("mute_music")) mute_music_ = audio["mute_music"];
                if (audio.contains("mute_sfx")) mute_sfx_ = audio["mute_sfx"];
                if (audio.contains("enable_ambient")) enable_ambient_ = audio["enable_ambient"];
            }

            // Load UI settings
            if (j.contains("ui")) {
                const auto& ui = j["ui"];
                if (ui.contains("scale")) {
                    ui_scale_ = ui["scale"];
                    ui_scale_ = std::clamp(ui_scale_, 0.5f, 2.0f);
                }
                if (ui.contains("color_mode")) {
                    const std::string mode_str = ui["color_mode"];
                    if (mode_str == "light") color_mode_ = ColorMode::Light;
                    else if (mode_str == "dark") color_mode_ = ColorMode::Dark;
                    else if (mode_str == "high_contrast") color_mode_ = ColorMode::HighContrast;
                }
            }

            // Load notification preferences
            if (j.contains("notifications")) {
                const auto& notif = j["notifications"];
                if (notif.contains("show_info")) notification_filter_.show_info = notif["show_info"];
                if (notif.contains("show_warning")) notification_filter_.show_warning = notif["show_warning"];
                if (notif.contains("show_error")) notification_filter_.show_error = notif["show_error"];
                if (notif.contains("show_success")) notification_filter_.show_success = notif["show_success"];
                if (notif.contains("show_achievement")) notification_filter_.show_achievement = notif["show_achievement"];
                if (notif.contains("show_event")) notification_filter_.show_event = notif["show_event"];
                if (notif.contains("show_read")) notification_filter_.show_read = notif["show_read"];
                if (notif.contains("show_unread")) notification_filter_.show_unread = notif["show_unread"];
                if (notif.contains("show_pinned")) notification_filter_.show_pinned = notif["show_pinned"];
                if (notif.contains("show_unpinned")) notification_filter_.show_unpinned = notif["show_unpinned"];
            }

            // Load accessibility settings
            if (j.contains("accessibility")) {
                const auto& access = j["accessibility"];
                if (access.contains("high_contrast_enabled")) high_contrast_enabled_ = access["high_contrast_enabled"];
                if (access.contains("font_scale")) {
                    font_scale_ = access["font_scale"];
                    font_scale_ = std::clamp(font_scale_, 0.5f, 3.0f);
                }
                if (access.contains("keyboard_navigation_enabled")) keyboard_navigation_enabled_ = access["keyboard_navigation_enabled"];
            }

            std::cout << "User preferences loaded successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading user preferences: " << e.what() << std::endl;
        }
    }

    void UserPreferences::SaveSettings() const {
        try {
            // Determine config file path
            std::string config_path;
            std::string config_dir;

#ifdef _WIN32
            if (const char* appdata = std::getenv("APPDATA")) {
                config_dir = std::string(appdata) + "/TowerForge";
                config_path = config_dir + "/" + SETTINGS_FILE;
            } else {
                config_path = SETTINGS_FILE;
            }
#else
            const char* home = std::getenv("HOME");
            if (home) {
                config_dir = std::string(home) + "/.towerforge";
                config_path = config_dir + "/" + SETTINGS_FILE;
            } else {
                config_path = SETTINGS_FILE;
            }
#endif

            // Create directory if it doesn't exist
            if (!config_dir.empty()) {
                std::filesystem::create_directories(config_dir);
            }

            nlohmann::json j;

            // Save audio settings
            j["audio"]["master_volume"] = master_volume_;
            j["audio"]["music_volume"] = music_volume_;
            j["audio"]["sfx_volume"] = sfx_volume_;
            j["audio"]["mute_all"] = mute_all_;
            j["audio"]["mute_music"] = mute_music_;
            j["audio"]["mute_sfx"] = mute_sfx_;
            j["audio"]["enable_ambient"] = enable_ambient_;

            // Save UI settings
            j["ui"]["scale"] = ui_scale_;
            std::string mode_str;
            switch (color_mode_) {
                case ColorMode::Light: mode_str = "light"; break;
                case ColorMode::Dark: mode_str = "dark"; break;
                case ColorMode::HighContrast: mode_str = "high_contrast"; break;
            }
            j["ui"]["color_mode"] = mode_str;

            // Save notification preferences
            j["notifications"]["show_info"] = notification_filter_.show_info;
            j["notifications"]["show_warning"] = notification_filter_.show_warning;
            j["notifications"]["show_error"] = notification_filter_.show_error;
            j["notifications"]["show_success"] = notification_filter_.show_success;
            j["notifications"]["show_achievement"] = notification_filter_.show_achievement;
            j["notifications"]["show_event"] = notification_filter_.show_event;
            j["notifications"]["show_read"] = notification_filter_.show_read;
            j["notifications"]["show_unread"] = notification_filter_.show_unread;
            j["notifications"]["show_pinned"] = notification_filter_.show_pinned;
            j["notifications"]["show_unpinned"] = notification_filter_.show_unpinned;

            // Save accessibility settings
            j["accessibility"]["high_contrast_enabled"] = high_contrast_enabled_;
            j["accessibility"]["font_scale"] = font_scale_;
            j["accessibility"]["keyboard_navigation_enabled"] = keyboard_navigation_enabled_;

            std::ofstream file(config_path);
            if (!file.is_open()) {
                std::cerr << "Failed to open user preferences file for writing" << std::endl;
                return;
            }

            file << j.dump(2);
            std::cout << "User preferences saved successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error saving user preferences: " << e.what() << std::endl;
        }
    }

} // namespace TowerForge::Core
