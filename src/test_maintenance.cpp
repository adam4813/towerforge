#include "core/ecs_world.hpp"
#include "core/facility_manager.hpp"
#include "core/components.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace TowerForge::Core;

int main() {
    std::cout << "Testing Maintenance System..." << std::endl;
    
    // Create ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Get facility manager
    FacilityManager& facility_mgr = ecs_world.GetFacilityManager();
    
    // Create a test facility (Office)
    std::cout << "\nCreating test Office facility on Floor 0..." << std::endl;
    flecs::entity office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        0,  // floor
        5,  // column
        8   // width
    );
    
    if (!office.is_alive()) {
        std::cerr << "Failed to create office facility!" << std::endl;
        return 1;
    }
    
    // Verify MaintenanceStatus was added
    if (!office.has<MaintenanceStatus>()) {
        std::cerr << "Office does not have MaintenanceStatus component!" << std::endl;
        return 1;
    }
    
    std::cout << "Office created successfully with MaintenanceStatus component" << std::endl;
    
    // Check initial maintenance status
    const MaintenanceStatus& initial_status = office.ensure<MaintenanceStatus>();
    std::cout << "\nInitial Maintenance Status: " << initial_status.GetStateString() << std::endl;
    std::cout << "Initial Maintenance Percent: " << initial_status.GetMaintenancePercent() << "%" << std::endl;
    std::cout << "Degrade Rate: " << initial_status.degrade_rate << "x" << std::endl;
    
    // Simulate time passing by running updates
    std::cout << "\nSimulating 10 seconds of game time (fast-forward)..." << std::endl;
    for (int i = 0; i < 10; i++) {
        ecs_world.Update(1.0f);  // 1 second per update
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Check status after simulation
    const MaintenanceStatus& after_status = office.ensure<MaintenanceStatus>();
    std::cout << "\nAfter 10 seconds:" << std::endl;
    std::cout << "Maintenance Status: " << after_status.GetStateString() << std::endl;
    std::cout << "Maintenance Percent: " << after_status.GetMaintenancePercent() << "%" << std::endl;
    std::cout << "Time Since Last Service: " << after_status.time_since_last_service << " seconds" << std::endl;
    
    // Test manual repair
    std::cout << "\nTesting manual repair..." << std::endl;
    bool repair_result = facility_mgr.RepairFacility(office);
    if (repair_result) {
        std::cout << "Repair successful!" << std::endl;
        const MaintenanceStatus& repaired_status = office.ensure<MaintenanceStatus>();
        std::cout << "After Repair - Status: " << repaired_status.GetStateString() << std::endl;
        std::cout << "After Repair - Time Since Service: " << repaired_status.time_since_last_service << " seconds" << std::endl;
    } else {
        std::cerr << "Repair failed!" << std::endl;
    }
    
    // Create an elevator to test fast degradation
    std::cout << "\nCreating test Elevator facility..." << std::endl;
    flecs::entity elevator = facility_mgr.CreateFacility(
        BuildingComponent::Type::Elevator,
        0,  // floor
        2,  // column
        2   // width
    );
    
    if (elevator.is_alive() && elevator.has<MaintenanceStatus>()) {
        const MaintenanceStatus& elev_status = elevator.ensure<MaintenanceStatus>();
        std::cout << "Elevator Maintenance Status: " << elev_status.GetStateString() << std::endl;
        std::cout << "Elevator Degrade Rate: " << elev_status.degrade_rate << "x (should be 2.0x)" << std::endl;
    }
    
    // Test facility with high occupancy (faster degradation)
    std::cout << "\nTesting occupancy-based degradation..." << std::endl;
    BuildingComponent& office_building = office.ensure<BuildingComponent>();
    office_building.current_occupancy = office_building.capacity;  // Full occupancy
    std::cout << "Set office to full occupancy: " << office_building.current_occupancy 
              << "/" << office_building.capacity << std::endl;
    
    // Run a few more updates to see faster degradation
    std::cout << "\nSimulating 5 more seconds with full occupancy..." << std::endl;
    for (int i = 0; i < 5; i++) {
        ecs_world.Update(1.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    const MaintenanceStatus& final_status = office.ensure<MaintenanceStatus>();
    std::cout << "Final Maintenance Status: " << final_status.GetStateString() << std::endl;
    std::cout << "Final Time Since Service: " << final_status.time_since_last_service << " seconds" << std::endl;
    
    std::cout << "\nMaintenance system test completed successfully!" << std::endl;
    return 0;
}
