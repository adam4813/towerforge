#pragma once

#include <raylib.h>
#include <vector>
#include <string>

namespace TowerForge::Core {
    class CommandHistory;
}

namespace towerforge::ui {

    /**
     * @brief Display entry for history panel
     */
    struct HistoryDisplayEntry {
        std::string description;
        std::string time_str;
        int cost_change;
        bool is_redo;  // true if from redo stack, false if from undo stack

        HistoryDisplayEntry(const std::string& desc, const std::string& time, const int cost, const bool redo)
            : description(desc), time_str(time), cost_change(cost), is_redo(redo) {}
    };

    /**
     * @brief History panel UI for viewing and navigating action history
     * 
     * Displays a list of undoable and redoable actions with timestamps,
     * allowing users to click on entries to undo/redo to that point.
     */
    class HistoryPanel {
    public:
        HistoryPanel();
        ~HistoryPanel();

        /**
         * @brief Render the history panel
         */
        void Render();

        /**
         * @brief Handle mouse click
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @return Number of steps to undo (positive) or redo (negative), or 0 for no action
         */
        int HandleClick(int mouse_x, int mouse_y) const;

        /**
         * @brief Update the history display from the command history
         * @param history The command history to display
         */
        void UpdateFromHistory(const TowerForge::Core::CommandHistory& history);

        /**
         * @brief Set visibility of history panel
         */
        void SetVisible(const bool visible) { visible_ = visible; }

        /**
         * @brief Check if history panel is visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Toggle visibility
         */
        void ToggleVisible() { visible_ = !visible_; }

        /**
         * @brief Check if mouse is over the panel
         */
        bool IsMouseOver(int mouse_x, int mouse_y) const;

    private:
        std::vector<HistoryDisplayEntry> entries_;
        bool visible_;
        int scroll_offset_;
        int hovered_index_;

        // Layout constants
        static constexpr int PANEL_WIDTH = 280;
        static constexpr int PANEL_MAX_HEIGHT = 400;
        static constexpr int ITEM_HEIGHT = 50;
        static constexpr int PADDING = 10;
        static constexpr int HEADER_HEIGHT = 30;

        Rectangle panel_bounds_;
        Rectangle content_bounds_;
        int visible_items_count_;
    };

}
