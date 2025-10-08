#include "ui/audio_settings_menu.h"
#include <cmath>

namespace towerforge {
namespace ui {

AudioSettingsMenu::AudioSettingsMenu()
    : selected_option_(0)
    , animation_time_(0.0f)
    , master_volume_(0.7f)
    , music_volume_(0.5f)
    , sfx_volume_(0.6f) {
}

AudioSettingsMenu::~AudioSettingsMenu() {
}

void AudioSettingsMenu::Update(float delta_time) {
    animation_time_ += delta_time;
}

void AudioSettingsMenu::Render() {
    RenderBackground();
    RenderHeader();
    RenderVolumeControls();
    RenderBackButton();
}

void AudioSettingsMenu::RenderBackground() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Semi-transparent background overlay
    DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
    
    // Main menu panel
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2 - 30;
    
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT + 100, ColorAlpha(Color{30, 30, 40, 255}, 0.95f));
    DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT + 100, GOLD);
}

void AudioSettingsMenu::RenderHeader() {
    int screen_width = GetScreenWidth();
    
    const char* title = "AUDIO SETTINGS";
    int title_width = MeasureText(title, 32);
    DrawText(title, (screen_width - title_width) / 2, 100, 32, GOLD);
    
    // Underline
    int line_width = title_width + 40;
    int line_x = (screen_width - line_width) / 2;
    DrawRectangle(line_x, 140, line_width, 2, GOLD);
}

void AudioSettingsMenu::RenderVolumeControls() {
    RenderVolumeSlider("Master Volume", master_volume_, SLIDER_START_Y, selected_option_ == 0);
    RenderVolumeSlider("Music Volume", music_volume_, SLIDER_START_Y + SLIDER_HEIGHT + SLIDER_SPACING, selected_option_ == 1);
    RenderVolumeSlider("Sound Effects", sfx_volume_, SLIDER_START_Y + 2 * (SLIDER_HEIGHT + SLIDER_SPACING), selected_option_ == 2);
}

void AudioSettingsMenu::RenderVolumeSlider(const char* label, float value, int y_pos, bool is_selected) {
    int screen_width = GetScreenWidth();
    int slider_x = (screen_width - 400) / 2;
    
    // Label
    Color label_color = is_selected ? GOLD : LIGHTGRAY;
    DrawText(label, slider_x, y_pos, 20, label_color);
    
    // Slider background
    int slider_bar_y = y_pos + 30;
    int slider_bar_width = 350;
    Color bg_color = is_selected ? ColorAlpha(GOLD, 0.2f) : ColorAlpha(DARKGRAY, 0.3f);
    DrawRectangle(slider_x, slider_bar_y, slider_bar_width, 10, bg_color);
    
    // Slider fill
    int fill_width = static_cast<int>(slider_bar_width * value);
    Color fill_color = is_selected ? GOLD : GRAY;
    DrawRectangle(slider_x, slider_bar_y, fill_width, 10, fill_color);
    
    // Slider thumb
    int thumb_x = slider_x + fill_width - 5;
    DrawCircle(thumb_x + 5, slider_bar_y + 5, 8, is_selected ? GOLD : WHITE);
    
    // Value percentage
    char value_text[16];
    snprintf(value_text, sizeof(value_text), "%d%%", static_cast<int>(value * 100));
    int value_width = MeasureText(value_text, 18);
    DrawText(value_text, slider_x + slider_bar_width + 20, y_pos + 25, 18, label_color);
    
    // Selection indicator
    if (is_selected) {
        float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
        DrawText(">", slider_x - 30, y_pos, 24, ColorAlpha(GOLD, pulse));
    }
}

void AudioSettingsMenu::RenderBackButton() {
    int screen_width = GetScreenWidth();
    int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;
    
    bool is_selected = (selected_option_ == 3);
    
    // Button background
    Color bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
    DrawRectangle(button_x, BACK_BUTTON_Y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, bg_color);
    
    // Button border
    Color border_color = is_selected ? GOLD : GRAY;
    int border_thickness = is_selected ? 3 : 2;
    DrawRectangleLines(button_x, BACK_BUTTON_Y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, border_color);
    
    // Button text
    const char* text = "Back";
    int text_width = MeasureText(text, 20);
    Color text_color = is_selected ? GOLD : LIGHTGRAY;
    DrawText(text, button_x + (BACK_BUTTON_WIDTH - text_width) / 2, 
             BACK_BUTTON_Y + (BACK_BUTTON_HEIGHT - 20) / 2, 20, text_color);
    
    // Selection indicator
    if (is_selected) {
        float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
        DrawText(">", button_x - 30, BACK_BUTTON_Y + 12, 24, ColorAlpha(GOLD, pulse));
    }
    
    // Instructions
    int screen_height = GetScreenHeight();
    const char* instruction = "Arrow Keys: Navigate | LEFT/RIGHT: Adjust | ESC/ENTER: Back";
    int instruction_width = MeasureText(instruction, 16);
    DrawText(instruction, (screen_width - instruction_width) / 2, 
             screen_height - 50, 16, LIGHTGRAY);
}

bool AudioSettingsMenu::HandleKeyboard() {
    // Navigate up
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selected_option_--;
        if (selected_option_ < 0) {
            selected_option_ = 3;
        }
    }
    
    // Navigate down
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selected_option_++;
        if (selected_option_ > 3) {
            selected_option_ = 0;
        }
    }
    
    // Adjust volume with left/right arrows
    if (selected_option_ >= 0 && selected_option_ <= 2) {
        float* volume_ptr = nullptr;
        if (selected_option_ == 0) volume_ptr = &master_volume_;
        else if (selected_option_ == 1) volume_ptr = &music_volume_;
        else if (selected_option_ == 2) volume_ptr = &sfx_volume_;
        
        if (volume_ptr) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                *volume_ptr -= 0.1f;
                if (*volume_ptr < 0.0f) *volume_ptr = 0.0f;
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                *volume_ptr += 0.1f;
                if (*volume_ptr > 1.0f) *volume_ptr = 1.0f;
            }
        }
    }
    
    // Back with ESC or ENTER on back button
    if (IsKeyPressed(KEY_ESCAPE)) {
        return true;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selected_option_ == 3) {
            return true;
        }
    }
    
    return false;
}

bool AudioSettingsMenu::HandleMouse(int mouse_x, int mouse_y, bool clicked) {
    int screen_width = GetScreenWidth();
    int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;
    
    // Check back button hover
    if (mouse_x >= button_x && mouse_x <= button_x + BACK_BUTTON_WIDTH &&
        mouse_y >= BACK_BUTTON_Y && mouse_y <= BACK_BUTTON_Y + BACK_BUTTON_HEIGHT) {
        selected_option_ = 3;
        if (clicked) {
            return true;
        }
    }
    
    // Check volume sliders
    int slider_x = (screen_width - 400) / 2;
    int slider_bar_width = 350;
    
    for (int i = 0; i < 3; i++) {
        int slider_y = SLIDER_START_Y + i * (SLIDER_HEIGHT + SLIDER_SPACING);
        int slider_bar_y = slider_y + 30;
        
        if (mouse_x >= slider_x && mouse_x <= slider_x + slider_bar_width &&
            mouse_y >= slider_y && mouse_y <= slider_y + SLIDER_HEIGHT) {
            selected_option_ = i;
            
            // If clicked, adjust volume based on click position
            if (clicked && mouse_y >= slider_bar_y && mouse_y <= slider_bar_y + 10) {
                float new_value = static_cast<float>(mouse_x - slider_x) / slider_bar_width;
                if (new_value < 0.0f) new_value = 0.0f;
                if (new_value > 1.0f) new_value = 1.0f;
                
                if (i == 0) master_volume_ = new_value;
                else if (i == 1) music_volume_ = new_value;
                else if (i == 2) sfx_volume_ = new_value;
            }
        }
    }
    
    return false;
}

} // namespace ui
} // namespace towerforge
