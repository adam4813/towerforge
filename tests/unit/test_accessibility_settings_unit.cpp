#include <gtest/gtest.h>
#include "core/accessibility_settings.hpp"

using namespace TowerForge::Core;

// Unit tests for AccessibilitySettings
// These tests verify accessibility configuration and validation

class AccessibilitySettingsUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Get singleton instance
        settings = &AccessibilitySettings::GetInstance();
    }

    AccessibilitySettings* settings;
};

TEST_F(AccessibilitySettingsUnitTest, DefaultValues) {
    EXPECT_FALSE(settings->IsHighContrastEnabled());
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 1.0f);
    EXPECT_TRUE(settings->IsKeyboardNavigationEnabled());
}

TEST_F(AccessibilitySettingsUnitTest, EnableHighContrast) {
    settings->SetHighContrastEnabled(true);
    EXPECT_TRUE(settings->IsHighContrastEnabled());
    
    settings->SetHighContrastEnabled(false);
    EXPECT_FALSE(settings->IsHighContrastEnabled());
}

TEST_F(AccessibilitySettingsUnitTest, SetFontScale) {
    settings->SetFontScale(1.5f);
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 1.5f);
    
    settings->SetFontScale(2.0f);
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 2.0f);
    
    settings->SetFontScale(0.8f);
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 0.8f);
}

TEST_F(AccessibilitySettingsUnitTest, FontScaleClamping) {
    // Values should be clamped to [0.5, 3.0]
    settings->SetFontScale(0.1f);
    EXPECT_GE(settings->GetFontScale(), 0.5f);
    
    settings->SetFontScale(5.0f);
    EXPECT_LE(settings->GetFontScale(), 3.0f);
}

TEST_F(AccessibilitySettingsUnitTest, KeyboardNavigationToggle) {
    settings->SetKeyboardNavigationEnabled(false);
    EXPECT_FALSE(settings->IsKeyboardNavigationEnabled());
    
    settings->SetKeyboardNavigationEnabled(true);
    EXPECT_TRUE(settings->IsKeyboardNavigationEnabled());
}

TEST_F(AccessibilitySettingsUnitTest, MultipleSettingsIndependent) {
    settings->SetHighContrastEnabled(true);
    settings->SetFontScale(1.8f);
    settings->SetKeyboardNavigationEnabled(true);
    
    EXPECT_TRUE(settings->IsHighContrastEnabled());
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 1.8f);
    EXPECT_TRUE(settings->IsKeyboardNavigationEnabled());
}

TEST_F(AccessibilitySettingsUnitTest, ToggleSettings) {
    // Initial state
    bool initial = settings->IsHighContrastEnabled();
    
    // Toggle
    settings->SetHighContrastEnabled(!initial);
    EXPECT_EQ(settings->IsHighContrastEnabled(), !initial);
    
    // Toggle back
    settings->SetHighContrastEnabled(initial);
    EXPECT_EQ(settings->IsHighContrastEnabled(), initial);
}

TEST_F(AccessibilitySettingsUnitTest, FontScaleProgression) {
    // Test various font scale values
    std::vector<float> scales = {0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f, 2.5f, 3.0f};
    
    for (float scale : scales) {
        settings->SetFontScale(scale);
        EXPECT_FLOAT_EQ(settings->GetFontScale(), scale);
    }
}

TEST_F(AccessibilitySettingsUnitTest, StateConsistency) {
    // Ensure settings maintain consistency across multiple operations
    for (int i = 0; i < 10; ++i) {
        settings->SetHighContrastEnabled(i % 2 == 0);
        settings->SetFontScale(1.0f + (i % 5) * 0.2f);
    }
    
    // Final values should be from last iteration
    EXPECT_FALSE(settings->IsHighContrastEnabled());  // 10 % 2 != 0
    EXPECT_FLOAT_EQ(settings->GetFontScale(), 1.8f);  // (9 % 5) * 0.2 = 0.8
}

