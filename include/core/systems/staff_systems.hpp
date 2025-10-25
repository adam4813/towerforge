#pragma once

#include <flecs.h>

namespace TowerForge::Core::Systems {

    class StaffSystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterStaffShiftManagement(flecs::world& world);
        static void RegisterStaffCleaning(flecs::world& world);
        static void RegisterStaffMaintenance(flecs::world& world);
        static void RegisterStaffMaintenanceStatus(flecs::world& world);
        static void RegisterStaffFirefighting(flecs::world& world);
        static void RegisterStaffSecurity(flecs::world& world);
        static void RegisterStaffManagerUpdate(flecs::world& world);
        static void RegisterStaffWages(flecs::world& world);
        static void RegisterStaffStatusReporting(flecs::world& world);
    };

}
