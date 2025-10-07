#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"
#include <iostream>

namespace TowerForge {
namespace Core {

ECSWorld::ECSWorld() 
    : tower_grid_(std::make_unique<TowerGrid>(10, 20)) {  // 10 floors, 20 columns initial
}

ECSWorld::~ECSWorld() {
}

void ECSWorld::Initialize() {
    std::cout << "Initializing ECS World..." << std::endl;
    
    RegisterComponents();
    RegisterSystems();
    
    std::cout << "ECS World initialized successfully" << std::endl;
}

bool ECSWorld::Update(float delta_time) {
    // Run one frame of the simulation
    // The delta_time is passed as a singleton to systems that need it
    world_.set<float>({delta_time});
    
    // Progress the world by one frame
    // This will execute all systems in the correct order
    return world_.progress(delta_time);
}

flecs::world& ECSWorld::GetWorld() {
    return world_;
}

flecs::entity ECSWorld::CreateEntity(const char* name) {
    if (name) {
        return world_.entity(name);
    }
    return world_.entity();
}

TowerGrid& ECSWorld::GetTowerGrid() {
    return *tower_grid_;
}

void ECSWorld::RegisterComponents() {
    // Register components with the ECS
    // This allows flecs to track and manage component metadata
    world_.component<Position>();
    world_.component<Velocity>();
    world_.component<Actor>();
    world_.component<BuildingComponent>();
    world_.component<GridPosition>();
    
    std::cout << "  Registered components: Position, Velocity, Actor, BuildingComponent, GridPosition" << std::endl;
}

void ECSWorld::RegisterSystems() {
    // Example system: Movement system that updates positions based on velocity
    // This system runs every frame for all entities that have both Position and Velocity
    world_.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& pos, const Velocity& vel) {
            pos.x += vel.dx;
            pos.y += vel.dy;
        });
    
    // Example system: Actor logging system (demonstrates querying actors)
    // This system runs once per frame to log active actors
    world_.system<const Actor, const Position>()
        .kind(flecs::OnUpdate)
        .interval(5.0f)  // Run every 5 seconds
        .each([](flecs::entity e, const Actor& actor, const Position& pos) {
            std::cout << "  Actor '" << actor.name 
                      << "' at position (" << pos.x << ", " << pos.y 
                      << "), destination floor: " << actor.floor_destination 
                      << std::endl;
        });
    
    // Example system: Building occupancy monitor
    // This system monitors building components and reports their status
    world_.system<const BuildingComponent>()
        .kind(flecs::OnUpdate)
        .interval(10.0f)  // Run every 10 seconds
        .each([](flecs::entity e, const BuildingComponent& component) {
            const char* type_name = "Unknown";
            switch(component.type) {
                case BuildingComponent::Type::Office:     type_name = "Office"; break;
                case BuildingComponent::Type::Restaurant: type_name = "Restaurant"; break;
                case BuildingComponent::Type::Shop:       type_name = "Shop"; break;
                case BuildingComponent::Type::Hotel:      type_name = "Hotel"; break;
                case BuildingComponent::Type::Elevator:   type_name = "Elevator"; break;
                case BuildingComponent::Type::Lobby:      type_name = "Lobby"; break;
            }
            
            std::cout << "  " << type_name 
                      << " on floor " << component.floor 
                      << ": " << component.current_occupancy 
                      << "/" << component.capacity << " occupied"
                      << std::endl;
        });
    
    std::cout << "  Registered systems: Movement, Actor Logging, Building Occupancy Monitor" << std::endl;
}

} // namespace Core
} // namespace TowerForge
