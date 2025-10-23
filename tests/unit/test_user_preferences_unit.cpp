#include <gtest/gtest.h>
#include "core/user_preferences.hpp"
#include <filesystem>

using namespace TowerForge::Core;

// Unit tests for UserPreferences
// These tests verify preferences persistence, validation, and state management

class UserPreferencesUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up preferences file before each test
        CleanupPreferencesFile();
    }

    void TearDown() override {
        // Clean up after test
        CleanupPreferencesFile();
    }

    void CleanupPreferencesFile() {
        std::string config_path = GetPreferencesPath();
        if (std::filesystem::exists(config_path)) {
            std::filesystem::remove(config_path);
        }
    }

    std::string GetPreferencesPath() {
#ifdef _WIN32
        if (const char* appdata = std::getenv("APPDATA")) {
            return std::string(appdata) + "/TowerForge/user_preferences.json";
        }
        return "user_preferences.json";
#else
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + "/.towerforge/user_preferences.json";
        }
        return "user_preferences.json";
#endif
    }
};

TEST_F(UserPreferencesUnitTest, DefaultValues) {
    auto& prefs = UserPreferences::GetInstance();
    
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.7f);
    EXPECT_FLOAT_EQ(prefs.GetMusicVolume(), 0.5f);
    EXPECT_FLOAT_EQ(prefs.GetSFXVolume(), 0.6f);
    EXPECT_FLOAT_EQ(prefs.GetUIScale(), 1.0f);
    EXPECT_FLOAT_EQ(prefs.GetFontScale(), 1.0f);
    EXPECT_FALSE(prefs.IsHighContrastEnabled());
    EXPECT_FALSE(prefs.GetMuteAll());
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::Light);  // Default is Light mode
}

TEST_F(UserPreferencesUnitTest, SetAndGetMasterVolume) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMasterVolume(0.8f);
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.8f);
    
    prefs.SetMasterVolume(0.0f);
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.0f);
    
    prefs.SetMasterVolume(1.0f);
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 1.0f);
}

TEST_F(UserPreferencesUnitTest, VolumeClampingToValidRange) {
    auto& prefs = UserPreferences::GetInstance();
    
    // Values should be clamped to [0.0, 1.0]
    prefs.SetMasterVolume(-0.5f);
    EXPECT_GE(prefs.GetMasterVolume(), 0.0f);
    
    prefs.SetMasterVolume(1.5f);
    EXPECT_LE(prefs.GetMasterVolume(), 1.0f);
}

TEST_F(UserPreferencesUnitTest, SetAndGetMusicVolume) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMusicVolume(0.75f);
    EXPECT_FLOAT_EQ(prefs.GetMusicVolume(), 0.75f);
}

TEST_F(UserPreferencesUnitTest, SetAndGetSFXVolume) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetSFXVolume(0.9f);
    EXPECT_FLOAT_EQ(prefs.GetSFXVolume(), 0.9f);
}

TEST_F(UserPreferencesUnitTest, SetAndGetUIScale) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetUIScale(1.5f);
    EXPECT_FLOAT_EQ(prefs.GetUIScale(), 1.5f);
    
    prefs.SetUIScale(0.8f);
    EXPECT_FLOAT_EQ(prefs.GetUIScale(), 0.8f);
}

TEST_F(UserPreferencesUnitTest, SetAndGetFontScale) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetFontScale(1.2f);
    EXPECT_FLOAT_EQ(prefs.GetFontScale(), 1.2f);
}

TEST_F(UserPreferencesUnitTest, SetAndGetHighContrast) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetHighContrastEnabled(true);
    EXPECT_TRUE(prefs.IsHighContrastEnabled());
    
    prefs.SetHighContrastEnabled(false);
    EXPECT_FALSE(prefs.IsHighContrastEnabled());
}

TEST_F(UserPreferencesUnitTest, SetAndGetMuteAll) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMuteAll(true);
    EXPECT_TRUE(prefs.GetMuteAll());
    
    prefs.SetMuteAll(false);
    EXPECT_FALSE(prefs.GetMuteAll());
}

TEST_F(UserPreferencesUnitTest, SetAndGetColorMode) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetColorMode(ColorMode::Light);
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::Light);
    
    prefs.SetColorMode(ColorMode::Dark);
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::Dark);
    
    prefs.SetColorMode(ColorMode::HighContrast);
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::HighContrast);
}

TEST_F(UserPreferencesUnitTest, PersistenceAcrossInstances) {
    // Set values in first instance
    {
        auto& prefs = UserPreferences::GetInstance();
        prefs.SetMasterVolume(0.85f);
        prefs.SetMusicVolume(0.65f);
        prefs.SetUIScale(1.3f);
        prefs.SetHighContrastEnabled(true);
    }
    
    // Note: Since UserPreferences is a singleton, we can't easily test
    // persistence across actual program restarts in a unit test.
    // The file should be created though
    EXPECT_TRUE(std::filesystem::exists(GetPreferencesPath()));
}

TEST_F(UserPreferencesUnitTest, MultipleModificationsPreserveLast) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMasterVolume(0.3f);
    prefs.SetMasterVolume(0.5f);
    prefs.SetMasterVolume(0.7f);
    
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.7f);
}

TEST_F(UserPreferencesUnitTest, IndependentVolumeControls) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMasterVolume(0.8f);
    prefs.SetMusicVolume(0.6f);
    prefs.SetSFXVolume(0.4f);
    
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.8f);
    EXPECT_FLOAT_EQ(prefs.GetMusicVolume(), 0.6f);
    EXPECT_FLOAT_EQ(prefs.GetSFXVolume(), 0.4f);
}

TEST_F(UserPreferencesUnitTest, IndependentScaleControls) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetUIScale(1.5f);
    prefs.SetFontScale(1.2f);
    
    EXPECT_FLOAT_EQ(prefs.GetUIScale(), 1.5f);
    EXPECT_FLOAT_EQ(prefs.GetFontScale(), 1.2f);
}

TEST_F(UserPreferencesUnitTest, MuteAllOverridesVolumes) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetMasterVolume(0.8f);
    prefs.SetMusicVolume(0.6f);
    prefs.SetSFXVolume(0.7f);
    
    prefs.SetMuteAll(true);
    
    // When muted, volumes should still return their set values
    // (the application logic should check GetMuteAll())
    EXPECT_TRUE(prefs.GetMuteAll());
    EXPECT_FLOAT_EQ(prefs.GetMasterVolume(), 0.8f);
}

TEST_F(UserPreferencesUnitTest, ColorModeAndHighContrastInteraction) {
    auto& prefs = UserPreferences::GetInstance();
    
    prefs.SetColorMode(ColorMode::HighContrast);
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::HighContrast);
    
    prefs.SetHighContrastEnabled(true);
    EXPECT_TRUE(prefs.IsHighContrastEnabled());
    
    // Both settings coexist independently
    EXPECT_EQ(prefs.GetColorMode(), ColorMode::HighContrast);
}

TEST_F(UserPreferencesUnitTest, SingletonBehavior) {
    auto& prefs1 = UserPreferences::GetInstance();
    auto& prefs2 = UserPreferences::GetInstance();
    
    // Should be the same instance
    prefs1.SetMasterVolume(0.55f);
    EXPECT_FLOAT_EQ(prefs2.GetMasterVolume(), 0.55f);
}
