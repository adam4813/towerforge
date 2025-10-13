#include "core/tower_grid.hpp"
#include <algorithm>

namespace TowerForge {
namespace Core {

TowerGrid::TowerGrid(int initial_floors, int initial_columns, int ground_floor_index)
    : floors_(initial_floors), columns_(initial_columns), 
      ground_floor_index_(ground_floor_index), basement_floors_(0) {
    ResizeGrid();
    
    // Initialize ground floor as built by default
    BuildFloor(ground_floor_index_, 0, -1);
}

void TowerGrid::ResizeGrid() {
    grid_.resize(floors_);
    for (auto& row : grid_) {
        row.resize(columns_);
    }
}

int TowerGrid::FloorToGridIndex(int floor) const {
    // Basement floors are negative, so we add basement_floors_ to offset
    return floor - (ground_floor_index_ - basement_floors_);
}

int TowerGrid::GridIndexToFloor(int grid_index) const {
    // Convert back from grid index to floor index
    return grid_index + (ground_floor_index_ - basement_floors_);
}

// Floor management

int TowerGrid::AddFloor() {
    floors_++;
    ResizeGrid();
    return floors_ - 1;
}

int TowerGrid::AddFloors(int count) {
    if (count <= 0) return floors_;
    int first_new_floor = floors_;
    floors_ += count;
    ResizeGrid();
    return first_new_floor;
}

bool TowerGrid::RemoveTopFloor() {
    // Cannot remove if only one floor remains
    if (floors_ <= 1) {
        return false;
    }
    
    // Check if the top floor is empty
    if (!IsFloorEmpty(floors_ - 1)) {
        return false;
    }
    
    floors_--;
    grid_.pop_back();
    return true;
}

int TowerGrid::AddBasementFloor() {
    // Add a floor at the beginning (basement)
    basement_floors_++;
    floors_++;
    
    // Insert new floor at the beginning of grid
    grid_.insert(grid_.begin(), std::vector<GridCell>(columns_));
    
    // Return the new basement floor index
    return ground_floor_index_ - basement_floors_;
}

int TowerGrid::AddBasementFloors(int count) {
    if (count <= 0) return ground_floor_index_ - basement_floors_;
    
    int first_new_basement = ground_floor_index_ - basement_floors_ - count;
    
    for (int i = 0; i < count; ++i) {
        AddBasementFloor();
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
    
    // Check if the bottom floor (first in grid) is empty
    if (!IsFloorEmpty(0)) {
        return false;
    }
    
    basement_floors_--;
    floors_--;
    grid_.erase(grid_.begin());
    return true;
}

// Column management

int TowerGrid::AddColumn() {
    columns_++;
    ResizeGrid();
    return columns_ - 1;
}

int TowerGrid::AddColumns(int count) {
    if (count <= 0) return columns_;
    int first_new_column = columns_;
    columns_ += count;
    ResizeGrid();
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
    for (auto& row : grid_) {
        row.pop_back();
    }
    return true;
}

// Facility placement

bool TowerGrid::PlaceFacility(int floor, int column, int width, int facility_id) {
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
    
    // Convert floor index to grid index
    int grid_idx = FloorToGridIndex(floor);
    
    // Place the facility and mark floor as built
    for (int i = 0; i < width; ++i) {
        grid_[grid_idx][column + i].occupied = true;
        grid_[grid_idx][column + i].facility_id = facility_id;
        grid_[grid_idx][column + i].floor_built = true;
    }
    
    return true;
}

bool TowerGrid::BuildFloor(int floor, int start_column, int width) {
    if (!IsValidPosition(floor, start_column)) {
        return false;
    }
    
    int grid_idx = FloorToGridIndex(floor);
    
    // If width is -1, build all remaining columns
    int actual_width = (width < 0) ? (columns_ - start_column) : width;
    
    // Validate width
    if (start_column + actual_width > columns_) {
        return false;
    }
    
    // Mark cells as built
    for (int i = 0; i < actual_width; ++i) {
        grid_[grid_idx][start_column + i].floor_built = true;
    }
    
    return true;
}

bool TowerGrid::IsFloorBuilt(int floor, int column) const {
    if (!IsValidPosition(floor, column)) {
        return false;
    }
    
    int grid_idx = FloorToGridIndex(floor);
    return grid_[grid_idx][column].floor_built;
}

bool TowerGrid::IsEntireFloorBuilt(int floor) const {
    int grid_idx = FloorToGridIndex(floor);
    
    if (grid_idx < 0 || grid_idx >= floors_) {
        return false;
    }
    
    for (int column = 0; column < columns_; ++column) {
        if (!grid_[grid_idx][column].floor_built) {
            return false;
        }
    }
    
    return true;
}

bool TowerGrid::RemoveFacility(int facility_id) {
    bool found = false;
    
    for (int grid_idx = 0; grid_idx < floors_; ++grid_idx) {
        for (int column = 0; column < columns_; ++column) {
            if (grid_[grid_idx][column].facility_id == facility_id) {
                grid_[grid_idx][column].occupied = false;
                grid_[grid_idx][column].facility_id = -1;
                found = true;
            }
        }
    }
    
    return found;
}

bool TowerGrid::RemoveFacilityAt(int floor, int column) {
    if (!IsValidPosition(floor, column)) {
        return false;
    }
    
    if (!grid_[floor][column].occupied) {
        return false;
    }
    
    int facility_id = grid_[floor][column].facility_id;
    return RemoveFacility(facility_id);
}

// Spatial queries

bool TowerGrid::IsOccupied(int floor, int column) const {
    if (!IsValidPosition(floor, column)) {
        return false;
    }
    
    int grid_idx = FloorToGridIndex(floor);
    return grid_[grid_idx][column].occupied;
}

int TowerGrid::GetFacilityAt(int floor, int column) const {
    if (!IsValidPosition(floor, column)) {
        return -1;
    }
    
    int grid_idx = FloorToGridIndex(floor);
    return grid_[grid_idx][column].facility_id;
}

bool TowerGrid::IsValidPosition(int floor, int column) const {
    int grid_idx = FloorToGridIndex(floor);
    return grid_idx >= 0 && grid_idx < floors_ && column >= 0 && column < columns_;
}

bool TowerGrid::IsSpaceAvailable(int floor, int column, int width) const {
    if (!IsValidPosition(floor, column)) {
        return false;
    }
    
    if (column + width > columns_) {
        return false;
    }
    
    int grid_idx = FloorToGridIndex(floor);
    
    for (int i = 0; i < width; ++i) {
        if (grid_[grid_idx][column + i].occupied) {
            return false;
        }
    }
    
    return true;
}

// Grid information

int TowerGrid::GetOccupiedCellCount() const {
    int count = 0;
    for (const auto& row : grid_) {
        for (const auto& cell : row) {
            if (cell.occupied) {
                count++;
            }
        }
    }
    return count;
}

void TowerGrid::Clear() {
    for (auto& row : grid_) {
        for (auto& cell : row) {
            cell.occupied = false;
            cell.facility_id = -1;
        }
    }
}

// Private helper methods

bool TowerGrid::IsColumnEmpty(int column) const {
    if (column < 0 || column >= columns_) {
        return false;
    }
    
    for (int grid_idx = 0; grid_idx < floors_; ++grid_idx) {
        if (grid_[grid_idx][column].occupied) {
            return false;
        }
    }
    
    return true;
}

bool TowerGrid::IsFloorEmpty(int grid_index) const {
    if (grid_index < 0 || grid_index >= floors_) {
        return false;
    }
    
    for (int column = 0; column < columns_; ++column) {
        if (grid_[grid_index][column].occupied) {
            return false;
        }
    }
    
    return true;
}

} // namespace Core
} // namespace TowerForge
