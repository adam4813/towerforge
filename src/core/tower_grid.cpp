#include "core/tower_grid.hpp"
#include <algorithm>

namespace TowerForge {
namespace Core {

TowerGrid::TowerGrid(int initial_floors, int initial_columns)
    : floors_(initial_floors), columns_(initial_columns) {
    ResizeGrid();
}

void TowerGrid::ResizeGrid() {
    grid_.resize(floors_);
    for (auto& row : grid_) {
        row.resize(columns_);
    }
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
    
    // Place the facility
    for (int i = 0; i < width; ++i) {
        grid_[floor][column + i].occupied = true;
        grid_[floor][column + i].facility_id = facility_id;
    }
    
    return true;
}

bool TowerGrid::RemoveFacility(int facility_id) {
    bool found = false;
    
    for (int floor = 0; floor < floors_; ++floor) {
        for (int column = 0; column < columns_; ++column) {
            if (grid_[floor][column].facility_id == facility_id) {
                grid_[floor][column].occupied = false;
                grid_[floor][column].facility_id = -1;
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
    
    return grid_[floor][column].occupied;
}

int TowerGrid::GetFacilityAt(int floor, int column) const {
    if (!IsValidPosition(floor, column)) {
        return -1;
    }
    
    return grid_[floor][column].facility_id;
}

bool TowerGrid::IsValidPosition(int floor, int column) const {
    return floor >= 0 && floor < floors_ && column >= 0 && column < columns_;
}

bool TowerGrid::IsSpaceAvailable(int floor, int column, int width) const {
    if (!IsValidPosition(floor, column)) {
        return false;
    }
    
    if (column + width > columns_) {
        return false;
    }
    
    for (int i = 0; i < width; ++i) {
        if (grid_[floor][column + i].occupied) {
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
    
    for (int floor = 0; floor < floors_; ++floor) {
        if (grid_[floor][column].occupied) {
            return false;
        }
    }
    
    return true;
}

bool TowerGrid::IsFloorEmpty(int floor) const {
    if (floor < 0 || floor >= floors_) {
        return false;
    }
    
    for (int column = 0; column < columns_; ++column) {
        if (grid_[floor][column].occupied) {
            return false;
        }
    }
    
    return true;
}

} // namespace Core
} // namespace TowerForge
