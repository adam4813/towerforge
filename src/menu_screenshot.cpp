/**
 * @file menu_screenshot.cpp
 * @brief Screenshot generator for main menu
 */

#include <iostream>
#include "rendering/renderer.h"
#include "ui/main_menu.h"
#include <raylib.h>

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Menu Screenshot Generator" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Main Menu");
    
    // Create main menu
    towerforge::ui::MainMenu main_menu;
    
    // Render a few frames
    for (int i = 0; i < 30; i++) {
        main_menu.Update(GetFrameTime());
        
        renderer.BeginFrame();
        main_menu.Render();
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("main_menu_with_tutorial.png");
    std::cout << "Screenshot saved to main_menu_with_tutorial.png" << std::endl;
    
    // Cleanup
    renderer.Shutdown();
    
    return 0;
}
