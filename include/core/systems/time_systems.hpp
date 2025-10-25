#pragma once

#include <flecs.h>

namespace TowerForge::Core::Systems {

    class TimeSystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterTimeSimulation(flecs::world& world);
        static void RegisterScheduleExecution(flecs::world& world);
        static void RegisterTimeLogging(flecs::world& world);
    };

}
