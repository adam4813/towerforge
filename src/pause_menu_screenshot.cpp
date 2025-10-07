#include <iostream>
#include "rendering/renderer.h"
#include "ui/pause_menu.h"
#include <raylib.h>

using namespace towerforge::ui;
using namespace towerforge::rendering;

int main() {
    std::cout << "Creating pause menu screenshot..." << std::endl;
    
    // Initialize renderer
    Renderer renderer;
    renderer.Initialize(800, 600, "Pause Menu Screenshot");
    
    // Create pause menu
    PauseMenu pause_menu;
    
    // Render a few frames to allow animations to start
    for (int i = 0; i < 30; i++) {
        renderer.BeginFrame();
        
        // Draw a simple game background (simulated)
        ClearBackground(DARKGRAY);
        
        // Draw some fake game elements (grid)
        for (int row = 0; row < 10; row++) {
            for (int col = 0; col < 15; col++) {
                int x = 300 + col * 30;
                int y = 100 + row * 40;
                DrawRectangleLines(x, y, 30, 40, ColorAlpha(WHITE, 0.2f));
                if ((row + col) % 3 == 0) {
                    DrawRectangle(x + 2, y + 2, 26, 36, ColorAlpha(SKYBLUE, 0.5f));
                }
            }
        }
        
        // Draw HUD elements
        DrawRectangle(0, 0, 800, 40, ColorAlpha(BLACK, 0.7f));
        DrawText("Funds: $25,000", 10, 10, 20, WHITE);
        DrawText("Population: 42", 200, 10, 20, WHITE);
        DrawText("Day 5 - 14:30", 400, 10, 20, WHITE);
        
        // Render pause menu overlay
        pause_menu.Update(1.0f / 60.0f);
        pause_menu.Render();
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    TakeScreenshot("pause_menu.png");
    std::cout << "Screenshot saved to pause_menu.png" << std::endl;
    
    // Also create one with quit confirmation showing
    pause_menu.ShowQuitConfirmation(true);
    
    for (int i = 0; i < 10; i++) {
        renderer.BeginFrame();
        
        // Draw game background again
        ClearBackground(DARKGRAY);
        for (int row = 0; row < 10; row++) {
            for (int col = 0; col < 15; col++) {
                int x = 300 + col * 30;
                int y = 100 + row * 40;
                DrawRectangleLines(x, y, 30, 40, ColorAlpha(WHITE, 0.2f));
                if ((row + col) % 3 == 0) {
                    DrawRectangle(x + 2, y + 2, 26, 36, ColorAlpha(SKYBLUE, 0.5f));
                }
            }
        }
        
        DrawRectangle(0, 0, 800, 40, ColorAlpha(BLACK, 0.7f));
        DrawText("Funds: $25,000", 10, 10, 20, WHITE);
        DrawText("Population: 42", 200, 10, 20, WHITE);
        DrawText("Day 5 - 14:30", 400, 10, 20, WHITE);
        
        pause_menu.Update(1.0f / 60.0f);
        pause_menu.Render();
        
        renderer.EndFrame();
    }
    
    TakeScreenshot("pause_menu_quit_confirmation.png");
    std::cout << "Screenshot saved to pause_menu_quit_confirmation.png" << std::endl;
    
    renderer.Shutdown();
    return 0;
}
