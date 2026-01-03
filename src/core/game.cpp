#include <iostream>
#include "core/game.h"
#include "core/components.hpp"
#include "core/ecs_world.hpp"
#include "core/user_preferences.hpp"
#include "core/scenes/title_scene.hpp"
#include "core/scenes/achievements_scene.hpp"
#include "core/scenes/settings_scene.hpp"
#include "core/scenes/credits_scene.hpp"
#include "core/scenes/ingame_scene.hpp"
#include "core/scenes/tutorial_scene.hpp"
#include "ui/notification_center.h"
#include "ui/action_bar.h"
#include "ui/speed_control_panel.h"
#include "ui/mouse_interface.h"
#include "ui/batch_renderer/batch_renderer.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace towerforge::ui;
using namespace towerforge::rendering;

namespace towerforge::core {
	Game::Game()
		: current_state_(GameState::TitleScreen)
		  , previous_state_(GameState::TitleScreen)
		  , audio_manager_(nullptr)
		  , active_scene_(nullptr) {
		game_state_.funds = 25000.0f;
		game_state_.income_rate = 500.0f;
		game_state_.population = 2;
		game_state_.current_day = 1;
		game_state_.current_time = 8.5f;
		game_state_.speed_multiplier = 1;
		game_state_.paused = false;
	}

	Game::~Game() {
		if (active_scene_) {
			active_scene_->Shutdown();
			active_scene_ = nullptr;
		}
	}

	bool Game::Initialize() {
		std::cout << "TowerForge - Tower Simulation Game" << std::endl;
		std::cout << "Version: 0.1.0" << std::endl;
		std::cout << "Initializing Raylib renderer..." << std::endl;

		// Load user preferences first (this happens in the singleton constructor)
		auto &preferences = UserPreferences::GetInstance();
		std::cout << "User preferences loaded" << std::endl;

		// Initialize renderer
		renderer_.Initialize(800, 600, "TowerForge");

		// TODO: Remove once raylib is removed and we can call engine.Initialize()
		gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		const auto renderer = &engine::rendering::GetRenderer();
		// Initialize renderer (if needed)
		renderer->Initialize(800, 600);

		// Initialize batch renderer
		batch_renderer::BatchRenderer::Initialize();
		std::cout << "Batch renderer initialized" << std::endl;

		// Set up main menu callback
		main_menu_.SetStateChangeCallback([this](const GameState new_state) {
			current_state_ = new_state;
		});

		// Initialize audio system
		audio_manager_ = &audio::AudioManager::GetInstance();
		audio_manager_->Initialize();

		// Apply audio preferences from loaded settings
		audio_manager_->SetMasterVolume(preferences.GetMasterVolume());
		audio_manager_->SetVolume(audio::AudioType::Music, preferences.GetMusicVolume());
		audio_manager_->SetVolume(audio::AudioType::SFX, preferences.GetSFXVolume());

		// Create achievement manager for persistent achievements
		achievement_manager_ = std::make_unique<AchievementManager>();
		achievement_manager_->Initialize();

		// Set achievement manager for achievements menu
		achievements_menu_.SetAchievementManager(achievement_manager_.get());

		// Transition to initial scene
		TransitionToState(GameState::TitleScreen);

		// Play main theme music (volume already set from preferences)
		audio_manager_->PlayMusic(audio::AudioCue::MainTheme, true, 1.0f);

		std::cout << "User preferences applied to all systems" << std::endl;


		SetExitKey(0);

		return true;
	}

	void Game::SetGameState(GameState state) {
		if (state != current_state_) {
			deferred_events_.push([this, state]() {
				TransitionToState(state);
			});
		}
	}

	void Game::ProcessDeferredEvents() {
		while (!deferred_events_.empty()) {
			auto event = std::move(deferred_events_.front());
			deferred_events_.pop();
			event();
		}
	}

	void Game::TransitionToState(GameState new_state) {
		// Shutdown current scene
		if (active_scene_) {
			active_scene_->Shutdown();
			active_scene_ = nullptr;
		}

		previous_state_ = current_state_;
		current_state_ = new_state;

		// Initialize and activate new scene
		switch (current_state_) {
			case GameState::TitleScreen:
				if (!title_scene_) {
					title_scene_ = std::make_unique<TitleScene>(this, main_menu_);
				}
				title_scene_->Initialize();
				active_scene_ = title_scene_.get();
				break;

			case GameState::Achievements:
				if (!achievements_scene_) {
					achievements_scene_ = std::make_unique<AchievementsScene>(this, achievements_menu_);
					achievements_scene_->Initialize();
				}
				active_scene_ = achievements_scene_.get();
				break;

			case GameState::Settings:
				if (!settings_scene_) {
					settings_scene_ = std::make_unique<SettingsScene>(this, general_settings_menu_,
					                                                  audio_settings_menu_,
					                                                  accessibility_settings_menu_);
				}
				settings_scene_->Initialize();
				active_scene_ = settings_scene_.get();
				break;

			case GameState::Credits:
				if (!credits_scene_) {
					credits_scene_ = std::make_unique<CreditsScene>(this);
					credits_scene_->Initialize();
				}
				active_scene_ = credits_scene_.get();
				break;

			case GameState::InGame:
				if (!ingame_scene_) {
					ingame_scene_ = std::make_unique<InGameScene>(this);
				}
				ingame_scene_->Initialize();
				active_scene_ = ingame_scene_.get();
				break;

			case GameState::Tutorial:
				if (!tutorial_scene_) {
					tutorial_scene_ = std::make_unique<TutorialScene>(this);
				}
				tutorial_scene_->Initialize();
				active_scene_ = tutorial_scene_.get();
				break;

			case GameState::Quit:
				active_scene_ = nullptr;
				break;

			case GameState::LoadGame:
				// TODO: Implement load game scene
				active_scene_ = nullptr;
				break;
		}
	}

	void Game::Run() {
		while (current_state_ != GameState::Quit && !renderer_.ShouldClose()) {
			const float delta_time = GetFrameTime();

			// Update audio system
			audio_manager_->Update(delta_time);


			std::uint32_t screen_width;
			std::uint32_t screen_height;
			engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

			static std::uint32_t last_window_width = screen_width;
			static std::uint32_t last_window_height = screen_height;
			if (last_window_width != screen_width || last_window_height != screen_height) {
				const auto renderer = &engine::rendering::GetRenderer();
				renderer->SetWindowSize(screen_width, screen_height);
				last_window_width = screen_width;
				last_window_height = screen_height;
			}
			//engine.Update(delta_time);

			// Update and render active scene
			if (active_scene_) {
				active_scene_->Update(delta_time);
				active_scene_->HandleMouseEvent({
					static_cast<float>(GetMouseX()),
					static_cast<float>(GetMouseY()),
					IsMouseButtonDown(MOUSE_LEFT_BUTTON),
					IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
					IsMouseButtonPressed(MOUSE_LEFT_BUTTON),
					IsMouseButtonPressed(MOUSE_RIGHT_BUTTON),
					GetMouseWheelMove(),
					GetMouseWheelMove()
				});

				renderer_.BeginFrame();
				renderer_.Clear(DARKGRAY);
				active_scene_->Render();
				renderer_.EndFrame();
			}

			// Process any deferred events (like state transitions) after update/render
			ProcessDeferredEvents();
		}
	}

	void Game::Shutdown() {
		// Cleanup active scene
		if (active_scene_) {
			active_scene_->Shutdown();
			active_scene_ = nullptr;
		}

		engine.Shutdown();

		// Shutdown batch renderer
		batch_renderer::BatchRenderer::Shutdown();

		renderer_.Shutdown();
		std::cout << "Exiting TowerForge..." << std::endl;
	}
}
