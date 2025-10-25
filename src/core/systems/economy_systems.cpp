#include "core/systems/economy_systems.hpp"
#include "core/components.hpp"
#include <iostream>

namespace TowerForge::Core::Systems {

    void EconomySystems::RegisterAll(flecs::world& world) {
        RegisterSatisfactionUpdate(world);
        RegisterSatisfactionReporting(world);
        RegisterFacilityEconomicsUpdate(world);
        RegisterDailyEconomyProcessing(world);
        RegisterRevenueCollection(world);
        RegisterEconomicStatusReporting(world);
    }

    void EconomySystems::RegisterSatisfactionUpdate(flecs::world& world) {
        world.system<Satisfaction, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](flecs::entity e, Satisfaction& satisfaction, const BuildingComponent& facility) {
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / facility.capacity;
                    if (occupancy_rate > 0.9f) {
                        satisfaction.crowding_penalty = (occupancy_rate - 0.9f) * 50.0f;
                    } else if (occupancy_rate < 0.3f) {
                        satisfaction.crowding_penalty = (0.3f - occupancy_rate) * 10.0f;
                    } else {
                        satisfaction.crowding_penalty *= 0.9f;
                    }
            
                    if (facility.type == BuildingComponent::Type::Restaurant || 
                        facility.type == BuildingComponent::Type::RetailShop) {
                        satisfaction.noise_penalty = occupancy_rate * 5.0f;
                    }
            
                    switch (facility.type) {
                        case BuildingComponent::Type::Hotel:
                            satisfaction.quality_bonus = 15.0f;
                            break;
                        case BuildingComponent::Type::Restaurant:
                            satisfaction.quality_bonus = 10.0f;
                            break;
                        case BuildingComponent::Type::Office:
                            satisfaction.quality_bonus = 5.0f;
                            break;
                        default:
                            satisfaction.quality_bonus = 2.0f;
                            break;
                    }
            
                    satisfaction.UpdateScore();
                });
    }

    void EconomySystems::RegisterSatisfactionReporting(flecs::world& world) {
        world.system<const Satisfaction, const Actor>()
                .kind(flecs::OnUpdate)
                .interval(15.0f)
                .each([](flecs::entity e, const Satisfaction& satisfaction, const Actor& actor) {
                    std::cout << "  [Satisfaction] " << actor.name 
                            << ": " << static_cast<int>(satisfaction.satisfaction_score) << "% ("
                            << satisfaction.GetLevelString() << ")"
                            << std::endl;
                });
    }

    void EconomySystems::RegisterFacilityEconomicsUpdate(flecs::world& world) {
        world.system<FacilityEconomics, const BuildingComponent, const Satisfaction>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](flecs::entity e, FacilityEconomics& economics, 
                         const BuildingComponent& facility, const Satisfaction& satisfaction) {
                    if (satisfaction.satisfaction_score > 70.0f && 
                        economics.current_tenants < economics.max_tenants) {
                        if (facility.current_occupancy < facility.capacity) {
                            economics.current_tenants++;
                        }
                    } else if (satisfaction.satisfaction_score < 30.0f && 
                               economics.current_tenants > 0) {
                        economics.current_tenants--;
                    }
            
                    economics.quality_multiplier = 0.5f + (satisfaction.satisfaction_score / 100.0f) * 1.5f;
                });
    }

    void EconomySystems::RegisterDailyEconomyProcessing(flecs::world& world) {
        world.system<TowerEconomy, const TimeManager>()
                .kind(flecs::OnUpdate)
                .each([](flecs::entity e, TowerEconomy& economy, const TimeManager& time_mgr) {
                    const int current_day = time_mgr.current_week * 7 + time_mgr.current_day;
            
                    if (current_day != economy.last_processed_day) {
                        if (economy.last_processed_day >= 0) {
                            economy.ProcessDailyTransactions();
                    
                            std::cout << "  === Daily Economics Report ===" << std::endl;
                            std::cout << "  Day Revenue: $" << economy.daily_revenue << std::endl;
                            std::cout << "  Day Expenses: $" << economy.daily_expenses << std::endl;
                            std::cout << "  Balance: $" << economy.total_balance << std::endl;
                            std::cout << "  ==============================" << std::endl;
                        }
                        economy.last_processed_day = current_day;
                    }
                });
    }

    void EconomySystems::RegisterRevenueCollection(flecs::world& world) {
        world.system<const FacilityEconomics>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, const FacilityEconomics& economics) {
                    auto& mut_economy = e.world().ensure<TowerEconomy>();
            
                    const float revenue_per_second = economics.CalculateDailyRevenue() / (24.0f * 3600.0f);
                    mut_economy.daily_revenue += revenue_per_second;
            
                    const float cost_per_second = economics.operating_cost / (24.0f * 3600.0f);
                    mut_economy.daily_expenses += cost_per_second;
                });
    }

    void EconomySystems::RegisterEconomicStatusReporting(flecs::world& world) {
        world.system<const FacilityEconomics, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(20.0f)
                .each([](flecs::entity e, const FacilityEconomics& economics, 
                         const BuildingComponent& facility) {
                    auto type_name = "Unknown";
                    switch(facility.type) {
                        case BuildingComponent::Type::Office:      type_name = "Office"; break;
                        case BuildingComponent::Type::Restaurant:  type_name = "Restaurant"; break;
                        case BuildingComponent::Type::RetailShop:  type_name = "RetailShop"; break;
                        case BuildingComponent::Type::Residential: type_name = "Residential"; break;
                        case BuildingComponent::Type::Hotel:       type_name = "Hotel"; break;
                        case BuildingComponent::Type::Elevator:    type_name = "Elevator"; break;
                        case BuildingComponent::Type::Lobby:       type_name = "Lobby"; break;
                    }
            
                    std::cout << "  [Economics] " << type_name << " Floor " << facility.floor
                            << ": Occupancy " << static_cast<int>(economics.GetOccupancyRate()) << "%"
                            << ", Daily Profit: $" << static_cast<int>(economics.CalculateNetProfit())
                            << " (Quality: " << static_cast<int>(economics.quality_multiplier * 100) << "%)"
                            << std::endl;
                });
    }

}
