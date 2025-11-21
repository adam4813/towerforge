#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/lua_mod_manager.hpp"
#include "core/systems/time_systems.hpp"
#include "core/systems/movement_systems.hpp"
#include "core/systems/economy_systems.hpp"
#include "core/systems/person_elevator_systems.hpp"
#include "core/systems/visitor_employee_systems.hpp"
#include "core/systems/facility_systems.hpp"
#include "core/systems/staff_systems.hpp"
#include <iostream>
#include <set>

namespace towerforge::core {

    ECSWorld::ECSWorld() 
        : tower_grid_(std::make_unique<TowerGrid>(1, 20, 0)) {  // Start with 1 floor (ground), 20 columns, ground at index 0
    }

    ECSWorld::ECSWorld(const int screen_width, const int screen_height, const int cell_width, const int cell_height) {
        // Calculate initial dimensions based on screen size
        // Horizontal cells: approximately one screen width of cells
        const int initial_columns = std::min(screen_width / cell_width, MAX_HORIZONTAL_CELLS);

        const int total_screen_floors = screen_height / cell_height;
        
        // Below ground floors: 1 screen height worth of floors
        const int initial_below_ground = std::min(total_screen_floors / 2, MAX_BELOW_GROUND_FLOORS);
        
        // Above ground floors: 3 screen heights worth of floors
        const int initial_above_ground = std::min(total_screen_floors, MAX_ABOVE_GROUND_FLOORS);
        
        // Total initial floors = ground floor (1) + above ground + below ground
        const int initial_floors = 1 + initial_above_ground + initial_below_ground;
        
        // Create tower grid with calculated dimensions
        tower_grid_ = std::make_unique<TowerGrid>(initial_floors, initial_columns, 0);
        
        // Set the initial upgrade limits
        tower_grid_->SetMaxAboveGroundFloors(MAX_ABOVE_GROUND_FLOORS);
        tower_grid_->SetMaxBelowGroundFloors(MAX_BELOW_GROUND_FLOORS);
        
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
        Systems::TimeSystems::RegisterAll(world_);
        Systems::MovementSystems::RegisterAll(world_);
        Systems::EconomySystems::RegisterAll(world_);
        Systems::PersonElevatorSystems::RegisterAll(world_);
        Systems::VisitorEmployeeSystems::RegisterAll(world_);
        Systems::FacilitySystems::RegisterAll(world_);
        Systems::StaffSystems::RegisterAll(world_);
    
        std::cout << "  Registered systems: Time Simulation, Schedule Execution, Movement, Actor Logging, Building Occupancy Monitor, Satisfaction Update, Satisfaction Reporting, Facility Economics, Daily Economy Processing, Revenue Collection, Economic Status Reporting, Person Horizontal Movement, Person Waiting, Person Elevator Riding, Person State Logging, Elevator Car Movement, Elevator Call, Person Elevator Boarding, Elevator Logging, Research Points Award, Visitor Needs Growth, Visitor Needs-Driven Behavior, Visitor Facility Interaction, Visitor Satisfaction Calculation, Visitor Behavior, Visitor Needs Display, Employee Shift Management, Employee Off-Duty Visitor, Job Opening Tracking, Visitor Spawning, Job Assignment, Visitor Cleanup, Facility Status Degradation, CleanlinessStatus Degradation, MaintenanceStatus Degradation, Maintenance Breakdown Notification, Cleanliness Notification, Staff Shift Management, Staff Cleaning, Staff Maintenance (FacilityStatus), Staff Maintenance (MaintenanceStatus), Staff Firefighting, Staff Security, Facility Status Impact, CleanlinessStatus Impact, Broken Facility Impact, Auto-Repair, Staff Manager Update, Staff Wages, Staff Status Reporting" << std::endl;
    }


}
