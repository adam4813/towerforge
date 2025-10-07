/**
 * @file credits_screen_screenshot.cpp
 * @brief Screenshot generation tool for TowerForge Credits Screen
 */

#include <iostream>
#include "rendering/renderer.h"
#include <raylib.h>

using namespace towerforge::rendering;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Credits Screen Screenshot Generator" << std::endl;
    
    // Create and initialize the renderer
    Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Credits");
    
    // Render credits screen
    renderer.BeginFrame();
    ClearBackground(Color{20, 20, 30, 255});
    
    int screen_width = GetScreenWidth();
    int y = 100;
    
    DrawText("CREDITS", (screen_width - MeasureText("CREDITS", 40)) / 2, y, 40, GOLD);
    y += 80;
    
    DrawText("TowerForge v0.1.0", (screen_width - MeasureText("TowerForge v0.1.0", 24)) / 2, y, 24, WHITE);
    y += 50;
    
    DrawText("A modern SimTower-inspired skyscraper simulation", 
             (screen_width - MeasureText("A modern SimTower-inspired skyscraper simulation", 18)) / 2, 
             y, 18, LIGHTGRAY);
    y += 60;
    
    DrawText("Built with:", (screen_width - MeasureText("Built with:", 20)) / 2, y, 20, LIGHTGRAY);
    y += 40;
    DrawText("- C++20", (screen_width - MeasureText("- C++20", 18)) / 2, y, 18, WHITE);
    y += 30;
    DrawText("- Raylib (rendering)", (screen_width - MeasureText("- Raylib (rendering)", 18)) / 2, y, 18, WHITE);
    y += 30;
    DrawText("- Flecs (ECS framework)", (screen_width - MeasureText("- Flecs (ECS framework)", 18)) / 2, y, 18, WHITE);
    y += 60;
    
    DrawText("Press ESC or ENTER to return to menu", 
             (screen_width - MeasureText("Press ESC or ENTER to return to menu", 16)) / 2, 
             y, 16, GRAY);
    
    renderer.EndFrame();
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("credits_screen.png");
    std::cout << "Screenshot saved to credits_screen.png" << std::endl;
    
    renderer.Shutdown();
    
    return 0;
}
