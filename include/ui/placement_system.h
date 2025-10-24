#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/components.hpp"
#include "core/command_history.hpp"
#include "ui/build_menu.h"
#include "ui/ui_element.h"

// Forward declaration
namespace towerforge::rendering {
    class Camera;
}

namespace towerforge::ui {

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
    
        ConstructionState(const int id, const float total, const int f, const int c, const int w)
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
     * @brief Process mouse events for confirmation dialogs
     * @param event Mouse event data
     * @return true if event was consumed
     */
        bool ProcessMouseEvent(const MouseEvent& event) const;
    
        /**
     * @brief Handle keyboard shortcuts
     * @return true if a shortcut was handled
     */
        bool HandleKeyboard();
    
        /**
     * @brief Set demolish mode
     */
        void SetDemolishMode(const bool enabled) { demolish_mode_ = enabled; }
    
        /**
     * @brief Check if in demolish mode
     */
        bool IsDemolishMode() const { return demolish_mode_; }
    
        /**
     * @brief Undo last action
     * @param funds Current funds to adjust
     * @return true if undo succeeded
     */
        bool Undo(float& funds);
    
        /**
     * @brief Redo last undone action
     * @param funds Current funds to adjust
     * @return true if redo succeeded
     */
        bool Redo(float& funds);
    
        /**
     * @brief Check if undo is available
     */
        bool CanUndo() const;
    
        /**
     * @brief Check if redo is available
     */
        bool CanRedo() const;
    
        /**
     * @brief Get the command history
     */
        const TowerForge::Core::CommandHistory& GetCommandHistory() const { return command_history_; }
    
        /**
     * @brief Set the camera for coordinate transformation
     */
        void SetCamera(rendering::Camera* camera) { camera_ = camera; }
    
        /**
     * @brief Update tooltips for grid placement
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param grid_offset_x Grid offset X
     * @param grid_offset_y Grid offset Y
     * @param cell_width Cell width
     * @param cell_height Cell height
     * @param current_funds Current player funds
     */
        void UpdateTooltips(int mouse_x, int mouse_y, int grid_offset_x, int grid_offset_y,
                            int cell_width, int cell_height, float current_funds) const;
    
        /**
     * @brief Set the tooltip manager
     */
        void SetTooltipManager(TooltipManager* tooltip_manager) { tooltip_manager_ = tooltip_manager; }
    
        /**
     * @brief Check if a confirmation dialog is currently showing
     */
        bool HasPendingConfirmation() const { 
            return demolish_confirmation_ && demolish_confirmation_->IsVisible(); 
        }
        
        /**
         * @brief Get pending funds change from confirmed demolish
         * @return Funds change (positive for refund) or 0
         */
        int GetPendingFundsChange();
    
    private:
        /**
     * @brief Convert mouse position to grid coordinates
     */
        bool MouseToGrid(int mouse_x, int mouse_y,
                         int grid_offset_x, int grid_offset_y,
                         int cell_width, int cell_height,
                         int& out_floor, int& out_column) const;
    
        /**
     * @brief Check if placement is valid
     */
        bool IsPlacementValid(int floor, int column, int width, float current_funds, int cost) const;
    
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
        static float GetBuildTime(int facility_type_index);
    
        /**
     * @brief Map facility type index to BuildingComponent::Type
     */
        static TowerForge::Core::BuildingComponent::Type GetFacilityType(int facility_type_index);
    
        TowerForge::Core::TowerGrid& grid_;
        TowerForge::Core::FacilityManager& facility_mgr_;
        BuildMenu& build_menu_;
        rendering::Camera* camera_;
    
        bool demolish_mode_;
        int hover_floor_;
        int hover_column_;
        bool hover_valid_;
    
        std::vector<ConstructionState> constructions_in_progress_;
        TowerForge::Core::CommandHistory command_history_;
        TooltipManager* tooltip_manager_;
    
        // Confirmation dialog for destructive actions
        std::unique_ptr<ConfirmationDialog> demolish_confirmation_;
        int pending_demolish_floor_;
        int pending_demolish_column_;
        float pending_demolish_funds_;
        int pending_funds_change_;  // Stores the result of confirmed demolish
    
        static constexpr float RECOVERY_PERCENTAGE = 0.5f; // 50% recovery on demolish
    };

}
