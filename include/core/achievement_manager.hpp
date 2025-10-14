#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <optional>

namespace TowerForge::Core {

    /**
 * @brief Types of achievements available in the game
 */
    enum class AchievementType {
        Population,      // Population milestones
        Income,          // Income/financial milestones
        Floors,          // Building size milestones
        Satisfaction,    // Tenant satisfaction milestones
        SpecialEvent     // Unique events
    };

    /**
 * @brief Single achievement definition
 */
    struct Achievement {
        std::string id;                    // Unique identifier
        std::string name;                  // Display name
        std::string description;           // Description
        std::string icon;                  // Icon/emoji for display
        AchievementType type;              // Type of achievement
        int requirement;                   // Requirement value (e.g., 100 for 100 tenants)
        bool unlocked;                     // Whether achievement is unlocked
        std::optional<std::chrono::system_clock::time_point> unlock_time;  // When it was unlocked
    
        Achievement(const std::string& id_val,
                    const std::string& name_val,
                    const std::string& desc_val,
                    const std::string& icon_val,
                    const AchievementType type_val,
                    const int req_val)
            : id(id_val),
              name(name_val),
              description(desc_val),
              icon(icon_val),
              type(type_val),
              requirement(req_val),
              unlocked(false),
              unlock_time(std::nullopt) {}
    
        /**
     * @brief Unlock this achievement
     */
        void Unlock() {
            if (!unlocked) {
                unlocked = true;
                unlock_time = std::chrono::system_clock::now();
            }
        }
    
        /**
     * @brief Get formatted unlock date string
     */
        std::string GetUnlockDateString() const;
    };

    /**
 * @brief Manager for tracking and storing achievements
 * 
 * Handles achievement definitions, progress tracking, unlocking,
 * and persistence.
 */
    class AchievementManager {
    public:
        AchievementManager();
        ~AchievementManager();
    
        /**
     * @brief Initialize the achievement system with default achievements
     */
        void Initialize();
    
        /**
     * @brief Update achievement progress based on game state
     * @param population Current tower population
     * @param total_income Total income earned
     * @param floor_count Number of floors in tower
     * @param avg_satisfaction Average satisfaction score
     */
        void CheckAchievements(int population, float total_income, int floor_count, float avg_satisfaction);
    
        /**
     * @brief Manually unlock a specific achievement
     * @param achievement_id ID of achievement to unlock
     * @return True if achievement was unlocked, false if already unlocked or not found
     */
        bool UnlockAchievement(const std::string& achievement_id);
    
        /**
     * @brief Get all achievements (both locked and unlocked)
     * @return Vector of all achievements
     */
        const std::vector<Achievement>& GetAllAchievements() const { return achievements_; }
    
        /**
     * @brief Get only unlocked achievements
     * @return Vector of unlocked achievements
     */
        std::vector<Achievement> GetUnlockedAchievements() const;
    
        /**
     * @brief Get only locked achievements
     * @return Vector of locked achievements
     */
        std::vector<Achievement> GetLockedAchievements() const;
    
        /**
     * @brief Get count of unlocked achievements
     * @return Number of unlocked achievements
     */
        int GetUnlockedCount() const;
    
        /**
     * @brief Get total number of achievements
     * @return Total achievement count
     */
        int GetTotalCount() const { return static_cast<int>(achievements_.size()); }
    
        /**
     * @brief Get progress percentage (0-100)
     * @return Percentage of achievements unlocked
     */
        float GetProgressPercentage() const;
    
        /**
     * @brief Check if there are newly unlocked achievements since last check
     * @return True if new achievements were unlocked
     */
        bool HasNewAchievements() const { return !newly_unlocked_.empty(); }
    
        /**
     * @brief Get and clear newly unlocked achievements
     * @return Vector of newly unlocked achievement IDs
     */
        std::vector<std::string> PopNewlyUnlocked();
    
        /**
     * @brief Serialize achievements to JSON
     * @return JSON representation of all achievements
     */
        nlohmann::json Serialize() const;
    
        /**
     * @brief Deserialize achievements from JSON
     * @param json JSON data to deserialize
     */
        void Deserialize(const nlohmann::json& json);
    
        /**
     * @brief Reset all achievements (for testing)
     */
        void ResetAll();

    private:
        void DefineDefaultAchievements();
        Achievement* FindAchievement(const std::string& id);
    
        std::vector<Achievement> achievements_;
        std::vector<std::string> newly_unlocked_;  // IDs of recently unlocked achievements
    
        // Track last values to avoid re-checking
        int last_population_;
        float last_income_;
        int last_floors_;
        float last_satisfaction_;
    };

}
