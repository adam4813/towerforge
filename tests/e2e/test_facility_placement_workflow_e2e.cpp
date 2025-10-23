#include <gtest/gtest.h>
#include "core/ecs_world.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

// E2E test for facility placement workflow
// This test verifies the complete workflow of placing facilities in a tower

class FacilityPlacementWorkflowE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
        ecs_world->Initialize();
    }

    void TearDown() override {
        ecs_world.reset();
    }

    std::unique_ptr<ECSWorld> ecs_world;
};

TEST_F(FacilityPlacementWorkflowE2ETest, PlaceSingleFacilityWorkflow) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Check floor availability
    EXPECT_GT(grid.GetFloorCount(), 0);
    
    // Step 2: Build floor if needed
    if (!grid.IsFloorBuilt(0, 0)) {
        ASSERT_TRUE(grid.BuildFloor(0, 0, 10));
    }
    
    // Step 3: Check space availability
    EXPECT_TRUE(grid.IsSpaceAvailable(0, 0, 3));
    
    // Step 4: Place facility
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        0, 0, 3
    );
    
    EXPECT_TRUE(office.is_valid());
    
    // Step 5: Verify placement on grid
    EXPECT_TRUE(grid.IsOccupied(0, 0));
    EXPECT_TRUE(grid.IsOccupied(0, 1));
    EXPECT_TRUE(grid.IsOccupied(0, 2));
    
    // Step 6: Verify entity has correct components
    EXPECT_TRUE(office.has<BuildingComponent>());
    EXPECT_TRUE(office.has<GridPosition>());
    
    const auto& building = office.get<BuildingComponent>();
    EXPECT_EQ(building.type, BuildingComponent::Type::Office);
    
    const auto& pos = office.get<GridPosition>();
    EXPECT_EQ(pos.floor, 0);
    EXPECT_EQ(pos.column, 0);
    EXPECT_EQ(pos.width, 3);
}

TEST_F(FacilityPlacementWorkflowE2ETest, BuildCompleteFloorWithFacilities) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Build entire floor
    int floor = 1;
    int total_width = 20;
    ASSERT_TRUE(grid.BuildFloor(floor, 0, total_width));
    
    // Step 2: Place multiple facilities on the floor
    auto office1 = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, floor, 0, 5
    );
    
    auto restaurant = facility_mgr.CreateFacility(
        BuildingComponent::Type::Restaurant, floor, 5, 4
    );
    
    auto office2 = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, floor, 9, 5
    );
    
    auto shop = facility_mgr.CreateFacility(
        BuildingComponent::Type::Shop, floor, 14, 3
    );
    
    // Step 3: Verify all placements
    EXPECT_TRUE(office1.is_valid());
    EXPECT_TRUE(restaurant.is_valid());
    EXPECT_TRUE(office2.is_valid());
    EXPECT_TRUE(shop.is_valid());
    
    // Step 4: Verify grid occupancy
    EXPECT_EQ(grid.GetOccupiedCellCount(), 17);
    
    // Step 5: Verify no overlaps
    EXPECT_EQ(grid.GetFacilityAt(floor, 0), static_cast<int>(office1.id()));
    EXPECT_EQ(grid.GetFacilityAt(floor, 5), static_cast<int>(restaurant.id()));
    EXPECT_EQ(grid.GetFacilityAt(floor, 9), static_cast<int>(office2.id()));
    EXPECT_EQ(grid.GetFacilityAt(floor, 14), static_cast<int>(shop.id()));
}

TEST_F(FacilityPlacementWorkflowE2ETest, ExpandTowerAndPlaceFacilities) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Add new floors
    int initial_floors = grid.GetFloorCount();
    grid.AddFloors(3);
    EXPECT_EQ(grid.GetFloorCount(), initial_floors + 3);
    
    // Step 2: Build new floors
    for (int i = initial_floors; i < grid.GetFloorCount(); ++i) {
        grid.BuildFloor(i, 0, 15);
    }
    
    // Step 3: Place facilities on new floors
    auto apt1 = facility_mgr.CreateFacility(
        BuildingComponent::Type::Apartment, initial_floors, 0, 2
    );
    auto apt2 = facility_mgr.CreateFacility(
        BuildingComponent::Type::Apartment, initial_floors, 5, 2
    );
    auto apt3 = facility_mgr.CreateFacility(
        BuildingComponent::Type::Apartment, initial_floors + 1, 0, 2
    );
    
    EXPECT_TRUE(apt1.is_valid());
    EXPECT_TRUE(apt2.is_valid());
    EXPECT_TRUE(apt3.is_valid());
    
    // Step 4: Run simulation to ensure stability
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(ecs_world->Update(0.016f));
    }
}

TEST_F(FacilityPlacementWorkflowE2ETest, PlaceBasementFacilities) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Add basement floors
    int basement1 = grid.AddBasementFloor();
    int basement2 = grid.AddBasementFloor();
    
    EXPECT_EQ(basement1, -1);
    EXPECT_EQ(basement2, -2);
    
    // Step 2: Build basement floors
    grid.BuildFloor(basement1, 0, 10);
    grid.BuildFloor(basement2, 0, 10);
    
    // Step 3: Place facilities in basement (parking, storage, etc.)
    auto storage1 = facility_mgr.CreateFacility(
        BuildingComponent::Type::ManagementOffice, basement1, 0, 5
    );
    auto storage2 = facility_mgr.CreateFacility(
        BuildingComponent::Type::ManagementOffice, basement2, 2, 4
    );
    
    EXPECT_TRUE(storage1.is_valid());
    EXPECT_TRUE(storage2.is_valid());
    
    // Step 4: Verify basement placements
    EXPECT_TRUE(grid.IsOccupied(basement1, 0));
    EXPECT_TRUE(grid.IsOccupied(basement2, 2));
}

TEST_F(FacilityPlacementWorkflowE2ETest, RemoveAndReplaceFacility) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    grid.BuildFloor(0, 0, 20);
    
    // Step 1: Place initial facility
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, 0, 5, 3
    );
    ASSERT_TRUE(office.is_valid());
    EXPECT_EQ(grid.GetOccupiedCellCount(), 3);
    
    // Step 2: Remove facility
    ASSERT_TRUE(facility_mgr.RemoveFacility(office));
    EXPECT_EQ(grid.GetOccupiedCellCount(), 0);
    EXPECT_FALSE(grid.IsOccupied(0, 5));
    
    // Step 3: Place replacement facility
    auto restaurant = facility_mgr.CreateFacility(
        BuildingComponent::Type::Restaurant, 0, 5, 4
    );
    EXPECT_TRUE(restaurant.is_valid());
    EXPECT_EQ(grid.GetOccupiedCellCount(), 4);
    
    // Step 4: Verify replacement
    EXPECT_EQ(grid.GetFacilityAt(0, 5), static_cast<int>(restaurant.id()));
}

TEST_F(FacilityPlacementWorkflowE2ETest, PreventInvalidPlacements) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    grid.BuildFloor(0, 0, 10);
    
    // Place first facility
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, 0, 2, 4
    );
    ASSERT_TRUE(office.is_valid());
    
    // Try to place overlapping facility (should fail)
    auto shop = facility_mgr.CreateFacility(
        BuildingComponent::Type::Shop, 0, 4, 3
    );
    EXPECT_FALSE(shop.is_valid());
    
    // Verify original facility is intact
    EXPECT_EQ(grid.GetFacilityAt(0, 4), static_cast<int>(office.id()));
    EXPECT_EQ(grid.GetOccupiedCellCount(), 4);
}

TEST_F(FacilityPlacementWorkflowE2ETest, PlaceFacilitiesAcrossMultipleFloorsWithSimulation) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Build multiple floors
    for (int floor = 0; floor < 5; ++floor) {
        grid.BuildFloor(floor, 0, 15);
    }
    
    // Place lobby on ground floor
    auto lobby = facility_mgr.CreateFacility(
        BuildingComponent::Type::Lobby, 0, 5, 5
    );
    
    // Place offices on floor 1
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 0, 3);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 5, 3);
    
    // Place restaurants on floor 2
    facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 2, 2, 4);
    
    // Place apartments on floors 3-4
    for (int floor = 3; floor < 5; ++floor) {
        for (int col = 0; col < 12; col += 2) {
            facility_mgr.CreateFacility(BuildingComponent::Type::Apartment, floor, col, 2);
        }
    }
    
    // Run simulation for extended period
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(ecs_world->Update(0.016f));
    }
    
    // Verify all facilities remain valid
    EXPECT_GT(grid.GetOccupiedCellCount(), 20);
}

TEST_F(FacilityPlacementWorkflowE2ETest, CalculateAndBuildFloorCosts) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Add new floor without building it
    int new_floor = grid.AddFloor();
    
    // Calculate cost to build floor for facility
    int cost = facility_mgr.CalculateFloorBuildCost(new_floor, 0, 5);
    EXPECT_GT(cost, 0);
    EXPECT_EQ(cost, 5 * TowerGrid::GetFloorBuildCost());
    
    // Build floors for facility
    ASSERT_TRUE(facility_mgr.BuildFloorsForFacility(new_floor, 0, 5));
    
    // Now place facility
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, new_floor, 0, 5
    );
    
    EXPECT_TRUE(office.is_valid());
    EXPECT_TRUE(grid.IsFloorBuilt(new_floor, 0));
}
