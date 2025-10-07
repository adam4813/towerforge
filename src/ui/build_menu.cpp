#include "ui/build_menu.h"
#include <sstream>

namespace towerforge {
namespace ui {

BuildMenu::BuildMenu() 
    : selected_facility_(-1)
    , visible_(true) {
    
    // Initialize facility types with costs and properties
    facility_types_.emplace_back("Lobby", "L", 1000, 10, GOLD);
    facility_types_.emplace_back("Office", "O", 5000, 8, SKYBLUE);
    facility_types_.emplace_back("Restaurant", "R", 8000, 6, RED);
    facility_types_.emplace_back("Shop", "S", 6000, 4, GREEN);
    facility_types_.emplace_back("Hotel", "H", 12000, 10, PURPLE);
    facility_types_.emplace_back("Elevator", "E", 15000, 2, GRAY);
}

BuildMenu::~BuildMenu() {
}

void BuildMenu::Render() {
    if (!visible_) {
        return;
    }
    
    int screen_height = GetScreenHeight();
    int menu_x = 10;
    int menu_y = 60;  // Below top bar
    
    int menu_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT + MENU_PADDING * 2;
    
    // Draw menu background
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, menu_height, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, 2, GOLD);
    
    // Draw title
    DrawText("BUILD MENU", menu_x + MENU_PADDING, menu_y + MENU_PADDING, 14, WHITE);
    
    int y = menu_y + MENU_PADDING + 20;
    
    // Draw facility types
    for (size_t i = 0; i < facility_types_.size(); ++i) {
        const auto& facility = facility_types_[i];
        
        // Highlight selected item
        if (static_cast<int>(i) == selected_facility_) {
            DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, 
                         ColorAlpha(WHITE, 0.2f));
        }
        
        // Draw icon with facility color
        DrawRectangle(menu_x + MENU_PADDING + 5, y + 5, 25, 25, facility.color);
        DrawText(facility.icon.c_str(), menu_x + MENU_PADDING + 12, y + 10, 16, WHITE);
        
        // Draw name
        DrawText(facility.name.c_str(), menu_x + MENU_PADDING + 35, y + 5, 14, WHITE);
        
        // Draw cost
        std::stringstream cost_ss;
        cost_ss << "$" << facility.cost;
        DrawText(cost_ss.str().c_str(), menu_x + MENU_PADDING + 35, y + 20, 12, GREEN);
        
        y += ITEM_HEIGHT;
    }
    
    // Draw hint at bottom
    DrawText("Click to select, then click grid to place", 
             menu_x + MENU_PADDING, y + 5, 10, LIGHTGRAY);
}

int BuildMenu::HandleClick(int mouse_x, int mouse_y) {
    if (!visible_) {
        return -1;
    }
    
    int menu_x = 10;
    int menu_y = 60;
    int menu_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT + MENU_PADDING * 2;
    
    // Check if click is within menu bounds
    if (mouse_x < menu_x || mouse_x > menu_x + MENU_WIDTH ||
        mouse_y < menu_y || mouse_y > menu_y + menu_height) {
        return -1;
    }
    
    // Calculate which item was clicked
    int y = menu_y + MENU_PADDING + 20;
    
    for (size_t i = 0; i < facility_types_.size(); ++i) {
        if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
            selected_facility_ = static_cast<int>(i);
            return selected_facility_;
        }
        y += ITEM_HEIGHT;
    }
    
    return -1;
}

} // namespace ui
} // namespace towerforge
