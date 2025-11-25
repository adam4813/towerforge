#include "ui/general_settings_menu.h"

#include "audio/audio_manager.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "core/accessibility_settings.hpp"

import engine;

namespace towerforge::ui {
	GeneralSettingsMenu::GeneralSettingsMenu()
		: selected_option_(0)
		  , animation_time_(0.0f)
		  , last_screen_width_(0)
		  , last_screen_height_(0)
		  , option_callback_(nullptr) {
		// Initialize menu items with their options
		menu_items_.push_back({"Audio Settings  >", SettingsOption::Audio});
		menu_items_.push_back({"Controls Settings  >", SettingsOption::Controls});
		menu_items_.push_back({"Display Settings  >", SettingsOption::Display});
		menu_items_.push_back({"Accessibility  >", SettingsOption::Accessibility});
		menu_items_.push_back({"Gameplay Settings  >", SettingsOption::Gameplay});
		menu_items_.push_back({"Back", SettingsOption::Back});
	}

	GeneralSettingsMenu::~GeneralSettingsMenu() = default;

	void GeneralSettingsMenu::SetOptionCallback(const OptionCallback &callback) {
		option_callback_ = callback;
	}

	void GeneralSettingsMenu::UpdateLayout() {
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();

		if (settings_panel_ != nullptr) {
			// Center the panel
			const int panel_x = (screen_width - MENU_WIDTH) / 2;
			const int menu_height = HEADER_Y + menu_items_.size() * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) +
			                        UITheme::PADDING_LARGE;
			settings_panel_->SetRelativePosition(static_cast<float>(panel_x),
			                                     static_cast<float>(HEADER_Y - UITheme::PADDING_LARGE));
			settings_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(menu_height));
		}

		last_screen_width_ = screen_width;
		last_screen_height_ = screen_height;
	}

	void GeneralSettingsMenu::Update(const float delta_time) {
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

	void GeneralSettingsMenu::Render() const {
		RenderDimOverlay();
		settings_panel_->Render();
		RenderIndicator();
	}

	void GeneralSettingsMenu::RenderDimOverlay() const {
		// Dim the background
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();
		DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
	}

	void GeneralSettingsMenu::RenderIndicator() const {
		if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
			const auto *button = menu_item_buttons_[selected_option_];
			const auto bounds = button->GetAbsoluteBounds();
			const int indicator_x = bounds.x - UITheme::PADDING_LARGE;
			const int indicator_font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_LARGE);
			const int indicator_y = bounds.y + bounds.height / 2 - indicator_font_size / 2;

			engine::ui::BatchRenderer::SubmitText(">", indicator_x, indicator_y, indicator_font_size,
			                                      UITheme::ToEngineColor(UITheme::PRIMARY));
		}
	}

	void GeneralSettingsMenu::HandleKeyboard() {
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


		if (new_selection != selected_option_ && new_selection >= 0 && new_selection < static_cast<int>(
			    menu_item_buttons_.size())) {
			auto *button = menu_item_buttons_[new_selection];
			auto button_position = button->GetAbsoluteBounds();
			button->OnHover({button_position.x, button_position.y});
		}

		// Select option with Enter/Space
		if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
			if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_items_.size())) {
				if (option_callback_) {
					option_callback_(menu_items_[selected_option_].option);
				}
			}
		}

		// ESC to go back
		if (IsKeyPressed(KEY_ESCAPE)) {
			if (option_callback_) {
				option_callback_(SettingsOption::Back);
			}
		}
	}

	bool GeneralSettingsMenu::ProcessMouseEvent(const MouseEvent &event) {
		return settings_panel_->ProcessMouseEvent({
			event.x,
			event.y,
			event.left_down,
			event.right_down,
			event.left_pressed,
			event.right_pressed
		});
	}

	void GeneralSettingsMenu::Shutdown() {
		menu_item_buttons_.clear();
		settings_panel_.reset();
	}

	void GeneralSettingsMenu::Initialize() {
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();

		// Create main panel
		const int panel_x = (screen_width - MENU_WIDTH) / 2;
		const int menu_height = HEADER_Y + menu_items_.size() * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) +
		                        UITheme::PADDING_LARGE;
		settings_panel_ = std::make_unique<engine::ui::elements::Panel>(
			panel_x,
			HEADER_Y - UITheme::PADDING_LARGE,
			MENU_WIDTH,
			menu_height
		);
		settings_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
		settings_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));

		// Add title text
		auto title_text = std::make_unique<engine::ui::elements::Text>(
			UITheme::PADDING_LARGE,
			UITheme::PADDING_MEDIUM,
			"Settings",
			UITheme::FONT_SIZE_TITLE,
			UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
		);
		settings_panel_->AddChild(std::move(title_text));

		// Create buttons for each menu item
		for (size_t i = 0; i < menu_items_.size(); ++i) {
			const int item_x = UITheme::PADDING_MEDIUM;
			const int item_y = HEADER_Y + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);

			auto button = std::make_unique<engine::ui::elements::Button>(
				item_x,
				item_y,
				MENU_WIDTH - UITheme::PADDING_MEDIUM * 2,
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
				if (selected_option_ >= 0 && selected_option_ < menu_item_buttons_.size()) {
					const auto old_button = menu_item_buttons_[selected_option_];
					old_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
					old_button->SetFontSize(UITheme::FONT_SIZE_LARGE);
					old_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
					old_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
				}

				// Set new selection
				for (std::size_t i = 0; i < menu_item_buttons_.size(); ++i) {
					if (menu_item_buttons_[i] == this_button) {
						this_button->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
						this_button->SetFontSize(UITheme::FONT_SIZE_LARGE);
						this_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
						this_button->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
						selected_option_ = i;
						return true;
					}
				}

				return false;
			});

			menu_item_buttons_.push_back(button.get());

			// Auto-hover the first button
			if (i == 0) {
				button->OnHover({item_x + 0.5f, item_y + 0.5f});
			}

			settings_panel_->AddChild(std::move(button));
		}

		UpdateLayout();
	}
}
