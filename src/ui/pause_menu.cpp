#include "ui/pause_menu.h"
#include <cmath>

namespace towerforge {
namespace ui {

PauseMenu::PauseMenu()
    : selected_option_(0)
    , animation_time_(0.0f)
    , show_quit_confirmation_(false)
    , quit_confirmation_selection_(0) {
    
    // Initialize menu items
    menu_items_.push_back({"Resume Game", PauseMenuOption::Resume});
    menu_items_.push_back({"Save Game", PauseMenuOption::SaveGame});
    menu_items_.push_back({"Load Game", PauseMenuOption::LoadGame});
    menu_items_.push_back({"Settings", PauseMenuOption::Settings});
    menu_items_.push_back({"Quit to Title", PauseMenuOption::QuitToTitle});
}

PauseMenu::~PauseMenu() {
}

void PauseMenu::Update(float delta_time) {
    animation_time_ += delta_time;
}

void PauseMenu::Render() {
    RenderOverlay();
    
    if (show_quit_confirmation_) {
        RenderQuitConfirmation();
    } else {
        RenderMenuOptions();
    }
}

void PauseMenu::RenderOverlay() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Draw semi-transparent overlay to dim the game
    DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
    
    // Draw subtle grid pattern for background
    for (int i = 0; i < screen_height; i += 50) {
        DrawLine(0, i, screen_width, i, ColorAlpha(DARKGRAY, 0.1f));
    }
    for (int i = 0; i < screen_width; i += 50) {
        DrawLine(i, 0, i, screen_height, ColorAlpha(DARKGRAY, 0.1f));
    }
    
    // Draw "PAUSED" title
    const char* title = "PAUSED";
    int title_font_size = 50;
    int title_width = MeasureText(title, title_font_size);
    int title_x = (screen_width - title_width) / 2;
    
    // Draw title shadow
    DrawText(title, title_x + 2, TITLE_Y + 2, title_font_size, ColorAlpha(BLACK, 0.5f));
    
    // Draw title with pulsing effect
    float pulse = sin(animation_time_ * 2.0f) * 0.1f + 0.9f;
    DrawText(title, title_x, TITLE_Y, title_font_size, ColorAlpha(GOLD, pulse));
    
    // Draw decorative line
    int line_width = 300;
    int line_x = (screen_width - line_width) / 2;
    DrawRectangle(line_x, TITLE_Y + 60, line_width, 2, GOLD);
}

void PauseMenu::RenderMenuOptions() {
    int screen_width = GetScreenWidth();
    
    for (size_t i = 0; i < menu_items_.size(); ++i) {
        int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
        int item_x = (screen_width - MENU_WIDTH) / 2;
        
        bool is_selected = (static_cast<int>(i) == selected_option_);
        
        // Draw menu item background
        Color bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
        DrawRectangle(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, bg_color);
        
        // Draw menu item border
        Color border_color = is_selected ? GOLD : GRAY;
        int border_thickness = is_selected ? 3 : 2;
        DrawRectangleLines(item_x, item_y, MENU_WIDTH, MENU_ITEM_HEIGHT, border_color);
        
        // Draw menu item text
        const char* text = menu_items_[i].label.c_str();
        int font_size = is_selected ? 24 : 22;
        int text_width = MeasureText(text, font_size);
        int text_x = item_x + (MENU_WIDTH - text_width) / 2;
        int text_y = item_y + (MENU_ITEM_HEIGHT - font_size) / 2;
        
        Color text_color = is_selected ? WHITE : LIGHTGRAY;
        
        // Add subtle animation to selected item
        if (is_selected) {
            float pulse = sin(animation_time_ * 3.0f) * 0.1f + 0.9f;
            text_color = ColorAlpha(WHITE, pulse);
        }
        
        DrawText(text, text_x, text_y, font_size, text_color);
        
        // Draw selection indicator
        if (is_selected) {
            int indicator_x = item_x - 30;
            int indicator_y = item_y + MENU_ITEM_HEIGHT / 2;
            DrawText(">", indicator_x, indicator_y - 12, 24, GOLD);
        }
    }
    
    // Draw instruction at bottom
    int screen_height = GetScreenHeight();
    const char* instruction = "ESC to Resume | ENTER to Select";
    int instruction_width = MeasureText(instruction, 16);
    DrawText(instruction, (screen_width - instruction_width) / 2, 
             screen_height - 50, 16, LIGHTGRAY);
}

void PauseMenu::RenderQuitConfirmation() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Draw confirmation dialog box
    int dialog_width = 500;
    int dialog_height = 250;
    int dialog_x = (screen_width - dialog_width) / 2;
    int dialog_y = (screen_height - dialog_height) / 2;
    
    // Draw dialog background
    DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, ColorAlpha(Color{30, 30, 40, 255}, 0.95f));
    DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height, GOLD);
    DrawRectangleLines(dialog_x + 2, dialog_y + 2, dialog_width - 4, dialog_height - 4, GOLD);
    
    // Draw warning icon (exclamation mark)
    DrawText("!", dialog_x + dialog_width / 2 - 10, dialog_y + 30, 40, ORANGE);
    
    // Draw confirmation message
    const char* message1 = "Quit to Title Screen?";
    const char* message2 = "Any unsaved progress will be lost.";
    int message1_width = MeasureText(message1, 24);
    int message2_width = MeasureText(message2, 18);
    
    DrawText(message1, dialog_x + (dialog_width - message1_width) / 2, dialog_y + 85, 24, WHITE);
    DrawText(message2, dialog_x + (dialog_width - message2_width) / 2, dialog_y + 120, 18, LIGHTGRAY);
    
    // Draw buttons
    int button_width = 150;
    int button_height = 45;
    int button_spacing = 30;
    int buttons_total_width = button_width * 2 + button_spacing;
    int button_start_x = dialog_x + (dialog_width - buttons_total_width) / 2;
    int button_y = dialog_y + dialog_height - 75;
    
    // Cancel button
    bool cancel_selected = (quit_confirmation_selection_ == 0);
    Color cancel_bg = cancel_selected ? ColorAlpha(GRAY, 0.5f) : ColorAlpha(DARKGRAY, 0.3f);
    Color cancel_border = cancel_selected ? WHITE : GRAY;
    
    DrawRectangle(button_start_x, button_y, button_width, button_height, cancel_bg);
    DrawRectangleLines(button_start_x, button_y, button_width, button_height, cancel_border);
    
    const char* cancel_text = "Cancel";
    int cancel_text_width = MeasureText(cancel_text, 20);
    DrawText(cancel_text, button_start_x + (button_width - cancel_text_width) / 2, 
             button_y + 12, 20, cancel_selected ? WHITE : LIGHTGRAY);
    
    // Confirm button
    bool confirm_selected = (quit_confirmation_selection_ == 1);
    Color confirm_bg = confirm_selected ? ColorAlpha(RED, 0.5f) : ColorAlpha(DARKGRAY, 0.3f);
    Color confirm_border = confirm_selected ? RED : GRAY;
    
    int confirm_x = button_start_x + button_width + button_spacing;
    DrawRectangle(confirm_x, button_y, button_width, button_height, confirm_bg);
    DrawRectangleLines(confirm_x, button_y, button_width, button_height, confirm_border);
    
    const char* confirm_text = "Quit";
    int confirm_text_width = MeasureText(confirm_text, 20);
    DrawText(confirm_text, confirm_x + (button_width - confirm_text_width) / 2, 
             button_y + 12, 20, confirm_selected ? WHITE : LIGHTGRAY);
    
    // Draw instruction
    const char* instruction = "LEFT/RIGHT to Select | ENTER to Confirm | ESC to Cancel";
    int instruction_width = MeasureText(instruction, 14);
    DrawText(instruction, dialog_x + (dialog_width - instruction_width) / 2, 
             dialog_y + dialog_height - 25, 14, DARKGRAY);
}

int PauseMenu::HandleKeyboard() {
    // If quit confirmation is showing, handle that separately
    if (show_quit_confirmation_) {
        return -1;
    }
    
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
    
    // ESC to resume
    if (IsKeyPressed(KEY_ESCAPE)) {
        return static_cast<int>(PauseMenuOption::Resume);
    }
    
    return -1;
}

int PauseMenu::HandleMouse(int mouse_x, int mouse_y, bool clicked) {
    // If quit confirmation is showing, don't handle menu mouse input
    if (show_quit_confirmation_) {
        return -1;
    }
    
    int screen_width = GetScreenWidth();
    
    for (size_t i = 0; i < menu_items_.size(); ++i) {
        int item_y = MENU_START_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
        int item_x = (screen_width - MENU_WIDTH) / 2;
        
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

int PauseMenu::HandleQuitConfirmation() {
    if (!show_quit_confirmation_) {
        return -1;
    }
    
    // Navigate left/right
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        quit_confirmation_selection_ = 0;  // Cancel
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        quit_confirmation_selection_ = 1;  // Confirm
    }
    
    // Confirm selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        int result = quit_confirmation_selection_;
        show_quit_confirmation_ = false;
        quit_confirmation_selection_ = 0;  // Reset to cancel
        return result;
    }
    
    // Cancel with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        show_quit_confirmation_ = false;
        quit_confirmation_selection_ = 0;  // Reset to cancel
        return 0;  // Cancel
    }
    
    // Handle mouse input for confirmation dialog
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        
        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();
        
        int dialog_width = 500;
        int dialog_height = 250;
        int dialog_x = (screen_width - dialog_width) / 2;
        int dialog_y = (screen_height - dialog_height) / 2;
        
        int button_width = 150;
        int button_height = 45;
        int button_spacing = 30;
        int buttons_total_width = button_width * 2 + button_spacing;
        int button_start_x = dialog_x + (dialog_width - buttons_total_width) / 2;
        int button_y = dialog_y + dialog_height - 75;
        
        // Check cancel button
        if (mouse_x >= button_start_x && mouse_x <= button_start_x + button_width &&
            mouse_y >= button_y && mouse_y <= button_y + button_height) {
            show_quit_confirmation_ = false;
            quit_confirmation_selection_ = 0;
            return 0;  // Cancel
        }
        
        // Check confirm button
        int confirm_x = button_start_x + button_width + button_spacing;
        if (mouse_x >= confirm_x && mouse_x <= confirm_x + button_width &&
            mouse_y >= button_y && mouse_y <= button_y + button_height) {
            show_quit_confirmation_ = false;
            quit_confirmation_selection_ = 0;
            return 1;  // Confirm
        }
    }
    
    return -1;
}

} // namespace ui
} // namespace towerforge
