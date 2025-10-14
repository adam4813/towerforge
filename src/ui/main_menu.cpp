#include "ui/main_menu.h"
#include <cmath>

namespace towerforge::ui {

    MainMenu::MainMenu()
        : selected_option_(0)
          , animation_time_(0.0f) {

        // Initialize menu items
        menu_items_.push_back({"New Game", MenuOption::NewGame});
        menu_items_.push_back({"Tutorial", MenuOption::Tutorial});
        menu_items_.push_back({"Load Game", MenuOption::LoadGame});
        menu_items_.push_back({"Achievements", MenuOption::Achievements});
        menu_items_.push_back({"Settings", MenuOption::Settings});
        menu_items_.push_back({"Credits", MenuOption::Credits});
        menu_items_.push_back({"Quit", MenuOption::Quit});
    }

    MainMenu::~MainMenu() = default;

    void MainMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
    }

    void MainMenu::Render() const {
        RenderBackground();
        RenderTitle();
        RenderMenuOptions();
        RenderVersion();
    }

    void MainMenu::RenderBackground() const {
        // Clear with a gradient-like dark background
        ClearBackground(Color{20, 20, 30, 255});

        // Draw decorative elements - simplified tower silhouette
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Draw subtle grid pattern in background
        for (int i = 0; i < screen_height; i += 40) {
            DrawLine(0, i, screen_width, i, ColorAlpha(DARKGRAY, 0.1f));
        }
        for (int i = 0; i < screen_width; i += 40) {
            DrawLine(i, 0, i, screen_height, ColorAlpha(DARKGRAY, 0.1f));
        }

        // Draw simple tower silhouettes on the sides
        DrawRectangle(50, 300, 60, 300, ColorAlpha(DARKBLUE, 0.3f));
        DrawRectangle(screen_width - 110, 250, 60, 350, ColorAlpha(DARKBLUE, 0.3f));

        // Add some "windows" to the silhouettes
        for (int y = 320; y < 580; y += 30) {
            for (int x = 60; x < 100; x += 20) {
                const float pulse = sin(animation_time_ * 2.0f + y * 0.1f) * 0.5f + 0.5f;
                DrawRectangle(x, y, 10, 15, ColorAlpha(YELLOW, 0.2f + pulse * 0.1f));
            }
        }
        for (int y = 270; y < 580; y += 30) {
            for (int x = screen_width - 100; x < screen_width - 60; x += 20) {
                const float pulse = sin(animation_time_ * 2.0f + y * 0.1f + 1.0f) * 0.5f + 0.5f;
                DrawRectangle(x, y, 10, 15, ColorAlpha(YELLOW, 0.2f + pulse * 0.1f));
            }
        }
    }

    void MainMenu::RenderTitle() {
        const int screen_width = GetScreenWidth();

        // Draw main title
        const auto title = "TOWERFORGE";
        constexpr int title_font_size = 60;
        const int title_width = MeasureText(title, title_font_size);
        const int title_x = (screen_width - title_width) / 2;

        // Draw title shadow for depth
        DrawText(title, title_x + 3, TITLE_Y + 3, title_font_size, ColorAlpha(BLACK, 0.5f));

        // Draw title with gradient effect (simulated with multiple draws)
        DrawText(title, title_x, TITLE_Y, title_font_size, GOLD);

        // Draw tagline
        const auto tagline = "\"Build, Survive, Thrive!\"";
        constexpr int tagline_font_size = 20;
        const int tagline_width = MeasureText(tagline, tagline_font_size);
        const int tagline_x = (screen_width - tagline_width) / 2;

        DrawText(tagline, tagline_x, TITLE_Y + 70, tagline_font_size, LIGHTGRAY);

        // Draw decorative line
        constexpr int line_width = 400;
        const int line_x = (screen_width - line_width) / 2;
        DrawRectangle(line_x, TITLE_Y + 100, line_width, 2, GOLD);
    }

    void MainMenu::RenderMenuOptions() const {
        const int screen_width = GetScreenWidth();

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            const int item_x = (screen_width - MENU_WIDTH) / 2;

            const bool is_selected = (static_cast<int>(i) == selected_option_);

            // Draw menu item background
            const Color bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.2f);
            DrawRectangle(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, bg_color);

            // Draw menu item border
            const Color border_color = is_selected ? GOLD : GRAY;
            int border_thickness = is_selected ? 3 : 2;
            DrawRectangleLines(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, border_color);

            // Draw menu item text
            const char* text = menu_items_[i].label.c_str();
            const int font_size = is_selected ? 24 : 22;
            const int text_width = MeasureText(text, font_size);
            const int text_x = item_x + (MENU_WIDTH - text_width) / 2;
            const int text_y = item_y + (MENU_ITEM_HEIGHT - font_size) / 2;

            Color text_color = is_selected ? WHITE : LIGHTGRAY;

            // Add subtle animation to selected item
            if (is_selected) {
                const float pulse = sin(animation_time_ * 3.0f) * 0.1f + 0.9f;
                text_color = ColorAlpha(WHITE, pulse);
            }

            DrawText(text, text_x, text_y, font_size, text_color);

            // Draw selection indicator
            if (is_selected) {
                const int indicator_x = item_x - 30;
                const int indicator_y = item_y + MENU_ITEM_HEIGHT / 2;
                DrawText(">", indicator_x, indicator_y - 12, 24, GOLD);
            }
        }
    }

    void MainMenu::RenderVersion() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        const std::string version_text = "Version: " + GetVersion();
        constexpr int font_size = 16;
        const int text_width = MeasureText(version_text.c_str(), font_size);

        DrawText(version_text.c_str(), screen_width - text_width - 20,
                 screen_height - 30, font_size, GRAY);
    }

    int MainMenu::HandleKeyboard() {
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

        // Quick access keys
        if (IsKeyPressed(KEY_N)) {
            return static_cast<int>(MenuOption::NewGame);
        }
        if (IsKeyPressed(KEY_T)) {
            return static_cast<int>(MenuOption::Tutorial);
        }
        if (IsKeyPressed(KEY_L)) {
            return static_cast<int>(MenuOption::LoadGame);
        }
        if (IsKeyPressed(KEY_C)) {
            return static_cast<int>(MenuOption::Credits);
        }
        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
            return static_cast<int>(MenuOption::Quit);
        }

        return -1;
    }

    int MainMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        const int screen_width = GetScreenWidth();

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            const int item_x = (screen_width - MENU_WIDTH) / 2;

            // Check if mouse is over this item
            if (mouse_x >= item_x && mouse_x <= item_x + MENU_WIDTH &&
                mouse_y >= item_y && mouse_y <= item_y + MENU_ITEM_HEIGHT) {

                // Update selected option on hover
                selected_option_ = static_cast<int>(i);

                // Return selection on click
                if (clicked) {
                    return static_cast<int>(i);
                }
            }
        }

        return -1;
    }

}
