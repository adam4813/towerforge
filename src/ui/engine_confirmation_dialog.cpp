#include "ui/engine_confirmation_dialog.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"

import engine;

namespace towerforge::ui {
    EngineConfirmationDialog::EngineConfirmationDialog(
        const std::string &title,
        const std::string &message,
        const std::string &confirm_text,
        const std::string &cancel_text)
        : title_(title)
          , message_(message)
          , confirm_text_(confirm_text)
          , cancel_text_(cancel_text)
          , visible_(false)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , title_text_(nullptr)
          , message_text_(nullptr)
          , confirm_button_(nullptr)
          , cancel_button_(nullptr) {
    }

    EngineConfirmationDialog::~EngineConfirmationDialog() = default;

    void EngineConfirmationDialog::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        std::uint32_t screen_width;
        std::uint32_t screen_height;

        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Calculate centered position
        const float panel_x = static_cast<float>((screen_width - DIALOG_WIDTH) / 2);
        const float panel_y = static_cast<float>((screen_height - DIALOG_HEIGHT) / 2);

        // Create main panel
        panel_ = std::make_unique<Panel>();
        panel_->SetRelativePosition(panel_x, panel_y);
        panel_->SetSize(static_cast<float>(DIALOG_WIDTH), static_cast<float>(DIALOG_HEIGHT));
        panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_MODAL, 0.98f)));
        panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        panel_->SetPadding(0);

        // Title
        auto title = std::make_unique<Text>(
            0, 0,
            title_,
            UITheme::FONT_SIZE_LARGE,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        title->SetRelativePosition(
            static_cast<float>(UITheme::PADDING_LARGE),
            static_cast<float>(UITheme::PADDING_LARGE)
        );
        title_text_ = title.get();
        panel_->AddChild(std::move(title));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetRelativePosition(
            static_cast<float>(UITheme::PADDING_LARGE),
            static_cast<float>(UITheme::PADDING_LARGE + UITheme::FONT_SIZE_LARGE + 8)
        );
        divider->SetSize(DIALOG_WIDTH - UITheme::PADDING_LARGE * 2, 1);
        divider->SetColor(UITheme::ToEngineColor(UITheme::BORDER_SUBTLE));
        panel_->AddChild(std::move(divider));

        // Message
        auto msg = std::make_unique<Text>(
            0, 0,
            message_,
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
        );
        msg->SetRelativePosition(
            static_cast<float>(UITheme::PADDING_LARGE),
            static_cast<float>(UITheme::PADDING_LARGE + UITheme::FONT_SIZE_LARGE + 20)
        );
        message_text_ = msg.get();
        panel_->AddChild(std::move(msg));

        // Button positions - centered at bottom
        constexpr float button_y = static_cast<float>(DIALOG_HEIGHT - BUTTON_HEIGHT - UITheme::PADDING_LARGE);
        constexpr float total_button_width = BUTTON_WIDTH * 2 + UITheme::PADDING_MEDIUM;
        constexpr float button_start_x = (DIALOG_WIDTH - total_button_width) / 2.0f;

        // Confirm button
        auto confirm_btn = std::make_unique<Button>(
            BUTTON_WIDTH, BUTTON_HEIGHT,
            confirm_text_,
            UITheme::FONT_SIZE_NORMAL
        );
        confirm_btn->SetRelativePosition(button_start_x, button_y);
        confirm_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::SUCCESS, 0.6f)));
        confirm_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::SUCCESS, 0.8f)));
        confirm_btn->SetPressedColor(UITheme::ToEngineColor(UITheme::SUCCESS));
        confirm_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::SUCCESS));
        confirm_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        confirm_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                if (confirm_callback_) {
                    confirm_callback_();
                }
                Hide();
                return true;
            }
            return false;
        });
        confirm_button_ = confirm_btn.get();
        panel_->AddChild(std::move(confirm_btn));

        // Cancel button
        auto cancel_btn = std::make_unique<Button>(
            BUTTON_WIDTH, BUTTON_HEIGHT,
            cancel_text_,
            UITheme::FONT_SIZE_NORMAL
        );
        cancel_btn->SetRelativePosition(button_start_x + BUTTON_WIDTH + UITheme::PADDING_MEDIUM, button_y);
        cancel_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::ERROR, 0.5f)));
        cancel_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::ERROR, 0.7f)));
        cancel_btn->SetPressedColor(UITheme::ToEngineColor(UITheme::ERROR));
        cancel_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::ERROR));
        cancel_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        cancel_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                if (cancel_callback_) {
                    cancel_callback_();
                }
                Hide();
                return true;
            }
            return false;
        });
        cancel_button_ = cancel_btn.get();
        panel_->AddChild(std::move(cancel_btn));

        panel_->InvalidateComponents();
        panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void EngineConfirmationDialog::Show() {
        visible_ = true;
        animation_time_ = 0.0f;
        UpdateLayout();
    }

    void EngineConfirmationDialog::Hide() {
        visible_ = false;
    }

    void EngineConfirmationDialog::Update(const float delta_time) {
        if (!visible_) return;

        animation_time_ += delta_time;

        // Check for screen resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        if (panel_) {
            panel_->UpdateComponentsRecursive(delta_time);
        }
    }

    void EngineConfirmationDialog::UpdateLayout() {
        if (!panel_) return;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        const float panel_x = static_cast<float>((screen_width - DIALOG_WIDTH) / 2);
        const float panel_y = static_cast<float>((screen_height - DIALOG_HEIGHT) / 2);

        panel_->SetRelativePosition(panel_x, panel_y);
        panel_->InvalidateComponents();
        panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void EngineConfirmationDialog::Render() const {
        if (!visible_ || !panel_) return;

        // Draw dim overlay behind dialog
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );

        panel_->Render();
        panel_->RenderComponentsRecursive();
    }

    bool EngineConfirmationDialog::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_ || !panel_) return false;

        panel_->ProcessMouseEvent(event);

        // Modal behavior: consume all events when visible
        return true;
    }

    void EngineConfirmationDialog::SetMessage(const std::string &message) {
        message_ = message;
        if (message_text_) {
            message_text_->SetText(message);
        }
    }

    void EngineConfirmationDialog::SetTitle(const std::string &title) {
        title_ = title;
        if (title_text_) {
            title_text_->SetText(title);
        }
    }
}
