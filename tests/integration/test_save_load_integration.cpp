#include <gtest/gtest.h>
#include "core/save_load_manager.hpp"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include <filesystem>
#include <fstream>

using namespace TowerForge::Core;

// Integration tests for SaveLoadManager
// These tests verify save/load functionality with ECSWorld and file system

class SaveLoadManagerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        save_mgr = std::make_unique<SaveLoadManager>();
        save_mgr->Initialize();
        
        ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
        ecs_world->Initialize();
        
        // Clean up test saves from previous runs
        CleanupTestSaves();
    }

    void TearDown() override {
        CleanupTestSaves();
    }

    void CleanupTestSaves() {
        std::vector<std::string> test_slots = {
            "test_slot_1", "test_slot_2", "test_slot_3", 
            "test_rename", "test_rename_new", "test_autosave"
        };
        
        for (const auto& slot : test_slots) {
            if (save_mgr && save_mgr->SlotExists(slot)) {
                save_mgr->DeleteSlot(slot);
            }
        }
    }

    std::unique_ptr<SaveLoadManager> save_mgr;
    std::unique_ptr<ECSWorld> ecs_world;
};

TEST_F(SaveLoadManagerIntegrationTest, Initialization) {
    EXPECT_NO_THROW(save_mgr->Initialize());
}

TEST_F(SaveLoadManagerIntegrationTest, SaveBasicGameState) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Set up initial game state
    grid.BuildFloor(0, 0, 10);
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    
    // Save game
    auto result = save_mgr->SaveGame("test_slot_1", "Test Tower", *ecs_world);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.error, SaveLoadError::None);
    EXPECT_TRUE(save_mgr->SlotExists("test_slot_1"));
}

TEST_F(SaveLoadManagerIntegrationTest, LoadBasicGameState) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Create and save initial state
    grid.BuildFloor(0, 0, 10);
    auto facility = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 2, 3);
    
    auto save_result = save_mgr->SaveGame("test_slot_2", "Test Tower", *ecs_world);
    ASSERT_TRUE(save_result.success);
    
    // Create fresh ECS world
    auto new_ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_ecs_world->Initialize();
    
    // Load game
    auto load_result = save_mgr->LoadGame("test_slot_2", *new_ecs_world);
    
    EXPECT_TRUE(load_result.success);
    EXPECT_EQ(load_result.error, SaveLoadError::None);
}

TEST_F(SaveLoadManagerIntegrationTest, GetSaveSlots) {
    // Create multiple save slots
    save_mgr->SaveGame("test_slot_1", "Tower 1", *ecs_world);
    save_mgr->SaveGame("test_slot_2", "Tower 2", *ecs_world);
    
    auto slots = save_mgr->GetSaveSlots();
    
    // Should have at least our test slots
    EXPECT_GE(slots.size(), 2);
    
    // Check that our slots are present
    bool found_slot1 = false;
    bool found_slot2 = false;
    
    for (const auto& slot : slots) {
        if (slot.slot_name == "test_slot_1") found_slot1 = true;
        if (slot.slot_name == "test_slot_2") found_slot2 = true;
    }
    
    EXPECT_TRUE(found_slot1);
    EXPECT_TRUE(found_slot2);
}

TEST_F(SaveLoadManagerIntegrationTest, DeleteSlot) {
    save_mgr->SaveGame("test_slot_3", "Test Tower", *ecs_world);
    ASSERT_TRUE(save_mgr->SlotExists("test_slot_3"));
    
    auto result = save_mgr->DeleteSlot("test_slot_3");
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(save_mgr->SlotExists("test_slot_3"));
}

TEST_F(SaveLoadManagerIntegrationTest, RenameSlot) {
    save_mgr->SaveGame("test_rename", "Test Tower", *ecs_world);
    ASSERT_TRUE(save_mgr->SlotExists("test_rename"));
    
    auto result = save_mgr->RenameSlot("test_rename", "test_rename_new");
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(save_mgr->SlotExists("test_rename"));
    EXPECT_TRUE(save_mgr->SlotExists("test_rename_new"));
}

TEST_F(SaveLoadManagerIntegrationTest, ValidSlotNames) {
    EXPECT_TRUE(SaveLoadManager::IsValidSlotName("valid_slot"));
    EXPECT_TRUE(SaveLoadManager::IsValidSlotName("slot123"));
    EXPECT_TRUE(SaveLoadManager::IsValidSlotName("my-save"));
    
    EXPECT_FALSE(SaveLoadManager::IsValidSlotName(""));
    EXPECT_FALSE(SaveLoadManager::IsValidSlotName("slot/with/slash"));
    EXPECT_FALSE(SaveLoadManager::IsValidSlotName("slot\\with\\backslash"));
}

TEST_F(SaveLoadManagerIntegrationTest, LoadNonexistentSlot) {
    auto result = save_mgr->LoadGame("nonexistent_slot", *ecs_world);
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, SaveLoadError::FileNotFound);
}

TEST_F(SaveLoadManagerIntegrationTest, AutosaveConfiguration) {
    // Check default state
    EXPECT_FALSE(save_mgr->IsAutosaveEnabled());
    
    // Enable autosave
    save_mgr->SetAutosaveEnabled(true);
    EXPECT_TRUE(save_mgr->IsAutosaveEnabled());
    
    // Set interval
    save_mgr->SetAutosaveInterval(300.0f);  // 5 minutes
    
    // Disable autosave
    save_mgr->SetAutosaveEnabled(false);
    EXPECT_FALSE(save_mgr->IsAutosaveEnabled());
}

TEST_F(SaveLoadManagerIntegrationTest, ManualAutosave) {
    auto& grid = ecs_world->GetTowerGrid();
    grid.BuildFloor(0, 0, 10);
    
    auto result = save_mgr->Autosave(*ecs_world);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(save_mgr->SlotExists("_autosave"));
}

TEST_F(SaveLoadManagerIntegrationTest, TimeSinceLastSave) {
    float initial_time = save_mgr->GetTimeSinceLastSave();
    
    save_mgr->SaveGame("test_slot_1", "Test", *ecs_world);
    
    float time_after_save = save_mgr->GetTimeSinceLastSave();
    
    // Time should reset after save
    EXPECT_LE(time_after_save, initial_time);
}

TEST_F(SaveLoadManagerIntegrationTest, SaveAndLoadComplexState) {
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Create complex state
    grid.AddFloors(2);
    grid.AddBasementFloor();
    
    for (int i = -1; i < 7; ++i) {
        grid.BuildFloor(i, 0, 10);
    }
    
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0, 3);
    facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 0, 5, 4);
    facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 1, 2, 2);
    facility_mgr.CreateFacility(BuildingComponent::Type::Residential, -1, 3, 2);
    
    int initial_occupancy = grid.GetOccupiedCellCount();
    
    // Save
    auto save_result = save_mgr->SaveGame("test_complex", "Complex Tower", *ecs_world);
    ASSERT_TRUE(save_result.success);
    
    // Create new world and load
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("test_complex", *new_world);
    EXPECT_TRUE(load_result.success);
    
    // Verify loaded state
    auto& loaded_grid = new_world->GetTowerGrid();
    EXPECT_EQ(loaded_grid.GetOccupiedCellCount(), initial_occupancy);
    
    // Clean up
    save_mgr->DeleteSlot("test_complex");
}

TEST_F(SaveLoadManagerIntegrationTest, SaveSlotMetadata) {
    save_mgr->SaveGame("test_metadata", "My Tower", *ecs_world);
    
    auto slots = save_mgr->GetSaveSlots();
    
    bool found = false;
    for (const auto& slot : slots) {
        if (slot.slot_name == "test_metadata") {
            found = true;
            EXPECT_EQ(slot.tower_name, "My Tower");
            EXPECT_FALSE(slot.save_date.empty());
            EXPECT_FALSE(slot.game_version.empty());
            break;
        }
    }
    
    EXPECT_TRUE(found);
}

TEST_F(SaveLoadManagerIntegrationTest, MultipleConsecutiveSaves) {
    auto& facility_mgr = ecs_world->GetFacilityManager();
    auto& grid = ecs_world->GetTowerGrid();
    
    grid.BuildFloor(0, 0, 10);
    
    // First save
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    auto result1 = save_mgr->SaveGame("test_slot_1", "Tower v1", *ecs_world);
    EXPECT_TRUE(result1.success);
    
    // Second save (overwrite)
    facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 0, 5);
    auto result2 = save_mgr->SaveGame("test_slot_1", "Tower v2", *ecs_world);
    EXPECT_TRUE(result2.success);
    
    // Load should get the latest version
    auto new_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    new_world->Initialize();
    
    auto load_result = save_mgr->LoadGame("test_slot_1", *new_world);
    EXPECT_TRUE(load_result.success);
}
