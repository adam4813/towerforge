#include "core/accessibility_settings.hpp"
#include <iostream>
#include <fstream>

namespace towerforge::core {

    void AccessibilitySettings::LoadSettings() {
        try {
            std::ifstream file(SETTINGS_FILE);
            if (!file.is_open()) {
                std::cout << "Accessibility settings file not found, using defaults" << std::endl;
                return;
            }

            nlohmann::json j;
            file >> j;

            if (j.contains("high_contrast_enabled")) {
                high_contrast_enabled_ = j["high_contrast_enabled"];
            }

            if (j.contains("font_scale")) {
                font_scale_ = j["font_scale"];
                font_scale_ = std::clamp(font_scale_, 0.5f, 3.0f);
            }

            if (j.contains("keyboard_navigation_enabled")) {
                keyboard_navigation_enabled_ = j["keyboard_navigation_enabled"];
            }

            std::cout << "Accessibility settings loaded successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading accessibility settings: " << e.what() << std::endl;
        }
    }

    void AccessibilitySettings::SaveSettings() const {
        try {
            nlohmann::json j;
            j["high_contrast_enabled"] = high_contrast_enabled_;
            j["font_scale"] = font_scale_;
            j["keyboard_navigation_enabled"] = keyboard_navigation_enabled_;

            std::ofstream file(SETTINGS_FILE);
            if (!file.is_open()) {
                std::cerr << "Failed to open accessibility settings file for writing" << std::endl;
                return;
            }

            file << j.dump(2);
            std::cout << "Accessibility settings saved successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error saving accessibility settings: " << e.what() << std::endl;
        }
    }

} // namespace towerforge::core::
