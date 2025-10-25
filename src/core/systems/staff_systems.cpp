#include "core/systems/staff_systems.hpp"
#include "core/components.hpp"
#include "core/facility_manager.hpp"
#include <iostream>

namespace TowerForge::Core::Systems {

    void StaffSystems::RegisterAll(flecs::world& world) {
        RegisterStaffShiftManagement(world);
        RegisterStaffCleaning(world);
        RegisterStaffMaintenance(world);
        RegisterStaffMaintenanceStatus(world);
        RegisterStaffFirefighting(world);
        RegisterStaffSecurity(world);
        RegisterStaffManagerUpdate(world);
        RegisterStaffWages(world);
        RegisterStaffStatusReporting(world);
    }

    void StaffSystems::RegisterStaffShiftManagement(flecs::world& world) {
        world.system<StaffAssignment, Person>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, StaffAssignment& assignment, Person& person) {
                    const auto& time_mgr = e.world().get<TimeManager>();
                    const bool should_be_working = assignment.ShouldBeWorking(time_mgr.current_hour);
                
                    if (should_be_working && !assignment.is_active) {
                        assignment.is_active = true;
                        person.current_need = "Working";
                        std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() 
                                << ") started shift" << std::endl;
                    } else if (!should_be_working && assignment.is_active) {
                        assignment.is_active = false;
                        person.current_need = "Off Duty";
                        std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() 
                                << ") ended shift" << std::endl;
                    }
                });
    }

    void StaffSystems::RegisterStaffCleaning(flecs::world& world) {
        world.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    if (!assignment.DoesCleaningWork()) return;
                
                    auto world = staff_entity.world();
                    bool cleaned_something = false;
                
                    world.each([&](const flecs::entity facility_entity, CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                            if (cleaned_something) return;
                        
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            if (cleanliness.NeedsCleaning()) {
                                cleanliness.Clean();
                                cleaned_something = true;
                            
                                const char* facility_type = "Facility";
                                switch (facility.type) {
                                    case BuildingComponent::Type::Office: facility_type = "Office"; break;
                                    case BuildingComponent::Type::Restaurant: facility_type = "Restaurant"; break;
                                    case BuildingComponent::Type::RetailShop: facility_type = "Retail Shop"; break;
                                    case BuildingComponent::Type::Arcade: facility_type = "Arcade"; break;
                                    case BuildingComponent::Type::Gym: facility_type = "Gym"; break;
                                    case BuildingComponent::Type::Hotel: facility_type = "Hotel"; break;
                                    case BuildingComponent::Type::Theater: facility_type = "Theater"; break;
                                    case BuildingComponent::Type::FlagshipStore: facility_type = "Flagship Store"; break;
                                    default: break;
                                }
                            
                                std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") cleaned " 
                                        << facility_type << " on Floor " << facility.floor 
                                        << " (Status: " << cleanliness.GetStateString() << ")" << std::endl;
                            }
                        });
                
                    if (!cleaned_something) {
                        world.each([&](const flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                                if (cleaned_something) return;
                            
                                bool is_assigned = false;
                                if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                    is_assigned = true;
                                } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                    is_assigned = true;
                                }
                            
                                if (!is_assigned) return;
                            
                                if (status.NeedsCleaning()) {
                                    status.Clean(assignment.work_efficiency);
                                    cleaned_something = true;
                                
                                    const char* facility_type = "Facility";
                                    switch (facility.type) {
                                        case BuildingComponent::Type::Office: facility_type = "Office"; break;
                                        case BuildingComponent::Type::Restaurant: facility_type = "Restaurant"; break;
                                        case BuildingComponent::Type::RetailShop: facility_type = "Retail Shop"; break;
                                        case BuildingComponent::Type::Arcade: facility_type = "Arcade"; break;
                                        case BuildingComponent::Type::Gym: facility_type = "Gym"; break;
                                        case BuildingComponent::Type::Hotel: facility_type = "Hotel"; break;
                                        default: break;
                                    }
                                
                                    std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") cleaned " 
                                            << facility_type << " on Floor " << facility.floor 
                                            << " (Cleanliness: " << static_cast<int>(status.cleanliness) << "%)" << std::endl;
                                }
                            });
                    }
                });
    }

    void StaffSystems::RegisterStaffMaintenance(flecs::world& world) {
        world.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(8.0f)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    if (!assignment.DoesMaintenanceWork()) return;
                
                    auto world = staff_entity.world();
                    bool maintained_something = false;
                
                    world.each([&](const flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                            if (maintained_something) return;
                        
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            if (status.NeedsMaintenance()) {
                                status.Maintain(assignment.work_efficiency);
                                maintained_something = true;
                            
                                const char* facility_type = "Facility";
                                switch (facility.type) {
                                    case BuildingComponent::Type::Office: facility_type = "Office"; break;
                                    case BuildingComponent::Type::Restaurant: facility_type = "Restaurant"; break;
                                    case BuildingComponent::Type::RetailShop: facility_type = "Retail Shop"; break;
                                    case BuildingComponent::Type::Arcade: facility_type = "Arcade"; break;
                                    case BuildingComponent::Type::Gym: facility_type = "Gym"; break;
                                    case BuildingComponent::Type::Hotel: facility_type = "Hotel"; break;
                                    default: break;
                                }
                            
                                std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") performed maintenance on " 
                                        << facility_type << " on Floor " << facility.floor 
                                        << " (Maintenance: " << static_cast<int>(status.maintenance_level) << "%)" << std::endl;
                            }
                        });
                });
    }

    void StaffSystems::RegisterStaffMaintenanceStatus(flecs::world& world) {
        world.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(8.0f)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    if (!assignment.DoesMaintenanceWork()) return;
                
                    auto world = staff_entity.world();
                    bool repaired_something = false;
                
                    world.each([&](const flecs::entity facility_entity, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                            if (repaired_something) return;
                        
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            if (maintenance.NeedsService()) {
                                maintenance.Repair();
                                repaired_something = true;
                            
                                const char* facility_type = FacilityManager::GetTypeName(facility.type);
                            
                                std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") repaired " 
                                        << facility_type << " on Floor " << facility.floor 
                                        << " (Status: " << maintenance.GetStateString() << ")" << std::endl;
                            }
                        });
                });
    }

    void StaffSystems::RegisterStaffFirefighting(flecs::world& world) {
        world.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    if (assignment.role != StaffRole::Firefighter && !assignment.DoesEmergencyWork()) return;
                
                    auto world = staff_entity.world();
                    bool extinguished_fire = false;
                
                    world.each([&](flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                            if (extinguished_fire) return;
                        
                            if (status.has_fire) {
                                status.ExtinguishFire();
                                extinguished_fire = true;
                            
                                const char* facility_type = "Facility";
                                switch (facility.type) {
                                    case BuildingComponent::Type::Office: facility_type = "Office"; break;
                                    case BuildingComponent::Type::Restaurant: facility_type = "Restaurant"; break;
                                    case BuildingComponent::Type::RetailShop: facility_type = "Retail Shop"; break;
                                    default: break;
                                }
                            
                                std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") extinguished fire at " 
                                        << facility_type << " on Floor " << facility.floor << std::endl;
                            }
                        });
                });
    }

    void StaffSystems::RegisterStaffSecurity(flecs::world& world) {
        world.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(3.0f)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    if (assignment.role != StaffRole::Security && !assignment.DoesEmergencyWork()) return;
                
                    auto world = staff_entity.world();
                    bool resolved_issue = false;
                
                    world.each([&](flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                            if (resolved_issue) return;
                        
                            if (status.has_security_issue) {
                                status.ResolveSecurityIssue();
                                resolved_issue = true;
                            
                                const char* facility_type = "Facility";
                                switch (facility.type) {
                                    case BuildingComponent::Type::RetailShop: facility_type = "Retail Shop"; break;
                                    case BuildingComponent::Type::FlagshipStore: facility_type = "Flagship Store"; break;
                                    default: facility_type = "Facility"; break;
                                }
                            
                                std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() << ") resolved security issue at " 
                                        << facility_type << " on Floor " << facility.floor << std::endl;
                            }
                        });
                });
    }

    void StaffSystems::RegisterStaffManagerUpdate(flecs::world& world) {
        world.system<StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity e, StaffManager& manager) {
                    auto world = e.world();
                
                    manager.total_staff_count = 0;
                    manager.firefighters = 0;
                    manager.security_guards = 0;
                    manager.janitors = 0;
                    manager.maintenance_staff = 0;
                    manager.cleaners = 0;
                    manager.repairers = 0;
                
                    world.each([&](flecs::entity staff_entity, const StaffAssignment& assignment) {
                        manager.total_staff_count++;
                        switch (assignment.role) {
                            case StaffRole::Firefighter:  manager.firefighters++; break;
                            case StaffRole::Security:     manager.security_guards++; break;
                            case StaffRole::Janitor:      manager.janitors++; break;
                            case StaffRole::Maintenance:  manager.maintenance_staff++; break;
                            case StaffRole::Cleaner:      manager.cleaners++; break;
                            case StaffRole::Repairer:     manager.repairers++; break;
                        }
                    });
                
                    manager.total_staff_wages = manager.total_staff_count * 50.0f;
                });
    }

    void StaffSystems::RegisterStaffWages(flecs::world& world) {
        world.system<const StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, const StaffManager& manager) {
                    if (manager.total_staff_count == 0) return;
                
                    auto& mut_economy = e.world().ensure<TowerEconomy>();
                
                    const float wage_per_second = manager.total_staff_wages / (24.0f * 3600.0f);
                    mut_economy.daily_expenses += wage_per_second;
                });
    }

    void StaffSystems::RegisterStaffStatusReporting(flecs::world& world) {
        world.system<const StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(30.0f)
                .each([](const flecs::entity e, const StaffManager& manager) {
                    if (manager.total_staff_count == 0) return;
                
                    std::cout << "  === Staff Report ===" << std::endl;
                    std::cout << "  Total Staff: " << manager.total_staff_count << std::endl;
                    if (manager.firefighters > 0) 
                        std::cout << "  Firefighters: " << manager.firefighters << std::endl;
                    if (manager.security_guards > 0) 
                        std::cout << "  Security: " << manager.security_guards << std::endl;
                    if (manager.janitors > 0) 
                        std::cout << "  Janitors: " << manager.janitors << std::endl;
                    if (manager.maintenance_staff > 0) 
                        std::cout << "  Maintenance: " << manager.maintenance_staff << std::endl;
                    if (manager.cleaners > 0) 
                        std::cout << "  Cleaners: " << manager.cleaners << std::endl;
                    if (manager.repairers > 0) 
                        std::cout << "  Repairers: " << manager.repairers << std::endl;
                    std::cout << "  Daily Wages: $" << manager.total_staff_wages << std::endl;
                    std::cout << "  ====================" << std::endl;
                });
    }

}
