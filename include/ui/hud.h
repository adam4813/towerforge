#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace towerforge::ui {

    // Forward declarations
    class UIWindowManager;
    class TooltipManager;
    class NotificationCenter;
    struct IncomeBreakdown;
    struct ElevatorAnalytics;
    struct PopulationBreakdown;
    class PopulationAnalyticsOverlay;

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
        float cleanliness;          // 0-100
        float maintenance_level;    // 0-100
        std::string cleanliness_rating;
        std::string maintenance_rating;
        bool has_fire;
        bool has_security_issue;
        // CleanlinessStatus state info
        std::string cleanliness_state;  // "Clean", "Needs Cleaning", or "Dirty"
        bool needs_cleaning;            // True if facility needs cleaning
        // MaintenanceStatus state info
        std::string maintenance_state;  // "Good", "Needs Service", or "Broken"
        bool needs_repair;              // True if facility needs repair
        bool is_broken;                 // True if facility is broken
        // Adjacency effects
        std::vector<std::string> adjacency_effects;  // List of adjacency effect descriptions
    };

    /**
 * @brief Structure to hold person/actor information
 */
    struct PersonInfo {
        int id;
        std::string name;
        std::string npc_type;      // "Visitor", "Employee", or "Staff"
        std::string state;
        std::string status;        // Current activity/status (e.g., "Shopping", "On shift: Office Worker", "Cleaning")
        int current_floor;
        int destination_floor;
        float wait_time;
        std::string needs;
        float satisfaction;
        // Staff-specific fields
        bool is_staff;
        std::string staff_role;    // "Janitor", "Maintenance", etc.
        bool on_duty;
        std::string shift_hours;
        // Visitor needs fields
        bool has_needs;
        std::string visitor_archetype;
        float hunger_need;
        float entertainment_need;
        float comfort_need;
        float shopping_need;
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
    
        Notification(const Type t, const std::string& msg, const float duration = 5.0f)
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
     * @brief Show facility info window (creates or updates)
     */
        void ShowFacilityInfo(const FacilityInfo& info) const;
    
        /**
     * @brief Show person info window (creates or updates)
     */
        void ShowPersonInfo(const PersonInfo& info) const;
    
        /**
     * @brief Show elevator info window (creates or updates)
     */
        void ShowElevatorInfo(const ElevatorInfo& info) const;
    
        /**
     * @brief Hide all info windows
     */
        void HideInfoPanels() const;
    
        /**
     * @brief Get the window manager
     */
        UIWindowManager* GetWindowManager() const { return window_manager_.get(); }
    
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
        bool HandleClick(int mouse_x, int mouse_y) const;
    
        /**
     * @brief Update tooltips based on mouse position
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     */
        void UpdateTooltips(int mouse_x, int mouse_y) const;
    
        /**
     * @brief Get the tooltip manager
     */
        TooltipManager* GetTooltipManager() const { return tooltip_manager_.get(); }
    
        /**
     * @brief Get the notification center
     */
        NotificationCenter* GetNotificationCenter() const { return notification_center_.get(); }
    
        /**
     * @brief Toggle notification center visibility
     */
        void ToggleNotificationCenter();
    
        /**
     * @brief Show income analytics overlay
     * @param data Income breakdown data to display
     */
        void ShowIncomeAnalytics(const IncomeBreakdown& data) const;
    
        /**
     * @brief Show elevator analytics overlay
     * @param data Elevator analytics data to display
     */
        void ShowElevatorAnalytics(const ElevatorAnalytics& data) const;
    
        /**
     * @brief Show population analytics overlay
     * @param data Population breakdown data to display
     */
        void ShowPopulationAnalytics(const PopulationBreakdown& data) const;
    
        /**
     * @brief Set callback for collecting income analytics
     * @param callback Function to call when income analytics are needed
     */
        void SetIncomeAnalyticsCallback(std::function<IncomeBreakdown()> callback);
    
        /**
     * @brief Set callback for collecting population analytics
     * @param callback Function to call when population analytics are needed
     */
        void SetPopulationAnalyticsCallback(std::function<PopulationBreakdown()> callback);
    
        /**
     * @brief Request income analytics to be shown
     */
        void RequestIncomeAnalytics();
    
        /**
     * @brief Request population analytics to be shown
     */
        void RequestPopulationAnalytics();
    
    private:
        void RenderTopBar() const;
        void RenderStarRating() const;
        void RenderNotifications();
        void RenderSpeedControls() const;
        void RenderEndGameSummary() const;
        
        bool IsMouseOverIncomeArea(int mouse_x, int mouse_y) const;
        bool IsMouseOverPopulationArea(int mouse_x, int mouse_y) const;

        static std::string FormatTime(float time);
    
        GameState game_state_;
    
        // Window manager for info windows
        std::unique_ptr<UIWindowManager> window_manager_;
    
        // Tooltip manager
        std::unique_ptr<TooltipManager> tooltip_manager_;
    
        // Notification center
        std::unique_ptr<NotificationCenter> notification_center_;
    
        // Legacy notifications (for backward compatibility with toasts)
        std::vector<Notification> notifications_;
        
        // Analytics callbacks
        std::function<IncomeBreakdown()> income_analytics_callback_;
        std::function<PopulationBreakdown()> population_analytics_callback_;
    
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

}
