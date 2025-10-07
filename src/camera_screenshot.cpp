/*
 * Camera Test Screenshot Application
 * 
 * This application demonstrates the camera system with visual elements
 * and takes a screenshot to show the camera controls overlay.
 */

#include <iostream>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::rendering;
using namespace towerforge::ui;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Camera System Screenshot Test" << std::endl;
    
    // Create and initialize the renderer
    Renderer renderer;
    renderer.Initialize(1024, 768, "TowerForge - Camera System Demo");
    
    // Create and initialize camera
    towerforge::rendering::Camera camera;
    camera.Initialize(1024, 768, 1200.0f, 800.0f);
    
    // Create HUD for game state display
    HUD hud;
    GameState game_state;
    game_state.funds = 25000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 15;
    game_state.current_day = 1;
    game_state.current_time = 14.5f;
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    hud.SetGameState(game_state);
    
    // Add notification about camera controls
    hud.AddNotification(Notification::Type::Info, "Camera controls active - Pan, Zoom, Follow", 100.0f);
    
    // Simulate following an entity
    camera.FollowEntity(400.0f, 400.0f, 42);
    
    // Render a few frames to let everything settle
    for (int i = 0; i < 5; i++) {
        camera.Update(1.0f / 60.0f);
        
        renderer.BeginFrame();
        renderer.Clear(Color{40, 40, 60, 255});  // Dark blue-gray background
        
        // Begin camera mode
        camera.BeginMode();
        
        // Draw grid background
        const int grid_size = 50;
        for (int x = 0; x < 1200; x += grid_size) {
            DrawLine(x, 0, x, 800, Color{60, 60, 80, 255});
        }
        for (int y = 0; y < 800; y += grid_size) {
            DrawLine(0, y, 1200, y, Color{60, 60, 80, 255});
        }
        
        // Draw tower structure (floors)
        for (int floor = 0; floor < 8; floor++) {
            int y = 100 + floor * 80;
            DrawRectangle(100, y, 1000, 70, Color{80, 80, 100, 255});
            DrawRectangleLines(100, y, 1000, 70, LIGHTGRAY);
            
            // Floor label
            const char* floor_text = TextFormat("Floor %d", floor + 1);
            DrawText(floor_text, 110, y + 25, 20, WHITE);
        }
        
        // Draw some facilities
        // Office
        DrawRectangle(200, 180, 300, 60, Color{100, 150, 200, 255});
        DrawRectangleLines(200, 180, 300, 60, WHITE);
        DrawText("OFFICE", 220, 200, 18, WHITE);
        DrawText("8/10 occupied", 330, 205, 12, LIGHTGRAY);
        
        // Restaurant
        DrawRectangle(600, 260, 250, 60, Color{200, 150, 100, 255});
        DrawRectangleLines(600, 260, 250, 60, WHITE);
        DrawText("RESTAURANT", 620, 280, 18, WHITE);
        DrawText("15/20 occupied", 710, 285, 12, LIGHTGRAY);
        
        // Shop
        DrawRectangle(300, 340, 200, 60, Color{150, 200, 150, 255});
        DrawRectangleLines(300, 340, 200, 60, WHITE);
        DrawText("SHOP", 350, 360, 18, WHITE);
        
        // Draw some people (circles)
        DrawCircle(400, 400, 20.0f, RED);  // Followed entity
        DrawCircle(250, 300, 15.0f, YELLOW);
        DrawCircle(700, 450, 15.0f, GREEN);
        DrawCircle(350, 200, 15.0f, BLUE);
        DrawCircle(800, 350, 15.0f, ORANGE);
        
        // Draw arrow pointing to followed entity
        DrawText("You are here!", 430, 390, 16, WHITE);
        DrawTriangle(
            Vector2{425, 400},
            Vector2{410, 390},
            Vector2{410, 410},
            WHITE
        );
        
        // End camera mode
        camera.EndMode();
        
        // Render HUD
        hud.Render();
        
        // Render camera overlays
        camera.RenderControlsOverlay();
        camera.RenderFollowIndicator();
        
        // Add instruction text
        DrawRectangle(10, 680, 500, 80, Fade(BLACK, 0.8f));
        DrawRectangleLines(10, 680, 500, 80, YELLOW);
        DrawText("CAMERA SYSTEM DEMONSTRATION", 20, 690, 16, YELLOW);
        DrawText("Pan: Arrow keys or click+drag", 20, 710, 12, LIGHTGRAY);
        DrawText("Zoom: Mouse wheel or +/- keys", 20, 725, 12, LIGHTGRAY);
        DrawText("Reset: Home key | Follow: F key", 20, 740, 12, LIGHTGRAY);
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("towerforge_camera_screenshot.png");
    std::cout << "Screenshot saved to towerforge_camera_screenshot.png" << std::endl;
    
    // Cleanup
    renderer.Shutdown();
    
    return 0;
}
