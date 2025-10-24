#pragma once

#include "ui_element.h"
#include <vector>
#include <functional>
#include <memory>

namespace towerforge::ui {

    /**
     * @brief Reusable scrollable grid layout component
     * 
     * Arranges child elements in a grid with configurable columns.
     * Supports vertical scrolling when content exceeds visible area.
     * Items are added dynamically and positioned automatically.
     */
    class GridPanel : public Panel {
    public:
        /**
         * @brief Grid item data structure
         */
        struct GridItem {
            std::unique_ptr<Button> button;
            int data_index;  // Index into user's data array
        };

        /**
         * @brief Callback type for item selection
         * @param data_index Index of the selected item in user's data
         */
        using ItemSelectedCallback = std::function<void(int data_index)>;

        /**
         * @brief Construct a grid panel
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the panel
         * @param height Height of the panel
         * @param columns Number of columns in the grid
         * @param item_size Size of each grid item (square)
         * @param spacing Spacing between items
         */
        GridPanel(float relative_x, float relative_y, float width, float height,
                  int columns, float item_size, float spacing);

        /**
         * @brief Add an item to the grid
         * @param label Item label
         * @param data_index Index in user's data array
         * @param background_color Item background color
         * @return Pointer to created button
         */
        Button* AddItem(const std::string& label, int data_index, Color background_color);

        /**
         * @brief Clear all items from the grid
         */
        void ClearItems();

        /**
         * @brief Render the grid panel
         */
        void Render() const override;

        /**
         * @brief Update grid state and handle scrolling
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Set item selected callback
         */
        void SetItemSelectedCallback(const ItemSelectedCallback &callback) { item_selected_callback_ = callback; }

        /**
         * @brief Set which item is selected (visual state)
         * @param data_index Index of the selected item, or -1 for none
         */
        void SetSelectedItem(int data_index);

        /**
         * @brief Get the number of items in the grid
         */
        int GetItemCount() const { return static_cast<int>(items_.size()); }

    private:
        void RepositionItems();
        void UpdateScrollBounds();

        std::vector<GridItem> items_;
        int columns_;
        float item_size_;
        float spacing_;
        float scroll_offset_;
        float max_scroll_;
        int selected_item_index_;
        ItemSelectedCallback item_selected_callback_;
    };

}
