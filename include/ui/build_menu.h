#pragma once

#include <functional>
#include <raylib.h>
#include <string>
#include <vector>
#include <memory>

namespace towerforge::ui {
    // Forward declarations
    class TooltipManager;
    class Panel;
    class Button;
    class TabBar;
    class GridPanel;
    struct MouseEvent;

    /**
     * @brief Facility categories for organized building menu
     */
    enum class FacilityCategory {
        Core,           // Lobby, Elevator
        Commercial,     // Shop, Restaurant
        Residential,    // Hotel
        Entertainment,  // Gym, Arcade, Theater
        Professional    // Office, Conference, Flagship
    };

    /**
 * @brief Structure representing a buildable facility type
 */
    struct FacilityType {
        std::string name;
        std::string icon;
        int cost;
        int width;
        Color color;
        FacilityCategory category;

        FacilityType(const std::string &n, const std::string &i, const int c, const int w, const Color col, const FacilityCategory cat)
            : name(n), icon(i), cost(c), width(w), color(col), category(cat) {
        }
    };

    /**
 * @brief Build menu for selecting and placing facilities
 * 
 * Refactored to use declarative, event-driven architecture with:
 * - Category tabs for facility organization
 * - Scrollable grid layout (6 per row, 2 rows visible)
 * - Callback-based facility selection
 * - Close button to clear placement
 */
    class BuildMenu {
    public:
        /**
         * @brief Callback type for facility selection
         */
        using FacilitySelectedCallback = std::function<void(int facility_index)>;
        
        /**
         * @brief Callback type for menu close
         */
        using CloseCallback = std::function<void()>;

        BuildMenu();

        ~BuildMenu();

        /**
     * @brief Render the build menu
     * @param can_undo Whether undo is available
     * @param can_redo Whether redo is available
     * @param demolish_mode Whether demolish mode is active
     */
        void Render(bool can_undo = false, bool can_redo = false, bool demolish_mode = false) const;

        /**
     * @brief Handle mouse click
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param can_undo Whether undo is available
     * @param can_redo Whether redo is available
     * @return Index of selected facility type, -2 for demolish, -3 for undo, -4 for redo, -5 for add floor, -6 for add basement, -1 for nothing
     */
        int HandleClick(int mouse_x, int mouse_y, bool can_undo = false, bool can_redo = false);

        /**
     * @brief Get the currently selected facility type
     * @return Index of selected facility, or -1 if none selected
     */
        int GetSelectedFacility() const { return selected_facility_; }

        /**
     * @brief Clear selection
     */
        void ClearSelection() { selected_facility_ = -1; }

        /**
     * @brief Get facility types
     */
        const std::vector<FacilityType> &GetFacilityTypes() const { return facility_types_; }

        /**
     * @brief Set visibility of build menu
     */
        void SetVisible(const bool visible) { visible_ = visible; }

        /**
     * @brief Check if build menu is visible
     */
        bool IsVisible() const { return visible_; }

        /**
     * @brief Set tutorial mode and highlight facility
     * @param tutorial_mode Whether tutorial mode is active
     * @param highlight_facility Name of facility to highlight (empty for none)
     */
        void SetTutorialMode(const bool tutorial_mode, const std::string &highlight_facility = "") {
            tutorial_mode_ = tutorial_mode;
            highlighted_facility_ = highlight_facility;
        }

        /**
     * @brief Update tooltips based on mouse position
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param current_funds Current player funds for dynamic tooltips
     */
        void UpdateTooltips(int mouse_x, int mouse_y, float current_funds) const;

        /**
     * @brief Render tooltips
     */
        void RenderTooltips() const;

        /**
     * @brief Set the tooltip manager
     */
        void SetTooltipManager(TooltipManager *tooltip_manager) { tooltip_manager_ = tooltip_manager; }

        /**
         * @brief Set facility selected callback
         */
        void SetFacilitySelectedCallback(const FacilitySelectedCallback &callback) { facility_selected_callback_ = callback; }

        /**
         * @brief Set close callback
         */
        void SetCloseCallback(const CloseCallback &callback) { close_callback_ = callback; }

        /**
         * @brief Update menu state
         */
        void Update(float delta_time) const;

        /**
         * @brief Handle mouse events
         */
        bool ProcessMouseEvent(const MouseEvent& event) const;

    private:
        void BuildUI();
        void SwitchCategory(FacilityCategory category);
        std::vector<int> GetFacilitiesForCategory(FacilityCategory category) const;
        void CreateFacilityButtons(const std::vector<int>& facility_indices);
        std::string GetCategoryName(FacilityCategory category) const;
        std::vector<FacilityType> facility_types_;
        int selected_facility_;
        bool visible_;
        bool tutorial_mode_;
        std::string highlighted_facility_;
        TooltipManager *tooltip_manager_;
        
        // Callbacks
        FacilitySelectedCallback facility_selected_callback_;
        CloseCallback close_callback_;
        
        // Current state
        FacilityCategory current_category_;
        
        // UI Components
        std::unique_ptr<Panel> root_panel_;           // Main container
        TabBar* tab_bar_;             // Category tabs
        GridPanel* facility_grid_;    // Scrollable facility grid
        std::unique_ptr<Button> close_button_;
        
        // Layout constants
        static constexpr int MENU_WIDTH = 800;
        static constexpr int MENU_HEIGHT = 300;
        static constexpr int TAB_HEIGHT = 40;
        static constexpr int FACILITY_BUTTON_SIZE = 120;
        static constexpr int FACILITIES_PER_ROW = 6;
        static constexpr int VISIBLE_ROWS = 2;
        static constexpr int GRID_PADDING = 10;
        static constexpr int CLOSE_BUTTON_SIZE = 30;
    };
}
