#pragma once

#include <flecs.h>

namespace towerforge::core::Systems {

    class PersonElevatorSystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterPersonHorizontalMovement(flecs::world& world);
        static void RegisterPersonWaiting(flecs::world& world);
        static void RegisterPersonElevatorRiding(flecs::world& world);
        static void RegisterPersonStateLogging(flecs::world& world);
        static void RegisterElevatorCarMovement(flecs::world& world);
        static void RegisterElevatorCall(flecs::world& world);
        static void RegisterPersonElevatorBoarding(flecs::world& world);
        static void RegisterElevatorLogging(flecs::world& world);
    };

}
