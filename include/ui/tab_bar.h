#pragma once

#include "ui_element.h"
#include <string>
#include <vector>
#include <functional>

namespace towerforge::ui {

    /**
     * @brief Reusable tab bar component for category/view switching
     * 
     * Displays a horizontal row of tabs where one is always active.
     * Uses Observer pattern to notify when tabs are clicked.
     */
    class TabBar : public UIElement {
    public:
        /**
         * @brief Callback type for tab selection
         * @param tab_index Index of the selected tab
         */
        using TabSelectedCallback = std::function<void(int tab_index)>;

        /**
         * @brief Construct a tab bar
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Total width of the tab bar
         * @param height Height of the tab bar
         * @param tab_labels Labels for each tab
         */
        TabBar(float relative_x, float relative_y, float width, float height,
               const std::vector<std::string>& tab_labels);

        /**
         * @brief Render the tab bar
         */
        void Render() const override;

        /**
         * @brief Update tab bar state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time) const;

        /**
         * @brief Set tab selected callback
         */
        void SetTabSelectedCallback(const TabSelectedCallback &callback) { tab_selected_callback_ = callback; }

        /**
         * @brief Set the active tab
         * @param index Index of the tab to make active
         */
        void SetActiveTab(int index);

        /**
         * @brief Get the active tab index
         */
        int GetActiveTab() const { return active_tab_index_; }

        /**
         * @brief Get number of tabs
         */
        int GetTabCount() const { return static_cast<int>(tab_buttons_.size()); }

    private:
        std::vector<Button*> tab_buttons_;  // Raw pointers to child buttons
        int active_tab_index_;
        TabSelectedCallback tab_selected_callback_;
    };

}
