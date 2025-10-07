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
     */
    void Render();
    
    /**
     * @brief Handle mouse click
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @return Index of selected facility type, or -1 if nothing selected
     */
    int HandleClick(int mouse_x, int mouse_y);
    
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
    
private:
    std::vector<FacilityType> facility_types_;
    int selected_facility_;
    bool visible_;
    
    static constexpr int MENU_WIDTH = 200;
    static constexpr int ITEM_HEIGHT = 40;
    static constexpr int MENU_PADDING = 5;
};

} // namespace ui
} // namespace towerforge
