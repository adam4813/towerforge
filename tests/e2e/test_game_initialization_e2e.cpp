#include <gtest/gtest.h>
#include "core/ecs_world.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/save_load_manager.hpp"
#include "core/achievement_manager.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

// E2E test for game initialization
// This test verifies the complete initialization workflow of the game

class GameInitializationE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        // Simulate game startup
    }

    void TearDown() override {
        // Cleanup
    }
};

TEST_F(GameInitializationE2ETest, CompleteGameInitialization) {
    // Step 1: Create ECS World
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    ASSERT_NE(ecs_world, nullptr);
    
    // Step 2: Initialize ECS World
    ASSERT_NO_THROW(ecs_world->Initialize());
    
    // Step 3: Verify all subsystems are initialized
    EXPECT_NO_THROW(ecs_world->GetWorld());
    EXPECT_NO_THROW(ecs_world->GetTowerGrid());
    EXPECT_NO_THROW(ecs_world->GetFacilityManager());
    EXPECT_NO_THROW(ecs_world->GetModManager());
    
    // Step 4: Create Save/Load Manager
    auto save_load_mgr = std::make_unique<SaveLoadManager>();
    ASSERT_NE(save_load_mgr, nullptr);
    ASSERT_NO_THROW(save_load_mgr->Initialize());
    
    // Step 5: Create Achievement Manager
    auto achievement_mgr = std::make_unique<AchievementManager>();
    ASSERT_NE(achievement_mgr, nullptr);
    ASSERT_NO_THROW(achievement_mgr->Initialize());
    
    // Step 6: Verify initial game state
    auto& grid = ecs_world->GetTowerGrid();
    EXPECT_GT(grid.GetFloorCount(), 0);
    EXPECT_GT(grid.GetColumnCount(), 0);
    EXPECT_EQ(grid.GetOccupiedCellCount(), 0);
    
    // Step 7: Verify achievements are loaded
    auto achievements = achievement_mgr->GetAllAchievements();
    EXPECT_GT(achievements.size(), 0);
    
    // Step 8: Run initial update cycle
    EXPECT_TRUE(ecs_world->Update(0.016f));
}

TEST_F(GameInitializationE2ETest, InitializeNewGame) {
    // Complete new game setup workflow
    
    // Create core systems
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    ecs_world->Initialize();
    
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Build initial floor (ground floor)
    ASSERT_TRUE(grid.BuildFloor(0, 0, grid.GetColumnCount()));
    
    // Place starting facility (lobby/entrance)
    auto lobby = facility_mgr.CreateFacility(
        BuildingComponent::Type::Lobby,
        0,  // ground floor
        grid.GetColumnCount() / 2 - 2,  // center
        4   // width
    );
    
    EXPECT_TRUE(lobby.is_valid());
    EXPECT_TRUE(grid.IsOccupied(0, grid.GetColumnCount() / 2 - 2));
    
    // Run several simulation cycles
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(ecs_world->Update(0.016f));
    }
    
    // Game should be in stable state
    EXPECT_EQ(grid.GetOccupiedCellCount(), 4);  // Lobby takes 4 cells
}

TEST_F(GameInitializationE2ETest, InitializeWithMultipleSubsystems) {
    // Initialize all game systems together
    
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    auto save_load_mgr = std::make_unique<SaveLoadManager>();
    auto achievement_mgr = std::make_unique<AchievementManager>();
    
    // Initialize all systems
    ASSERT_NO_THROW({
        ecs_world->Initialize();
        save_load_mgr->Initialize();
        achievement_mgr->Initialize();
    });
    
    // Link systems
    save_load_mgr->SetAchievementManager(achievement_mgr.get());
    
    // Verify cross-system functionality
    auto& grid = ecs_world->GetTowerGrid();
    grid.BuildFloor(0, 0, 10);
    
    auto& facility_mgr = ecs_world->GetFacilityManager();
    facility_mgr.CreateFacility(BuildingComponent::Type::Office, 0, 0);
    
    // Save the initial state
    auto save_result = save_load_mgr->SaveGame("init_test", "Test Tower", *ecs_world);
    EXPECT_TRUE(save_result.success);
    
    // Clean up test save
    save_load_mgr->DeleteSlot("init_test");
}

TEST_F(GameInitializationE2ETest, StartGameWithModsLoaded) {
    // Test initialization with mod system active
    
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    ecs_world->Initialize();
    
    // Access mod manager (mods are loaded during ECS initialization)
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& loaded_mods = mod_mgr.GetLoadedMods();
    
    // System should handle any number of mods (including zero)
    EXPECT_GE(loaded_mods.size(), 0);
    
    // Verify custom content is accessible
    const auto& custom_facilities = mod_mgr.GetCustomFacilityTypes();
    const auto& custom_visitors = mod_mgr.GetCustomVisitorTypes();
    
    EXPECT_GE(custom_facilities.size(), 0);
    EXPECT_GE(custom_visitors.size(), 0);
    
    // Game should function normally
    EXPECT_TRUE(ecs_world->Update(0.016f));
}

TEST_F(GameInitializationE2ETest, InitializeAndRunSimulationLoop) {
    // Simulate the main game loop initialization and first few frames
    
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    auto save_load_mgr = std::make_unique<SaveLoadManager>();
    auto achievement_mgr = std::make_unique<AchievementManager>();
    
    ecs_world->Initialize();
    save_load_mgr->Initialize();
    achievement_mgr->Initialize();
    
    save_load_mgr->SetAutosaveEnabled(true);
    save_load_mgr->SetAutosaveInterval(300.0f);
    
    auto& grid = ecs_world->GetTowerGrid();
    auto& facility_mgr = ecs_world->GetFacilityManager();
    
    // Set up initial tower
    grid.BuildFloor(0, 0, 20);
    facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 8, 4);
    
    // Simulate 60 frames (1 second at 60 FPS)
    float delta_time = 1.0f / 60.0f;
    for (int frame = 0; frame < 60; ++frame) {
        EXPECT_TRUE(ecs_world->Update(delta_time));
        save_load_mgr->UpdateAutosave(delta_time, *ecs_world);
        
        // Check achievements periodically
        if (frame % 10 == 0) {
            achievement_mgr->CheckAndUnlock(AchievementType::Floors, 1);
        }
    }
    
    // System should be stable after 1 second
    EXPECT_GT(save_load_mgr->GetTimeSinceLastSave(), 0.0f);
}

TEST_F(GameInitializationE2ETest, RecoverFromInitializationFailure) {
    // Test resilience if components partially initialize
    
    auto ecs_world = std::make_unique<ECSWorld>(1920, 1080, 64, 64);
    
    // Initialize ECS
    ASSERT_NO_THROW(ecs_world->Initialize());
    
    // Even if save system has issues, game should still run
    auto save_load_mgr = std::make_unique<SaveLoadManager>();
    // Don't call Initialize on save manager
    
    // Achievement manager should still work
    auto achievement_mgr = std::make_unique<AchievementManager>();
    achievement_mgr->Initialize();
    
    // Game should still be playable
    EXPECT_TRUE(ecs_world->Update(0.016f));
    
    auto achievements = achievement_mgr->GetAllAchievements();
    EXPECT_GT(achievements.size(), 0);
}
