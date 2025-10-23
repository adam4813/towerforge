#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "core/achievement_manager.hpp"

namespace towerforge::ui {

    /**
 * @brief Achievements menu for displaying unlocked and locked achievements
 * 
 * Shows a list of achievements with icons, names, descriptions,
 * unlock dates, and progress for locked achievements.
 */
    class AchievementsMenu {
    public:
        AchievementsMenu();
        ~AchievementsMenu();
    
        /**
     * @brief Render the achievements menu
     */
        void Render();
    
        /**
     * @brief Update menu state (called every frame)
     * @param delta_time Time elapsed since last frame
     */
        void Update(float delta_time);
    
        /**
     * @brief Handle keyboard input for menu navigation
     * @return True if ESC was pressed to close menu
     */
        bool HandleKeyboard();
    
        /**
     * @brief Handle mouse input for scrolling
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param mouse_wheel Mouse wheel delta
     */
        void HandleMouse(int mouse_x, int mouse_y, float mouse_wheel);
    
        /**
     * @brief Set the achievement manager to display
     * @param manager Pointer to achievement manager
     */
        void SetAchievementManager(TowerForge::Core::AchievementManager* manager);
    
        /**
     * @brief Set current game statistics for progress display
     * @param population Current population
     * @param total_income Total income earned
     * @param floor_count Number of floors
     * @param avg_satisfaction Average satisfaction
     */
        void SetGameStats(int population, float total_income, int floor_count, float avg_satisfaction);
    
    private:
        static void RenderBackground();
        void RenderHeader() const;
        void RenderAchievementList() const;
        void RenderAchievementItem(const TowerForge::Core::Achievement& achievement, int y_pos, bool is_unlocked) const;

        static void RenderFooter();
        std::string GetProgressText(const TowerForge::Core::Achievement& achievement) const;
    
        TowerForge::Core::AchievementManager* achievement_manager_;
    
        // Current game stats for progress calculation
        int current_population_;
        float current_income_;
        int current_floors_;
        float current_satisfaction_;
    
        // UI state
        float scroll_offset_;
        float animation_time_;
    
        // Layout constants
        static constexpr int MENU_WIDTH = 600;
        static constexpr int MENU_HEIGHT = 500;
        static constexpr int HEADER_HEIGHT = 60;
        static constexpr int FOOTER_HEIGHT = 40;
        static constexpr int ITEM_HEIGHT = 80;
        static constexpr int ITEM_PADDING = 10;
        static constexpr int SCROLL_SPEED = 30;
    };

}
