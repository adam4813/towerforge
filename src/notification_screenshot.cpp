#include <iostream>
#include "rendering/renderer.h"
#include "ui/hud.h"
#include "ui/notification_center.h"
#include <raylib.h>

using namespace towerforge::ui;

int main() {
    std::cout << "TowerForge - Notification Center Screenshot Generator" << std::endl;
    
    // Initialize renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(1280, 800, "Notification Center Screenshot");
    
    // Create HUD with notification center
    HUD hud;
    
    // Set up game state
    GameState game_state;
    game_state.funds = 25000.0f;
    game_state.income_rate = 500.0f;
    game_state.population = 125;
    game_state.current_day = 5;
    game_state.current_time = 14.5f;
    game_state.speed_multiplier = 2;
    game_state.paused = false;
    game_state.rating.stars = 3;
    game_state.rating.average_satisfaction = 75.0f;
    game_state.rating.total_tenants = 125;
    game_state.rating.total_floors = 15;
    game_state.rating.hourly_income = 500.0f;
    
    hud.SetGameState(game_state);
    
    // Get notification center and add sample notifications
    auto* nc = hud.GetNotificationCenter();
    
    // Add various types of notifications to demonstrate the system
    nc->AddNotification(
        "Achievement Unlocked",
        "First Tenants - Have 10 tenants in your tower",
        NotificationType::Achievement,
        NotificationPriority::High,
        -1.0f
    );
    
    nc->AddNotification(
        "Low Funds Warning",
        "Your funds are running low. Build income-generating facilities!",
        NotificationType::Warning,
        NotificationPriority::Medium,
        -1.0f
    );
    
    nc->AddNotification(
        "Facility Placed",
        "Office facility successfully placed on floor 5",
        NotificationType::Success,
        NotificationPriority::Low,
        -1.0f
    );
    
    nc->AddNotification(
        "Welcome to TowerForge",
        "Start building your tower empire! Press N to toggle notifications.",
        NotificationType::Info,
        NotificationPriority::Medium,
        -1.0f
    );
    
    nc->AddNotification(
        "Rush Hour Started",
        "Expect increased elevator traffic during peak hours",
        NotificationType::Event,
        NotificationPriority::Medium,
        -1.0f
    );
    
    nc->AddNotification(
        "Build Error",
        "Cannot place facility - insufficient funds",
        NotificationType::Error,
        NotificationPriority::Low,
        -1.0f
    );
    
    // Show the notification center
    nc->Show();
    
    // Update once to process
    hud.Update(0.016f);
    
    // Render a few frames to ensure everything is drawn
    for (int i = 0; i < 5; i++) {
        renderer.BeginFrame();
        renderer.Clear(Color{30, 30, 40, 255});
        hud.Render();
        renderer.EndFrame();
        
        // Take screenshot on the last frame
        if (i == 4) {
            TakeScreenshot("notification_center_screenshot.png");
            std::cout << "Screenshot saved to notification_center_screenshot.png" << std::endl;
        }
    }
    
    renderer.Shutdown();
    return 0;
}
