#include "ui/accessibility_settings_menu.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/panel_header_overlay.h"
#include "ui/dim_overlay.h"
#include "ui/mouse_interface.h"
#include "core/accessibility_settings.hpp"
#include "core/user_preferences.hpp"

namespace towerforge::ui {

    AccessibilitySettingsMenu::AccessibilitySettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , high_contrast_enabled_(false)
          , font_scale_(1.0f)
          , keyboard_navigation_enabled_(true)
          , back_callback_(nullptr) {
        
        // Create main panel centered on screen
        const Color background_color = ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f);
        settings_panel_ = std::make_unique<Panel>(0, 0, MENU_WIDTH, 550, background_color, UITheme::BORDER_ACCENT);
        settings_panel_->SetPadding(UITheme::PADDING_MEDIUM);
        
        // Create header overlay
        header_overlay_ = std::make_unique<PanelHeaderOverlay>("Accessibility Settings");
        
        // Create dim overlay
        dim_overlay_ = std::make_unique<DimOverlay>();

        // Load settings from preferences first (don't sync to UI yet - components don't exist)
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        high_contrast_enabled_ = prefs.IsHighContrastEnabled();
        font_scale_ = prefs.GetFontScale();
        keyboard_navigation_enabled_ = prefs.IsKeyboardNavigationEnabled();

        // Position items relative to panel (0-based, panel handles absolute positioning)
        // Start below header which takes ~80-100px (title + underline + spacing)
        int y_pos = CONTENT_START_Y;
        constexpr int content_x = 40;  // Left margin for content (away from border)

        // High Contrast Checkbox
        auto high_contrast = std::make_unique<Checkbox>(
            static_cast<float>(content_x),
            static_cast<float>(y_pos),
            "High Contrast Mode"
        );
        high_contrast->SetChecked(high_contrast_enabled_);
        high_contrast->SetToggleCallback([this](bool checked) {
            high_contrast_enabled_ = checked;
            ApplySettings();
        });
        high_contrast_checkbox_ = high_contrast.get();
        interactive_elements_.push_back(high_contrast_checkbox_);
        settings_panel_->AddChild(std::move(high_contrast));

        y_pos += ITEM_HEIGHT + ITEM_SPACING;

        // Font Scale Slider
        auto font_slider = std::make_unique<Slider>(
            static_cast<float>(content_x),
            static_cast<float>(y_pos),
            SLIDER_WIDTH,
            30,
            0.8f, 1.5f,
            "Font Scale"
        );
        font_slider->SetValue(font_scale_);
        font_slider->SetValueChangedCallback([this](float value) {
            font_scale_ = value;
            ApplySettings();
        });
        font_scale_slider_ = font_slider.get();
        interactive_elements_.push_back(font_scale_slider_);
        settings_panel_->AddChild(std::move(font_slider));

        y_pos += ITEM_HEIGHT + ITEM_SPACING;

        // Keyboard Navigation Checkbox
        auto keyboard_nav = std::make_unique<Checkbox>(
            static_cast<float>(content_x),
            static_cast<float>(y_pos),
            "Keyboard Navigation"
        );
        keyboard_nav->SetChecked(keyboard_navigation_enabled_);
        keyboard_nav->SetToggleCallback([this](bool checked) {
            keyboard_navigation_enabled_ = checked;
            ApplySettings();
        });
        keyboard_nav_checkbox_ = keyboard_nav.get();
        interactive_elements_.push_back(keyboard_nav_checkbox_);
        settings_panel_->AddChild(std::move(keyboard_nav));

        y_pos += ITEM_HEIGHT + ITEM_SPACING * 3;

        // Back Button
        auto back_btn = std::make_unique<Button>(
            static_cast<float>(content_x),
            static_cast<float>(y_pos),
            150,
            40,
            "Back",
            UITheme::BUTTON_BACKGROUND,
            UITheme::BUTTON_BORDER
        );
        back_btn->SetClickCallback([this]() {
            if (back_callback_) {
                back_callback_();
            }
        });
        back_button_ = back_btn.get();
        interactive_elements_.push_back(back_button_);
        settings_panel_->AddChild(std::move(back_btn));

        // Calculate initial layout
        UpdateLayout();

        // Set initial selection appearance
        UpdateSelection(selected_option_);
    }

    AccessibilitySettingsMenu::~AccessibilitySettingsMenu() = default;

    void AccessibilitySettingsMenu::UpdateLayout() {
        last_screen_width_ = GetScreenWidth();
        last_screen_height_ = GetScreenHeight();

        // Center the panel
        const int panel_x = (last_screen_width_ - MENU_WIDTH) / 2;
        const int panel_y = (last_screen_height_ - 550) / 2;
        
        settings_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), 550.0f);
    }

    void AccessibilitySettingsMenu::UpdateSelection(const int new_selection) {
        const auto& accessibility = towerforge::core::AccessibilitySettings::GetInstance();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        // Clear old selection
        if (selected_option_ >= 0 && selected_option_ < static_cast<int>(interactive_elements_.size())) {
            interactive_elements_[selected_option_]->SetFocused(false);
        }

        // Set new selection
        if (new_selection >= 0 && new_selection < static_cast<int>(interactive_elements_.size())) {
            interactive_elements_[new_selection]->SetFocused(true);
        }

        selected_option_ = new_selection;
    }

    void AccessibilitySettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void AccessibilitySettingsMenu::SyncWithSettings() {
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        high_contrast_enabled_ = prefs.IsHighContrastEnabled();
        font_scale_ = prefs.GetFontScale();
        keyboard_navigation_enabled_ = prefs.IsKeyboardNavigationEnabled();
        
        // Update UI components
        if (high_contrast_checkbox_) high_contrast_checkbox_->SetChecked(high_contrast_enabled_);
        if (font_scale_slider_) font_scale_slider_->SetValue(font_scale_);
        if (keyboard_nav_checkbox_) keyboard_nav_checkbox_->SetChecked(keyboard_navigation_enabled_);
        
        // Also sync with legacy AccessibilitySettings
        auto& settings = towerforge::core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::ApplySettings() const {
        // Save to unified UserPreferences
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        prefs.SetHighContrastEnabled(high_contrast_enabled_);
        prefs.SetFontScale(font_scale_);
        prefs.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
        
        // Also apply to legacy AccessibilitySettings
        auto& settings = towerforge::core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::Render() const {
        // Render dim overlay
        dim_overlay_->Render();

        // Render main panel (includes all children automatically)
        settings_panel_->Render();

        // Render header overlay on top
        const Rectangle panel_bounds = settings_panel_->GetAbsoluteBounds();
        header_overlay_->Render(
            static_cast<int>(panel_bounds.x),
            static_cast<int>(panel_bounds.y),
            MENU_WIDTH
        );
    }

    bool AccessibilitySettingsMenu::ProcessMouseEvent(const MouseEvent& event) {
        // Process through panel (handles all children)
        const bool consumed = settings_panel_->ProcessMouseEvent(event);

        // Update selection based on hover
        for (size_t i = 0; i < interactive_elements_.size(); ++i) {
            if (interactive_elements_[i]->IsHovered()) {
                UpdateSelection(static_cast<int>(i));
                break;
            }
        }

        return consumed;
    }

    bool AccessibilitySettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        // Legacy wrapper - delegates to modern API
        const MouseEvent event(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            false,
            false,
            clicked,
            false
        );
        ProcessMouseEvent(event);

        // Legacy API expects bool return for "back button clicked"
        // With callbacks, we don't track this, so return false
        return false;
    }

    bool AccessibilitySettingsMenu::HandleKeyboard() {
        int new_selection = selected_option_;

        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            new_selection--;
            if (new_selection < 0) {
                new_selection = static_cast<int>(interactive_elements_.size()) - 1;
            }
            UpdateSelection(new_selection);
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            new_selection++;
            if (new_selection >= static_cast<int>(interactive_elements_.size())) {
                new_selection = 0;
            }
            UpdateSelection(new_selection);
        }

        // Activate selected item
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (selected_option_ >= 0 && selected_option_ < static_cast<int>(interactive_elements_.size())) {
                auto* elem = interactive_elements_[selected_option_];
                
                // Create a click event
                const MouseEvent click_event(0, 0, false, false, true, false);
                elem->OnClick(click_event);
            }
        }

        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (back_callback_) {
                back_callback_();
            }
            return true;
        }

        return false;
    }

} // namespace towerforge::ui
