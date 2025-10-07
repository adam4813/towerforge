#include <iostream>
#include "rendering/renderer.h"
#include "rendering/camera.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "ui/hud.h"
#include "ui/build_menu.h"
#include "ui/placement_system.h"

using namespace TowerForge::Core;
using namespace towerforge::ui;
using namespace towerforge::rendering;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Elevator System Demo" << std::endl;
    std::cout << "Initializing Raylib renderer..." << std::endl;
    
    // Create and initialize the renderer
    Renderer renderer;
    renderer.Initialize(1200, 800, "TowerForge - Elevator System Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create the global TimeManager as a singleton
    // Normal speed for better observation
    ecs_world.GetWorld().set<TimeManager>({1.0f});
    
    // Create the global TowerEconomy as a singleton
    ecs_world.GetWorld().set<TowerEconomy>({10000.0f});
    
    std::cout << "Creating elevator system demo..." << std::endl;
    
    // Create elevator shaft serving floors 0-5 at column 10
    auto shaft1 = ecs_world.CreateEntity("MainElevatorShaft");
    shaft1.set<ElevatorShaft>({10, 0, 5, 1});
    
    // Create elevator car for this shaft
    auto car1 = ecs_world.CreateEntity("Elevator1");
    car1.set<ElevatorCar>({static_cast<int>(shaft1.id()), 0, 8});
    
    // Create a second elevator shaft at column 5
    auto shaft2 = ecs_world.CreateEntity("SecondaryElevatorShaft");
    shaft2.set<ElevatorShaft>({5, 0, 3, 1});
    
    auto car2 = ecs_world.CreateEntity("Elevator2");
    car2.set<ElevatorCar>({static_cast<int>(shaft2.id()), 0, 6});
    
    // Create some facilities on different floors
    auto lobby = ecs_world.GetFacilityManager().CreateFacility(
        BuildingComponent::Type::Lobby, 0, 0, 20, "MainLobby");
    
    auto office1 = ecs_world.GetFacilityManager().CreateFacility(
        BuildingComponent::Type::Office, 1, 0, 10, "Office1");
    
    auto office2 = ecs_world.GetFacilityManager().CreateFacility(
        BuildingComponent::Type::Office, 2, 0, 10, "Office2");
    
    auto shop1 = ecs_world.GetFacilityManager().CreateFacility(
        BuildingComponent::Type::RetailShop, 3, 0, 8, "Shop1");
    
    auto residential1 = ecs_world.GetFacilityManager().CreateFacility(
        BuildingComponent::Type::Residential, 4, 0, 10, "Residential1");
    
    // Create people that will use the elevators
    // Person 1: Starting in lobby, going to office on floor 2
    auto person1 = ecs_world.CreateEntity("Alice");
    Person alice("Alice", 0, 2.0f);  // Start at floor 0, column 2
    alice.SetDestination(2, 8.0f, "Going to work");
    person1.set<Person>(alice);
    person1.set<Satisfaction>({80.0f});
    
    // Person 2: Starting on floor 3, going to lobby
    auto person2 = ecs_world.CreateEntity("Bob");
    Person bob("Bob", 3, 6.0f);  // Start at floor 3, column 6
    bob.SetDestination(0, 15.0f, "Going home");
    person2.set<Person>(bob);
    person2.set<Satisfaction>({75.0f});
    
    // Person 3: Starting in lobby, going to floor 4
    auto person3 = ecs_world.CreateEntity("Charlie");
    Person charlie("Charlie", 0, 12.0f);  // Start at floor 0, column 12
    charlie.SetDestination(4, 8.0f, "Going home");
    person3.set<Person>(charlie);
    person3.set<Satisfaction>({85.0f});
    
    // Person 4: Starting on floor 2, going to floor 4
    auto person4 = ecs_world.CreateEntity("Diana");
    Person diana("Diana", 2, 3.0f);  // Start at floor 2, column 3
    diana.SetDestination(4, 5.0f, "Visiting friend");
    person4.set<Person>(diana);
    person4.set<Satisfaction>({70.0f});
    
    std::cout << "Demo initialized. Starting simulation..." << std::endl;
    
    // Grid rendering configuration
    const int grid_offset_x = 50;
    const int grid_offset_y = 50;
    const int cell_width = 40;
    const int cell_height = 40;
    
    auto& grid = ecs_world.GetTowerGrid();
    
    // Main game loop
    float elapsed_time = 0.0f;
    const float screenshot_time = 10.0f;  // Take screenshot after 10 seconds
    bool screenshot_taken = false;
    
    while (!renderer.ShouldClose()) {
        // Update simulation
        float time_step = 1.0f / 60.0f;  // 60 FPS
        elapsed_time += time_step;
        ecs_world.Update(time_step);
        
        // Take screenshot after specified time
        if (!screenshot_taken && elapsed_time >= screenshot_time) {
            TakeScreenshot("docs/elevator_demo_screenshot.png");
            std::cout << "Screenshot saved to docs/elevator_demo_screenshot.png" << std::endl;
            screenshot_taken = true;
        }
        
        renderer.BeginFrame();
        renderer.Clear(Color{30, 30, 40, 255});
        
        // Draw grid
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            for (int col = 0; col < grid.GetColumnCount(); ++col) {
                int x = grid_offset_x + col * cell_width;
                int y = grid_offset_y + floor * cell_height;
                
                // Draw grid cell outline
                DrawRectangleLines(x, y, cell_width, cell_height, ColorAlpha(WHITE, 0.15f));
                
                // Draw facility if present
                if (grid.IsOccupied(floor, col)) {
                    Color facility_color = SKYBLUE;
                    DrawRectangle(x + 2, y + 2, cell_width - 4, cell_height - 4, facility_color);
                }
            }
        }
        
        // Draw floor labels
        for (int floor = 0; floor < grid.GetFloorCount(); ++floor) {
            int y = grid_offset_y + floor * cell_height;
            DrawText(TextFormat("F%d", floor), grid_offset_x - 35, y + 12, 14, LIGHTGRAY);
        }
        
        // Draw elevator shafts
        auto shaft_query = ecs_world.GetWorld().query<const ElevatorShaft>();
        shaft_query.each([&](flecs::entity e, const ElevatorShaft& shaft) {
            // Draw shaft as vertical column
            for (int floor = shaft.bottom_floor; floor <= shaft.top_floor; ++floor) {
                int x = grid_offset_x + shaft.column * cell_width;
                int y = grid_offset_y + floor * cell_height;
                
                // Draw shaft background
                DrawRectangle(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{60, 60, 70, 255});
                DrawRectangleLines(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{100, 100, 120, 255});
            }
        });
        
        // Draw elevator cars
        auto car_query = ecs_world.GetWorld().query<const ElevatorCar>();
        car_query.each([&](flecs::entity e, const ElevatorCar& car) {
            // Find the shaft for this car
            auto shaft_entity = ecs_world.GetWorld().entity(car.shaft_entity_id);
            if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                const ElevatorShaft& shaft = shaft_entity.ensure<ElevatorShaft>();
                
                int x = grid_offset_x + shaft.column * cell_width;
                int y = grid_offset_y + static_cast<int>(car.current_floor * cell_height);
                
                // Color based on state
                Color car_color;
                switch (car.state) {
                    case ElevatorState::Idle:
                        car_color = GRAY;
                        break;
                    case ElevatorState::MovingUp:
                        car_color = SKYBLUE;
                        break;
                    case ElevatorState::MovingDown:
                        car_color = PURPLE;
                        break;
                    case ElevatorState::DoorsOpening:
                    case ElevatorState::DoorsClosing:
                        car_color = YELLOW;
                        break;
                    case ElevatorState::DoorsOpen:
                        car_color = GREEN;
                        break;
                }
                
                // Draw car as rectangle
                DrawRectangle(x + 6, y + 6, cell_width - 12, cell_height - 12, car_color);
                
                // Draw occupancy
                if (car.current_occupancy > 0) {
                    DrawText(TextFormat("%d", car.current_occupancy), x + 18, y + 14, 16, BLACK);
                }
                
                // Draw door indicator
                if (car.state == ElevatorState::DoorsOpen || 
                    car.state == ElevatorState::DoorsOpening ||
                    car.state == ElevatorState::DoorsClosing) {
                    DrawRectangle(x + 8, y + 8, 4, cell_height - 16, BLACK);
                    DrawRectangle(x + cell_width - 12, y + 8, 4, cell_height - 16, BLACK);
                }
            }
        });
        
        // Draw people (Person entities)
        auto person_query = ecs_world.GetWorld().query<const Person>();
        person_query.each([&](flecs::entity e, const Person& person) {
            int person_x = grid_offset_x + static_cast<int>(person.current_column * cell_width) + cell_width / 2;
            int person_y = grid_offset_y + person.current_floor * cell_height + cell_height / 2;
            
            // Color based on state
            Color person_color;
            switch (person.state) {
                case PersonState::Idle:
                    person_color = LIGHTGRAY;
                    break;
                case PersonState::Walking:
                    person_color = BLUE;
                    break;
                case PersonState::WaitingForElevator:
                    person_color = ORANGE;
                    break;
                case PersonState::InElevator:
                    person_color = PURPLE;
                    break;
                case PersonState::AtDestination:
                    person_color = GREEN;
                    break;
            }
            
            // Draw person as circle
            DrawCircle(person_x, person_y, 8, person_color);
            DrawCircle(person_x, person_y, 6, BLACK);
            DrawCircle(person_x, person_y, 4, person_color);
            
            // Draw destination indicator
            int dest_x = grid_offset_x + static_cast<int>(person.destination_column * cell_width) + cell_width / 2;
            int dest_y = grid_offset_y + person.destination_floor * cell_height + cell_height / 2;
            DrawCircle(dest_x, dest_y, 4, Color{person_color.r, person_color.g, person_color.b, 150});
        });
        
        // Draw title
        DrawText("TowerForge - Elevator System Demo", 50, 10, 20, WHITE);
        
        // Draw elevator legend
        int legend_x = 900;
        int legend_y = 50;
        DrawText("Elevator States:", legend_x, legend_y, 16, WHITE);
        DrawRectangle(legend_x, legend_y + 25, 20, 20, GRAY);
        DrawText("Idle", legend_x + 25, legend_y + 25, 14, WHITE);
        DrawRectangle(legend_x, legend_y + 50, 20, 20, SKYBLUE);
        DrawText("Moving Up", legend_x + 25, legend_y + 50, 14, WHITE);
        DrawRectangle(legend_x, legend_y + 75, 20, 20, PURPLE);
        DrawText("Moving Down", legend_x + 25, legend_y + 75, 14, WHITE);
        DrawRectangle(legend_x, legend_y + 100, 20, 20, GREEN);
        DrawText("Doors Open", legend_x + 25, legend_y + 100, 14, WHITE);
        DrawRectangle(legend_x, legend_y + 125, 20, 20, YELLOW);
        DrawText("Doors Opening/Closing", legend_x + 25, legend_y + 125, 14, WHITE);
        
        // Draw person legend
        DrawText("Person States:", legend_x, legend_y + 170, 16, WHITE);
        DrawCircle(legend_x + 10, legend_y + 200, 8, BLUE);
        DrawText("Walking", legend_x + 25, legend_y + 193, 14, WHITE);
        DrawCircle(legend_x + 10, legend_y + 225, 8, ORANGE);
        DrawText("Waiting for Elevator", legend_x + 25, legend_y + 218, 14, WHITE);
        DrawCircle(legend_x + 10, legend_y + 250, 8, PURPLE);
        DrawText("In Elevator", legend_x + 25, legend_y + 243, 14, WHITE);
        DrawCircle(legend_x + 10, legend_y + 275, 8, GREEN);
        DrawText("At Destination", legend_x + 25, legend_y + 268, 14, WHITE);
        
        // Draw debug info panel
        int debug_y = 350;
        DrawText("Live Status:", legend_x, debug_y, 16, GOLD);
        debug_y += 25;
        
        // Show elevator status
        auto car_debug_query = ecs_world.GetWorld().query<const ElevatorCar>();
        car_debug_query.each([&](flecs::entity e, const ElevatorCar& car) {
            if (debug_y < 750) {
                DrawText(TextFormat("%s: %s", e.name().c_str(), car.GetStateString()), 
                         legend_x, debug_y, 12, WHITE);
                debug_y += 15;
                DrawText(TextFormat("  Floor: %.1f, Occupancy: %d/%d", 
                         car.current_floor, car.current_occupancy, car.max_capacity),
                         legend_x, debug_y, 12, LIGHTGRAY);
                debug_y += 15;
                if (!car.stop_queue.empty()) {
                    std::string stops = "  Stops: [";
                    for (size_t i = 0; i < car.stop_queue.size(); i++) {
                        stops += std::to_string(car.stop_queue[i]);
                        if (i < car.stop_queue.size() - 1) stops += ", ";
                    }
                    stops += "]";
                    DrawText(stops.c_str(), legend_x, debug_y, 12, YELLOW);
                    debug_y += 15;
                }
                debug_y += 5;
            }
        });
        
        // Show person status
        auto person_debug_query = ecs_world.GetWorld().query<const Person>();
        person_debug_query.each([&](flecs::entity e, const Person& person) {
            if (debug_y < 750) {
                DrawText(TextFormat("%s: %s", person.name.c_str(), person.GetStateString()), 
                         legend_x, debug_y, 12, WHITE);
                debug_y += 15;
                DrawText(TextFormat("  F%d(%.1f) -> F%d(%.1f)", 
                         person.current_floor, person.current_column,
                         person.destination_floor, person.destination_column),
                         legend_x, debug_y, 12, LIGHTGRAY);
                debug_y += 20;
            }
        });
        
        // Draw instructions
        DrawText("Press ESC to exit", 50, 750, 14, LIGHTGRAY);
        DrawText(TextFormat("Time: %.1fs", elapsed_time), 50, 770, 14, LIGHTGRAY);
        
        renderer.EndFrame();
        
        // Exit after screenshot is taken and a bit more time for observation
        if (screenshot_taken && elapsed_time >= screenshot_time + 5.0f) {
            break;
        }
    }
    
    renderer.Shutdown();
    std::cout << "Demo completed." << std::endl;
    
    return 0;
}
