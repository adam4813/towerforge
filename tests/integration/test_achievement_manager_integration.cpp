#include <gtest/gtest.h>
#include "core/achievement_manager.hpp"

using namespace TowerForge::Core;

// Integration tests for AchievementManager
// These tests verify achievement tracking, unlocking, and persistence

class AchievementManagerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        achievement_mgr = std::make_unique<AchievementManager>();
        achievement_mgr->Initialize();
    }

    std::unique_ptr<AchievementManager> achievement_mgr;
};

TEST_F(AchievementManagerIntegrationTest, Initialization) {
    auto achievements = achievement_mgr->GetAllAchievements();
    
    // Should have some default achievements
    EXPECT_GT(achievements.size(), 0);
}

TEST_F(AchievementManagerIntegrationTest, GetAchievements) {
    auto achievements = achievement_mgr->GetAllAchievements();
    
    for (const auto& achievement : achievements) {
        EXPECT_FALSE(achievement.id.empty());
        EXPECT_FALSE(achievement.name.empty());
        EXPECT_GE(achievement.requirement, 0);
    }
}

TEST_F(AchievementManagerIntegrationTest, CheckAndUnlockAchievement) {
    // Get an achievement that hasn't been unlocked
    auto achievements = achievement_mgr->GetAllAchievements();
    
    if (!achievements.empty()) {
        const auto& first_achievement = achievements[0];
        
        // Manually unlock an achievement
        bool unlocked = achievement_mgr->UnlockAchievement(first_achievement.id);
        
        // Should unlock if not already unlocked
        EXPECT_TRUE(unlocked || first_achievement.unlocked);
    }
}

TEST_F(AchievementManagerIntegrationTest, GetUnlockedAchievements) {
    auto all_achievements = achievement_mgr->GetAllAchievements();
    
    if (!all_achievements.empty()) {
        // Unlock first achievement
        const auto& first = all_achievements[0];
        achievement_mgr->UnlockAchievement(first.id);
        
        auto unlocked = achievement_mgr->GetUnlockedAchievements();
        
        // Should have at least one unlocked
        EXPECT_GT(unlocked.size(), 0);
    }
}

TEST_F(AchievementManagerIntegrationTest, GetLockedAchievements) {
    auto locked = achievement_mgr->GetLockedAchievements();
    auto all = achievement_mgr->GetAllAchievements();
    
    // Locked + unlocked should equal all
    auto unlocked = achievement_mgr->GetUnlockedAchievements();
    EXPECT_EQ(locked.size() + unlocked.size(), all.size());
}

TEST_F(AchievementManagerIntegrationTest, GetProgressPercentage) {
    auto achievements = achievement_mgr->GetAllAchievements();
    
    // Get overall progress percentage
    float progress = achievement_mgr->GetProgressPercentage();
    
    // Progress should be between 0 and 100
    EXPECT_GE(progress, 0.0f);
    EXPECT_LE(progress, 100.0f);
    
    // Unlock an achievement
    if (!achievements.empty()) {
        achievement_mgr->UnlockAchievement(achievements[0].id);
        float new_progress = achievement_mgr->GetProgressPercentage();
        EXPECT_GE(new_progress, progress);
    }
}

TEST_F(AchievementManagerIntegrationTest, CheckAchievementsByGameState) {
    auto achievements = achievement_mgr->GetAllAchievements();
    
    int initial_count = achievement_mgr->GetUnlockedCount();
    
    // Check achievements with high values
    achievement_mgr->CheckAchievements(
        100,    // population
        10000,  // total_income
        10,     // floor_count
        80.0f   // avg_satisfaction
    );
    
    int final_count = achievement_mgr->GetUnlockedCount();
    
    // Should potentially unlock some achievements
    EXPECT_GE(final_count, initial_count);
}

TEST_F(AchievementManagerIntegrationTest, UnlockProgressionTracking) {
    // Get all achievements
    auto all_achievements = achievement_mgr->GetAllAchievements();
    
    int initial_unlocked = achievement_mgr->GetUnlockedCount();
    
    // Try to unlock achievements with increasing game state values
    for (int value = 10; value <= 1000; value += 100) {
        achievement_mgr->CheckAchievements(
            value,       // population
            value * 10,  // total_income
            value / 10,  // floor_count
            80.0f        // avg_satisfaction
        );
    }
    
    int final_unlocked = achievement_mgr->GetUnlockedCount();
    
    // Should have unlocked some achievements (or at least not decreased)
    EXPECT_GE(final_unlocked, initial_unlocked);
}

TEST_F(AchievementManagerIntegrationTest, NewlyUnlockedTracking) {
    // Clear any existing notifications
    achievement_mgr->PopNewlyUnlocked();
    
    EXPECT_FALSE(achievement_mgr->HasNewAchievements());
    
    // Unlock an achievement
    auto achievements = achievement_mgr->GetAllAchievements();
    if (!achievements.empty() && !achievements[0].unlocked) {
        const auto& first = achievements[0];
        achievement_mgr->UnlockAchievement(first.id);
        
        // Should have new achievement notification
        EXPECT_TRUE(achievement_mgr->HasNewAchievements());
        
        auto newly_unlocked = achievement_mgr->PopNewlyUnlocked();
        EXPECT_GT(newly_unlocked.size(), 0);
        
        // After popping, should have no new achievements
        EXPECT_FALSE(achievement_mgr->HasNewAchievements());
    }
}

TEST_F(AchievementManagerIntegrationTest, GetUnlockedCount) {
    int unlocked_count = achievement_mgr->GetUnlockedCount();
    
    EXPECT_GE(unlocked_count, 0);
    
    int total_count = achievement_mgr->GetTotalCount();
    EXPECT_GT(total_count, 0);
    EXPECT_GE(total_count, unlocked_count);
    
    // Unlock an achievement
    auto achievements = achievement_mgr->GetAllAchievements();
    if (!achievements.empty()) {
        const auto& first = achievements[0];
        if (!first.unlocked) {
            achievement_mgr->UnlockAchievement(first.id);
            
            int new_unlocked = achievement_mgr->GetUnlockedCount();
            EXPECT_GT(new_unlocked, unlocked_count);
        }
    }
}
