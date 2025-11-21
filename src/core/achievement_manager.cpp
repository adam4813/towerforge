#include "core/achievement_manager.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge::core {

    // Helper function to format time
    std::string Achievement::GetUnlockDateString() const {
        if (!unlock_time.has_value()) {
            return "Locked";
        }
    
        auto time_t_val = std::chrono::system_clock::to_time_t(unlock_time.value());
        std::tm tm_val;
    
#ifdef _WIN32
        localtime_s(&tm_val, &time_t_val);
#else
        localtime_r(&time_t_val, &tm_val);
#endif
    
        std::ostringstream oss;
        oss << std::put_time(&tm_val, "%Y-%m-%d %H:%M");
        return oss.str();
    }

    AchievementManager::AchievementManager()
        : last_population_(0),
          last_income_(0.0f),
          last_floors_(0),
          last_satisfaction_(0.0f) {
    }

    AchievementManager::~AchievementManager() {
    }

    void AchievementManager::Initialize() {
        DefineDefaultAchievements();
    }

    void AchievementManager::DefineDefaultAchievements() {
        achievements_.clear();
    
        // Population achievements
        achievements_.emplace_back("pop_10", "First Tenants", "Have 10 tenants in your tower", "👥", AchievementType::Population, 10);
        achievements_.emplace_back("pop_50", "Growing Community", "Have 50 tenants in your tower", "🏘️", AchievementType::Population, 50);
        achievements_.emplace_back("pop_100", "Century Club", "Have 100 tenants in your tower", "🏆", AchievementType::Population, 100);
        achievements_.emplace_back("pop_250", "Thriving Tower", "Have 250 tenants in your tower", "🌟", AchievementType::Population, 250);
        achievements_.emplace_back("pop_500", "Mega Tower", "Have 500 tenants in your tower", "🏙️", AchievementType::Population, 500);
        achievements_.emplace_back("pop_1000", "Skyscraper City", "Have 1000 tenants in your tower", "🌆", AchievementType::Population, 1000);
    
        // Income achievements
        achievements_.emplace_back("income_10k", "First Profit", "Earn $10,000 in total revenue", "💵", AchievementType::Income, 10000);
        achievements_.emplace_back("income_50k", "Making Money", "Earn $50,000 in total revenue", "💰", AchievementType::Income, 50000);
        achievements_.emplace_back("income_100k", "Six Figures", "Earn $100,000 in total revenue", "💸", AchievementType::Income, 100000);
        achievements_.emplace_back("income_500k", "Real Estate Mogul", "Earn $500,000 in total revenue", "🤑", AchievementType::Income, 500000);
        achievements_.emplace_back("income_1m", "Millionaire", "Earn $1,000,000 in total revenue", "💎", AchievementType::Income, 1000000);
    
        // Floor/building achievements
        achievements_.emplace_back("floors_5", "Rising Up", "Build a tower with 5 floors", "🏢", AchievementType::Floors, 5);
        achievements_.emplace_back("floors_10", "Two Stories", "Build a tower with 10 floors", "🏗️", AchievementType::Floors, 10);
        achievements_.emplace_back("floors_25", "Quarter Century", "Build a tower with 25 floors", "🏛️", AchievementType::Floors, 25);
        achievements_.emplace_back("floors_50", "Halfway to the Sky", "Build a tower with 50 floors", "🗼", AchievementType::Floors, 50);
        achievements_.emplace_back("floors_100", "Centennial Tower", "Build a tower with 100 floors", "🏙️", AchievementType::Floors, 100);
    
        // Satisfaction achievements
        achievements_.emplace_back("satisfaction_80", "Happy Tenants", "Maintain 80% average satisfaction", "😊", AchievementType::Satisfaction, 80);
        achievements_.emplace_back("satisfaction_90", "Very Satisfied", "Maintain 90% average satisfaction", "😃", AchievementType::Satisfaction, 90);
        achievements_.emplace_back("satisfaction_95", "Five Star Tower", "Maintain 95% average satisfaction", "⭐", AchievementType::Satisfaction, 95);
    
        // Special events
        achievements_.emplace_back("first_day", "Day One", "Complete your first day in business", "📅", AchievementType::SpecialEvent, 1);
        achievements_.emplace_back("first_week", "Week One", "Survive your first week", "🗓️", AchievementType::SpecialEvent, 7);
        achievements_.emplace_back("first_month", "One Month Strong", "Operate for 30 days", "📆", AchievementType::SpecialEvent, 30);
    }

    void AchievementManager::CheckAchievements(const int population, const float total_income, const int floor_count, const float avg_satisfaction) {
        // Check population achievements
        if (population != last_population_) {
            for (auto& achievement : achievements_) {
                if (achievement.type == AchievementType::Population && 
                    !achievement.unlocked && 
                    population >= achievement.requirement) {
                    achievement.Unlock();
                    newly_unlocked_.push_back(achievement.id);
                }
            }
            last_population_ = population;
        }
    
        // Check income achievements
        if (total_income != last_income_) {
            for (auto& achievement : achievements_) {
                if (achievement.type == AchievementType::Income && 
                    !achievement.unlocked && 
                    total_income >= achievement.requirement) {
                    achievement.Unlock();
                    newly_unlocked_.push_back(achievement.id);
                }
            }
            last_income_ = total_income;
        }
    
        // Check floor achievements
        if (floor_count != last_floors_) {
            for (auto& achievement : achievements_) {
                if (achievement.type == AchievementType::Floors && 
                    !achievement.unlocked && 
                    floor_count >= achievement.requirement) {
                    achievement.Unlock();
                    newly_unlocked_.push_back(achievement.id);
                }
            }
            last_floors_ = floor_count;
        }
    
        // Check satisfaction achievements (only if satisfaction is meaningful)
        if (population > 0 && avg_satisfaction != last_satisfaction_) {
            for (auto& achievement : achievements_) {
                if (achievement.type == AchievementType::Satisfaction && 
                    !achievement.unlocked && 
                    avg_satisfaction >= achievement.requirement) {
                    achievement.Unlock();
                    newly_unlocked_.push_back(achievement.id);
                }
            }
            last_satisfaction_ = avg_satisfaction;
        }
    }

    bool AchievementManager::UnlockAchievement(const std::string& achievement_id) {
        Achievement* achievement = FindAchievement(achievement_id);
        if (achievement && !achievement->unlocked) {
            achievement->Unlock();
            newly_unlocked_.push_back(achievement_id);
            return true;
        }
        return false;
    }

    std::vector<Achievement> AchievementManager::GetUnlockedAchievements() const {
        std::vector<Achievement> unlocked;
        for (const auto& achievement : achievements_) {
            if (achievement.unlocked) {
                unlocked.push_back(achievement);
            }
        }
        return unlocked;
    }

    std::vector<Achievement> AchievementManager::GetLockedAchievements() const {
        std::vector<Achievement> locked;
        for (const auto& achievement : achievements_) {
            if (!achievement.unlocked) {
                locked.push_back(achievement);
            }
        }
        return locked;
    }

    int AchievementManager::GetUnlockedCount() const {
        int count = 0;
        for (const auto& achievement : achievements_) {
            if (achievement.unlocked) {
                count++;
            }
        }
        return count;
    }

    float AchievementManager::GetProgressPercentage() const {
        if (achievements_.empty()) {
            return 0.0f;
        }
        return (static_cast<float>(GetUnlockedCount()) / static_cast<float>(achievements_.size())) * 100.0f;
    }

    std::vector<std::string> AchievementManager::PopNewlyUnlocked() {
        std::vector<std::string> result = newly_unlocked_;
        newly_unlocked_.clear();
        return result;
    }

    nlohmann::json AchievementManager::Serialize() const {
        nlohmann::json json;
        json["version"] = "1.0";
        json["achievements"] = nlohmann::json::array();
    
        for (const auto& achievement : achievements_) {
            nlohmann::json ach_json;
            ach_json["id"] = achievement.id;
            ach_json["unlocked"] = achievement.unlocked;
        
            if (achievement.unlock_time.has_value()) {
                auto time_since_epoch = achievement.unlock_time.value().time_since_epoch();
                auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
                ach_json["unlock_time"] = seconds;
            }
        
            json["achievements"].push_back(ach_json);
        }
    
        return json;
    }

    void AchievementManager::Deserialize(const nlohmann::json& json) {
        if (!json.contains("achievements")) {
            return;
        }
    
        for (const auto& ach_json : json["achievements"]) {
            std::string id = ach_json.value("id", "");
            Achievement* achievement = FindAchievement(id);
        
            if (achievement) {
                achievement->unlocked = ach_json.value("unlocked", false);
            
                if (ach_json.contains("unlock_time")) {
                    auto seconds = ach_json["unlock_time"].get<int64_t>();
                    auto duration = std::chrono::seconds(seconds);
                    achievement->unlock_time = std::chrono::system_clock::time_point(duration);
                }
            }
        }
    }

    void AchievementManager::ResetAll() {
        for (auto& achievement : achievements_) {
            achievement.unlocked = false;
            achievement.unlock_time = std::nullopt;
        }
        newly_unlocked_.clear();
        last_population_ = 0;
        last_income_ = 0.0f;
        last_floors_ = 0;
        last_satisfaction_ = 0.0f;
    }

    Achievement* AchievementManager::FindAchievement(const std::string& id) {
        for (auto& achievement : achievements_) {
            if (achievement.id == id) {
                return &achievement;
            }
        }
        return nullptr;
    }

}
