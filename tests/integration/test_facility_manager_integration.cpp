#include <gtest/gtest.h>
#include "core/facility_manager.hpp"
#include "core/tower_grid.hpp"
#include "core/components.hpp"
#include <flecs.h>

using namespace towerforge::core;

// Integration tests for FacilityManager
// These tests verify the interactions between FacilityManager, TowerGrid, and ECS

class FacilityManagerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<flecs::world>();
        grid = std::make_unique<TowerGrid>(10, 20, 0);
        facility_mgr = std::make_unique<FacilityManager>(*world, *grid);
        
        // Build initial floors for testing
        for (int i = 0; i < 10; ++i) {
            grid->BuildFloor(i, 0, 20);
        }
    }

    std::unique_ptr<flecs::world> world;
    std::unique_ptr<TowerGrid> grid;
    std::unique_ptr<FacilityManager> facility_mgr;
};

TEST_F(FacilityManagerIntegrationTest, CreateBasicFacility) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 0, 0, 0
    );

    EXPECT_TRUE(facility.is_valid());
    EXPECT_TRUE(facility.has<BuildingComponent>());
    EXPECT_TRUE(facility.has<GridPosition>());

    const auto& building = facility.get<BuildingComponent>();
    EXPECT_EQ(building.type, BuildingComponent::Type::Office);

    const auto& pos = facility.get<GridPosition>();
    EXPECT_EQ(pos.floor, 0);
    EXPECT_EQ(pos.column, 0);
}

TEST_F(FacilityManagerIntegrationTest, FacilityPlacementOnGrid) {
    auto office = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 2, 5, 3
    );

    EXPECT_TRUE(office.is_valid());

    // Verify grid placement
    EXPECT_TRUE(grid->IsOccupied(2, 5));
    EXPECT_TRUE(grid->IsOccupied(2, 6));
    EXPECT_TRUE(grid->IsOccupied(2, 7));
    EXPECT_EQ(grid->GetFacilityAt(2, 5), static_cast<int>(office.id()));
}

TEST_F(FacilityManagerIntegrationTest, MultipleFacilityTypes) {
    auto office = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 0, 0
    );
    auto restaurant = facility_mgr->CreateFacility(
        BuildingComponent::Type::Restaurant, 0, 8
    );
    auto apartment = facility_mgr->CreateFacility(
        BuildingComponent::Type::Residential, 1, 0
    );

    EXPECT_TRUE(office.is_valid());
    EXPECT_TRUE(restaurant.is_valid());
    EXPECT_TRUE(apartment.is_valid());

    // Verify types
    EXPECT_EQ(office.get<BuildingComponent>().type, BuildingComponent::Type::Office);
    EXPECT_EQ(restaurant.get<BuildingComponent>().type, BuildingComponent::Type::Restaurant);
    EXPECT_EQ(apartment.get<BuildingComponent>().type, BuildingComponent::Type::Residential);
}

TEST_F(FacilityManagerIntegrationTest, FacilityRemoval) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 3, 10, 2
    );

    EXPECT_TRUE(grid->IsOccupied(3, 10));

    // Remove facility
    EXPECT_TRUE(facility_mgr->RemoveFacility(facility));

    // Verify grid is cleared
    EXPECT_FALSE(grid->IsOccupied(3, 10));
    EXPECT_FALSE(grid->IsOccupied(3, 11));

    // Verify entity is no longer valid
    EXPECT_FALSE(facility.is_valid() || facility.is_alive());
}

TEST_F(FacilityManagerIntegrationTest, RemoveFacilityAt) {
    facility_mgr->CreateFacility(
        BuildingComponent::Type::RetailShop, 5, 8, 3
    );

    EXPECT_TRUE(grid->IsOccupied(5, 9));

    // Remove by position
    EXPECT_TRUE(facility_mgr->RemoveFacilityAt(5, 8));

    EXPECT_FALSE(grid->IsOccupied(5, 8));
    EXPECT_FALSE(grid->IsOccupied(5, 9));
    EXPECT_FALSE(grid->IsOccupied(5, 10));
}

TEST_F(FacilityManagerIntegrationTest, DefaultWidthAndCapacity) {
    // Test default width
    EXPECT_EQ(FacilityManager::GetDefaultWidth(BuildingComponent::Type::Office), 8);
    EXPECT_EQ(FacilityManager::GetDefaultWidth(BuildingComponent::Type::Restaurant), 6);
    EXPECT_EQ(FacilityManager::GetDefaultWidth(BuildingComponent::Type::Residential), 6);

    // Test default capacity
    EXPECT_EQ(FacilityManager::GetDefaultCapacity(BuildingComponent::Type::Office), 20);
    EXPECT_EQ(FacilityManager::GetDefaultCapacity(BuildingComponent::Type::Restaurant), 30);
}

TEST_F(FacilityManagerIntegrationTest, FacilityWithComponents) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 1, 5
    );

    // Check for additional components
    const auto& building = facility.get<BuildingComponent>();
    EXPECT_GT(building.capacity, 0);
    EXPECT_GE(building.current_occupancy, 0);
}

TEST_F(FacilityManagerIntegrationTest, FloorBuildCostCalculation) {
    // Create new floor without building it first
    int new_floor = grid->AddFloor();
    
    // Calculate cost to build floor for a facility
    int cost = facility_mgr->CalculateFloorBuildCost(new_floor, 0, 5);
    
    // Cost should be positive (5 cells * floor cost)
    EXPECT_GT(cost, 0);
    EXPECT_EQ(cost, 5 * TowerGrid::GetFloorBuildCost());
}

TEST_F(FacilityManagerIntegrationTest, BuildFloorsForFacility) {
    int new_floor = grid->AddFloor();
    
    // Initially not built
    EXPECT_FALSE(grid->IsFloorBuilt(new_floor, 0));
    
    // Build floors for facility
    EXPECT_TRUE(facility_mgr->BuildFloorsForFacility(new_floor, 2, 4));
    
    // Verify floors are built
    EXPECT_TRUE(grid->IsFloorBuilt(new_floor, 2));
    EXPECT_TRUE(grid->IsFloorBuilt(new_floor, 3));
    EXPECT_TRUE(grid->IsFloorBuilt(new_floor, 4));
    EXPECT_TRUE(grid->IsFloorBuilt(new_floor, 5));
}

TEST_F(FacilityManagerIntegrationTest, ComplexPlacementScenario) {
    // Create facilities on different floors
    auto f1 = facility_mgr->CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    auto f2 = facility_mgr->CreateFacility(BuildingComponent::Type::RetailShop, 0, 5, 2);
    auto f3 = facility_mgr->CreateFacility(BuildingComponent::Type::Restaurant, 1, 2, 4);
    auto f4 = facility_mgr->CreateFacility(BuildingComponent::Type::Residential, 2, 8, 2);

    // Verify all facilities are placed correctly
    EXPECT_TRUE(f1.is_valid());
    EXPECT_TRUE(f2.is_valid());
    EXPECT_TRUE(f3.is_valid());
    EXPECT_TRUE(f4.is_valid());

    // Verify grid occupancy
    EXPECT_EQ(grid->GetOccupiedCellCount(), 11);

    // Remove one facility
    facility_mgr->RemoveFacility(f2);
    EXPECT_EQ(grid->GetOccupiedCellCount(), 9);
}

TEST_F(FacilityManagerIntegrationTest, FacilityTypeRetrieval) {
    auto office = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 4, 10
    );

    int facility_id = static_cast<int>(office.id());
    auto type = facility_mgr->GetFacilityType(facility_id);
    
    EXPECT_EQ(type, BuildingComponent::Type::Office);
}

TEST_F(FacilityManagerIntegrationTest, PreventOverlappingPlacement) {
    // Place first facility
    auto f1 = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 3, 5, 4
    );
    EXPECT_TRUE(f1.is_valid());

    // Try to place overlapping facility (should fail)
    auto f2 = facility_mgr->CreateFacility(
        BuildingComponent::Type::RetailShop, 3, 7, 3
    );
    EXPECT_FALSE(f2.is_valid());

    // Verify only first facility is on grid
    EXPECT_EQ(grid->GetFacilityAt(3, 7), static_cast<int>(f1.id()));
}

TEST_F(FacilityManagerIntegrationTest, CleanFacilityOperation) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::Restaurant, 2, 3
    );

    // Add CleanlinessStatus component manually for testing
    CleanlinessStatus clean_status;
    clean_status.status = CleanlinessStatus::State::Dirty;
    clean_status.time_since_last_clean = 0.0f;
    facility.set<CleanlinessStatus>(clean_status);

    // Clean the facility
    EXPECT_TRUE(facility_mgr->CleanFacility(facility));

    // Verify cleanliness status
    const auto& status = facility.get<CleanlinessStatus>();
    EXPECT_EQ(status.status, CleanlinessStatus::State::Clean);
}

TEST_F(FacilityManagerIntegrationTest, RepairFacilityOperation) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::Office, 1, 8
    );

    // Add MaintenanceStatus component manually for testing
    MaintenanceStatus maint_status;
    maint_status.status = MaintenanceStatus::State::NeedsService;
    maint_status.time_since_last_service = 0.0f;
    maint_status.auto_repair_enabled = false;
    facility.set<MaintenanceStatus>(maint_status);

    // Repair the facility
    EXPECT_TRUE(facility_mgr->RepairFacility(facility));

    // Verify maintenance status
    const auto& status = facility.get<MaintenanceStatus>();
    EXPECT_EQ(status.status, MaintenanceStatus::State::Good);
}

TEST_F(FacilityManagerIntegrationTest, AutoRepairSetting) {
    auto facility = facility_mgr->CreateFacility(
        BuildingComponent::Type::RetailShop, 6, 12
    );

    // Add MaintenanceStatus component
    MaintenanceStatus maint_status;
    maint_status.status = MaintenanceStatus::State::Good;
    maint_status.time_since_last_service = 100.0f;
    maint_status.auto_repair_enabled = false;
    facility.set<MaintenanceStatus>(maint_status);

    // Enable auto-repair
    EXPECT_TRUE(facility_mgr->SetAutoRepair(facility, true));

    const auto& status = facility.get<MaintenanceStatus>();
    EXPECT_TRUE(status.auto_repair_enabled);

    // Disable auto-repair
    EXPECT_TRUE(facility_mgr->SetAutoRepair(facility, false));
    const auto& status2 = facility.get<MaintenanceStatus>();
    EXPECT_FALSE(status2.auto_repair_enabled);
}
