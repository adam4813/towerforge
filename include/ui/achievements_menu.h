#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "core/achievement_manager.hpp"

import engine;

namespace towerforge::ui {
    struct MouseEvent;

    /**
     * @brief Achievements menu for displaying unlocked and locked achievements
     * 
     * Shows a list of achievements with icons, names, descriptions,
     * unlock dates, and progress for locked achievements.
     * Uses citrus engine UI elements and components.
     */
    class AchievementsMenu {
    public:
        using CloseCallback = std::function<void()>;

        AchievementsMenu();

        ~AchievementsMenu();

        /**
         * @brief Initialize the menu UI components
         */
        void Initialize();

        /**
         * @brief Shutdown and cleanup resources
         */
        void Shutdown();

        /**
         * @brief Render the achievements menu
         */
        void Render() const;

        /**
         * @brief Update menu state (called every frame)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent &event) const;

        /**
         * @brief Handle keyboard input for menu navigation
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        void HandleKeyboard() const;

        /**
         * @brief Set the callback for closing the menu
         * @param callback Function to call when back is pressed
         */
        void SetCloseCallback(const CloseCallback &callback);

        /**
         * @brief Set the achievement manager to display
         * @param manager Pointer to achievement manager
         */
        void SetAchievementManager(towerforge::core::AchievementManager *manager);

        /**
         * @brief Set current game statistics for progress display
         * @param population Current population
         * @param total_income Total income earned
         * @param floor_count Number of floors
         * @param avg_satisfaction Average satisfaction
         */
        void SetGameStats(int population, float total_income, int floor_count, float avg_satisfaction);

    private:
        void UpdateLayout();

        void RebuildAchievementList() const;

        /**
         * @brief Create a UI container for displaying an achievement item
         * @param achievement The achievement data to display
         * @param is_unlocked Whether the achievement is unlocked (affects styling and status text)
         * @param item_width Width of the item container
         * @return Unique pointer to the constructed container
         */
        std::unique_ptr<engine::ui::elements::Container> CreateAchievementItem(
            const towerforge::core::Achievement& achievement,
            bool is_unlocked,
            float item_width) const;

        std::string GetProgressText(const towerforge::core::Achievement &achievement) const;

        static void RenderDimOverlay();

        std::unique_ptr<engine::ui::elements::Panel> main_panel_{};
        engine::ui::elements::Container *achievement_list_container_{nullptr}; // Raw pointer, owned by main_panel_

        towerforge::core::AchievementManager *achievement_manager_{nullptr};
        CloseCallback close_callback_;

        // Current game stats for progress calculation
        int current_population_;
        float current_income_;
        int current_floors_;
        float current_satisfaction_;

        // UI state
        float animation_time_;
        int last_screen_width_;
        int last_screen_height_;

        // Layout constants
        static constexpr int MENU_WIDTH = 600;
        static constexpr int MENU_HEIGHT = 500;
        static constexpr int ITEM_HEIGHT = 80;
        static constexpr int ITEM_PADDING = 10;
        static constexpr int HEADER_HEIGHT = 80;
    };
}
