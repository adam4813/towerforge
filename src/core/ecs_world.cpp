#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include <iostream>

namespace TowerForge {
namespace Core {

ECSWorld::ECSWorld() 
    : tower_grid_(std::make_unique<TowerGrid>(10, 20)) {  // 10 floors, 20 columns initial
}

ECSWorld::~ECSWorld() {
}

void ECSWorld::Initialize() {
    std::cout << "Initializing ECS World..." << std::endl;
    
    RegisterComponents();
    RegisterSystems();
    
    // Create facility manager after world is initialized
    facility_manager_ = std::make_unique<FacilityManager>(world_, *tower_grid_);
    
    std::cout << "ECS World initialized successfully" << std::endl;
}

bool ECSWorld::Update(float delta_time) {
    // Run one frame of the simulation
    // The delta_time is passed as a singleton to systems that need it
    world_.set<float>({delta_time});
    
    // Progress the world by one frame
    // This will execute all systems in the correct order
    return world_.progress(delta_time);
}

flecs::world& ECSWorld::GetWorld() {
    return world_;
}

flecs::entity ECSWorld::CreateEntity(const char* name) {
    if (name) {
        return world_.entity(name);
    }
    return world_.entity();
}

TowerGrid& ECSWorld::GetTowerGrid() {
    return *tower_grid_;
}

FacilityManager& ECSWorld::GetFacilityManager() {
    return *facility_manager_;
}

void ECSWorld::RegisterComponents() {
    // Register components with the ECS
    // This allows flecs to track and manage component metadata
    world_.component<Position>();
    world_.component<Velocity>();
    world_.component<Actor>();
    world_.component<BuildingComponent>();
    world_.component<TimeManager>();
    world_.component<DailySchedule>();
    world_.component<GridPosition>();
    world_.component<Satisfaction>();
    world_.component<FacilityEconomics>();
    world_.component<TowerEconomy>();
    
    std::cout << "  Registered components: Position, Velocity, Actor, BuildingComponent, TimeManager, DailySchedule, GridPosition, Satisfaction, FacilityEconomics, TowerEconomy" << std::endl;
}

void ECSWorld::RegisterSystems() {
    // Time simulation system - runs first to update simulation time
    // This system updates the global TimeManager singleton each frame
    world_.system<TimeManager>()
        .kind(flecs::PreUpdate)  // Run before other systems
        .each([](flecs::entity e, TimeManager& time_mgr) {
            // Get delta time from the world
            float delta_time = e.world().delta_time();
            
            // Calculate how much simulation time passes this frame
            float hours_elapsed = time_mgr.hours_per_second * time_mgr.simulation_speed * delta_time;
            
            // Update current hour
            time_mgr.current_hour += hours_elapsed;
            
            // Handle day rollover
            while (time_mgr.current_hour >= 24.0f) {
                time_mgr.current_hour -= 24.0f;
                time_mgr.current_day++;
                
                // Handle week rollover
                if (time_mgr.current_day >= 7) {
                    time_mgr.current_day = 0;
                    time_mgr.current_week++;
                }
            }
        });
    
    // Schedule execution system - triggers scheduled actions for entities
    // This system runs for all entities that have both DailySchedule and Actor components
    world_.system<DailySchedule, const Actor>()
        .kind(flecs::OnUpdate)
        .each([](flecs::entity e, DailySchedule& schedule, const Actor& actor) {
            // Get the global time manager
            const auto& time_mgr = e.world().get<TimeManager>();
            
            // Get the appropriate schedule based on weekend/weekday
            const auto& active_schedule = schedule.GetActiveSchedule(time_mgr.IsWeekend());
            
            // Check each scheduled action
            for (const auto& action : active_schedule) {
                // Check if we've crossed the trigger time since last frame
                bool should_trigger = false;
                
                if (schedule.last_triggered_hour < 0.0f) {
                    // First frame, check if we're past the trigger time
                    should_trigger = (time_mgr.current_hour >= action.trigger_hour);
                } else {
                    // Check if we crossed the trigger time
                    // Handle wrap-around at midnight
                    if (schedule.last_triggered_hour > time_mgr.current_hour) {
                        // Day rolled over - only trigger if action time is less than current time
                        // (i.e., action happens in the new day and we've passed it)
                        should_trigger = (time_mgr.current_hour >= action.trigger_hour);
                    } else if (schedule.last_triggered_hour < action.trigger_hour && 
                               time_mgr.current_hour >= action.trigger_hour) {
                        should_trigger = true;
                    }
                }
                
                if (should_trigger) {
                    // Execute the scheduled action
                    const char* action_name = "Unknown";
                    switch (action.type) {
                        case ScheduledAction::Type::ArriveWork:
                            action_name = "Arriving at work";
                            break;
                        case ScheduledAction::Type::LeaveWork:
                            action_name = "Leaving work";
                            break;
                        case ScheduledAction::Type::LunchBreak:
                            action_name = "Taking lunch break";
                            break;
                        case ScheduledAction::Type::Idle:
                            action_name = "Going idle";
                            break;
                        case ScheduledAction::Type::Custom:
                            action_name = "Custom action";
                            break;
                    }
                    
                    std::cout << "  [" << time_mgr.GetTimeString() << "] "
                              << actor.name << ": " << action_name << std::endl;
                }
            }
            
            // Update last triggered hour
            schedule.last_triggered_hour = time_mgr.current_hour;
        });
    
    // Time logging system - periodically logs the current simulation time
    world_.system<const TimeManager>()
        .kind(flecs::OnUpdate)
        .interval(10.0f)  // Log every 10 real-time seconds
        .each([](flecs::entity e, const TimeManager& time_mgr) {
            std::cout << "  === Simulation Time: " << time_mgr.GetTimeString() 
                      << " " << time_mgr.GetDayName() 
                      << ", Week " << time_mgr.current_week
                      << " (Speed: " << time_mgr.simulation_speed << "x) ===" << std::endl;
        });
    
    // Example system: Movement system that updates positions based on velocity
    // This system runs every frame for all entities that have both Position and Velocity
    world_.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& pos, const Velocity& vel) {
            pos.x += vel.dx;
            pos.y += vel.dy;
        });
    
    // Example system: Actor logging system (demonstrates querying actors)
    // This system runs once per frame to log active actors
    world_.system<const Actor, const Position>()
        .kind(flecs::OnUpdate)
        .interval(5.0f)  // Run every 5 seconds
        .each([](flecs::entity e, const Actor& actor, const Position& pos) {
            std::cout << "  Actor '" << actor.name 
                      << "' at position (" << pos.x << ", " << pos.y 
                      << "), destination floor: " << actor.floor_destination 
                      << std::endl;
        });
    
    // Example system: Building occupancy monitor
    // This system monitors building components and reports their status
    world_.system<const BuildingComponent>()
        .kind(flecs::OnUpdate)
        .interval(10.0f)  // Run every 10 seconds
        .each([](flecs::entity e, const BuildingComponent& component) {
            const char* type_name = FacilityManager::GetTypeName(component.type);
            
            std::cout << "  " << type_name 
                      << " on floor " << component.floor 
                      << ": " << component.current_occupancy 
                      << "/" << component.capacity << " occupied"
                      << std::endl;
        });
    
    // Satisfaction update system
    // Updates tenant satisfaction based on various factors
    world_.system<Satisfaction, const BuildingComponent>()
        .kind(flecs::OnUpdate)
        .interval(1.0f)  // Update every second
        .each([](flecs::entity e, Satisfaction& satisfaction, const BuildingComponent& facility) {
            // Calculate crowding penalty based on occupancy
            float occupancy_rate = static_cast<float>(facility.current_occupancy) / facility.capacity;
            if (occupancy_rate > 0.9f) {
                // Overcrowded - high penalty
                satisfaction.crowding_penalty = (occupancy_rate - 0.9f) * 50.0f;
            } else if (occupancy_rate < 0.3f) {
                // Too empty - small penalty (feels abandoned)
                satisfaction.crowding_penalty = (0.3f - occupancy_rate) * 10.0f;
            } else {
                // Good occupancy
                satisfaction.crowding_penalty *= 0.9f;  // Decay existing penalty
            }
            
            // Noise penalty based on facility type and occupancy
            if (facility.type == BuildingComponent::Type::Restaurant || 
                facility.type == BuildingComponent::Type::Shop) {
                satisfaction.noise_penalty = occupancy_rate * 5.0f;
            }
            
            // Quality bonus based on facility type
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
            
            // Update the overall satisfaction score
            satisfaction.UpdateScore();
        });
    
    // Satisfaction reporting system
    // Periodically reports satisfaction levels
    world_.system<const Satisfaction, const Actor>()
        .kind(flecs::OnUpdate)
        .interval(15.0f)  // Report every 15 seconds
        .each([](flecs::entity e, const Satisfaction& satisfaction, const Actor& actor) {
            std::cout << "  [Satisfaction] " << actor.name 
                      << ": " << static_cast<int>(satisfaction.satisfaction_score) << "% ("
                      << satisfaction.GetLevelString() << ")"
                      << std::endl;
        });
    
    // Facility economics update system
    // Processes revenue and costs for facilities
    world_.system<FacilityEconomics, const BuildingComponent, const Satisfaction>()
        .kind(flecs::OnUpdate)
        .interval(1.0f)  // Update every second
        .each([](flecs::entity e, FacilityEconomics& economics, 
                 const BuildingComponent& facility, const Satisfaction& satisfaction) {
            // Update current tenants based on satisfaction and capacity
            // High satisfaction attracts more tenants
            if (satisfaction.satisfaction_score > 70.0f && 
                economics.current_tenants < economics.max_tenants) {
                // Chance to gain a tenant
                if (facility.current_occupancy < facility.capacity) {
                    economics.current_tenants++;
                }
            } else if (satisfaction.satisfaction_score < 30.0f && 
                       economics.current_tenants > 0) {
                // Chance to lose a tenant
                economics.current_tenants--;
            }
            
            // Adjust quality multiplier based on satisfaction
            economics.quality_multiplier = 0.5f + (satisfaction.satisfaction_score / 100.0f) * 1.5f;
        });
    
    // Daily economy processing system
    // Processes daily transactions at the start of each new day
    world_.system<TowerEconomy, const TimeManager>()
        .kind(flecs::OnUpdate)
        .each([](flecs::entity e, TowerEconomy& economy, const TimeManager& time_mgr) {
            int current_day = time_mgr.current_week * 7 + time_mgr.current_day;
            
            // Check if it's a new day
            if (current_day != economy.last_processed_day) {
                if (economy.last_processed_day >= 0) {
                    // Process yesterday's transactions
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
    
    // Revenue collection system
    // Collects revenue from all facilities daily
    world_.system<const FacilityEconomics>()
        .kind(flecs::OnUpdate)
        .interval(1.0f)  // Check every second
        .each([](flecs::entity e, const FacilityEconomics& economics) {
            // Get tower economy singleton (mutable reference)
            auto& mut_economy = e.world().ensure<TowerEconomy>();
            
            // Calculate revenue for this tick (1 second interval)
            // Daily revenue is spread over 24 hours
            float revenue_per_second = economics.CalculateDailyRevenue() / (24.0f * 3600.0f);
            mut_economy.daily_revenue += revenue_per_second;
            
            // Add operating costs
            float cost_per_second = economics.operating_cost / (24.0f * 3600.0f);
            mut_economy.daily_expenses += cost_per_second;
        });
    
    // Economic status reporting system
    world_.system<const FacilityEconomics, const BuildingComponent>()
        .kind(flecs::OnUpdate)
        .interval(20.0f)  // Report every 20 seconds
        .each([](flecs::entity e, const FacilityEconomics& economics, 
                 const BuildingComponent& facility) {
            const char* type_name = "Unknown";
            switch(facility.type) {
                case BuildingComponent::Type::Office:     type_name = "Office"; break;
                case BuildingComponent::Type::Restaurant: type_name = "Restaurant"; break;
                case BuildingComponent::Type::Shop:       type_name = "Shop"; break;
                case BuildingComponent::Type::Hotel:      type_name = "Hotel"; break;
                case BuildingComponent::Type::Elevator:   type_name = "Elevator"; break;
                case BuildingComponent::Type::Lobby:      type_name = "Lobby"; break;
            }
            
            std::cout << "  [Economics] " << type_name << " Floor " << facility.floor
                      << ": Occupancy " << static_cast<int>(economics.GetOccupancyRate()) << "%"
                      << ", Daily Profit: $" << static_cast<int>(economics.CalculateNetProfit())
                      << " (Quality: " << static_cast<int>(economics.quality_multiplier * 100) << "%)"
                      << std::endl;
        });
    
    std::cout << "  Registered systems: Time Simulation, Schedule Execution, Movement, Actor Logging, Building Occupancy Monitor, Satisfaction Update, Satisfaction Reporting, Facility Economics, Daily Economy Processing, Revenue Collection, Economic Status Reporting" << std::endl;
}

} // namespace Core
} // namespace TowerForge
