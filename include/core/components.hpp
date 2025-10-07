#pragma once

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
 * @brief Component for Person entities with state machine and movement tracking
 * 
 * This component extends Actor with detailed state tracking for simulation
 * of individuals moving through the tower. People can walk on floors,
 * use elevators, and have specific destinations.
 */
struct Person {
    std::string name;
    PersonState state;
    
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
           float speed = 2.0f)
        : name(n),
          state(PersonState::Idle),
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

} // namespace Core
} // namespace TowerForge
