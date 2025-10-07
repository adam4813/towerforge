/**
 * @file achievements_screenshot.cpp
 * @brief Screenshot generator for the Achievements Menu
 * 
 * This tool generates a screenshot of the achievements menu with some
 * achievements unlocked for documentation purposes.
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/achievement_manager.hpp"
#include "ui/achievements_menu.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Achievements Menu Screenshot Generator" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Achievements");
    
    // Create achievement manager
    AchievementManager achievement_manager;
    achievement_manager.Initialize();
    
    // Unlock some achievements for demonstration
    achievement_manager.UnlockAchievement("pop_10");
    achievement_manager.UnlockAchievement("pop_50");
    achievement_manager.UnlockAchievement("income_10k");
    achievement_manager.UnlockAchievement("floors_5");
    achievement_manager.UnlockAchievement("satisfaction_80");
    
    // Create achievements menu
    AchievementsMenu achievements_menu;
    achievements_menu.SetAchievementManager(&achievement_manager);
    
    // Set current game stats (for progress display)
    achievements_menu.SetGameStats(75, 25000.0f, 8, 85.0f);
    
    // Render a few frames to ensure everything is drawn correctly
    for (int i = 0; i < 5; i++) {
        achievements_menu.Update(0.016f);
        
        renderer.BeginFrame();
        ClearBackground(Color{20, 20, 30, 255});
        achievements_menu.Render();
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("achievements_menu.png");
    std::cout << "Screenshot saved: achievements_menu.png" << std::endl;
    
    renderer.Shutdown();
    return 0;
}
