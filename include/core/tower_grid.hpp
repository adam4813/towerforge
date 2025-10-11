#pragma once

#include <vector>
#include <optional>
#include <memory>

namespace TowerForge {
namespace Core {

// Forward declaration
struct BuildingComponent;

/**
 * @brief Represents a cell in the tower grid
 * 
 * Each cell can be empty or contain a facility.
 */
struct GridCell {
    bool occupied = false;
    int facility_id = -1;  // Entity ID of the facility occupying this cell
    bool floor_built = false;  // Whether this floor position has been constructed
    
    GridCell() = default;
};

/**
 * @brief 2D Grid system for tower structure
 * 
 * This grid manages the spatial layout of the tower, tracking floors (vertical)
 * and columns (horizontal). It supports placement and removal of facilities
 * and provides spatial query functions.
 */
class TowerGrid {
public:
    /**
     * @brief Construct a new Tower Grid with initial dimensions
     * 
     * @param initial_floors Number of floors to start with (default: 1)
     * @param initial_columns Number of columns to start with (default: 10)
     * @param ground_floor_index Floor index representing ground level (default: 0)
     */
    TowerGrid(int initial_floors = 1, int initial_columns = 10, int ground_floor_index = 0);
    
    ~TowerGrid() = default;
    
    // Floor management
    
    /**
     * @brief Add a new floor at the top of the tower
     * @return The index of the newly added floor
     */
    int AddFloor();
    
    /**
     * @brief Add multiple floors at the top of the tower
     * @param count Number of floors to add
     * @return The index of the first newly added floor
     */
    int AddFloors(int count);
    
    /**
     * @brief Remove the topmost floor if it's empty
     * @return true if the floor was removed, false if it was occupied or is the last floor
     */
    bool RemoveTopFloor();
    
    /**
     * @brief Add a basement floor below the current lowest floor
     * @return The index of the newly added basement floor (negative)
     */
    int AddBasementFloor();
    
    /**
     * @brief Add multiple basement floors below the current lowest floor
     * @param count Number of basement floors to add
     * @return The index of the first newly added basement floor
     */
    int AddBasementFloors(int count);
    
    /**
     * @brief Remove the lowest basement floor if it's empty
     * @return true if the basement floor was removed, false if it was occupied or is ground level
     */
    bool RemoveBottomFloor();
    
    // Column management
    
    /**
     * @brief Add a new column to the right side of the tower
     * @return The index of the newly added column
     */
    int AddColumn();
    
    /**
     * @brief Add multiple columns to the right side of the tower
     * @param count Number of columns to add
     * @return The index of the first newly added column
     */
    int AddColumns(int count);
    
    /**
     * @brief Remove the rightmost column if it's empty
     * @return true if the column was removed, false if it was occupied or is the last column
     */
    bool RemoveRightColumn();
    
    // Facility placement
    
    /**
     * @brief Place a facility at the specified position
     * 
     * @param floor Floor index (0-based)
     * @param column Column index (0-based)
     * @param width Width of the facility in grid cells
     * @param facility_id The entity ID of the facility
     * @return true if placement was successful, false if the space is occupied or out of bounds
     */
    bool PlaceFacility(int floor, int column, int width, int facility_id);
    
    /**
     * @brief Remove a facility from the grid
     * 
     * @param facility_id The entity ID of the facility to remove
     * @return true if the facility was found and removed, false otherwise
     */
    bool RemoveFacility(int facility_id);
    
    /**
     * @brief Remove a facility at a specific position
     * 
     * @param floor Floor index
     * @param column Column index
     * @return true if a facility was removed, false if the position was empty
     */
    bool RemoveFacilityAt(int floor, int column);
    
    // Floor and column building state
    
    /**
     * @brief Mark a floor as built (constructed)
     * 
     * @param floor Floor index
     * @param start_column Starting column to build (default: 0)
     * @param width Number of columns to build (default: all remaining columns)
     * @return true if successful, false if floor is out of bounds
     */
    bool BuildFloor(int floor, int start_column = 0, int width = -1);
    
    /**
     * @brief Check if a floor position is built
     * 
     * @param floor Floor index
     * @param column Column index
     * @return true if the floor is built at that position, false otherwise
     */
    bool IsFloorBuilt(int floor, int column) const;
    
    /**
     * @brief Check if an entire floor row is built
     * 
     * @param floor Floor index
     * @return true if all columns on this floor are built
     */
    bool IsEntireFloorBuilt(int floor) const;
    
    /**
     * @brief Get the cost to build a floor cell
     * 
     * @return Cost in currency units to build one floor cell
     */
    static int GetFloorBuildCost() { return 50; }
    
    // Spatial queries
    
    /**
     * @brief Check if a position is occupied
     * 
     * @param floor Floor index
     * @param column Column index
     * @return true if the position is occupied, false otherwise
     */
    bool IsOccupied(int floor, int column) const;
    
    /**
     * @brief Get the facility ID at a specific position
     * 
     * @param floor Floor index
     * @param column Column index
     * @return The facility ID if occupied, -1 otherwise
     */
    int GetFacilityAt(int floor, int column) const;
    
    /**
     * @brief Check if a position is within grid bounds
     * 
     * @param floor Floor index
     * @param column Column index
     * @return true if the position is valid, false otherwise
     */
    bool IsValidPosition(int floor, int column) const;
    
    /**
     * @brief Check if a range of cells is available for placement
     * 
     * @param floor Floor index
     * @param column Starting column index
     * @param width Width in cells
     * @return true if all cells in the range are empty, false otherwise
     */
    bool IsSpaceAvailable(int floor, int column, int width) const;
    
    // Grid information
    
    /**
     * @brief Get the current number of floors
     * @return Number of floors
     */
    int GetFloorCount() const { return floors_; }
    
    /**
     * @brief Get the current number of columns
     * @return Number of columns
     */
    int GetColumnCount() const { return columns_; }
    
    /**
     * @brief Get the ground floor index
     * @return Floor index representing ground level (usually 0, but may be offset if basements exist)
     */
    int GetGroundFloorIndex() const { return ground_floor_index_; }
    
    /**
     * @brief Get the lowest floor index (may be negative for basements)
     * @return The lowest floor index
     */
    int GetLowestFloorIndex() const { return ground_floor_index_ - basement_floors_; }
    
    /**
     * @brief Get the highest floor index
     * @return The highest floor index
     */
    int GetHighestFloorIndex() const { return ground_floor_index_ + floors_ - basement_floors_ - 1; }
    
    /**
     * @brief Get the total number of occupied cells
     * @return Count of occupied cells
     */
    int GetOccupiedCellCount() const;
    
    /**
     * @brief Clear all facilities from the grid
     */
    void Clear();

private:
    int floors_;
    int columns_;
    int ground_floor_index_;  // Index representing ground level (0 by default)
    int basement_floors_;      // Number of basement floors (negative indices)
    std::vector<std::vector<GridCell>> grid_;  // grid_[floor][column]
    
    /**
     * @brief Resize the internal grid structure
     */
    void ResizeGrid();
    
    /**
     * @brief Convert a floor index to grid array index
     * @param floor Floor index (can be negative for basements)
     * @return Array index in grid_ (always positive)
     */
    int FloorToGridIndex(int floor) const;
    
    /**
     * @brief Convert a grid array index to floor index
     * @param grid_index Array index in grid_
     * @return Floor index (can be negative for basements)
     */
    int GridIndexToFloor(int grid_index) const;
    
    /**
     * @brief Check if a column is completely empty
     * @param column Column index
     * @return true if all cells in the column are empty
     */
    bool IsColumnEmpty(int column) const;
    
    /**
     * @brief Check if a floor is completely empty
     * @param floor Floor index
     * @return true if all cells in the floor are empty
     */
    bool IsFloorEmpty(int floor) const;
};

} // namespace Core
} // namespace TowerForge
