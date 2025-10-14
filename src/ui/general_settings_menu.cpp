#include "ui/general_settings_menu.h"
#include <cmath>

namespace towerforge::ui {

    GeneralSettingsMenu::GeneralSettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f) {

        // Initialize menu items
        menu_items_.push_back({"Audio Settings  >", SettingsOption::Audio});
        menu_items_.push_back({"Controls Settings  >", SettingsOption::Controls});
        menu_items_.push_back({"Display Settings  >", SettingsOption::Display});
        menu_items_.push_back({"Accessibility  >", SettingsOption::Accessibility});
        menu_items_.push_back({"Gameplay Settings  >", SettingsOption::Gameplay});
        menu_items_.push_back({"Back", SettingsOption::Back});
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

        const auto title = "GENERAL SETTINGS MENU";
        const int title_width = MeasureText(title, 28);
        DrawText(title, (screen_width - title_width) / 2, HEADER_Y, 28, GOLD);

        // Underline
        const int line_width = title_width + 40;
        const int line_x = (screen_width - line_width) / 2;
        DrawRectangle(line_x, HEADER_Y + 35, line_width, 2, GOLD);
    }

    void GeneralSettingsMenu::RenderMenuOptions() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            const int item_x = (screen_width - MENU_WIDTH) / 2;

            const bool is_selected = (static_cast<int>(i) == selected_option_);

            // Draw menu item background
            const Color bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
            DrawRectangle(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, bg_color);

            // Draw menu item border
            const Color border_color = is_selected ? GOLD : GRAY;
            int border_thickness = is_selected ? 3 : 2;
            DrawRectangleLines(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, border_color);

            // Draw menu item text
            const char* text = menu_items_[i].label.c_str();
            int text_width = MeasureText(text, 20);
            const Color text_color = is_selected ? GOLD : LIGHTGRAY;
            DrawText(text, item_x + 20, item_y + (MENU_ITEM_HEIGHT - 20) / 2, 20, text_color);

            // Draw selection indicator
            if (is_selected) {
                const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
                const int indicator_x = item_x - 30;
                const int indicator_y = item_y + MENU_ITEM_HEIGHT / 2;
                DrawText(">", indicator_x, indicator_y - 12, 24, ColorAlpha(GOLD, pulse));
            }
        }

        // Draw instruction at bottom
        const auto instruction = "UP/DOWN: Navigate | ENTER: Select | ESC: Back";
        const int instruction_width = MeasureText(instruction, 16);
        DrawText(instruction, (screen_width - instruction_width) / 2,
                 screen_height - 50, 16, LIGHTGRAY);
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
        const int screen_width = GetScreenWidth();

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            const int item_x = (screen_width - MENU_WIDTH) / 2;

            // Check if mouse is over this item
            if (mouse_x >= item_x && mouse_x <= item_x + MENU_WIDTH &&
                mouse_y >= item_y && mouse_y <= item_y + MENU_ITEM_HEIGHT) {
                selected_option_ = static_cast<int>(i);

                if (clicked) {
                    return selected_option_;
                }
            }
        }

        return -1;
    }

}
