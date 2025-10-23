#include <gtest/gtest.h>
#include "core/ecs_world.hpp"
#include "core/save_load_manager.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

// E2E test for save/load workflow
// This test verifies the complete workflow of saving and loading game state

class SaveLoadWorkflowE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
        ecs_world->Initialize();
        
        save_mgr = std::make_unique<SaveLoadManager>();
        save_mgr->Initialize();
        
        // Clean up test saves
        CleanupTestSaves();
    }

    void TearDown() override {
        CleanupTestSaves();
        ecs_world.reset();
        save_mgr.reset();
    }

    void CleanupTestSaves() {
        std::vector<std::string> test_slots = {
            "e2e_test_1", "e2e_test_2", "e2e_complex", "e2e_quicksave"
        };
        
        for (const auto& slot : test_slots) {
            if (save_mgr && save_mgr->SlotExists(slot)) {
                save_mgr->DeleteSlot(slot);
            }
        }
    }

    std::unique_ptr<ECSWorld> ecs_world;
    std::unique_ptr<SaveLoadManager> save_mgr;
};

TEST_F(SaveLoadWorkflowE2ETest, CompleteSaveLoadCycle) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Create game state
    grid.BuildFloor(0, 0, 10);
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office, 0, 0, 3
    );
    ASSERT_TRUE(office.is_valid());
    
    int initial_occupancy = grid.GetOccupiedCellCount();
    
    // Step 2: Save game
    auto save_result = save_mgr->SaveGame("e2e_test_1", "Test Tower", *ecs_world);
    ASSERT_TRUE(save_result.success);
    EXPECT_TRUE(save_mgr->SlotExists("e2e_test_1"));
    
    // Step 3: Create fresh world
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    // Step 4: Load game into fresh world
    auto load_result = save_mgr->LoadGame("e2e_test_1", *new_world);
    ASSERT_TRUE(load_result.success);
    
    // Step 5: Verify loaded state matches saved state
    auto& loaded_grid = new_world->GetTowerGrid();
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), initial_occupancy);
    EXPECT_TRUE(loaded_grid.IsOccupied(0, 0));
}

TEST_F(SaveLoadWorkflowE2ETest, SaveComplexTowerAndReload) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Step 1: Build complex tower
    grid.AddFloors(5);
    grid.AddBasementFloors(2);
    
    // Build all floors
    for (int floor = grid.GetLowestFloorIndex(); floor <= grid.GetHighestFloorIndex(); ++floor) {
        grid.BuildFloor(floor, 0, 15);
    }
    
    // Place various facilities
    facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 5, 5);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 0, 3);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 5, 3);
    facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 2, 2, 4);
    facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 3, 0, 2);
    facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 3, 3, 2);
    facility_mgr.CreateFacility(BuildingComponent::Type::ManagementOffice, -1, 0, 5);
    facility_mgr.CreateFacility(BuildingComponent::Type::ManagementOffice, -2, 5, 4);
    
    int initial_occupancy = grid.GetOccupiedCellCount();
    int initial_floors = grid.GetFloorCount();
    
    // Step 2: Run simulation
    for (int i = 0; i < 50; ++i) {
        ecs_world->Update(0.016f);
    }
    
    // Step 3: Save complex state
    auto save_result = save_mgr->SaveGame("e2e_complex", "Complex Tower", *ecs_world);
    ASSERT_TRUE(save_result.success);
    
    // Step 4: Load into new world
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("e2e_complex", *new_world);
    ASSERT_TRUE(load_result.success);
    
    // Step 5: Verify complex state
    auto& loaded_grid = new_world->GetTowerGrid();
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), initial_occupancy);
    EXPECT_EQ(loaded_grid.GetFloorCount(), initial_floors);
    EXPECT_TRUE(loaded_grid.IsOccupied(0, 5));  // Lobby
    EXPECT_TRUE(loaded_grid.IsOccupied(-1, 0)); // Basement storage
}

TEST_F(SaveLoadWorkflowE2ETest, MultipleQuickSaves) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    grid.BuildFloor(0, 0, 20);
    
    // Save state 1
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    auto result1 = save_mgr->SaveGame("e2e_quicksave", "v1", *ecs_world);
    ASSERT_TRUE(result1.success);
    
    // Modify and save state 2 (overwrite)
    facility_mgr.CreateFacility(BuildingComponent::Type::Shop, 0, 5, 2);
    auto result2 = save_mgr->SaveGame("e2e_quicksave", "v2", *ecs_world);
    ASSERT_TRUE(result2.success);
    
    // Load should get latest state
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("e2e_quicksave", *new_world);
    ASSERT_TRUE(load_result.success);
    
    auto& loaded_grid = new_world->GetTowerGrid();
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), 5);  // 3 + 2
}

TEST_F(SaveLoadWorkflowE2ETest, SaveLoadWithSlotManagement) {
    auto& grid = ecs_world->GetTowerGrid();
    
    grid.BuildFloor(0, 0, 10);
    
    // Create multiple save slots
    save_mgr->SaveGame("e2e_test_1", "Tower 1", *ecs_world);
    save_mgr->SaveGame("e2e_test_2", "Tower 2", *ecs_world);
    
    // Verify slots exist
    auto slots = save_mgr->GetSaveSlots();
    EXPECT_GE(slots.size(), 2);
    
    bool found_1 = false, found_2 = false;
    for (const auto& slot : slots) {
        if (slot.slot_name == "e2e_test_1") found_1 = true;
        if (slot.slot_name == "e2e_test_2") found_2 = true;
    }
    EXPECT_TRUE(found_1);
    EXPECT_TRUE(found_2);
    
    // Delete one slot
    auto delete_result = save_mgr->DeleteSlot("e2e_test_1");
    EXPECT_TRUE(delete_result.success);
    EXPECT_FALSE(save_mgr->SlotExists("e2e_test_1"));
    EXPECT_TRUE(save_mgr->SlotExists("e2e_test_2"));
}

TEST_F(SaveLoadWorkflowE2ETest, AutosaveWorkflow) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Enable autosave with short interval
    save_mgr->SetAutosaveEnabled(true);
    save_mgr->SetAutosaveInterval(5.0f);  // 5 seconds
    
    grid.BuildFloor(0, 0, 10);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    
    // Simulate time passing
    float total_time = 0.0f;
    for (int i = 0; i < 400; ++i) {  // ~6.6 seconds
        float delta = 0.016f;
        total_time += delta;
        
        ecs_world->Update(delta);
        save_mgr->UpdateAutosave(delta, *ecs_world);
    }
    
    // Autosave should have triggered
    EXPECT_TRUE(save_mgr->SlotExists("_autosave"));
    
    // Load autosave
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("_autosave", *new_world);
    EXPECT_TRUE(load_result.success);
    
    save_mgr->DeleteSlot("_autosave");
}

TEST_F(SaveLoadWorkflowE2ETest, SaveAfterModifyingTower) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Initial state
    grid.BuildFloor(0, 0, 10);
    auto office = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    
    // Save initial
    save_mgr->SaveGame("e2e_test_1", "Initial", *ecs_world);
    
    // Modify tower
    facility_mgr.RemoveFacility(office);
    grid.AddFloor();
    grid.BuildFloor(1, 0, 10);
    facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 1, 0, 4);
    
    int modified_occupancy = grid.GetOccupiedCellCount();
    
    // Save modified state
    save_mgr->SaveGame("e2e_test_1", "Modified", *ecs_world);
    
    // Load and verify modified state
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("e2e_test_1", *new_world);
    ASSERT_TRUE(load_result.success);
    
    auto& loaded_grid = new_world->GetTowerGrid();
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), modified_occupancy);
    EXPECT_FALSE(loaded_grid.IsOccupied(0, 0));  // Old office removed
    EXPECT_TRUE(loaded_grid.IsOccupied(1, 0));   // New restaurant
}

TEST_F(SaveLoadWorkflowE2ETest, ContinuePlayingAfterLoad) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Create and save initial state
    grid.BuildFloor(0, 0, 10);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    save_mgr->SaveGame("e2e_test_1", "Test", *ecs_world);
    
    // Load into new world
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    save_mgr->LoadGame("e2e_test_1", *new_world);
    
    // Continue playing - add more facilities
    auto& loaded_grid = new_world->GetTowerGrid();
    auto& loaded_facility_mgr = new_world->GetFacilityManager();
    
    auto shop = loaded_facility_mgr.CreateFacility(
        BuildingComponent::Type::Shop, 0, 5, 2
    );
    
    EXPECT_TRUE(shop.is_valid());
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), 5);
    
    // Run simulation
    for (int i = 0; i < 50; ++i) {
        EXPECT_TRUE(new_world->Update(0.016f));
    }
    
    // State should be stable
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), 5);
}
