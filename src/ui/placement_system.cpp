#include "ui/placement_system.h"
#include "rendering/camera.h"
#include <iostream>
#include <algorithm>

namespace towerforge {
namespace ui {

PlacementSystem::PlacementSystem(TowerForge::Core::TowerGrid& grid,
                                 TowerForge::Core::FacilityManager& facility_mgr,
                                 BuildMenu& build_menu)
    : grid_(grid)
    , facility_mgr_(facility_mgr)
    , build_menu_(build_menu)
    , camera_(nullptr)
    , demolish_mode_(false)
    , hover_floor_(-1)
    , hover_column_(-1)
    , hover_valid_(false) {
}

PlacementSystem::~PlacementSystem() {
}

void PlacementSystem::Update(float delta_time) {
    // Update construction progress
    for (auto it = constructions_in_progress_.begin(); it != constructions_in_progress_.end();) {
        it->build_time_elapsed += delta_time;
        
        if (it->IsComplete()) {
            // Construction complete - remove from list
            it = constructions_in_progress_.erase(it);
        } else {
            ++it;
        }
    }
}

void PlacementSystem::Render(int grid_offset_x, int grid_offset_y, int cell_width, int cell_height) {
    // Get mouse position and convert to world coordinates if camera is set
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    
    if (camera_ != nullptr) {
        float world_x, world_y;
        camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);
        mouse_x = static_cast<int>(world_x);
        mouse_y = static_cast<int>(world_y);
    }
    
    // Convert to grid coordinates
    int floor, column;
    if (MouseToGrid(mouse_x, mouse_y, grid_offset_x, grid_offset_y, cell_width, cell_height, floor, column)) {
        hover_floor_ = floor;
        hover_column_ = column;
        
        // Determine hover validity
        if (demolish_mode_) {
            // Valid if there's a facility to demolish
            hover_valid_ = grid_.IsOccupied(floor, column);
        } else {
            int selected = build_menu_.GetSelectedFacility();
            if (selected >= 0) {
                const auto& types = build_menu_.GetFacilityTypes();
                const auto& facility_type = types[selected];
                
                // Check if placement is valid (will be checked with funds in HandleClick)
                hover_valid_ = grid_.IsSpaceAvailable(floor, column, facility_type.width);
            } else {
                hover_valid_ = false;
            }
        }
    } else {
        hover_floor_ = -1;
        hover_column_ = -1;
        hover_valid_ = false;
    }
    
    // Render placement preview
    if (hover_floor_ >= 0 && hover_column_ >= 0) {
        int selected = build_menu_.GetSelectedFacility();
        
        if (demolish_mode_) {
            // Red highlight for demolish
            if (hover_valid_) {
                int facility_id = grid_.GetFacilityAt(hover_floor_, hover_column_);
                if (facility_id >= 0) {
                    // Find the full width of the facility
                    int start_col = hover_column_;
                    while (start_col > 0 && grid_.GetFacilityAt(hover_floor_, start_col - 1) == facility_id) {
                        start_col--;
                    }
                    int end_col = hover_column_;
                    while (end_col < grid_.GetColumnCount() - 1 && 
                           grid_.GetFacilityAt(hover_floor_, end_col + 1) == facility_id) {
                        end_col++;
                    }
                    int width = end_col - start_col + 1;
                    
                    // Draw red outline around facility to demolish
                    int x = grid_offset_x + start_col * cell_width;
                    int y = grid_offset_y + hover_floor_ * cell_height;
                    DrawRectangle(x, y, width * cell_width, cell_height, ColorAlpha(RED, 0.3f));
                    DrawRectangleLines(x, y, width * cell_width, cell_height, RED);
                }
            }
        } else if (selected >= 0) {
            const auto& types = build_menu_.GetFacilityTypes();
            const auto& facility_type = types[selected];
            
            // Draw ghost preview
            int x = grid_offset_x + hover_column_ * cell_width;
            int y = grid_offset_y + hover_floor_ * cell_height;
            
            Color preview_color = hover_valid_ ? ColorAlpha(GREEN, 0.3f) : ColorAlpha(RED, 0.3f);
            Color outline_color = hover_valid_ ? GREEN : RED;
            
            DrawRectangle(x, y, facility_type.width * cell_width, cell_height, preview_color);
            DrawRectangleLines(x, y, facility_type.width * cell_width, cell_height, outline_color);
            
            // Draw icon
            DrawText(facility_type.icon.c_str(), x + 5, y + 5, 20, WHITE);
            
            // Draw cost indicator
            if (hover_valid_) {
                DrawText(TextFormat("$%d", facility_type.cost), x + 5, y + cell_height - 20, 12, GREEN);
            } else {
                DrawText("INVALID", x + 5, y + cell_height - 20, 12, RED);
            }
        }
    }
    
    // Render construction progress
    for (const auto& construction : constructions_in_progress_) {
        int x = grid_offset_x + construction.column * cell_width;
        int y = grid_offset_y + construction.floor * cell_height;
        
        // Draw construction overlay
        DrawRectangle(x, y, construction.width * cell_width, cell_height, 
                     ColorAlpha(ORANGE, 0.4f));
        
        // Draw progress bar
        int bar_width = construction.width * cell_width - 10;
        int bar_height = 8;
        int bar_x = x + 5;
        int bar_y = y + cell_height / 2 - bar_height / 2;
        
        DrawRectangle(bar_x, bar_y, bar_width, bar_height, DARKGRAY);
        DrawRectangle(bar_x, bar_y, static_cast<int>(bar_width * construction.GetProgress()), 
                     bar_height, YELLOW);
        
        // Draw construction text
        DrawText(TextFormat("Building... %d%%", static_cast<int>(construction.GetProgress() * 100)),
                x + 5, y + 5, 10, WHITE);
    }
}

int PlacementSystem::HandleClick(int mouse_x, int mouse_y,
                                 int grid_offset_x, int grid_offset_y,
                                 int cell_width, int cell_height,
                                 float current_funds) {
    int floor, column;
    if (!MouseToGrid(mouse_x, mouse_y, grid_offset_x, grid_offset_y, 
                     cell_width, cell_height, floor, column)) {
        return 0;
    }
    
    if (demolish_mode_) {
        // Demolish facility
        float funds = current_funds;
        if (DemolishFacility(floor, column, funds)) {
            return static_cast<int>(funds - current_funds); // Positive (refund)
        }
    } else {
        int selected = build_menu_.GetSelectedFacility();
        if (selected >= 0) {
            float funds = current_funds;
            if (PlaceFacility(floor, column, selected, funds)) {
                return static_cast<int>(funds - current_funds); // Negative (cost)
            }
        }
    }
    
    return 0;
}

bool PlacementSystem::HandleKeyboard() {
    // Keyboard shortcuts
    // 1-5 for facility types
    if (IsKeyPressed(KEY_ONE)) {
        if (build_menu_.GetFacilityTypes().size() > 0) {
            demolish_mode_ = false;
            // Select index 0 (Lobby)
            return true;
        }
    } else if (IsKeyPressed(KEY_TWO)) {
        if (build_menu_.GetFacilityTypes().size() > 1) {
            demolish_mode_ = false;
            // Select index 1 (Office)
            return true;
        }
    } else if (IsKeyPressed(KEY_THREE)) {
        if (build_menu_.GetFacilityTypes().size() > 2) {
            demolish_mode_ = false;
            // Select index 2
            return true;
        }
    } else if (IsKeyPressed(KEY_FOUR)) {
        if (build_menu_.GetFacilityTypes().size() > 3) {
            demolish_mode_ = false;
            // Select index 3
            return true;
        }
    } else if (IsKeyPressed(KEY_FIVE)) {
        if (build_menu_.GetFacilityTypes().size() > 4) {
            demolish_mode_ = false;
            // Select index 4
            return true;
        }
    }
    
    // D for demolish
    if (IsKeyPressed(KEY_D)) {
        demolish_mode_ = !demolish_mode_;
        build_menu_.ClearSelection();
        return true;
    }
    
    // Ctrl+Z for undo
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
        Undo();
        return true;
    }
    
    // Ctrl+Y for redo
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y)) {
        Redo();
        return true;
    }
    
    return false;
}

void PlacementSystem::Undo() {
    if (undo_stack_.empty()) {
        return;
    }
    
    // Get last action
    PlacementAction action = undo_stack_.back();
    undo_stack_.pop_back();
    
    // Reverse the action
    if (action.type == PlacementAction::Type::Place) {
        // Remove the placed facility
        facility_mgr_.RemoveFacilityAt(action.floor, action.column);
    } else {
        // Re-place the demolished facility (simplified - doesn't restore exact state)
        // In a full implementation, we'd store more details
    }
    
    // Add to redo stack
    redo_stack_.push_back(action);
    if (redo_stack_.size() > MAX_UNDO_ACTIONS) {
        redo_stack_.erase(redo_stack_.begin());
    }
}

void PlacementSystem::Redo() {
    if (redo_stack_.empty()) {
        return;
    }
    
    // Get last undone action
    PlacementAction action = redo_stack_.back();
    redo_stack_.pop_back();
    
    // Re-apply the action
    if (action.type == PlacementAction::Type::Place) {
        // Re-place the facility
        auto facility_type = GetFacilityType(action.facility_type_index);
        facility_mgr_.CreateFacility(facility_type, action.floor, action.column, action.width);
    } else {
        // Re-demolish
        facility_mgr_.RemoveFacilityAt(action.floor, action.column);
    }
    
    // Add back to undo stack
    undo_stack_.push_back(action);
}

bool PlacementSystem::MouseToGrid(int mouse_x, int mouse_y,
                                  int grid_offset_x, int grid_offset_y,
                                  int cell_width, int cell_height,
                                  int& out_floor, int& out_column) {
    // Convert mouse position to grid coordinates
    int rel_x = mouse_x - grid_offset_x;
    int rel_y = mouse_y - grid_offset_y;
    
    if (rel_x < 0 || rel_y < 0) {
        return false;
    }
    
    out_column = rel_x / cell_width;
    out_floor = rel_y / cell_height;
    
    // Check bounds
    if (out_floor < 0 || out_floor >= grid_.GetFloorCount() ||
        out_column < 0 || out_column >= grid_.GetColumnCount()) {
        return false;
    }
    
    return true;
}

bool PlacementSystem::IsPlacementValid(int floor, int column, int width, 
                                      float current_funds, int cost) {
    // Check funds
    if (current_funds < cost) {
        return false;
    }
    
    // Check space availability
    if (!grid_.IsSpaceAvailable(floor, column, width)) {
        return false;
    }
    
    // Additional validation could go here (adjacency rules, etc.)
    
    return true;
}

bool PlacementSystem::PlaceFacility(int floor, int column, int facility_type_index, float& funds) {
    const auto& types = build_menu_.GetFacilityTypes();
    if (facility_type_index < 0 || facility_type_index >= static_cast<int>(types.size())) {
        return false;
    }
    
    const auto& facility_type = types[facility_type_index];
    
    // Validate placement
    if (!IsPlacementValid(floor, column, facility_type.width, funds, facility_type.cost)) {
        return false;
    }
    
    // Deduct cost
    funds -= facility_type.cost;
    
    // Map to BuildingComponent::Type
    auto bc_type = GetFacilityType(facility_type_index);
    
    // Create facility
    auto entity = facility_mgr_.CreateFacility(bc_type, floor, column, facility_type.width);
    if (!entity) {
        // Failed to create - refund
        funds += facility_type.cost;
        return false;
    }
    
    // Add to construction queue
    float build_time = GetBuildTime(facility_type_index);
    constructions_in_progress_.emplace_back(
        static_cast<int>(entity.id()), build_time, floor, column, facility_type.width
    );
    
    // Add to undo stack
    PlacementAction action(PlacementAction::Type::Place, static_cast<int>(entity.id()),
                          floor, column, facility_type.width, facility_type_index,
                          facility_type.cost);
    undo_stack_.push_back(action);
    if (undo_stack_.size() > MAX_UNDO_ACTIONS) {
        undo_stack_.erase(undo_stack_.begin());
    }
    
    // Clear redo stack
    redo_stack_.clear();
    
    return true;
}

bool PlacementSystem::DemolishFacility(int floor, int column, float& funds) {
    if (!grid_.IsOccupied(floor, column)) {
        return false;
    }
    
    int facility_id = grid_.GetFacilityAt(floor, column);
    if (facility_id < 0) {
        return false;
    }
    
    // Calculate refund (50% of original cost)
    // For now, use a simple estimate based on facility type
    int refund = 500; // Placeholder - should look up actual cost
    
    // Remove facility
    if (!facility_mgr_.RemoveFacilityAt(floor, column)) {
        return false;
    }
    
    // Add refund
    funds += refund;
    
    // Add to undo stack
    PlacementAction action(PlacementAction::Type::Demolish, facility_id,
                          floor, column, 1, -1, refund);
    undo_stack_.push_back(action);
    if (undo_stack_.size() > MAX_UNDO_ACTIONS) {
        undo_stack_.erase(undo_stack_.begin());
    }
    
    // Clear redo stack
    redo_stack_.clear();
    
    return true;
}

float PlacementSystem::GetBuildTime(int facility_type_index) {
    // Build times in seconds (real-time)
    // Lobby: 10s, Office: 15s, Restaurant: 20s, Shop: 15s, Hotel: 25s
    // Gym: 18s, Arcade: 16s, Theater: 22s, Conference: 20s, Flagship: 28s, Elevator: 12s
    static const float build_times[] = {10.0f, 15.0f, 20.0f, 15.0f, 25.0f, 18.0f, 16.0f, 22.0f, 20.0f, 28.0f, 12.0f};
    
    if (facility_type_index >= 0 && facility_type_index < 11) {
        return build_times[facility_type_index];
    }
    
    return 10.0f; // Default
}

TowerForge::Core::BuildingComponent::Type PlacementSystem::GetFacilityType(int facility_type_index) {
    // Map build menu index to BuildingComponent::Type
    // Based on BuildMenu initialization order:
    // 0: Lobby, 1: Office, 2: Restaurant, 3: Shop (RetailShop), 4: Hotel, 
    // 5: Gym, 6: Arcade, 7: Theater, 8: Conference (ConferenceHall), 
    // 9: Flagship (FlagshipStore), 10: Elevator
    
    switch (facility_type_index) {
        case 0: return TowerForge::Core::BuildingComponent::Type::Lobby;
        case 1: return TowerForge::Core::BuildingComponent::Type::Office;
        case 2: return TowerForge::Core::BuildingComponent::Type::Restaurant;
        case 3: return TowerForge::Core::BuildingComponent::Type::RetailShop;
        case 4: return TowerForge::Core::BuildingComponent::Type::Hotel;
        case 5: return TowerForge::Core::BuildingComponent::Type::Gym;
        case 6: return TowerForge::Core::BuildingComponent::Type::Arcade;
        case 7: return TowerForge::Core::BuildingComponent::Type::Theater;
        case 8: return TowerForge::Core::BuildingComponent::Type::ConferenceHall;
        case 9: return TowerForge::Core::BuildingComponent::Type::FlagshipStore;
        case 10: return TowerForge::Core::BuildingComponent::Type::Elevator;
        default: return TowerForge::Core::BuildingComponent::Type::Office;
    }
}

} // namespace ui
} // namespace towerforge
