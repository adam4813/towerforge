#pragma once

#include <flecs.h>

namespace towerforge::core::Systems {

    class EconomySystems {
    public:
        static void RegisterAll(flecs::world& world);
    
    private:
        static void RegisterSatisfactionUpdate(flecs::world& world);
        static void RegisterSatisfactionReporting(flecs::world& world);
        static void RegisterFacilityEconomicsUpdate(flecs::world& world);
        static void RegisterDailyEconomyProcessing(flecs::world& world);
        static void RegisterRevenueCollection(flecs::world& world);
        static void RegisterEconomicStatusReporting(flecs::world& world);
    };

}
