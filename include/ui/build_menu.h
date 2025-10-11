#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge {
namespace ui {

/**
 * @brief Structure representing a buildable facility type
 */
struct FacilityType {
    std::string name;
    std::string icon;
    int cost;
    int width;
    Color color;
    
    FacilityType(const std::string& n, const std::string& i, int c, int w, Color col)
        : name(n), icon(i), cost(c), width(w), color(col) {}
};

/**
 * @brief Build menu for selecting and placing facilities
 */
class BuildMenu {
public:
    BuildMenu();
    ~BuildMenu();
    
    /**
     * @brief Render the build menu
     * @param can_undo Whether undo is available
     * @param can_redo Whether redo is available
     * @param demolish_mode Whether demolish mode is active
     */
    void Render(bool can_undo = false, bool can_redo = false, bool demolish_mode = false);
    
    /**
     * @brief Handle mouse click
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param can_undo Whether undo is available
     * @param can_redo Whether redo is available
     * @return Index of selected facility type, -2 for demolish, -3 for undo, -4 for redo, -1 for nothing
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
    const std::vector<FacilityType>& GetFacilityTypes() const { return facility_types_; }
    
    /**
     * @brief Set visibility of build menu
     */
    void SetVisible(bool visible) { visible_ = visible; }
    
    /**
     * @brief Check if build menu is visible
     */
    bool IsVisible() const { return visible_; }
    
    /**
     * @brief Set tutorial mode and highlight facility
     * @param tutorial_mode Whether tutorial mode is active
     * @param highlight_facility Name of facility to highlight (empty for none)
     */
    void SetTutorialMode(bool tutorial_mode, const std::string& highlight_facility = "") {
        tutorial_mode_ = tutorial_mode;
        highlighted_facility_ = highlight_facility;
    }
    
private:
    std::vector<FacilityType> facility_types_;
    int selected_facility_;
    bool visible_;
    bool tutorial_mode_;
    std::string highlighted_facility_;
    
    static constexpr int MENU_WIDTH = 200;
    static constexpr int ITEM_HEIGHT = 40;
    static constexpr int MENU_PADDING = 5;
};

} // namespace ui
} // namespace towerforge
