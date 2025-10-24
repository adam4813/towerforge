#include <gtest/gtest.h>
#include "core/ecs_world.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

// Integration tests for ECSWorld
// These tests verify ECS initialization, entity/system management, and subsystem integration

class ECSWorldIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    }

    std::unique_ptr<ECSWorld> ecs_world;
};

TEST_F(ECSWorldIntegrationTest, Initialization) {
    ecs_world->Initialize();
    
    // Verify world is initialized and subsystems are available
    EXPECT_NO_THROW(ecs_world->GetWorld());
    EXPECT_NO_THROW(ecs_world->GetTowerGrid());
    EXPECT_NO_THROW(ecs_world->GetFacilityManager());
    EXPECT_NO_THROW(ecs_world->GetModManager());
}

TEST_F(ECSWorldIntegrationTest, EntityCreation) {
    ecs_world->Initialize();
    
    auto entity1 = ecs_world->CreateEntity("test_entity_1");
    auto entity2 = ecs_world->CreateEntity("test_entity_2");
    
    EXPECT_TRUE(entity1.is_valid());
    EXPECT_TRUE(entity2.is_valid());
    EXPECT_NE(entity1.id(), entity2.id());
}

TEST_F(ECSWorldIntegrationTest, TowerGridAccess) {
    ecs_world->Initialize();
    
    auto& grid = ecs_world->GetTowerGrid();
    
    // Verify grid is functional
    EXPECT_GT(grid.GetFloorCount(), 0);
    EXPECT_GT(grid.GetColumnCount(), 0);
}

TEST_F(ECSWorldIntegrationTest, FacilityManagerAccess) {
    ecs_world->Initialize();
    
    auto& facility_mgr = ecs_world->GetFacilityManager();
    auto& grid = ecs_world->GetTowerGrid();
    
    // Build floor for placement
    grid.BuildFloor(0, 0, 10);
    
    // Create a facility through the manager
    auto facility = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, 0, 0
    );
    
    EXPECT_TRUE(facility.is_valid());
    EXPECT_TRUE(grid.IsOccupied(0, 0));
}

TEST_F(ECSWorldIntegrationTest, ModManagerAccess) {
    ecs_world->Initialize();
    
    const auto& mod_mgr = ecs_world->GetModManager();
    
    // Mod manager should be initialized (may have no mods loaded)
    const auto& mods = mod_mgr.GetLoadedMods();
    EXPECT_NO_THROW(mods.size());
}

TEST_F(ECSWorldIntegrationTest, UpdateCycle) {
    ecs_world->Initialize();
    
    // Update should return true (simulation continues)
    EXPECT_TRUE(ecs_world->Update(0.016f));
    EXPECT_TRUE(ecs_world->Update(0.016f));
}

TEST_F(ECSWorldIntegrationTest, MultipleEntitiesWithComponents) {
    ecs_world->Initialize();
    
    auto& world = ecs_world->GetWorld();
    
    // Create multiple entities with components
    auto e1 = ecs_world->CreateEntity("entity_1");
    e1.set<BuildingComponent>({BuildingComponent::Type::Office, 20, 0});
    
    auto e2 = ecs_world->CreateEntity("entity_2");
    e2.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 30, 0});
    
    auto e3 = ecs_world->CreateEntity("entity_3");
    e3.set<BuildingComponent>({BuildingComponent::Type::Residential, 10, 0});
    
    // Query entities with BuildingComponent
    int count = 0;
    world.each([&](flecs::entity e, BuildingComponent& building) {
        count++;
        EXPECT_GT(building.capacity, 0);
    });
    
    EXPECT_EQ(count, 3);
}

TEST_F(ECSWorldIntegrationTest, VerticalExpansionUpgrade) {
    ecs_world->Initialize();
    ResearchTree research_tree;
    research_tree.InitializeDefaultTree();
    research_tree.AwardPoints(50);
    ecs_world->GetWorld().set<ResearchTree>(research_tree);
    
    auto& grid = ecs_world->GetTowerGrid();
    
    int initial_max_above = grid.GetMaxAboveGroundFloors();
    int initial_max_below = grid.GetMaxBelowGroundFloors();
    
    // Apply vertical expansion upgrades
    ecs_world->ApplyVerticalExpansionUpgrades();
    
    // Limits should remain valid (exact behavior depends on implementation)
    EXPECT_GE(grid.GetMaxAboveGroundFloors(), initial_max_above);
    EXPECT_GE(grid.GetMaxBelowGroundFloors(), initial_max_below);
}

TEST_F(ECSWorldIntegrationTest, FacilityCreationThroughECSWorld) {
    ecs_world->Initialize();
    
    auto& facility_mgr = ecs_world->GetFacilityManager();
    auto& grid = ecs_world->GetTowerGrid();
    
    // Build multiple floors
    for (int i = 0; i < 5; ++i) {
        grid.BuildFloor(i, 0, 20);
    }
    
    // Create facilities on different floors
    auto f1 = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    auto f2 = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 1, 5, 2);
    auto f3 = facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 2, 10, 4);
    
    EXPECT_TRUE(f1.is_valid());
    EXPECT_TRUE(f2.is_valid());
    EXPECT_TRUE(f3.is_valid());
    
    // Verify grid state
    EXPECT_EQ(grid.GetOccupiedCellCount(), 9);
}

TEST_F(ECSWorldIntegrationTest, ComponentQueryAfterUpdate) {
    ecs_world->Initialize();
    
    auto& world = ecs_world->GetWorld();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    auto& grid = ecs_world->GetTowerGrid();
    
    grid.BuildFloor(0, 0, 10);
    
    // Create facilities
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 0, 8);
    
    // Run update
    ecs_world->Update(0.016f);
    
    // Query components - count entities with both BuildingComponent and GridPosition
    int facility_count = 0;
    world.each([&](flecs::entity e) {
        if (e.has<BuildingComponent>() && e.has<GridPosition>()) {
            facility_count++;
            const auto& pos = e.get<GridPosition>();
            EXPECT_GE(pos.floor, 0);
            EXPECT_GE(pos.column, 0);
        }
    });
    
    EXPECT_EQ(facility_count, 2);
}

TEST_F(ECSWorldIntegrationTest, GridAndManagerInteraction) {
    ecs_world->Initialize();
    
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Expand grid
    grid.AddFloors(2);
    grid.AddColumns(5);
    
    int new_floor = grid.GetFloorCount() - 1;
    grid.BuildFloor(new_floor, 0, grid.GetColumnCount());
    
    // Place facility on expanded area
    auto facility = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        new_floor,
        grid.GetColumnCount() - 3,
        2
    );
    
    EXPECT_TRUE(facility.is_valid());
    EXPECT_TRUE(grid.IsOccupied(new_floor, grid.GetColumnCount() - 3));
}

TEST_F(ECSWorldIntegrationTest, SimulationWithTimeProgression) {
    ecs_world->Initialize();
    
    auto& facility_mgr = ecs_world->GetFacilityManager();
    auto& grid = ecs_world->GetTowerGrid();
    
    grid.BuildFloor(0, 0, 10);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    
    // Run multiple update cycles
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(ecs_world->Update(0.016f));
    }
    
    // Simulation should remain stable
    EXPECT_EQ(grid.GetOccupiedCellCount(), 3);
}
