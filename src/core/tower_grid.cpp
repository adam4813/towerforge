#include "core/tower_grid.hpp"
#include <algorithm>

namespace TowerForge::Core {

    TowerGrid::TowerGrid(const int initial_floors, const int initial_columns, const int ground_floor_index)
        : floors_(initial_floors), columns_(initial_columns), 
          ground_floor_index_(ground_floor_index), basement_floors_(0),
          max_above_ground_floors_(initial_floors), // Start with initial as max
          max_below_ground_floors_(1) {  // Start with 1 basement floor allowed
        
        // Clamp initial dimensions to absolute maximums
        if (floors_ > MAX_ABOVE_GROUND_FLOORS) {
            floors_ = MAX_ABOVE_GROUND_FLOORS;
        }
        if (columns_ > MAX_HORIZONTAL_CELLS) {
            columns_ = MAX_HORIZONTAL_CELLS;
        }
        
        // Initialize floors in the map
        for (int i = 0; i < floors_; ++i) {
            const int floor_num = ground_floor_index_ + i;
            grid_[floor_num] = std::vector<GridCell>(columns_);
        }
    
        // Initialize ground floor as built by default
        BuildFloor(ground_floor_index_, 0, -1);
    }

    void TowerGrid::EnsureFloorExists(const int floor) {
        if (grid_.find(floor) == grid_.end()) {
            grid_[floor] = std::vector<GridCell>(columns_);
        }
    }

    // Floor management

    int TowerGrid::AddFloor() {
        // Check if we can add more floors
        if (!CanAddFloors(1)) {
            return -1;  // Cannot add floor
        }
        
        const int new_floor = GetHighestFloorIndex() + 1;
        floors_++;
        grid_[new_floor] = std::vector<GridCell>(columns_);
        return new_floor;
    }

    int TowerGrid::AddFloors(const int count) {
        if (count <= 0) return floors_;
        
        // Check if we can add this many floors
        if (!CanAddFloors(count)) {
            return -1;  // Cannot add floors
        }
        
        const int first_new_floor = GetHighestFloorIndex() + 1;
        for (int i = 0; i < count; ++i) {
            const int new_floor = first_new_floor + i;
            grid_[new_floor] = std::vector<GridCell>(columns_);
            floors_++;
        }
        return first_new_floor;
    }

    bool TowerGrid::RemoveTopFloor() {
        // Cannot remove if only one floor remains
        if (floors_ <= 1) {
            return false;
        }
    
        const int top_floor = GetHighestFloorIndex();
        
        // Check if the top floor is empty
        if (!IsFloorEmpty(top_floor)) {
            return false;
        }
    
        floors_--;
        grid_.erase(top_floor);
        return true;
    }

    int TowerGrid::AddBasementFloor() {
        // Check if we can add more basement floors
        if (!CanAddBasementFloors(1)) {
            return -1;  // Cannot add basement floor
        }
        
        basement_floors_++;
        floors_++;
    
        // Add new floor at the bottom
        const int new_basement = ground_floor_index_ - basement_floors_;
        grid_[new_basement] = std::vector<GridCell>(columns_);
    
        return new_basement;
    }

    int TowerGrid::AddBasementFloors(const int count) {
        if (count <= 0) return ground_floor_index_ - basement_floors_;

        // Check if we can add this many basement floors
        if (!CanAddBasementFloors(count)) {
            return -1;  // Cannot add basement floors
        }

        const int first_new_basement = ground_floor_index_ - basement_floors_ - count;
    
        for (int i = 0; i < count; ++i) {
            basement_floors_++;
            floors_++;
            const int new_basement = ground_floor_index_ - basement_floors_;
            grid_[new_basement] = std::vector<GridCell>(columns_);
        }
    
        return first_new_basement;
    }

    bool TowerGrid::RemoveBottomFloor() {
        // Cannot remove if no basement floors exist
        if (basement_floors_ <= 0) {
            return false;
        }
    
        // Cannot remove if only one floor remains
        if (floors_ <= 1) {
            return false;
        }
    
        const int bottom_floor = GetLowestFloorIndex();
        
        // Check if the bottom floor is empty
        if (!IsFloorEmpty(bottom_floor)) {
            return false;
        }
    
        basement_floors_--;
        floors_--;
        grid_.erase(bottom_floor);
        return true;
    }

    // Column management

    int TowerGrid::AddColumn() {
        // Check if we can add more columns
        if (!CanAddColumns(1)) {
            return -1;  // Cannot add column
        }
        
        columns_++;
        // Resize all existing floors to accommodate new column
        for (auto& [floor_num, floor_cells] : grid_) {
            floor_cells.resize(columns_);
        }
        return columns_ - 1;
    }

    int TowerGrid::AddColumns(const int count) {
        if (count <= 0) return columns_;
        
        // Check if we can add this many columns
        if (!CanAddColumns(count)) {
            return -1;  // Cannot add columns
        }
        
        const int first_new_column = columns_;
        columns_ += count;
        // Resize all existing floors to accommodate new columns
        for (auto& [floor_num, floor_cells] : grid_) {
            floor_cells.resize(columns_);
        }
        return first_new_column;
    }

    bool TowerGrid::RemoveRightColumn() {
        // Cannot remove if only one column remains
        if (columns_ <= 1) {
            return false;
        }
    
        // Check if the rightmost column is empty
        if (!IsColumnEmpty(columns_ - 1)) {
            return false;
        }
    
        columns_--;
        for (auto& [floor_num, floor_cells] : grid_) {
            floor_cells.pop_back();
        }
        return true;
    }

    // Facility placement

    bool TowerGrid::PlaceFacility(const int floor, const int column, const int width, const int facility_id) {
        // Validate bounds
        if (!IsValidPosition(floor, column)) {
            return false;
        }
    
        // Check if width is valid
        if (width <= 0 || column + width > columns_) {
            return false;
        }
    
        // Check if space is available
        if (!IsSpaceAvailable(floor, column, width)) {
            return false;
        }
    
        // Ensure floor exists in grid
        EnsureFloorExists(floor);
    
        // Place the facility and mark floor as built
        for (int i = 0; i < width; ++i) {
            grid_[floor][column + i].occupied = true;
            grid_[floor][column + i].facility_id = facility_id;
            grid_[floor][column + i].floor_built = true;
        }
    
        return true;
    }

    bool TowerGrid::BuildFloor(const int floor, const int start_column, const int width) {
        if (!IsValidPosition(floor, start_column)) {
            return false;
        }

        // Ensure floor exists in grid
        EnsureFloorExists(floor);
    
        // If width is -1, build all remaining columns
        const int actual_width = (width < 0) ? (columns_ - start_column) : width;
    
        // Validate width
        if (start_column + actual_width > columns_) {
            return false;
        }
    
        // Mark cells as built
        for (int i = 0; i < actual_width; ++i) {
            grid_[floor][start_column + i].floor_built = true;
        }
    
        return true;
    }

    bool TowerGrid::IsFloorBuilt(const int floor, const int column) const {
        if (!IsValidPosition(floor, column)) {
            return false;
        }

        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            return false;
        }
        return it->second[column].floor_built;
    }

    bool TowerGrid::IsEntireFloorBuilt(const int floor) const {
        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            return false;
        }
    
        for (int column = 0; column < columns_; ++column) {
            if (!it->second[column].floor_built) {
                return false;
            }
        }
    
        return true;
    }

    bool TowerGrid::GetBuiltFloorRange(int& min_floor, int& max_floor) const {
        bool found_built = false;
        min_floor = 0;
        max_floor = 0;

        // Scan all floors to find the range of built floors
        for (const auto& [floor_num, floor_cells] : grid_) {
            // Check if any cell on this floor is built
            bool floor_has_built_cell = false;
            for (int column = 0; column < columns_; ++column) {
                if (floor_cells[column].floor_built) {
                    floor_has_built_cell = true;
                    break;
                }
            }

            if (floor_has_built_cell) {
                if (!found_built) {
                    // First built floor found
                    min_floor = floor_num;
                    max_floor = floor_num;
                    found_built = true;
                } else {
                    // Update range
                    min_floor = std::min(min_floor, floor_num);
                    max_floor = std::max(max_floor, floor_num);
                }
            }
        }

        return found_built;
    }

    bool TowerGrid::RemoveFacility(const int facility_id) {
        bool found = false;
    
        for (auto& [floor_num, floor_cells] : grid_) {
            for (int column = 0; column < columns_; ++column) {
                if (floor_cells[column].facility_id == facility_id) {
                    floor_cells[column].occupied = false;
                    floor_cells[column].facility_id = -1;
                    found = true;
                }
            }
        }
    
        return found;
    }

    bool TowerGrid::RemoveFacilityAt(const int floor, const int column) {
        if (!IsValidPosition(floor, column)) {
            return false;
        }
    
        auto it = grid_.find(floor);
        if (it == grid_.end() || !it->second[column].occupied) {
            return false;
        }

        const int facility_id = it->second[column].facility_id;
        return RemoveFacility(facility_id);
    }

    // Spatial queries

    bool TowerGrid::IsOccupied(const int floor, const int column) const {
        if (!IsValidPosition(floor, column)) {
            return false;
        }

        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            return false;
        }
        return it->second[column].occupied;
    }

    int TowerGrid::GetFacilityAt(const int floor, const int column) const {
        if (!IsValidPosition(floor, column)) {
            return -1;
        }

        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            return -1;
        }
        return it->second[column].facility_id;
    }

    bool TowerGrid::IsValidPosition(const int floor, const int column) const {
        // Check if floor is within allowed range
        const int lowest = GetLowestFloorIndex();
        const int highest = GetHighestFloorIndex();
        return floor >= lowest && floor <= highest && column >= 0 && column < columns_;
    }

    bool TowerGrid::IsSpaceAvailable(const int floor, const int column, const int width) const {
        if (!IsValidPosition(floor, column)) {
            return false;
        }
    
        if (column + width > columns_) {
            return false;
        }

        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            // Floor doesn't exist yet, so space is available
            return true;
        }
    
        for (int i = 0; i < width; ++i) {
            if (it->second[column + i].occupied) {
                return false;
            }
        }
    
        return true;
    }

    // Grid information

    int TowerGrid::GetOccupiedCellCount() const {
        int count = 0;
        for (const auto& [floor_num, floor_cells] : grid_) {
            for (const auto& cell : floor_cells) {
                if (cell.occupied) {
                    count++;
                }
            }
        }
        return count;
    }

    void TowerGrid::Clear() {
        for (auto& [floor_num, floor_cells] : grid_) {
            for (auto& cell : floor_cells) {
                cell.occupied = false;
                cell.facility_id = -1;
            }
        }
    }

    // Private helper methods

    bool TowerGrid::IsColumnEmpty(const int column) const {
        if (column < 0 || column >= columns_) {
            return false;
        }
    
        for (const auto& [floor_num, floor_cells] : grid_) {
            if (floor_cells[column].occupied) {
                return false;
            }
        }
    
        return true;
    }

    bool TowerGrid::IsFloorEmpty(const int floor) const {
        auto it = grid_.find(floor);
        if (it == grid_.end()) {
            return true; // Non-existent floor is empty
        }
    
        for (int column = 0; column < columns_; ++column) {
            if (it->second[column].occupied) {
                return false;
            }
        }
    
        return true;
    }

    // Dimension limit methods

    void TowerGrid::SetMaxAboveGroundFloors(const int max_floors) {
        // Clamp to absolute maximum
        max_above_ground_floors_ = std::min(max_floors, MAX_ABOVE_GROUND_FLOORS);
        // Ensure at least current count
        max_above_ground_floors_ = std::max(max_above_ground_floors_, GetAboveGroundFloorCount());
    }

    void TowerGrid::SetMaxBelowGroundFloors(const int max_floors) {
        // Clamp to absolute maximum
        max_below_ground_floors_ = std::min(max_floors, MAX_BELOW_GROUND_FLOORS);
        // Ensure at least current count
        max_below_ground_floors_ = std::max(max_below_ground_floors_, basement_floors_);
    }

    bool TowerGrid::CanAddFloors(const int count) const {
        const int current_above_ground = GetAboveGroundFloorCount();
        return (current_above_ground + count) <= max_above_ground_floors_;
    }

    bool TowerGrid::CanAddBasementFloors(const int count) const {
        return (basement_floors_ + count) <= max_below_ground_floors_;
    }

    bool TowerGrid::CanAddColumns(const int count) const {
        return (columns_ + count) <= MAX_HORIZONTAL_CELLS;
    }

    int TowerGrid::GetAboveGroundFloorCount() const {
        return floors_ - basement_floors_;
    }

}
