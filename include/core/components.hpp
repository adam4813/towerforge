#pragma once

#include <algorithm>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace TowerForge {
namespace Core {

// Forward declaration
class TowerGrid;

/**
 * @brief Component for entities with a position in 2D space
 */
struct Position {
    float x;
    float y;

    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @brief Component for entities with velocity
 */
struct Velocity {
    float dx;
    float dy;

    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
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

    Actor(const std::string& name = "Actor", int dest = 0, float spd = 1.0f)
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
           int floor = 0, 
           float col = 0.0f,
           float speed = 2.0f,
           NPCType type = NPCType::Visitor)
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
    void SetDestination(int floor, float column, const std::string& need = "Moving") {
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
 * @brief Component for visitor NPCs
 * 
 * Tracks visitor-specific information like their activity and visit duration.
 */
struct VisitorInfo {
    VisitorActivity activity;      // What the visitor is currently doing
    float visit_duration;          // How long they've been in the tower (seconds)
    float max_visit_duration;      // When they'll leave (seconds)
    int target_facility_floor;     // Floor of facility they're visiting (-1 if none)
    
    VisitorInfo(VisitorActivity act = VisitorActivity::Visiting)
        : activity(act),
          visit_duration(0.0f),
          max_visit_duration(300.0f),  // 5 minutes default
          target_facility_floor(-1) {}
    
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
                   int floor = 0, 
                   int col = 0,
                   float start_hour = 9.0f,
                   float end_hour = 17.0f)
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
    bool ShouldBeWorking(float current_hour, int current_day) const {
        // Check if today is a work day
        bool is_work_day = false;
        for (int day : work_days) {
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
 * - Restaurant: Food service facility (legacy type, kept for compatibility)
 * - Hotel: Temporary lodging (legacy type, kept for compatibility)
 * - Elevator: Vertical transportation (legacy type, kept for compatibility)
 */
struct BuildingComponent {
    enum class Type {
        Office,        // Commercial office space
        Residential,   // Residential condominiums
        RetailShop,    // Retail shop
        Lobby,         // Main entrance/lobby
        Restaurant,    // Food service (legacy)
        Hotel,         // Hotel rooms (legacy)
        Elevator       // Vertical transport (legacy)
    };

    Type type;
    int floor;              // Which floor this component is on
    int width;              // Width in tiles
    int capacity;           // Maximum occupancy
    int current_occupancy;  // Current number of people

    BuildingComponent(Type t = Type::Office, int f = 0, int w = 1, int cap = 10)
        : type(t), floor(f), width(w), capacity(cap), current_occupancy(0) {}
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
    
    TimeManager(float hours_per_sec = 1.0f)
        : current_hour(8.0f),      // Start at 8 AM
          current_day(0),          // Start on Monday
          current_week(1),         // Start at week 1
          simulation_speed(1.0f),  // Normal speed
          hours_per_second(hours_per_sec) {}
    
    /**
     * @brief Get the current time as a formatted string (HH:MM)
     */
    std::string GetTimeString() const {
        int hours = static_cast<int>(current_hour);
        int minutes = static_cast<int>((current_hour - hours) * 60.0f);
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
    
    ScheduledAction(Type t = Type::Idle, float hour = 9.0f)
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
    void AddWeekdayAction(ScheduledAction::Type type, float hour) {
        weekday_schedule.push_back(ScheduledAction(type, hour));
    }
    
    /**
     * @brief Add an action to the weekend schedule
     */
    void AddWeekendAction(ScheduledAction::Type type, float hour) {
        weekend_schedule.push_back(ScheduledAction(type, hour));
    }
    
    /**
     * @brief Get the appropriate schedule based on whether it's a weekend
     */
    const std::vector<ScheduledAction>& GetActiveSchedule(bool is_weekend) const {
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
    
    GridPosition(int f = 0, int c = 0, int w = 1)
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
    
    Satisfaction(float initial_score = 75.0f)
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
        float total_change = quality_bonus - (wait_time_penalty + crowding_penalty + noise_penalty);
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
    
    FacilityEconomics(float rent = 100.0f, float cost = 20.0f, int max_ten = 10)
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
    
    TowerEconomy(float initial_balance = 10000.0f)
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
    
    ElevatorShaft(int col = 0, int bottom = 0, int top = 0, int cars = 1)
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
    bool ServesFloor(int floor) const {
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
    
    ElevatorCar(int shaft_id = -1, int start_floor = 0, int capacity = 8)
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
    void AddStop(int floor) {
        // Check if floor is already in queue
        for (int f : stop_queue) {
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
        
        int current = GetCurrentFloorInt();
        
        // Find closest stop in current direction
        if (state == ElevatorState::MovingUp || state == ElevatorState::Idle) {
            // Look for stops above current floor
            for (int floor : stop_queue) {
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
        int current = GetCurrentFloorInt();
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
    
    PersonElevatorRequest(int shaft_id = -1, int call = 0, int dest = 0)
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
    
    int cost;                          // Research points required
    std::vector<std::string> prerequisites;  // IDs of required nodes
    
    // Position in tree/grid
    int grid_row;
    int grid_column;
    
    // Effect values (meaning depends on type)
    float effect_value;                // Numeric effect (e.g., +20% speed, +5 capacity)
    std::string effect_target;         // What the effect applies to (e.g., "Office", "Elevator")
    
    ResearchNode(const std::string& id_str = "node",
                const std::string& name_str = "Research",
                ResearchNodeType node_type = ResearchNodeType::IncomeBonus,
                int node_cost = 10,
                int row = 0,
                int col = 0)
        : id(id_str),
          name(name_str),
          description(""),
          icon("🔒"),
          type(node_type),
          state(ResearchNodeState::Locked),
          cost(node_cost),
          grid_row(row),
          grid_column(col),
          effect_value(0.0f),
          effect_target("") {}
    
    /**
     * @brief Get the state as a string for debugging
     */
    const char* GetStateString() const {
        switch (state) {
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
            case ResearchNodeState::Locked: return "🔒";
            case ResearchNodeState::Upgradable: return "✨";
            case ResearchNodeState::Unlocked: return "✅";
            default: return "❓";
        }
    }
};

/**
 * @brief Global singleton component for research/upgrade tree progress
 * 
 * Tracks available research points, unlocked nodes, and applies
 * global bonuses from research.
 */
struct ResearchTree {
    int research_points;               // Available points to spend
    int total_points_earned;           // Lifetime points earned
    std::vector<ResearchNode> nodes;   // All research nodes
    
    // Global bonuses from research
    float income_multiplier;           // Multiplier for all income (1.0 = normal)
    float satisfaction_bonus;          // Additive bonus to satisfaction
    float construction_speed_multiplier; // Multiplier for construction speed
    float cost_reduction;              // Percentage reduction in costs (0.0 - 1.0)
    float elevator_speed_multiplier;   // Multiplier for elevator speed
    int elevator_capacity_bonus;       // Additional elevator capacity
    
    ResearchTree()
        : research_points(0),
          total_points_earned(0),
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
        
        // Row 0: Basic upgrades (starting tier)
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
        nodes.back().effect_target = "Shop";
        
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
        
        // Row 2: Advanced upgrades
        nodes.push_back(ResearchNode("construction_speed", "Rapid Construction", 
            ResearchNodeType::ConstructionSpeed, 25, 2, 0));
        nodes.back().description = "Reduces construction time by 50%";
        nodes.back().icon = "🏗️";
        nodes.back().effect_value = 0.5f;
        
        nodes.push_back(ResearchNode("cost_reduction", "Efficient Building", 
            ResearchNodeType::CostReduction, 30, 2, 1));
        nodes.back().description = "Reduces all costs by 20%";
        nodes.back().icon = "📉";
        nodes.back().effect_value = 0.2f;
        
        nodes.push_back(ResearchNode("satisfaction_boost", "Quality Service", 
            ResearchNodeType::SatisfactionBonus, 25, 2, 2));
        nodes.back().description = "Increases satisfaction by 10 points";
        nodes.back().icon = "😊";
        nodes.back().effect_value = 10.0f;
        nodes.back().prerequisites.push_back("income_boost");
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
     */
    bool CanUnlock(const ResearchNode& node) const {
        // Check if already unlocked
        if (node.state == ResearchNodeState::Unlocked) {
            return false;
        }
        
        // Check if we have enough points
        if (research_points < node.cost) {
            return false;
        }
        
        // Check prerequisites
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
        research_points -= node->cost;
        
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
     * @brief Award research points (e.g., for reaching milestones)
     */
    void AwardPoints(int points) {
        research_points += points;
        total_points_earned += points;
        UpdateNodeStates();
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

} // namespace Core
} // namespace TowerForge
