#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/lua_mod_manager.hpp"
#include <iostream>
#include <set>

namespace TowerForge::Core {

    ECSWorld::ECSWorld() 
        : tower_grid_(std::make_unique<TowerGrid>(1, 20, 0)) {  // Start with 1 floor (ground), 20 columns, ground at index 0
    }

    ECSWorld::ECSWorld(const int screen_width, const int screen_height, const int cell_width, const int cell_height) {
        // Calculate initial dimensions based on screen size
        // Horizontal cells: approximately one screen width of cells
        const int initial_columns = std::min(screen_width / cell_width, MAX_HORIZONTAL_CELLS);
        
        // Below ground floors: 1 screen height worth of floors
        const int initial_below_ground = std::min(screen_height / cell_height, MAX_BELOW_GROUND_FLOORS);
        
        // Above ground floors: 3 screen heights worth of floors
        const int initial_above_ground = std::min((screen_height * 3) / cell_height, MAX_ABOVE_GROUND_FLOORS);
        
        // Total initial floors = ground floor (1) + above ground + below ground
        const int initial_floors = initial_above_ground + initial_below_ground;
        
        // Ground floor index is offset by basement floors
        const int ground_floor_index = initial_below_ground;
        
        // Create tower grid with calculated dimensions
        tower_grid_ = std::make_unique<TowerGrid>(initial_floors, initial_columns, ground_floor_index);
        
        // Set the initial upgrade limits
        tower_grid_->SetMaxAboveGroundFloors(initial_above_ground);
        tower_grid_->SetMaxBelowGroundFloors(initial_below_ground);
        
        // Build the initial basement floors
        for (int i = 0; i < initial_below_ground; ++i) {
            tower_grid_->AddBasementFloor();
        }
    }

    ECSWorld::~ECSWorld() {
    }

    void ECSWorld::Initialize() {
        std::cout << "Initializing ECS World..." << std::endl;
    
        RegisterComponents();
        RegisterSystems();
    
        // Create facility manager after world is initialized
        facility_manager_ = std::make_unique<FacilityManager>(world_, *tower_grid_);
    
        // Create and initialize mod manager
        mod_manager_ = std::make_unique<LuaModManager>();
        if (mod_manager_->Initialize(this)) {
            // Load mods from the mods/ directory
            mod_manager_->LoadMods("mods/");
        }
    
        std::cout << "ECS World initialized successfully" << std::endl;
    }

    bool ECSWorld::Update(float delta_time) const {
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

    flecs::entity ECSWorld::CreateEntity(const char* name) const {
        if (name) {
            return world_.entity(name);
        }
        return world_.entity();
    }

    TowerGrid& ECSWorld::GetTowerGrid() const {
        return *tower_grid_;
    }

    FacilityManager& ECSWorld::GetFacilityManager() const {
        return *facility_manager_;
    }

    LuaModManager& ECSWorld::GetModManager() const {
        return *mod_manager_;
    }

    void ECSWorld::ApplyVerticalExpansionUpgrades() {
        // Get the research tree singleton
        const ResearchTree& research_tree = world_.get<ResearchTree>();

        // Track the maximum values from unlocked nodes
        int max_above_ground = tower_grid_->GetMaxAboveGroundFloors();
        int max_below_ground = tower_grid_->GetMaxBelowGroundFloors();

        // Iterate through all unlocked vertical expansion nodes
        for (const auto& node : research_tree.nodes) {
            if (node.state == ResearchNodeState::Unlocked && 
                node.type == ResearchNodeType::VerticalExpansion) {
                
                // Apply based on effect target
                if (node.effect_target == "AboveGround") {
                    max_above_ground = std::max(max_above_ground, static_cast<int>(node.effect_value));
                } else if (node.effect_target == "BelowGround") {
                    max_below_ground = std::max(max_below_ground, static_cast<int>(node.effect_value));
                }
            }
        }

        // Apply the new limits to the tower grid
        tower_grid_->SetMaxAboveGroundFloors(max_above_ground);
        tower_grid_->SetMaxBelowGroundFloors(max_below_ground);
    }

    void ECSWorld::RegisterComponents() const {
        // Register components with the ECS
        // This allows flecs to track and manage component metadata
        world_.component<Position>();
        world_.component<Velocity>();
        world_.component<Actor>();
        world_.component<Person>();
        world_.component<VisitorInfo>();
        world_.component<VisitorNeeds>();
        world_.component<EmploymentInfo>();
        world_.component<BuildingComponent>();
        world_.component<TimeManager>();
        world_.component<NPCSpawner>();
        world_.component<DailySchedule>();
        world_.component<GridPosition>();
        world_.component<Satisfaction>();
        world_.component<FacilityEconomics>();
        world_.component<TowerEconomy>();
        world_.component<ElevatorShaft>();
        world_.component<ElevatorCar>();
        world_.component<PersonElevatorRequest>();
        world_.component<StaffAssignment>();
        world_.component<FacilityStatus>();
        world_.component<StaffManager>();
        world_.component<CleanlinessStatus>();
        world_.component<MaintenanceStatus>();
    
        std::cout << "  Registered components: Position, Velocity, Actor, Person, VisitorInfo, VisitorNeeds, EmploymentInfo, BuildingComponent, TimeManager, NPCSpawner, DailySchedule, GridPosition, Satisfaction, FacilityEconomics, TowerEconomy, ElevatorShaft, ElevatorCar, PersonElevatorRequest, StaffAssignment, FacilityStatus, StaffManager, CleanlinessStatus, MaintenanceStatus" << std::endl;
    }

    void ECSWorld::RegisterSystems() const {
        // Time simulation system - runs first to update simulation time
        // This system updates the global TimeManager singleton each frame
        world_.system<TimeManager>()
                .kind(flecs::PreUpdate)  // Run before other systems
                .each([](const flecs::entity e, TimeManager& time_mgr) {
                    // Get delta time from the world
                    const float delta_time = e.world().delta_time();
            
                    // Calculate how much simulation time passes this frame
                    const float hours_elapsed = time_mgr.hours_per_second * time_mgr.simulation_speed * delta_time;
            
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
                .each([](const flecs::entity e, DailySchedule& schedule, const Actor& actor) {
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
                            auto action_name = "Unknown";
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
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / facility.capacity;
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
                        facility.type == BuildingComponent::Type::RetailShop) {
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
                    const int current_day = time_mgr.current_week * 7 + time_mgr.current_day;
            
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
                .each([](const flecs::entity e, const FacilityEconomics& economics) {
                    // Get tower economy singleton (mutable reference)
                    auto& mut_economy = e.world().ensure<TowerEconomy>();
            
                    // Calculate revenue for this tick (1 second interval)
                    // Daily revenue is spread over 24 hours
                    const float revenue_per_second = economics.CalculateDailyRevenue() / (24.0f * 3600.0f);
                    mut_economy.daily_revenue += revenue_per_second;
            
                    // Add operating costs
                    const float cost_per_second = economics.operating_cost / (24.0f * 3600.0f);
                    mut_economy.daily_expenses += cost_per_second;
                });
    
        // Economic status reporting system
        world_.system<const FacilityEconomics, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(20.0f)  // Report every 20 seconds
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
    
        // Person horizontal movement system
        // Handles walking on the same floor towards a destination
        world_.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    const float delta_time = e.world().delta_time();
            
                    if (person.state == PersonState::Walking) {
                        // Calculate direction to destination
                        const float direction = (person.destination_column > person.current_column) ? 1.0f : -1.0f;
                        const float distance_to_dest = std::abs(person.destination_column - person.current_column);
                
                        // Move towards destination
                        const float move_amount = person.move_speed * delta_time;
                
                        if (move_amount >= distance_to_dest) {
                            // Reached destination this frame
                            person.current_column = person.destination_column;
                    
                            // Check if we've also reached vertical destination
                            if (person.HasReachedVerticalDestination()) {
                                person.state = PersonState::AtDestination;
                            } else {
                                // Need to change floors
                                person.state = PersonState::WaitingForElevator;
                            }
                        } else {
                            // Continue moving
                            person.current_column += direction * move_amount;
                        }
                    }
                });
    
        // Person waiting system
        // Creates elevator requests for people who need to change floors
        world_.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    // If person is waiting for elevator but doesn't have a request, create one
                    if (person.state == PersonState::WaitingForElevator && 
                        !e.has<PersonElevatorRequest>()) {
                
                        // Find nearest elevator shaft on current floor
                        // For now, create a request for shaft at column 0 (TODO: find nearest shaft)
                        int shaft_id = -1;
                        int shaft_column = -1;
                
                        // Search for an elevator shaft
                        e.world().each<ElevatorShaft>([&](const flecs::entity shaft_entity, const ElevatorShaft& shaft) {
                            if (shaft_id == -1 && shaft.ServesFloor(person.current_floor) && 
                                shaft.ServesFloor(person.destination_floor)) {
                                shaft_id = static_cast<int>(shaft_entity.id());
                                shaft_column = shaft.column;
                            }
                        });
                
                        if (shaft_id != -1) {
                            // Create elevator request
                            e.set<PersonElevatorRequest>({
                                shaft_id,
                                person.current_floor,
                                person.destination_floor
                            });
                    
                            // Person should walk to the elevator shaft column if not already there
                            if (std::abs(person.current_column - static_cast<float>(shaft_column)) > 0.1f) {
                                person.destination_column = static_cast<float>(shaft_column);
                                person.state = PersonState::Walking;
                            }
                        } else {
                            // No elevator available, fallback to old behavior (for backward compatibility)
                            person.wait_time += e.world().delta_time();
                            if (person.wait_time > 5.0f) {
                                person.state = PersonState::InElevator;
                                person.wait_time = 0.0f;
                            }
                        }
                    }
                });
    
        // Person elevator riding system (fallback for people without PersonElevatorRequest)
        // This is for backward compatibility with existing code
        world_.system<Person>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person) {
                    // Only run for people in elevator without an elevator request
                    if (person.state == PersonState::InElevator && !e.has<PersonElevatorRequest>()) {
                        const float delta_time = e.world().delta_time();
                        person.wait_time += delta_time;
                
                        // Simulate elevator travel taking 3 seconds
                        if (person.wait_time > 3.0f) {
                            // Arrive at destination floor
                            person.current_floor = person.destination_floor;
                            person.wait_time = 0.0f;
                    
                            // Check if we need to walk to horizontal destination
                            if (!person.HasReachedHorizontalDestination()) {
                                person.state = PersonState::Walking;
                            } else {
                                person.state = PersonState::AtDestination;
                            }
                        }
                    }
                });
    
        // Person state logging system
        // Logs person state changes for debugging
        world_.system<const Person>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Log every 5 seconds
                .each([](flecs::entity e, const Person& person) {
                    std::cout << "  [Person] " << person.name 
                            << " - State: " << person.GetStateString()
                            << ", Floor: " << person.current_floor << " (" << person.current_column << ")"
                            << ", Dest: Floor " << person.destination_floor << " (" << person.destination_column << ")"
                            << ", Need: " << person.current_need;
            
                    if (person.state == PersonState::WaitingForElevator || person.state == PersonState::InElevator) {
                        std::cout << ", Wait: " << static_cast<int>(person.wait_time) << "s";
                    }
            
                    std::cout << std::endl;
                });
    
        // Elevator car movement system
        // Handles elevator movement between floors and state transitions
        world_.system<ElevatorCar>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, ElevatorCar& car) {
                    const float delta_time = e.world().delta_time();
            
                    // Update state timer
                    car.state_timer += delta_time;
            
                    switch (car.state) {
                        case ElevatorState::Idle:
                            // Check if there are any stops in the queue
                            if (!car.stop_queue.empty()) {
                                car.target_floor = car.GetNextStop();
                                const int current_floor = car.GetCurrentFloorInt();
                        
                                if (car.target_floor > current_floor) {
                                    car.state = ElevatorState::MovingUp;
                                } else if (car.target_floor < current_floor) {
                                    car.state = ElevatorState::MovingDown;
                                } else {
                                    // Already at target floor, open doors
                                    car.state = ElevatorState::DoorsOpening;
                                    car.state_timer = 0.0f;
                                }
                            }
                            break;
                
                        case ElevatorState::MovingUp:
                        case ElevatorState::MovingDown: {
                            // Move towards target floor
                            const float direction = (car.state == ElevatorState::MovingUp) ? 1.0f : -1.0f;
                            const float distance_to_target = std::abs(car.target_floor - car.current_floor);
                            const float move_amount = car.floors_per_second * delta_time;
                    
                            if (move_amount >= distance_to_target) {
                                // Arrived at target floor
                                car.current_floor = static_cast<float>(car.target_floor);
                                car.state = ElevatorState::DoorsOpening;
                                car.state_timer = 0.0f;
                            } else {
                                // Continue moving
                                car.current_floor += direction * move_amount;
                            }
                            break;
                        }
                
                        case ElevatorState::DoorsOpening:
                            if (car.state_timer >= car.door_transition_duration) {
                                car.state = ElevatorState::DoorsOpen;
                                car.state_timer = 0.0f;
                                car.RemoveCurrentStop();
                            }
                            break;
                
                        case ElevatorState::DoorsOpen:
                            if (car.state_timer >= car.door_open_duration) {
                                car.state = ElevatorState::DoorsClosing;
                                car.state_timer = 0.0f;
                            }
                            break;
                
                        case ElevatorState::DoorsClosing:
                            if (car.state_timer >= car.door_transition_duration) {
                                car.state = ElevatorState::Idle;
                                car.state_timer = 0.0f;
                            }
                            break;
                    }
                });
    
        // Elevator call system
        // Assigns people waiting for elevators to elevator cars
        world_.system<Person, PersonElevatorRequest>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity person_entity, const Person& person, PersonElevatorRequest& request) {
                    const float delta_time = person_entity.world().delta_time();
            
                    // Update wait time
                    request.wait_time += delta_time;
            
                    // If not yet assigned to a car, find the best car
                    if (request.car_entity_id == -1 && person.state == PersonState::WaitingForElevator) {
                        // Find the shaft entity
                        const auto shaft_entity = person_entity.world().entity(request.shaft_entity_id);
                        if (shaft_entity.is_valid() && shaft_entity.has<ElevatorShaft>()) {
                            // Find all cars in this shaft
                            person_entity.world().each<ElevatorCar>([&](const flecs::entity car_entity, ElevatorCar& car) {
                                if (car.shaft_entity_id == request.shaft_entity_id && 
                                    request.car_entity_id == -1) {
                                    // Simple assignment: use first available car
                                    // TODO: Implement smarter scheduling (closest car, direction, etc.)
                                    request.car_entity_id = static_cast<int>(car_entity.id());
                                    car.AddStop(request.call_floor);
                                    car.AddStop(request.destination_floor);
                                }
                            });
                        }
                    }
                });
    
        // Person elevator boarding system
        // Handles people getting on and off elevators
        world_.system<Person, PersonElevatorRequest>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity person_entity, Person& person, PersonElevatorRequest& request) {
                    if (request.car_entity_id == -1) return;
            
                    // Get the car entity
                    const auto car_entity = person_entity.world().entity(request.car_entity_id);
                    if (!car_entity.is_valid() || !car_entity.has<ElevatorCar>()) return;
            
                    ElevatorCar& car = car_entity.ensure<ElevatorCar>();
            
                    // Check if person is waiting and car has arrived
                    if (person.state == PersonState::WaitingForElevator && 
                        car.state == ElevatorState::DoorsOpen &&
                        car.GetCurrentFloorInt() == request.call_floor &&
                        car.HasCapacity()) {
                        // Board the elevator
                        person.state = PersonState::InElevator;
                        person.current_floor = car.GetCurrentFloorInt();
                        person.wait_time = 0.0f;
                        car.current_occupancy++;
                        car.passenger_destinations.push_back(request.destination_floor);
                        request.is_boarding = false;
                    }
            
                    // Check if person is in elevator and has arrived at destination
                    if (person.state == PersonState::InElevator &&
                        car.state == ElevatorState::DoorsOpen &&
                        car.GetCurrentFloorInt() == request.destination_floor) {
                        // Exit the elevator
                        person.current_floor = car.GetCurrentFloorInt();
                        car.current_occupancy--;
                
                        // Remove destination from car's passenger list
                        const auto it = std::find(car.passenger_destinations.begin(), 
                                            car.passenger_destinations.end(), 
                                            request.destination_floor);
                        if (it != car.passenger_destinations.end()) {
                            car.passenger_destinations.erase(it);
                        }
                
                        // Remove the elevator request component
                        person_entity.remove<PersonElevatorRequest>();
                
                        // Check if person needs to walk to final destination
                        if (!person.HasReachedHorizontalDestination()) {
                            person.state = PersonState::Walking;
                        } else {
                            person.state = PersonState::AtDestination;
                        }
                    }
                });
    
        // Elevator logging system
        // Logs elevator state for debugging
        world_.system<const ElevatorCar>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)  // Log every 10 seconds
                .each([](const flecs::entity e, const ElevatorCar& car) {
                    std::cout << "  [Elevator] Car " << e.name().c_str()
                            << " - State: " << car.GetStateString()
                            << ", Floor: " << car.current_floor
                            << ", Occupancy: " << car.current_occupancy << "/" << car.max_capacity;
            
                    if (!car.stop_queue.empty()) {
                        std::cout << ", Stops: [";
                        for (size_t i = 0; i < car.stop_queue.size(); i++) {
                            std::cout << car.stop_queue[i];
                            if (i < car.stop_queue.size() - 1) std::cout << ", ";
                        }
                        std::cout << "]";
                    }
            
                    std::cout << std::endl;
                });
    
        // Tower Points Generation System
        // Generates tower points based on management staff count
        world_.system<ResearchTree, const TimeManager>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second for smooth point generation
                .each([](const flecs::entity e, ResearchTree& research, const TimeManager& time_mgr) {
                    // Count management staff across all management facilities
                    int management_staff_count = 0;
                    e.world().each<const BuildingComponent>([&](const BuildingComponent& building) {
                        if (building.IsManagementFacility()) {
                            management_staff_count += building.current_staff;
                        }
                    });
            
                    // Update the staff count in research tree
                    research.UpdateManagementStaffCount(management_staff_count);
            
                    // Generate tower points based on time elapsed
                    // Convert game time to hours (delta_time is in seconds of game time)
                    const float delta_hours = e.world().delta_time() / 3600.0f;  // Convert to hours
                    research.GenerateTowerPoints(delta_hours);
                });
    
        // Visitor needs growth system
        // Increases visitor needs over time
        world_.system<VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, VisitorNeeds& needs) {
                    const float delta_time = e.world().delta_time();
                    needs.UpdateNeeds(delta_time);
                });
    
        // Visitor needs-driven behavior system
        // Makes visitors seek facilities based on their needs
        world_.system<Person, VisitorInfo, VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Check every 5 seconds
                .each([](const flecs::entity visitor_entity, Person& person, VisitorInfo& visitor, const VisitorNeeds& needs) {
                    // Don't change destination if visitor is leaving or seeking a job
                    if (visitor.activity == VisitorActivity::Leaving || 
                        visitor.activity == VisitorActivity::JobSeeking) {
                        return;
                    }
                    
                    // Don't interrupt if already at a destination and interacting
                    if (person.state == PersonState::AtDestination && visitor.is_interacting) {
                        return;
                    }
                    
                    // Check if any need is high (> 60) and find a matching facility
                    const float high_need_threshold = 60.0f;
                    const char* needed_facility_type = nullptr;
                    
                    if (needs.hunger > high_need_threshold) {
                        needed_facility_type = "Restaurant";
                    } else if (needs.entertainment > high_need_threshold) {
                        // Prefer arcades and theaters
                        needed_facility_type = (rand() % 2 == 0) ? "Arcade" : "Theater";
                    } else if (needs.comfort > high_need_threshold) {
                        needed_facility_type = "Hotel";
                    } else if (needs.shopping > high_need_threshold) {
                        needed_facility_type = (rand() % 2 == 0) ? "RetailShop" : "FlagshipStore";
                    }
                    
                    if (needed_facility_type != nullptr) {
                        // Find a facility of the needed type
                        flecs::entity target_facility;
                        int target_floor = -1;
                        float target_column = -1.0f;
                        
                        visitor_entity.world().each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                            if (target_floor != -1) return;  // Already found one
                            
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
                            // Set destination to the facility
                            person.SetDestination(target_floor, target_column, 
                                                  std::string("Seeking ") + needed_facility_type);
                            visitor.target_facility_floor = target_floor;
                            visitor.is_interacting = false;
                            visitor.interaction_time = 0.0f;
                            
                            // Update activity based on facility type
                            if (strcmp(needed_facility_type, "RetailShop") == 0 || 
                                strcmp(needed_facility_type, "FlagshipStore") == 0) {
                                visitor.activity = VisitorActivity::Shopping;
                            } else {
                                visitor.activity = VisitorActivity::Visiting;
                            }
                        }
                    }
                });
    
        // Visitor facility interaction system
        // Handles visitors using facilities to reduce their needs
        world_.system<Person, VisitorInfo, VisitorNeeds, Satisfaction>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor, VisitorNeeds& needs, Satisfaction& satisfaction) {
                    const float delta_time = e.world().delta_time();
                    
                    // Check if visitor is at a destination
                    if (person.state == PersonState::AtDestination && 
                        visitor.activity != VisitorActivity::Leaving &&
                        visitor.activity != VisitorActivity::JobSeeking) {
                        
                        if (!visitor.is_interacting) {
                            // Start interacting
                            visitor.is_interacting = true;
                            visitor.interaction_time = 0.0f;
                            // Set random interaction time between 15-30 seconds
                            visitor.required_interaction_time = 15.0f + (static_cast<float>(rand()) / RAND_MAX) * 15.0f;
                        }
                        
                        visitor.interaction_time += delta_time;
                        
                        // Find the facility at current location
                        e.world().each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                            if (facility.floor == person.current_floor &&
                                person.current_column >= static_cast<float>(facility.column) &&
                                person.current_column < static_cast<float>(facility.column + facility.width)) {
                                
                                // Reduce needs based on facility type
                                const float reduction_per_second = 40.0f / visitor.required_interaction_time;  // Reduce by 40 over interaction time
                                
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
                        
                        // Check if interaction is complete
                        if (visitor.interaction_time >= visitor.required_interaction_time) {
                            visitor.is_interacting = false;
                            visitor.interaction_time = 0.0f;
                            visitor.time_at_destination = 0.0f;
                            
                            // Decide next action: continue visiting or leave
                            // If all needs are low, consider leaving
                            if (needs.GetHighestNeed() < 30.0f) {
                                visitor.activity = VisitorActivity::Leaving;
                                person.SetDestination(0, 5.0f, "Leaving tower");
                            }
                        }
                    }
                });
    
        // Visitor satisfaction system
        // Calculates satisfaction based on how well needs are being met
        world_.system<VisitorNeeds, Satisfaction>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Update every 2 seconds
                .each([](const flecs::entity e, const VisitorNeeds& needs, Satisfaction& satisfaction) {
                    // Calculate satisfaction based on unmet needs
                    // Lower needs = higher satisfaction
                    const float avg_need = (needs.hunger + needs.entertainment + needs.comfort + needs.shopping) / 4.0f;
                    
                    // Target satisfaction: 100% when all needs are at 0, decreasing as needs increase
                    const float target_satisfaction = 100.0f - avg_need;
                    
                    // Smoothly adjust satisfaction toward target
                    const float adjustment_rate = 0.1f;  // 10% per update
                    const float diff = target_satisfaction - satisfaction.satisfaction_score;
                    satisfaction.satisfaction_score += diff * adjustment_rate;
                    
                    // Clamp to valid range
                    satisfaction.satisfaction_score = std::max(0.0f, std::min(100.0f, satisfaction.satisfaction_score));
                    
                    // Add penalty if needs are critically high
                    if (needs.GetHighestNeed() > 80.0f) {
                        satisfaction.wait_time_penalty = 5.0f;  // High unmet needs cause frustration
                    }
                });
    
        // Visitor behavior system
        // Updates visitor activities and manages their lifecycle
        world_.system<Person, VisitorInfo>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor) {
                    const float delta_time = e.world().delta_time();
                    visitor.visit_duration += delta_time;
            
                    // Track time at destination (for non-needs-based visitors)
                    if (person.state == PersonState::AtDestination && !visitor.is_interacting) {
                        visitor.time_at_destination += delta_time;
                    }
            
                    // Check if visitor should leave based on max duration
                    if (visitor.ShouldLeave() && visitor.activity != VisitorActivity::Leaving) {
                        visitor.activity = VisitorActivity::Leaving;
                        // Set destination to lobby (floor 0)
                        person.SetDestination(0, 5.0f, "Leaving tower");
                    }
            
                    // Update current_need based on activity
                    person.current_need = visitor.GetActivityString();
                });
    
        // Visitor needs display system
        // Updates person status to show their highest need
        world_.system<Person, VisitorInfo, const VisitorNeeds>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, Person& person, VisitorInfo& visitor, const VisitorNeeds& needs) {
                    person.current_need = std::string(visitor.GetActivityString()) + " - " + needs.GetHighestNeedType();
                });
    
        // Employee shift management system
        // Handles employee work schedules and shift transitions
        world_.system<Person, EmploymentInfo>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, Person& person, EmploymentInfo& employment) {
                    // Get the global time manager
                    if (!e.world().has<TimeManager>()) return;
                    const TimeManager& time_mgr = e.world().get<TimeManager>();

                    const bool should_be_working = employment.ShouldBeWorking(time_mgr.current_hour, time_mgr.current_day);
            
                    // Handle shift transitions
                    if (should_be_working && !employment.currently_on_shift) {
                        // Start shift - go to workplace
                        employment.currently_on_shift = true;
                        person.SetDestination(employment.workplace_floor, 
                                              static_cast<float>(employment.workplace_column),
                                              "Going to work");
                    } else if (!should_be_working && employment.currently_on_shift) {
                        // End shift - leave workplace
                        employment.currently_on_shift = false;
                        person.current_need = "Off duty";
                    }
            
                    // Update current_need to reflect employment status
                    if (employment.currently_on_shift) {
                        person.current_need = employment.GetStatusString();
                    }
                });
    
        // Employee off-duty visitor behavior system
        // When employees are off-duty, they can become visitors
        world_.system<Person, EmploymentInfo>()
                .kind(flecs::OnUpdate)
                .interval(30.0f)  // Check every 30 seconds
                .each([](const flecs::entity e, Person& person, const EmploymentInfo& employment) {
                    // If employee is off duty and not already a visitor
                    if (!employment.currently_on_shift && !e.has<VisitorInfo>()) {
                        // 20% chance to visit as a regular visitor
                        if ((rand() % 100) < 20) {
                            // Add visitor component temporarily
                            e.set<VisitorInfo>({VisitorActivity::Visiting});
                            person.current_need = "Visiting (off duty)";
                        }
                    }
                });
    
        // Job opening tracking system
        // Updates the number of job openings for each facility
        world_.system<BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Check every 5 seconds
                .each([](const flecs::entity facility_entity, BuildingComponent& facility) {
                    const int required = facility.GetRequiredEmployees();
                    if (required == 0) {
                        facility.job_openings = 0;
                        return;
                    }
            
                    // Count current employees at this facility
                    int current_employees = 0;
                    const auto query = facility_entity.world().query<const Person, const EmploymentInfo>();
                    query.each([&](flecs::entity e, const Person& person, const EmploymentInfo& emp) {
                        if (emp.workplace_floor == facility.floor) {
                            current_employees++;
                        }
                    });
            
                    // Update job openings
                    facility.job_openings = std::max(0, required - current_employees);
                });
    
        // Visitor spawning system
        // Dynamically spawns visitors at the lobby entrance
        world_.system<NPCSpawner>()
                .kind(flecs::OnUpdate)
                .each([](const flecs::entity e, NPCSpawner& spawner) {
                    const float delta_time = e.world().delta_time();
                    spawner.time_since_last_spawn += delta_time;
            
                    // Count active visitors
                    int active_visitor_count = 0;
                    const auto visitor_count_query = e.world().query<const Person, const VisitorInfo>();
                    visitor_count_query.each([&](const flecs::entity, const Person&, const VisitorInfo&) {
                        active_visitor_count++;
                    });
            
                    // Don't spawn if at or over visitor cap
                    if (active_visitor_count >= spawner.max_active_visitors) {
                        return;
                    }
            
                    // Count total job openings and collect visitable facilities
                    int total_job_openings = 0;
                    int facility_count = 0;
                    std::vector<flecs::entity> visitable_facilities;
                    const auto facility_query = e.world().query<const BuildingComponent>();
                    facility_query.each([&](const flecs::entity facility_entity, const BuildingComponent& facility) {
                        facility_count++;
                        total_job_openings += facility.job_openings;
                        
                        // Collect facilities that visitors might want to visit
                        if (facility.type == BuildingComponent::Type::RetailShop ||
                            facility.type == BuildingComponent::Type::Restaurant ||
                            facility.type == BuildingComponent::Type::Arcade ||
                            facility.type == BuildingComponent::Type::Theater ||
                            facility.type == BuildingComponent::Type::FlagshipStore) {
                            visitable_facilities.push_back(facility_entity);
                        }
                    });
            
                    // Dynamic spawn rate based on facility count
                    const float spawn_interval = spawner.GetDynamicSpawnInterval(facility_count);
            
                    // Spawn a visitor if enough time has passed
                    if (spawner.time_since_last_spawn >= spawn_interval) {
                        spawner.time_since_last_spawn = 0.0f;
                
                        // Determine visitor type based on job availability
                        VisitorActivity activity;
                        if (total_job_openings > 0 && (rand() % 100) < 40) {
                            // 40% chance to be job-seeking if jobs are available
                            activity = VisitorActivity::JobSeeking;
                        } else {
                            // Otherwise, random activity
                            const int r = rand() % 100;
                            if (r < 60) {
                                activity = VisitorActivity::Shopping;
                            } else {
                                activity = VisitorActivity::Visiting;
                            }
                        }
                
                        // Create the visitor at the lobby entrance
                        std::string visitor_name = "Visitor" + std::to_string(spawner.next_visitor_id++);
                        const auto visitor = e.world().entity(visitor_name.c_str());
                        visitor.set<Person>({visitor_name, 0, 2.0f, 2.0f, NPCType::Visitor});
                        visitor.set<VisitorInfo>({activity});
                        visitor.set<Satisfaction>({75.0f});
                        
                        // Assign random visitor archetype and create needs
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
                
                        // Assign a destination for shopping/visiting visitors
                        if ((activity == VisitorActivity::Shopping || activity == VisitorActivity::Visiting) 
                            && !visitable_facilities.empty()) {
                            // Pick a random facility to visit (using modulo is acceptable for small arrays)
                            const size_t random_index = static_cast<size_t>(rand()) % visitable_facilities.size();
                            const auto target_facility = visitable_facilities[random_index];
                            
                            if (target_facility.has<BuildingComponent>()) {
                                const auto& building = target_facility.ensure<BuildingComponent>();
                                
                                // Set destination to the center of the facility
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
    
        // Job assignment system
        // Assigns job-seeking visitors to open positions
        world_.system<Person, VisitorInfo>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Check every 2 seconds
                .each([](const flecs::entity visitor_entity, Person& person, const VisitorInfo& visitor) {
                    // Only process job-seeking visitors
                    if (visitor.activity != VisitorActivity::JobSeeking) {
                        return;
                    }
            
                    // Check if visitor is at lobby and ready to take a job
                    if (person.current_floor != 0 || person.state != PersonState::Idle) {
                        return;
                    }
            
                    // Find a facility with job openings
                    flecs::entity target_facility;
                    int target_floor = -1;
                    int target_column = -1;
                    std::string job_title;
                    float shift_start = 9.0f;
                    float shift_end = 17.0f;

                    const auto facility_query = visitor_entity.world().query<BuildingComponent>();
                    facility_query.each([&](const flecs::entity facility_entity, BuildingComponent& facility) {
                        // Skip if already found a job or facility has no openings
                        if (target_floor != -1 || !facility.HasJobOpenings()) {
                            return;
                        }
                
                        // Assign job based on facility type
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
                                return;  // Not a job facility
                        }
                
                        // Found a job! Save the details
                        target_facility = facility_entity;
                        target_floor = facility.floor;
                        target_column = facility.column + (facility.width / 2);  // Center of facility (integer division is fine here)
                        facility.job_openings--;  // Reduce opening count
                    });
            
                    // If a job was found, convert visitor to employee
                    if (target_floor != -1) {
                        // Remove visitor component
                        visitor_entity.remove<VisitorInfo>();
                
                        // Add employment component
                        const EmploymentInfo employment(job_title, target_floor, target_column, shift_start, shift_end);
                        visitor_entity.set<EmploymentInfo>(employment);
                
                        // Update person type
                        person.npc_type = NPCType::Employee;
                        person.current_need = "New hire: " + job_title;
                
                        // Update spawner stats
                        if (visitor_entity.world().has<NPCSpawner>()) {
                            auto& spawner = visitor_entity.world().get_mut<NPCSpawner>();
                            spawner.total_employees_hired++;
                        }
                
                        std::cout << "  [Hired] " << person.name << " as " << job_title 
                                << " on Floor " << target_floor << std::endl;
                    }
                });
    
        // Visitor cleanup system
        // Removes visitors who have left the tower
        world_.system<const Person, const VisitorInfo>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Check every 2 seconds
                .each([](const flecs::entity e, const Person& person, const VisitorInfo& visitor) {
                    // If visitor is leaving and has reached the lobby
                    if (visitor.activity == VisitorActivity::Leaving && 
                        person.state == PersonState::AtDestination &&
                        person.current_floor == 0) {
                        // Remove the entity (visitor leaves the tower)
                        e.destruct();
                    }
                });
    
        // Facility status degradation system
        // Updates facility cleanliness and maintenance over time
        world_.system<FacilityStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, FacilityStatus& status, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                    status.Update(delta_time, facility.current_occupancy);
                });
    
        // CleanlinessStatus degradation system
        // Updates cleanliness state based on time and facility usage
        world_.system<CleanlinessStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                
                    // Calculate occupancy factor (busier facilities get dirty faster)
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / 
                                                 std::max(1, facility.capacity);
                    const float occupancy_factor = 1.0f + (occupancy_rate * 2.0f);  // 1x to 3x multiplier
                
                    // Update cleanliness state
                    cleanliness.Update(delta_time, occupancy_factor);
                });
    
        // MaintenanceStatus degradation system
        // Updates maintenance state based on time and facility usage
        world_.system<MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    const float delta_time = e.world().delta_time();
                
                    // Calculate usage factor (busier facilities break down faster)
                    const float occupancy_rate = static_cast<float>(facility.current_occupancy) / 
                                                 std::max(1, facility.capacity);
                    const float usage_factor = 1.0f + (occupancy_rate * 1.5f);  // 1x to 2.5x multiplier
                
                    // Update maintenance state
                    maintenance.Update(delta_time, usage_factor);
                });
    
        // Maintenance breakdown notification system
        // Sends notifications when facilities break down or need service
        world_.system<const MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Check every 5 seconds
                .each([](const flecs::entity e, const MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    // Track which facilities have been notified to avoid spam
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
                        // Clear notification flags when repaired
                        notified_broken.erase(entity_id);
                        notified_needs_service.erase(entity_id);
                    }
                });
    
        // Cleanliness notification system
        // Sends notifications when facilities become dirty
        world_.system<const CleanlinessStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Check every 5 seconds
                .each([](const flecs::entity e, const CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                    // Only notify when facility becomes dirty
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
                        // Clear notification flags when cleaned
                        notified_dirty.erase(entity_id);
                        notified_needs_cleaning.erase(entity_id);
                    }
                });
    
        // Staff shift management system
        // Activates/deactivates staff based on shift times
        world_.system<StaffAssignment, Person>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Check every second
                .each([](const flecs::entity e, StaffAssignment& assignment, Person& person) {
                    const auto& time_mgr = e.world().get<TimeManager>();
                    const bool should_be_working = assignment.ShouldBeWorking(time_mgr.current_hour);
                
                    if (should_be_working && !assignment.is_active) {
                        // Start shift
                        assignment.is_active = true;
                        person.current_need = "Working";
                        std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() 
                                << ") started shift" << std::endl;
                    } else if (!should_be_working && assignment.is_active) {
                        // End shift
                        assignment.is_active = false;
                        person.current_need = "Off Duty";
                        std::cout << "  [Staff] " << person.name << " (" << assignment.GetRoleName() 
                                << ") ended shift" << std::endl;
                    }
                });
    
        // Staff cleaning system
        // Janitors and cleaners automatically clean facilities
        world_.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Check every 5 seconds
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    // Only active staff who do cleaning work
                    if (!assignment.is_active) return;
                    if (!assignment.DoesCleaningWork()) return;
                
                    // Find facilities that need cleaning
                    auto world = staff_entity.world();
                    bool cleaned_something = false;
                
                    // First try to clean facilities with CleanlinessStatus
                    world.each([&](flecs::entity facility_entity, CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
                            if (cleaned_something) return;  // Only clean one facility per cycle
                        
                            // Check if staff is assigned to this facility or floor
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            // Check if facility needs cleaning
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
                
                    // Fallback to FacilityStatus for compatibility
                    if (!cleaned_something) {
                        world.each([&](flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                                if (cleaned_something) return;  // Only clean one facility per cycle
                            
                                // Check if staff is assigned to this facility or floor
                                bool is_assigned = false;
                                if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                    is_assigned = true;
                                } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                    is_assigned = true;
                                }
                            
                                if (!is_assigned) return;
                            
                                // Check if facility needs cleaning
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
    
        // Staff maintenance system
        // Maintenance staff and repairers automatically maintain facilities
        world_.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(8.0f)  // Check every 8 seconds (maintenance is slower)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    // Only active staff who do maintenance work
                    if (!assignment.is_active) return;
                    if (!assignment.DoesMaintenanceWork()) return;
                
                    // Find facilities that need maintenance
                    auto world = staff_entity.world();
                    bool maintained_something = false;
                
                    world.each([&](flecs::entity facility_entity, FacilityStatus& status, const BuildingComponent& facility) {
                            if (maintained_something) return;
                        
                            // Check if staff is assigned to this facility or floor
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            // Check if facility needs maintenance
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
    
        // Staff maintenance system for MaintenanceStatus
        // Maintenance staff and repairers automatically repair facilities with MaintenanceStatus
        world_.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(8.0f)  // Check every 8 seconds (maintenance is slower)
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    // Only active staff who do maintenance work
                    if (!assignment.is_active) return;
                    if (!assignment.DoesMaintenanceWork()) return;
                
                    // Find facilities that need maintenance
                    auto world = staff_entity.world();
                    bool repaired_something = false;
                
                    world.each([&](flecs::entity facility_entity, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                            if (repaired_something) return;  // Only repair one facility per cycle
                        
                            // Check if staff is assigned to this facility or floor
                            bool is_assigned = false;
                            if (assignment.assigned_facility_entity == static_cast<int>(facility_entity.id())) {
                                is_assigned = true;
                            } else if (assignment.assigned_floor == -1 || assignment.assigned_floor == facility.floor) {
                                is_assigned = true;
                            }
                        
                            if (!is_assigned) return;
                        
                            // Check if facility needs service or is broken
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
    
        // Staff firefighting system
        // Firefighters respond to fires
        world_.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Check frequently for fires
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    // Check if this is a firefighter (built-in or custom with emergency work type)
                    if (assignment.role != StaffRole::Firefighter && !assignment.DoesEmergencyWork()) return;
                
                    // Find facilities with fires
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
    
        // Staff security system
        // Security guards handle security issues
        world_.system<const StaffAssignment, const Person>()
                .kind(flecs::OnUpdate)
                .interval(3.0f)  // Check every 3 seconds
                .each([](const flecs::entity staff_entity, const StaffAssignment& assignment, const Person& person) {
                    if (!assignment.is_active) return;
                    // Check if this is security (built-in or custom with emergency work type)
                    if (assignment.role != StaffRole::Security && !assignment.DoesEmergencyWork()) return;
                
                    // Find facilities with security issues
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
    
        // Facility status impact on satisfaction system
        // Poor cleanliness/maintenance reduces satisfaction (cozy, not punitive)
        world_.system<Satisfaction, const FacilityStatus>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Update every 2 seconds
                .each([](const flecs::entity e, Satisfaction& satisfaction, const FacilityStatus& status) {
                    // Gentle penalties for poor conditions
                    if (status.cleanliness < 50.0f) {
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    }
                    if (status.maintenance_level < 50.0f) {
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    }
                
                    // Small bonus for excellent conditions
                    if (status.cleanliness >= 90.0f && status.maintenance_level >= 90.0f) {
                        satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.5f);
                    }
                });
    
        // CleanlinessStatus impact on satisfaction system
        // Dirty facilities gently reduce satisfaction (cozy, not punitive)
        world_.system<Satisfaction, const CleanlinessStatus>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Update every 2 seconds
                .each([](const flecs::entity e, Satisfaction& satisfaction, const CleanlinessStatus& cleanliness) {
                    // Gentle, progressive penalties based on cleanliness state
                    switch (cleanliness.status) {
                        case CleanlinessStatus::State::Dirty:
                            // Dirty facilities have a noticeable but not severe impact
                            satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 2.0f);
                            break;
                        case CleanlinessStatus::State::NeedsCleaning:
                            // Facilities that need cleaning have a small impact
                            satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 0.5f);
                            break;
                        case CleanlinessStatus::State::Clean:
                            // Clean facilities get a small bonus
                            satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.3f);
                            break;
                    }
                });
    
        // Broken facility impact system
        // Broken facilities stop generating revenue and reduce satisfaction for nearby tenants
        world_.system<const MaintenanceStatus, BuildingComponent, Satisfaction>()
                .kind(flecs::OnUpdate)
                .interval(2.0f)  // Update every 2 seconds
                .each([](const flecs::entity e, const MaintenanceStatus& maintenance, 
                         BuildingComponent& facility, Satisfaction& satisfaction) {
                    if (maintenance.IsBroken()) {
                        // Broken facilities are non-operational
                        // Mark facility as having issues by reducing current occupancy to 0
                        // (This simulates people avoiding/leaving the broken facility)
                        if (facility.current_occupancy > 0) {
                            facility.current_occupancy = 0;
                        }
                    
                        // Reduce satisfaction significantly (but not catastrophically)
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 3.0f);
                    } else if (maintenance.status == MaintenanceStatus::State::NeedsService) {
                        // Facilities needing service have minor impact
                        satisfaction.quality_bonus = std::max(0.0f, satisfaction.quality_bonus - 1.0f);
                    } else if (maintenance.status == MaintenanceStatus::State::Good) {
                        // Good maintenance provides small bonus
                        satisfaction.quality_bonus = std::min(20.0f, satisfaction.quality_bonus + 0.2f);
                    }
                });
    
        // Auto-repair system
        // Automatically repairs broken facilities if auto_repair is enabled and funds are available
        world_.system<MaintenanceStatus, const BuildingComponent>()
                .kind(flecs::OnUpdate)
                .interval(10.0f)  // Check every 10 seconds
                .each([](const flecs::entity e, MaintenanceStatus& maintenance, const BuildingComponent& facility) {
                    // Only auto-repair if enabled and facility is broken or needs service
                    if (!maintenance.auto_repair_enabled) return;
                    if (!maintenance.NeedsService()) return;
                
                    // Get TowerEconomy singleton from world
                    auto world = e.world();
                    if (!world.has<TowerEconomy>()) return;
                
                    const TowerEconomy& economy = world.get<TowerEconomy>();
                
                    // Repair cost constants
                    constexpr float BASE_REPAIR_COST = 100.0f;
                    constexpr float BROKEN_REPAIR_COST = 500.0f;
                    constexpr float ELEVATOR_COST_MULTIPLIER = 2.0f;
                    constexpr float COMPLEX_FACILITY_MULTIPLIER = 1.5f;
                
                    // Calculate repair cost based on facility type and severity
                    float repair_cost = maintenance.IsBroken() ? BROKEN_REPAIR_COST : BASE_REPAIR_COST;
                
                    // Different facility types have different repair costs
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
                
                    // Only repair if we can afford it
                    if (economy.total_balance >= repair_cost) {
                        // Deduct cost from tower economy
                        auto& mut_economy = world.ensure<TowerEconomy>();
                        mut_economy.total_balance -= repair_cost;
                        mut_economy.total_expenses += repair_cost;
                    
                        // Perform repair
                        maintenance.Repair();
                    
                        const char* facility_type = FacilityManager::GetTypeName(facility.type);
                        std::cout << "  [Auto-Repair] " << facility_type << " on Floor " << facility.floor 
                                << " repaired automatically (Cost: $" << static_cast<int>(repair_cost) << ")" << std::endl;
                    }
                });
    
        // Staff manager update system
        // Updates staff counts and wages
        world_.system<StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(5.0f)  // Update every 5 seconds
                .each([](const flecs::entity e, StaffManager& manager) {
                    auto world = e.world();
                
                    // Reset counts
                    manager.total_staff_count = 0;
                    manager.firefighters = 0;
                    manager.security_guards = 0;
                    manager.janitors = 0;
                    manager.maintenance_staff = 0;
                    manager.cleaners = 0;
                    manager.repairers = 0;
                
                    // Count staff by role
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
                
                    // Calculate wages (simplified: $50/day per staff member)
                    manager.total_staff_wages = manager.total_staff_count * 50.0f;
                });
    
        // Staff wage system
        // Deducts staff wages from tower economy
        world_.system<const StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(1.0f)  // Update every second
                .each([](const flecs::entity e, const StaffManager& manager) {
                    if (manager.total_staff_count == 0) return;
                
                    auto& mut_economy = e.world().ensure<TowerEconomy>();
                
                    // Add wages to daily expenses (spread over 24 hours)
                    const float wage_per_second = manager.total_staff_wages / (24.0f * 3600.0f);
                    mut_economy.daily_expenses += wage_per_second;
                });
    
        // Staff status reporting system
        world_.system<const StaffManager>()
                .kind(flecs::OnUpdate)
                .interval(30.0f)  // Report every 30 seconds
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
    
        std::cout << "  Registered systems: Time Simulation, Schedule Execution, Movement, Actor Logging, Building Occupancy Monitor, Satisfaction Update, Satisfaction Reporting, Facility Economics, Daily Economy Processing, Revenue Collection, Economic Status Reporting, Person Horizontal Movement, Person Waiting, Person Elevator Riding, Person State Logging, Elevator Car Movement, Elevator Call, Person Elevator Boarding, Elevator Logging, Research Points Award, Visitor Needs Growth, Visitor Needs-Driven Behavior, Visitor Facility Interaction, Visitor Satisfaction Calculation, Visitor Behavior, Visitor Needs Display, Employee Shift Management, Employee Off-Duty Visitor, Job Opening Tracking, Visitor Spawning, Job Assignment, Visitor Cleanup, Facility Status Degradation, CleanlinessStatus Degradation, MaintenanceStatus Degradation, Maintenance Breakdown Notification, Cleanliness Notification, Staff Shift Management, Staff Cleaning, Staff Maintenance (FacilityStatus), Staff Maintenance (MaintenanceStatus), Staff Firefighting, Staff Security, Facility Status Impact, CleanlinessStatus Impact, Broken Facility Impact, Auto-Repair, Staff Manager Update, Staff Wages, Staff Status Reporting" << std::endl;
    }

}
