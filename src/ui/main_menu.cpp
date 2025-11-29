#include <cmath>
#include "ui/batch_renderer/batch_adapter.h"
#include "ui/main_menu.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "core/game.h"

import engine;

namespace towerforge::ui {
	MainMenu::MainMenu()
		: selected_option_(0)
		  , animation_time_(0.0f)
		  , last_screen_width_(0)
		  , last_screen_height_(0)
		  , state_change_callback_(nullptr) {
		// Initialize menu items with their target states
		menu_items_.push_back({"New Game", core::GameState::InGame});
		menu_items_.push_back({"Tutorial", core::GameState::Tutorial});
		menu_items_.push_back({"Load Game", core::GameState::InGame}); // TODO: Separate LoadGame state?
		menu_items_.push_back({"Achievements", core::GameState::Achievements});
		menu_items_.push_back({"Settings", core::GameState::Settings});
		menu_items_.push_back({"Credits", core::GameState::Credits});
		menu_items_.push_back({"Quit", core::GameState::Quit});
	}

	MainMenu::~MainMenu() = default;

	void MainMenu::SetStateChangeCallback(const StateChangeCallback &callback) {
		state_change_callback_ = callback;
	}

	void MainMenu::UpdateLayout() {
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();

		if (main_panel_ != nullptr) {
			main_panel_->SetSize(static_cast<float>(screen_width), static_cast<float>(screen_height));
			main_panel_->InvalidateComponents();
			main_panel_->UpdateComponentsRecursive();
		}

		// Cache screen size to detect changes
		last_screen_width_ = screen_width;
		last_screen_height_ = screen_height;
	}

	void MainMenu::Update(const float delta_time) {
		animation_time_ += delta_time;

		// Check for window resize and update layout if needed
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();
		if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
			UpdateLayout();
		}

		if (selected_option_ >= 0 && selected_option_ < static_cast<int>(menu_item_buttons_.size())) {
			auto *button = menu_item_buttons_[selected_option_];
			const float pulse = sin(animation_time_ * UITheme::ANIMATION_SPEED_NORMAL) * 0.1f + 0.9f;
			button->SetTextColor(UITheme::ToEngineColor(ColorAlpha(UITheme::TEXT_PRIMARY, pulse)));
		}
	}

	void MainMenu::Render() const {
		RenderBackground();
		main_panel_->Render();
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

	void MainMenu::RenderBackground() const {
		// Clear with theme background color
		ClearBackground(UITheme::BACKGROUND_DARK);

		// Draw subtle grid pattern in background
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();
		for (int i = 0; i < screen_height; i += 40) {
			batch_renderer::adapter::DrawLine(0, i, screen_width, i, UITheme::DECORATIVE_GRID);
		}
		for (int i = 0; i < screen_width; i += 40) {
			batch_renderer::adapter::DrawLine(i, 0, i, screen_height, UITheme::DECORATIVE_GRID);
		}

		// Draw simple tower silhouettes on the sides
		batch_renderer::adapter::DrawRectangle(50, 300, 60, 300, ColorAlpha(UITheme::DECORATIVE_WINDOW, 0.3f));
		batch_renderer::adapter::DrawRectangle(screen_width - 110, 250, 60, 350,
		                                       ColorAlpha(UITheme::DECORATIVE_WINDOW, 0.3f));

		// Add some "windows" to the silhouettes
		for (int y = 320; y < 580; y += 30) {
			for (int x = 60; x < 100; x += 20) {
				const float pulse = sin(animation_time_ * 2.0f + y * 0.1f) * 0.5f + 0.5f;
				batch_renderer::adapter::DrawRectangle(x, y, 10, 15, ColorAlpha(YELLOW, 0.2f + pulse * 0.1f));
			}
		}
		for (int y = 270; y < 580; y += 30) {
			for (int x = screen_width - 100; x < screen_width - 60; x += 20) {
				const float pulse = sin(animation_time_ * 2.0f + y * 0.1f + 1.0f) * 0.5f + 0.5f;
				batch_renderer::adapter::DrawRectangle(x, y, 10, 15, ColorAlpha(YELLOW, 0.2f + pulse * 0.1f));
			}
		}
	}

	void MainMenu::HandleKeyboard() const {
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

		// Select option
		if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
			state_change_callback_(menu_items_[selected_option_].target_state);
		}

		if (IsKeyPressed(KEY_ESCAPE)) {
			state_change_callback_(core::GameState::Quit);
		}
	}

	bool MainMenu::ProcessMouseEvent(const MouseEvent &event) const {
		return main_panel_->ProcessMouseEvent({
			event.x,
			event.y,
			event.left_down,
			event.right_down,
			event.left_pressed,
			event.right_pressed
		});
	}

	void MainMenu::Shutdown() {
		menu_item_buttons_.clear();
		main_panel_.reset();
	}

	void MainMenu::Initialize() {
		using namespace engine::ui::components;
		using namespace engine::ui::elements;

		main_panel_ = std::make_unique<engine::ui::elements::Panel>();
		main_panel_->SetSize(static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()));
		main_panel_->SetOpacity(0);
		main_panel_->SetPadding(64.0f);
		main_panel_->AddComponent<LayoutComponent>(
			std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
		);

		const auto title = "TOWERFORGE";
		auto title_container_stack = engine::ui::ContainerBuilder()
				.Opacity(0)
				.Size(MeasureText(title, UITheme::FONT_SIZE_TITLE) + 3, UITheme::FONT_SIZE_TITLE + 3)
				.Layout(std::make_unique<StackLayout>())
				.Build();
		auto title_shadow_text = std::make_unique<Text>(
			0, 0,
			title,
			UITheme::FONT_SIZE_TITLE,
			UITheme::ToEngineColor(ColorAlpha(BLACK, 0.5f))
		);
		title_container_stack->AddChild(std::move(title_shadow_text));
		auto title_text = std::make_unique<Text>(
			0, 0,
			title,
			UITheme::FONT_SIZE_TITLE,
			UITheme::ToEngineColor(UITheme::PRIMARY)
		);
		title_container_stack->AddChild(std::move(title_text));

		main_panel_->AddChild(std::move(title_container_stack));

		auto divider = std::make_unique<Divider>();
		divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
		divider->SetSize(UITheme::ClampSize(UITheme::ResponsiveWidth(0.35f), 300, 500), 2);
		main_panel_->AddChild(std::move(divider));

		auto tagline_text = std::make_unique<Text>(
			0, 0,
			"\"Build, Survive, Thrive!\"",
			UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_MEDIUM),
			UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
		);
		main_panel_->AddChild(std::move(tagline_text));

		constexpr float item_width = 250.0f;
		constexpr float item_height = 40.0f;

		auto button_container = engine::ui::ContainerBuilder()
				.Opacity(0)
				.Size(item_width, item_height * menu_items_.size() + UITheme::MARGIN_SMALL * menu_items_.size() - 1)
				.Layout(std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center))
				.Build();
		for (auto &[label, target_state]: menu_items_) {
			auto button = std::make_unique<engine::ui::elements::Button>(
				item_width, item_height, label, UITheme::FONT_SIZE_MEDIUM);
			button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
			button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
			button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
			button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));

			button->SetClickCallback([this, state = target_state](const engine::ui::MouseEvent &event) {
				if (!event.left_pressed) return false;

				audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
				state_change_callback_(state);
				return true;
			});
			button->SetHoverCallback([this, this_button = button.get()](const engine::ui::MouseEvent &) {
				// Clear old selection
				if (selected_option_ >= 0 && selected_option_ < menu_item_buttons_.size()) {
					const auto old_selected_button = menu_item_buttons_[selected_option_];
					old_selected_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
					old_selected_button->SetFontSize(UITheme::FONT_SIZE_MEDIUM);
					old_selected_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
					old_selected_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
				}

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
			button_container->AddChild(std::move(button));
		}

		// Auto-hover the first button, before adding to container
		menu_item_buttons_[0]->OnHover({0.5f, 0.5f});

		main_panel_->AddChild(std::move(button_container));

		auto version_text = std::make_unique<Text>(
			0, 0,
			"Version: " + GetVersion(),
			UITheme::FONT_SIZE_NORMAL,
			UITheme::ToEngineColor(UITheme::BORDER_DEFAULT)
		);
		main_panel_->AddChild(std::move(version_text));

		UpdateLayout();
	}
}
