#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include "core/achievement_manager.hpp"

namespace towerforge::ui {

    // Forward declarations
    class Panel;
    class PanelHeaderOverlay;
    class DimOverlay;
    struct MouseEvent;

    /**
     * @brief Achievements menu for displaying unlocked and locked achievements
     * 
     * Shows a list of achievements with icons, names, descriptions,
     * unlock dates, and progress for locked achievements.
     * Now uses Panel architecture for proper event handling.
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
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event);
    
        /**
         * @brief Handle keyboard input for menu navigation
         * @return True if ESC was pressed to close menu
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        bool HandleKeyboard();
    
        /**
         * @brief Handle mouse input for scrolling
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param mouse_wheel Mouse wheel delta
         * @deprecated Use ProcessMouseEvent instead
         */
        void HandleMouse(int mouse_x, int mouse_y, float mouse_wheel);
    
        /**
         * @brief Set the achievement manager to display
         * @param manager Pointer to achievement manager
         */
        void SetAchievementManager(towerforge::core::AchievementManager* manager);
    
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
        void RenderAchievementList() const;
        void RenderAchievementItem(const towerforge::core::Achievement& achievement, int y_pos, int x, int width, bool is_unlocked) const;
        std::string GetProgressText(const towerforge::core::Achievement& achievement) const;
    
        std::unique_ptr<Panel> main_panel_;
        std::unique_ptr<PanelHeaderOverlay> header_overlay_;
        std::unique_ptr<DimOverlay> dim_overlay_;

        towerforge::core::AchievementManager* achievement_manager_;
    
        // Current game stats for progress calculation
        int current_population_;
        float current_income_;
        int current_floors_;
        float current_satisfaction_;
    
        // UI state
        float scroll_offset_;
        float animation_time_;
        int last_screen_width_;
        int last_screen_height_;
        mutable bool close_requested_;  // Set by back button, checked by HandleKeyboard
    
        // Layout constants
        static constexpr int MENU_WIDTH = 600;
        static constexpr int MENU_HEIGHT = 500;
        static constexpr int ITEM_HEIGHT = 80;
        static constexpr int ITEM_PADDING = 10;
        static constexpr int SCROLL_SPEED = 30;
    };

}
