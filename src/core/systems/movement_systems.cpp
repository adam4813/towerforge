#include "core/systems/movement_systems.hpp"
#include "core/components.hpp"
#include "core/facility_manager.hpp"
#include <iostream>

namespace towerforge::core::Systems {

    void MovementSystems::RegisterAll(flecs::world& world) {
        RegisterPositionUpdate(world);
        RegisterActorLogging(world);
        RegisterBuildingOccupancyMonitor(world);
    }

    void MovementSystems::RegisterPositionUpdate(flecs::world& world) {
        world.system<Position, const Velocity>()
                .each([](flecs::entity e, Position& pos, const Velocity& vel) {
                    pos.x += vel.dx;
                    pos.y += vel.dy;
                });
    }

    void MovementSystems::RegisterActorLogging(flecs::world& world) {
        world.system<const Actor, const Position>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](flecs::entity e, const Actor& actor, const Position& pos) {
                    std::cout << "  Actor '" << actor.name 
                            << "' at position (" << pos.x << ", " << pos.y 
                            << "), destination floor: " << actor.floor_destination 
                            << std::endl;
                });
    }

    void MovementSystems::RegisterBuildingOccupancyMonitor(flecs::world& world) {
        world.system<const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)
                .each([](flecs::entity e, const BuildingComponent& component) {
                    const char* type_name = FacilityManager::GetTypeName(component.type);
            
                    std::cout << "  " << type_name 
                            << " on floor " << component.floor 
                            << ": " << component.current_occupancy 
                            << "/" << component.capacity << " occupied"
                            << std::endl;
                });
    }

}
