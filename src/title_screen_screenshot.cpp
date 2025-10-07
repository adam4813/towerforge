/**
 * @file title_screen_screenshot.cpp
 * @brief Screenshot generation tool for TowerForge Title Screen
 */

#include <iostream>
#include "rendering/renderer.h"
#include "ui/main_menu.h"
#include <raylib.h>

using namespace towerforge::ui;
using namespace towerforge::rendering;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Title Screen Screenshot Generator" << std::endl;
    
    // Create and initialize the renderer
    Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Title Screen");
    
    // Create main menu
    MainMenu main_menu;
    
    // Run for a few frames to let animations settle
    for (int i = 0; i < 60; ++i) {
        main_menu.Update(0.016f);
    }
    
    // Render one frame
    renderer.BeginFrame();
    main_menu.Render();
    renderer.EndFrame();
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("title_screen.png");
    std::cout << "Screenshot saved to title_screen.png" << std::endl;
    
    renderer.Shutdown();
    
    return 0;
}
