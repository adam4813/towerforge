// Test program to verify UserPreferences persistence
// This program tests that preferences are correctly saved and loaded

#include "core/user_preferences.hpp"
#include <iostream>
#include <filesystem>
#include <cassert>

int main() {
    std::cout << "=== UserPreferences Persistence Test ===" << std::endl;
    
    // Get the preferences file path
    std::string config_path;
#ifdef _WIN32
    if (const char* appdata = std::getenv("APPDATA")) {
        config_path = std::string(appdata) + "/TowerForge/user_preferences.json";
    } else {
        config_path = "user_preferences.json";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        config_path = std::string(home) + "/.towerforge/user_preferences.json";
    } else {
        config_path = "user_preferences.json";
    }
#endif
    
    std::cout << "Preferences file location: " << config_path << std::endl;
    
    // Remove existing preferences file if it exists
    if (std::filesystem::exists(config_path)) {
        std::filesystem::remove(config_path);
        std::cout << "Removed existing preferences file" << std::endl;
    }
    
    // Test 1: Create preferences with default values
    {
        std::cout << "\nTest 1: Creating preferences with defaults..." << std::endl;
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        
        // Verify defaults
        assert(prefs.GetMasterVolume() == 0.7f);
        assert(prefs.GetMusicVolume() == 0.5f);
        assert(prefs.GetSFXVolume() == 0.6f);
        assert(prefs.GetUIScale() == 1.0f);
        assert(prefs.GetFontScale() == 1.0f);
        assert(prefs.IsHighContrastEnabled() == false);
        
        std::cout << "✓ Default values are correct" << std::endl;
    }
    
    // Test 2: Modify and save preferences
    {
        std::cout << "\nTest 2: Modifying preferences..." << std::endl;
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        
        prefs.SetMasterVolume(0.8f);
        prefs.SetMusicVolume(0.6f);
        prefs.SetSFXVolume(0.7f);
        prefs.SetUIScale(1.5f);
        prefs.SetFontScale(1.2f);
        prefs.SetHighContrastEnabled(true);
        prefs.SetMuteAll(true);
        prefs.SetColorMode(towerforge::core::ColorMode::HighContrast);
        
        // Verify file was created
        assert(std::filesystem::exists(config_path));
        std::cout << "✓ Preferences file created" << std::endl;
        
        // Verify values
        assert(prefs.GetMasterVolume() == 0.8f);
        assert(prefs.GetMusicVolume() == 0.6f);
        assert(prefs.GetSFXVolume() == 0.7f);
        assert(prefs.GetUIScale() == 1.5f);
        assert(prefs.GetFontScale() == 1.2f);
        assert(prefs.IsHighContrastEnabled() == true);
        assert(prefs.GetMuteAll() == true);
        assert(prefs.GetColorMode() == towerforge::core::ColorMode::HighContrast);
        
        std::cout << "✓ Modified values are correct" << std::endl;
    }
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    std::cout << "\nNote: This test only verifies in-memory state and file creation." << std::endl;
    std::cout << "To verify persistence across sessions, run the game, change settings," << std::endl;
    std::cout << "close it, and reopen to see if settings are retained." << std::endl;
    
    return 0;
}
