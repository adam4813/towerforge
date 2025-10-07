#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge {
namespace ui {

/**
 * @brief Structure to hold tower rating information
 */
struct TowerRating {
    int stars = 1;                    // Current star rating (1-5)
    float average_satisfaction = 0.0f; // Average satisfaction across all tenants
    int total_tenants = 0;            // Total number of tenants
    int total_floors = 1;             // Total number of floors
    float hourly_income = 0.0f;       // Net income per hour
    
    // Next milestone info
    int next_star_tenants = 25;       // Tenants needed for next star
    float next_star_satisfaction = 0.0f; // Satisfaction needed for next star
    int next_star_floors = 0;         // Floors needed for next star
    float next_star_income = 0.0f;    // Income needed for next star
};

/**
 * @brief Structure to hold game state information for HUD display
 */
struct GameState {
    float funds = 25000.0f;
    float income_rate = 500.0f;
    int population = 0;
    int current_day = 1;
    float current_time = 8.5f;  // 8:30 AM represented as 8.5
    int speed_multiplier = 1;   // 1x, 2x, 4x
    bool paused = false;
    TowerRating rating;           // Tower rating and progression
};

/**
 * @brief Structure to hold facility information
 */
struct FacilityInfo {
    std::string type;
    int floor;
    int occupancy;
    int max_occupancy;
    float revenue;
    float satisfaction;
    int tenant_count;
};

/**
 * @brief Structure to hold person/actor information
 */
struct PersonInfo {
    int id;
    std::string state;
    int current_floor;
    int destination_floor;
    float wait_time;
    std::string needs;
    float satisfaction;
};

/**
 * @brief Structure to hold elevator information
 */
struct ElevatorInfo {
    int id;
    int current_floor;
    std::string direction;
    int occupancy;
    int max_occupancy;
    int next_stop;
    std::vector<std::pair<int, int>> queue;  // floor, waiting count
};

/**
 * @brief Structure for notifications
 */
struct Notification {
    enum class Type {
        Warning,
        Success,
        Info,
        Error
    };
    
    Type type;
    std::string message;
    float time_remaining;  // seconds until disappears
    
    Notification(Type t, const std::string& msg, float duration = 5.0f)
        : type(t), message(msg), time_remaining(duration) {}
};

/**
 * @brief Main HUD class for rendering game information
 */
class HUD {
public:
    HUD();
    ~HUD();
    
    /**
     * @brief Update HUD state (called every frame)
     * @param delta_time Time elapsed since last frame
     */
    void Update(float delta_time);
    
    /**
     * @brief Render the HUD
     */
    void Render();
    
    /**
     * @brief Set the game state to display
     */
    void SetGameState(const GameState& state);
    
    /**
     * @brief Show facility info panel
     */
    void ShowFacilityInfo(const FacilityInfo& info);
    
    /**
     * @brief Show person info panel
     */
    void ShowPersonInfo(const PersonInfo& info);
    
    /**
     * @brief Show elevator info panel
     */
    void ShowElevatorInfo(const ElevatorInfo& info);
    
    /**
     * @brief Hide all info panels
     */
    void HideInfoPanels();
    
    /**
     * @brief Add a notification
     */
    void AddNotification(Notification::Type type, const std::string& message, float duration = 5.0f);
    
    /**
     * @brief Handle mouse click for entity selection
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @return true if click was on a HUD element
     */
    bool HandleClick(int mouse_x, int mouse_y);
    
private:
    void RenderTopBar();
    void RenderStarRating();
    void RenderFacilityPanel();
    void RenderPersonPanel();
    void RenderElevatorPanel();
    void RenderNotifications();
    void RenderSpeedControls();
    void RenderEndGameSummary();
    
    std::string FormatTime(float time);
    std::string GetSatisfactionEmoji(float satisfaction);
    
    GameState game_state_;
    
    // Info panels
    bool show_facility_panel_;
    bool show_person_panel_;
    bool show_elevator_panel_;
    
    FacilityInfo facility_info_;
    PersonInfo person_info_;
    ElevatorInfo elevator_info_;
    
    // Notifications
    std::vector<Notification> notifications_;
    
    // UI layout constants
    static constexpr int TOP_BAR_HEIGHT = 40;
    static constexpr int PANEL_WIDTH = 250;
    static constexpr int PANEL_PADDING = 10;
    static constexpr int NOTIFICATION_WIDTH = 300;
    static constexpr int NOTIFICATION_HEIGHT = 30;
    static constexpr int SPEED_CONTROL_WIDTH = 200;
    static constexpr int SPEED_CONTROL_HEIGHT = 40;
    static constexpr int STAR_RATING_WIDTH = 230;
    static constexpr int STAR_RATING_HEIGHT = 180;
};

} // namespace ui
} // namespace towerforge
