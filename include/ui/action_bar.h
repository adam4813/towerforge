#pragma once

#include "ui_element.h"
#include <vector>
#include <string>
#include <functional>

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

        static constexpr int BUTTON_WIDTH = 120;
        static constexpr int BUTTON_SPACING = 10;
        
    public:
        // Calculate the total width needed for the action bar
        static constexpr int CalculateBarWidth() {
            return 6 * BUTTON_WIDTH + 5 * BUTTON_SPACING + 10; // 6 buttons + spacing + padding
        }
    };

}
