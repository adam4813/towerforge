#include "ui/accessibility_settings_menu.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "core/accessibility_settings.hpp"
#include "core/user_preferences.hpp"

import engine;

namespace towerforge::ui {
    AccessibilitySettingsMenu::AccessibilitySettingsMenu()
        : back_callback_(nullptr)
          , high_contrast_checkbox_(nullptr)
          , font_scale_slider_(nullptr)
          , keyboard_nav_checkbox_(nullptr)
          , back_button_(nullptr)
          , selected_option_(0)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , high_contrast_enabled_(false)
          , font_scale_(1.0f)
          , keyboard_navigation_enabled_(true) {
        // Load settings from preferences
        auto &prefs = towerforge::core::UserPreferences::GetInstance();
        high_contrast_enabled_ = prefs.IsHighContrastEnabled();
        font_scale_ = prefs.GetFontScale();
        keyboard_navigation_enabled_ = prefs.IsKeyboardNavigationEnabled();
    }

    AccessibilitySettingsMenu::~AccessibilitySettingsMenu() = default;

    void AccessibilitySettingsMenu::Initialize() {
        using namespace engine::ui::components;

        // Create main panel with vertical layout
        engine::ui::BatchRenderer::EndFrame();
        settings_panel_ = std::make_unique<engine::ui::elements::Panel>();
        settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;
        settings_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));

        settings_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        settings_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        settings_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        settings_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Add title text
        auto title_text = std::make_unique<engine::ui::elements::Text>(
            0, 0,
            "Accessibility Settings",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
        );
        settings_panel_->AddChild(std::move(title_text));

        // Add divider below title
        auto divider = std::make_unique<engine::ui::elements::Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(MENU_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        settings_panel_->AddChild(std::move(divider));

        // Calculate content width
        constexpr float content_width = MENU_WIDTH - UITheme::PADDING_LARGE * 2;

        // Create controls container with vertical layout
        auto controls_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(content_width, ITEM_HEIGHT * 3 + ITEM_SPACING * 2)
                .Layout(std::make_unique<VerticalLayout>(ITEM_SPACING, Alignment::Stretch))
                .Build();

        // High Contrast Checkbox
        auto high_contrast = std::make_unique<engine::ui::elements::Checkbox>(
            "High Contrast Mode"
        );
        high_contrast->SetSize(high_contrast->GetWidth(), ITEM_HEIGHT);
        high_contrast->SetChecked(high_contrast_enabled_);
        high_contrast->SetToggleCallback([this](const bool checked) {
            high_contrast_enabled_ = checked;
            ApplySettings();
        });
        high_contrast_checkbox_ = high_contrast.get();
        controls_container->AddChild(std::move(high_contrast));

        // Font Scale Slider
        auto font_slider = std::make_unique<engine::ui::elements::Slider>(
            content_width, ITEM_HEIGHT,
            0.8f, 1.5f, font_scale_
        );
        font_slider->SetLabel("Font Scale");
        font_slider->SetShowValue(true);
        font_slider->SetValueChangedCallback([this](const float value) {
            font_scale_ = value;
            ApplySettings();
        });
        font_scale_slider_ = font_slider.get();
        controls_container->AddChild(std::move(font_slider));

        // Keyboard Navigation Checkbox
        auto keyboard_nav = std::make_unique<engine::ui::elements::Checkbox>(
            "Keyboard Navigation"
        );
        keyboard_nav->SetSize(keyboard_nav->GetWidth(), ITEM_HEIGHT);
        keyboard_nav->SetChecked(keyboard_navigation_enabled_);
        keyboard_nav->SetToggleCallback([this](const bool checked) {
            keyboard_navigation_enabled_ = checked;
            ApplySettings();
        });
        keyboard_nav_checkbox_ = keyboard_nav.get();
        controls_container->AddChild(std::move(keyboard_nav));

        settings_panel_->AddChild(std::move(controls_container));

        // Back Button
        constexpr float button_width = 150.0f;
        auto back_btn = std::make_unique<engine::ui::elements::Button>(
            button_width, UITheme::BUTTON_HEIGHT_LARGE,
            "Back", UITheme::FONT_SIZE_LARGE
        );
        back_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        back_btn->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        back_btn->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        back_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        back_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && back_callback_) {
                back_callback_();
                return true;
            }
            return false;
        });
        back_button_ = back_btn.get();
        settings_panel_->AddChild(std::move(back_btn));

        // Calculate initial layout
        UpdateLayout();
    }

    void AccessibilitySettingsMenu::Shutdown() {
        settings_panel_.reset();
        high_contrast_checkbox_ = nullptr;
        font_scale_slider_ = nullptr;
        keyboard_nav_checkbox_ = nullptr;
        back_button_ = nullptr;
    }

    void AccessibilitySettingsMenu::UpdateLayout() {
        last_screen_width_ = GetScreenWidth();
        last_screen_height_ = GetScreenHeight();

        if (!settings_panel_) {
            return;
        }

        const int panel_x = (last_screen_width_ - MENU_WIDTH) / 2;
        const int panel_y = (last_screen_height_ - MENU_HEIGHT) / 2;

        settings_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));
        settings_panel_->InvalidateComponents();
        settings_panel_->UpdateComponentsRecursive();
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
        const auto &prefs = towerforge::core::UserPreferences::GetInstance();
        high_contrast_enabled_ = prefs.IsHighContrastEnabled();
        font_scale_ = prefs.GetFontScale();
        keyboard_navigation_enabled_ = prefs.IsKeyboardNavigationEnabled();

        // Update UI components
        if (high_contrast_checkbox_) high_contrast_checkbox_->SetChecked(high_contrast_enabled_);
        if (font_scale_slider_) font_scale_slider_->SetValue(font_scale_);
        if (keyboard_nav_checkbox_) keyboard_nav_checkbox_->SetChecked(keyboard_navigation_enabled_);

        // Also sync with legacy AccessibilitySettings
        auto &settings = towerforge::core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::ApplySettings() const {
        // Save to unified UserPreferences
        auto &prefs = towerforge::core::UserPreferences::GetInstance();
        prefs.SetHighContrastEnabled(high_contrast_enabled_);
        prefs.SetFontScale(font_scale_);
        prefs.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);

        // Also apply to legacy AccessibilitySettings
        auto &settings = towerforge::core::AccessibilitySettings::GetInstance();
        settings.SetHighContrastEnabled(high_contrast_enabled_);
        settings.SetFontScale(font_scale_);
        settings.SetKeyboardNavigationEnabled(keyboard_navigation_enabled_);
    }

    void AccessibilitySettingsMenu::Render() const {
        RenderDimOverlay();
        settings_panel_->Render();
    }

    void AccessibilitySettingsMenu::RenderDimOverlay() {
        // Dim the background
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        engine::ui::BatchRenderer::SubmitQuad(engine::ui::Rectangle(0, 0, screen_width, screen_height),
                                              UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
    }

    bool AccessibilitySettingsMenu::ProcessMouseEvent(const MouseEvent &event) const {
        // Process through panel (handles all children)
        return settings_panel_->ProcessMouseEvent({
            event.x,
            event.y,
            event.left_down,
            event.right_down,
            event.left_pressed,
            event.right_pressed
        });
    }

    void AccessibilitySettingsMenu::HandleKeyboard() const {
        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (back_callback_) {
                back_callback_();
            }
        }
    }
} // namespace towerforge::ui
