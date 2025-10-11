#include "ui/mods_menu.h"
#include <raylib.h>
#include <algorithm>

namespace towerforge {
namespace ui {

ModsMenu::ModsMenu()
    : visible_(false), mod_manager_(nullptr), selected_mod_index_(-1), scroll_offset_(0.0f) {
}

ModsMenu::~ModsMenu() {
}

void ModsMenu::SetModManager(TowerForge::Core::LuaModManager* mod_manager) {
    mod_manager_ = mod_manager;
}

void ModsMenu::Show() {
    visible_ = true;
    selected_mod_index_ = -1;
    scroll_offset_ = 0.0f;
}

void ModsMenu::Hide() {
    visible_ = false;
}

void ModsMenu::Render() {
    if (!visible_ || !mod_manager_) {
        return;
    }
    
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Semi-transparent overlay
    DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 180});
    
    // Menu dimensions
    int menu_width = std::min(800, screen_width - 100);
    int menu_height = std::min(600, screen_height - 100);
    int menu_x = (screen_width - menu_width) / 2;
    int menu_y = (screen_height - menu_height) / 2;
    
    // Menu background
    DrawRectangle(menu_x, menu_y, menu_width, menu_height, (Color){40, 40, 50, 255});
    DrawRectangleLines(menu_x, menu_y, menu_width, menu_height, (Color){100, 100, 120, 255});
    
    // Title
    const char* title = "Mods Manager";
    int title_size = 28;
    int title_width = MeasureText(title, title_size);
    DrawText(title, menu_x + (menu_width - title_width) / 2, menu_y + 20, title_size, GOLD);
    
    // Instructions
    const char* instructions = "Press ESC to close";
    int inst_size = 14;
    DrawText(instructions, menu_x + 20, menu_y + menu_height - 30, inst_size, LIGHTGRAY);
    
    // Get mods list
    const auto& mods = mod_manager_->GetLoadedMods();
    
    if (mods.empty()) {
        // No mods loaded
        const char* no_mods_msg = "No mods found in the mods/ directory";
        int msg_width = MeasureText(no_mods_msg, 18);
        DrawText(no_mods_msg, menu_x + (menu_width - msg_width) / 2, 
                 menu_y + menu_height / 2, 18, GRAY);
        
        const char* help_msg = "Place .lua mod files in the mods/ folder";
        int help_width = MeasureText(help_msg, 14);
        DrawText(help_msg, menu_x + (menu_width - help_width) / 2, 
                 menu_y + menu_height / 2 + 30, 14, DARKGRAY);
    } else {
        // Display mods list
        int list_y = menu_y + 70;
        int list_height = menu_height - 120;
        int item_height = 120;
        int visible_items = list_height / item_height;
        
        // Header
        DrawText("Loaded Mods:", menu_x + 20, menu_y + 60, 18, WHITE);
        
        char count_str[64];
        snprintf(count_str, sizeof(count_str), "Total: %d", static_cast<int>(mods.size()));
        DrawText(count_str, menu_x + menu_width - 120, menu_y + 60, 16, LIGHTGRAY);
        
        // Scroll handling
        int max_scroll = std::max(0, static_cast<int>(mods.size()) - visible_items);
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scroll_offset_ -= wheel;
            scroll_offset_ = std::max(0.0f, std::min(static_cast<float>(max_scroll), scroll_offset_));
        }
        
        int start_index = static_cast<int>(scroll_offset_);
        int end_index = std::min(start_index + visible_items + 1, static_cast<int>(mods.size()));
        
        // Render mods
        for (int i = start_index; i < end_index; ++i) {
            const auto& mod = mods[i];
            int y = list_y + (i - start_index) * item_height;
            
            // Mod item background
            Color bg_color = (i % 2 == 0) ? (Color){50, 50, 60, 255} : (Color){45, 45, 55, 255};
            if (i == selected_mod_index_) {
                bg_color = (Color){60, 60, 80, 255};
            }
            
            DrawRectangle(menu_x + 20, y, menu_width - 40, item_height - 10, bg_color);
            DrawRectangleLines(menu_x + 20, y, menu_width - 40, item_height - 10, 
                             (Color){80, 80, 100, 255});
            
            int text_x = menu_x + 35;
            int text_y = y + 10;
            
            // Mod name
            DrawText(mod.name.c_str(), text_x, text_y, 20, 
                    mod.loaded_successfully ? WHITE : RED);
            
            text_y += 25;
            
            // Version and author
            if (!mod.version.empty() || !mod.author.empty()) {
                char info[256];
                if (!mod.version.empty() && !mod.author.empty()) {
                    snprintf(info, sizeof(info), "v%s by %s", 
                            mod.version.c_str(), mod.author.c_str());
                } else if (!mod.version.empty()) {
                    snprintf(info, sizeof(info), "v%s", mod.version.c_str());
                } else {
                    snprintf(info, sizeof(info), "by %s", mod.author.c_str());
                }
                DrawText(info, text_x, text_y, 14, GRAY);
                text_y += 18;
            }
            
            // Description
            if (!mod.description.empty()) {
                // Truncate if too long
                std::string desc = mod.description;
                if (desc.length() > 80) {
                    desc = desc.substr(0, 77) + "...";
                }
                DrawText(desc.c_str(), text_x, text_y, 12, LIGHTGRAY);
                text_y += 16;
            }
            
            // Status
            if (mod.loaded_successfully) {
                const char* status = mod.enabled ? "Enabled" : "Disabled";
                Color status_color = mod.enabled ? GREEN : ORANGE;
                DrawText(status, text_x, text_y, 14, status_color);
                
                // Click to toggle
                const char* hint = "(Click to toggle)";
                DrawText(hint, text_x + 100, text_y, 12, DARKGRAY);
                
                // Handle click
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse_pos = GetMousePosition();
                    if (mouse_pos.x >= menu_x + 20 && mouse_pos.x <= menu_x + menu_width - 20 &&
                        mouse_pos.y >= y && mouse_pos.y <= y + item_height - 10) {
                        // Toggle mod
                        if (mod.enabled) {
                            mod_manager_->DisableMod(mod.id);
                        } else {
                            mod_manager_->EnableMod(mod.id);
                        }
                    }
                }
            } else {
                DrawText("Failed to load", text_x, text_y, 14, RED);
                
                // Show error message if available
                if (!mod.error_message.empty()) {
                    std::string error = "Error: " + mod.error_message;
                    if (error.length() > 60) {
                        error = error.substr(0, 57) + "...";
                    }
                    DrawText(error.c_str(), text_x + 120, text_y, 11, MAROON);
                }
            }
        }
        
        // Scroll indicator
        if (max_scroll > 0) {
            int scrollbar_x = menu_x + menu_width - 25;
            int scrollbar_y = list_y;
            int scrollbar_height = list_height;
            
            DrawRectangle(scrollbar_x, scrollbar_y, 10, scrollbar_height, (Color){30, 30, 40, 255});
            
            float thumb_height = (static_cast<float>(visible_items) / mods.size()) * scrollbar_height;
            float thumb_y = scrollbar_y + (scroll_offset_ / max_scroll) * (scrollbar_height - thumb_height);
            
            DrawRectangle(scrollbar_x, static_cast<int>(thumb_y), 10, 
                         static_cast<int>(thumb_height), (Color){100, 100, 120, 255});
        }
    }
    
    // Handle ESC to close
    if (IsKeyPressed(KEY_ESCAPE)) {
        Hide();
    }
}

} // namespace ui
} // namespace towerforge
