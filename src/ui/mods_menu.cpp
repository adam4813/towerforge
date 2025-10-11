#include "ui/mods_menu.h"
#include <cmath>

namespace towerforge {
namespace ui {

// Menu layout constants
static const int MENU_WIDTH = 800;
static const int MENU_HEIGHT = 600;
static const int MOD_ITEM_HEIGHT = 60;
static const int PADDING = 20;

ModsMenu::ModsMenu()
    : visible_(false)
    , animation_time_(0.0f)
    , selected_mod_index_(0) {
    
    // Initialize some sample mods
    mods_.push_back({"Enhanced Graphics", "GraphicsMod Team", "1.0.0", true});
    mods_.push_back({"Extra Facilities", "Builder123", "2.1.0", true});
    mods_.push_back({"Custom AI", "AIModder", "1.5.2", false});
}

ModsMenu::~ModsMenu() {
}

void ModsMenu::Update(float delta_time) {
    if (!visible_) return;
    animation_time_ += delta_time;
}

void ModsMenu::Render() {
    if (!visible_) return;
    
    RenderBackground();
    RenderModList();
    RenderModDetails();
}

void ModsMenu::RenderBackground() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Semi-transparent background overlay (GNU-style compound literal)
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 180});
    
    // Main menu panel (GNU-style compound literal)
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, (Color){40, 40, 50, 255});
    DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, GOLD);
    
    // Draw title background (GNU-style compound literal)
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, 60, (Color){30, 30, 40, 255});
    DrawText("MODS MENU", menu_x + PADDING, menu_y + 20, 24, GOLD);
}

void ModsMenu::RenderModList() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    int list_y = menu_y + 80;
    
    for (size_t i = 0; i < mods_.size(); ++i) {
        int item_y = list_y + i * (MOD_ITEM_HEIGHT + 10);
        bool is_selected = (static_cast<int>(i) == selected_mod_index_);
        
        // Draw mod item background (GNU-style compound literal)
        if (is_selected) {
            DrawRectangle(menu_x + PADDING, item_y, MENU_WIDTH / 2 - PADDING * 2, 
                         MOD_ITEM_HEIGHT, (Color){60, 60, 80, 255});
            DrawRectangleLines(menu_x + PADDING, item_y, MENU_WIDTH / 2 - PADDING * 2, 
                              MOD_ITEM_HEIGHT, GOLD);
        } else {
            DrawRectangle(menu_x + PADDING, item_y, MENU_WIDTH / 2 - PADDING * 2, 
                         MOD_ITEM_HEIGHT, (Color){50, 50, 60, 255});
            DrawRectangleLines(menu_x + PADDING, item_y, MENU_WIDTH / 2 - PADDING * 2, 
                              MOD_ITEM_HEIGHT, (Color){80, 80, 90, 255});
        }
        
        // Draw mod name
        DrawText(mods_[i].name.c_str(), menu_x + PADDING + 10, item_y + 10, 18, WHITE);
        
        // Draw version
        DrawText(mods_[i].version.c_str(), menu_x + PADDING + 10, item_y + 35, 14, LIGHTGRAY);
        
        // Draw enabled status (GNU-style compound literal)
        Color status_color = mods_[i].enabled ? (Color){0, 255, 0, 255} : (Color){255, 0, 0, 255};
        const char* status_text = mods_[i].enabled ? "ENABLED" : "DISABLED";
        DrawText(status_text, menu_x + MENU_WIDTH / 2 - 120, item_y + 20, 16, status_color);
    }
}

void ModsMenu::RenderModDetails() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    int details_x = menu_x + MENU_WIDTH / 2 + PADDING;
    int details_y = menu_y + 80;
    int details_width = MENU_WIDTH / 2 - PADDING * 2;
    int details_height = MENU_HEIGHT - 100;
    
    // Draw details panel background (GNU-style compound literal)
    DrawRectangle(details_x, details_y, details_width, details_height, (Color){35, 35, 45, 255});
    DrawRectangleLines(details_x, details_y, details_width, details_height, (Color){70, 70, 80, 255});
    
    if (selected_mod_index_ >= 0 && selected_mod_index_ < static_cast<int>(mods_.size())) {
        const auto& mod = mods_[selected_mod_index_];
        
        // Draw details header (GNU-style compound literal)
        DrawRectangle(details_x, details_y, details_width, 40, (Color){45, 45, 55, 255});
        DrawText("MOD DETAILS", details_x + 10, details_y + 10, 16, GOLD);
        
        int text_y = details_y + 60;
        DrawText("Name:", details_x + 10, text_y, 14, LIGHTGRAY);
        DrawText(mod.name.c_str(), details_x + 80, text_y, 14, WHITE);
        
        text_y += 30;
        DrawText("Author:", details_x + 10, text_y, 14, LIGHTGRAY);
        DrawText(mod.author.c_str(), details_x + 80, text_y, 14, WHITE);
        
        text_y += 30;
        DrawText("Version:", details_x + 10, text_y, 14, LIGHTGRAY);
        DrawText(mod.version.c_str(), details_x + 80, text_y, 14, WHITE);
        
        text_y += 30;
        DrawText("Status:", details_x + 10, text_y, 14, LIGHTGRAY);
        
        // Status with color (GNU-style compound literal)
        Color status_color = mod.enabled ? (Color){0, 255, 0, 255} : (Color){255, 0, 0, 255};
        const char* status_text = mod.enabled ? "Enabled" : "Disabled";
        DrawText(status_text, details_x + 80, text_y, 14, status_color);
    }
    
    // Draw instructions at bottom (GNU-style compound literal)
    DrawRectangle(menu_x, menu_y + MENU_HEIGHT - 40, MENU_WIDTH, 40, (Color){30, 30, 40, 255});
    DrawText("ESC: Close | UP/DOWN: Navigate | SPACE: Toggle", 
             menu_x + PADDING, menu_y + MENU_HEIGHT - 28, 14, LIGHTGRAY);
}

} // namespace ui
} // namespace towerforge
