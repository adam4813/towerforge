#include "ui/accessibility_settings_menu.h"
#include "core/accessibility_settings.hpp"
#include "core/user_preferences.hpp"
#include <cmath>
#include <string>

namespace towerforge::ui {

    AccessibilitySettingsMenu::AccessibilitySettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , high_contrast_enabled_(false)
          , font_scale_(1.0f)
          , keyboard_navigation_enabled_(true) {
        SyncWithSettings();
    }

    AccessibilitySettingsMenu::~AccessibilitySettingsMenu() = default;

    void AccessibilitySettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
    }

    void AccessibilitySettingsMenu::SyncWithSettings() {
        // Use unified UserPreferences
        auto& prefs = TowerForge::Core::UserPreferences::GetInstance();
        high_contrast_enabled_ = prefs.IsHighContrastEnabled();
        font_scale_ = prefs.GetFontScale();
        keyboard_navigation_enabled_ = prefs.IsKeyboardNavigationEnabled();
        
        // Also sync with legacy AccessibilitySettings for backwards compatibility
        auto& settings = TowerForge::Core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::ApplySettings() {
        // Save to unified UserPreferences
        auto& prefs = TowerForge::Core::UserPreferences::GetInstance();
        prefs.SetHighContrastEnabled(high_contrast_enabled_);
        prefs.SetFontScale(font_scale_);
        prefs.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
        
        // Also apply to legacy AccessibilitySettings for backwards compatibility
        auto& settings = TowerForge::Core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::Render() {
        RenderBackground();
        RenderHeader();
        RenderControls();
        RenderBackButton();
    }

    void AccessibilitySettingsMenu::RenderBackground() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent background overlay
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));

        // Main menu panel
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;

        DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, ColorAlpha(Color{30, 30, 40, 255}, 0.95f));
        DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, GOLD);
    }

    void AccessibilitySettingsMenu::RenderHeader() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;
        
        // Apply accessibility settings
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        const auto title = "ACCESSIBILITY SETTINGS";
        const int scaled_font_size = static_cast<int>(26 * font_scale);
        const int title_width = MeasureText(title, scaled_font_size);
        const Color title_color = high_contrast ? YELLOW : GOLD;
        DrawText(title, (screen_width - title_width) / 2, menu_y + 30, scaled_font_size, title_color);

        // Underline
        const int line_width = title_width + 40;
        const int line_x = (screen_width - line_width) / 2;
        DrawRectangle(line_x, menu_y + 65, line_width, 2, title_color);
    }

    void AccessibilitySettingsMenu::RenderControls() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;
        
        // Get current accessibility settings for rendering
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        // High-contrast mode checkbox
        RenderCheckbox("High-Contrast Mode", high_contrast_enabled_, 
                      menu_y + ITEMS_START_Y, selected_option_ == 0);

        // Font scale slider
        RenderFontScaleSlider();

        // Keyboard navigation checkbox
        RenderCheckbox("Keyboard Navigation", keyboard_navigation_enabled_, 
                      menu_y + ITEMS_START_Y + (ITEM_HEIGHT + ITEM_SPACING) * 2, 
                      selected_option_ == 2);

        // Instructions at bottom with font scaling
        const auto instruction = "UP/DOWN: Navigate | LEFT/RIGHT or SPACE: Adjust | ESC: Back";
        const int instruction_font_size = static_cast<int>(14 * font_scale);
        const int instruction_width = MeasureText(instruction, instruction_font_size);
        const Color instruction_color = high_contrast ? WHITE : LIGHTGRAY;
        DrawText(instruction, (screen_width - instruction_width) / 2,
                 menu_y + MENU_HEIGHT - 30, instruction_font_size, instruction_color);
    }

    void AccessibilitySettingsMenu::RenderCheckbox(const char* label, const bool checked, 
                                                   const int y_pos, const bool is_selected) const {
        const int screen_width = GetScreenWidth();
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        
        // Get current accessibility settings for rendering
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        const int checkbox_size = 24;
        const int checkbox_x = menu_x + 50;
        const int label_x = checkbox_x + checkbox_size + 15;

        // Highlight if selected
        if (is_selected) {
            const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
            const Color highlight_color = high_contrast ? YELLOW : GOLD;
            DrawRectangle(menu_x + 20, y_pos - 5, MENU_WIDTH - 40, ITEM_HEIGHT, 
                         ColorAlpha(highlight_color, 0.2f * pulse));
            DrawRectangleLines(menu_x + 20, y_pos - 5, MENU_WIDTH - 40, ITEM_HEIGHT, highlight_color);
        }

        // Draw checkbox
        const Color box_color = is_selected ? (high_contrast ? YELLOW : GOLD) : (high_contrast ? WHITE : GRAY);
        DrawRectangleLines(checkbox_x, y_pos + 10, checkbox_size, checkbox_size, box_color);
        
        if (checked) {
            DrawRectangle(checkbox_x + 4, y_pos + 14, checkbox_size - 8, checkbox_size - 8, box_color);
        }

        // Draw label with font scaling
        const Color text_color = is_selected ? (high_contrast ? BLACK : GOLD) : (high_contrast ? WHITE : LIGHTGRAY);
        const int label_font_size = static_cast<int>(20 * font_scale);
        DrawText(label, label_x, y_pos + 12, label_font_size, text_color);
    }

    void AccessibilitySettingsMenu::RenderFontScaleSlider() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;
        const int menu_x = (screen_width - MENU_WIDTH) / 2;

        const int y_pos = menu_y + ITEMS_START_Y + (ITEM_HEIGHT + ITEM_SPACING);
        const bool is_selected = (selected_option_ == 1);

        // Highlight if selected
        if (is_selected) {
            const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
            DrawRectangle(menu_x + 20, y_pos - 5, MENU_WIDTH - 40, ITEM_HEIGHT, 
                         ColorAlpha(GOLD, 0.2f * pulse));
            DrawRectangleLines(menu_x + 20, y_pos - 5, MENU_WIDTH - 40, ITEM_HEIGHT, GOLD);
        }

        // Draw label
        const Color text_color = is_selected ? GOLD : LIGHTGRAY;
        DrawText("Font Scale", menu_x + 50, y_pos + 5, 20, text_color);

        // Draw slider
        const int slider_width = 300;
        const int slider_x = menu_x + 50;
        const int slider_y = y_pos + 30;
        const int slider_height = 6;

        const Color slider_color = is_selected ? GOLD : GRAY;
        DrawRectangle(slider_x, slider_y, slider_width, slider_height, ColorAlpha(slider_color, 0.3f));

        // Draw slider thumb
        const float normalized_value = (font_scale_ - 0.5f) / (3.0f - 0.5f);
        const int thumb_x = slider_x + static_cast<int>(normalized_value * slider_width);
        const int thumb_size = 16;
        DrawCircle(thumb_x, slider_y + slider_height / 2, thumb_size / 2, slider_color);

        // Draw value text
        const std::string value_text = std::to_string(static_cast<int>(font_scale_ * 100)) + "%";
        DrawText(value_text.c_str(), slider_x + slider_width + 15, y_pos + 5, 20, text_color);
    }

    void AccessibilitySettingsMenu::RenderBackButton() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;

        const int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;
        const int button_y = menu_y + BACK_BUTTON_Y;

        const bool is_selected = (selected_option_ == 3);
        const Color button_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
        const Color border_color = is_selected ? GOLD : GRAY;
        const Color text_color = is_selected ? GOLD : LIGHTGRAY;

        DrawRectangle(button_x, button_y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, button_color);
        DrawRectangleLines(button_x, button_y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, border_color);

        const auto back_text = "Back";
        const int text_width = MeasureText(back_text, 20);
        DrawText(back_text, button_x + (BACK_BUTTON_WIDTH - text_width) / 2,
                 button_y + (BACK_BUTTON_HEIGHT - 20) / 2, 20, text_color);
    }

    bool AccessibilitySettingsMenu::HandleKeyboard() {
        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selected_option_--;
            if (selected_option_ < 0) {
                selected_option_ = 3; // Wrap to back button
            }
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selected_option_++;
            if (selected_option_ > 3) {
                selected_option_ = 0; // Wrap to first option
            }
        }

        // Adjust values with left/right or space
        if (selected_option_ == 0) { // High-contrast toggle
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || 
                IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
                high_contrast_enabled_ = !high_contrast_enabled_;
                ApplySettings();
            }
        } else if (selected_option_ == 1) { // Font scale slider
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                font_scale_ = std::max(0.5f, font_scale_ - 0.1f);
                ApplySettings();
            } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                font_scale_ = std::min(3.0f, font_scale_ + 0.1f);
                ApplySettings();
            }
        } else if (selected_option_ == 2) { // Keyboard navigation toggle
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || 
                IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
                keyboard_navigation_enabled_ = !keyboard_navigation_enabled_;
                ApplySettings();
            }
        } else if (selected_option_ == 3) { // Back button
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                return true;
            }
        }

        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE)) {
            return true;
        }

        return false;
    }

    bool AccessibilitySettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;
        const int menu_x = (screen_width - MENU_WIDTH) / 2;

        // Check back button
        const int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;
        const int button_y = menu_y + BACK_BUTTON_Y;
        if (mouse_x >= button_x && mouse_x <= button_x + BACK_BUTTON_WIDTH &&
            mouse_y >= button_y && mouse_y <= button_y + BACK_BUTTON_HEIGHT) {
            selected_option_ = 3;
            if (clicked) {
                return true;
            }
        }

        // Check high-contrast checkbox (option 0)
        const int checkbox_y = menu_y + ITEMS_START_Y;
        if (mouse_x >= menu_x + 20 && mouse_x <= menu_x + MENU_WIDTH - 20 &&
            mouse_y >= checkbox_y - 5 && mouse_y <= checkbox_y - 5 + ITEM_HEIGHT) {
            selected_option_ = 0;
            if (clicked) {
                high_contrast_enabled_ = !high_contrast_enabled_;
                ApplySettings();
            }
        }

        // Check font scale slider (option 1)
        const int slider_y = menu_y + ITEMS_START_Y + (ITEM_HEIGHT + ITEM_SPACING);
        if (mouse_x >= menu_x + 20 && mouse_x <= menu_x + MENU_WIDTH - 20 &&
            mouse_y >= slider_y - 5 && mouse_y <= slider_y - 5 + ITEM_HEIGHT) {
            selected_option_ = 1;
            
            // Handle slider dragging
            if (clicked || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                const int slider_x = menu_x + 50;
                const int slider_width = 300;
                const float normalized_x = std::clamp(
                    static_cast<float>(mouse_x - slider_x) / slider_width, 
                    0.0f, 1.0f
                );
                font_scale_ = 0.5f + normalized_x * (3.0f - 0.5f);
                ApplySettings();
            }
        }

        // Check keyboard navigation checkbox (option 2)
        const int keyboard_nav_y = menu_y + ITEMS_START_Y + (ITEM_HEIGHT + ITEM_SPACING) * 2;
        if (mouse_x >= menu_x + 20 && mouse_x <= menu_x + MENU_WIDTH - 20 &&
            mouse_y >= keyboard_nav_y - 5 && mouse_y <= keyboard_nav_y - 5 + ITEM_HEIGHT) {
            selected_option_ = 2;
            if (clicked) {
                keyboard_navigation_enabled_ = !keyboard_navigation_enabled_;
                ApplySettings();
            }
        }

        return false;
    }

} // namespace towerforge::ui
