#include "ui/mods_menu.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <algorithm>
#include <sstream>

import engine;

namespace towerforge::ui {
    ModsMenu::ModsMenu()
        : visible_(false)
          , mod_manager_(nullptr)
          , selected_mod_index_(-1)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , last_mods_count_(0)
          , mods_list_container_(nullptr) {
    }

    ModsMenu::~ModsMenu() = default;

    void ModsMenu::SetModManager(core::LuaModManager *mod_manager) {
        mod_manager_ = mod_manager;
    }

    void ModsMenu::Show() {
        visible_ = true;
        selected_mod_index_ = -1;
        RebuildModsList();
    }

    void ModsMenu::Hide() {
        visible_ = false;
    }

    void ModsMenu::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Create main panel with vertical layout
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        main_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Add title
        auto title = std::make_unique<Text>(
            0, 0,
            "Mods Manager",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        main_panel_->AddChild(std::move(title));

        // Add divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(MENU_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Add instructions text
        auto instructions = std::make_unique<Text>(
            0, 0,
            "Click mods to toggle | ESC to close",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        main_panel_->AddChild(std::move(instructions));

        // Create scrollable container for mods list
        constexpr float list_width = MENU_WIDTH - UITheme::PADDING_LARGE * 2;
        constexpr float list_height = MENU_HEIGHT - HEADER_HEIGHT - 100;

        auto list_container = engine::ui::ContainerBuilder()
                .Size(list_width, list_height)
                .Layout<VerticalLayout>(MOD_ITEM_SPACING, Alignment::Center)
                .Scrollable(ScrollDirection::Vertical)
                .Padding(UITheme::PADDING_LARGE)
                .ClipChildren()
                .Build();

        mods_list_container_ = list_container.get();
        main_panel_->AddChild(std::move(list_container));

        // Add back button
        auto back_button = std::make_unique<Button>(
            UITheme::BUTTON_WIDTH_MEDIUM,
            UITheme::BUTTON_HEIGHT_MEDIUM,
            "Back",
            UITheme::FONT_SIZE_NORMAL
        );
        back_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        back_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        back_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        back_button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        back_button->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                Hide();
                return true;
            }
            return false;
        });
        main_panel_->AddChild(std::move(back_button));

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;

        UpdateLayout();
    }

    void ModsMenu::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (main_panel_) {
            const int panel_x = (screen_width - MENU_WIDTH) / 2;
            const int panel_y = (screen_height - MENU_HEIGHT) / 2;
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void ModsMenu::RebuildModsList() {
        if (!mods_list_container_ || !mod_manager_) {
            return;
        }

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Clear existing mod buttons
        mod_buttons_.clear();
        mods_list_container_->ClearChildren();

        const auto &mods = mod_manager_->GetLoadedMods();
        last_mods_count_ = mods.size();

        if (mods.empty()) {
            // Show "no mods" message
            auto no_mods_text = std::make_unique<Text>(
                0, 0,
                "No mods found in the mods/ directory",
                UITheme::FONT_SIZE_MEDIUM,
                UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
            );
            mods_list_container_->AddChild(std::move(no_mods_text));

            auto help_text = std::make_unique<Text>(
                0, 0,
                "Place .lua mod files in the mods/ folder",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(UITheme::TEXT_DISABLED)
            );
            mods_list_container_->AddChild(std::move(help_text));
        } else {
            // Create a button for each mod
            constexpr float item_width = MENU_WIDTH - UITheme::PADDING_LARGE * 4;

            for (std::size_t i = 0; i < mods.size(); ++i) {
                const auto &mod = mods[i];

                // Build the button label with mod info
                std::ostringstream label;
                label << mod.name;
                if (!mod.version.empty()) {
                    label << " v" << mod.version;
                }
                if (!mod.author.empty()) {
                    label << " by " << mod.author;
                }
                label << "\n";

                if (!mod.description.empty()) {
                    std::string desc = mod.description;
                    if (desc.length() > 60) {
                        desc = desc.substr(0, 57) + "...";
                    }
                    label << desc << "\n";
                }

                if (mod.loaded_successfully) {
                    label << (mod.enabled ? "[Enabled]" : "[Disabled]");
                } else {
                    label << "[Failed to load]";
                    if (!mod.error_message.empty()) {
                        std::string error = mod.error_message;
                        if (error.length() > 40) {
                            error = error.substr(0, 37) + "...";
                        }
                        label << " - " << error;
                    }
                }

                auto button = std::make_unique<Button>(
                    item_width,
                    MOD_ITEM_HEIGHT,
                    label.str(),
                    UITheme::FONT_SIZE_SMALL
                );

                // Set colors based on mod state
                if (mod.loaded_successfully) {
                    if (mod.enabled) {
                        button->SetBorderColor(UITheme::ToEngineColor(UITheme::SUCCESS));
                        button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
                    } else {
                        button->SetBorderColor(UITheme::ToEngineColor(UITheme::WARNING));
                        button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
                    }
                } else {
                    button->SetBorderColor(UITheme::ToEngineColor(UITheme::ERROR));
                    button->SetTextColor(UITheme::ToEngineColor(UITheme::ERROR));
                }

                button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
                button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));

                // Set click callback to toggle mod
                const std::string mod_id = mod.id;
                const bool is_enabled = mod.enabled;
                const bool loaded_ok = mod.loaded_successfully;

                button->SetClickCallback([this, mod_id, is_enabled, loaded_ok](const engine::ui::MouseEvent &event) {
                    if (event.left_pressed && loaded_ok) {
                        audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                        if (is_enabled) {
                            mod_manager_->DisableMod(mod_id);
                        } else {
                            mod_manager_->EnableMod(mod_id);
                        }
                        RebuildModsList();
                        return true;
                    }
                    return false;
                });

                // Set hover callback
                button->SetHoverCallback([this, idx = i, this_button = button.get()](const engine::ui::MouseEvent &) {
                    // Update selection visual
                    if (selected_mod_index_ >= 0 && selected_mod_index_ < static_cast<int>(mod_buttons_.size())) {
                        auto *old_button = mod_buttons_[selected_mod_index_];
                        old_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
                    }

                    selected_mod_index_ = static_cast<int>(idx);
                    this_button->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.2f)));
                    return true;
                });

                mod_buttons_.push_back(button.get());
                mods_list_container_->AddChild(std::move(button));
            }
        }

        mods_list_container_->InvalidateComponents();
        mods_list_container_->UpdateComponentsRecursive();

        // Update scroll component content size
        if (auto *scroll = mods_list_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void ModsMenu::Update(const float delta_time) {
        if (!visible_ || !main_panel_) {
            return;
        }

        // Check for window resize
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        // Check if mods list changed
        if (mod_manager_) {
            const auto &mods = mod_manager_->GetLoadedMods();
            if (mods.size() != last_mods_count_) {
                RebuildModsList();
            }
        }
    }

    void ModsMenu::Render() const {
        if (!visible_ || !main_panel_) {
            return;
        }

        RenderDimOverlay();
        main_panel_->Render();
    }

    void ModsMenu::RenderDimOverlay() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    bool ModsMenu::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_ || !main_panel_) {
            return false;
        }
        return main_panel_->ProcessMouseEvent(event);
    }

    void ModsMenu::HandleKeyboard() {
        if (!visible_) {
            return;
        }

        // Navigate up
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            if (!mod_buttons_.empty()) {
                int new_selection = selected_mod_index_ - 1;
                if (new_selection < 0) {
                    new_selection = static_cast<int>(mod_buttons_.size()) - 1;
                }
                if (new_selection >= 0 && new_selection < static_cast<int>(mod_buttons_.size())) {
                    auto *button = mod_buttons_[new_selection];
                    auto bounds = button->GetAbsoluteBounds();
                    button->OnHover({(bounds.x), (bounds.y)});
                }
            }
        }

        // Navigate down
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            if (!mod_buttons_.empty()) {
                int new_selection = selected_mod_index_ + 1;
                if (new_selection >= static_cast<int>(mod_buttons_.size())) {
                    new_selection = 0;
                }
                if (new_selection >= 0 && new_selection < static_cast<int>(mod_buttons_.size())) {
                    auto *button = mod_buttons_[new_selection];
                    auto bounds = button->GetAbsoluteBounds();
                    button->OnHover({(bounds.x), (bounds.y)});
                }
            }
        }

        // Toggle mod with Enter/Space
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (selected_mod_index_ >= 0 && selected_mod_index_ < static_cast<int>(mod_buttons_.size())) {
                auto *button = mod_buttons_[selected_mod_index_];
                const auto bounds = button->GetAbsoluteBounds();
                engine::ui::MouseEvent click_event{};
                click_event.x = bounds.x + 1;
                click_event.y = bounds.y + 1;
                click_event.left_pressed = true;
                button->OnClick(click_event);
            }
        }

        // ESC to close
        if (IsKeyPressed(KEY_ESCAPE)) {
            Hide();
        }
    }

    void ModsMenu::Shutdown() {
        mod_buttons_.clear();
        mods_list_container_ = nullptr;
        main_panel_.reset();
    }
}
