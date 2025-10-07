#include <iostream>
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "=== TowerForge Grid System Demo ===" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << std::endl;
    
    // Create and initialize the ECS world
    std::cout << "Initializing ECS World..." << std::endl;
    ECSWorld ecs_world;
    ecs_world.Initialize();
    std::cout << std::endl;
    
    // Get the tower grid
    auto& grid = ecs_world.GetTowerGrid();
    
    std::cout << "=== Initial Grid State ===" << std::endl;
    std::cout << "Floors: " << grid.GetFloorCount() << std::endl;
    std::cout << "Columns: " << grid.GetColumnCount() << std::endl;
    std::cout << "Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << std::endl;
    
    // Place facilities
    std::cout << "=== Placing Facilities ===" << std::endl;
    
    std::cout << "Placing Lobby (floor 0, column 0, width 10)... ";
    bool success = grid.PlaceFacility(0, 0, 10, 1);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "Placing Office (floor 1, column 2, width 8)... ";
    success = grid.PlaceFacility(1, 2, 8, 2);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "Placing Restaurant (floor 2, column 5, width 6)... ";
    success = grid.PlaceFacility(2, 5, 6, 3);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "Placing Shop (floor 3, column 1, width 4)... ";
    success = grid.PlaceFacility(3, 1, 4, 4);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "Placing Hotel (floor 4, column 8, width 10)... ";
    success = grid.PlaceFacility(4, 8, 10, 5);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << std::endl;
    
    // Try overlapping placement (should fail)
    std::cout << "=== Testing Collision Detection ===" << std::endl;
    std::cout << "Attempting to place overlapping facility (floor 1, column 5, width 3)... ";
    success = grid.PlaceFacility(1, 5, 3, 6);
    std::cout << (success ? "SUCCESS (unexpected!)" : "FAILED (expected)") << std::endl;
    std::cout << std::endl;
    
    // Query grid
    std::cout << "=== Spatial Queries ===" << std::endl;
    std::cout << "Is position (0, 0) occupied? " 
              << (grid.IsOccupied(0, 0) ? "Yes" : "No") << std::endl;
    std::cout << "Is position (0, 15) occupied? " 
              << (grid.IsOccupied(0, 15) ? "Yes" : "No") << std::endl;
    std::cout << "Facility at (0, 5): " << grid.GetFacilityAt(0, 5) << std::endl;
    std::cout << "Facility at (1, 6): " << grid.GetFacilityAt(1, 6) << std::endl;
    std::cout << "Facility at (0, 15): " << grid.GetFacilityAt(0, 15) << std::endl;
    std::cout << "Is space available at (floor 5, column 0, width 8)? " 
              << (grid.IsSpaceAvailable(5, 0, 8) ? "Yes" : "No") << std::endl;
    std::cout << "Is space available at (floor 1, column 11, width 5)? " 
              << (grid.IsSpaceAvailable(1, 11, 5) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Add floors and columns
    std::cout << "=== Dynamic Grid Expansion ===" << std::endl;
    std::cout << "Adding 5 floors... ";
    int first_new_floor = grid.AddFloors(5);
    std::cout << "First new floor index: " << first_new_floor << std::endl;
    
    std::cout << "Adding 10 columns... ";
    int first_new_col = grid.AddColumns(10);
    std::cout << "First new column index: " << first_new_col << std::endl;
    
    std::cout << "New grid size: " << grid.GetFloorCount() << " floors x " 
              << grid.GetColumnCount() << " columns" << std::endl;
    std::cout << std::endl;
    
    // Place facility in new space
    std::cout << "Placing Penthouse on new floor (floor 14, column 25, width 5)... ";
    success = grid.PlaceFacility(14, 25, 5, 7);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << std::endl;
    
    // Remove a facility
    std::cout << "=== Removing Facilities ===" << std::endl;
    std::cout << "Removing Restaurant (ID 3)... ";
    success = grid.RemoveFacility(3);
    std::cout << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Occupied cells after removal: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << "Is position (2, 5) now available? " 
              << (!grid.IsOccupied(2, 5) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Create entities with GridPosition component
    std::cout << "=== Creating Entities with Grid Positions ===" << std::endl;
    auto lobby_entity = ecs_world.CreateEntity("Lobby");
    lobby_entity.set<GridPosition>({0, 0, 10});
    lobby_entity.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    
    auto office_entity = ecs_world.CreateEntity("Office");
    office_entity.set<GridPosition>({1, 2, 8});
    office_entity.set<BuildingComponent>({BuildingComponent::Type::Office, 1, 8, 20});
    
    std::cout << "Created 2 entities with GridPosition components" << std::endl;
    std::cout << std::endl;
    
    // Final state
    std::cout << "=== Final Grid State ===" << std::endl;
    std::cout << "Floors: " << grid.GetFloorCount() << std::endl;
    std::cout << "Columns: " << grid.GetColumnCount() << std::endl;
    std::cout << "Occupied cells: " << grid.GetOccupiedCellCount() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Demo Complete ===" << std::endl;
    return 0;
}
