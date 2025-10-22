/**
 * @file screenshot_app.cpp
 * @brief Screenshot generation tool for TowerForge Tower Grid System documentation
 * 
 * USAGE INSTRUCTIONS FOR FUTURE MAINTENANCE:
 * ==========================================
 * 
 * WHEN TO UPDATE THIS TOOL:
 * ------------------------
 * - When the Tower Grid System API changes (new methods, different behavior)
 * - When adding new facility types to the system
 * - When the grid dimensions or layout need to be visualized differently
 * - When the visual style or color scheme needs updating
 * - When documentation screenshots need to be regenerated
 * 
 * HOW TO USE THIS TOOL:
 * --------------------
 * 1. Build the project:
 *    ```bash
 *    mkdir build && cd build
 *    cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
 *    cmake --build .
 *    ```
 * 
 * 2. Run with Xvfb (headless X server) to generate screenshots:
 *    ```bash
 *    xvfb-run -a ./bin/screenshot_app
 *    ```
 *    This creates: towerforge_screenshot.png in the current directory
 * 
 * 3. Copy the screenshot to the docs folder:
 *    ```bash
 *    cp towerforge_screenshot.png ../docs/grid_demo_screenshot.png
 *    ```
 * 
 * 4. Update the PR/README with the new screenshot if needed
 * 
 * HOW TO UPDATE THE VISUALIZATION:
 * --------------------------------
 * - Facility placement (lines ~90-100): Use facility_mgr.CreateFacility() calls
 *   to change which facilities are displayed
 * 
 * - Colors (lines ~140-165): Update the switch statement to change facility colors
 *   or add new facility types
 * 
 * - Grid layout (lines ~117-120): Modify grid_offset_x/y, cell_width/height to
 *   change the visual grid dimensions
 * 
 * - Legend (lines ~175-190): Update to reflect new facility types or colors
 * 
 * - Text/Labels (lines 157-173): Modify to update title, descriptions, or stats
 * 
 * TROUBLESHOOTING:
 * ---------------
 * - If screenshot is blank: Increase the frame count (line 110) to allow more
 *   rendering time
 * 
 * - If colors don't match: Verify the facility IDs in the switch statement
 *   match the IDs used in PlaceFacility() calls
 * 
 * - If running without Xvfb fails: Install xvfb-run or run on a system with
 *   an X server (display)
 * 
 * - If screenshot path fails: Check write permissions for /tmp or modify the
 *   path in line 180
 */

#include <iostream>
#include "rendering/renderer.h"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include <raylib.h>

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Screenshot Generator" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    
    // Create and initialize the renderer
    towerforge::rendering::Renderer renderer;
    renderer.Initialize(800, 600, "TowerForge - Facility System Demo");
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    // Set up global time manager (1x speed for screenshot)
    ecs_world.GetWorld().set<TimeManager>({1.0f});
    
    // Set up global tower economy
    ecs_world.GetWorld().set<TowerEconomy>({10000.0f});
  
    // Get the tower grid and facility manager
    auto& grid = ecs_world.GetTowerGrid();
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create facilities using the FacilityManager
    auto lobby = facility_mgr.CreateFacility(BuildingComponent::Type::Lobby, 0, 0);
    auto office = facility_mgr.CreateFacility(BuildingComponent::Type::Office, 1, 2);
    auto residential = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 5);
    auto shop = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 1);
    auto restaurant = facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 4, 8);
    
    // Create elevator system for demonstration
    auto elevator_shaft = ecs_world.CreateEntity("ElevatorShaft");
    elevator_shaft.set<ElevatorShaft>({10, 0, 4, 1});  // Column 10, floors 0-4, 1 car
    
    auto elevator_car = ecs_world.CreateEntity("ElevatorCar");
    elevator_car.set<ElevatorCar>({static_cast<int>(elevator_shaft.id()), 0, 8});  // Start at floor 0, capacity 8
    
    // Create some example people using the new Person component
    // Person 1: Spawning in lobby, going to office on floor 1
    auto person1 = ecs_world.CreateEntity("Alice");
    Person alice("Alice", 0, 2.0f);  // Start at floor 0, column 2
    alice.SetDestination(1, 8.0f, "Going to work");  // Go to floor 1, column 8
    person1.set<Person>(alice);
    person1.set<Satisfaction>({80.0f});
    
    // Person 2: Starting on floor 2, going to lobby
    auto person2 = ecs_world.CreateEntity("Bob");
    Person bob("Bob", 2, 12.0f);  // Start at floor 2, column 12
    bob.SetDestination(0, 3.0f, "Going home");  // Go to floor 0 (lobby), column 3
    person2.set<Person>(bob);
    person2.set<Satisfaction>({75.0f});
    
    // Person 3: Walking on same floor
    auto person3 = ecs_world.CreateEntity("Charlie");
    Person charlie("Charlie", 3, 5.0f);  // Start at floor 3, column 5
    charlie.SetDestination(3, 15.0f, "Going to shop");  // Go to same floor, column 15
    person3.set<Person>(charlie);
    person3.set<Satisfaction>({70.0f});
    
    // Person 4: At destination (idle)
    auto person4 = ecs_world.CreateEntity("Diana");
    Person diana("Diana", 4, 10.0f);  // Start at floor 4, column 10
    // No destination set, will be idle
    person4.set<Person>(diana);
    person4.set<Satisfaction>({90.0f});
    
    // Keep old actors for compatibility
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    actor1.set<Satisfaction>({85.0f});  // High satisfaction
    
    // Add a daily schedule for John
    DailySchedule john_schedule;
    john_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 9.0f);
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.0f);
    john_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 17.0f);
    actor1.set<DailySchedule>(john_schedule);
    
    auto actor2 = ecs_world.CreateEntity("Sarah");
    actor2.set<Position>({20.0f, 0.0f});
    actor2.set<Velocity>({-0.3f, 0.0f});
    actor2.set<Actor>({"Sarah", 3, 0.8f});
    actor2.set<Satisfaction>({65.0f});  // Average satisfaction
    
    // Add a daily schedule for Sarah
    DailySchedule sarah_schedule;
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::ArriveWork, 8.5f);
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LunchBreak, 12.5f);
    sarah_schedule.AddWeekdayAction(ScheduledAction::Type::LeaveWork, 16.5f);
    actor2.set<DailySchedule>(sarah_schedule);
    
    // Add economics to facilities
    lobby.set<FacilityEconomics>({50.0f, 10.0f, 50});
    lobby.set<Satisfaction>({90.0f});
    
    office.set<FacilityEconomics>({150.0f, 30.0f, 20});
    office.set<Satisfaction>({75.0f});
    
    restaurant.set<FacilityEconomics>({200.0f, 60.0f, 30});
    restaurant.set<Satisfaction>({70.0f});
    
    // Render frames to allow people to move and show different states
    // Run simulation for 3 seconds (180 frames at 60 FPS) to capture mid-movement
    for (int i = 0; i < 180; i++) {
        ecs_world.Update(1.0f / 60.0f);
        
        // Only render the last few frames to capture the movement state
        if (i < 175) {
            continue;  // Skip rendering for performance
        }
        
        renderer.BeginFrame();
        renderer.Clear(DARKGRAY);
        
        // Draw grid representation
        constexpr int grid_offset_x = 50;
        constexpr int grid_offset_y = 50;
        constexpr int cell_width = 30;
        constexpr int cell_height = 40;
        
        // Helper function to convert floor index to screen Y coordinate
        const int ground_floor_screen_y = grid_offset_y + (5 / 2) * cell_height; // 5 floors total
        auto FloorToScreenY = [ground_floor_screen_y, cell_height](int floor) -> int {
            return ground_floor_screen_y - (floor * cell_height);
        };
        
        // Draw background (sky above ground, earth below)
        const int ground_y = FloorToScreenY(0);
        DrawRectangle(0, 0, 800, ground_y, Color{135, 206, 235, 255}); // Sky
        DrawRectangle(0, ground_y + cell_height, 800, 600, Color{101, 67, 33, 255}); // Earth
        
        // Draw grid lines
        for (int floor = 0; floor < 6; floor++) {
            int y = FloorToScreenY(floor);
            DrawLine(grid_offset_x, y, grid_offset_x + 600, y, GRAY);
        }
        
        for (int col = 0; col <= 20; col++) {
            int x = grid_offset_x + col * cell_width;
            // Draw vertical lines from lowest to highest floor
            int y_start = FloorToScreenY(0) + cell_height;
            int y_end = FloorToScreenY(4);
            DrawLine(x, y_start, x, y_end, GRAY);
        }
        
        // Draw occupied cells (facilities)
        for (int floor = 0; floor < 5; floor++) {
            for (int col = 0; col < 20; col++) {
                if (grid.IsOccupied(floor, col)) {
                    int x = grid_offset_x + col * cell_width + 2;
                    int y = FloorToScreenY(floor) + 2;
                    
                    // Get facility type and color from facility manager
                    int facilityId = grid.GetFacilityAt(floor, col);
                    auto facilityType = facility_mgr.GetFacilityType(facilityId);
                    
                    Color color;
                    switch (facilityType) {
                        case BuildingComponent::Type::Lobby:
                            color = GOLD; break;
                        case BuildingComponent::Type::Office:
                            color = SKYBLUE; break;
                        case BuildingComponent::Type::Residential:
                            color = PURPLE; break;
                        case BuildingComponent::Type::RetailShop:
                            color = GREEN; break;
                        case BuildingComponent::Type::Restaurant:
                            color = RED; break;
                        case BuildingComponent::Type::Hotel:
                            color = DARKBLUE; break;
                        case BuildingComponent::Type::Elevator:
                            color = GRAY; break;
                        case BuildingComponent::Type::Gym:
                            color = ORANGE; break;
                        case BuildingComponent::Type::Arcade:
                            color = MAGENTA; break;
                        case BuildingComponent::Type::Theater:
                            color = VIOLET; break;
                        case BuildingComponent::Type::ConferenceHall:
                            color = SKYBLUE; break;
                        case BuildingComponent::Type::FlagshipStore:
                            color = Color{0, 206, 209, 255}; break;
                        default:
                            color = SKYBLUE; break;
                    }
                    
                    DrawRectangle(x, y, cell_width - 4, cell_height - 4, color);
                }
            }
        }
        
        // Draw elevator shafts
        auto shaft_query = ecs_world.GetWorld().query<const ElevatorShaft>();
        shaft_query.each([&](flecs::entity e, const ElevatorShaft& shaft) {
            // Draw shaft as vertical column
            for (int floor = shaft.bottom_floor; floor <= shaft.top_floor; ++floor) {
                const int x = grid_offset_x + shaft.column * cell_width;
                const int y = FloorToScreenY(floor);
                
                // Draw shaft background
                DrawRectangle(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{60, 60, 70, 255});
                DrawRectangleLines(x + 4, y + 4, cell_width - 8, cell_height - 8, Color{100, 100, 120, 255});
            }
        });
        
        // Draw elevator cars
        auto car_query = ecs_world.GetWorld().query<const ElevatorCar>();
        car_query.each([&](flecs::entity e, const ElevatorCar& car) {
            // Find the shaft for this car
            const auto shaft_entity = ecs_world.GetWorld().entity(car.shaft_entity_id);
            if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                const ElevatorShaft& shaft = shaft_entity.ensure<ElevatorShaft>();

                const int x = grid_offset_x + shaft.column * cell_width;
                const int y = FloorToScreenY(static_cast<int>(car.current_floor));
                
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
                
                // Draw occupancy indicator
                if (car.current_occupancy > 0) {
                    DrawText(TextFormat("%d", car.current_occupancy), x + 16, y + 18, 14, BLACK);
                }
            }
        });
        
        // Draw people (Person entities)
        auto person_query = ecs_world.GetWorld().query<const Person>();
        person_query.each([&](flecs::entity e, const Person& person) {
            // Calculate screen position from floor and column
            const int person_x = grid_offset_x + static_cast<int>(person.current_column * cell_width);
            const int person_y = FloorToScreenY(person.current_floor) + cell_height / 2;
            
            // Draw person as a circle
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
                default:
                    person_color = WHITE;
                    break;
            }
            
            DrawCircle(person_x, person_y, 8, person_color);
            DrawCircle(person_x, person_y, 6, BLACK);  // Inner circle
            DrawCircle(person_x, person_y, 4, person_color);
            
            // Draw destination indicator
            const int dest_x = grid_offset_x + static_cast<int>(person.destination_column * cell_width);
            const int dest_y = FloorToScreenY(person.destination_floor) + cell_height / 2;
            DrawLine(person_x, person_y, dest_x, dest_y, Color{255, 255, 255, 100});
            DrawCircle(dest_x, dest_y, 4, Color{person_color.r, person_color.g, person_color.b, 150});
        });
        
        // Draw title and legend
        DrawText("TowerForge - Person Movement System", 50, 10, 20, WHITE);
        
        // Display current simulation time (top-left panel)
        const auto& time_mgr = ecs_world.GetWorld().get<TimeManager>();
        std::string time_str = "Time: " + time_mgr.GetTimeString();
        std::string day_str = std::string("Day: ") + time_mgr.GetDayName();
        std::string week_str = "Week: " + std::to_string(time_mgr.current_week);
        
        // Background panel for time display
        DrawRectangle(520, 50, 250, 100, Color{0, 0, 0, 180});
        DrawText(time_str.c_str(), 530, 60, 18, WHITE);
        DrawText(day_str.c_str(), 530, 85, 18, WHITE);
        DrawText(week_str.c_str(), 530, 110, 18, WHITE);
        
        // Day/night cycle indicator
        Color cycle_color = time_mgr.IsBusinessHours() ? YELLOW : DARKBLUE;
        const char* cycle_text = time_mgr.IsBusinessHours() ? "DAY" : "NIGHT";
        DrawRectangle(530, 130, 80, 30, Color{0, 0, 0, 180});
        DrawText(cycle_text, 540, 135, 16, cycle_color);
        
        DrawText("Grid: 10 floors x 20 columns", 50, 280, 16, LIGHTGRAY);
        DrawText("Legend:", 50, 310, 16, WHITE);
        DrawRectangle(50, 335, 20, 15, GOLD);
        DrawText("Lobby", 80, 335, 14, WHITE);
        DrawRectangle(50, 355, 20, 15, SKYBLUE);
        DrawText("Office", 80, 355, 14, WHITE);
        DrawRectangle(50, 375, 20, 15, PURPLE);
        DrawText("Residential", 80, 375, 14, WHITE);
        DrawRectangle(50, 395, 20, 15, GREEN);
        DrawText("RetailShop", 80, 395, 14, WHITE);
        DrawRectangle(50, 415, 20, 15, RED);
        DrawText("Restaurant", 80, 415, 14, WHITE);
        
        // Person state legend
        DrawText("Person States:", 50, 450, 14, WHITE);
        DrawCircle(60, 473, 6, BLUE);
        DrawText("Walking", 75, 468, 12, WHITE);
        DrawCircle(155, 473, 6, ORANGE);
        DrawText("Waiting", 170, 468, 12, WHITE);
        DrawCircle(60, 493, 6, PURPLE);
        DrawText("In Elevator", 75, 488, 12, WHITE);
        DrawCircle(155, 493, 6, GREEN);
        DrawText("At Dest", 170, 488, 12, WHITE);
        
        // Draw info panel
        DrawText(TextFormat("Occupied cells: %d", grid.GetOccupiedCellCount()), 50, 520, 16, LIGHTGRAY);
        DrawText(TextFormat("Floors: %d | Columns: %d", grid.GetFloorCount(), grid.GetColumnCount()), 50, 540, 16, LIGHTGRAY);
        
        // Person debug info panel
        DrawRectangle(520, 170, 250, 180, Color{0, 0, 0, 180});
        DrawText("People Status:", 530, 180, 16, YELLOW);
        
        int person_info_y = 205;
        auto person_debug_query = ecs_world.GetWorld().query<const Person>();
        person_debug_query.each([&](flecs::entity e, const Person& person) {
            if (person_info_y < 340) {  // Don't overflow panel
                const std::string info = person.name + ": " + std::string(person.GetStateString());
                DrawText(info.c_str(), 530, person_info_y, 12, WHITE);
                person_info_y += 15;

                const std::string location = "  F" + std::to_string(person.current_floor) + 
                                     " C" + std::to_string(static_cast<int>(person.current_column)) +
                                     " -> F" + std::to_string(person.destination_floor) +
                                     " C" + std::to_string(static_cast<int>(person.destination_column));
                DrawText(location.c_str(), 530, person_info_y, 10, LIGHTGRAY);
                person_info_y += 15;

                const std::string need = "  Need: " + person.current_need;
                DrawText(need.c_str(), 530, person_info_y, 10, LIGHTGRAY);
                person_info_y += 20;
            }
        });
        
        renderer.EndFrame();
    }
    
    // Take screenshot
    std::cout << "Taking screenshot..." << std::endl;
    TakeScreenshot("towerforge_screenshot.png");
    std::cout << "Screenshot saved to towerforge_screenshot.png" << std::endl;
    
    // Cleanup
    renderer.Shutdown();
    
    return 0;
}
