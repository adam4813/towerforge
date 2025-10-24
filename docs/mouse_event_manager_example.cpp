// Example: Using MouseEventManager for interactive game facilities
// This demonstrates how to use the MouseEventManager for non-UI game objects

#include "ui/mouse_event_manager.h"
#include <raylib.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

namespace example {

// Example facility class that needs mouse interaction
class GameFacility {
public:
    GameFacility(const int id, const Rectangle bounds, const std::string& name)
        : id_(id), bounds_(bounds), name_(name), event_handle_(-1) {}

    void RegisterWithEventManager(towerforge::ui::MouseEventManager& manager) {
        // Create hover callback
        auto hover_callback = [this](const towerforge::ui::MouseEvent& event) {
            std::cout << "Hovering over facility: " << name_ << std::endl;
            // Could show tooltip or highlight facility here
            return false; // Don't consume hover events
        };

        // Create click callback
        auto click_callback = [this](const towerforge::ui::MouseEvent& event) {
            if (event.left_pressed) {
                std::cout << "Clicked facility: " << name_ << std::endl;
                // Could show facility details window
                return true; // Consume click event
            }
            return false;
        };

        // Register region with higher priority for important facilities
        int priority = (name_ == "Flagship") ? 20 : 10;
        
        towerforge::ui::MouseEventRegion region(
            bounds_,
            hover_callback,
            click_callback,
            priority,
            this // user_data pointer for additional context
        );

        event_handle_ = manager.RegisterRegion(region);
    }

    void UnregisterFromEventManager(towerforge::ui::MouseEventManager& manager) {
        if (event_handle_ != -1) {
            manager.UnregisterRegion(event_handle_);
            event_handle_ = -1;
        }
    }

    void UpdatePosition(const Rectangle new_bounds, towerforge::ui::MouseEventManager& manager) {
        bounds_ = new_bounds;
        if (event_handle_ != -1) {
            manager.UpdateRegionBounds(event_handle_, bounds_);
        }
    }

    Rectangle GetBounds() const { return bounds_; }
    std::string GetName() const { return name_; }

private:
    int id_;
    Rectangle bounds_;
    std::string name_;
    int event_handle_; // Handle for mouse event manager
};

// Example game world that manages facilities
class GameWorld {
public:
    GameWorld() {
        // Create some example facilities
        facilities_.push_back(std::make_unique<GameFacility>(
            1, Rectangle{100, 200, 80, 60}, "Office A"
        ));
        facilities_.push_back(std::make_unique<GameFacility>(
            2, Rectangle{200, 200, 80, 60}, "Restaurant"
        ));
        facilities_.push_back(std::make_unique<GameFacility>(
            3, Rectangle{300, 200, 120, 80}, "Flagship"
        ));

        // Register all facilities with event manager
        for (auto& facility : facilities_) {
            facility->RegisterWithEventManager(event_manager_);
        }
    }

    void Update() {
        // Get current mouse state from Raylib
        float mouse_x = GetMouseX();
        float mouse_y = GetMouseY();
        bool left_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool right_down = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
        bool left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool right_pressed = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);

        // Create mouse event
        towerforge::ui::MouseEvent event(
            mouse_x, mouse_y,
            left_down, right_down,
            left_pressed, right_pressed
        );

        // Process mouse events for all registered facilities
        event_manager_.ProcessMouseEvents(event);
    }

    void AddFacility(int id, Rectangle bounds, const std::string& name) {
        auto facility = std::make_unique<GameFacility>(id, bounds, name);
        facility->RegisterWithEventManager(event_manager_);
        facilities_.push_back(std::move(facility));
    }

    void RemoveFacility(int id) {
        auto it = std::find_if(facilities_.begin(), facilities_.end(),
            [id](const auto& f) { return f->GetName().find(std::to_string(id)) != std::string::npos; });
        
        if (it != facilities_.end()) {
            (*it)->UnregisterFromEventManager(event_manager_);
            facilities_.erase(it);
        }
    }

    void Render() const {
        // Render all facilities
        for (const auto& facility : facilities_) {
            DrawRectangleRec(facility->GetBounds(), BLUE);
            DrawRectangleLinesEx(facility->GetBounds(), 2, DARKBLUE);
            
            // Draw facility name
            const auto bounds = facility->GetBounds();
            DrawText(facility->GetName().c_str(),
                    bounds.x + 5, bounds.y + 5, 12, WHITE);
        }
    }

private:
    towerforge::ui::MouseEventManager event_manager_;
    std::vector<std::unique_ptr<GameFacility>> facilities_;
};

} // namespace example

// Usage in game loop:
/*
int main() {
    InitWindow(800, 600, "MouseEventManager Example");
    
    example::GameWorld world;
    
    while (!WindowShouldClose()) {
        // Update
        world.Update();
        
        // Render
        BeginDrawing();
        ClearBackground(RAYWHITE);
        world.Render();
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
*/
