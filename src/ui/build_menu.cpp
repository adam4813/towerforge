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

    void RenderBuildMenuItem(const FacilityType &facility, const bool is_selected, const bool is_disabled,
                             const bool is_highlighted,
                             const Rectangle bounds) {
        if (is_selected) {
            DrawRectangleRec(bounds, ColorAlpha(WHITE, 0.2f));
        }

        if (is_highlighted) {
            const Rectangle highlight_bounds = {bounds.x - 1, bounds.y - 1, bounds.width + 2, bounds.height + 2};
            DrawRectangleLinesEx(highlight_bounds, 2, ColorAlpha(GOLD, 0.5f));
        }

        const Rectangle icon_bounds = {bounds.x + 5, bounds.y + 5, 25, 25};
        const Color icon_color = is_disabled ? ColorAlpha(facility.color, 0.3f) : facility.color;
        DrawRectangleRec(icon_bounds, icon_color);

        const Color text_color = is_disabled ? ColorAlpha(WHITE, 0.3f) : WHITE;
        DrawText(facility.icon.c_str(), bounds.x + 12, bounds.y + 10, 16, text_color);
        DrawText(facility.name.c_str(), bounds.x + 35, bounds.y + 5, 14, text_color);

        // Draw cost
        std::stringstream cost_ss;
        cost_ss << "$" << facility.cost;
        const Color cost_color = is_disabled ? ColorAlpha(GREEN, 0.3f) : GREEN;
        DrawText(cost_ss.str().c_str(), bounds.x + 35, bounds.y + 20, 12, cost_color);
    }

    enum PanelBorder {
        TOP, BOTTOM, NONE
    };

    void RenderPanel(const Rectangle bounds, const char *title, const float padding, const PanelBorder border) {
        DrawRectangleRec(bounds, ColorAlpha(BLACK, 0.8f));
        DrawText(title, bounds.x + padding, bounds.y + padding, 14, WHITE);

        switch (border) {
            case TOP:
                DrawRectangle(bounds.x, bounds.y , bounds.width, 2, GOLD);
                break;
            case BOTTOM:
                DrawRectangle(bounds.x, bounds.y + bounds.height - 2, bounds.width, 2, GOLD);
                break;
            case NONE:
            default: ;
        }
    }

    void RenderToolButton(const Rectangle bounds, const char *label, const bool is_active, const Color active_color, const bool is_disabled, const Color text_color = WHITE) {
        const Color bg_color = is_active ? ColorAlpha(active_color, 0.3f) : ColorAlpha(DARKGRAY, 0.5f);
        DrawRectangleRec(bounds, bg_color);
        if (is_active) {
            DrawRectangleLinesEx(bounds, 2, active_color);
        }
        DrawText(label, bounds.x + 10, bounds.y + 12, 14, is_disabled ? ColorAlpha(text_color, 0.3f) : text_color);
    }

    void BuildMenu::Render(const bool can_undo, const bool can_redo, const bool demolish_mode) const {
        if (!visible_) {
            return;
        }

        const int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;
        constexpr int tools_height = 5 * ITEM_HEIGHT + 10; // 5 tool buttons (including floor expansion) + spacing

        const Rectangle panel_bounds = {
            static_cast<float>(10),
            static_cast<float>(60),
            static_cast<float>(MENU_WIDTH),
            static_cast<float>(facilities_height + tools_height + MENU_PADDING * 4 + 80) // +80 for headers
        };

        const Rectangle content_bounds = {
            panel_bounds.x + MENU_PADDING,
            panel_bounds.y + MENU_PADDING,
            panel_bounds.width - MENU_PADDING * 2,
            panel_bounds.height - MENU_PADDING * 2
        };

        RenderPanel(panel_bounds, "FACILITIES", MENU_PADDING, TOP);

        int y = content_bounds.y + 20;

        // Draw facility types
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            const auto &facility = facility_types_[i];

            const bool is_highlighted = tutorial_mode_ && !highlighted_facility_.empty() &&
                                        facility.name == highlighted_facility_;
            const bool is_disabled = tutorial_mode_ && !highlighted_facility_.empty() &&
                                     facility.name != highlighted_facility_;
            const bool is_selected = !demolish_mode && static_cast<int>(i) == selected_facility_;

            const Rectangle item_bounds = {
                content_bounds.x,
                static_cast<float>(y),
                content_bounds.width,
                static_cast<float>(ITEM_HEIGHT)
            };

            RenderBuildMenuItem(facility, is_selected, is_disabled, is_highlighted, item_bounds);

            y += ITEM_HEIGHT;
        }

        // Draw separator
        y += 10;
        DrawRectangle(panel_bounds.x, y, panel_bounds.width, 2, GRAY);
        y += 12;

        // Draw tools header
        DrawText("TOOLS", content_bounds.x, y, 14, WHITE);
        y += 20;

        // Draw demolish button
        const Color demolish_bg = demolish_mode ? ColorAlpha(RED, 0.3f) : ColorAlpha(DARKGRAY, 0.5f);
        DrawRectangle(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, demolish_bg);
        if (demolish_mode) {
            DrawRectangleLines(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, RED);
        }
        DrawText("Demolish (D)", content_bounds.x + 10, y + 12, 14, demolish_mode ? RED : WHITE);
        y += ITEM_HEIGHT;

        // Draw undo button
        const Color undo_color = can_undo ? WHITE : GRAY;
        DrawRectangle(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Undo (Ctrl+Z)", content_bounds.x + 10, y + 12, 14, undo_color);
        y += ITEM_HEIGHT;

        // Draw redo button
        const Color redo_color = can_redo ? WHITE : GRAY;
        DrawRectangle(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Redo (Ctrl+Y)", content_bounds.x + 10, y + 12, 14, redo_color);
        y += ITEM_HEIGHT;

        // Draw separator for expansion section
        y += 5;
        DrawRectangle(panel_bounds.x, y, panel_bounds.width, 2, GRAY);
        y += 7;
        DrawText("EXPANSION", content_bounds.x, y, 14, WHITE);
        y += 20;

        // Draw Add Floor button
        DrawRectangle(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Add Floor (+)", content_bounds.x + 10, y + 12, 14, SKYBLUE);
        y += ITEM_HEIGHT;

        // Draw Add Basement button
        DrawRectangle(content_bounds.x, y, content_bounds.width, ITEM_HEIGHT - 5, ColorAlpha(DARKGRAY, 0.5f));
        DrawText("Add Basement (-)", content_bounds.x + 10, y + 12, 14, ORANGE);
        y += ITEM_HEIGHT;

        // Draw hint at bottom
        y += 5;
        if (demolish_mode) {
            DrawText("Click facility to demolish (50% refund)", content_bounds.x, y, 9, LIGHTGRAY);
        } else if (selected_facility_ >= 0) {
            DrawText("Click grid to place facility", content_bounds.x, y, 9, LIGHTGRAY);
        } else {
            DrawText("Select facility to build", content_bounds.x, y, 9, LIGHTGRAY);
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
        for (const auto &facility: facility_types_) {
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
        if (TooltipManager::IsHovering(mouse_x, mouse_y, menu_x + MENU_PADDING, y, MENU_WIDTH - MENU_PADDING * 2,
                                       ITEM_HEIGHT - 5)) {
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
