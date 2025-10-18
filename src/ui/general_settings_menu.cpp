#include "ui/general_settings_menu.h"
#include "ui/ui_element.h"
#include "core/accessibility_settings.hpp"
#include <cmath>

namespace towerforge::ui {

    GeneralSettingsMenu::GeneralSettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , selected_menu_option_(-1) {

        // Initialize menu items
        menu_items_.push_back({"Audio Settings  >", SettingsOption::Audio});
        menu_items_.push_back({"Controls Settings  >", SettingsOption::Controls});
        menu_items_.push_back({"Display Settings  >", SettingsOption::Display});
        menu_items_.push_back({"Accessibility  >", SettingsOption::Accessibility});
        menu_items_.push_back({"Gameplay Settings  >", SettingsOption::Gameplay});
        menu_items_.push_back({"Back", SettingsOption::Back});

        // Create main panel
        settings_panel_ = std::make_unique<Panel>(0, 0, 800, 600, BLANK, BLANK);

        // Create Button objects for each menu item and add as children
        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            auto button = std::make_unique<Button>(
                0, // x will be set during render
                static_cast<float>(item_y),
                static_cast<float>(MENU_WIDTH),
                static_cast<float>(MENU_ITEM_HEIGHT),
                menu_items_[i].label,
                ColorAlpha(DARKGRAY, 0.2f),
                DARKGRAY
            );
            button->SetFontSize(24);

            // Set click callback
            const int option_index = static_cast<int>(i);
            button->SetClickCallback([this, option_index]() {
                selected_menu_option_ = option_index;
            });

            // Store raw pointer for later access
            Button* button_ptr = button.get();
            menu_item_buttons_.push_back(button_ptr);

            // Add as child to panel
            settings_panel_->AddChild(std::move(button));
        }
    }

    GeneralSettingsMenu::~GeneralSettingsMenu() = default;

    void GeneralSettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
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
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        
        // Apply accessibility settings
        const auto& accessibility = TowerForge::Core::AccessibilitySettings::GetInstance();
        const float font_scale = accessibility.GetFontScale();
        const bool high_contrast = accessibility.IsHighContrastEnabled();

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            const int item_x = (screen_width - MENU_WIDTH) / 2;

            const bool is_selected = (static_cast<int>(i) == selected_option_);

            // Update button position and appearance based on selection
            auto& button = menu_item_buttons_[i];
            button->SetRelativePosition(static_cast<float>(item_x), static_cast<float>(item_y));
            
            // Apply high-contrast colors if enabled
            Color bg_color, border_color, text_color;
            if (high_contrast) {
                bg_color = is_selected ? ColorAlpha(YELLOW, 0.5f) : ColorAlpha(WHITE, 0.2f);
                border_color = is_selected ? YELLOW : WHITE;
                text_color = is_selected ? BLACK : WHITE;
            } else {
                bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
                border_color = is_selected ? GOLD : GRAY;
                text_color = is_selected ? GOLD : LIGHTGRAY;
            }
            
            button->SetBackgroundColor(bg_color);
            button->SetBorderColor(border_color);
            button->SetTextColor(text_color);

            // Render the button
            button->Render();

            // Draw selection indicator
            if (is_selected) {
                const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
                const int indicator_x = item_x - 30;
                const int indicator_y = item_y + MENU_ITEM_HEIGHT / 2;
                const int indicator_font_size = static_cast<int>(24 * font_scale);
                const Color indicator_color = high_contrast ? YELLOW : GOLD;
                DrawText(">", indicator_x, indicator_y - indicator_font_size / 2, 
                        indicator_font_size, ColorAlpha(indicator_color, pulse));
            }
        }

        // Draw instruction at bottom
        const auto instruction = "UP/DOWN: Navigate | ENTER: Select | ESC: Back";
        const int instruction_font_size = static_cast<int>(16 * font_scale);
        const int instruction_width = MeasureText(instruction, instruction_font_size);
        const Color instruction_color = high_contrast ? WHITE : LIGHTGRAY;
        DrawText(instruction, (screen_width - instruction_width) / 2,
                 screen_height - 50, instruction_font_size, instruction_color);
    }

    int GeneralSettingsMenu::HandleKeyboard() {
        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selected_option_--;
            if (selected_option_ < 0) {
                selected_option_ = static_cast<int>(menu_items_.size()) - 1;
            }
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selected_option_++;
            if (selected_option_ >= static_cast<int>(menu_items_.size())) {
                selected_option_ = 0;
            }
        }

        // Select option
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            return selected_option_;
        }

        // ESC to go back (same as Back button)
        if (IsKeyPressed(KEY_ESCAPE)) {
            return static_cast<int>(SettingsOption::Back);
        }

        return -1;
    }

    int GeneralSettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        // Create mouse event
        MouseEvent event(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            false, // left_down
            false, // right_down
            clicked, // left_pressed
            false  // right_pressed
        );

        // Reset selected menu option
        selected_menu_option_ = -1;

        // Process mouse event through the panel
        settings_panel_->ProcessMouseEvent(event);

        // Update selected_option_ based on which button is hovered
        for (size_t i = 0; i < menu_item_buttons_.size(); ++i) {
            if (menu_item_buttons_[i]->IsHovered()) {
                selected_option_ = static_cast<int>(i);
                break;
            }
        }

        return selected_menu_option_;
    }

}
