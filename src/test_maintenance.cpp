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
    
    // Test auto-repair functionality
    std::cout << "\n=== Testing Auto-Repair Functionality ===" << std::endl;
    
    // Create a TowerEconomy singleton with initial funds using explicit constructor
    ecs_world.GetWorld().set<TowerEconomy>(TowerEconomy(10000.0f));
    
    std::cout << "Created TowerEconomy singleton with $10,000 initial balance" << std::endl;
    
    // Expand the tower grid to have more floors
    TowerGrid& grid = ecs_world.GetTowerGrid();
    grid.AddFloors(2);  // Add 2 floors above
    std::cout << "Added 2 floors to tower (now has " << grid.GetFloorCount() << " floors)" << std::endl;
    
    // Create a new office and enable auto-repair
    flecs::entity auto_office = facility_mgr.CreateFacility(
        BuildingComponent::Type::Office,
        1,  // floor 1 (should exist now)
        5,  // column
        8   // width
    );
    
    if (auto_office.is_alive()) {
        std::cout << "\nCreated office with auto-repair enabled" << std::endl;
        
        // Enable auto-repair
        facility_mgr.SetAutoRepair(auto_office, true);
        
        MaintenanceStatus& auto_maint = auto_office.ensure<MaintenanceStatus>();
        std::cout << "Auto-repair enabled: " << (auto_maint.auto_repair_enabled ? "YES" : "NO") << std::endl;
        
        // Manually set to broken state for testing
        auto_maint.status = MaintenanceStatus::State::Broken;
        auto_maint.time_since_last_service = 15000.0f;  // Simulate long time without service
        
        std::cout << "Manually set facility to BROKEN state" << std::endl;
        std::cout << "Initial Balance: $" << ecs_world.GetWorld().get<TowerEconomy>().total_balance << std::endl;
        
        // Run an update cycle to trigger auto-repair (system runs every 10 seconds)
        std::cout << "\nRunning updates to trigger auto-repair (waits 10 seconds)..." << std::endl;
        for (int i = 0; i < 12; i++) {
            ecs_world.Update(1.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        // Check if repair happened
        const MaintenanceStatus& after_auto = auto_office.ensure<MaintenanceStatus>();
        const TowerEconomy& after_economy = ecs_world.GetWorld().get<TowerEconomy>();
        
        std::cout << "\nAfter auto-repair:" << std::endl;
        std::cout << "Maintenance Status: " << after_auto.GetStateString() << std::endl;
        std::cout << "Time Since Service: " << after_auto.time_since_last_service << " seconds" << std::endl;
        std::cout << "Tower Balance: $" << after_economy.total_balance << std::endl;
        std::cout << "Total Expenses: $" << after_economy.total_expenses << std::endl;
        
        if (after_auto.status == MaintenanceStatus::State::Good) {
            std::cout << "\n✓ Auto-repair worked successfully!" << std::endl;
        } else {
            std::cout << "\n✗ Auto-repair did not trigger" << std::endl;
        }
    }
    
    std::cout << "\nMaintenance system test completed successfully!" << std::endl;
    return 0;
}
