#include "ui/build_menu.h"
#include <sstream>

namespace towerforge {
namespace ui {

BuildMenu::BuildMenu() 
    : selected_facility_(-1)
    , visible_(true)
    , tutorial_mode_(false)
    , highlighted_facility_("") {
    
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

void BuildMenu::Render(bool can_undo, bool can_redo, bool demolish_mode) {
    if (!visible_) {
        return;
    }
    
    int screen_height = GetScreenHeight();
    int menu_x = 10;
    int menu_y = 60;  // Below top bar
    
    int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;
    int tools_height = 3 * ITEM_HEIGHT + 10; // 3 tool buttons + spacing
    int total_height = facilities_height + tools_height + MENU_PADDING * 4 + 40; // +40 for headers
    
    // Draw menu background
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, total_height, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, 2, GOLD);
    
    // Draw title
    DrawText("FACILITIES", menu_x + MENU_PADDING, menu_y + MENU_PADDING, 14, WHITE);
    
    int y = menu_y + MENU_PADDING + 20;
    
    // Draw facility types
    for (size_t i = 0; i < facility_types_.size(); ++i) {
        const auto& facility = facility_types_[i];
        
        bool is_highlighted = tutorial_mode_ && !highlighted_facility_.empty() && 
                             facility.name == highlighted_facility_;
        bool is_disabled = tutorial_mode_ && !highlighted_facility_.empty() && 
                          facility.name != highlighted_facility_;
        
        // Highlight selected item (only if not in demolish mode)
        if (!demolish_mode && static_cast<int>(i) == selected_facility_) {
            DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, 
                         ColorAlpha(WHITE, 0.2f));
        }
        
        // Highlight tutorial-required facility with gold border
        if (is_highlighted) {
            DrawRectangleLines(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, 
                             GOLD);
            DrawRectangleLines(menu_x + MENU_PADDING + 1, y + 1, MENU_WIDTH - MENU_PADDING * 2 - 2, ITEM_HEIGHT - 7, 
                             ColorAlpha(GOLD, 0.5f));
        }
        
        // Draw icon with facility color (dimmed if disabled)
        Color icon_color = is_disabled ? ColorAlpha(facility.color, 0.3f) : facility.color;
        Color text_color = is_disabled ? ColorAlpha(WHITE, 0.3f) : WHITE;
        
        DrawRectangle(menu_x + MENU_PADDING + 5, y + 5, 25, 25, icon_color);
        DrawText(facility.icon.c_str(), menu_x + MENU_PADDING + 12, y + 10, 16, text_color);
        
        // Draw name
        DrawText(facility.name.c_str(), menu_x + MENU_PADDING + 35, y + 5, 14, text_color);
        
        // Draw cost
        std::stringstream cost_ss;
        cost_ss << "$" << facility.cost;
        DrawText(cost_ss.str().c_str(), menu_x + MENU_PADDING + 35, y + 20, 12, 
                is_disabled ? ColorAlpha(GREEN, 0.3f) : GREEN);
        
        y += ITEM_HEIGHT;
    }
    
    // Draw separator
    y += 10;
    DrawRectangle(menu_x, y, MENU_WIDTH, 2, GRAY);
    y += 12;
    
    // Draw tools header
    DrawText("TOOLS", menu_x + MENU_PADDING, y, 14, WHITE);
    y += 20;
    
    // Draw demolish button
    Color demolish_bg = demolish_mode ? ColorAlpha(RED, 0.3f) : ColorAlpha(DARKGRAY, 0.5f);
    DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, demolish_bg);
    if (demolish_mode) {
        DrawRectangleLines(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, RED);
    }
    DrawText("Demolish (D)", menu_x + MENU_PADDING + 10, y + 12, 14, demolish_mode ? RED : WHITE);
    y += ITEM_HEIGHT;
    
    // Draw undo button
    Color undo_color = can_undo ? WHITE : GRAY;
    DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, 
                 ColorAlpha(DARKGRAY, 0.5f));
    DrawText("Undo (Ctrl+Z)", menu_x + MENU_PADDING + 10, y + 12, 14, undo_color);
    y += ITEM_HEIGHT;
    
    // Draw redo button
    Color redo_color = can_redo ? WHITE : GRAY;
    DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, 
                 ColorAlpha(DARKGRAY, 0.5f));
    DrawText("Redo (Ctrl+Y)", menu_x + MENU_PADDING + 10, y + 12, 14, redo_color);
    y += ITEM_HEIGHT;
    
    // Draw hint at bottom
    y += 5;
    if (demolish_mode) {
        DrawText("Click facility to demolish (50% refund)", 
                 menu_x + MENU_PADDING, y, 9, LIGHTGRAY);
    } else if (selected_facility_ >= 0) {
        DrawText("Click grid to place facility", 
                 menu_x + MENU_PADDING, y, 9, LIGHTGRAY);
    } else {
        DrawText("Select facility to build", 
                 menu_x + MENU_PADDING, y, 9, LIGHTGRAY);
    }
}

int BuildMenu::HandleClick(int mouse_x, int mouse_y, bool can_undo, bool can_redo) {
    if (!visible_) {
        return -1;
    }
    
    int menu_x = 10;
    int menu_y = 60;
    int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;
    
    // Check if click is within menu bounds
    int total_height = facilities_height + 3 * ITEM_HEIGHT + MENU_PADDING * 4 + 60;
    if (mouse_x < menu_x || mouse_x > menu_x + MENU_WIDTH ||
        mouse_y < menu_y || mouse_y > menu_y + total_height) {
        return -1;
    }
    
    // Calculate which item was clicked
    int y = menu_y + MENU_PADDING + 20;
    
    // Check facility types
    for (size_t i = 0; i < facility_types_.size(); ++i) {
        if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
            // In tutorial mode, only allow clicking highlighted facility
            if (tutorial_mode_ && !highlighted_facility_.empty()) {
                if (facility_types_[i].name != highlighted_facility_) {
                    return -1; // Disabled in tutorial
                }
            }
            selected_facility_ = static_cast<int>(i);
            return selected_facility_;
        }
        y += ITEM_HEIGHT;
    }
    
    // Skip separator and tools header
    y += 10 + 2 + 12 + 20;
    
    // Check demolish button
    if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
        selected_facility_ = -1; // Clear facility selection
        return -2; // Demolish
    }
    y += ITEM_HEIGHT;
    
    // Check undo button
    if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
        if (can_undo) {
            return -3; // Undo
        }
    }
    y += ITEM_HEIGHT;
    
    // Check redo button
    if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
        if (can_redo) {
            return -4; // Redo
        }
    }
    
    return -1;
}

} // namespace ui
} // namespace towerforge
