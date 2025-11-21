#pragma once

#include <memory>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/save_load_manager.hpp"
#include "core/achievement_manager.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"
#include "ui/history_panel.h"
#include "ui/main_menu.h"
#include "ui/pause_menu.h"
#include "ui/save_load_menu.h"
#include "ui/research_tree_menu.h"
#include "ui/achievements_menu.h"
#include "ui/general_settings_menu.h"
#include "ui/audio_settings_menu.h"
#include "ui/accessibility_settings_menu.h"
#include "ui/tutorial_manager.h"
#include "ui/mods_menu.h"
#include "ui/help_system.h"
#include "ui/analytics_overlay.h"
#include "audio/audio_manager.h"

// Forward declare scene classes
namespace towerforge::core {
	class GameScene;
	class TitleScene;
	class AchievementsScene;
	class SettingsScene;
	class CreditsScene;
	class InGameScene;
	class TutorialScene;
}

namespace towerforge::core {
	/**
 * @brief Game state enumeration
 */
	enum class GameState {
		TitleScreen,
		Tutorial,
		InGame,
		Settings,
		LoadGame,
		Credits,
		Achievements,
		Quit
	};

	/**
 * @brief Main game class that manages game states and logic
 */
	class Game {
	public:
		Game();

		~Game();

		/**
	 * @brief Initialize the game
	 * @return true if initialization succeeded, false otherwise
	 */
		bool Initialize();

		/**
	 * @brief Run the main game loop
	 */
		void Run() const;

		/**
	 * @brief Shutdown and cleanup
	 */
		void Shutdown();

		// Public accessors for scenes
		rendering::Renderer &GetRenderer() { return renderer_; }
		ui::AchievementsMenu &GetAchievementsMenu() { return achievements_menu_; }
		ui::MainMenu &GetMainMenu() { return main_menu_; }

		void SetGameState(GameState state);

		GameState GetGameState() const { return current_state_; }
		audio::AudioManager *GetAudioManager() const { return audio_manager_; }
		AchievementManager *GetAchievementManager() const { return achievement_manager_.get(); }

	private:
		void TransitionToState(GameState new_state);

		// Game state
		GameState current_state_;
		GameState previous_state_;

		// Core systems (always active)
		rendering::Renderer renderer_;
		audio::AudioManager *audio_manager_;

		// Scene management
		GameScene *active_scene_;
		std::unique_ptr<TitleScene> title_scene_;
		std::unique_ptr<AchievementsScene> achievements_scene_;
		std::unique_ptr<SettingsScene> settings_scene_;
		std::unique_ptr<CreditsScene> credits_scene_;
		std::unique_ptr<InGameScene> ingame_scene_;
		std::unique_ptr<TutorialScene> tutorial_scene_;

		// UI for title screen (kept for backward compatibility during transition)
		ui::MainMenu main_menu_;
		ui::AchievementsMenu achievements_menu_;
		ui::GeneralSettingsMenu general_settings_menu_;
		ui::AudioSettingsMenu audio_settings_menu_;
		ui::AccessibilitySettingsMenu accessibility_settings_menu_;

		// Achievement manager (shared across scenes)
		std::unique_ptr<AchievementManager> achievement_manager_;

		ui::GameState game_state_;
		ui::GeneralSettingsMenu pause_general_settings_menu_;
		ui::AudioSettingsMenu pause_audio_settings_menu_;
		ui::AccessibilitySettingsMenu pause_accessibility_settings_menu_;
	};
}
