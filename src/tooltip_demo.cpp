#include <raylib.h>
#include "ui/tooltip.h"
#include "ui/build_menu.h"
#include "ui/hud.h"
#include <memory>
#include <sstream>

using namespace towerforge::ui;

int main() {
    // Initialize window
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "TowerForge - Tooltip System Demo");
    SetTargetFPS(60);
    
    // Create tooltip manager
    auto tooltip_manager = std::make_unique<TooltipManager>();
    
    // Create build menu
    BuildMenu build_menu;
    build_menu.SetTooltipManager(tooltip_manager.get());
    
    // Create HUD
    HUD hud;
    
    // Set up game state for HUD
    GameState game_state;
    game_state.funds = 25000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 125;
    game_state.current_day = 5;
    game_state.current_time = 8.5f;
    game_state.speed_multiplier = 1;
    game_state.paused = false;
    game_state.rating.stars = 3;
    game_state.rating.average_satisfaction = 75.0f;
    game_state.rating.total_tenants = 50;
    game_state.rating.total_floors = 10;
    game_state.rating.hourly_income = 500.0f;
    game_state.rating.next_star_tenants = 75;
    game_state.rating.next_star_satisfaction = 80.0f;
    
    hud.SetGameState(game_state);
    
    // Demo buttons
    struct Button {
        int x, y, width, height;
        const char* label;
        const char* tooltip;
    };
    
    Button demo_buttons[] = {
        {250, 150, 200, 50, "Build Facility", "Click to open build menu.\nHotkey: B"},
        {250, 220, 200, 50, "Demolish Mode", "Enter demolish mode to remove facilities.\nRefunds 50% of cost.\nHotkey: D"},
        {250, 290, 200, 50, "Undo (Ctrl+Z)", "Undo last placement or demolition.\nHotkey: Ctrl+Z"},
        {250, 360, 200, 50, "Redo (Ctrl+Y)", "Redo previously undone action.\nHotkey: Ctrl+Y"},
    };
    
    int hover_button = -1;
    float demo_time = 0.0f;
    
    // Main game loop
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        demo_time += delta_time;
        
        // Update HUD
        hud.Update(delta_time);
        
        // Get mouse position
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        
        // Update tooltip manager
        tooltip_manager->Update(mouse_x, mouse_y);
        
        // Update HUD tooltips
        hud.UpdateTooltips(mouse_x, mouse_y);
        
        // Update build menu tooltips
        build_menu.UpdateTooltips(mouse_x, mouse_y, game_state.funds);
        
        // Check demo buttons
        hover_button = -1;
        for (int i = 0; i < 4; i++) {
            auto& btn = demo_buttons[i];
            if (tooltip_manager->IsHovering(mouse_x, mouse_y, btn.x, btn.y, btn.width, btn.height)) {
                hover_button = i;
                Tooltip tooltip(btn.tooltip);
                tooltip_manager->ShowTooltip(tooltip, btn.x, btn.y, btn.width, btn.height);
                break;
            }
        }
        
        // Update game state periodically for demo
        if (static_cast<int>(demo_time) % 2 == 0) {
            game_state.funds += 10.0f * delta_time;
        }
        hud.SetGameState(game_state);
        
        // Rendering
        BeginDrawing();
        
        ClearBackground(Color{30, 30, 40, 255});
        
        // Draw title
        DrawText("TowerForge Tooltip System Demo", 20, 20, 32, GOLD);
        DrawText("Hover over UI elements to see tooltips", 20, 60, 16, LIGHTGRAY);
        
        // Draw demo buttons
        for (int i = 0; i < 4; i++) {
            auto& btn = demo_buttons[i];
            Color bg_color = (i == hover_button) ? ColorAlpha(GOLD, 0.3f) : ColorAlpha(DARKGRAY, 0.5f);
            DrawRectangle(btn.x, btn.y, btn.width, btn.height, bg_color);
            DrawRectangleLines(btn.x, btn.y, btn.width, btn.height, GOLD);
            
            int text_width = MeasureText(btn.label, 16);
            int text_x = btn.x + (btn.width - text_width) / 2;
            int text_y = btn.y + (btn.height - 16) / 2;
            DrawText(btn.label, text_x, text_y, 16, WHITE);
        }
        
        // Render HUD (includes top bar and speed controls with tooltips)
        hud.Render();
        
        // Render build menu
        build_menu.Render();
        
        // Render tooltips on top
        tooltip_manager->Render();
        
        // Instructions
        DrawText("Move mouse over:", 20, screenHeight - 120, 14, SKYBLUE);
        DrawText("- Top bar elements (funds, population, time)", 20, screenHeight - 100, 12, LIGHTGRAY);
        DrawText("- Build menu items (left side)", 20, screenHeight - 80, 12, LIGHTGRAY);
        DrawText("- Speed controls (bottom right)", 20, screenHeight - 60, 12, LIGHTGRAY);
        DrawText("- Demo buttons (center)", 20, screenHeight - 40, 12, LIGHTGRAY);
        DrawText("Press ESC to exit", 20, screenHeight - 20, 12, RED);
        
        EndDrawing();
    }
    
    CloseWindow();
    
    return 0;
}
