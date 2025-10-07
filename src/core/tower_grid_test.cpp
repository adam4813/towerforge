#include "core/tower_grid.hpp"
#include <iostream>
#include <cassert>

using namespace TowerForge::Core;

// Simple test framework
int test_count = 0;
int passed_count = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " << #name << "... "; \
        test_count++; \
        try { \
            test_##name(); \
            passed_count++; \
            std::cout << "PASSED" << std::endl; \
        } catch (const std::exception& e) { \
            std::cout << "FAILED: " << e.what() << std::endl; \
        } \
    } \
    void test_##name()

#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        throw std::runtime_error("Assertion failed: " #expr); \
    }

#define ASSERT_FALSE(expr) \
    if (expr) { \
        throw std::runtime_error("Assertion failed: !" #expr); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " != " #actual); \
    }

// Test: Grid initialization
TEST(grid_initialization) {
    TowerGrid grid(5, 10);
    
    ASSERT_EQ(5, grid.GetFloorCount());
    ASSERT_EQ(10, grid.GetColumnCount());
    ASSERT_EQ(0, grid.GetOccupiedCellCount());
}

// Test: Add floors
TEST(add_floors) {
    TowerGrid grid(2, 5);
    
    int new_floor = grid.AddFloor();
    ASSERT_EQ(2, new_floor);
    ASSERT_EQ(3, grid.GetFloorCount());
    
    int first_new = grid.AddFloors(3);
    ASSERT_EQ(3, first_new);
    ASSERT_EQ(6, grid.GetFloorCount());
}

// Test: Add columns
TEST(add_columns) {
    TowerGrid grid(3, 4);
    
    int new_col = grid.AddColumn();
    ASSERT_EQ(4, new_col);
    ASSERT_EQ(5, grid.GetColumnCount());
    
    int first_new = grid.AddColumns(2);
    ASSERT_EQ(5, first_new);
    ASSERT_EQ(7, grid.GetColumnCount());
}

// Test: Place facility
TEST(place_facility) {
    TowerGrid grid(3, 10);
    
    // Place a facility of width 3 at floor 1, column 2
    bool result = grid.PlaceFacility(1, 2, 3, 42);
    ASSERT_TRUE(result);
    
    // Verify cells are occupied
    ASSERT_TRUE(grid.IsOccupied(1, 2));
    ASSERT_TRUE(grid.IsOccupied(1, 3));
    ASSERT_TRUE(grid.IsOccupied(1, 4));
    ASSERT_FALSE(grid.IsOccupied(1, 5));
    
    // Verify facility ID
    ASSERT_EQ(42, grid.GetFacilityAt(1, 2));
    ASSERT_EQ(42, grid.GetFacilityAt(1, 3));
    ASSERT_EQ(42, grid.GetFacilityAt(1, 4));
    
    // Verify occupied count
    ASSERT_EQ(3, grid.GetOccupiedCellCount());
}

// Test: Place facility out of bounds
TEST(place_facility_out_of_bounds) {
    TowerGrid grid(3, 10);
    
    // Floor out of bounds
    ASSERT_FALSE(grid.PlaceFacility(5, 2, 3, 1));
    
    // Column out of bounds
    ASSERT_FALSE(grid.PlaceFacility(1, 15, 3, 2));
    
    // Width extends beyond grid
    ASSERT_FALSE(grid.PlaceFacility(1, 9, 3, 3));
}

// Test: Place facility on occupied space
TEST(place_facility_overlapping) {
    TowerGrid grid(3, 10);
    
    // Place first facility
    ASSERT_TRUE(grid.PlaceFacility(1, 2, 3, 1));
    
    // Try to place overlapping facility (should fail)
    ASSERT_FALSE(grid.PlaceFacility(1, 3, 2, 2));
    ASSERT_FALSE(grid.PlaceFacility(1, 1, 2, 3));
    
    // Place non-overlapping facility (should succeed)
    ASSERT_TRUE(grid.PlaceFacility(1, 5, 2, 4));
}

// Test: Remove facility by ID
TEST(remove_facility_by_id) {
    TowerGrid grid(3, 10);
    
    // Place facilities
    grid.PlaceFacility(1, 2, 3, 100);
    grid.PlaceFacility(2, 5, 2, 200);
    
    ASSERT_EQ(5, grid.GetOccupiedCellCount());
    
    // Remove facility 100
    bool removed = grid.RemoveFacility(100);
    ASSERT_TRUE(removed);
    ASSERT_EQ(2, grid.GetOccupiedCellCount());
    
    // Verify cells are empty
    ASSERT_FALSE(grid.IsOccupied(1, 2));
    ASSERT_FALSE(grid.IsOccupied(1, 3));
    ASSERT_FALSE(grid.IsOccupied(1, 4));
    
    // Facility 200 should still be there
    ASSERT_TRUE(grid.IsOccupied(2, 5));
    ASSERT_TRUE(grid.IsOccupied(2, 6));
    
    // Try to remove non-existent facility
    ASSERT_FALSE(grid.RemoveFacility(999));
}

// Test: Remove facility at position
TEST(remove_facility_at_position) {
    TowerGrid grid(3, 10);
    
    grid.PlaceFacility(1, 2, 3, 100);
    
    // Remove facility at any cell it occupies
    bool removed = grid.RemoveFacilityAt(1, 3);
    ASSERT_TRUE(removed);
    
    // All cells should be empty now
    ASSERT_FALSE(grid.IsOccupied(1, 2));
    ASSERT_FALSE(grid.IsOccupied(1, 3));
    ASSERT_FALSE(grid.IsOccupied(1, 4));
    ASSERT_EQ(0, grid.GetOccupiedCellCount());
    
    // Try to remove from empty position
    ASSERT_FALSE(grid.RemoveFacilityAt(1, 3));
}

// Test: Space availability check
TEST(space_availability) {
    TowerGrid grid(3, 10);
    
    grid.PlaceFacility(1, 2, 3, 1);
    
    // Space occupied
    ASSERT_FALSE(grid.IsSpaceAvailable(1, 2, 3));
    ASSERT_FALSE(grid.IsSpaceAvailable(1, 3, 2));
    
    // Space available
    ASSERT_TRUE(grid.IsSpaceAvailable(1, 5, 3));
    ASSERT_TRUE(grid.IsSpaceAvailable(2, 2, 3));
    
    // Out of bounds
    ASSERT_FALSE(grid.IsSpaceAvailable(1, 9, 3));
    ASSERT_FALSE(grid.IsSpaceAvailable(5, 2, 3));
}

// Test: Valid position check
TEST(valid_position_check) {
    TowerGrid grid(3, 10);
    
    ASSERT_TRUE(grid.IsValidPosition(0, 0));
    ASSERT_TRUE(grid.IsValidPosition(2, 9));
    
    ASSERT_FALSE(grid.IsValidPosition(-1, 5));
    ASSERT_FALSE(grid.IsValidPosition(5, 5));
    ASSERT_FALSE(grid.IsValidPosition(1, -1));
    ASSERT_FALSE(grid.IsValidPosition(1, 10));
}

// Test: Clear grid
TEST(clear_grid) {
    TowerGrid grid(3, 10);
    
    grid.PlaceFacility(0, 0, 2, 1);
    grid.PlaceFacility(1, 5, 3, 2);
    grid.PlaceFacility(2, 8, 1, 3);
    
    ASSERT_EQ(6, grid.GetOccupiedCellCount());
    
    grid.Clear();
    
    ASSERT_EQ(0, grid.GetOccupiedCellCount());
    ASSERT_FALSE(grid.IsOccupied(0, 0));
    ASSERT_FALSE(grid.IsOccupied(1, 5));
    ASSERT_FALSE(grid.IsOccupied(2, 8));
}

// Test: Remove top floor (empty)
TEST(remove_top_floor_empty) {
    TowerGrid grid(5, 10);
    
    // Remove empty top floor
    bool removed = grid.RemoveTopFloor();
    ASSERT_TRUE(removed);
    ASSERT_EQ(4, grid.GetFloorCount());
    
    // Remove multiple floors
    grid.RemoveTopFloor();
    grid.RemoveTopFloor();
    ASSERT_EQ(2, grid.GetFloorCount());
}

// Test: Remove top floor (occupied)
TEST(remove_top_floor_occupied) {
    TowerGrid grid(3, 10);
    
    // Place facility on top floor
    grid.PlaceFacility(2, 3, 2, 1);
    
    // Try to remove occupied floor (should fail)
    bool removed = grid.RemoveTopFloor();
    ASSERT_FALSE(removed);
    ASSERT_EQ(3, grid.GetFloorCount());
}

// Test: Cannot remove last floor
TEST(cannot_remove_last_floor) {
    TowerGrid grid(1, 10);
    
    bool removed = grid.RemoveTopFloor();
    ASSERT_FALSE(removed);
    ASSERT_EQ(1, grid.GetFloorCount());
}

// Test: Remove right column (empty)
TEST(remove_right_column_empty) {
    TowerGrid grid(5, 10);
    
    // Remove empty right column
    bool removed = grid.RemoveRightColumn();
    ASSERT_TRUE(removed);
    ASSERT_EQ(9, grid.GetColumnCount());
    
    // Remove multiple columns
    grid.RemoveRightColumn();
    grid.RemoveRightColumn();
    ASSERT_EQ(7, grid.GetColumnCount());
}

// Test: Remove right column (occupied)
TEST(remove_right_column_occupied) {
    TowerGrid grid(3, 10);
    
    // Place facility in rightmost column
    grid.PlaceFacility(1, 9, 1, 1);
    
    // Try to remove occupied column (should fail)
    bool removed = grid.RemoveRightColumn();
    ASSERT_FALSE(removed);
    ASSERT_EQ(10, grid.GetColumnCount());
}

// Test: Cannot remove last column
TEST(cannot_remove_last_column) {
    TowerGrid grid(3, 1);
    
    bool removed = grid.RemoveRightColumn();
    ASSERT_FALSE(removed);
    ASSERT_EQ(1, grid.GetColumnCount());
}

// Test: Multiple facilities
TEST(multiple_facilities) {
    TowerGrid grid(5, 20);
    
    // Place multiple facilities
    grid.PlaceFacility(0, 0, 5, 1);    // Lobby
    grid.PlaceFacility(1, 2, 4, 2);    // Office
    grid.PlaceFacility(1, 10, 6, 3);   // Restaurant
    grid.PlaceFacility(2, 5, 3, 4);    // Shop
    grid.PlaceFacility(3, 0, 8, 5);    // Hotel
    
    ASSERT_EQ(26, grid.GetOccupiedCellCount());
    
    // Remove specific facility
    grid.RemoveFacility(3);  // Remove restaurant
    ASSERT_EQ(20, grid.GetOccupiedCellCount());
    
    // Verify restaurant space is now available
    ASSERT_TRUE(grid.IsSpaceAvailable(1, 10, 6));
    
    // Verify other facilities still present
    ASSERT_EQ(1, grid.GetFacilityAt(0, 2));
    ASSERT_EQ(2, grid.GetFacilityAt(1, 3));
    ASSERT_EQ(4, grid.GetFacilityAt(2, 6));
    ASSERT_EQ(5, grid.GetFacilityAt(3, 5));
}

// Test: Edge cases with width
TEST(facility_width_edge_cases) {
    TowerGrid grid(3, 10);
    
    // Width of 1
    ASSERT_TRUE(grid.PlaceFacility(0, 0, 1, 1));
    ASSERT_EQ(1, grid.GetOccupiedCellCount());
    
    // Width spanning entire row
    ASSERT_TRUE(grid.PlaceFacility(1, 0, 10, 2));
    ASSERT_EQ(11, grid.GetOccupiedCellCount());
    
    // Invalid width (0 or negative)
    ASSERT_FALSE(grid.PlaceFacility(2, 0, 0, 3));
    ASSERT_FALSE(grid.PlaceFacility(2, 0, -1, 4));
}

int main() {
    std::cout << "=== TowerGrid Unit Tests ===" << std::endl << std::endl;
    
    // Run all tests
    run_test_grid_initialization();
    run_test_add_floors();
    run_test_add_columns();
    run_test_place_facility();
    run_test_place_facility_out_of_bounds();
    run_test_place_facility_overlapping();
    run_test_remove_facility_by_id();
    run_test_remove_facility_at_position();
    run_test_space_availability();
    run_test_valid_position_check();
    run_test_clear_grid();
    run_test_remove_top_floor_empty();
    run_test_remove_top_floor_occupied();
    run_test_cannot_remove_last_floor();
    run_test_remove_right_column_empty();
    run_test_remove_right_column_occupied();
    run_test_cannot_remove_last_column();
    run_test_multiple_facilities();
    run_test_facility_width_edge_cases();
    
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << test_count << std::endl;
    std::cout << "Passed: " << passed_count << std::endl;
    std::cout << "Failed: " << (test_count - passed_count) << std::endl;
    
    return (test_count == passed_count) ? 0 : 1;
}
