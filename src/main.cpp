#include <iostream>
#include "core/ecs_world.hpp"
#include "core/components.hpp"

using namespace TowerForge::Core;

int main(int argc, char* argv[]) {
    std::cout << "TowerForge - Tower Simulation Game" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << std::endl;
    
    // Create and initialize the ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    std::cout << std::endl << "Creating example entities..." << std::endl;
    
    // Create some example actors (people)
    auto actor1 = ecs_world.CreateEntity("John");
    actor1.set<Position>({10.0f, 0.0f});
    actor1.set<Velocity>({0.5f, 0.0f});
    actor1.set<Actor>({"John", 5, 1.0f});
    
    auto actor2 = ecs_world.CreateEntity("Sarah");
    actor2.set<Position>({20.0f, 0.0f});
    actor2.set<Velocity>({-0.3f, 0.0f});
    actor2.set<Actor>({"Sarah", 3, 0.8f});
    
    // Create some example building components
    auto lobby = ecs_world.CreateEntity("Lobby");
    lobby.set<Position>({0.0f, 0.0f});
    lobby.set<BuildingComponent>({BuildingComponent::Type::Lobby, 0, 10, 50});
    
    auto office1 = ecs_world.CreateEntity("Office_Floor_5");
    office1.set<Position>({0.0f, 50.0f});
    office1.set<BuildingComponent>({BuildingComponent::Type::Office, 5, 8, 20});
    
    auto restaurant = ecs_world.CreateEntity("Restaurant_Floor_3");
    restaurant.set<Position>({0.0f, 30.0f});
    restaurant.set<BuildingComponent>({BuildingComponent::Type::Restaurant, 3, 6, 30});
    
    std::cout << "  Created 2 actors and 3 building components" << std::endl;
    std::cout << std::endl << "Running simulation..." << std::endl;
    std::cout << std::endl;
    
    // Run the simulation for 30 seconds (simulated time)
    const float time_step = 1.0f / 60.0f;  // 60 FPS
    const float total_time = 30.0f;
    float elapsed_time = 0.0f;
    
    while (elapsed_time < total_time) {
        if (!ecs_world.Update(time_step)) {
            break;
        }
        elapsed_time += time_step;
    }
    
    std::cout << std::endl << "Simulation completed after " << elapsed_time << " seconds" << std::endl;
    std::cout << "ECS test successful!" << std::endl;
    
    return 0;
}

