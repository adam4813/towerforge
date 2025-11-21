#include "core/systems/visitor_employee_systems.hpp"
#include "core/components.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

namespace towerforge::core::Systems {

    void VisitorEmployeeSystems::RegisterAll(flecs::world& world) {
        RegisterResearchPointsGeneration(world);
        RegisterVisitorNeedsGrowth(world);
        RegisterVisitorNeedsBehavior(world);
        RegisterVisitorFacilityInteraction(world);
        RegisterVisitorSatisfaction(world);
        RegisterVisitorBehavior(world);
        RegisterVisitorNeedsDisplay(world);
        RegisterEmployeeShiftManagement(world);
        RegisterEmployeeOffDutyVisitor(world);
        RegisterJobOpeningTracking(world);
        RegisterVisitorSpawning(world);
        RegisterJobAssignment(world);
        RegisterVisitorCleanup(world);
    }

    void VisitorEmployeeSystems::RegisterResearchPointsGeneration(flecs::world& world) {
        world.system<ResearchTree, const TimeManager>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, ResearchTree& research, const TimeManager& time_mgr) {
                    int management_staff_count = 0;
                    e.world().each<const BuildingComponent>([&](const BuildingComponent& building) {
                        if (building.IsManagementFacility()) {
                            management_staff_count += building.current_staff;
                        }
                    });
            
                    research.UpdateManagementStaffCount(management_staff_count);
            
                    const float delta_hours = e.world().delta_time() / 3600.0f;
                    research.GenerateTowerPoints(delta_hours);
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorNeedsGrowth(flecs::world& world) {
        world.system<VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, VisitorNeeds& needs) {
                    const float delta_time = e.world().delta_time();
                    needs.UpdateNeeds(delta_time);
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorNeedsBehavior(flecs::world& world) {
        world.system<Person, VisitorInfo, VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity visitor_entity, Person& person, VisitorInfo& visitor, const VisitorNeeds& needs) {
                    if (visitor.activity == VisitorActivity::Leaving || 
                        visitor.activity == VisitorActivity::JobSeeking) {
                        return;
                    }
                    
                    if (person.state == PersonState::AtDestination && visitor.is_interacting) {
                        return;
                    }
                    
                    const float high_need_threshold = 60.0f;
                    const char* needed_facility_type = nullptr;
                    
                    if (needs.hunger > high_need_threshold) {
                        needed_facility_type = "Restaurant";
                    } else if (needs.entertainment > high_need_threshold) {
                        needed_facility_type = (rand() % 2 == 0) ? "Arcade" : "Theater";
                    } else if (needs.comfort > high_need_threshold) {
                        needed_facility_type = "Hotel";
                    } else if (needs.shopping > high_need_threshold) {
                        needed_facility_type = (rand() % 2 == 0) ? "RetailShop" : "FlagshipStore";
                    }
                    
                    if (needed_facility_type != nullptr) {
                        flecs::entity target_facility;
                        int target_floor = -1;
                        float target_column = -1.0f;
                        
                        visitor_entity.world().each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                            if (target_floor != -1) return;
                            
                            const char* facility_type_name = nullptr;
                            switch (facility.type) {
                                case BuildingComponent::Type::Restaurant: facility_type_name = "Restaurant"; break;
                                case BuildingComponent::Type::Arcade: facility_type_name = "Arcade"; break;
                                case BuildingComponent::Type::Theater: facility_type_name = "Theater"; break;
                                case BuildingComponent::Type::Hotel: facility_type_name = "Hotel"; break;
                                case BuildingComponent::Type::RetailShop: facility_type_name = "RetailShop"; break;
                                case BuildingComponent::Type::FlagshipStore: facility_type_name = "FlagshipStore"; break;
                                default: break;
                            }
                            
                            if (facility_type_name && strcmp(facility_type_name, needed_facility_type) == 0) {
                                target_facility = facility_entity;
                                target_floor = facility.floor;
                                target_column = static_cast<float>(facility.column) + (static_cast<float>(facility.width) / 2.0f);
                            }
                        });
                        
                        if (target_floor != -1) {
                            person.SetDestination(target_floor, target_column, 
                                                  std::string("Seeking ") + needed_facility_type);
                            visitor.target_facility_floor = target_floor;
                            visitor.is_interacting = false;
                            visitor.interaction_time = 0.0f;
                            
                            if (strcmp(needed_facility_type, "RetailShop") == 0 || 
                                strcmp(needed_facility_type, "FlagshipStore") == 0) {
                                visitor.activity = VisitorActivity::Shopping;
                            } else {
                                visitor.activity = VisitorActivity::Visiting;
                            }
                        }
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorFacilityInteraction(flecs::world& world) {
        world.system<Person, VisitorInfo, VisitorNeeds, Satisfaction>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor, VisitorNeeds& needs, Satisfaction& satisfaction) {
                    const float delta_time = e.world().delta_time();
                    
                    if (person.state == PersonState::AtDestination && 
                        visitor.activity != VisitorActivity::Leaving &&
                        visitor.activity != VisitorActivity::JobSeeking) {
                        
                        if (!visitor.is_interacting) {
                            visitor.is_interacting = true;
                            visitor.interaction_time = 0.0f;
                            visitor.required_interaction_time = 15.0f + (static_cast<float>(rand()) / RAND_MAX) * 15.0f;
                        }
                        
                        visitor.interaction_time += delta_time;
                        
                        e.world().each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                            if (facility.floor == person.current_floor &&
                                person.current_column >= static_cast<float>(facility.column) &&
                                person.current_column < static_cast<float>(facility.column + facility.width)) {
                                
                                const float reduction_per_second = 40.0f / visitor.required_interaction_time;
                                
                                switch (facility.type) {
                                    case BuildingComponent::Type::Restaurant:
                                        needs.ReduceNeed("Hunger", reduction_per_second * delta_time);
                                        break;
                                    case BuildingComponent::Type::Arcade:
                                    case BuildingComponent::Type::Theater:
                                        needs.ReduceNeed("Entertainment", reduction_per_second * delta_time);
                                        break;
                                    case BuildingComponent::Type::Hotel:
                                        needs.ReduceNeed("Comfort", reduction_per_second * delta_time);
                                        break;
                                    case BuildingComponent::Type::RetailShop:
                                    case BuildingComponent::Type::FlagshipStore:
                                        needs.ReduceNeed("Shopping", reduction_per_second * delta_time);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        });
                        
                        if (visitor.interaction_time >= visitor.required_interaction_time) {
                            visitor.is_interacting = false;
                            visitor.interaction_time = 0.0f;
                            visitor.time_at_destination = 0.0f;
                            
                            if (needs.GetHighestNeed() < 30.0f) {
                                visitor.activity = VisitorActivity::Leaving;
                                person.SetDestination(0, 5.0f, "Leaving tower");
                            }
                        }
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorSatisfaction(flecs::world& world) {
        world.system<VisitorNeeds, Satisfaction>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity e, const VisitorNeeds& needs, Satisfaction& satisfaction) {
                    const float avg_need = (needs.hunger + needs.entertainment + needs.comfort + needs.shopping) / 4.0f;
                    
                    const float target_satisfaction = 100.0f - avg_need;
                    
                    const float adjustment_rate = 0.1f;
                    const float diff = target_satisfaction - satisfaction.satisfaction_score;
                    satisfaction.satisfaction_score += diff * adjustment_rate;
                    
                    satisfaction.satisfaction_score = std::max(0.0f, std::min(100.0f, satisfaction.satisfaction_score));
                    
                    if (needs.GetHighestNeed() > 80.0f) {
                        satisfaction.wait_time_penalty = 5.0f;
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorBehavior(flecs::world& world) {
        world.system<Person, VisitorInfo>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor) {
                    const float delta_time = e.world().delta_time();
                    visitor.visit_duration += delta_time;
            
                    if (person.state == PersonState::AtDestination && !visitor.is_interacting) {
                        visitor.time_at_destination += delta_time;
                    }
            
                    if (visitor.ShouldLeave() && visitor.activity != VisitorActivity::Leaving) {
                        visitor.activity = VisitorActivity::Leaving;
                        person.SetDestination(0, 5.0f, "Leaving tower");
                    }
            
                    person.current_need = visitor.GetActivityString();
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorNeedsDisplay(flecs::world& world) {
        world.system<Person, VisitorInfo, const VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor, const VisitorNeeds& needs) {
                    person.current_need = std::string(visitor.GetActivityString()) + " - " + needs.GetHighestNeedType();
                });
    }

    void VisitorEmployeeSystems::RegisterEmployeeShiftManagement(flecs::world& world) {
        world.system<Person, EmploymentInfo>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, EmploymentInfo& employment) {
                    if (!e.world().has<TimeManager>()) return;
                    const TimeManager& time_mgr = e.world().get<TimeManager>();

                    const bool should_be_working = employment.ShouldBeWorking(time_mgr.current_hour, time_mgr.current_day);
            
                    if (should_be_working && !employment.currently_on_shift) {
                        employment.currently_on_shift = true;
                        person.SetDestination(employment.workplace_floor, 
                                              static_cast<float>(employment.workplace_column),
                                              "Going to work");
                    } else if (!should_be_working && employment.currently_on_shift) {
                        employment.currently_on_shift = false;
                        person.current_need = "Off duty";
                    }
            
                    if (employment.currently_on_shift) {
                        person.current_need = employment.GetStatusString();
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterEmployeeOffDutyVisitor(flecs::world& world) {
        world.system<Person, EmploymentInfo>()
                .kind(flecs::OnUpdate)
                .interval(30.0f)
                .each([](const flecs::entity e, Person& person, const EmploymentInfo& employment) {
                    if (!employment.currently_on_shift && !e.has<VisitorInfo>()) {
                        if ((rand() % 100) < 20) {
                            e.set<VisitorInfo>({VisitorActivity::Visiting});
                            person.current_need = "Visiting (off duty)";
                        }
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterJobOpeningTracking(flecs::world& world) {
        world.system<BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)
                .each([](const flecs::entity facility_entity, BuildingComponent& facility) {
                    const int required = facility.GetRequiredEmployees();
                    if (required == 0) {
                        facility.job_openings = 0;
                        return;
                    }
            
                    int current_employees = 0;
                    const auto query = facility_entity.world().query<const Person, const EmploymentInfo>();
                    query.each([&](flecs::entity e, const Person& person, const EmploymentInfo& emp) {
                        if (emp.workplace_floor == facility.floor) {
                            current_employees++;
                        }
                    });
            
                    facility.job_openings = std::max(0, required - current_employees);
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorSpawning(flecs::world& world) {
        world.system<NPCSpawner>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, NPCSpawner& spawner) {
                    const float delta_time = e.world().delta_time();
                    spawner.time_since_last_spawn += delta_time;
            
                    int active_visitor_count = 0;
                    const auto visitor_count_query = e.world().query<const Person, const VisitorInfo>();
                    visitor_count_query.each([&](const flecs::entity, const Person&, const VisitorInfo&) {
                        active_visitor_count++;
                    });
            
                    if (active_visitor_count >= spawner.max_active_visitors) {
                        return;
                    }
            
                    int total_job_openings = 0;
                    int facility_count = 0;
                    std::vector<flecs::entity> visitable_facilities;
                    const auto facility_query = e.world().query<const BuildingComponent>();
                    facility_query.each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                        facility_count++;
                        total_job_openings += facility.job_openings;
                        
                        if (facility.type == BuildingComponent::Type::RetailShop ||
                            facility.type == BuildingComponent::Type::Restaurant ||
                            facility.type == BuildingComponent::Type::Arcade ||
                            facility.type == BuildingComponent::Type::Theater ||
                            facility.type == BuildingComponent::Type::FlagshipStore) {
                            visitable_facilities.push_back(facility_entity);
                        }
                    });
            
                    const float spawn_interval = spawner.GetDynamicSpawnInterval(facility_count);
            
                    if (spawner.time_since_last_spawn >= spawn_interval) {
                        spawner.time_since_last_spawn = 0.0f;
                
                        VisitorActivity activity;
                        if (total_job_openings > 0 && (rand() % 100) < 40) {
                            activity = VisitorActivity::JobSeeking;
                        } else {
                            const int r = rand() % 100;
                            if (r < 60) {
                                activity = VisitorActivity::Shopping;
                            } else {
                                activity = VisitorActivity::Visiting;
                            }
                        }
                
                        std::string visitor_name = "Visitor" + std::to_string(spawner.next_visitor_id++);
                        const auto visitor = e.world().entity(visitor_name.c_str());
                        visitor.set<Person>({visitor_name, 0, 2.0f, 2.0f, NPCType::Visitor});
                        visitor.set<VisitorInfo>({activity});
                        visitor.set<Satisfaction>({75.0f});
                        
                        VisitorArchetype archetype;
                        const int archetype_roll = rand() % 100;
                        if (archetype_roll < 25) {
                            archetype = VisitorArchetype::BusinessPerson;
                        } else if (archetype_roll < 50) {
                            archetype = VisitorArchetype::Tourist;
                        } else if (archetype_roll < 75) {
                            archetype = VisitorArchetype::Shopper;
                        } else {
                            archetype = VisitorArchetype::Casual;
                        }
                        visitor.set<VisitorNeeds>({archetype});
                
                        if ((activity == VisitorActivity::Shopping || activity == VisitorActivity::Visiting) 
                            && !visitable_facilities.empty()) {
                            const size_t random_index = static_cast<size_t>(rand()) % visitable_facilities.size();
                            const auto target_facility = visitable_facilities[random_index];
                            
                            if (target_facility.has<BuildingComponent>()) {
                                const auto& building = target_facility.ensure<BuildingComponent>();
                                
                                auto& person_ref = visitor.ensure<Person>();
                                const int target_floor = building.floor;
                                const float target_column = static_cast<float>(building.column) + (static_cast<float>(building.width) / 2.0f);
                                person_ref.SetDestination(target_floor, target_column, activity == VisitorActivity::Shopping ? "Shopping" : "Visiting");
                                
                                auto& visitor_info = visitor.ensure<VisitorInfo>();
                                visitor_info.target_facility_floor = target_floor;
                                
                                std::cout << "  [Spawned] " << visitor_name << " (" 
                                        << (activity == VisitorActivity::Shopping ? "Shopping" : "Visiting")
                                        << ") heading to Floor " << target_floor << std::endl;
                            }
                        } else {
                            std::cout << "  [Spawned] " << visitor_name << " (" 
                                    << (activity == VisitorActivity::JobSeeking ? "Job Seeking" : 
                                            (activity == VisitorActivity::Shopping ? "Shopping" : "Visiting"))
                                    << ")" << std::endl;
                        }
                
                        spawner.total_visitors_spawned++;
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterJobAssignment(flecs::world& world) {
        world.system<Person, VisitorInfo>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity visitor_entity, Person& person, const VisitorInfo& visitor) {
                    if (visitor.activity != VisitorActivity::JobSeeking) {
                        return;
                    }
            
                    if (person.current_floor != 0 || person.state != PersonState::Idle) {
                        return;
                    }
            
                    flecs::entity target_facility;
                    int target_floor = -1;
                    int target_column = -1;
                    std::string job_title;
                    float shift_start = 9.0f;
                    float shift_end = 17.0f;

                    const auto facility_query = visitor_entity.world().query<BuildingComponent>();
                    facility_query.each([&](const flecs::entity facility_entity, BuildingComponent& facility) {
                        if (target_floor != -1 || !facility.HasJobOpenings()) {
                            return;
                        }
                
                        switch (facility.type) {
                            case BuildingComponent::Type::Office:
                                job_title = "Office Worker";
                                shift_start = 9.0f;
                                shift_end = 17.0f;
                                break;
                            case BuildingComponent::Type::RetailShop:
                                job_title = "Shop Clerk";
                                shift_start = 10.0f;
                                shift_end = 19.0f;
                                break;
                            case BuildingComponent::Type::Restaurant:
                                job_title = "Restaurant Staff";
                                shift_start = 11.0f;
                                shift_end = 22.0f;
                                break;
                            case BuildingComponent::Type::Hotel:
                                job_title = "Hotel Staff";
                                shift_start = 8.0f;
                                shift_end = 20.0f;
                                break;
                            default:
                                return;
                        }
                
                        target_facility = facility_entity;
                        target_floor = facility.floor;
                        target_column = facility.column + (facility.width / 2);
                        facility.job_openings--;
                    });
            
                    if (target_floor != -1) {
                        visitor_entity.remove<VisitorInfo>();
                
                        const EmploymentInfo employment(job_title, target_floor, target_column, shift_start, shift_end);
                        visitor_entity.set<EmploymentInfo>(employment);
                
                        person.npc_type = NPCType::Employee;
                        person.current_need = "New hire: " + job_title;
                
                        if (visitor_entity.world().has<NPCSpawner>()) {
                            auto& spawner = visitor_entity.world().get_mut<NPCSpawner>();
                            spawner.total_employees_hired++;
                        }
                
                        std::cout << "  [Hired] " << person.name << " as " << job_title 
                                << " on Floor " << target_floor << std::endl;
                    }
                });
    }

    void VisitorEmployeeSystems::RegisterVisitorCleanup(flecs::world& world) {
        world.system<const Person, const VisitorInfo>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)
                .each([](const flecs::entity e, const Person& person, const VisitorInfo& visitor) {
                    if (visitor.activity == VisitorActivity::Leaving && 
                        person.state == PersonState::AtDestination &&
                        person.current_floor == 0) {
                        e.destruct();
                    }
                });
    }

}
