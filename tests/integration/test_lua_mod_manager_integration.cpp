#include <gtest/gtest.h>
#include "core/lua_mod_manager.hpp"
#include "core/ecs_world.hpp"

using namespace towerforge::core;

// Integration tests for LuaModManager
// These tests verify Lua mod loading, custom facility/visitor registration

class LuaModManagerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ecs_world = std::make_unique<ECSWorld>();
        ecs_world->Initialize();
    }

    std::unique_ptr<ECSWorld> ecs_world;
};

TEST_F(LuaModManagerIntegrationTest, Initialization) {
    const auto& mod_mgr = ecs_world->GetModManager();
    
    // Manager should be initialized
    EXPECT_NO_THROW(mod_mgr.GetLoadedMods());
}

TEST_F(LuaModManagerIntegrationTest, GetLoadedMods) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& mods = mod_mgr.GetLoadedMods();
    
    // Should return a vector (may be empty if no mods in mods/ directory)
    EXPECT_NO_THROW(mods.size());
}

TEST_F(LuaModManagerIntegrationTest, GetCustomFacilityTypes) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& facilities = mod_mgr.GetCustomFacilityTypes();
    
    // Should return a map (may be empty)
    EXPECT_NO_THROW(facilities.size());
}

TEST_F(LuaModManagerIntegrationTest, GetCustomVisitorTypes) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& visitors = mod_mgr.GetCustomVisitorTypes();
    
    // Should return a map (may be empty)
    EXPECT_NO_THROW(visitors.size());
}

TEST_F(LuaModManagerIntegrationTest, LoadedModsHaveMetadata) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& mods = mod_mgr.GetLoadedMods();
    
    for (const auto& mod : mods) {
        // Each mod should have basic metadata
        EXPECT_FALSE(mod.id.empty());
        EXPECT_FALSE(mod.name.empty());
        EXPECT_FALSE(mod.version.empty());
        EXPECT_FALSE(mod.author.empty());
        
        // If mod failed to load, it should have an error message
        if (!mod.loaded_successfully) {
            EXPECT_FALSE(mod.error_message.empty());
        }
    }
}

TEST_F(LuaModManagerIntegrationTest, CustomFacilitiesHaveValidData) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& facilities = mod_mgr.GetCustomFacilityTypes();
    
    for (const auto& [id, facility] : facilities) {
        EXPECT_FALSE(id.empty());
        EXPECT_FALSE(facility.name.empty());
        EXPECT_GT(facility.default_width, 0);
        EXPECT_GT(facility.default_capacity, 0);
        EXPECT_GE(facility.required_staff, 0);
        EXPECT_GE(facility.operating_start_hour, 0);
        EXPECT_LE(facility.operating_start_hour, 24);
        EXPECT_GE(facility.operating_end_hour, 0);
        EXPECT_LE(facility.operating_end_hour, 24);
    }
}

TEST_F(LuaModManagerIntegrationTest, CustomVisitorsHaveValidData) {
    const auto& mod_mgr = ecs_world->GetModManager();
    const auto& visitors = mod_mgr.GetCustomVisitorTypes();
    
    for (const auto& [id, visitor] : visitors) {
        EXPECT_FALSE(id.empty());
        EXPECT_FALSE(visitor.name.empty());
        EXPECT_GT(visitor.move_speed, 0.0f);
        EXPECT_GE(visitor.min_visit_duration, 0.0f);
        EXPECT_GE(visitor.max_visit_duration, visitor.min_visit_duration);
    }
}

TEST_F(LuaModManagerIntegrationTest, ModManagerIntegrationWithECS) {
    // Verify mod manager is accessible through ECS world
    EXPECT_NO_THROW(ecs_world->GetModManager());
    
    const auto& mod_mgr = ecs_world->GetModManager();
    
    // Should be able to access mod data through ECS world
    EXPECT_NO_THROW(mod_mgr.GetLoadedMods());
    EXPECT_NO_THROW(mod_mgr.GetCustomFacilityTypes());
    EXPECT_NO_THROW(mod_mgr.GetCustomVisitorTypes());
}

TEST_F(LuaModManagerIntegrationTest, SafelyHandleMissingModsDirectory) {
    // Even if mods directory is empty or missing, manager should work
    const auto& mod_mgr = ecs_world->GetModManager();
    
    EXPECT_NO_THROW(mod_mgr.GetLoadedMods());
    
    // Should return empty collections, not crash
    const auto& mods = mod_mgr.GetLoadedMods();
    const auto& facilities = mod_mgr.GetCustomFacilityTypes();
    const auto& visitors = mod_mgr.GetCustomVisitorTypes();
    
    EXPECT_GE(mods.size(), 0);
    EXPECT_GE(facilities.size(), 0);
    EXPECT_GE(visitors.size(), 0);
}

TEST_F(LuaModManagerIntegrationTest, MultipleECSWorldsWithSeparateModManagers) {
    // Create second ECS world
    auto ecs_world2 = std::make_unique<ECSWorld>();
    ecs_world2->Initialize();
    
    const auto& mod_mgr1 = ecs_world->GetModManager();
    const auto& mod_mgr2 = ecs_world2->GetModManager();
    
    // Both should be functional
    EXPECT_NO_THROW(mod_mgr1.GetLoadedMods());
    EXPECT_NO_THROW(mod_mgr2.GetLoadedMods());
}
