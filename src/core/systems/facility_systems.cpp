#include "core/systems/facility_systems.hpp"
#include "core/components.hpp"
#include "core/facility_manager.hpp"
#include <iostream>
#include <set>
#include <algorithm>

namespace towerforge::core::Systems {

    void FacilitySystems::RegisterAll(flecs::world& world) {
        RegisterFacilityStatusDegradation(world);
        RegisterCleanlinessDegradation(world);
        RegisterMaintenanceDegradation(world);
        RegisterMaintenanceNotification(world);
        RegisterCleanlinessNotification(world);
        RegisterFacilityStatusImpact(world);
        RegisterCleanlinessImpact(world);
        RegisterBrokenFacilityImpact(world);
        RegisterAutoRepair(world);
    }

    void FacilitySystems::RegisterFacilityStatusDegradation(flecs::world& world) {
        world.system<FacilityStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, FacilityStatus& status, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                    status.Update(delta_time, facility.current_occupancy);
                });
    }

    void FacilitySystems::RegisterCleanlinessDegradation(flecs::world& world) {
        world.system<CleanlinessStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / 
                                                 std::max(1, facility.capacity);
                    const float occupancy_factor = 1.0f + (occupancy_rate * 2.0f);
                
                    cleanliness.Update(delta_time, occupancy_factor);
                });
    }

    void FacilitySystems::RegisterMaintenanceDegradation(flecs::world& world) {
        world.system<MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / 
                                                 std::max(1, facility.capacity);
                    const float usage_factor = 1.0f + (occupancy_rate * 1.5f);
                
                    maintenance.Update(delta_time, usage_factor);
                });
    }

    void FacilitySystems::RegisterMaintenanceNotification(flecs::world& world) {
        world.system<const MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity e, const MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    static std::set<flecs::entity_t> notified_broken;
                    static std::set<flecs::entity_t> notified_needs_service;
                
                    const auto entity_id = e.id();
                
                    if (maintenance.status == MaintenanceStatus::State::Broken) {
                        if (notified_broken.find(entity_id) == notified_broken.end()) {
                            notified_broken.insert(entity_id);
                            notified_needs_service.erase(entity_id);
                        
                            const char* facility_type = FacilityManager::GetTypeName(facility.type);
                            std::cout << "  [NOTIFICATION] " << facility_type << " on Floor " << facility.floor 
                                    << " is broken! It needs repairs." << std::endl;
                        }
                    } else if (maintenance.status == MaintenanceStatus::State::NeedsService) {
                        if (notified_needs_service.find(entity_id) == notified_needs_service.end()) {
                            notified_needs_service.insert(entity_id);
                        
                            const char* facility_type = FacilityManager::GetTypeName(facility.type);
                            std::cout << "  [NOTIFICATION] " << facility_type << " on Floor " << facility.floor 
                                    << " needs maintenance service." << std::endl;
                        }
                    } else if (maintenance.status == MaintenanceStatus::State::Good) {
                        notified_broken.erase(entity_id);
                        notified_needs_service.erase(entity_id);
                    }
                });
    }

    void FacilitySystems::RegisterCleanlinessNotification(flecs::world& world) {
        world.system<const CleanlinessStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity e, const CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                    static std::set<flecs::entity_t> notified_dirty;
                    static std::set<flecs::entity_t> notified_needs_cleaning;
                
                    const auto entity_id = e.id();
                
                    if (cleanliness.status == CleanlinessStatus::State::Dirty) {
                        if (notified_dirty.find(entity_id) == notified_dirty.end()) {
                            notified_dirty.insert(entity_id);
                            notified_needs_cleaning.erase(entity_id);
                        
                            const char* facility_type = FacilityManager::GetTypeName(facility.type);
                            std::cout << "  [NOTIFICATION] " << facility_type << " on Floor " << facility.floor 
                                    << " is dirty! Guests are unhappy." << std::endl;
                        }
                    } else if (cleanliness.status == CleanlinessStatus::State::NeedsCleaning) {
                        if (notified_needs_cleaning.find(entity_id) == notified_needs_cleaning.end()) {
                            notified_needs_cleaning.insert(entity_id);
                        
                            const char* facility_type = FacilityManager::GetTypeName(facility.type);
                            std::cout << "  [NOTIFICATION] " << facility_type << " on Floor " << facility.floor 
                                    << " needs cleaning." << std::endl;
                        }
                    } else if (cleanliness.status == CleanlinessStatus::State::Clean) {
                        notified_dirty.erase(entity_id);
                        notified_needs_cleaning.erase(entity_id);
                    }
                });
    }

    void FacilitySystems::RegisterFacilityStatusImpact(flecs::world& world) {
        world.system<Satisfaction, const FacilityStatus>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity e, Satisfaction& satisfaction, const FacilityStatus& status) {
                    if (status.cleanliness < 50.0f) {
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    }
                    if (status.maintenance_level < 50.0f) {
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    }
                
                    if (status.cleanliness >= 90.0f && status.maintenance_level >= 90.0f) {
                        satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.5f);
                    }
                });
    }

    void FacilitySystems::RegisterCleanlinessImpact(flecs::world& world) {
        world.system<Satisfaction, const CleanlinessStatus>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity e, Satisfaction& satisfaction, const CleanlinessStatus& cleanliness) {
                    switch (cleanliness.status) {
                        case CleanlinessStatus::State::Dirty:
                            satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 2.0f);
                            break;
                        case CleanlinessStatus::State::NeedsCleaning:
                            satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 0.5f);
                            break;
                        case CleanlinessStatus::State::Clean:
                            satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.3f);
                            break;
                    }
                });
    }

    void FacilitySystems::RegisterBrokenFacilityImpact(flecs::world& world) {
        world.system<const MaintenanceStatus, BuildingComponent, Satisfaction>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity e, const MaintenanceStatus& maintenance, 
                         BuildingComponent& facility, Satisfaction& satisfaction) {
                    if (maintenance.IsBroken()) {
                        if (facility.current_occupancy > 0) {
                            facility.current_occupancy = 0;
                        }
                    
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 3.0f);
                    } else if (maintenance.status == MaintenanceStatus::State::NeedsService) {
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    } else if (maintenance.status == MaintenanceStatus::State::Good) {
                        satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.2f);
                    }
                });
    }

    void FacilitySystems::RegisterAutoRepair(flecs::world& world) {
        world.system<MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)
                .each([](const flecs::entity e, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    if (!maintenance.auto_repair_enabled) return;
                    if (!maintenance.NeedsService()) return;
                
                    auto world = e.world();
                    if (!world.has<TowerEconomy>()) return;
                
                    const TowerEconomy& economy = world.get<TowerEconomy>();
                
                    constexpr float BASE_REPAIR_COST = 100.0f;
                    constexpr float BROKEN_REPAIR_COST = 500.0f;
                    constexpr float ELEVATOR_COST_MULTIPLIER = 2.0f;
                    constexpr float COMPLEX_FACILITY_MULTIPLIER = 1.5f;
                
                    float repair_cost = maintenance.IsBroken() ? BROKEN_REPAIR_COST : BASE_REPAIR_COST;
                
                    switch (facility.type) {
                        case BuildingComponent::Type::Elevator:
                            repair_cost *= ELEVATOR_COST_MULTIPLIER;
                            break;
                        case BuildingComponent::Type::Hotel:
                        case BuildingComponent::Type::Restaurant:
                        case BuildingComponent::Type::Theater:
                            repair_cost *= COMPLEX_FACILITY_MULTIPLIER;
                            break;
                        default:
                            break;
                    }
                
                    if (economy.total_balance >= repair_cost) {
                        auto& mut_economy = world.ensure<TowerEconomy>();
                        mut_economy.total_balance -= repair_cost;
                        mut_economy.total_expenses += repair_cost;
                    
                        maintenance.Repair();
                    
                        const char* facility_type = FacilityManager::GetTypeName(facility.type);
                        std::cout << "  [Auto-Repair] " << facility_type << " on Floor " << facility.floor 
                                << " repaired automatically (Cost: $" << static_cast<int>(repair_cost) << ")" << std::endl;
                    }
                });
    }

}
