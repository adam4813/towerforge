#include "ui/audio_settings_menu.h"

#include "audio/audio_manager.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "core/user_preferences.hpp"

import engine;

namespace towerforge::ui {

    AudioSettingsMenu::AudioSettingsMenu()
        : back_callback_(nullptr)
          , master_slider_(nullptr)
          , music_slider_(nullptr)
          , sfx_slider_(nullptr)
          , mute_all_checkbox_(nullptr)
          , mute_music_checkbox_(nullptr)
          , mute_sfx_checkbox_(nullptr)
          , enable_ambient_checkbox_(nullptr)
          , back_button_(nullptr)
          , selected_index_(0)
          , animation_time_(0.0f)
          , last_screen_height_(0)
          , last_screen_width_(0)
          , master_volume_(0.7f)
          , music_volume_(0.5f)
          , sfx_volume_(0.6f)
          , mute_all_(false)
          , mute_music_(false)
          , mute_sfx_(false)
          , enable_ambient_(true) {
        
        LoadSettings();
    }

    AudioSettingsMenu::~AudioSettingsMenu() = default;

    void AudioSettingsMenu::Initialize() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Create main panel
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;
        settings_panel_ = std::make_unique<engine::ui::elements::Panel>(
            panel_x, panel_y, MENU_WIDTH, MENU_HEIGHT
        );
        settings_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        settings_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        settings_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));

        // Add title text (positions are relative to content area after padding)
        auto title_text = std::make_unique<engine::ui::elements::Text>(
            0, 0,
            "Audio Settings",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
        );
        settings_panel_->AddChild(std::move(title_text));


        // Calculate content width (panel width minus padding on both sides)
        const float content_width = MENU_WIDTH - UITheme::PADDING_LARGE * 2;

        // Create Master Volume Slider
        auto master_slider = std::make_unique<engine::ui::elements::Slider>(
            0, SLIDER_START_Y,
            content_width, SLIDER_HEIGHT,
            0.0f, 1.0f, master_volume_
        );
        master_slider->SetLabel("Master Volume");
        master_slider->SetShowValue(true);
        master_slider->SetValueChangedCallback([this](const float value) {
            master_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetMasterVolume(value);
            SaveSettings();
        });
        master_slider_ = master_slider.get();
        settings_panel_->AddChild(std::move(master_slider));

        // Create Music Volume Slider
        auto music_slider = std::make_unique<engine::ui::elements::Slider>(
            0, SLIDER_START_Y + SLIDER_HEIGHT + SLIDER_SPACING,
            content_width, SLIDER_HEIGHT,
            0.0f, 1.0f, music_volume_
        );
        music_slider->SetLabel("Music Volume");
        music_slider->SetShowValue(true);
        music_slider->SetValueChangedCallback([this](const float value) {
            music_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetVolume(audio::AudioType::Music, value);
            SaveSettings();
        });
        music_slider_ = music_slider.get();
        settings_panel_->AddChild(std::move(music_slider));

        // Create SFX Volume Slider
        auto sfx_slider = std::make_unique<engine::ui::elements::Slider>(
            0, SLIDER_START_Y + 2 * (SLIDER_HEIGHT + SLIDER_SPACING),
            content_width, SLIDER_HEIGHT,
            0.0f, 1.0f, sfx_volume_
        );
        sfx_slider->SetLabel("Sound Effects");
        sfx_slider->SetShowValue(true);
        sfx_slider->SetValueChangedCallback([this](const float value) {
            sfx_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetVolume(audio::AudioType::SFX, value);
            audio_mgr.PlaySFX(audio::AudioCue::MenuConfirm);
            SaveSettings();
        });
        sfx_slider_ = sfx_slider.get();
        settings_panel_->AddChild(std::move(sfx_slider));

        // Create Mute All Checkbox
        auto mute_all_checkbox = std::make_unique<engine::ui::elements::Checkbox>(
            0, CHECKBOX_START_Y, "Mute All Audio"
        );
        mute_all_checkbox->SetChecked(mute_all_);
        mute_all_checkbox->SetToggleCallback([this](const bool checked) {
            mute_all_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_all_checkbox_ = mute_all_checkbox.get();
        settings_panel_->AddChild(std::move(mute_all_checkbox));

        // Create Mute Music Checkbox
        auto mute_music_checkbox = std::make_unique<engine::ui::elements::Checkbox>(
            0, CHECKBOX_START_Y + CHECKBOX_HEIGHT + CHECKBOX_SPACING,
            "Mute Music"
        );
        mute_music_checkbox->SetChecked(mute_music_);
        mute_music_checkbox->SetToggleCallback([this](const bool checked) {
            mute_music_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_music_checkbox_ = mute_music_checkbox.get();
        settings_panel_->AddChild(std::move(mute_music_checkbox));

        // Create Mute SFX Checkbox
        auto mute_sfx_checkbox = std::make_unique<engine::ui::elements::Checkbox>(
            0, CHECKBOX_START_Y + 2 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING),
            "Mute Sound Effects"
        );
        mute_sfx_checkbox->SetChecked(mute_sfx_);
        mute_sfx_checkbox->SetToggleCallback([this](const bool checked) {
            mute_sfx_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_sfx_checkbox_ = mute_sfx_checkbox.get();
        settings_panel_->AddChild(std::move(mute_sfx_checkbox));

        // Create Enable Ambient Checkbox
        auto enable_ambient_checkbox = std::make_unique<engine::ui::elements::Checkbox>(
            0, CHECKBOX_START_Y + 3 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING),
            "Enable Ambient Sounds"
        );
        enable_ambient_checkbox->SetChecked(enable_ambient_);
        enable_ambient_checkbox->SetToggleCallback([this](const bool checked) {
            enable_ambient_ = checked;
            SaveSettings();
        });
        enable_ambient_checkbox_ = enable_ambient_checkbox.get();
        settings_panel_->AddChild(std::move(enable_ambient_checkbox));

        // Create Back Button - center it within content area
        const float button_width = 150.0f;
        const float button_x = (content_width - button_width) / 2.0f;
        auto back_button = std::make_unique<engine::ui::elements::Button>(
            button_x, BACK_BUTTON_Y,
            button_width, UITheme::BUTTON_HEIGHT_LARGE,
            "Back", UITheme::FONT_SIZE_LARGE
        );
        back_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        back_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        back_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        back_button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        back_button->SetClickCallback([this](const engine::ui::MouseEvent& event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                if (back_callback_) {
                    back_callback_();
                }
                return true;
            }
            return false;
        });
        back_button_ = back_button.get();
        settings_panel_->AddChild(std::move(back_button));

        UpdateLayout();
        ApplyAudioSettings();
    }

    void AudioSettingsMenu::Shutdown() {
        settings_panel_.reset();
        master_slider_ = nullptr;
        music_slider_ = nullptr;
        sfx_slider_ = nullptr;
        mute_all_checkbox_ = nullptr;
        mute_music_checkbox_ = nullptr;
        mute_sfx_checkbox_ = nullptr;
        enable_ambient_checkbox_ = nullptr;
        back_button_ = nullptr;
    }

    void AudioSettingsMenu::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        
        if (settings_panel_ != nullptr) {
            const int panel_x = (screen_width - MENU_WIDTH) / 2;
            const int panel_y = (screen_height - MENU_HEIGHT) / 2;
            settings_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void AudioSettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void AudioSettingsMenu::SyncWithAudioManager() {
        const auto& audio_mgr = audio::AudioManager::GetInstance();
        if (audio_mgr.IsInitialized()) {
            master_volume_ = audio_mgr.GetMasterVolume();
            music_volume_ = audio_mgr.GetVolume(audio::AudioType::Music);
            sfx_volume_ = audio_mgr.GetVolume(audio::AudioType::SFX);
            
            // Update slider values without triggering callbacks
            if (master_slider_) master_slider_->SetValue(master_volume_);
            if (music_slider_) music_slider_->SetValue(music_volume_);
            if (sfx_slider_) sfx_slider_->SetValue(sfx_volume_);
        }
    }

    void AudioSettingsMenu::Render() const {
        if (last_screen_width_ != GetScreenWidth() || last_screen_height_ != GetScreenHeight()) {
            const_cast<AudioSettingsMenu*>(this)->UpdateLayout();
        }

        // Render dim overlay
        RenderDimOverlay();

        // Render the panel (which renders its own background, border, and all children)
        settings_panel_->Render();
    }

    void AudioSettingsMenu::RenderDimOverlay() const {
        // Dim the background
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
    }

    void AudioSettingsMenu::HandleKeyboard() {
        // Navigate up/down through interactive elements
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selected_index_--;
            if (selected_index_ < 0) {
                selected_index_ = 7; // 3 sliders + 4 checkboxes + 1 button = 8 elements (0-7)
            }
        }
        
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selected_index_++;
            if (selected_index_ > 7) {
                selected_index_ = 0;
            }
        }

        // ESC to go back
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (back_callback_) {
                back_callback_();
            }
        }
    }

    bool AudioSettingsMenu::ProcessMouseEvent(const MouseEvent& event) {
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

    void AudioSettingsMenu::LoadSettings() {
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        master_volume_ = prefs.GetMasterVolume();
        music_volume_ = prefs.GetMusicVolume();
        sfx_volume_ = prefs.GetSFXVolume();
        mute_all_ = prefs.GetMuteAll();
        mute_music_ = prefs.GetMuteMusic();
        mute_sfx_ = prefs.GetMuteSFX();
        enable_ambient_ = prefs.GetEnableAmbient();
    }

    void AudioSettingsMenu::SaveSettings() const {
        auto& prefs = towerforge::core::UserPreferences::GetInstance();
        prefs.SetMasterVolume(master_volume_);
        prefs.SetMusicVolume(music_volume_);
        prefs.SetSFXVolume(sfx_volume_);
        prefs.SetMuteAll(mute_all_);
        prefs.SetMuteMusic(mute_music_);
        prefs.SetMuteSFX(mute_sfx_);
        prefs.SetEnableAmbient(enable_ambient_);
    }

    void AudioSettingsMenu::ApplyAudioSettings() {
        const float effective_master = mute_all_ ? 0.0f : master_volume_;
        SetMasterVolume(effective_master);
        SyncWithAudioManager();
    }

}
