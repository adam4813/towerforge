#include <iostream>
#include "rendering/renderer.h"

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Tower Defense Game Engine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Initializing Raylib renderer..." << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - 2D Vector Rendering Demo");
    
    std::cout << "Renderer initialized. Window opened." << std::endl;
    std::cout << "Press ESC or close window to exit." << std::endl;
    
    // Main game loop
    while (!renderer.ShouldClose()) {
        renderer.BeginFrame();
        
        // Clear background to dark gray
        renderer.Clear(DARKGRAY);
        
        // Draw a test rectangle (representing a building floor)
        renderer.DrawRectangle(250, 200, 300, 80, SKYBLUE);
        
        // Draw a test circle (representing a person or elevator)
        renderer.DrawCircle(400, 400, 30.0f, RED);
        
        renderer.EndFrame();
    }
    
    // Cleanup
    renderer.Shutdown();
    std::cout << "Renderer shut down. Exiting." << std::endl;
    
    return 0;
}
