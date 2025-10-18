#include "ui/main_menu.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include <cmath>

namespace towerforge::ui {

    MainMenu::MainMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , selected_menu_option_(-1) {

        // Initialize menu items
        menu_items_.push_back({"New Game", MenuOption::NewGame});
        menu_items_.push_back({"Tutorial", MenuOption::Tutorial});
        menu_items_.push_back({"Load Game", MenuOption::LoadGame});
        menu_items_.push_back({"Achievements", MenuOption::Achievements});
        menu_items_.push_back({"Settings", MenuOption::Settings});
        menu_items_.push_back({"Credits", MenuOption::Credits});
        menu_items_.push_back({"Quit", MenuOption::Quit});
        
        // MainMenu is a Panel (container) - position and size don't matter much since it's full screen
        main_panel_ = std::make_unique<Panel>(0, 0, 800, 600, BLANK, BLANK);
        
        // Create Button objects for each menu item and add them as children
        // Buttons are children of the main panel with relative positioning
        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
            // x position will be set during render
            auto button = std::make_unique<Button>(
                0, // x will be set during render
                static_cast<float>(item_y),
                static_cast<float>(MENU_WIDTH),
                static_cast<float>(MENU_ITEM_HEIGHT),
                menu_items_[i].label,
                UITheme::BUTTON_BACKGROUND,
                UITheme::BUTTON_BORDER
            );
            button->SetFontSize(UITheme::FONT_SIZE_MEDIUM); // Default size, will be adjusted for selected item
            
            // Set click callback for this button
            const int option_index = static_cast<int>(i);
            button->SetClickCallback([this, option_index]() {
                selected_menu_option_ = option_index;
            });
            
            // Store raw pointer for later access
            Button* button_ptr = button.get();
            menu_item_buttons_.push_back(button_ptr);
            
            // Add button as child to main panel
            main_panel_->AddChild(std::move(button));
        }
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
        // Clear with theme background color
        ClearBackground(UITheme::BACKGROUND_DARK);

        // Draw subtle grid pattern in background
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        for (int i = 0; i < screen_height; i += 40) {
            DrawLine(0, i, screen_width, i, UITheme::DECORATIVE_GRID);
        }
        for (int i = 0; i < screen_width; i += 40) {
            DrawLine(i, 0, i, screen_height, UITheme::DECORATIVE_GRID);
        }

        // Draw simple tower silhouettes on the sides
        DrawRectangle(50, 300, 60, 300, ColorAlpha(UITheme::DECORATIVE_WINDOW, 0.3f));
        DrawRectangle(screen_width - 110, 250, 60, 350, ColorAlpha(UITheme::DECORATIVE_WINDOW, 0.3f));

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

        // Draw main title with responsive sizing
        const auto title = "TOWERFORGE";
        const int title_font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_TITLE);
        const int title_width = MeasureText(title, title_font_size);
        const int title_x = UITheme::CenterPosition(screen_width, title_width);

        // Draw title shadow for depth
        DrawText(title, title_x + 3, TITLE_Y + 3, title_font_size, ColorAlpha(BLACK, 0.5f));

        // Draw title with primary color
        DrawText(title, title_x, TITLE_Y, title_font_size, UITheme::PRIMARY);

        // Draw tagline with responsive sizing
        const auto tagline = "\"Build, Survive, Thrive!\"";
        const int tagline_font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_MEDIUM);
        const int tagline_width = MeasureText(tagline, tagline_font_size);
        const int tagline_x = UITheme::CenterPosition(screen_width, tagline_width);

        DrawText(tagline, tagline_x, TITLE_Y + 70, tagline_font_size, UITheme::TEXT_SECONDARY);

        // Draw decorative line with responsive width (35% of screen, clamped between 300-500px)
        const int line_width = UITheme::ClampSize(UITheme::ResponsiveWidth(0.35f), 300, 500);
        const int line_x = UITheme::CenterPosition(screen_width, line_width);
        DrawRectangle(line_x, TITLE_Y + 100, line_width, 2, UITheme::PRIMARY);
    }

    void MainMenu::RenderMenuOptions() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Responsive menu sizing (25% of screen width, clamped between 250-400px)
        const int responsive_menu_width = UITheme::ClampSize(UITheme::ResponsiveWidth(0.25f), 250, 400);
        // Scale item height with screen height, clamped between 40-60px
        const int responsive_item_height = UITheme::ClampSize(screen_height / 18, 40, 60);
        const int responsive_spacing = UITheme::MARGIN_SMALL;

        for (size_t i = 0; i < menu_items_.size(); ++i) {
            const int item_y = MENU_START_Y + i * (responsive_item_height + responsive_spacing);
            const int item_x = UITheme::CenterPosition(screen_width, responsive_menu_width);

            const bool is_selected = (static_cast<int>(i) == selected_option_);

            // Update button position and appearance based on selection
            auto& button = menu_item_buttons_[i];
            button->SetRelativePosition(static_cast<float>(item_x), static_cast<float>(item_y));
            button->SetSize(static_cast<float>(responsive_menu_width), static_cast<float>(responsive_item_height));
            button->SetBackgroundColor(is_selected ? ColorAlpha(UITheme::PRIMARY, 0.3f) : UITheme::BUTTON_BACKGROUND);
            button->SetBorderColor(is_selected ? UITheme::PRIMARY : UITheme::BUTTON_BORDER);
            
            // Responsive font sizing
            const int base_font_size = is_selected ? UITheme::FONT_SIZE_LARGE : UITheme::FONT_SIZE_MEDIUM;
            button->SetFontSize(UITheme::ResponsiveFontSize(base_font_size));
            
            // Add subtle animation to selected item
            Color text_color = is_selected ? UITheme::TEXT_PRIMARY : UITheme::TEXT_SECONDARY;
            if (is_selected) {
                const float pulse = sin(animation_time_ * UITheme::ANIMATION_SPEED_NORMAL) * 0.1f + 0.9f;
                text_color = ColorAlpha(UITheme::TEXT_PRIMARY, pulse);
            }
            button->SetTextColor(text_color);
            
            // Render the button
            button->Render();

            // Draw selection indicator
            if (is_selected) {
                const int indicator_x = item_x - UITheme::PADDING_LARGE;
                const int indicator_font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_LARGE);
                const int indicator_y = item_y + responsive_item_height / 2 - indicator_font_size / 2;
                DrawText(">", indicator_x, indicator_y, indicator_font_size, UITheme::PRIMARY);
            }
        }
    }

    void MainMenu::RenderVersion() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        const std::string version_text = "Version: " + GetVersion();
        const int font_size = UITheme::FONT_SIZE_NORMAL;
        const int text_width = MeasureText(version_text.c_str(), font_size);

        DrawText(version_text.c_str(), screen_width - text_width - UITheme::PADDING_LARGE,
                 screen_height - UITheme::PADDING_LARGE - UITheme::PADDING_SMALL, font_size, UITheme::BORDER_DEFAULT);
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
        // Create mouse event
        MouseEvent event(
            static_cast<float>(mouse_x), 
            static_cast<float>(mouse_y),
            false, // left_down (not used currently)
            false, // right_down (not used currently)
            clicked, // left_pressed
            false  // right_pressed
        );

        // Reset selected menu option
        selected_menu_option_ = -1;

        // Process mouse event through the panel (which will propagate to children)
        main_panel_->ProcessMouseEvent(event);

        // Update selected_option_ based on which button is hovered
        for (size_t i = 0; i < menu_item_buttons_.size(); ++i) {
            if (menu_item_buttons_[i]->IsHovered()) {
                selected_option_ = static_cast<int>(i);
                break;
            }
        }

        // Return the selected menu option if a button was clicked
        return selected_menu_option_;
    }

}
