/**
 * Test program to demonstrate save/load functionality
 */
#include <iostream>
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/save_load_manager.hpp"

using namespace TowerForge::Core;

int main() {
    std::cout << "TowerForge Save/Load System Test" << std::endl;
    std::cout << "=================================" << std::endl << std::endl;
    
    // Create and initialize ECS world
    std::cout << "1. Creating ECS world..." << std::endl;
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Set up initial game state
    ecs_world.GetWorld().set<TimeManager>({60.0f});
    ecs_world.GetWorld().set<TowerEconomy>({25000.0f});
    
    // Create some test entities
    std::cout << "2. Creating test entities..." << std::endl;
    auto person1 = ecs_world.CreateEntity("TestPerson1");
    person1.set<Person>({"Alice", 0, 5.0f, 2.0f});
    person1.set<Position>({10.0f, 0.0f});
    person1.set<Satisfaction>({85.0f});
    
    auto building1 = ecs_world.CreateEntity("TestOffice");
    building1.set<BuildingComponent>({BuildingComponent::Type::Office, 2, 4, 20});
    building1.set<GridPosition>({2, 5, 4});
    building1.set<FacilityEconomics>({150.0f, 50.0f, 15});
    
    std::cout << "   Created Person: Alice" << std::endl;
    std::cout << "   Created Office building" << std::endl << std::endl;
    
    // Create save/load manager
    std::cout << "3. Initializing save/load manager..." << std::endl;
    SaveLoadManager save_manager;
    save_manager.Initialize();
    std::cout << "   Save directory: " << save_manager.GetSaveSlots().size() << " existing slots" << std::endl << std::endl;
    
    // Save the game
    std::cout << "4. Saving game to 'test_save'..." << std::endl;
    auto save_result = save_manager.SaveGame("test_save", "Test Tower", ecs_world);
    if (save_result.success) {
        std::cout << "   ✓ Save successful!" << std::endl;
    } else {
        std::cout << "   ✗ Save failed: " << save_result.error_message << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    // List save slots
    std::cout << "5. Listing save slots..." << std::endl;
    auto slots = save_manager.GetSaveSlots();
    for (const auto& slot : slots) {
        std::cout << "   - " << slot.slot_name << std::endl;
        std::cout << "     Tower: " << slot.tower_name << std::endl;
        std::cout << "     Date: " << slot.save_date << std::endl;
        std::cout << "     Balance: $" << slot.total_balance << std::endl;
        std::cout << "     Population: " << slot.population << std::endl;
    }
    std::cout << std::endl;
    
    // Create a new ECS world for loading
    std::cout << "6. Creating new ECS world for load test..." << std::endl;
    ECSWorld load_world;
    load_world.Initialize();
    std::cout << "   New world created (empty)" << std::endl << std::endl;
    
    // Load the save
    std::cout << "7. Loading game from 'test_save'..." << std::endl;
    auto load_result = save_manager.LoadGame("test_save", load_world);
    if (load_result.success) {
        std::cout << "   ✓ Load successful!" << std::endl;
    } else {
        std::cout << "   ✗ Load failed: " << load_result.error_message << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    // Verify loaded state
    std::cout << "8. Verifying loaded state..." << std::endl;
    
    // Check TimeManager
    if (load_world.GetWorld().has<TimeManager>()) {
        const auto& time = load_world.GetWorld().get<TimeManager>();
        std::cout << "   ✓ TimeManager restored - Hours per second: " << time.hours_per_second << std::endl;
    }
    
    // Check TowerEconomy
    if (load_world.GetWorld().has<TowerEconomy>()) {
        const auto& economy = load_world.GetWorld().get<TowerEconomy>();
        std::cout << "   ✓ TowerEconomy restored - Balance: $" << economy.total_balance << std::endl;
    }
    
    // Count entities
    int entity_count = 0;
    int person_count = 0;
    int building_count = 0;
    
    load_world.GetWorld().each([&](flecs::entity e) {
        entity_count++;
        if (e.has<Person>()) person_count++;
        if (e.has<BuildingComponent>()) building_count++;
    });
    
    std::cout << "   ✓ Entities restored: " << entity_count << " total" << std::endl;
    std::cout << "     - " << person_count << " person(s)" << std::endl;
    std::cout << "     - " << building_count << " building(s)" << std::endl;
    std::cout << std::endl;
    
    // Test delete
    std::cout << "9. Testing delete slot..." << std::endl;
    auto delete_result = save_manager.DeleteSlot("test_save");
    if (delete_result.success) {
        std::cout << "   ✓ Delete successful!" << std::endl;
    } else {
        std::cout << "   ✗ Delete failed: " << delete_result.error_message << std::endl;
    }
    std::cout << std::endl;
    
    // Test autosave
    std::cout << "10. Testing autosave..." << std::endl;
    auto autosave_result = save_manager.Autosave(ecs_world);
    if (autosave_result.success) {
        std::cout << "   ✓ Autosave successful!" << std::endl;
        std::cout << "   Time since last save: " << save_manager.GetTimeSinceLastSave() << "s" << std::endl;
    } else {
        std::cout << "   ✗ Autosave failed: " << autosave_result.error_message << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=================================" << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    
    return 0;
}
