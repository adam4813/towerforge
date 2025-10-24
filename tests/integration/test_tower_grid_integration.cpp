#include <gtest/gtest.h>
#include "core/tower_grid.hpp"

using namespace TowerForge::Core;

// Integration tests for TowerGrid
// These tests verify the interactions between TowerGrid's floor/column management,
// facility placement, and spatial queries

class TowerGridIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        grid = std::make_unique<TowerGrid>(5, 10, 0);
    }

    std::unique_ptr<TowerGrid> grid;
};

TEST_F(TowerGridIntegrationTest, InitialState) {
    EXPECT_EQ(grid->GetFloorCount(), 5);
    EXPECT_EQ(grid->GetColumnCount(), 10);
    EXPECT_EQ(grid->GetGroundFloorIndex(), 0);
    EXPECT_EQ(grid->GetOccupiedCellCount(), 0);
}

TEST_F(TowerGridIntegrationTest, FloorExpansionAndPlacement) {
    // Add floors and place facilities
    int new_floor = grid->AddFloor();
    EXPECT_EQ(new_floor, 5);
    EXPECT_EQ(grid->GetFloorCount(), 6);

    // Build the floor before placing
    EXPECT_TRUE(grid->BuildFloor(new_floor, 0, 5));

    // Place a facility on the new floor
    EXPECT_TRUE(grid->PlaceFacility(new_floor, 0, 3, 100));
    EXPECT_TRUE(grid->IsOccupied(new_floor, 0));
    EXPECT_TRUE(grid->IsOccupied(new_floor, 1));
    EXPECT_TRUE(grid->IsOccupied(new_floor, 2));
    EXPECT_FALSE(grid->IsOccupied(new_floor, 3));

    // Verify facility ID
    EXPECT_EQ(grid->GetFacilityAt(new_floor, 0), 100);
    EXPECT_EQ(grid->GetFacilityAt(new_floor, 1), 100);
    EXPECT_EQ(grid->GetFacilityAt(new_floor, 2), 100);
}

TEST_F(TowerGridIntegrationTest, BasementExpansionAndPlacement) {
    // Add basement floors
    int basement = grid->AddBasementFloor();
    EXPECT_EQ(basement, -1);
    EXPECT_EQ(grid->GetLowestFloorIndex(), -1);

    // Build the basement floor
    EXPECT_TRUE(grid->BuildFloor(basement, 0, 4));

    // Place facility in basement
    EXPECT_TRUE(grid->PlaceFacility(basement, 0, 4, 200));
    EXPECT_EQ(grid->GetFacilityAt(basement, 0), 200);
    EXPECT_EQ(grid->GetOccupiedCellCount(), 4);
}

TEST_F(TowerGridIntegrationTest, MultipleFloorAdditionsWithPlacement) {
    // Add multiple floors at once
    int first_new = grid->AddFloors(3);
    EXPECT_EQ(first_new, 5);
    EXPECT_EQ(grid->GetFloorCount(), 8);

    // Build floors
    for (int i = 5; i < 8; ++i) {
        grid->BuildFloor(i, 0, -1);
    }

    // Place facilities on different floors
    EXPECT_TRUE(grid->PlaceFacility(5, 0, 2, 101));
    EXPECT_TRUE(grid->PlaceFacility(6, 3, 3, 102));
    EXPECT_TRUE(grid->PlaceFacility(7, 5, 2, 103));

    EXPECT_EQ(grid->GetOccupiedCellCount(), 7);
}

TEST_F(TowerGridIntegrationTest, ColumnExpansionAndWideEacility) {
    // Add columns
    int new_col = grid->AddColumns(5);
    EXPECT_EQ(new_col, 10);
    EXPECT_EQ(grid->GetColumnCount(), 15);

    // Build floor across all columns
    grid->BuildFloor(0, 0, 15);

    // Place a wide facility using new columns
    EXPECT_TRUE(grid->PlaceFacility(0, 10, 5, 300));
    EXPECT_TRUE(grid->IsOccupied(0, 10));
    EXPECT_TRUE(grid->IsOccupied(0, 14));
    EXPECT_EQ(grid->GetFacilityAt(0, 12), 300);
}

TEST_F(TowerGridIntegrationTest, FacilityRemovalAndReplacement) {
    // Build and place initial facility
    grid->BuildFloor(0, 0, 10);
    EXPECT_TRUE(grid->PlaceFacility(0, 0, 4, 400));
    EXPECT_EQ(grid->GetOccupiedCellCount(), 4);

    // Remove facility
    EXPECT_TRUE(grid->RemoveFacility(400));
    EXPECT_EQ(grid->GetOccupiedCellCount(), 0);
    EXPECT_FALSE(grid->IsOccupied(0, 0));

    // Place a different facility in the same space
    EXPECT_TRUE(grid->PlaceFacility(0, 0, 4, 401));
    EXPECT_EQ(grid->GetFacilityAt(0, 2), 401);
}

TEST_F(TowerGridIntegrationTest, SpaceAvailabilityChecks) {
    // Build floor
    grid->BuildFloor(0, 0, 10);
    
    // Place a facility
    grid->PlaceFacility(0, 2, 3, 500);

    // Check space availability
    EXPECT_TRUE(grid->IsSpaceAvailable(0, 0, 2));  // Before facility
    EXPECT_FALSE(grid->IsSpaceAvailable(0, 1, 3)); // Overlaps facility
    EXPECT_FALSE(grid->IsSpaceAvailable(0, 2, 3)); // On facility
    EXPECT_TRUE(grid->IsSpaceAvailable(0, 5, 2));  // After facility
}

TEST_F(TowerGridIntegrationTest, FloorRemovalWithEmptyFloors) {
    // Add and build extra floors
    grid->AddFloors(2);
    grid->BuildFloor(5, 0, 10);
    grid->BuildFloor(6, 0, 10);

    // Remove empty top floor
    EXPECT_TRUE(grid->RemoveTopFloor());
    EXPECT_EQ(grid->GetFloorCount(), 6);

    // Try to remove another
    EXPECT_TRUE(grid->RemoveTopFloor());
    EXPECT_EQ(grid->GetFloorCount(), 5);
}

TEST_F(TowerGridIntegrationTest, FloorRemovalBlockedByOccupiedCells) {
    // Add floor and place facility
    int top = grid->AddFloor();
    grid->BuildFloor(top, 0, 5);
    grid->PlaceFacility(top, 0, 2, 600);

    // Cannot remove occupied floor
    EXPECT_FALSE(grid->RemoveTopFloor());
    EXPECT_EQ(grid->GetFloorCount(), 6);

    // Remove facility, then floor can be removed
    grid->RemoveFacility(600);
    EXPECT_TRUE(grid->RemoveTopFloor());
    EXPECT_EQ(grid->GetFloorCount(), 5);
}

TEST_F(TowerGridIntegrationTest, ComplexMultiFloorScenario) {
    // Build a complex tower with basements, ground, and upper floors
    grid->AddBasementFloors(2);
    grid->AddFloors(3);

    // Build all floors
    for (int f = grid->GetLowestFloorIndex(); f <= grid->GetHighestFloorIndex(); ++f) {
        grid->BuildFloor(f, 0, 10);
    }

    // Place facilities on different floors
    grid->PlaceFacility(-2, 0, 3, 701);  // Basement 2
    grid->PlaceFacility(-1, 4, 2, 702);  // Basement 1
    grid->PlaceFacility(0, 6, 4, 703);   // Ground
    grid->PlaceFacility(3, 0, 5, 704);   // Floor 3
    grid->PlaceFacility(5, 5, 3, 705);   // Floor 5

    // Verify all placements
    EXPECT_EQ(grid->GetFacilityAt(-2, 1), 701);
    EXPECT_EQ(grid->GetFacilityAt(-1, 5), 702);
    EXPECT_EQ(grid->GetFacilityAt(0, 7), 703);
    EXPECT_EQ(grid->GetFacilityAt(3, 2), 704);
    EXPECT_EQ(grid->GetFacilityAt(5, 6), 705);

    EXPECT_EQ(grid->GetOccupiedCellCount(), 17);
}

TEST_F(TowerGridIntegrationTest, BuiltFloorRangeTracking) {
    // Add floors but only build some
    grid->AddFloors(3);
    grid->BuildFloor(2, 0, 5);
    grid->BuildFloor(5, 2, 3);
    grid->BuildFloor(7, 0, 2);

    int min_floor, max_floor;
    EXPECT_TRUE(grid->GetBuiltFloorRange(min_floor, max_floor));
    EXPECT_EQ(min_floor, 0);
    EXPECT_EQ(max_floor, 7);
}

TEST_F(TowerGridIntegrationTest, DimensionLimitsAndUpgrades) {
    // Check initial limits
    EXPECT_EQ(grid->GetMaxAboveGroundFloors(), 200);
    EXPECT_EQ(grid->GetMaxBelowGroundFloors(), 20);

    // Try to add within limits
    EXPECT_TRUE(grid->CanAddFloors(10));
    EXPECT_TRUE(grid->CanAddBasementFloors(5));

    // Upgrade limits
    grid->SetMaxAboveGroundFloors(250);
    grid->SetMaxBelowGroundFloors(30);

    EXPECT_EQ(grid->GetMaxAboveGroundFloors(), 200);
    EXPECT_EQ(grid->GetMaxBelowGroundFloors(), 20);
}

TEST_F(TowerGridIntegrationTest, ClearAllFacilities) {
    // Build and populate grid
    grid->BuildFloor(0, 0, 10);
    grid->BuildFloor(1, 0, 10);
    grid->PlaceFacility(0, 0, 3, 800);
    grid->PlaceFacility(0, 5, 2, 801);
    grid->PlaceFacility(1, 2, 4, 802);

    EXPECT_EQ(grid->GetOccupiedCellCount(), 9);

    // Clear all
    grid->Clear();
    EXPECT_EQ(grid->GetOccupiedCellCount(), 0);
    EXPECT_FALSE(grid->IsOccupied(0, 0));
    EXPECT_FALSE(grid->IsOccupied(1, 2));
}
