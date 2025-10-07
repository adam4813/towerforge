#include "ui/research_tree_menu.h"
#include <raylib.h>
#include <string>
#include <cmath>

namespace towerforge {
namespace ui {

ResearchTreeMenu::ResearchTreeMenu()
    : visible_(false)
    , animation_time_(0.0f)
    , hovered_node_index_(-1) {
}

ResearchTreeMenu::~ResearchTreeMenu() {
}

void ResearchTreeMenu::Update(float delta_time) {
    if (!visible_) return;
    
    animation_time_ += delta_time;
}

void ResearchTreeMenu::Render(TowerForge::Core::ResearchTree& research_tree) {
    if (!visible_) return;
    
    // Semi-transparent overlay
    RenderOverlay();
    
    // Main menu background
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    // Dark background with border
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, Color{20, 20, 30, 250});
    DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, GOLD);
    
    // Render header
    RenderHeader(research_tree);
    
    // Render tree grid
    RenderTreeGrid(research_tree);
    
    // Render details panel if node is hovered
    if (hovered_node_index_ >= 0 && 
        hovered_node_index_ < static_cast<int>(research_tree.nodes.size())) {
        RenderNodeDetails(research_tree.nodes[hovered_node_index_]);
    }
}

void ResearchTreeMenu::RenderOverlay() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    DrawRectangle(0, 0, screen_width, screen_height, Color{0, 0, 0, 180});
}

void ResearchTreeMenu::RenderHeader(const TowerForge::Core::ResearchTree& research_tree) {
    int screen_width = GetScreenWidth();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (GetScreenHeight() - MENU_HEIGHT) / 2;
    
    // Title
    const char* title = "RESEARCH/UPGRADE TREE";
    int title_width = MeasureText(title, 24);
    DrawText(title, menu_x + (MENU_WIDTH - title_width) / 2, menu_y + 20, 24, GOLD);
    
    // Research points display
    std::string points_text = "Research Points: " + std::to_string(research_tree.research_points);
    DrawText(points_text.c_str(), menu_x + 20, menu_y + 50, 16, WHITE);
    
    // Total earned
    std::string total_text = "Total Earned: " + std::to_string(research_tree.total_points_earned);
    DrawText(total_text.c_str(), menu_x + 250, menu_y + 50, 16, LIGHTGRAY);
    
    // Close hint
    DrawText("Press ESC to close", menu_x + MENU_WIDTH - 200, menu_y + 50, 14, LIGHTGRAY);
    
    // Separator line
    DrawLine(menu_x + 10, menu_y + HEADER_HEIGHT, 
             menu_x + MENU_WIDTH - 10, menu_y + HEADER_HEIGHT, GRAY);
}

void ResearchTreeMenu::RenderTreeGrid(const TowerForge::Core::ResearchTree& research_tree) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    // Get mouse position for hover detection
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    hovered_node_index_ = -1;
    
    // Render nodes in grid
    for (size_t i = 0; i < research_tree.nodes.size(); ++i) {
        const auto& node = research_tree.nodes[i];
        
        int node_x = menu_x + GRID_START_X + node.grid_column * (NODE_SIZE + NODE_SPACING);
        int node_y = menu_y + GRID_START_Y + node.grid_row * (NODE_SIZE + NODE_SPACING);
        
        // Check if mouse is over this node
        bool hovered = (mouse_x >= node_x && mouse_x <= node_x + NODE_SIZE &&
                       mouse_y >= node_y && mouse_y <= node_y + NODE_SIZE);
        
        if (hovered) {
            hovered_node_index_ = static_cast<int>(i);
        }
        
        RenderNode(node, node_x, node_y, hovered);
    }
    
    // Draw connections between nodes (prerequisites)
    for (const auto& node : research_tree.nodes) {
        if (node.prerequisites.empty()) continue;
        
        int node_x = menu_x + GRID_START_X + node.grid_column * (NODE_SIZE + NODE_SPACING) + NODE_SIZE / 2;
        int node_y = menu_y + GRID_START_Y + node.grid_row * (NODE_SIZE + NODE_SPACING) + NODE_SIZE / 2;
        
        for (const auto& prereq_id : node.prerequisites) {
            const auto* prereq = const_cast<TowerForge::Core::ResearchTree&>(research_tree).FindNode(prereq_id);
            if (prereq) {
                int prereq_x = menu_x + GRID_START_X + prereq->grid_column * (NODE_SIZE + NODE_SPACING) + NODE_SIZE / 2;
                int prereq_y = menu_y + GRID_START_Y + prereq->grid_row * (NODE_SIZE + NODE_SPACING) + NODE_SIZE / 2;
                
                Color line_color = (prereq->state == TowerForge::Core::ResearchNodeState::Unlocked) ? DARKGRAY : Color{60, 60, 60, 255};
                DrawLine(prereq_x, prereq_y, node_x, node_y, line_color);
            }
        }
    }
}

void ResearchTreeMenu::RenderNode(const TowerForge::Core::ResearchNode& node, int x, int y, bool hovered) {
    // Background color based on state
    Color bg_color;
    Color border_color;
    
    switch (node.state) {
        case TowerForge::Core::ResearchNodeState::Locked:
            bg_color = Color{40, 40, 50, 255};
            border_color = DARKGRAY;
            break;
        case TowerForge::Core::ResearchNodeState::Upgradable:
            bg_color = Color{80, 60, 20, 255};
            border_color = GOLD;
            break;
        case TowerForge::Core::ResearchNodeState::Unlocked:
            bg_color = Color{20, 60, 40, 255};
            border_color = LIME;
            break;
        default:
            bg_color = DARKGRAY;
            border_color = GRAY;
            break;
    }
    
    // Highlight if hovered
    if (hovered) {
        DrawRectangle(x - 2, y - 2, NODE_SIZE + 4, NODE_SIZE + 4, YELLOW);
    }
    
    // Draw node background
    DrawRectangle(x, y, NODE_SIZE, NODE_SIZE, bg_color);
    DrawRectangleLines(x, y, NODE_SIZE, NODE_SIZE, border_color);
    
    // Draw icon (larger emoji/symbol)
    std::string display_icon = node.GetDisplayIcon();
    int icon_size = 32;
    DrawText(display_icon.c_str(), x + (NODE_SIZE - icon_size) / 2, y + 15, icon_size, WHITE);
    
    // Draw name (truncated if needed)
    const char* name = node.name.c_str();
    int name_width = MeasureText(name, 12);
    if (name_width > NODE_SIZE - 10) {
        // Truncate name
        std::string short_name = node.name.substr(0, 8) + "...";
        DrawText(short_name.c_str(), x + 5, y + NODE_SIZE - 30, 12, WHITE);
    } else {
        DrawText(name, x + (NODE_SIZE - name_width) / 2, y + NODE_SIZE - 30, 12, WHITE);
    }
    
    // Draw cost
    std::string cost_text = std::to_string(node.cost) + " pts";
    int cost_width = MeasureText(cost_text.c_str(), 10);
    DrawText(cost_text.c_str(), x + (NODE_SIZE - cost_width) / 2, y + NODE_SIZE - 15, 10, LIGHTGRAY);
}

void ResearchTreeMenu::RenderNodeDetails(const TowerForge::Core::ResearchNode& node) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    // Details panel on the right side
    int panel_x = menu_x + MENU_WIDTH - DETAILS_PANEL_WIDTH - 10;
    int panel_y = menu_y + HEADER_HEIGHT + 10;
    int panel_height = MENU_HEIGHT - HEADER_HEIGHT - 20;
    
    // Background
    DrawRectangle(panel_x, panel_y, DETAILS_PANEL_WIDTH, panel_height, Color{30, 30, 40, 250});
    DrawRectangleLines(panel_x, panel_y, DETAILS_PANEL_WIDTH, panel_height, GOLD);
    
    int text_x = panel_x + 15;
    int text_y = panel_y + 15;
    
    // Name
    DrawText(node.name.c_str(), text_x, text_y, 18, GOLD);
    text_y += 30;
    
    // State
    std::string state_text = "Status: ";
    state_text += node.GetStateString();
    DrawText(state_text.c_str(), text_x, text_y, 14, WHITE);
    text_y += 25;
    
    // Cost
    std::string cost_text = "Cost: " + std::to_string(node.cost) + " points";
    DrawText(cost_text.c_str(), text_x, text_y, 14, WHITE);
    text_y += 25;
    
    // Description (word-wrapped)
    if (!node.description.empty()) {
        DrawText("Description:", text_x, text_y, 12, LIGHTGRAY);
        text_y += 20;
        
        // Simple word wrapping
        std::string desc = node.description;
        int max_width = DETAILS_PANEL_WIDTH - 30;
        int line_height = 14;
        
        size_t pos = 0;
        while (pos < desc.length()) {
            size_t end = desc.find(' ', pos + 1);
            if (end == std::string::npos) end = desc.length();
            
            std::string line = desc.substr(pos, end - pos);
            int line_width = MeasureText(line.c_str(), 11);
            
            if (line_width > max_width) {
                // Find last space that fits
                size_t last_space = pos;
                for (size_t i = pos; i < end; ++i) {
                    std::string test = desc.substr(pos, i - pos);
                    if (MeasureText(test.c_str(), 11) > max_width) {
                        break;
                    }
                    if (desc[i] == ' ') last_space = i;
                }
                
                if (last_space > pos) {
                    line = desc.substr(pos, last_space - pos);
                    pos = last_space + 1;
                } else {
                    pos = end;
                }
            } else {
                pos = end + 1;
            }
            
            DrawText(line.c_str(), text_x, text_y, 11, WHITE);
            text_y += line_height;
        }
        
        text_y += 10;
    }
    
    // Prerequisites
    if (!node.prerequisites.empty()) {
        DrawText("Requires:", text_x, text_y, 12, LIGHTGRAY);
        text_y += 20;
        
        for (const auto& prereq_id : node.prerequisites) {
            std::string prereq_text = "- " + prereq_id;
            DrawText(prereq_text.c_str(), text_x + 10, text_y, 11, GRAY);
            text_y += 16;
        }
        
        text_y += 10;
    }
    
    // Effect description
    if (node.effect_value != 0.0f || !node.effect_target.empty()) {
        DrawText("Effect:", text_x, text_y, 12, LIGHTGRAY);
        text_y += 20;
        
        std::string effect_text;
        switch (node.type) {
            case TowerForge::Core::ResearchNodeType::ElevatorSpeed:
                effect_text = "+" + std::to_string(static_cast<int>(node.effect_value * 100)) + "% elevator speed";
                break;
            case TowerForge::Core::ResearchNodeType::ElevatorCapacity:
                effect_text = "+" + std::to_string(static_cast<int>(node.effect_value)) + " capacity";
                break;
            case TowerForge::Core::ResearchNodeType::IncomeBonus:
                effect_text = "+" + std::to_string(static_cast<int>(node.effect_value * 100)) + "% income";
                break;
            case TowerForge::Core::ResearchNodeType::SatisfactionBonus:
                effect_text = "+" + std::to_string(static_cast<int>(node.effect_value)) + " satisfaction";
                break;
            case TowerForge::Core::ResearchNodeType::ConstructionSpeed:
                effect_text = "+" + std::to_string(static_cast<int>(node.effect_value * 100)) + "% build speed";
                break;
            case TowerForge::Core::ResearchNodeType::CostReduction:
                effect_text = "-" + std::to_string(static_cast<int>(node.effect_value * 100)) + "% costs";
                break;
            case TowerForge::Core::ResearchNodeType::FacilityUnlock:
                effect_text = "Unlocks " + node.effect_target;
                break;
        }
        
        DrawText(effect_text.c_str(), text_x + 10, text_y, 11, LIME);
        text_y += 16;
    }
    
    // Click hint
    if (node.state == TowerForge::Core::ResearchNodeState::Upgradable) {
        text_y = panel_y + panel_height - 40;
        DrawText("Click to unlock!", text_x, text_y, 14, YELLOW);
    }
}

bool ResearchTreeMenu::HandleMouse(int mouse_x, int mouse_y, bool clicked, 
                                  TowerForge::Core::ResearchTree& research_tree) {
    if (!visible_ || !clicked) return false;
    
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    // Check if clicked on a node
    for (size_t i = 0; i < research_tree.nodes.size(); ++i) {
        auto& node = research_tree.nodes[i];
        
        int node_x = menu_x + GRID_START_X + node.grid_column * (NODE_SIZE + NODE_SPACING);
        int node_y = menu_y + GRID_START_Y + node.grid_row * (NODE_SIZE + NODE_SPACING);
        
        if (mouse_x >= node_x && mouse_x <= node_x + NODE_SIZE &&
            mouse_y >= node_y && mouse_y <= node_y + NODE_SIZE) {
            
            // Try to unlock this node
            if (node.state == TowerForge::Core::ResearchNodeState::Upgradable) {
                return research_tree.UnlockNode(node.id);
            }
            
            return false;
        }
    }
    
    return false;
}

} // namespace ui
} // namespace towerforge
