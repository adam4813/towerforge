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

} // namespace Core
} // namespace TowerForge
