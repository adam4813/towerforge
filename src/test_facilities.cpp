/**
 * @file test_facilities.cpp
 * @brief Manual test for facility placement and removal
 * 
 * This program demonstrates and validates:
 * - Creating facilities using FacilityManager
 * - Placing facilities on the grid
 * - Removing facilities by entity
 * - Removing facilities by grid position
 * - Verifying grid state after operations
 */

#include <iostream>
#include <cassert>
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/facility_manager.hpp"

using namespace TowerForge::Core;

void PrintSeparator() {
    std::cout << "\n========================================\n" << std::endl;
}

int main() {
    std::cout << "TowerForge - Facility System Test" << std::endl;
    std::cout << "Testing facility placement and removal..." << std::endl;
    
    // Initialize ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    auto& grid = ecs_world.GetTowerGrid();
    auto& facility_mgr = ecs_world.GetFacilityManager();
    
    PrintSeparator();
    std::cout << "TEST 1: Create facilities using FacilityManager" << std::endl;
    std::cout << "Creating 4 different facility types..." << std::endl;
    
    auto lobby = facility_mgr.CreateFacility(
        BuildingComponent::Type::Lobby, 
        0, 0, 0, "TestLobby"
    );
    std::cout << "  Created Lobby at (0, 0)" << std::endl;
    
    auto office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        1, 2, 0, "TestOffice"
    );
    std::cout << "  Created Office at (1, 2)" << std::endl;
    
    auto residential = facility_mgr.CreateFacility(
        BuildingComponent::Type::Residential,
        2, 5, 0, "TestResidential"
    );
    std::cout << "  Created Residential at (2, 5)" << std::endl;
    
    auto shop = facility_mgr.CreateFacility(
        BuildingComponent::Type::RetailShop,
        3, 1, 0, "TestShop"
    );
    std::cout << "  Created RetailShop at (3, 1)" << std::endl;
    
    // Verify all facilities were created
    assert(lobby.is_alive() && "Lobby should be created");
    assert(office.is_alive() && "Office should be created");
    assert(residential.is_alive() && "Residential should be created");
    assert(shop.is_alive() && "Shop should be created");
    
    std::cout << "\n✓ All facilities created successfully" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 2: Verify grid placement" << std::endl;
    
    int occupied_count = grid.GetOccupiedCellCount();
    std::cout << "  Total occupied cells: " << occupied_count << std::endl;
    
    // Lobby: width 10, Office: width 8, Residential: width 6, Shop: width 4
    // Total: 28 cells
    assert(occupied_count == 28 && "Should have 28 occupied cells");
    
    assert(grid.IsOccupied(0, 0) && "Lobby position should be occupied");
    assert(grid.IsOccupied(1, 2) && "Office position should be occupied");
    assert(grid.IsOccupied(2, 5) && "Residential position should be occupied");
    assert(grid.IsOccupied(3, 1) && "Shop position should be occupied");
    
    std::cout << "✓ Grid placement verified" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 3: Verify facility attributes" << std::endl;
    
    auto lobby_component = lobby.get<BuildingComponent>();
    std::cout << "  Lobby - Width: " << lobby_component.width 
              << ", Capacity: " << lobby_component.capacity << std::endl;
    assert(lobby_component.width == 10 && "Lobby width should be 10");
    assert(lobby_component.capacity == 50 && "Lobby capacity should be 50");
    
    auto office_component = office.get<BuildingComponent>();
    std::cout << "  Office - Width: " << office_component.width 
              << ", Capacity: " << office_component.capacity << std::endl;
    assert(office_component.width == 8 && "Office width should be 8");
    assert(office_component.capacity == 20 && "Office capacity should be 20");
    
    auto residential_component = residential.get<BuildingComponent>();
    std::cout << "  Residential - Width: " << residential_component.width 
              << ", Capacity: " << residential_component.capacity << std::endl;
    assert(residential_component.width == 6 && "Residential width should be 6");
    assert(residential_component.capacity == 4 && "Residential capacity should be 4");
    
    auto shop_component = shop.get<BuildingComponent>();
    std::cout << "  RetailShop - Width: " << shop_component.width 
              << ", Capacity: " << shop_component.capacity << std::endl;
    assert(shop_component.width == 4 && "Shop width should be 4");
    assert(shop_component.capacity == 15 && "Shop capacity should be 15");
    
    std::cout << "✓ All facility attributes correct" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 4: Remove facility by entity" << std::endl;
    
    bool removed = facility_mgr.RemoveFacility(shop);
    assert(removed && "Shop should be removed successfully");
    assert(!shop.is_alive() && "Shop entity should no longer be alive");
    assert(!grid.IsOccupied(3, 1) && "Shop position should no longer be occupied");
    
    int new_occupied = grid.GetOccupiedCellCount();
    std::cout << "  Occupied cells after removal: " << new_occupied << std::endl;
    assert(new_occupied == 24 && "Should have 24 occupied cells (28 - 4)");
    
    std::cout << "✓ Facility removed by entity successfully" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 5: Remove facility by grid position" << std::endl;
    
    bool removed_at = facility_mgr.RemoveFacilityAt(2, 5);
    assert(removed_at && "Residential should be removed successfully");
    assert(!residential.is_alive() && "Residential entity should no longer be alive");
    assert(!grid.IsOccupied(2, 5) && "Residential position should no longer be occupied");
    
    new_occupied = grid.GetOccupiedCellCount();
    std::cout << "  Occupied cells after removal: " << new_occupied << std::endl;
    assert(new_occupied == 18 && "Should have 18 occupied cells (24 - 6)");
    
    std::cout << "✓ Facility removed by position successfully" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 6: Attempt to create facility in occupied space" << std::endl;
    
    auto invalid_facility = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        0, 0  // This space is occupied by lobby
    );
    
    assert(!invalid_facility.is_alive() && "Should not create facility in occupied space");
    std::cout << "✓ Correctly prevented placement in occupied space" << std::endl;
    
    PrintSeparator();
    std::cout << "TEST 7: Create facility with custom width" << std::endl;
    
    auto custom_office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        4, 0, 5, "CustomOffice"  // Custom width of 5 instead of default 8
    );
    
    assert(custom_office.is_alive() && "Custom office should be created");
    auto custom_component = custom_office.get<BuildingComponent>();
    assert(custom_component.width == 5 && "Custom width should be 5");
    
    std::cout << "  Created office with custom width: " << custom_component.width << std::endl;
    std::cout << "✓ Custom width facility created successfully" << std::endl;
    
    PrintSeparator();
    std::cout << "\n🎉 ALL TESTS PASSED! 🎉" << std::endl;
    std::cout << "\nFacility System Summary:" << std::endl;
    std::cout << "  ✓ Facility creation works correctly" << std::endl;
    std::cout << "  ✓ Grid placement is accurate" << std::endl;
    std::cout << "  ✓ Facility attributes are set properly" << std::endl;
    std::cout << "  ✓ Removal by entity works" << std::endl;
    std::cout << "  ✓ Removal by position works" << std::endl;
    std::cout << "  ✓ Collision detection prevents invalid placements" << std::endl;
    std::cout << "  ✓ Custom facility parameters are supported" << std::endl;
    
    PrintSeparator();
    
    return 0;
}
