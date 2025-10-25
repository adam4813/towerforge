#pragma once

#include <flecs.h>

namespace TowerForge::Core::Systems {

    class MovementSystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterPositionUpdate(flecs::world& world);
        static void RegisterActorLogging(flecs::world& world);
        static void RegisterBuildingOccupancyMonitor(flecs::world& world);
    };

}
