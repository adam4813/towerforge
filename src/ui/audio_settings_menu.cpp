#include "ui/audio_settings_menu.h"
#include "audio/audio_manager.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace towerforge::ui {

    AudioSettingsMenu::AudioSettingsMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , master_volume_(0.7f)
          , music_volume_(0.5f)
          , sfx_volume_(0.6f)
          , mute_all_(false)
          , mute_music_(false)
          , mute_sfx_(false)
          , enable_ambient_(true) {
        LoadSettings();
        ApplyAudioSettings();
    }

    AudioSettingsMenu::~AudioSettingsMenu() {
        SaveSettings();
    }

    void AudioSettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
    }

    void AudioSettingsMenu::SyncWithAudioManager() {
        const auto& audio_mgr = audio::AudioManager::GetInstance();
        if (audio_mgr.IsInitialized()) {
            master_volume_ = audio_mgr.GetMasterVolume();
            music_volume_ = audio_mgr.GetVolume(audio::AudioType::Music);
            sfx_volume_ = audio_mgr.GetVolume(audio::AudioType::SFX);
        }
    }

    void AudioSettingsMenu::Render() {
        RenderBackground();
        RenderHeader();
        RenderVolumeControls();
        RenderToggleControls();
        RenderBackButton();
    }

    void AudioSettingsMenu::RenderBackground() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent background overlay
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));

        // Main menu panel
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - (MENU_HEIGHT + 100)) / 2 - 30;

        DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT + 100, ColorAlpha(Color{30, 30, 40, 255}, 0.95f));
        DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT + 100, GOLD);
    }

    void AudioSettingsMenu::RenderHeader() {
        const int screen_width = GetScreenWidth();

        const auto title = "AUDIO SETTINGS";
        const int title_width = MeasureText(title, 32);
        DrawText(title, (screen_width - title_width) / 2, 100, 32, GOLD);

        // Underline
        const int line_width = title_width + 40;
        const int line_x = (screen_width - line_width) / 2;
        DrawRectangle(line_x, 140, line_width, 2, GOLD);
    }

    void AudioSettingsMenu::RenderVolumeControls() const {
        RenderVolumeSlider("Master Volume", master_volume_, SLIDER_START_Y, selected_option_ == 0);
        RenderVolumeSlider("Music Volume", music_volume_, SLIDER_START_Y + SLIDER_HEIGHT + SLIDER_SPACING, selected_option_ == 1);
        RenderVolumeSlider("Sound Effects", sfx_volume_, SLIDER_START_Y + 2 * (SLIDER_HEIGHT + SLIDER_SPACING), selected_option_ == 2);
    }

    void AudioSettingsMenu::RenderVolumeSlider(const char* label, const float value, const int y_pos, const bool is_selected) const {
        const int screen_width = GetScreenWidth();
        const int slider_x = (screen_width - 400) / 2;

        // Label
        const Color label_color = is_selected ? GOLD : LIGHTGRAY;
        DrawText(label, slider_x, y_pos, 20, label_color);

        // Slider background
        const int slider_bar_y = y_pos + 30;
        constexpr int slider_bar_width = 350;
        const Color bg_color = is_selected ? ColorAlpha(GOLD, 0.2f) : ColorAlpha(DARKGRAY, 0.3f);
        DrawRectangle(slider_x, slider_bar_y, slider_bar_width, 10, bg_color);

        // Slider fill
        const int fill_width = static_cast<int>(slider_bar_width * value);
        const Color fill_color = is_selected ? GOLD : GRAY;
        DrawRectangle(slider_x, slider_bar_y, fill_width, 10, fill_color);

        // Slider thumb
        const int thumb_x = slider_x + fill_width - 5;
        DrawCircle(thumb_x + 5, slider_bar_y + 5, 8, is_selected ? GOLD : WHITE);

        // Value percentage
        char value_text[16];
        snprintf(value_text, sizeof(value_text), "%d%%", static_cast<int>(value * 100));
        int value_width = MeasureText(value_text, 18);
        DrawText(value_text, slider_x + slider_bar_width + 20, y_pos + 25, 18, label_color);

        // Selection indicator
        if (is_selected) {
            const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
            DrawText(">", slider_x - 30, y_pos, 24, ColorAlpha(GOLD, pulse));
        }
    }

    void AudioSettingsMenu::RenderBackButton() const {
        const int screen_width = GetScreenWidth();
        const int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;

        const bool is_selected = selected_option_ == 7;

        // Button background
        const Color bg_color = is_selected ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.3f);
        DrawRectangle(button_x, BACK_BUTTON_Y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, bg_color);

        // Button border
        const Color border_color = is_selected ? GOLD : GRAY;
        int border_thickness = is_selected ? 3 : 2;
        DrawRectangleLines(button_x, BACK_BUTTON_Y, BACK_BUTTON_WIDTH, BACK_BUTTON_HEIGHT, border_color);

        // Button text
        const auto text = "Back";
        const int text_width = MeasureText(text, 20);
        const Color text_color = is_selected ? GOLD : LIGHTGRAY;
        DrawText(text, button_x + (BACK_BUTTON_WIDTH - text_width) / 2,
                 BACK_BUTTON_Y + (BACK_BUTTON_HEIGHT - 20) / 2, 20, text_color);

        // Selection indicator
        if (is_selected) {
            const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
            DrawText(">", button_x - 30, BACK_BUTTON_Y + 12, 24, ColorAlpha(GOLD, pulse));
        }

        // Instructions
        const int screen_height = GetScreenHeight();
        const auto instruction = "Arrow Keys: Navigate | LEFT/RIGHT: Adjust | SPACE: Toggle | ESC/ENTER: Back";
        const int instruction_width = MeasureText(instruction, 14);
        DrawText(instruction, (screen_width - instruction_width) / 2,
                 screen_height - 50, 14, LIGHTGRAY);
    }

    void AudioSettingsMenu::RenderToggleControls() const {
        RenderCheckbox("[ ] Mute All", mute_all_, CHECKBOX_START_Y, selected_option_ == 3);
        RenderCheckbox("[ ] Mute Music", mute_music_, CHECKBOX_START_Y + CHECKBOX_HEIGHT + CHECKBOX_SPACING, selected_option_ == 4);
        RenderCheckbox("[ ] Mute SFX", mute_sfx_, CHECKBOX_START_Y + 2 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING), selected_option_ == 5);
        RenderCheckbox("[ ] Enable Ambient Sound", enable_ambient_, CHECKBOX_START_Y + 3 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING), selected_option_ == 6);
    }

    void AudioSettingsMenu::RenderCheckbox(const char* label, const bool checked, const int y_pos, const bool is_selected) const {
        const int screen_width = GetScreenWidth();
        const int checkbox_x = (screen_width - 400) / 2;

        // Checkbox box
        constexpr int box_size = 20;
        const Color box_color = is_selected ? GOLD : LIGHTGRAY;
        DrawRectangleLines(checkbox_x, y_pos + 5, box_size, box_size, box_color);

        // Checkbox fill if checked
        if (checked) {
            DrawRectangle(checkbox_x + 4, y_pos + 9, box_size - 8, box_size - 8, is_selected ? GOLD : GRAY);
        }

        // Label
        const Color label_color = is_selected ? GOLD : LIGHTGRAY;
        DrawText(label, checkbox_x + box_size + 10, y_pos + 5, 18, label_color);

        // Selection indicator
        if (is_selected) {
            const float pulse = 0.5f + 0.5f * sinf(animation_time_ * 4.0f);
            DrawText(">", checkbox_x - 30, y_pos, 24, ColorAlpha(GOLD, pulse));
        }
    }

    bool AudioSettingsMenu::HandleKeyboard() {
        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selected_option_--;
            if (selected_option_ < 0) {
                selected_option_ = 7;
            }
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selected_option_++;
            if (selected_option_ > 7) {
                selected_option_ = 0;
            }
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
        }

        // Adjust volume with left/right arrows
        if (selected_option_ >= 0 && selected_option_ <= 2) {
            float* volume_ptr = nullptr;
            if (selected_option_ == 0) volume_ptr = &master_volume_;
            else if (selected_option_ == 1) volume_ptr = &music_volume_;
            else if (selected_option_ == 2) volume_ptr = &sfx_volume_;

            if (volume_ptr) {
                bool volume_changed = false;
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    *volume_ptr -= 0.1f;
                    if (*volume_ptr < 0.0f) *volume_ptr = 0.0f;
                    volume_changed = true;
                    ApplyAudioSettings();
                    SaveSettings();
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    *volume_ptr += 0.1f;
                    if (*volume_ptr > 1.0f) *volume_ptr = 1.0f;
                    volume_changed = true;
                }

                // Apply changes to AudioManager
                if (volume_changed) {
                    auto& audio_mgr = audio::AudioManager::GetInstance();
                    if (selected_option_ == 0) {
                        audio_mgr.SetMasterVolume(master_volume_);
                    } else if (selected_option_ == 1) {
                        audio_mgr.SetVolume(audio::AudioType::Music, music_volume_);
                    } else if (selected_option_ == 2) {
                        audio_mgr.SetVolume(audio::AudioType::SFX, sfx_volume_);
                        // Play test sound
                        audio_mgr.PlaySFX(audio::AudioCue::MenuConfirm);
                    }
                    ApplyAudioSettings();
                    SaveSettings();
                }
            }
        }

        // Toggle checkboxes with space
        if (selected_option_ >= 3 && selected_option_ <= 6) {
            if (IsKeyPressed(KEY_SPACE)) {
                if (selected_option_ == 3) mute_all_ = !mute_all_;
                else if (selected_option_ == 4) mute_music_ = !mute_music_;
                else if (selected_option_ == 5) mute_sfx_ = !mute_sfx_;
                else if (selected_option_ == 6) enable_ambient_ = !enable_ambient_;
                ApplyAudioSettings();
                SaveSettings();
            }
        }

        // Back with ESC or ENTER on back button
        if (IsKeyPressed(KEY_ESCAPE)) {
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
            return true;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (selected_option_ == 7) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                return true;
            }
        }

        return false;
    }

    bool AudioSettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        const int screen_width = GetScreenWidth();
        const int button_x = (screen_width - BACK_BUTTON_WIDTH) / 2;

        // Check back button hover
        if (mouse_x >= button_x && mouse_x <= button_x + BACK_BUTTON_WIDTH &&
            mouse_y >= BACK_BUTTON_Y && mouse_y <= BACK_BUTTON_Y + BACK_BUTTON_HEIGHT) {
            selected_option_ = 7;
            if (clicked) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                return true;
            }
        }

        // Check volume sliders
        const int slider_x = (screen_width - 400) / 2;
        constexpr int slider_bar_width = 350;

        for (int i = 0; i < 3; i++) {
            const int slider_y = SLIDER_START_Y + i * (SLIDER_HEIGHT + SLIDER_SPACING);
            const int slider_bar_y = slider_y + 30;

            if (mouse_x >= slider_x && mouse_x <= slider_x + slider_bar_width &&
                mouse_y >= slider_y && mouse_y <= slider_y + SLIDER_HEIGHT) {
                selected_option_ = i;

                // If clicked, adjust volume based on click position
                if (clicked && mouse_y >= slider_bar_y && mouse_y <= slider_bar_y + 10) {
                    float new_value = static_cast<float>(mouse_x - slider_x) / slider_bar_width;
                    if (new_value < 0.0f) new_value = 0.0f;
                    if (new_value > 1.0f) new_value = 1.0f;

                    auto& audio_mgr = audio::AudioManager::GetInstance();

                    if (i == 0) {
                        master_volume_ = new_value;
                        audio_mgr.SetMasterVolume(master_volume_);
                    }
                    else if (i == 1) {
                        music_volume_ = new_value;
                        audio_mgr.SetVolume(audio::AudioType::Music, music_volume_);
                    }
                    else if (i == 2) {
                        sfx_volume_ = new_value;
                        audio_mgr.SetVolume(audio::AudioType::SFX, sfx_volume_);
                        // Play test sound
                        audio_mgr.PlaySFX(audio::AudioCue::MenuConfirm);
                    }

                    ApplyAudioSettings();
                    SaveSettings();
                }
            }
        }

        // Check checkboxes
        const int checkbox_x = (screen_width - 400) / 2;
        int box_size = 20;

        for (int i = 0; i < 4; i++) {
            const int checkbox_y = CHECKBOX_START_Y + i * (CHECKBOX_HEIGHT + CHECKBOX_SPACING);

            if (mouse_x >= checkbox_x && mouse_x <= checkbox_x + 300 &&
                mouse_y >= checkbox_y && mouse_y <= checkbox_y + CHECKBOX_HEIGHT) {
                selected_option_ = i + 3;

                if (clicked) {
                    if (i == 0) mute_all_ = !mute_all_;
                    else if (i == 1) mute_music_ = !mute_music_;
                    else if (i == 2) mute_sfx_ = !mute_sfx_;
                    else if (i == 3) enable_ambient_ = !enable_ambient_;

                    ApplyAudioSettings();
                    SaveSettings();
                }
            }
        }

        return false;
    }

    void AudioSettingsMenu::LoadSettings() {
        // Determine config file path
        std::string config_path;

#ifdef _WIN32
        if (const char* appdata = std::getenv("APPDATA")) {
            config_path = std::string(appdata) + "/TowerForge/audio_settings.cfg";
        } else {
            config_path = "audio_settings.cfg";
        }
#else
        const char* home = std::getenv("HOME");
        if (home) {
            config_path = std::string(home) + "/.towerforge/audio_settings.cfg";
        } else {
            config_path = "audio_settings.cfg";
        }
#endif

        // Try to load settings from file
        if (std::ifstream file(config_path); file.is_open()) {
            int master_vol_int = 70, music_vol_int = 50, sfx_vol_int = 60;
            int mute_all_int = 0, mute_music_int = 0, mute_sfx_int = 0, enable_ambient_int = 1;

            file >> master_vol_int >> music_vol_int >> sfx_vol_int;
            file >> mute_all_int >> mute_music_int >> mute_sfx_int >> enable_ambient_int;

            master_volume_ = master_vol_int / 100.0f;
            music_volume_ = music_vol_int / 100.0f;
            sfx_volume_ = sfx_vol_int / 100.0f;

            mute_all_ = (mute_all_int == 1);
            mute_music_ = (mute_music_int == 1);
            mute_sfx_ = (mute_sfx_int == 1);
            enable_ambient_ = (enable_ambient_int == 1);

            file.close();
        }
    }

    void AudioSettingsMenu::SaveSettings() const {
        // Determine config file path
        std::string config_path;
        std::string config_dir;

#ifdef _WIN32
        if (const char* appdata = std::getenv("APPDATA")) {
            config_dir = std::string(appdata) + "/TowerForge";
            config_path = config_dir + "/audio_settings.cfg";
        } else {
            config_path = "audio_settings.cfg";
        }
#else
        const char* home = std::getenv("HOME");
        if (home) {
            config_dir = std::string(home) + "/.towerforge";
            config_path = config_dir + "/audio_settings.cfg";
        } else {
            config_path = "audio_settings.cfg";
        }
#endif

        // Create directory if it doesn't exist
        if (!config_dir.empty()) {
            std::filesystem::create_directory(config_dir.c_str());
        }

        // Save settings to file
        if (std::ofstream file(config_path); file.is_open()) {
            file << static_cast<int>(master_volume_ * 100) << " ";
            file << static_cast<int>(music_volume_ * 100) << " ";
            file << static_cast<int>(sfx_volume_ * 100) << "\n";
            file << (mute_all_ ? 1 : 0) << " ";
            file << (mute_music_ ? 1 : 0) << " ";
            file << (mute_sfx_ ? 1 : 0) << " ";
            file << (enable_ambient_ ? 1 : 0) << "\n";
            file.close();
        }
    }

    void AudioSettingsMenu::ApplyAudioSettings() {
        // Apply master volume (this affects all audio)
        const float effective_master = mute_all_ ? 0.0f : master_volume_;
        SetMasterVolume(effective_master);
        SyncWithAudioManager();

        // Note: Raylib doesn't have separate SetMusicVolume/SetSoundVolume functions
        // that persist globally. Instead, you need to set volume on individual
        // Music/Sound instances. For now, we just control master volume.
        // The mute_music_, mute_sfx_, and enable_ambient_ flags would be used
        // when actually playing audio to determine if individual sounds should play.
    }

}
