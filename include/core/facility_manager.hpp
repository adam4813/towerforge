#pragma once

#include <flecs.h>
#include <memory>
#include "core/components.hpp"
#include "core/tower_grid.hpp"

namespace TowerForge {
namespace Core {

/**
 * @brief Manages facility creation, destruction, and lifecycle
 * 
 * The FacilityManager provides a high-level interface for creating and
 * managing facilities in the tower. It handles:
 * - Creating facility entities with appropriate components
 * - Placing facilities on the tower grid
 * - Removing facilities and cleaning up grid references
 * - Providing default attributes for each facility type
 */
class FacilityManager {
public:
    /**
     * @brief Construct a new Facility Manager
     * 
     * @param world Reference to the ECS world
     * @param grid Reference to the tower grid
     */
    FacilityManager(flecs::world& world, TowerGrid& grid);
    
    ~FacilityManager() = default;
    
    /**
     * @brief Create and place a facility on the tower grid
     * 
     * Creates a new entity with BuildingComponent and GridPosition components,
     * and places it on the tower grid.
     * 
     * @param type Type of facility to create
     * @param floor Floor to place the facility on (0-based)
     * @param column Column to place the facility at (0-based)
     * @param width Width of the facility in grid cells (optional, uses default if 0)
     * @param name Optional name for the facility entity
     * @return Entity ID of the created facility, or null entity if creation failed
     */
    flecs::entity CreateFacility(
        BuildingComponent::Type type,
        int floor,
        int column,
        int width = 0,
        const char* name = nullptr
    );
    
    /**
     * @brief Remove a facility from the tower
     * 
     * Removes the facility from the grid and destroys the entity.
     * 
     * @param facility_entity The facility entity to remove
     * @return true if the facility was removed successfully, false otherwise
     */
    bool RemoveFacility(flecs::entity facility_entity);
    
    /**
     * @brief Remove a facility at a specific grid position
     * 
     * @param floor Floor index
     * @param column Column index
     * @return true if a facility was removed, false if the position was empty
     */
    bool RemoveFacilityAt(int floor, int column);
    
    /**
     * @brief Get default width for a facility type
     * 
     * @param type Facility type
     * @return Default width in grid cells
     */
    static int GetDefaultWidth(BuildingComponent::Type type);
    
    /**
     * @brief Get default capacity for a facility type
     * 
     * @param type Facility type
     * @return Default maximum occupancy
     */
    static int GetDefaultCapacity(BuildingComponent::Type type);
    
    /**
     * @brief Get a human-readable name for a facility type
     * 
     * @param type Facility type
     * @return String representation of the type
     */
    static const char* GetTypeName(BuildingComponent::Type type);
    
    /**
     * @brief Get the color for a facility type (for rendering)
     * 
     * @param type Facility type
     * @return Color code for the facility type
     */
    static unsigned int GetFacilityColor(BuildingComponent::Type type);
    
    /**
     * @brief Get the facility type for an entity ID (from grid)
     * 
     * @param facility_entity_id Entity ID from grid
     * @return The facility type, or Type::Office if not found
     */
    BuildingComponent::Type GetFacilityType(int facility_entity_id) const;
    
    /**
     * @brief Calculate the cost to build floors for a facility placement
     * 
     * Checks which cells need floor construction and calculates the cost.
     * 
     * @param floor Floor index
     * @param column Starting column
     * @param width Width of the facility
     * @return Total cost to build the necessary floors
     */
    int CalculateFloorBuildCost(int floor, int column, int width) const;
    
    /**
     * @brief Build the floors needed for a facility placement
     * 
     * Marks the floor cells as built for the given range.
     * 
     * @param floor Floor index
     * @param column Starting column
     * @param width Width of the facility
     * @return true if successful, false otherwise
     */
    bool BuildFloorsForFacility(int floor, int column, int width);

private:
    flecs::world& world_;
    TowerGrid& grid_;
};

} // namespace Core
} // namespace TowerForge
