#include "ui/general_settings_menu.h"
#include "ui/ui_element.h"
#include "core/accessibility_settings.hpp"
#include <cmath>

namespace towerforge::ui {
    GeneralSettingsMenu::GeneralSettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , option_callback_(nullptr) {

        // Initialize menu items with their options
        menu_items_.push_back({"Audio Settings  >", SettingsOption::Audio});
        menu_items_.push_back({"Controls Settings  >", SettingsOption::Controls});
        menu_items_.push_back({"Display Settings  >", SettingsOption::Display});
        menu_items_.push_back({"Accessibility  >", SettingsOption::Accessibility});
        menu_items_.push_back({"Gameplay Settings  >", SettingsOption::Gameplay});
        menu_items_.push_back({"Back", SettingsOption::Back});

        // Create main panel centered on screen
        settings_panel_ = std::make_unique<Panel>(0, 0, MENU_WIDTH, 600, BLANK, BLANK);

        // Create Button objects for each menu item
        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            
            auto button = std::make_unique<Button>(
                0, // X relative to panel
                static_cast<float>(item_y),
                static_cast<float>(MENU_WIDTH),
                static_cast<float>(MENU_ITEM_HEIGHT),
                menu_items_[i].label,
                ColorAlpha(DARKGRAY, 0.2f),
                DARKGRAY
            );
            button->SetFontSize(24);
            button->SetTextColor(LIGHTGRAY);

            // Set click callback - button triggers option directly
            const SettingsOption option = menu_items_[i].option;
            button->SetClickCallback([this, option]() {
                if (option_callback_) {
                    option_callback_(option);
                }
            });

            // Store raw pointer
            Button* button_ptr = button.get();
            menu_item_buttons_.push_back(button_ptr);

            // Add as child to panel
            settings_panel_->AddChild(std::move(button));
        }
        
        // Calculate initial layout
        UpdateLayout();
        
        // Set initial selection appearance
        UpdateButtonSelection(selected_option_);
    }

    GeneralSettingsMenu::~GeneralSettingsMenu() = default;

    void GeneralSettingsMenu::SetOptionCallback(OptionCallback callback) {
        option_callback_ = callback;
    }

    void GeneralSettingsMenu::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        
        // Center the panel horizontally
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        settings_panel_->SetRelativePosition(static_cast<float>(panel_x), 0);
        settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(screen_height));
        
        // Buttons automatically positioned via panel (no need to update)
        
        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void GeneralSettingsMenu::UpdateButtonSelection(int new_selection) {
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const bool high_contrast = accessibility.IsHighContrastEnabled();
        
        // Clear old selection
        if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
            Button* old_button = menu_item_buttons_[selected_option_];
            old_button->SetFocused(false);
            
            if (high_contrast) {
                old_button->SetBackgroundColor(ColorAlpha(WHITE, 0.2f));
                old_button->SetBorderColor(WHITE);
                old_button->SetTextColor(WHITE);
            } else {
                old_button->SetBackgroundColor(ColorAlpha(DARKGRAY, 0.3f));
                old_button->SetBorderColor(GRAY);
                old_button->SetTextColor(LIGHTGRAY);
            }
        }
        
        // Set new selection
        if (new_selection >= 0 && new_selection < static_cast<int>(menu_item_buttons_.size())) {
            Button* new_button = menu_item_buttons_[new_selection];
            new_button->SetFocused(true);
            
            if (high_contrast) {
                new_button->SetBackgroundColor(ColorAlpha(YELLOW, 0.5f));
                new_button->SetBorderColor(YELLOW);
                new_button->SetTextColor(BLACK);
            } else {
                new_button->SetBackgroundColor(ColorAlpha(GOLD, 0.3f));
                new_button->SetBorderColor(GOLD);
                new_button->SetTextColor(GOLD);
            }
        }
        
        selected_option_ = new_selection;
    }

    void GeneralSettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
        
        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
        
        // Update buttons
        for (Button* button : menu_item_buttons_) {
            button->Update(delta_time);
        }
    }

    void GeneralSettingsMenu::Render() const {
        RenderBackground();
        RenderHeader();
        RenderMenuOptions();
    }

    void GeneralSettingsMenu::RenderBackground() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent background overlay
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));

        // Main menu panel
        const int menu_height = MENU_START_Y + menu_items_.size() * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) + 80;
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        constexpr int menu_y = HEADER_Y - 20;

        DrawRectangle(menu_x, menu_y, MENU_WIDTH, menu_height, ColorAlpha(Color{30, 30, 40, 255}, 0.95f));
        DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, menu_height, GOLD);
    }

    void GeneralSettingsMenu::RenderHeader() {
        const int screen_width = GetScreenWidth();
        
        // Apply font scaling from accessibility settings
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        const auto title = "GENERAL SETTINGS MENU";
        const int scaled_font_size = static_cast<int>(28 * font_scale);
        const int title_width = MeasureText(title, scaled_font_size);
        const Color title_color = high_contrast ? YELLOW : GOLD;
        DrawText(title, (screen_width - title_width) / 2, HEADER_Y, scaled_font_size, title_color);

        // Underline
        const int line_width = title_width + 40;
        const int line_x = (screen_width - line_width) / 2;
        DrawRectangle(line_x, HEADER_Y + 35, line_width, 2, title_color);
    }

    void GeneralSettingsMenu::RenderMenuOptions() const {
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        for (size_t i = 0; i < menu_item_buttons_.size(); ++i) {
            const bool is_selected = (static_cast<int>(i) == selected_option_);
            const Button* button = menu_item_buttons_[i];

            // Render the button (geometry and appearance already set)
            button->Render();

            // Draw selection indicator
            if (is_selected) {
                const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
                const Rectangle bounds = button->GetAbsoluteBounds();
                const int indicator_x = bounds.x - 30;
                const int indicator_y = bounds.y + bounds.height / 2;
                const int indicator_font_size = static_cast<int>(24 * font_scale);
                const Color indicator_color = high_contrast ? YELLOW : GOLD;
                DrawText(">", indicator_x, indicator_y - indicator_font_size / 2, 
                        indicator_font_size, ColorAlpha(indicator_color, pulse));
            }
        }

        // Draw instruction at bottom
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const auto instruction = "UP/DOWN: Navigate | ENTER: Select | ESC: Back";
        const int instruction_font_size = static_cast<int>(16 * font_scale);
        const int instruction_width = MeasureText(instruction, instruction_font_size);
        const Color instruction_color = high_contrast ? WHITE : LIGHTGRAY;
        DrawText(instruction, (screen_width - instruction_width) / 2,
                 screen_height - 50, instruction_font_size, instruction_color);
    }

    void GeneralSettingsMenu::HandleKeyboard() {
        int new_selection = selected_option_;
        
        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            new_selection--;
            if (new_selection < 0) {
                new_selection = static_cast<int>(menu_items_.size()) - 1;
            }
            UpdateButtonSelection(new_selection);
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            new_selection++;
            if (new_selection >= static_cast<int>(menu_items_.size())) {
                new_selection = 0;
            }
            UpdateButtonSelection(new_selection);
        }

        // Select option with Enter/Space
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_items_.size())) {
                const SettingsOption option = menu_items_[selected_option_].option;
                if (option_callback_) {
                    option_callback_(option);
                }
            }
        }

        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (option_callback_) {
                option_callback_(SettingsOption::Back);
            }
        }
    }

    void GeneralSettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        // Create mouse event
        const MouseEvent event(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            false, // left_down
            false, // right_down
            clicked, // left_pressed
            false  // right_pressed
        );

        // Process mouse event through the panel (buttons will handle clicks via callbacks)
        settings_panel_->ProcessMouseEvent(event);

        // Update hover selection for visual feedback
        for (size_t i = 0; i < menu_item_buttons_.size(); ++i) {
            if (menu_item_buttons_[i]->IsHovered()) {
                UpdateButtonSelection(static_cast<int>(i));
                break;
            }
        }
    }
}
