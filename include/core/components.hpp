#pragma once

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace TowerForge::Core {

    // Forward declaration
    class TowerGrid;

    /**
 * @brief Component for entities with a position in 2D space
 */
    struct Position {
        float x;
        float y;

        Position(const float x = 0.0f, const float y = 0.0f) : x(x), y(y) {}
    };

    /**
 * @brief Component for entities with velocity
 */
    struct Velocity {
        float dx;
        float dy;

        Velocity(const float dx = 0.0f, const float dy = 0.0f) : dx(dx), dy(dy) {}
    };

    /**
 * @brief Component for actors (people) in the building
 * 
 * Actors represent people who move around the building,
 * use elevators, and interact with facilities.
 */
    struct Actor {
        std::string name;
        int floor_destination;  // Target floor
        float speed;            // Movement speed

        Actor(const std::string& name = "Actor", const int dest = 0, const float spd = 1.0f)
            : name(name), floor_destination(dest), speed(spd) {}
    };

    /**
 * @brief State machine states for Person entities
 * 
 * Defines the possible states a person can be in while moving through the tower.
 */
    enum class PersonState {
        Idle,                  // Standing still, no destination
        Walking,               // Moving horizontally on same floor
        WaitingForElevator,    // Waiting for elevator to arrive
        InElevator,            // Currently in an elevator
        AtDestination          // Reached final destination
    };

    /**
 * @brief NPC type classification
 */
    enum class NPCType {
        Visitor,               // Temporary visitor (shopping, sightseeing, etc.)
        Employee               // Employee with a job in the tower
    };

    /**
 * @brief Activity type for visitors
 */
    enum class VisitorActivity {
        Shopping,              // Browsing shops
        JobSeeking,            // Looking for employment
        Visiting,              // General visiting
        Leaving                // Exiting the tower
    };

    /**
 * @brief Component for Person entities with state machine and movement tracking
 * 
 * This component extends Actor with detailed state tracking for simulation
 * of individuals moving through the tower. People can walk on floors,
 * use elevators, and have specific destinations.
 */
    struct Person {
        std::string name;
        PersonState state;
        NPCType npc_type;         // Type of NPC (visitor or employee)
    
        // Current location
        int current_floor;
        float current_column;     // Float for smooth horizontal movement
    
        // Destination
        int destination_floor;
        float destination_column;
    
        // Movement
        float move_speed;         // Horizontal movement speed (columns per second)
        float wait_time;          // Time spent waiting (e.g., for elevator)
    
        // Needs/goals
        std::string current_need; // What the person is trying to do
    
        Person(const std::string& n = "Person",
               const int floor = 0,
               const float col = 0.0f,
               const float speed = 2.0f,
               const NPCType type = NPCType::Visitor)
            : name(n),
              state(PersonState::Idle),
              npc_type(type),
              current_floor(floor),
              current_column(col),
              destination_floor(floor),
              destination_column(col),
              move_speed(speed),
              wait_time(0.0f),
              current_need("Idle") {}
    
        /**
     * @brief Get the state as a string for debugging
     */
        const char* GetStateString() const {
            switch (state) {
                case PersonState::Idle:                return "Idle";
                case PersonState::Walking:             return "Walking";
                case PersonState::WaitingForElevator:  return "WaitingForElevator";
                case PersonState::InElevator:          return "InElevator";
                case PersonState::AtDestination:       return "AtDestination";
                default:                               return "Unknown";
            }
        }
    
        /**
     * @brief Check if person has reached their horizontal destination on current floor
     */
        bool HasReachedHorizontalDestination() const {
            return std::abs(current_column - destination_column) < 0.1f;
        }
    
        /**
     * @brief Check if person has reached their vertical destination
     */
        bool HasReachedVerticalDestination() const {
            return current_floor == destination_floor;
        }
    
        /**
     * @brief Set a new destination on a different floor
     */
        void SetDestination(const int floor, const float column, const std::string& need = "Moving") {
            destination_floor = floor;
            destination_column = column;
            current_need = need;
        
            // Update state based on destination
            if (floor != current_floor) {
                // Need to change floors
                state = PersonState::WaitingForElevator;
            } else if (!HasReachedHorizontalDestination()) {
                // Same floor, different column
                state = PersonState::Walking;
            } else {
                // Already at destination
                state = PersonState::AtDestination;
            }
        }
    };

    /**
 * @brief Visitor profile archetypes
 */
    enum class VisitorArchetype {
        BusinessPerson,    // Focused on efficiency, prefers offices and food
        Tourist,           // Seeks entertainment and sightseeing
        Shopper,           // Primarily interested in retail experiences
        Casual             // General visitor with balanced needs
    };

    /**
 * @brief Component tracking visitor needs
 * 
 * Each need ranges from 0.0 (fully satisfied) to 100.0 (critical).
 * Visitors seek facilities to reduce their needs.
 */
    struct VisitorNeeds {
        float hunger;         // Need for food/restaurants
        float entertainment;  // Need for fun/arcades/theaters
        float comfort;        // Need for rest/hotels
        float shopping;       // Desire to shop/browse retail

        VisitorArchetype archetype;  // Visitor personality type

        VisitorNeeds(const VisitorArchetype type = VisitorArchetype::Casual)
            : hunger(0.0f),
              entertainment(0.0f),
              comfort(0.0f),
              shopping(0.0f),
              archetype(type) {
            // Initialize random needs based on archetype
            InitializeForArchetype();
        }

        /**
     * @brief Initialize needs based on visitor archetype
     */
        void InitializeForArchetype() {
            switch (archetype) {
                case VisitorArchetype::BusinessPerson:
                    hunger = 30.0f + (rand() % 20);      // Moderate hunger
                    entertainment = 10.0f + (rand() % 10); // Low entertainment need
                    comfort = 20.0f + (rand() % 15);      // Some comfort need
                    shopping = 5.0f + (rand() % 10);      // Low shopping interest
                    break;
                case VisitorArchetype::Tourist:
                    hunger = 20.0f + (rand() % 15);       // Moderate hunger
                    entertainment = 40.0f + (rand() % 30); // High entertainment need
                    comfort = 25.0f + (rand() % 20);      // Moderate comfort need
                    shopping = 30.0f + (rand() % 20);     // Moderate shopping interest
                    break;
                case VisitorArchetype::Shopper:
                    hunger = 15.0f + (rand() % 15);       // Low hunger initially
                    entertainment = 20.0f + (rand() % 15); // Moderate entertainment
                    comfort = 15.0f + (rand() % 10);      // Low comfort need
                    shopping = 50.0f + (rand() % 30);     // High shopping desire
                    break;
                case VisitorArchetype::Casual:
                default:
                    hunger = 25.0f + (rand() % 20);       // Balanced needs
                    entertainment = 25.0f + (rand() % 20);
                    comfort = 25.0f + (rand() % 20);
                    shopping = 25.0f + (rand() % 20);
                    break;
            }
        }

        /**
     * @brief Get the highest need value
     */
        float GetHighestNeed() const {
            return std::max({hunger, entertainment, comfort, shopping});
        }

        /**
     * @brief Get the type of the highest need
     */
        const char* GetHighestNeedType() const {
            const float max_need = GetHighestNeed();
            if (max_need == hunger) return "Hunger";
            if (max_need == entertainment) return "Entertainment";
            if (max_need == comfort) return "Comfort";
            if (max_need == shopping) return "Shopping";
            return "None";
        }

        /**
     * @brief Get archetype as string
     */
        const char* GetArchetypeString() const {
            switch (archetype) {
                case VisitorArchetype::BusinessPerson: return "Business Person";
                case VisitorArchetype::Tourist: return "Tourist";
                case VisitorArchetype::Shopper: return "Shopper";
                case VisitorArchetype::Casual: return "Casual Visitor";
                default: return "Unknown";
            }
        }

        /**
     * @brief Update needs over time (they increase)
     */
        void UpdateNeeds(const float delta_time) {
            // Needs grow at different rates based on archetype
            float hunger_rate = 2.0f;
            float entertainment_rate = 1.5f;
            float comfort_rate = 1.0f;
            float shopping_rate = 1.0f;

            switch (archetype) {
                case VisitorArchetype::BusinessPerson:
                    hunger_rate = 3.0f;  // Busy people get hungry faster
                    break;
                case VisitorArchetype::Tourist:
                    entertainment_rate = 2.5f;  // Tourists want more fun
                    comfort_rate = 2.0f;         // And more rest
                    break;
                case VisitorArchetype::Shopper:
                    shopping_rate = 2.5f;  // Shoppers want to shop
                    break;
                case VisitorArchetype::Casual:
                default:
                    // Use default rates
                    break;
            }

            hunger += hunger_rate * delta_time;
            entertainment += entertainment_rate * delta_time;
            comfort += comfort_rate * delta_time;
            shopping += shopping_rate * delta_time;

            // Clamp to maximum
            hunger = std::min(100.0f, hunger);
            entertainment = std::min(100.0f, entertainment);
            comfort = std::min(100.0f, comfort);
            shopping = std::min(100.0f, shopping);
        }

        /**
     * @brief Reduce a specific need
     */
        void ReduceNeed(const char* need_type, const float amount) {
            if (strcmp(need_type, "Hunger") == 0) {
                hunger = std::max(0.0f, hunger - amount);
            } else if (strcmp(need_type, "Entertainment") == 0) {
                entertainment = std::max(0.0f, entertainment - amount);
            } else if (strcmp(need_type, "Comfort") == 0) {
                comfort = std::max(0.0f, comfort - amount);
            } else if (strcmp(need_type, "Shopping") == 0) {
                shopping = std::max(0.0f, shopping - amount);
            }
        }
    };

    /**
 * @brief Component for visitor NPCs
 * 
 * Tracks visitor-specific information like their activity and visit duration.
 */
    struct VisitorInfo {
        VisitorActivity activity;      // What the visitor is currently doing
        float visit_duration;          // How long they've been in the tower (seconds)
        float max_visit_duration;      // When they'll leave (seconds)
        int target_facility_floor;     // Floor of facility they're visiting (-1 if none)
        float time_at_destination;     // Time spent at current destination (seconds)
        bool is_interacting;           // Currently using a facility
        float interaction_time;        // Time spent at current facility
        float required_interaction_time; // How long to interact with facility
    
        VisitorInfo(const VisitorActivity act = VisitorActivity::Visiting)
            : activity(act),
              visit_duration(0.0f),
              max_visit_duration(300.0f),  // 5 minutes default
              target_facility_floor(-1),
              time_at_destination(0.0f),
              is_interacting(false),
              interaction_time(0.0f),
              required_interaction_time(20.0f) {}  // 20 seconds default
    
        /**
     * @brief Get the activity as a string
     */
        const char* GetActivityString() const {
            switch (activity) {
                case VisitorActivity::Shopping:    return "Shopping";
                case VisitorActivity::JobSeeking:  return "Job Seeking";
                case VisitorActivity::Visiting:    return "Visiting";
                case VisitorActivity::Leaving:     return "Leaving";
                default:                           return "Unknown";
            }
        }
    
        /**
     * @brief Check if visitor should leave
     */
        bool ShouldLeave() const {
            return visit_duration >= max_visit_duration;
        }
    };

    /**
 * @brief Component for employee NPCs
 * 
 * Tracks employment information including workplace, job title, and shift schedule.
 */
    struct EmploymentInfo {
        std::string job_title;         // Job title (e.g., "Office Worker", "Shop Clerk")
        int workplace_floor;           // Floor where they work
        int workplace_column;          // Column where workplace is located
    
        // Shift schedule (simple 5-day week)
        float shift_start_hour;        // Hour when shift starts (e.g., 9.0 for 9 AM)
        float shift_end_hour;          // Hour when shift ends (e.g., 17.0 for 5 PM)
        std::vector<int> work_days;    // Days of week they work (0=Monday, 6=Sunday)
    
        bool currently_on_shift;       // Whether employee is currently working
    
        EmploymentInfo(const std::string& title = "Employee",
                       const int floor = 0,
                       const int col = 0,
                       const float start_hour = 9.0f,
                       const float end_hour = 17.0f)
            : job_title(title),
              workplace_floor(floor),
              workplace_column(col),
              shift_start_hour(start_hour),
              shift_end_hour(end_hour),
              currently_on_shift(false) {
            // Default to Monday-Friday (0-4)
            work_days = {0, 1, 2, 3, 4};
        }
    
        /**
     * @brief Check if employee should be working based on current time
     */
        bool ShouldBeWorking(const float current_hour, const int current_day) const {
            // Check if today is a work day
            bool is_work_day = false;
            for (const int day : work_days) {
                if (day == current_day) {
                    is_work_day = true;
                    break;
                }
            }
        
            if (!is_work_day) {
                return false;
            }
        
            // Check if current time is within shift hours
            return current_hour >= shift_start_hour && current_hour < shift_end_hour;
        }
    
        /**
     * @brief Get status string for UI display
     */
        std::string GetStatusString() const {
            if (currently_on_shift) {
                return "On shift: " + job_title;
            } else {
                return "Off duty";
            }
        }
    };

    /**
 * @brief Component for building components (offices, residences, shops, etc.)
 * 
 * BuildingComponents represent the various facilities and rooms
 * that can be placed in the tower.
 * 
 * Facility Types:
 * - Office: Commercial workspace for office workers. Generates rent income.
 * - Residential: Condominiums for tower residents. Provides housing and generates rent.
 * - RetailShop: Commercial shops selling goods/services. Generates retail income.
 * - Lobby: Main entrance/exit point for the tower. Required on ground floor.
 * - Restaurant: Food service facility. Requires cooks and servers.
 * - Hotel: Temporary lodging. Requires receptionists and cleaning staff.
 * - Elevator: Vertical transportation.
 * - Gym: Fitness and wellness center. Requires trainers/attendants.
 * - Arcade: Entertainment venue. Requires clerks.
 * - Theater: Entertainment venue. Requires ushers.
 * - ConferenceHall: Event space. Requires event coordinators.
 * - FlagshipStore: Large retail store. Requires multiple shop staff.
 */
    struct BuildingComponent {
        enum class Type {
            Office,           // Commercial office space
            Residential,      // Residential condominiums
            RetailShop,       // Retail shop
            Lobby,            // Main entrance/lobby
            Restaurant,       // Food service
            Hotel,            // Hotel rooms
            Elevator,         // Vertical transport
            Gym,              // Fitness/wellness center
            Arcade,           // Arcade entertainment
            Theater,          // Theater entertainment
            ConferenceHall,   // Conference/event space
            FlagshipStore,    // Large retail store
            ManagementOffice, // Tower management office (generates tower points)
            SatelliteOffice   // Satellite management office (boosts tower points generation)
        };

        Type type;
        int floor;              // Which floor this component is on
        int column;             // Which column this component starts at
        int width;              // Width in tiles
        int capacity;           // Maximum occupancy
        int current_occupancy;  // Current number of people
        int job_openings;       // Number of unfilled jobs at this facility
        int current_staff;      // Current number of staff assigned
        float operating_start_hour;  // Start of operating hours (e.g., 9.0 for 9 AM)
        float operating_end_hour;    // End of operating hours (e.g., 21.0 for 9 PM)

        BuildingComponent(const Type t = Type::Office, const int f = 0, const int col = 0, const int w = 1, const int cap = 10)
            : type(t), floor(f), column(col), width(w), capacity(cap), current_occupancy(0), job_openings(0),
              current_staff(0), operating_start_hour(9.0f), operating_end_hour(17.0f) {}
    
        /**
     * @brief Get the number of employees needed for this facility type
     */
        int GetRequiredEmployees() const {
            switch (type) {
                case Type::Office:           return capacity / 5;  // 1 employee per 5 capacity
                case Type::RetailShop:       return 2;             // Shops need 2 employees
                case Type::Restaurant:       return 4;             // Restaurants need 4 employees (cooks, servers)
                case Type::Hotel:            return 5;             // Hotels need 5 employees (receptionists, cleaners)
                case Type::Gym:              return 3;             // Gyms need 3 employees (trainers/attendants)
                case Type::Arcade:           return 2;             // Arcades need 2 employees (clerks)
                case Type::Theater:          return 3;             // Theaters need 3 employees (ushers, staff)
                case Type::ConferenceHall:   return 2;             // Conference halls need 2 employees (coordinators)
                case Type::FlagshipStore:    return 4;             // Flagship stores need 4 employees (staff)
                case Type::ManagementOffice: return 5;             // Management offices need 5 staff (executives, managers, analysts)
                case Type::SatelliteOffice:  return 3;             // Satellite offices need 3 staff (managers, analysts)
                default:                     return 0;             // Other types don't need employees
            }
        }
    
        /**
     * @brief Check if this is a management facility that generates tower points
     */
        bool IsManagementFacility() const {
            return type == Type::ManagementOffice || type == Type::SatelliteOffice;
        }
    
        /**
     * @brief Check if this facility has job openings
     */
        bool HasJobOpenings() const {
            return job_openings > 0;
        }
    
        /**
     * @brief Check if facility is operational (has minimum staff and within operating hours)
     */
        bool IsOperational(const float current_hour) const {
            // Facilities that don't need staff are always operational
            const int required_staff = GetRequiredEmployees();
            if (required_staff == 0) {
                return true;
            }
        
            // Check if we have minimum staff
            if (current_staff < required_staff) {
                return false;
            }
        
            // Check if within operating hours
            if (current_hour < operating_start_hour || current_hour >= operating_end_hour) {
                return false;
            }
        
            return true;
        }
    };

    /**
 * @brief Global singleton component for tower-wide NPC spawning
 * 
 * Manages spawning of visitors and tracking of available jobs.
 */
    struct NPCSpawner {
        float time_since_last_spawn;        // Time since last visitor spawn
        float spawn_interval;               // Base interval between spawns (seconds)
        int total_visitors_spawned;         // Total count of spawned visitors
        int total_employees_hired;          // Total count of employees hired
        int next_visitor_id;                // ID counter for naming visitors
        int max_active_visitors;            // Maximum number of active visitors at once
    
        NPCSpawner(const float interval = 30.0f, const int max_visitors = 50)
            : time_since_last_spawn(0.0f),
              spawn_interval(interval),
              total_visitors_spawned(0),
              total_employees_hired(0),
              next_visitor_id(1),
              max_active_visitors(max_visitors) {}
    
        /**
     * @brief Calculate dynamic spawn rate based on tower state
     */
        float GetDynamicSpawnInterval(const int facility_count) const {
            // More facilities = more visitors
            // Base: 30 seconds, decreases to 10 seconds with 10+ facilities
            const float adjusted = spawn_interval * (1.0f - (facility_count * 0.02f));
            return std::max(10.0f, std::min(60.0f, adjusted));
        }
    };

    /**
 * @brief Global singleton component for managing simulation time
 * 
 * This component tracks the current simulation time, including hours,
 * days, and weeks. It also manages the simulation speed for pause
 * and fast-forward functionality.
 */
    struct TimeManager {
        float current_hour;        // Current hour of the day (0.0 - 24.0)
        int current_day;           // Current day of the week (0 = Monday, 6 = Sunday)
        int current_week;          // Current week number
        float simulation_speed;    // Speed multiplier (0.0 = paused, 1.0 = normal, 2.0 = 2x speed)
        float hours_per_second;    // How many in-game hours pass per real-time second
    
        TimeManager(const float hours_per_sec = 1.0f)
            : current_hour(8.0f),      // Start at 8 AM
              current_day(0),          // Start on Monday
              current_week(1),         // Start at week 1
              simulation_speed(1.0f),  // Normal speed
              hours_per_second(hours_per_sec) {}
    
        /**
     * @brief Get the current time as a formatted string (HH:MM)
     */
        std::string GetTimeString() const {
            const int hours = static_cast<int>(current_hour);
            const int minutes = static_cast<int>((current_hour - hours) * 60.0f);
            char buffer[6];
            snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
            return std::string(buffer);
        }
    
        /**
     * @brief Get the current day name
     */
        const char* GetDayName() const {
            static const char* day_names[] = {
                "Monday", "Tuesday", "Wednesday", "Thursday", 
                "Friday", "Saturday", "Sunday"
            };
            return day_names[current_day % 7];
        }
    
        /**
     * @brief Check if current time is within business hours (9 AM - 5 PM)
     */
        bool IsBusinessHours() const {
            return current_hour >= 9.0f && current_hour < 17.0f;
        }
    
        /**
     * @brief Check if it's a weekend
     */
        bool IsWeekend() const {
            return current_day == 5 || current_day == 6;  // Saturday or Sunday
        }
    };

    /**
 * @brief Scheduled action that can be triggered at a specific time
 */
    struct ScheduledAction {
        enum class Type {
            ArriveWork,     // Arrive at work location
            LeaveWork,      // Leave work and go home
            LunchBreak,     // Take lunch break
            Idle,           // No specific action, idle behavior
            Custom          // Custom action (for future extensibility)
        };
    
        Type type;
        float trigger_hour;  // Hour of day when this action should trigger (0.0 - 24.0)
    
        ScheduledAction(const Type t = Type::Idle, const float hour = 9.0f)
            : type(t), trigger_hour(hour) {}
    };

    /**
 * @brief Component for entities with daily/weekly schedules
 * 
 * This component allows entities to have routines that trigger
 * at specific times of day. Schedules can differ for weekdays
 * and weekends.
 */
    struct DailySchedule {
        std::vector<ScheduledAction> weekday_schedule;  // Actions for Monday-Friday
        std::vector<ScheduledAction> weekend_schedule;  // Actions for Saturday-Sunday
        float last_triggered_hour;                      // Last hour when a schedule was triggered
    
        DailySchedule()
            : last_triggered_hour(-1.0f) {}
    
        /**
     * @brief Add an action to the weekday schedule
     */
        void AddWeekdayAction(const ScheduledAction::Type type, const float hour) {
            weekday_schedule.push_back(ScheduledAction(type, hour));
        }
    
        /**
     * @brief Add an action to the weekend schedule
     */
        void AddWeekendAction(const ScheduledAction::Type type, const float hour) {
            weekend_schedule.push_back(ScheduledAction(type, hour));
        }
    
        /**
     * @brief Get the appropriate schedule based on whether it's a weekend
     */
        const std::vector<ScheduledAction>& GetActiveSchedule(const bool is_weekend) const {
            return is_weekend ? weekend_schedule : weekday_schedule;
        }
    };
  
    /**
 * @brief Component for grid-based position
 * 
 * Represents a position in the tower's grid system.
 */
    struct GridPosition {
        int floor;
        int column;
        int width;  // Width occupied in grid cells
    
        GridPosition(const int f = 0, const int c = 0, const int w = 1)
            : floor(f), column(c), width(w) {}
    };

    /**
 * @brief Component for tenant satisfaction
 * 
 * Tracks satisfaction levels for tenants based on various factors
 * like wait times, noise, facility quality, and crowding.
 */
    struct Satisfaction {
        enum class Level {
            VeryPoor,     // 0-20%
            Poor,         // 21-40%
            Average,      // 41-60%
            Good,         // 61-80%
            Excellent     // 81-100%
        };
    
        float satisfaction_score;  // 0.0 to 100.0
        float wait_time_penalty;   // Accumulated penalty from elevator wait times
        float crowding_penalty;    // Penalty from facility overcrowding
        float noise_penalty;       // Penalty from noise levels
        float quality_bonus;       // Bonus from facility quality
    
        Satisfaction(const float initial_score = 75.0f)
            : satisfaction_score(initial_score),
              wait_time_penalty(0.0f),
              crowding_penalty(0.0f),
              noise_penalty(0.0f),
              quality_bonus(0.0f) {}
    
        /**
     * @brief Get the satisfaction level category
     */
        Level GetLevel() const {
            if (satisfaction_score <= 20.0f) return Level::VeryPoor;
            if (satisfaction_score <= 40.0f) return Level::Poor;
            if (satisfaction_score <= 60.0f) return Level::Average;
            if (satisfaction_score <= 80.0f) return Level::Good;
            return Level::Excellent;
        }
    
        /**
     * @brief Get the satisfaction level as a string
     */
        const char* GetLevelString() const {
            switch (GetLevel()) {
                case Level::VeryPoor: return "Very Poor";
                case Level::Poor: return "Poor";
                case Level::Average: return "Average";
                case Level::Good: return "Good";
                case Level::Excellent: return "Excellent";
                default: return "Unknown";
            }
        }
    
        /**
     * @brief Update satisfaction based on accumulated factors
     */
        void UpdateScore() {
            // Calculate total penalty/bonus
            const float total_change = quality_bonus - (wait_time_penalty + crowding_penalty + noise_penalty);
            satisfaction_score += total_change * 0.1f;  // Gradual change
        
            // Clamp to valid range
            if (satisfaction_score < 0.0f) satisfaction_score = 0.0f;
            if (satisfaction_score > 100.0f) satisfaction_score = 100.0f;
        
            // Decay penalties over time
            wait_time_penalty *= 0.95f;
            crowding_penalty *= 0.95f;
            noise_penalty *= 0.95f;
        }
    };

    /**
 * @brief Component for facility economics
 * 
 * Tracks revenue, costs, and rent for building facilities.
 */
    struct FacilityEconomics {
        float base_rent;           // Base rent per tenant per day
        float revenue;             // Accumulated revenue
        float operating_cost;      // Daily operating cost
        float quality_multiplier;  // Multiplier for rent based on quality (0.5 - 2.0)
        int max_tenants;           // Maximum number of tenants
        int current_tenants;       // Current number of tenants
    
        FacilityEconomics(const float rent = 100.0f, const float cost = 20.0f, const int max_ten = 10)
            : base_rent(rent),
              revenue(0.0f),
              operating_cost(cost),
              quality_multiplier(1.0f),
              max_tenants(max_ten),
              current_tenants(0) {}
    
        /**
     * @brief Calculate daily revenue based on current occupancy and quality
     */
        float CalculateDailyRevenue() const {
            return base_rent * current_tenants * quality_multiplier;
        }
    
        /**
     * @brief Calculate net profit (revenue - costs)
     */
        float CalculateNetProfit() const {
            return CalculateDailyRevenue() - operating_cost;
        }
    
        /**
     * @brief Get occupancy percentage
     */
        float GetOccupancyRate() const {
            if (max_tenants == 0) return 0.0f;
            return (static_cast<float>(current_tenants) / max_tenants) * 100.0f;
        }
    };

    /**
 * @brief Individual adjacency effect from a neighboring facility
 */
    struct AdjacencyEffect {
        enum class Type {
            Revenue,       // Affects revenue/income
            Satisfaction,  // Affects tenant satisfaction
            Traffic        // Affects visitor traffic
        };

        Type type;
        float value;              // Positive for bonus, negative for penalty (percentage)
        std::string source_type;  // Type of neighboring facility causing this effect
        std::string description;  // Human-readable description

        AdjacencyEffect(const Type t = Type::Satisfaction, 
                       const float val = 0.0f,
                       const std::string& src = "",
                       const std::string& desc = "")
            : type(t), value(val), source_type(src), description(desc) {}
    };

    /**
 * @brief Component tracking all adjacency effects on a facility
 * 
 * Stores the list of active adjacency bonuses and penalties from
 * neighboring facilities. Updated when facilities are placed or removed.
 */
    struct AdjacencyEffects {
        std::vector<AdjacencyEffect> effects;  // All active adjacency effects

        AdjacencyEffects() = default;

        /**
     * @brief Clear all effects
     */
        void Clear() {
            effects.clear();
        }

        /**
     * @brief Add an adjacency effect
     */
        void AddEffect(const AdjacencyEffect& effect) {
            effects.push_back(effect);
        }

        /**
     * @brief Get total bonus/penalty for a specific type
     */
        float GetTotalForType(const AdjacencyEffect::Type type) const {
            float total = 0.0f;
            for (const auto& effect : effects) {
                if (effect.type == type) {
                    total += effect.value;
                }
            }
            return total;
        }

        /**
     * @brief Check if there are any effects
     */
        bool HasEffects() const {
            return !effects.empty();
        }

        /**
     * @brief Get count of effects
     */
        size_t GetEffectCount() const {
            return effects.size();
        }
    };

    /**
 * @brief Global singleton component for tower-wide economy tracking
 * 
 * Tracks the overall financial status of the tower including
 * total revenue, expenses, and balance.
 */
    struct TowerEconomy {
        float total_balance;       // Current cash balance
        float total_revenue;       // Accumulated total revenue
        float total_expenses;      // Accumulated total expenses
        float daily_revenue;       // Revenue for current day
        float daily_expenses;      // Expenses for current day
        int last_processed_day;    // Last day when economics were processed
    
        TowerEconomy(const float initial_balance = 10000.0f)
            : total_balance(initial_balance),
              total_revenue(0.0f),
              total_expenses(0.0f),
              daily_revenue(0.0f),
              daily_expenses(0.0f),
              last_processed_day(-1) {}
    
        /**
     * @brief Process daily financial transactions
     */
        void ProcessDailyTransactions() {
            total_balance += daily_revenue - daily_expenses;
            total_revenue += daily_revenue;
            total_expenses += daily_expenses;
        
            // Reset daily counters
            daily_revenue = 0.0f;
            daily_expenses = 0.0f;
        }
    
        /**
     * @brief Get the current profit/loss status
     */
        float GetNetProfit() const {
            return total_revenue - total_expenses;
        }
    
        /**
     * @brief Check if tower is profitable
     */
        bool IsProfitable() const {
            return daily_revenue > daily_expenses;
        }
    };

    /**
 * @brief State machine states for Elevator Car entities
 * 
 * Defines the possible states an elevator car can be in during operation.
 */
    enum class ElevatorState {
        Idle,           // Stationary, no pending requests
        MovingUp,       // Moving upward between floors
        MovingDown,     // Moving downward between floors
        DoorsOpening,   // Doors are opening
        DoorsOpen,      // Doors are open, passengers can board/exit
        DoorsClosing    // Doors are closing
    };

    /**
 * @brief Component for Elevator Shaft entities
 * 
 * Represents a vertical shaft that contains one or more elevator cars.
 * The shaft defines the physical space and floors served by the elevator system.
 */
    struct ElevatorShaft {
        int column;              // Grid column where shaft is located
        int bottom_floor;        // Lowest floor served
        int top_floor;           // Highest floor served
        int car_count;           // Number of cars in this shaft
    
        ElevatorShaft(const int col = 0, const int bottom = 0, const int top = 0, const int cars = 1)
            : column(col),
              bottom_floor(bottom),
              top_floor(top),
              car_count(cars) {}
    
        /**
     * @brief Get the total number of floors served
     */
        int GetFloorRange() const {
            return top_floor - bottom_floor + 1;
        }
    
        /**
     * @brief Check if a floor is served by this shaft
     */
        bool ServesFloor(const int floor) const {
            return floor >= bottom_floor && floor <= top_floor;
        }
    };

    /**
 * @brief Component for Elevator Car entities
 * 
 * Represents an individual elevator car that moves within a shaft.
 * Handles passenger transport, movement, and scheduling.
 */
    struct ElevatorCar {
        int shaft_entity_id;      // Reference to the shaft this car belongs to
        float current_floor;      // Current position (float for smooth movement between floors)
        int target_floor;         // Next floor destination
        ElevatorState state;      // Current state in the state machine
    
        // Capacity and occupancy
        int max_capacity;         // Maximum number of passengers
        int current_occupancy;    // Current number of passengers
    
        // Queue management
        std::vector<int> stop_queue;      // Floors where car needs to stop (sorted)
        std::vector<int> passenger_destinations;  // Destination floors of current passengers
    
        // Timing
        float state_timer;        // Timer for current state (doors, movement)
        float door_open_duration; // How long doors stay open (seconds)
        float door_transition_duration;  // How long it takes doors to open/close (seconds)
        float floors_per_second;  // Movement speed
    
        ElevatorCar(const int shaft_id = -1, const int start_floor = 0, const int capacity = 8)
            : shaft_entity_id(shaft_id),
              current_floor(static_cast<float>(start_floor)),
              target_floor(start_floor),
              state(ElevatorState::Idle),
              max_capacity(capacity),
              current_occupancy(0),
              state_timer(0.0f),
              door_open_duration(2.0f),      // 2 seconds
              door_transition_duration(1.0f), // 1 second
              floors_per_second(2.0f) {}     // 2 floors/second
    
        /**
     * @brief Get the current state as a string for debugging
     */
        const char* GetStateString() const {
            switch (state) {
                case ElevatorState::Idle:          return "Idle";
                case ElevatorState::MovingUp:      return "MovingUp";
                case ElevatorState::MovingDown:    return "MovingDown";
                case ElevatorState::DoorsOpening:  return "DoorsOpening";
                case ElevatorState::DoorsOpen:     return "DoorsOpen";
                case ElevatorState::DoorsClosing:  return "DoorsClosing";
                default:                           return "Unknown";
            }
        }
    
        /**
     * @brief Check if the car is at a floor (within tolerance)
     */
        bool IsAtFloor() const {
            return std::abs(current_floor - static_cast<float>(static_cast<int>(current_floor + 0.5f))) < 0.01f;
        }
    
        /**
     * @brief Get the current floor as an integer
     */
        int GetCurrentFloorInt() const {
            return static_cast<int>(current_floor + 0.5f);
        }
    
        /**
     * @brief Check if the car has capacity for more passengers
     */
        bool HasCapacity() const {
            return current_occupancy < max_capacity;
        }
    
        /**
     * @brief Add a stop to the queue (maintains sorted order)
     */
        void AddStop(const int floor) {
            // Check if floor is already in queue
            for (const int f : stop_queue) {
                if (f == floor) return;
            }
        
            // Add and sort based on direction
            stop_queue.push_back(floor);
            std::sort(stop_queue.begin(), stop_queue.end());
        }
    
        /**
     * @brief Get the next stop in the queue
     */
        int GetNextStop() const {
            if (stop_queue.empty()) return GetCurrentFloorInt();

            const int current = GetCurrentFloorInt();
        
            // Find closest stop in current direction
            if (state == ElevatorState::MovingUp || state == ElevatorState::Idle) {
                // Look for stops above current floor
                for (const int floor : stop_queue) {
                    if (floor >= current) return floor;
                }
            }
        
            if (state == ElevatorState::MovingDown || (state == ElevatorState::Idle && !stop_queue.empty())) {
                // Look for stops below current floor (reverse order)
                for (auto it = stop_queue.rbegin(); it != stop_queue.rend(); ++it) {
                    if (*it <= current) return *it;
                }
            }
        
            // No stops in current direction, return any stop
            return stop_queue.empty() ? current : stop_queue[0];
        }
    
        /**
     * @brief Remove current floor from stop queue
     */
        void RemoveCurrentStop() {
            const int current = GetCurrentFloorInt();
            stop_queue.erase(
                std::remove(stop_queue.begin(), stop_queue.end(), current),
                stop_queue.end()
            );
        }
    };

    /**
 * @brief Component linking a Person to an Elevator
 * 
 * Attached to Person entities when they are waiting for or riding an elevator.
 */
    struct PersonElevatorRequest {
        int shaft_entity_id;      // Which shaft the person is using
        int car_entity_id;        // Which car the person is in (-1 if waiting)
        int call_floor;           // Floor where person called the elevator
        int destination_floor;    // Where person wants to go
        float wait_time;          // How long person has been waiting
        bool is_boarding;         // True if person is currently boarding
    
        PersonElevatorRequest(const int shaft_id = -1, const int call = 0, const int dest = 0)
            : shaft_entity_id(shaft_id),
              car_entity_id(-1),
              call_floor(call),
              destination_floor(dest),
              wait_time(0.0f),
              is_boarding(false) {}
    };

    /**
 * @brief State of a research node in the upgrade tree
 */
    enum class ResearchNodeState {
        Locked,       // Not yet unlocked, requirements not met
        Hidden,       // Hidden until prerequisites are met
        Upgradable,   // Requirements met, can be unlocked
        Unlocked      // Already unlocked
    };

    /**
 * @brief Type of bonus/upgrade provided by a research node
 */
    enum class ResearchNodeType {
        FacilityUnlock,    // Unlocks a new facility type
        ElevatorSpeed,     // Increases elevator speed
        ElevatorCapacity,  // Increases elevator capacity
        IncomeBonus,       // Increases income rate
        SatisfactionBonus, // Increases tenant satisfaction
        ConstructionSpeed, // Faster construction times
        CostReduction      // Reduces facility costs
    };

    /**
 * @brief A single node in the research/upgrade tree
 */
    struct ResearchNode {
        std::string id;                    // Unique identifier
        std::string name;                  // Display name
        std::string description;           // Description of the upgrade
        std::string icon;                  // Icon character/emoji
        ResearchNodeType type;             // Type of upgrade
        ResearchNodeState state;           // Current state
    
        int cost;                          // Tower points required
        std::vector<std::string> prerequisites;  // IDs of required nodes
    
        // Conditional prerequisites
        int min_star_rating;               // Minimum tower star rating (0 = no requirement)
        int min_population;                // Minimum tower population (0 = no requirement)
        std::vector<std::string> required_facilities;  // Facility types that must exist
    
        // Position in tree/grid
        int grid_row;
        int grid_column;
    
        // Effect values (meaning depends on type)
        float effect_value;                // Numeric effect (e.g., +20% speed, +5 capacity)
        std::string effect_target;         // What the effect applies to (e.g., "Office", "Elevator")
    
        ResearchNode(const std::string& id_str = "node",
                     const std::string& name_str = "Research",
                     const ResearchNodeType node_type = ResearchNodeType::IncomeBonus,
                     const int node_cost = 10,
                     const int row = 0,
                     const int col = 0)
            : id(id_str),
              name(name_str),
              description(""),
              icon("🔒"),
              type(node_type),
              state(ResearchNodeState::Locked),
              cost(node_cost),
              min_star_rating(0),
              min_population(0),
              grid_row(row),
              grid_column(col),
              effect_value(0.0f),
              effect_target("") {}
    
        /**
     * @brief Get the state as a string for debugging
     */
        const char* GetStateString() const {
            switch (state) {
                case ResearchNodeState::Hidden: return "Hidden";
                case ResearchNodeState::Locked: return "Locked";
                case ResearchNodeState::Upgradable: return "Upgradable";
                case ResearchNodeState::Unlocked: return "Unlocked";
                default: return "Unknown";
            }
        }
    
        /**
     * @brief Get appropriate icon based on state
     */
        std::string GetDisplayIcon() const {
            switch (state) {
                case ResearchNodeState::Hidden: return "❓";
                case ResearchNodeState::Locked: return "🔒";
                case ResearchNodeState::Upgradable: return "✨";
                case ResearchNodeState::Unlocked: return "✅";
                default: return "❓";
            }
        }
    };

    /**
 * @brief Staff role types for facility and tower management
 */
    enum class StaffRole {
        Firefighter,     // Responds to fires in facilities
        Security,        // Handles security issues (shoplifters, etc.)
        Janitor,         // General cleaning of facilities
        Maintenance,     // Repairs broken equipment and prevents breakdowns
        Cleaner,         // Specialized cleaning staff
        Repairer         // Specialized repair staff
    };

    /**
 * @brief Component for staff assignment tracking
 * 
 * Tracks staff assignments to facilities or floors, schedules,
 * and current work status. Staff automatically perform their
 * assigned duties during their shift hours.
 * Supports both built-in roles and custom roles from Lua mods.
 */
    struct StaffAssignment {
        StaffRole role;
        std::string custom_role_id;      // Custom role ID from Lua (empty if built-in)
        std::string work_type;           // "cleaning", "maintenance", "emergency", or "custom"
        int assigned_floor;              // Floor assigned to (-1 for tower-wide)
        int assigned_facility_entity;    // Specific facility entity ID (-1 for floor-wide)
        float shift_start_time;          // Hour when shift starts (0-24)
        float shift_end_time;            // Hour when shift ends (0-24)
        bool is_active;                  // Currently on duty
        bool auto_assigned;              // Automatically assigned by system
        float work_efficiency;           // 0.0-1.0, affects how quickly tasks are completed
    
        StaffAssignment(const StaffRole r = StaffRole::Janitor,
                       const int floor = -1,
                       const float start = 8.0f,
                       const float end = 17.0f)
            : role(r),
              custom_role_id(""),
              work_type(""),
              assigned_floor(floor),
              assigned_facility_entity(-1),
              shift_start_time(start),
              shift_end_time(end),
              is_active(false),
              auto_assigned(true),
              work_efficiency(1.0f) {
            // Set default work type based on built-in role
            switch (r) {
                case StaffRole::Janitor:
                case StaffRole::Cleaner:
                    work_type = "cleaning";
                    break;
                case StaffRole::Maintenance:
                case StaffRole::Repairer:
                    work_type = "maintenance";
                    break;
                case StaffRole::Firefighter:
                case StaffRole::Security:
                    work_type = "emergency";
                    break;
            }
        }
    
        /**
     * @brief Check if this is a custom role from Lua
     */
        bool IsCustomRole() const {
            return !custom_role_id.empty();
        }
    
        /**
     * @brief Get role as a string
     */
        const char* GetRoleName() const {
            if (IsCustomRole()) {
                return custom_role_id.c_str();
            }
            switch (role) {
                case StaffRole::Firefighter:  return "Firefighter";
                case StaffRole::Security:     return "Security";
                case StaffRole::Janitor:      return "Janitor";
                case StaffRole::Maintenance:  return "Maintenance";
                case StaffRole::Cleaner:      return "Cleaner";
                case StaffRole::Repairer:     return "Repairer";
                default:                      return "Unknown";
            }
        }
    
        /**
     * @brief Check if this staff performs cleaning work
     */
        bool DoesCleaningWork() const {
            return work_type == "cleaning";
        }
    
        /**
     * @brief Check if this staff performs maintenance work
     */
        bool DoesMaintenanceWork() const {
            return work_type == "maintenance";
        }
    
        /**
     * @brief Check if this staff performs emergency work
     */
        bool DoesEmergencyWork() const {
            return work_type == "emergency";
        }
    
        /**
     * @brief Check if staff should be working at current time
     */
        bool ShouldBeWorking(const float current_hour) const {
            // Handle overnight shifts (e.g., 22:00 to 06:00)
            if (shift_start_time > shift_end_time) {
                return current_hour >= shift_start_time || current_hour < shift_end_time;
            }
            return current_hour >= shift_start_time && current_hour < shift_end_time;
        }
    };

    /**
 * @brief Component for facility cleanliness tracking
 * 
 * Each facility has a cleanliness status that degrades over time or with heavy use.
 * Cleanliness impacts visitor/tenant satisfaction and facility performance.
 * Cleaning is performed by janitor staff.
 */
    struct CleanlinessStatus {
        enum class State { Clean, NeedsCleaning, Dirty };
        State status;
        float time_since_last_clean;  // Seconds since last cleaned
        float dirty_rate;              // How quickly facility gets dirty (tuned per facility type and usage)

        CleanlinessStatus()
            : status(State::Clean),
              time_since_last_clean(0.0f),
              dirty_rate(1.0f) {}

        /**
     * @brief Get the state as a string for display
     */
        const char* GetStateString() const {
            switch (status) {
                case State::Clean:          return "Clean";
                case State::NeedsCleaning:  return "Needs Cleaning";
                case State::Dirty:          return "Dirty";
                default:                    return "Unknown";
            }
        }

        /**
     * @brief Get cleanliness as a percentage (for compatibility with existing systems)
     */
        float GetCleanlinessPercent() const {
            switch (status) {
                case State::Clean:          return 100.0f;
                case State::NeedsCleaning:  return 60.0f;
                case State::Dirty:          return 30.0f;
                default:                    return 50.0f;
            }
        }

        /**
     * @brief Update cleanliness state based on time elapsed
     * @param delta_time Time elapsed in seconds
     * @param occupancy_factor Multiplier based on facility usage (higher = faster degradation)
     */
        void Update(const float delta_time, const float occupancy_factor = 1.0f) {
            time_since_last_clean += delta_time;

            // Thresholds for state transitions (can be tuned)
            const float needs_cleaning_threshold = 1800.0f / dirty_rate;  // 30 minutes base
            const float dirty_threshold = 3600.0f / dirty_rate;           // 60 minutes base

            // Apply occupancy factor to thresholds (busier facilities get dirty faster)
            const float adjusted_needs_cleaning = needs_cleaning_threshold / occupancy_factor;
            const float adjusted_dirty = dirty_threshold / occupancy_factor;

            // Update state based on time since last clean
            if (time_since_last_clean >= adjusted_dirty) {
                status = State::Dirty;
            } else if (time_since_last_clean >= adjusted_needs_cleaning) {
                status = State::NeedsCleaning;
            } else {
                status = State::Clean;
            }
        }

        /**
     * @brief Perform cleaning action
     */
        void Clean() {
            status = State::Clean;
            time_since_last_clean = 0.0f;
        }

        /**
     * @brief Check if facility needs cleaning
     */
        bool NeedsCleaning() const {
            return status == State::NeedsCleaning || status == State::Dirty;
        }

        /**
     * @brief Check if facility is dirty
     */
        bool IsDirty() const {
            return status == State::Dirty;
        }
    };

    /**
 * @brief Component for facility maintenance tracking
 * 
 * Each facility has a maintenance status that degrades over time or with use.
 * Maintenance failures are gentle (not catastrophic), providing gentle nudges
 * rather than harsh penalties. Players retain agency through repair options.
 */
    struct MaintenanceStatus {
        enum class State { Good, NeedsService, Broken };
        State status;
        float time_since_last_service;  // Seconds since last serviced
        float degrade_rate;              // How quickly facility degrades (tuned per facility type)
        bool auto_repair_enabled;        // Whether to automatically repair when broken

        MaintenanceStatus()
            : status(State::Good),
              time_since_last_service(0.0f),
              degrade_rate(1.0f),
              auto_repair_enabled(false) {}

        /**
     * @brief Get the state as a string for display
     */
        const char* GetStateString() const {
            switch (status) {
                case State::Good:          return "Good";
                case State::NeedsService:  return "Needs Service";
                case State::Broken:        return "Broken";
                default:                   return "Unknown";
            }
        }

        /**
     * @brief Get maintenance as a percentage (for compatibility with existing systems)
     */
        float GetMaintenancePercent() const {
            switch (status) {
                case State::Good:          return 100.0f;
                case State::NeedsService:  return 60.0f;
                case State::Broken:        return 0.0f;
                default:                   return 50.0f;
            }
        }

        /**
     * @brief Update maintenance state based on time elapsed and usage
     * @param delta_time Time elapsed in seconds
     * @param usage_factor Multiplier based on facility usage (higher = faster degradation)
     */
        void Update(const float delta_time, const float usage_factor = 1.0f) {
            time_since_last_service += delta_time;

            // Thresholds for state transitions (can be tuned)
            const float needs_service_threshold = 7200.0f / degrade_rate;  // 2 hours base
            const float broken_threshold = 14400.0f / degrade_rate;         // 4 hours base

            // Apply usage factor to thresholds (busier facilities break faster)
            const float adjusted_needs_service = needs_service_threshold / usage_factor;
            const float adjusted_broken = broken_threshold / usage_factor;

            // Update state based on time since last service
            if (time_since_last_service >= adjusted_broken) {
                status = State::Broken;
            } else if (time_since_last_service >= adjusted_needs_service) {
                status = State::NeedsService;
            } else {
                status = State::Good;
            }
        }

        /**
     * @brief Perform repair/service action
     */
        void Repair() {
            status = State::Good;
            time_since_last_service = 0.0f;
        }

        /**
     * @brief Check if facility needs service
     */
        bool NeedsService() const {
            return status == State::NeedsService || status == State::Broken;
        }

        /**
     * @brief Check if facility is broken
     */
        bool IsBroken() const {
            return status == State::Broken;
        }
    };

    /**
 * @brief Facility maintenance and cleanliness status
 * 
 * Tracks the cleanliness and maintenance condition of a facility.
 * Poor conditions can reduce satisfaction but don't cause hard penalties.
 * Supports both built-in events and custom events from Lua mods.
 */
    struct FacilityStatus {
        float cleanliness;          // 0.0-100.0, degrades over time with use
        float maintenance_level;    // 0.0-100.0, degrades over time
        bool has_fire;              // Active fire that needs firefighter
        bool has_security_issue;    // Active security issue (shoplifter, etc.)
        std::vector<std::string> active_custom_events;  // Custom event IDs from Lua
        float time_since_cleaning;  // Seconds since last cleaned
        float time_since_maintenance; // Seconds since last maintained
        float degradation_rate;     // How quickly cleanliness/maintenance degrades

        FacilityStatus()
            : cleanliness(100.0f),
              maintenance_level(100.0f),
              has_fire(false),
              has_security_issue(false),
              time_since_cleaning(0.0f),
              time_since_maintenance(0.0f),
              degradation_rate(1.0f) {}
    
        /**
     * @brief Get cleanliness as a rating string
     */
        const char* GetCleanlinessRating() const {
            if (cleanliness >= 90.0f) return "Spotless";
            if (cleanliness >= 70.0f) return "Clean";
            if (cleanliness >= 50.0f) return "Acceptable";
            if (cleanliness >= 30.0f) return "Dirty";
            return "Filthy";
        }
    
        /**
     * @brief Get maintenance level as a rating string
     */
        const char* GetMaintenanceRating() const {
            if (maintenance_level >= 90.0f) return "Excellent";
            if (maintenance_level >= 70.0f) return "Good";
            if (maintenance_level >= 50.0f) return "Fair";
            if (maintenance_level >= 30.0f) return "Poor";
            return "Critical";
        }
    
        /**
     * @brief Check if facility needs cleaning
     */
        bool NeedsCleaning() const {
            return cleanliness < 70.0f;
        }
    
        /**
     * @brief Check if facility needs maintenance
     */
        bool NeedsMaintenance() const {
            return maintenance_level < 70.0f;
        }
    
        /**
     * @brief Check if facility has any active events
     */
        bool HasActiveEvents() const {
            return has_fire || has_security_issue || !active_custom_events.empty();
        }
    
        /**
     * @brief Add a custom event
     */
        void AddCustomEvent(const std::string& event_id) {
            // Check if event already exists
            for (const auto& evt : active_custom_events) {
                if (evt == event_id) return;
            }
            active_custom_events.push_back(event_id);
        }
    
        /**
     * @brief Remove a custom event
     */
        void RemoveCustomEvent(const std::string& event_id) {
            active_custom_events.erase(
                std::remove(active_custom_events.begin(), active_custom_events.end(), event_id),
                active_custom_events.end());
        }
    
        /**
     * @brief Check if a specific custom event is active
     */
        bool HasCustomEvent(const std::string& event_id) const {
            for (const auto& evt : active_custom_events) {
                if (evt == event_id) return true;
            }
            return false;
        }
    
        /**
     * @brief Update status over time
     */
        void Update(const float delta_time, const int current_occupancy) {
            time_since_cleaning += delta_time;
            time_since_maintenance += delta_time;
        
            // Degrade cleanliness based on occupancy and time
            const float occupancy_factor = 1.0f + (current_occupancy * 0.1f);
            cleanliness -= degradation_rate * occupancy_factor * delta_time / 3600.0f;
            if (cleanliness < 0.0f) cleanliness = 0.0f;
        
            // Degrade maintenance over time (slower than cleanliness)
            maintenance_level -= (degradation_rate * 0.5f) * delta_time / 3600.0f;
            if (maintenance_level < 0.0f) maintenance_level = 0.0f;
        }
    
        /**
     * @brief Perform cleaning action
     */
        void Clean(const float efficiency) {
            const float clean_amount = 30.0f * efficiency;
            cleanliness += clean_amount;
            if (cleanliness > 100.0f) cleanliness = 100.0f;
            time_since_cleaning = 0.0f;
        }
    
        /**
     * @brief Perform maintenance action
     */
        void Maintain(const float efficiency) {
            const float maintain_amount = 25.0f * efficiency;
            maintenance_level += maintain_amount;
            if (maintenance_level > 100.0f) maintenance_level = 100.0f;
            time_since_maintenance = 0.0f;
        }
    
        /**
     * @brief Extinguish fire
     */
        void ExtinguishFire() {
            has_fire = false;
            // Fire causes some damage to maintenance level
            maintenance_level -= 10.0f;
            if (maintenance_level < 0.0f) maintenance_level = 0.0f;
        }
    
        /**
     * @brief Resolve security issue
     */
        void ResolveSecurityIssue() {
            has_security_issue = false;
        }
    };

    /**
 * @brief Global singleton for staff management
 * 
 * Tracks all staff in the tower and manages hiring/firing.
 */
    struct StaffManager {
        int total_staff_count;           // Total number of staff employed
        int firefighters;                // Count by role
        int security_guards;
        int janitors;
        int maintenance_staff;
        int cleaners;
        int repairers;
        float total_staff_wages;         // Daily wages for all staff
        bool auto_hire_enabled;          // Automatically hire staff when needed
    
        StaffManager()
            : total_staff_count(0),
              firefighters(0),
              security_guards(0),
              janitors(0),
              maintenance_staff(0),
              cleaners(0),
              repairers(0),
              total_staff_wages(0.0f),
              auto_hire_enabled(true) {}
    
        /**
     * @brief Get staff count by role
     */
        int GetStaffCount(const StaffRole role) const {
            switch (role) {
                case StaffRole::Firefighter:  return firefighters;
                case StaffRole::Security:     return security_guards;
                case StaffRole::Janitor:      return janitors;
                case StaffRole::Maintenance:  return maintenance_staff;
                case StaffRole::Cleaner:      return cleaners;
                case StaffRole::Repairer:     return repairers;
                default:                      return 0;
            }
        }
    
        /**
     * @brief Calculate recommended staff count based on facilities
     */
        static int CalculateRecommendedStaff(const StaffRole role, const int facility_count, const int total_floors) {
            switch (role) {
                case StaffRole::Janitor:
                    // 1 janitor per 3 facilities or per 5 floors
                    return std::max((facility_count + 2) / 3, (total_floors + 4) / 5);
                case StaffRole::Maintenance:
                    // 1 maintenance per 5 facilities
                    return (facility_count + 4) / 5;
                case StaffRole::Firefighter:
                    // 1 firefighter per 10 floors
                    return std::max(1, (total_floors + 9) / 10);
                case StaffRole::Security:
                    // 1 security per 15 facilities or per 10 floors
                    return std::max((facility_count + 14) / 15, (total_floors + 9) / 10);
                case StaffRole::Cleaner:
                    // Cleaners are specialized janitors
                    return (facility_count + 4) / 5;
                case StaffRole::Repairer:
                    // Repairers are specialized maintenance
                    return (facility_count + 6) / 7;
                default:
                    return 0;
            }
        }
    };

    /**
 * @brief Global singleton component for research/upgrade tree progress
 * 
 * Tracks available tower points, unlocked nodes, and applies
 * global bonuses from research. Tower points are generated by
 * management staff and facilities rather than milestone achievements.
 */
    struct ResearchTree {
        int tower_points;                  // Available tower points to spend
        int total_points_earned;           // Lifetime tower points earned
        std::vector<ResearchNode> nodes;   // All research nodes
    
        // Management staff tracking
        int management_staff_count;        // Total management staff (executives, managers, analysts)
        float tower_points_per_hour;       // Current tower points generation rate
        float accumulated_points;          // Fractional points accumulated
    
        // Global bonuses from research
        float income_multiplier;           // Multiplier for all income (1.0 = normal)
        float satisfaction_bonus;          // Additive bonus to satisfaction
        float construction_speed_multiplier; // Multiplier for construction speed
        float cost_reduction;              // Percentage reduction in costs (0.0 - 1.0)
        float elevator_speed_multiplier;   // Multiplier for elevator speed
        int elevator_capacity_bonus;       // Additional elevator capacity
    
        ResearchTree()
            : tower_points(0),
              total_points_earned(0),
              management_staff_count(0),
              tower_points_per_hour(0.0f),
              accumulated_points(0.0f),
              income_multiplier(1.0f),
              satisfaction_bonus(0.0f),
              construction_speed_multiplier(1.0f),
              cost_reduction(0.0f),
              elevator_speed_multiplier(1.0f),
              elevator_capacity_bonus(0) {}
    
        /**
     * @brief Initialize default research tree with nodes
     */
        void InitializeDefaultTree() {
            nodes.clear();
        
            // Row 0: Basic upgrades (starting tier) - unlocked by default or very cheap
            nodes.push_back(ResearchNode("basic_elevator", "Fast Elevators", 
                                         ResearchNodeType::ElevatorSpeed, 10, 0, 0));
            nodes.back().description = "Increases elevator speed by 50%";
            nodes.back().icon = "🚀";
            nodes.back().effect_value = 0.5f;
        
            nodes.push_back(ResearchNode("office_unlock", "Office Spaces", 
                                         ResearchNodeType::FacilityUnlock, 5, 0, 1));
            nodes.back().description = "Unlock commercial office facilities";
            nodes.back().icon = "🏢";
            nodes.back().effect_target = "Office";
        
            nodes.push_back(ResearchNode("shop_unlock", "Retail Shops", 
                                         ResearchNodeType::FacilityUnlock, 5, 0, 2));
            nodes.back().description = "Unlock retail shop facilities";
            nodes.back().icon = "🏪";
            nodes.back().effect_target = "RetailShop";
        
            nodes.push_back(ResearchNode("management_office_unlock", "Tower Management", 
                                         ResearchNodeType::FacilityUnlock, 15, 0, 3));
            nodes.back().description = "Unlock management office to generate tower points";
            nodes.back().icon = "🏛️";
            nodes.back().effect_target = "ManagementOffice";
            nodes.back().min_population = 20;  // Requires 20 people before management is needed
        
            // Row 1: Mid-tier upgrades
            nodes.push_back(ResearchNode("express_shafts", "Express Elevators", 
                                         ResearchNodeType::ElevatorSpeed, 20, 1, 0));
            nodes.back().description = "High-speed elevator technology";
            nodes.back().icon = "⚡";
            nodes.back().effect_value = 1.0f;  // +100% speed
            nodes.back().prerequisites.push_back("basic_elevator");
        
            nodes.push_back(ResearchNode("large_elevators", "Large Elevators", 
                                         ResearchNodeType::ElevatorCapacity, 15, 1, 1));
            nodes.back().description = "Increases elevator capacity by 4";
            nodes.back().icon = "📦";
            nodes.back().effect_value = 4.0f;
            nodes.back().prerequisites.push_back("basic_elevator");
        
            nodes.push_back(ResearchNode("income_boost", "Revenue Optimization", 
                                         ResearchNodeType::IncomeBonus, 15, 1, 2));
            nodes.back().description = "Increases all income by 25%";
            nodes.back().icon = "💰";
            nodes.back().effect_value = 0.25f;
            nodes.back().prerequisites.push_back("office_unlock");
        
            nodes.push_back(ResearchNode("satellite_office_unlock", "Branch Management", 
                                         ResearchNodeType::FacilityUnlock, 25, 1, 3));
            nodes.back().description = "Unlock satellite offices for distributed management";
            nodes.back().icon = "🏢";
            nodes.back().effect_target = "SatelliteOffice";
            nodes.back().prerequisites.push_back("management_office_unlock");
            nodes.back().min_star_rating = 2;
        
            nodes.push_back(ResearchNode("restaurant_unlock", "Fine Dining", 
                                         ResearchNodeType::FacilityUnlock, 30, 1, 4));
            nodes.back().description = "Unlock restaurant facilities";
            nodes.back().icon = "🍽️";
            nodes.back().effect_target = "Restaurant";
            nodes.back().prerequisites.push_back("shop_unlock");
            nodes.back().min_star_rating = 2;
            nodes.back().min_population = 50;
        
            // Row 2: Advanced upgrades
            nodes.push_back(ResearchNode("construction_speed", "Rapid Construction", 
                                         ResearchNodeType::ConstructionSpeed, 25, 2, 0));
            nodes.back().description = "Reduces construction time by 50%";
            nodes.back().icon = "🏗️";
            nodes.back().effect_value = 0.5f;
            nodes.back().min_star_rating = 3;
        
            nodes.push_back(ResearchNode("cost_reduction", "Efficient Building", 
                                         ResearchNodeType::CostReduction, 30, 2, 1));
            nodes.back().description = "Reduces all costs by 20%";
            nodes.back().icon = "📉";
            nodes.back().effect_value = 0.2f;
            nodes.back().required_facilities.push_back("ManagementOffice");
        
            nodes.push_back(ResearchNode("satisfaction_boost", "Quality Service", 
                                         ResearchNodeType::SatisfactionBonus, 25, 2, 2));
            nodes.back().description = "Increases satisfaction by 10 points";
            nodes.back().icon = "😊";
            nodes.back().effect_value = 10.0f;
            nodes.back().prerequisites.push_back("income_boost");
        
            nodes.push_back(ResearchNode("arcade_unlock", "Gaming Entertainment", 
                                         ResearchNodeType::FacilityUnlock, 35, 2, 3));
            nodes.back().description = "Unlock arcade entertainment facilities";
            nodes.back().icon = "🎮";
            nodes.back().effect_target = "Arcade";
            nodes.back().prerequisites.push_back("restaurant_unlock");
            nodes.back().min_star_rating = 3;
            nodes.back().min_population = 100;
        
            nodes.push_back(ResearchNode("gym_unlock", "Wellness Center", 
                                         ResearchNodeType::FacilityUnlock, 40, 2, 4));
            nodes.back().description = "Unlock gym and fitness facilities";
            nodes.back().icon = "💪";
            nodes.back().effect_target = "Gym";
            nodes.back().prerequisites.push_back("restaurant_unlock");
            nodes.back().min_star_rating = 3;
            nodes.back().min_population = 75;
        }
    
        /**
     * @brief Find a node by ID
     */
        ResearchNode* FindNode(const std::string& node_id) {
            for (auto& node : nodes) {
                if (node.id == node_id) {
                    return &node;
                }
            }
            return nullptr;
        }
    
        /**
     * @brief Check if a node can be unlocked (prerequisites met)
     * @param node The research node to check
     * @param star_rating Current tower star rating (for conditional prereqs)
     * @param population Current tower population (for conditional prereqs)
     * @param built_facilities Set of facility type names that exist in tower
     */
        bool CanUnlock(const ResearchNode& node, const int star_rating = 0, const int population = 0, 
                       const std::vector<std::string>& built_facilities = {}) const {
            // Check if already unlocked
            if (node.state == ResearchNodeState::Unlocked) {
                return false;
            }
        
            // Check if we have enough points
            if (tower_points < node.cost) {
                return false;
            }
        
            // Check conditional prerequisites
            if (node.min_star_rating > 0 && star_rating < node.min_star_rating) {
                return false;
            }
        
            if (node.min_population > 0 && population < node.min_population) {
                return false;
            }
        
            // Check required facilities
            for (const auto& required_facility : node.required_facilities) {
                bool found = false;
                for (const auto& built_facility : built_facilities) {
                    if (built_facility == required_facility) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false;
                }
            }
        
            // Check research node prerequisites
            for (const auto& prereq_id : node.prerequisites) {
                bool found = false;
                for (const auto& n : nodes) {
                    if (n.id == prereq_id && n.state == ResearchNodeState::Unlocked) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false;
                }
            }
        
            return true;
        }
    
        /**
     * @brief Check if a node's visibility prerequisites are met
     * A node is hidden until its visibility prerequisites are satisfied
     */
        static bool IsVisible(const ResearchNode& node, const int star_rating = 0, const int population = 0) {
            // Nodes are visible if they have no visibility requirements, or if requirements are met
            // For now, we'll make nodes visible if star rating or population requirements are met
            // OR if they have no such requirements
            if (node.min_star_rating > 0 && star_rating < node.min_star_rating) {
                return false;
            }
        
            if (node.min_population > 0 && population < node.min_population) {
                return false;
            }
        
            return true;
        }
    
        /**
     * @brief Update all node states based on current conditions
     */
        void UpdateNodeStates() {
            for (auto& node : nodes) {
                if (node.state == ResearchNodeState::Unlocked) {
                    continue;  // Already unlocked
                }
            
                if (CanUnlock(node)) {
                    node.state = ResearchNodeState::Upgradable;
                } else {
                    node.state = ResearchNodeState::Locked;
                }
            }
        }
    
        /**
     * @brief Unlock a node and apply its effects
     */
        bool UnlockNode(const std::string& node_id) {
            ResearchNode* node = FindNode(node_id);
            if (!node || !CanUnlock(*node)) {
                return false;
            }
        
            // Deduct cost
            tower_points -= node->cost;
        
            // Mark as unlocked
            node->state = ResearchNodeState::Unlocked;
        
            // Apply effects based on type
            switch (node->type) {
                case ResearchNodeType::ElevatorSpeed:
                    elevator_speed_multiplier += node->effect_value;
                    break;
                case ResearchNodeType::ElevatorCapacity:
                    elevator_capacity_bonus += static_cast<int>(node->effect_value);
                    break;
                case ResearchNodeType::IncomeBonus:
                    income_multiplier += node->effect_value;
                    break;
                case ResearchNodeType::SatisfactionBonus:
                    satisfaction_bonus += node->effect_value;
                    break;
                case ResearchNodeType::ConstructionSpeed:
                    construction_speed_multiplier += node->effect_value;
                    break;
                case ResearchNodeType::CostReduction:
                    cost_reduction += node->effect_value;
                    break;
                case ResearchNodeType::FacilityUnlock:
                    // Facility unlocks are handled separately
                    break;
            }
        
            // Update states of other nodes
            UpdateNodeStates();
        
            return true;
        }
    
        /**
     * @brief Award tower points (e.g., from management staff generation)
     */
        void AwardPoints(const int points) {
            tower_points += points;
            total_points_earned += points;
            UpdateNodeStates();
        }
    
        /**
     * @brief Generate tower points based on management staff and time elapsed
     * @param delta_time Time elapsed since last update (in hours)
     */
        void GenerateTowerPoints(const float delta_time) {
            // Calculate points per hour based on management staff
            // Base: 1 point per hour per staff member
            // Can be enhanced by management facilities in the future
            tower_points_per_hour = static_cast<float>(management_staff_count);
        
            // Accumulate fractional points
            accumulated_points += tower_points_per_hour * delta_time;
        
            // Award whole points
            const int whole_points = static_cast<int>(accumulated_points);
            if (whole_points > 0) {
                tower_points += whole_points;
                total_points_earned += whole_points;
                accumulated_points -= whole_points;
                UpdateNodeStates();
            }
        }
    
        /**
     * @brief Update management staff count
     * Should be called when management staff are hired/fired
     */
        void UpdateManagementStaffCount(const int count) {
            management_staff_count = count;
        }
    
        /**
     * @brief Check if a facility type is unlocked
     */
        bool IsFacilityUnlocked(const std::string& facility_type) const {
            // Lobby and basic facilities are always unlocked
            if (facility_type == "Lobby" || facility_type == "Elevator") {
                return true;
            }
        
            // Check if corresponding research node is unlocked
            for (const auto& node : nodes) {
                if (node.type == ResearchNodeType::FacilityUnlock &&
                    node.effect_target == facility_type &&
                    node.state == ResearchNodeState::Unlocked) {
                    return true;
                }
            }
        
            return false;
        }
    };

}
