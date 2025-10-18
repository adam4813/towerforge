#include "ui/build_menu.h"
#include "ui/tooltip.h"
#include "ui/ui_element.h"
#include "ui/help_system.h"
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


        constexpr int title_height = 20;
        const int facilities_height = static_cast<int>(facility_types_.size()) * ITEM_HEIGHT;
        constexpr int tools_height = 5 * ITEM_HEIGHT + 10; // 5 tool buttons (including floor expansion) + spacing

        panel_bounds_ = {
            static_cast<float>(10),
            static_cast<float>(60),
            static_cast<float>(MENU_WIDTH),
            static_cast<float>(facilities_height + tools_height + MENU_PADDING * 4 + 80) // +80 for headers
        };

        content_bounds_ = {
            panel_bounds_.x + MENU_PADDING,
            panel_bounds_.y + MENU_PADDING + title_height,
            panel_bounds_.width - MENU_PADDING * 2,
            panel_bounds_.height - MENU_PADDING * 2
        };
        
        // Create Panel object for unified UI system
        panel_ = std::make_unique<Panel>(
            panel_bounds_.x,
            panel_bounds_.y,
            panel_bounds_.width,
            panel_bounds_.height,
            ColorAlpha(BLACK, 0.8f),
            BLANK  // No border from Panel itself, we use custom decorative borders
        );
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
                DrawRectangle(bounds.x, bounds.y, bounds.width, 2, GOLD);
                break;
            case BOTTOM:
                DrawRectangle(bounds.x, bounds.y + bounds.height - 2, bounds.width, 2, GOLD);
                break;
            case NONE:
            default: ;
        }
    }

    /**
     * @brief Enhanced RenderPanel that works with Panel objects
     * Renders a panel with title and optional border decoration
     */
    void RenderPanel(const Panel& panel, const char *title, const float padding, const PanelBorder border) {
        const Rectangle bounds = panel.GetAbsoluteBounds();
        
        // Render the panel itself (background and border from Panel)
        panel.Render();
        
        // Draw title
        DrawText(title, bounds.x + padding, bounds.y + padding, 14, WHITE);

        // Draw decorative border based on type
        switch (border) {
            case TOP:
                DrawRectangle(bounds.x, bounds.y, bounds.width, 2, GOLD);
                break;
            case BOTTOM:
                DrawRectangle(bounds.x, bounds.y + bounds.height - 2, bounds.width, 2, GOLD);
                break;
            case NONE:
            default: ;
        }
    }

    void RenderToolButton(const Rectangle bounds, const char *label, const bool is_active, const Color active_color,
                          const bool is_disabled, const Color text_color = WHITE) {
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

        RenderPanel(panel_bounds_, "FACILITIES", MENU_PADDING, TOP);

        // Help icon in the panel header area
        const Rectangle help_icon_bounds = {
            panel_bounds_.x + panel_bounds_.width - 35.0f,
            panel_bounds_.y + MENU_PADDING - 2.0f,
            20.0f,
            20.0f
        };
        HelpSystem::RenderHelpIcon(help_icon_bounds, GetMouseX(), GetMouseY());

        int y = content_bounds_.y;

        // Draw facility types
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            const auto &facility = facility_types_[i];

            const bool is_highlighted = tutorial_mode_ && !highlighted_facility_.empty() &&
                                        facility.name == highlighted_facility_;
            const bool is_disabled = tutorial_mode_ && !highlighted_facility_.empty() &&
                                     facility.name != highlighted_facility_;
            const bool is_selected = !demolish_mode && static_cast<int>(i) == selected_facility_;

            const Rectangle item_bounds = {
                content_bounds_.x,
                static_cast<float>(y),
                content_bounds_.width,
                static_cast<float>(ITEM_HEIGHT)
            };

            RenderBuildMenuItem(facility, is_selected, is_disabled, is_highlighted, item_bounds);

            y += ITEM_HEIGHT;
        }

        // Draw separator
        y += 10;
        DrawRectangle(panel_bounds_.x, y, panel_bounds_.width, 2, GRAY);
        y += 12;

        // Draw tools header
        DrawText("TOOLS", content_bounds_.x, y, 14, WHITE);
        y += 20;

        Rectangle tool_button_bounds = {content_bounds_.x, static_cast<float>(y), content_bounds_.width, ITEM_HEIGHT};
        RenderToolButton(tool_button_bounds, "Demolish (D)", demolish_mode, RED, false);
        y += ITEM_HEIGHT;

        tool_button_bounds.y = static_cast<float>(y);
        RenderToolButton(tool_button_bounds, "Undo (Ctrl+Z)", false, WHITE, !can_undo);
        y += ITEM_HEIGHT;

        tool_button_bounds.y = static_cast<float>(y);
        RenderToolButton(tool_button_bounds, "Redo (Ctrl+Y)", false, WHITE, !can_redo);
        y += ITEM_HEIGHT;

        // Draw separator for expansion section
        y += 5;
        DrawRectangle(panel_bounds_.x, y, panel_bounds_.width, 2, GRAY);
        y += 7;
        DrawText("EXPANSION", content_bounds_.x, y, 14, WHITE);
        y += 20;

        tool_button_bounds.y = static_cast<float>(y);
        RenderToolButton(tool_button_bounds, "Add Floor (+)", false, WHITE, false, SKYBLUE);
        y += ITEM_HEIGHT;

        tool_button_bounds.y = static_cast<float>(y);
        RenderToolButton(tool_button_bounds, "Add Basement (-)", false, WHITE, false, ORANGE);
        y += ITEM_HEIGHT;

        if (demolish_mode) {
            DrawText("Click facility to demolish (50% refund)", content_bounds_.x, y, 9, LIGHTGRAY);
        } else if (selected_facility_ >= 0) {
            DrawText("Click grid to place facility", content_bounds_.x, y, 9, LIGHTGRAY);
        } else {
            DrawText("Select facility to build", content_bounds_.x, y, 9, LIGHTGRAY);
        }
    }

    int BuildMenu::HandleClick(const int mouse_x, const int mouse_y, const bool can_undo, const bool can_redo) {
        if (!visible_) {
            return -1;
        }

        if (!CheckCollisionPointRec({static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, panel_bounds_)) {
            return -1;
        }

        // Calculate which item was clicked
        int y = content_bounds_.y;

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

        int y = content_bounds_.y;

        Rectangle check_rect = {content_bounds_.x, content_bounds_.y, content_bounds_.width, ITEM_HEIGHT};

        // Check facility types
        for (const auto &facility: facility_types_) {
            check_rect.y = static_cast<float>(y);
            if (TooltipManager::IsHoveringRec(mouse_x, mouse_y, check_rect)) {
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
                tooltip_manager_->ShowTooltip(tooltip, content_bounds_.x, y, content_bounds_.width, ITEM_HEIGHT);
                return;
            }
            y += ITEM_HEIGHT;
        }

        // Skip separator and tools header
        y += 10 + 2 + 12 + 20;

        // Check demolish button
        check_rect.y = static_cast<float>(y);
        if (TooltipManager::IsHoveringRec(mouse_x, mouse_y, check_rect)) {
            Tooltip tooltip("Enter demolish mode to remove facilities.\nRefunds 50% of construction cost.\nHotkey: D");
            tooltip_manager_->ShowTooltip(tooltip, content_bounds_.x, y, content_bounds_.width, ITEM_HEIGHT);
            return;
        }
        y += ITEM_HEIGHT;

        // Check undo button
        check_rect.y = static_cast<float>(y);
        if (TooltipManager::IsHoveringRec(mouse_x, mouse_y, check_rect)) {
            Tooltip tooltip("Undo last placement or demolition.\nHotkey: Ctrl+Z");
            tooltip_manager_->ShowTooltip(tooltip, content_bounds_.x, y, content_bounds_.width, ITEM_HEIGHT);
            return;
        }
        y += ITEM_HEIGHT;

        // Check redo button
        check_rect.y = static_cast<float>(y);
        if (TooltipManager::IsHoveringRec(mouse_x, mouse_y, check_rect)) {
            Tooltip tooltip("Redo previously undone action.\nHotkey: Ctrl+Y");
            tooltip_manager_->ShowTooltip(tooltip, content_bounds_.x, y, content_bounds_.width, ITEM_HEIGHT);
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
