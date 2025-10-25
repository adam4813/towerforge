#pragma once

#include "ui_element.h"
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

namespace towerforge::ui {

    /**
     * @brief Action bar displayed at bottom of HUD
     * 
     * Contains buttons for main game actions like Build, Info, Staff Management, etc.
     * Uses Observer pattern to notify when actions are triggered.
     */
    class ActionBar : public Panel {
    public:
        /**
         * @brief Action type enumeration
         */
        enum class Action {
            Build,
            FacilityInfo,
            VisitorInfo,
            StaffManagement,
            Research,
            Settings
        };

        /**
         * @brief Callback type for action button clicks
         * @param action The action that was triggered
         */
        using ActionCallback = std::function<void(Action action)>;

        /**
         * @brief Construct an action bar
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the action bar
         * @param height Height of the action bar
         */
        ActionBar(float relative_x, float relative_y, float width, float height);

        /**
         * @brief Render the action bar
         */
        void Render() const override;

        /**
         * @brief Update action bar state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time) override;

        /**
         * @brief Set action callback
         */
        void SetActionCallback(const ActionCallback &callback) { action_callback_ = callback; }

        /**
         * @brief Set which action button is active/highlighted
         * @param action Action to highlight, or -1 for none
         */
        void SetActiveAction(Action action);

        /**
         * @brief Clear active action
         */
        void ClearActiveAction();

        /**
         * @brief Process mouse events
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event);

    private:
        void CreateActionButton(Action action, const std::string& label, float x);

        std::vector<Button*> action_buttons_;
        ActionCallback action_callback_;
        int active_action_index_;

        // Button sizing constants
        static constexpr int MIN_BUTTON_WIDTH = 60;   // Minimum button width before text truncates
        static constexpr int MAX_BUTTON_WIDTH = 140;  // Maximum button width
        static constexpr int BUTTON_SPACING = 10;
        static constexpr float MAX_BAR_WIDTH_PERCENT = 0.5f;  // 50% of screen width
        
    public:
        // Calculate the total width needed for the action bar based on screen width
        static int CalculateBarWidth() {
            const int screen_width = GetScreenWidth();
            const int max_bar_width = static_cast<int>(screen_width * MAX_BAR_WIDTH_PERCENT);
            
            // Calculate button width that fits within max bar width
            // Formula: bar_width = num_buttons * button_width + (num_buttons - 1) * spacing + padding
            constexpr int num_buttons = 6;
            constexpr int padding = 10;
            const int available_width = max_bar_width - (num_buttons - 1) * BUTTON_SPACING - padding;
            const int button_width = available_width / num_buttons;
            
            // Clamp button width between min and max
            const int clamped_button_width = std::clamp(button_width, MIN_BUTTON_WIDTH, MAX_BUTTON_WIDTH);
            
            return num_buttons * clamped_button_width + (num_buttons - 1) * BUTTON_SPACING + padding;
        }
        
        // Calculate individual button width based on screen width
        static int CalculateButtonWidth() {
            const int screen_width = GetScreenWidth();
            const int max_bar_width = static_cast<int>(screen_width * MAX_BAR_WIDTH_PERCENT);
            
            constexpr int num_buttons = 6;
            constexpr int padding = 10;
            const int available_width = max_bar_width - (num_buttons - 1) * BUTTON_SPACING - padding;
            const int button_width = available_width / num_buttons;
            
            return std::clamp(button_width, MIN_BUTTON_WIDTH, MAX_BUTTON_WIDTH);
        }
    };

}
