#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/components.hpp"
#include "ui/build_menu.h"

namespace towerforge {
namespace ui {

/**
 * @brief State of construction for a facility
 */
struct ConstructionState {
    int entity_id;           // Facility entity ID
    float build_time_total;  // Total time to build in seconds
    float build_time_elapsed;// Elapsed build time
    int floor;              // Floor being built on
    int column;             // Column position
    int width;              // Width of facility
    
    ConstructionState(int id, float total, int f, int c, int w)
        : entity_id(id), build_time_total(total), build_time_elapsed(0.0f),
          floor(f), column(c), width(w) {}
    
    float GetProgress() const {
        if (build_time_total <= 0.0f) return 1.0f;
        return build_time_elapsed / build_time_total;
    }
    
    bool IsComplete() const {
        return build_time_elapsed >= build_time_total;
    }
};

/**
 * @brief Action for undo/redo system
 */
struct PlacementAction {
    enum class Type {
        Place,
        Demolish
    };
    
    Type type;
    int entity_id;
    int floor;
    int column;
    int width;
    int facility_type_index;
    int cost;
    
    PlacementAction(Type t, int id, int f, int c, int w, int type_idx, int cst)
        : type(t), entity_id(id), floor(f), column(c), width(w),
          facility_type_index(type_idx), cost(cst) {}
};

/**
 * @brief Interactive building and placement system
 * 
 * Handles mouse interaction for placing and removing facilities,
 * visual feedback, construction progress, and undo/redo.
 */
class PlacementSystem {
public:
    PlacementSystem(TowerForge::Core::TowerGrid& grid,
                   TowerForge::Core::FacilityManager& facility_mgr,
                   BuildMenu& build_menu);
    ~PlacementSystem();
    
    /**
     * @brief Update the placement system
     * @param delta_time Time elapsed since last update
     */
    void Update(float delta_time);
    
    /**
     * @brief Render placement preview and construction visuals
     * @param grid_offset_x X offset for grid rendering
     * @param grid_offset_y Y offset for grid rendering
     * @param cell_width Width of each grid cell
     * @param cell_height Height of each grid cell
     */
    void Render(int grid_offset_x, int grid_offset_y, int cell_width, int cell_height);
    
    /**
     * @brief Handle mouse click for placement/demolition
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param grid_offset_x X offset for grid rendering
     * @param grid_offset_y Y offset for grid rendering
     * @param cell_width Width of each grid cell
     * @param cell_height Height of each grid cell
     * @param current_funds Current available funds
     * @return Cost change (negative for placement, positive for demolition)
     */
    int HandleClick(int mouse_x, int mouse_y, 
                    int grid_offset_x, int grid_offset_y,
                    int cell_width, int cell_height,
                    float current_funds);
    
    /**
     * @brief Handle keyboard shortcuts
     * @return true if a shortcut was handled
     */
    bool HandleKeyboard();
    
    /**
     * @brief Set demolish mode
     */
    void SetDemolishMode(bool enabled) { demolish_mode_ = enabled; }
    
    /**
     * @brief Check if in demolish mode
     */
    bool IsDemolishMode() const { return demolish_mode_; }
    
    /**
     * @brief Undo last action
     */
    void Undo();
    
    /**
     * @brief Redo last undone action
     */
    void Redo();
    
    /**
     * @brief Check if undo is available
     */
    bool CanUndo() const { return !undo_stack_.empty(); }
    
    /**
     * @brief Check if redo is available
     */
    bool CanRedo() const { return !redo_stack_.empty(); }
    
private:
    /**
     * @brief Convert mouse position to grid coordinates
     */
    bool MouseToGrid(int mouse_x, int mouse_y,
                    int grid_offset_x, int grid_offset_y,
                    int cell_width, int cell_height,
                    int& out_floor, int& out_column);
    
    /**
     * @brief Check if placement is valid
     */
    bool IsPlacementValid(int floor, int column, int width, float current_funds, int cost);
    
    /**
     * @brief Place a facility
     */
    bool PlaceFacility(int floor, int column, int facility_type_index, float& funds);
    
    /**
     * @brief Demolish a facility
     */
    bool DemolishFacility(int floor, int column, float& funds);
    
    /**
     * @brief Get build time for facility type
     */
    float GetBuildTime(int facility_type_index);
    
    /**
     * @brief Map facility type index to BuildingComponent::Type
     */
    TowerForge::Core::BuildingComponent::Type GetFacilityType(int facility_type_index);
    
    TowerForge::Core::TowerGrid& grid_;
    TowerForge::Core::FacilityManager& facility_mgr_;
    BuildMenu& build_menu_;
    
    bool demolish_mode_;
    int hover_floor_;
    int hover_column_;
    bool hover_valid_;
    
    std::vector<ConstructionState> constructions_in_progress_;
    std::vector<PlacementAction> undo_stack_;
    std::vector<PlacementAction> redo_stack_;
    
    static constexpr int MAX_UNDO_ACTIONS = 20;
    static constexpr float RECOVERY_PERCENTAGE = 0.5f; // 50% recovery on demolish
};

} // namespace ui
} // namespace towerforge
