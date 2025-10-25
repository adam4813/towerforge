#pragma once

#include <flecs.h>

namespace TowerForge::Core::Systems {

    class FacilitySystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterFacilityStatusDegradation(flecs::world& world);
        static void RegisterCleanlinessDegradation(flecs::world& world);
        static void RegisterMaintenanceDegradation(flecs::world& world);
        static void RegisterMaintenanceNotification(flecs::world& world);
        static void RegisterCleanlinessNotification(flecs::world& world);
        static void RegisterFacilityStatusImpact(flecs::world& world);
        static void RegisterCleanlinessImpact(flecs::world& world);
        static void RegisterBrokenFacilityImpact(flecs::world& world);
        static void RegisterAutoRepair(flecs::world& world);
    };

}
