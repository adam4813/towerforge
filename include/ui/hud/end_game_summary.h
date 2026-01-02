#pragma once

#include <memory>

import engine;

namespace towerforge::ui {
	struct GameState;

	/**
     * @brief End game summary overlay shown when max stars achieved
     * 
     * Declarative UI component following UI Development Bible patterns.
     * Modal overlay that displays final statistics.
     */
	class EndGameSummary {
	public:
		EndGameSummary();

		~EndGameSummary() = default;

		/**
         * @brief Update overlay position on resize
         */
		void Update(float delta_time);

		/**
         * @brief Render the end game summary
         */
		void Render() const;

		/**
         * @brief Initialize the menu UI components
         */
		void Initialize();

		/**
         * @brief Shutdown and cleanup resources
         */
		void Shutdown();

		/**
         * @brief Set the game state reference for display
         */
		void SetGameState(const GameState *state) { game_state_ = state; }

		/**
         * @brief Check if should be visible (max stars achieved)
         */
		bool ShouldShow() const;

		static constexpr int BOX_WIDTH = 700;
		static constexpr int BOX_HEIGHT = 450;

	private:
		static void RenderDimOverlay();

		void UpdateLayout();

		const GameState *game_state_;

		std::unique_ptr<engine::ui::elements::Panel> summary_panel_;
		int last_screen_width_;
		int last_screen_height_;
	};
} // namespace towerforge::ui