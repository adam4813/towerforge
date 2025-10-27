#include "ui/audio_settings_menu.h"
#include "ui/ui_theme.h"
#include "ui/panel_header_overlay.h"
#include "ui/dim_overlay.h"
#include "audio/audio_manager.h"
#include "core/user_preferences.hpp"

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

        // Create main panel with background and border
        const Color background_color = ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f);
        settings_panel_ = std::make_unique<Panel>(
            0, 0, MENU_WIDTH, MENU_HEIGHT,
            background_color,
            UITheme::BORDER_ACCENT
        );
        
        // Create header overlay
        header_overlay_ = std::make_unique<PanelHeaderOverlay>("AUDIO SETTINGS");
        
        // Create dim overlay
        dim_overlay_ = std::make_unique<DimOverlay>();

        // Create Master Volume Slider - positioned relative to panel (0,0)
        auto master_slider = std::make_unique<Slider>(
            50, SLIDER_START_Y, 
            MENU_WIDTH - 150, SLIDER_HEIGHT,
            0.0f, 1.0f, "Master Volume"
        );
        master_slider->SetValue(master_volume_);
        master_slider->SetValueChangedCallback([this](const float value) {
            master_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetMasterVolume(value);
            SaveSettings();
        });
        master_slider_ = master_slider.get();
        interactive_elements_.push_back(master_slider_);
        settings_panel_->AddChild(std::move(master_slider));

        // Create Music Volume Slider - positioned relative to panel (0,0)
        auto music_slider = std::make_unique<Slider>(
            50, SLIDER_START_Y + SLIDER_HEIGHT + SLIDER_SPACING,
            MENU_WIDTH - 150, SLIDER_HEIGHT,
            0.0f, 1.0f, "Music Volume"
        );
        music_slider->SetValue(music_volume_);
        music_slider->SetValueChangedCallback([this](const float value) {
            music_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetVolume(audio::AudioType::Music, value);
            SaveSettings();
        });
        music_slider_ = music_slider.get();
        interactive_elements_.push_back(music_slider_);
        settings_panel_->AddChild(std::move(music_slider));

        // Create SFX Volume Slider - positioned relative to panel (0,0)
        auto sfx_slider = std::make_unique<Slider>(
            50, SLIDER_START_Y + 2 * (SLIDER_HEIGHT + SLIDER_SPACING),
            MENU_WIDTH - 150, SLIDER_HEIGHT,
            0.0f, 1.0f, "Sound Effects"
        );
        sfx_slider->SetValue(sfx_volume_);
        sfx_slider->SetValueChangedCallback([this](const float value) {
            sfx_volume_ = value;
            auto& audio_mgr = audio::AudioManager::GetInstance();
            audio_mgr.SetVolume(audio::AudioType::SFX, value);
            audio_mgr.PlaySFX(audio::AudioCue::MenuConfirm);
            SaveSettings();
        });
        sfx_slider_ = sfx_slider.get();
        interactive_elements_.push_back(sfx_slider_);
        settings_panel_->AddChild(std::move(sfx_slider));

        // Create Mute All Checkbox - positioned relative to panel (0,0)
        auto mute_all_checkbox = std::make_unique<Checkbox>(
            50, CHECKBOX_START_Y, "Mute All Audio"
        );
        mute_all_checkbox->SetChecked(mute_all_);
        mute_all_checkbox->SetToggleCallback([this](const bool checked) {
            mute_all_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_all_checkbox_ = mute_all_checkbox.get();
        interactive_elements_.push_back(mute_all_checkbox_);
        settings_panel_->AddChild(std::move(mute_all_checkbox));

        // Create Mute Music Checkbox - positioned relative to panel (0,0)
        auto mute_music_checkbox = std::make_unique<Checkbox>(
            50, CHECKBOX_START_Y + CHECKBOX_HEIGHT + CHECKBOX_SPACING,
            "Mute Music"
        );
        mute_music_checkbox->SetChecked(mute_music_);
        mute_music_checkbox->SetToggleCallback([this](const bool checked) {
            mute_music_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_music_checkbox_ = mute_music_checkbox.get();
        interactive_elements_.push_back(mute_music_checkbox_);
        settings_panel_->AddChild(std::move(mute_music_checkbox));

        // Create Mute SFX Checkbox - positioned relative to panel (0,0)
        auto mute_sfx_checkbox = std::make_unique<Checkbox>(
            50, CHECKBOX_START_Y + 2 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING),
            "Mute Sound Effects"
        );
        mute_sfx_checkbox->SetChecked(mute_sfx_);
        mute_sfx_checkbox->SetToggleCallback([this](const bool checked) {
            mute_sfx_ = checked;
            ApplyAudioSettings();
            SaveSettings();
        });
        mute_sfx_checkbox_ = mute_sfx_checkbox.get();
        interactive_elements_.push_back(mute_sfx_checkbox_);
        settings_panel_->AddChild(std::move(mute_sfx_checkbox));

        // Create Enable Ambient Checkbox - positioned relative to panel (0,0)
        auto enable_ambient_checkbox = std::make_unique<Checkbox>(
            50, CHECKBOX_START_Y + 3 * (CHECKBOX_HEIGHT + CHECKBOX_SPACING),
            "Enable Ambient Sounds"
        );
        enable_ambient_checkbox->SetChecked(enable_ambient_);
        enable_ambient_checkbox->SetToggleCallback([this](const bool checked) {
            enable_ambient_ = checked;
            SaveSettings();
        });
        enable_ambient_checkbox_ = enable_ambient_checkbox.get();
        interactive_elements_.push_back(enable_ambient_checkbox_);
        settings_panel_->AddChild(std::move(enable_ambient_checkbox));

        // Create Back Button - positioned relative to panel (0,0)
        auto back_button = std::make_unique<Button>(
            (MENU_WIDTH - 150) / 2, BACK_BUTTON_Y,
            150, UITheme::BUTTON_HEIGHT_LARGE,
            "Back",
            UITheme::BUTTON_BACKGROUND,
            UITheme::BUTTON_BORDER_ACCENT
        );
        back_button->SetFontSize(UITheme::FONT_SIZE_LARGE);
        back_button->SetTextColor(UITheme::TEXT_SECONDARY);
        back_button->SetClickCallback([this]() {
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
            if (back_callback_) {
                back_callback_();
            }
        });
        back_button_ = back_button.get();
        interactive_elements_.push_back(back_button_);
        settings_panel_->AddChild(std::move(back_button));

        UpdateLayout();
        UpdateSelection(selected_index_);
        ApplyAudioSettings();
    }

    AudioSettingsMenu::~AudioSettingsMenu() = default;

    void AudioSettingsMenu::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;
        
        settings_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void AudioSettingsMenu::UpdateSelection(const int new_selection) {
        // Clear old selection
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(interactive_elements_.size())) {
            interactive_elements_[selected_index_]->SetFocused(false);
        }
        
        // Set new selection
        selected_index_ = new_selection;
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(interactive_elements_.size())) {
            interactive_elements_[selected_index_]->SetFocused(true);
        }
    }

    void AudioSettingsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
        
        // Update panel
        settings_panel_->Update(delta_time);

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
        
        // Update sliders (for dragging state)
        if (master_slider_) master_slider_->Update(delta_time);
        if (music_slider_) music_slider_->Update(delta_time);
        if (sfx_slider_) sfx_slider_->Update(delta_time);
        if (back_button_) back_button_->Update(delta_time);
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

    void AudioSettingsMenu::Render() {
        animation_time_ += GetFrameTime();

        if (last_screen_width_ != GetScreenWidth() || last_screen_height_ != GetScreenHeight()) {
            UpdateLayout();
        }

        // Render dim overlay
        dim_overlay_->Render();

        // Render the panel (which renders its own background, border, and all children)
        settings_panel_->Render();

        // Render header on top of panel
        RenderHeader();
    }

    void AudioSettingsMenu::RenderHeader() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;

        header_overlay_->Render(panel_x, panel_y, MENU_WIDTH);
    }

    void AudioSettingsMenu::HandleKeyboard() {
        // Navigate up/down through interactive elements
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            int new_selection = selected_index_ - 1;
            if (new_selection < 0) {
                new_selection = static_cast<int>(interactive_elements_.size()) - 1;
            }
            UpdateSelection(new_selection);
        }
        
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            int new_selection = selected_index_ + 1;
            if (new_selection >= static_cast<int>(interactive_elements_.size())) {
                new_selection = 0;
            }
            UpdateSelection(new_selection);
        }

        // Let focused element handle its own input
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(interactive_elements_.size())) {
            UIElement* focused = interactive_elements_[selected_index_];
            
            // Try as Slider first
            if (auto* slider = dynamic_cast<Slider*>(focused)) {
                slider->HandleKeyboard();
            }
            // Try as Checkbox
            else if (auto* checkbox = dynamic_cast<Checkbox*>(focused)) {
                checkbox->HandleKeyboard();
            }
            // Try as Button
            else if (auto* button = dynamic_cast<Button*>(focused)) {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    MouseEvent fake_event(0, 0, false, false, true, false);
                    button->OnClick(fake_event);
                }
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
        const bool consumed = settings_panel_->ProcessMouseEvent(event);

        // Update selection based on hover
        for (size_t i = 0; i < interactive_elements_.size(); ++i) {
            if (interactive_elements_[i]->IsHovered()) {
                UpdateSelection(static_cast<int>(i));
                break;
            }
        }

        return consumed;
    }

    void AudioSettingsMenu::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        // Legacy wrapper - delegates to modern API
        const MouseEvent event(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            false,
            false,
            clicked,
            false
        );
        ProcessMouseEvent(event);
    }

    void AudioSettingsMenu::LoadSettings() {
        auto& prefs = TowerForge::Core::UserPreferences::GetInstance();
        master_volume_ = prefs.GetMasterVolume();
        music_volume_ = prefs.GetMusicVolume();
        sfx_volume_ = prefs.GetSFXVolume();
        mute_all_ = prefs.GetMuteAll();
        mute_music_ = prefs.GetMuteMusic();
        mute_sfx_ = prefs.GetMuteSFX();
        enable_ambient_ = prefs.GetEnableAmbient();
    }

    void AudioSettingsMenu::SaveSettings() const {
        auto& prefs = TowerForge::Core::UserPreferences::GetInstance();
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
