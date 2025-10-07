#include <iostream>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/research_tree_menu.h"
#include <raylib.h>

using namespace TowerForge::Core;
using namespace towerforge::ui;
using namespace towerforge::rendering;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Research Tree Demo" << std::endl;
    
    // Create and initialize the renderer
    Renderer renderer;
    renderer.Initialize(800, 600, "Research Tree Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create the global ResearchTree as a singleton
    ResearchTree research_tree;
    research_tree.InitializeDefaultTree();
    research_tree.AwardPoints(100);  // Give plenty of points for demo
    ecs_world.GetWorld().set<ResearchTree>(research_tree);
    
    // Create research menu
    ResearchTreeMenu research_menu;
    research_menu.SetVisible(true);  // Open by default for demo
    
    std::cout << "Rendering research tree menu..." << std::endl;
    
    // Render for a few frames to ensure everything is displayed
    for (int frame = 0; frame < 60; ++frame) {
        if (renderer.ShouldClose()) break;
        
        float delta_time = GetFrameTime();
        research_menu.Update(delta_time);
        
        // Handle mouse interaction
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ResearchTree& research_tree_ref = ecs_world.GetWorld().get_mut<ResearchTree>();
            research_menu.HandleMouse(GetMouseX(), GetMouseY(), true, research_tree_ref);
        }
        
        renderer.BeginFrame();
        
        // Draw dark background
        ClearBackground(Color{20, 20, 30, 255});
        
        // Draw title
        DrawText("RESEARCH TREE DEMO", 20, 20, 24, GOLD);
        DrawText("Click nodes to unlock upgrades", 20, 50, 14, LIGHTGRAY);
        
        // Render research menu
        ResearchTree& research_tree_ref = ecs_world.GetWorld().get_mut<ResearchTree>();
        research_menu.Render(research_tree_ref);
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("research_tree_demo.png");
    std::cout << "Screenshot saved as research_tree_demo.png" << std::endl;
    
    return 0;
}
