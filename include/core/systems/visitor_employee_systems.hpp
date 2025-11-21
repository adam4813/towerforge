#pragma once

#include <flecs.h>

namespace towerforge::core::Systems {

    class VisitorEmployeeSystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterResearchPointsGeneration(flecs::world& world);
        static void RegisterVisitorNeedsGrowth(flecs::world& world);
        static void RegisterVisitorNeedsBehavior(flecs::world& world);
        static void RegisterVisitorFacilityInteraction(flecs::world& world);
        static void RegisterVisitorSatisfaction(flecs::world& world);
        static void RegisterVisitorBehavior(flecs::world& world);
        static void RegisterVisitorNeedsDisplay(flecs::world& world);
        static void RegisterEmployeeShiftManagement(flecs::world& world);
        static void RegisterEmployeeOffDutyVisitor(flecs::world& world);
        static void RegisterJobOpeningTracking(flecs::world& world);
        static void RegisterVisitorSpawning(flecs::world& world);
        static void RegisterJobAssignment(flecs::world& world);
        static void RegisterVisitorCleanup(flecs::world& world);
    };

}
