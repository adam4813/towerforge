#include "ui/pause_menu.h"
#include "audio/audio_manager.h"

namespace towerforge::ui {
    PauseMenu::PauseMenu()
        : selected_option_(0)
          , animation_time_(0.0f)
          , show_quit_confirmation_(false)
          , quit_confirmation_selection_(0)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , option_callback_(nullptr) {
        // Initialize menu items
        menu_items_.push_back({"Resume Game", PauseMenuOption::Resume});
        menu_items_.push_back({"Save Game", PauseMenuOption::SaveGame});
        menu_items_.push_back({"Load Game", PauseMenuOption::LoadGame});
        menu_items_.push_back({"Settings", PauseMenuOption::Settings});
        menu_items_.push_back({"Mods", PauseMenuOption::Mods});
        menu_items_.push_back({"Quit to Title", PauseMenuOption::QuitToTitle});
    }

    PauseMenu::~PauseMenu() = default;

    void PauseMenu::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Create main panel with vertical layout
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_height = HEADER_HEIGHT + static_cast<int>(menu_items_.size()) * (
                                    MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) - MENU_ITEM_SPACING;
        const int panel_y = (screen_height - menu_height) / 2;

        pause_panel_ = std::make_unique<engine::ui::elements::Panel>();
        pause_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        pause_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(menu_height));
        pause_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        pause_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        pause_panel_->SetBorderWidth(2.0f);
        pause_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        pause_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Add title text
        auto title_text = std::make_unique<Text>(
            0, 0,
            "PAUSED",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        pause_panel_->AddChild(std::move(title_text));

        // Add divider below title
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(MENU_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        pause_panel_->AddChild(std::move(divider));

        // Create button container with vertical layout
        constexpr float item_width = MENU_WIDTH - UITheme::PADDING_MEDIUM * 2;
        auto button_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(item_width,
                      static_cast<float>(MENU_ITEM_HEIGHT * menu_items_.size() + MENU_ITEM_SPACING * (
                                             menu_items_.size() - 1)))
                .Layout(std::make_unique<VerticalLayout>(static_cast<float>(MENU_ITEM_SPACING), Alignment::Center))
                .Build();

        // Create buttons for each menu item
        for (size_t i = 0; i < menu_items_.size(); ++i) {
            auto button = std::make_unique<engine::ui::elements::Button>(
                static_cast<int>(item_width),
                MENU_ITEM_HEIGHT,
                menu_items_[i].label,
                UITheme::FONT_SIZE_LARGE
            );

            button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
            button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
            button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
            button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));

            // Set click callback
            button->SetClickCallback([this, option = menu_items_[i].option](const engine::ui::MouseEvent &event) {
                if (event.left_pressed) {
                    audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                    if (option_callback_) {
                        option_callback_(option);
                    }
                    return true;
                }
                return false;
            });

            // Set hover callback
            button->SetHoverCallback([this, this_button = button.get()](const engine::ui::MouseEvent &) {
                // Clear old selection
                if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
                    const auto old_button = menu_item_buttons_[selected_option_];
                    old_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
                    old_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
                    old_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
                }

                // Set new selection
                for (std::size_t j = 0; j < menu_item_buttons_.size(); ++j) {
                    if (menu_item_buttons_[j] == this_button) {
                        this_button->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
                        this_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
                        this_button->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
                        selected_option_ = static_cast<int>(j);
                        return true;
                    }
                }

                return false;
            });

            menu_item_buttons_.push_back(button.get());
            button_container->AddChild(std::move(button));
        }

        // Auto-hover the first button
        if (!menu_item_buttons_.empty()) {
            menu_item_buttons_[0]->OnHover({0.5f, 0.5f});
        }

        pause_panel_->AddChild(std::move(button_container));

        // Build confirmation dialog
        BuildConfirmationDialog();

        UpdateLayout();
    }

    void PauseMenu::BuildConfirmationDialog() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        constexpr int dialog_width = 500;
        constexpr int dialog_height = 250;
        const int dialog_x = (screen_width - dialog_width) / 2;
        const int dialog_y = (screen_height - dialog_height) / 2;

        confirmation_panel_ = std::make_unique<Panel>();
        confirmation_panel_->SetRelativePosition(static_cast<float>(dialog_x), static_cast<float>(dialog_y));
        confirmation_panel_->SetSize(static_cast<float>(dialog_width), static_cast<float>(dialog_height));
        confirmation_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        confirmation_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        confirmation_panel_->SetBorderWidth(2.0f);
        confirmation_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        confirmation_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_MEDIUM, Alignment::Center)
        );

        // Warning icon
        auto warning_icon = std::make_unique<Text>(
            0, 0, "!", 40, UITheme::ToEngineColor(ORANGE)
        );
        confirmation_panel_->AddChild(std::move(warning_icon));

        // Title
        auto title = std::make_unique<Text>(
            0, 0, "Quit to Title Screen?", UITheme::FONT_SIZE_LARGE, UITheme::ToEngineColor(WHITE)
        );
        confirmation_panel_->AddChild(std::move(title));

        // Message
        auto message = std::make_unique<Text>(
            0, 0, "Any unsaved progress will be lost.", UITheme::FONT_SIZE_MEDIUM, UITheme::ToEngineColor(LIGHTGRAY)
        );
        confirmation_panel_->AddChild(std::move(message));

        // Button container with horizontal layout
        constexpr int button_width = 150;
        constexpr int button_height = 45;
        constexpr int button_spacing = 30;

        auto button_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(static_cast<float>(button_width * 2 + button_spacing), static_cast<float>(button_height))
                .Layout(std::make_unique<HorizontalLayout>(static_cast<float>(button_spacing), Alignment::Center))
                .Build();

        // Cancel button
        auto cancel_btn = std::make_unique<Button>(
            button_width, button_height, "Cancel", UITheme::FONT_SIZE_MEDIUM
        );
        cancel_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(GRAY, 0.3f)));
        cancel_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(GRAY, 0.5f)));
        cancel_btn->SetBorderColor(UITheme::ToEngineColor(WHITE));
        cancel_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        cancel_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                show_quit_confirmation_ = false;
                quit_confirmation_selection_ = 0;
                return true;
            }
            return false;
        });
        cancel_btn->SetHoverCallback([this](const engine::ui::MouseEvent &) {
            quit_confirmation_selection_ = 0;
            UpdateConfirmationButtonStyles();
            return true;
        });
        cancel_button_ = cancel_btn.get();
        button_container->AddChild(std::move(cancel_btn));

        // Confirm button
        auto confirm_btn = std::make_unique<Button>(
            button_width, button_height, "Quit", UITheme::FONT_SIZE_MEDIUM
        );
        confirm_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.3f)));
        confirm_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.5f)));
        confirm_btn->SetBorderColor(UITheme::ToEngineColor(RED));
        confirm_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        confirm_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && quit_confirmation_callback_) {
                show_quit_confirmation_ = false;
                quit_confirmation_selection_ = 1;
                quit_confirmation_callback_();
                return true;
            }
            return false;
        });
        confirm_btn->SetHoverCallback([this](const engine::ui::MouseEvent &) {
            quit_confirmation_selection_ = 1;
            UpdateConfirmationButtonStyles();
            return true;
        });
        confirm_button_ = confirm_btn.get();
        button_container->AddChild(std::move(confirm_btn));

        confirmation_panel_->AddChild(std::move(button_container));

        // Instruction text
        auto instruction = std::make_unique<Text>(
            0, 0, "LEFT/RIGHT to Select | ENTER to Confirm | ESC to Cancel",
            UITheme::FONT_SIZE_SMALL, UITheme::ToEngineColor(DARKGRAY)
        );
        confirmation_panel_->AddChild(std::move(instruction));

        confirmation_panel_->InvalidateComponents();
        confirmation_panel_->UpdateComponentsRecursive();
    }

    void PauseMenu::UpdateConfirmationButtonStyles() const {
        if (cancel_button_) {
            const bool cancel_selected = quit_confirmation_selection_ == 0;
            cancel_button_->SetNormalColor(UITheme::ToEngineColor(
                cancel_selected ? ColorAlpha(GRAY, 0.5f) : ColorAlpha(DARKGRAY, 0.3f)));
            cancel_button_->SetBorderColor(UITheme::ToEngineColor(cancel_selected ? WHITE : GRAY));
        }
        if (confirm_button_) {
            const bool confirm_selected = quit_confirmation_selection_ == 1;
            confirm_button_->SetNormalColor(UITheme::ToEngineColor(
                confirm_selected ? ColorAlpha(RED, 0.5f) : ColorAlpha(DARKGRAY, 0.3f)));
            confirm_button_->SetBorderColor(UITheme::ToEngineColor(confirm_selected ? RED : GRAY));
        }
    }

    void PauseMenu::UpdateConfirmationLayout() {
        if (!confirmation_panel_) return;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        constexpr int dialog_width = 500;
        constexpr int dialog_height = 250;
        const int dialog_x = (screen_width - dialog_width) / 2;
        const int dialog_y = (screen_height - dialog_height) / 2;

        confirmation_panel_->SetRelativePosition(static_cast<float>(dialog_x), static_cast<float>(dialog_y));
        confirmation_panel_->InvalidateComponents();
        confirmation_panel_->UpdateComponentsRecursive();
    }

    void PauseMenu::Shutdown() {
        menu_item_buttons_.clear();
        pause_panel_.reset();
        cancel_button_ = nullptr;
        confirm_button_ = nullptr;
        confirmation_panel_.reset();
    }

    void PauseMenu::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        if (pause_panel_ != nullptr) {
            const int panel_x = (screen_width - MENU_WIDTH) / 2;
            const int menu_height = HEADER_HEIGHT + static_cast<int>(menu_items_.size()) * (
                                        MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) - MENU_ITEM_SPACING;
            const int panel_y = (screen_height - menu_height) / 2;
            pause_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            pause_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(menu_height));
            pause_panel_->InvalidateComponents();
            pause_panel_->UpdateComponentsRecursive();
        }

        UpdateConfirmationLayout();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void PauseMenu::Update(const float delta_time) {
        animation_time_ += delta_time;

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        // Animate selected button
        if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
            auto *button = menu_item_buttons_[selected_option_];
            const float pulse = sin(animation_time_ * UITheme::ANIMATION_SPEED_NORMAL) * 0.1f + 0.9f;
            button->SetTextColor(UITheme::ToEngineColor(ColorAlpha(UITheme::TEXT_PRIMARY, pulse)));
        }
    }

    void PauseMenu::Render() const {
        if (show_quit_confirmation_) {
            RenderDimOverlay();
            RenderQuitConfirmation();
        } else {
            RenderDimOverlay();
            if (pause_panel_) {
                pause_panel_->Render();
            }
            RenderIndicator();
        }
    }

    void PauseMenu::RenderDimOverlay() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, static_cast<float>(screen_width), static_cast<float>(screen_height)),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    void PauseMenu::RenderIndicator() const {
        if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
            const auto *button = menu_item_buttons_[selected_option_];
            const auto bounds = button->GetAbsoluteBounds();
            const int indicator_x = static_cast<int>(bounds.x) - UITheme::PADDING_LARGE;
            const int indicator_font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_LARGE);
            const int indicator_y = static_cast<int>(bounds.y + bounds.height / 2) - indicator_font_size / 2;

            engine::ui::BatchRenderer::SubmitText(">", static_cast<float>(indicator_x), static_cast<float>(indicator_y),
                                                  indicator_font_size, UITheme::ToEngineColor(UITheme::PRIMARY));
        }
    }

    void PauseMenu::HandleKeyboard() const {
        if (show_quit_confirmation_ && HandleQuitConfirmation() != 0) {
            return;
        }

        int new_selection = selected_option_;

        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            new_selection--;
            if (new_selection < 0) {
                new_selection = static_cast<int>(menu_items_.size()) - 1;
            }
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            new_selection++;
            if (new_selection >= static_cast<int>(menu_items_.size())) {
                new_selection = 0;
            }
        }

        if (new_selection != selected_option_ && new_selection >= 0 &&
            new_selection < static_cast<int>(menu_item_buttons_.size())) {
            auto *button = menu_item_buttons_[new_selection];
            auto button_position = button->GetAbsoluteBounds();
            button->OnHover({button_position.x, button_position.y});
        }

        // Select option with Enter/Space
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_items_.size())) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                if (option_callback_) {
                    option_callback_(menu_items_[selected_option_].option);
                }
            }
        }

        // ESC to resume
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (option_callback_) {
                option_callback_(PauseMenuOption::Resume);
            }
        }
    }

    bool PauseMenu::ProcessMouseEvent(const MouseEvent &event) const {
        const auto mouse_event = engine::ui::MouseEvent{
            event.x,
            event.y,
            event.left_down,
            event.right_down,
            event.left_pressed,
            event.right_pressed
        };
        if (show_quit_confirmation_ && confirmation_panel_->ProcessMouseEvent(mouse_event)) {
            return true;
        }

        if (pause_panel_) {
            return pause_panel_->ProcessMouseEvent(mouse_event);
        }

        return false;
    }

    void PauseMenu::RenderQuitConfirmation() const {
        if (confirmation_panel_) {
            UpdateConfirmationButtonStyles();
            confirmation_panel_->Render();
        }
    }

    int PauseMenu::HandleQuitConfirmation() const {
        if (!show_quit_confirmation_) {
            return -1;
        }

        // Navigate left/right
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            quit_confirmation_selection_ = 0; // Cancel
            UpdateConfirmationButtonStyles();
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            quit_confirmation_selection_ = 1; // Confirm
            UpdateConfirmationButtonStyles();
        }

        // Confirm selection
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            const int result = quit_confirmation_selection_;
            show_quit_confirmation_ = false;
            quit_confirmation_selection_ = 0;
            return result;
        }

        // Cancel with ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            show_quit_confirmation_ = false;
            quit_confirmation_selection_ = 0;
            return 0;
        }

        return -1;
    }
}
