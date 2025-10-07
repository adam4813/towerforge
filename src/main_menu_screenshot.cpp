/**
 * @file main_menu_screenshot.cpp
 * @brief Screenshot generator for the Main Menu with Achievements option
 */

#include <iostream>
#include "rendering/renderer.h"
#include "ui/main_menu.h"
#include <raylib.h>

using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Main Menu Screenshot Generator" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Main Menu");
    
    // Create main menu
    MainMenu main_menu;
    
    // Render a few frames to ensure everything is drawn correctly
    for (int i = 0; i < 5; i++) {
        main_menu.Update(0.016f);
        
        renderer.BeginFrame();
        main_menu.Render();
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("main_menu_with_achievements.png");
    std::cout << "Screenshot saved: main_menu_with_achievements.png" << std::endl;
    
    renderer.Shutdown();
    return 0;
}
