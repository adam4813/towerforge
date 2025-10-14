#include "ui/build_menu.h"
#include "ui/tooltip.h"
#include <sstream>

namespace towerforge::ui {

    BuildMenu::BuildMenu()
        : selected_facility_(-1)
          , visible_(true)
          , tutorial_mode_(false)
          , tooltip_manager_(nullptr) {

        // Initialize facility types with costs and properties
        facility_types_.emplace_back("Lobby", "L", 1000, 10, GOLD);
        facility_types_.emplace_back("Office", "O", 5000, 8, SKYBLUE);
        facility_types_.emplace_back("Restaurant", "R", 8000, 6, RED);
        facility_types_.emplace_back("Shop", "S", 6000, 4, GREEN);
        facility_types_.emplace_back("Hotel", "H", 12000, 10, PURPLE);
        facility_types_.emplace_back("Gym", "G", 10000, 7, ORANGE);
        facility_types_.emplace_back("Arcade", "A", 9000, 5, MAGENTA);
        facility_types_.emplace_back("Theater", "T", 15000, 8, VIOLET);
        facility_types_.emplace_back("Conference", "C", 13000, 9, SKYBLUE);
        facility_types_.emplace_back("Flagship", "F", 18000, 12, Color{0, 206, 209, 255});
        facility_types_.emplace_back("Elevator", "E", 15000, 2, GRAY);
    }

    BuildMenu::~BuildMenu() = default;

    void BuildMenu::Render(const bool can_undo, const bool can_redo, const bool demolish_mode) const {
        if (!visible_) {
            return;
        }

        int screen_height = GetScreenHeight();
        constexpr int menu_x = 10;
        constexpr int menu_y = 60;  // Below top bar

        const int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;
        constexpr int tools_height = 5 * ITEM_HEIGHT + 10; // 5 tool buttons (including floor expansion) + spacing
        const int total_height = facilities_height + tools_height + MENU_PADDING * 4 + 80; // +80 for headers

        // Draw menu background
        DrawRectangle(menu_x, menu_y, MENU_WIDTH, total_height, ColorAlpha(BLACK, 0.8f));
        DrawRectangle(menu_x, menu_y, MENU_WIDTH, 2, GOLD);

        // Draw title
        DrawText("FACILITIES", menu_x + MENU_PADDING, menu_y + MENU_PADDING, 14, WHITE);

        int y = menu_y + MENU_PADDING + 20;

        // Draw facility types
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            const auto& facility = facility_types_[i];

            const bool is_highlighted = tutorial_mode_ && !highlighted_facility_.empty() &&
                                  facility.name == highlighted_facility_;
            const bool is_disabled = tutorial_mode_ && !highlighted_facility_.empty() &&
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
            const Color icon_color = is_disabled ? ColorAlpha(facility.color, 0.3f) : facility.color;
            const Color text_color = is_disabled ? ColorAlpha(WHITE, 0.3f) : WHITE;

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
        const Color demolish_bg = demolish_mode ? ColorAlpha(RED, 0.3f) : ColorAlpha(DARKGRAY, 0.5f);
        DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, demolish_bg);
        if (demolish_mode) {
            DrawRectangleLines(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5, RED);
        }
        DrawText("Demolish (D)", menu_x + MENU_PADDING + 10, y + 12, 14, demolish_mode ? RED : WHITE);
        y += ITEM_HEIGHT;

        // Draw undo button
        const Color undo_color = can_undo ? WHITE : GRAY;
        DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5,
                      ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Undo (Ctrl+Z)", menu_x + MENU_PADDING + 10, y + 12, 14, undo_color);
        y += ITEM_HEIGHT;

        // Draw redo button
        const Color redo_color = can_redo ? WHITE : GRAY;
        DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5,
                      ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Redo (Ctrl+Y)", menu_x + MENU_PADDING + 10, y + 12, 14, redo_color);
        y += ITEM_HEIGHT;

        // Draw separator for expansion section
        y += 5;
        DrawRectangle(menu_x, y, MENU_WIDTH, 2, GRAY);
        y += 7;
        DrawText("EXPANSION", menu_x + MENU_PADDING, y, 14, WHITE);
        y += 20;

        // Draw Add Floor button
        DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5,
                      ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Add Floor (+)", menu_x + MENU_PADDING + 10, y + 12, 14, SKYBLUE);
        y += ITEM_HEIGHT;

        // Draw Add Basement button
        DrawRectangle(menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5,
                      ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Add Basement (-)", menu_x + MENU_PADDING + 10, y + 12, 14, ORANGE);
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

    int BuildMenu::HandleClick(const int mouse_x, const int mouse_y, const bool can_undo, const bool can_redo) {
        if (!visible_) {
            return -1;
        }

        constexpr int menu_x = 10;
        constexpr int menu_y = 60;
        const int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;

        // Check if click is within menu bounds
        const int total_height = facilities_height + 5 * ITEM_HEIGHT + MENU_PADDING * 4 + 100;
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
        y += ITEM_HEIGHT;

        // Skip expansion header
        y += 5 + 2 + 7 + 20;

        // Check Add Floor button
        if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
            return -5; // Add Floor
        }
        y += ITEM_HEIGHT;

        // Check Add Basement button
        if (mouse_y >= y && mouse_y < y + ITEM_HEIGHT) {
            return -6; // Add Basement
        }

        return -1;
    }

    void BuildMenu::UpdateTooltips(int mouse_x, int mouse_y, float current_funds) const {
        if (!visible_ || !tooltip_manager_) {
            return;
        }

        int menu_x = 10;
        int menu_y = 60;
        int y = menu_y + MENU_PADDING + 20;

        // Check facility types
        for (const auto & facility : facility_types_) {
            if (TooltipManager::IsHovering(mouse_x, mouse_y, menu_x + MENU_PADDING, y,
                                             MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5)) {
                // Create dynamic tooltip
                std::stringstream tooltip_text;
                tooltip_text << facility.name << " - $" << facility.cost << "\n";
                tooltip_text << "Width: " << facility.width << " cells\n";

                // Add state-based information
                if (current_funds < facility.cost) {
                    tooltip_text << "[INSUFFICIENT FUNDS]";
                } else if (tutorial_mode_ && !highlighted_facility_.empty() &&
                           facility.name != highlighted_facility_) {
                    tooltip_text << "[Locked in tutorial mode]";
                } else {
                    tooltip_text << "Click to select for placement";
                }

                Tooltip tooltip(tooltip_text.str());
                tooltip_manager_->ShowTooltip(tooltip, menu_x + MENU_PADDING, y,
                                              MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5);
                return;
            }
            y += ITEM_HEIGHT;
        }

        // Skip separator and tools header
        y += 10 + 2 + 12 + 20;

        // Check demolish button
        if (TooltipManager::IsHovering(mouse_x, mouse_y, menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5)) {
            Tooltip tooltip("Enter demolish mode to remove facilities.\nRefunds 50% of construction cost.\nHotkey: D");
            tooltip_manager_->ShowTooltip(tooltip, menu_x + MENU_PADDING, y,
                                          MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5);
            return;
        }
        y += ITEM_HEIGHT;

        // Check undo button
        if (TooltipManager::IsHovering(mouse_x, mouse_y, menu_x + MENU_PADDING, y,
                                         MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5)) {
            Tooltip tooltip("Undo last placement or demolition.\nHotkey: Ctrl+Z");
            tooltip_manager_->ShowTooltip(tooltip, menu_x + MENU_PADDING, y,
                                          MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5);
            return;
        }
        y += ITEM_HEIGHT;

        // Check redo button
        if (TooltipManager::IsHovering(mouse_x, mouse_y, menu_x + MENU_PADDING, y,
                                         MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5)) {
            Tooltip tooltip("Redo previously undone action.\nHotkey: Ctrl+Y");
            tooltip_manager_->ShowTooltip(tooltip, menu_x + MENU_PADDING, y,
                                          MENU_WIDTH - MENU_PADDING * 2, ITEM_HEIGHT - 5);
            return;
        }

        // No tooltip to show
        tooltip_manager_->HideTooltip();
    }

    void BuildMenu::RenderTooltips() const {
        if (tooltip_manager_) {
            tooltip_manager_->Render();
        }
    }

}
