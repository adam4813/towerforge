#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <chrono>

namespace TowerForge {
namespace Core {

// Forward declarations
class ECSWorld;
class TowerGrid;
class FacilityManager;
class AchievementManager;

/**
 * @brief Metadata for a save slot
 */
struct SaveSlotInfo {
    std::string slot_name;
    std::string tower_name;
    std::string file_path;
    std::string save_date;      // ISO 8601 format
    std::string game_version;
    int current_day;
    float current_time;
    float total_balance;
    int population;
    
    SaveSlotInfo() = default;
    SaveSlotInfo(const std::string& name, const std::string& path)
        : slot_name(name), file_path(path), tower_name("Unknown"),
          save_date(""), game_version("0.1.0"), current_day(1),
          current_time(8.0f), total_balance(0.0f), population(0) {}
};

/**
 * @brief Error types that can occur during save/load operations
 */
enum class SaveLoadError {
    None,
    DiskFull,
    CorruptFile,
    VersionMismatch,
    FileNotFound,
    PermissionDenied,
    InvalidSlotName,
    SlotAlreadyExists,
    UnknownError
};

/**
 * @brief Result of a save/load operation
 */
struct SaveLoadResult {
    bool success;
    SaveLoadError error;
    std::string error_message;
    
    SaveLoadResult() : success(true), error(SaveLoadError::None), error_message("") {}
    SaveLoadResult(SaveLoadError err, const std::string& msg)
        : success(false), error(err), error_message(msg) {}
    
    static SaveLoadResult Success() { return SaveLoadResult(); }
    static SaveLoadResult Failure(SaveLoadError err, const std::string& msg) {
        return SaveLoadResult(err, msg);
    }
};

/**
 * @brief Manager for save/load operations
 * 
 * Handles serialization/deserialization of game state using nlohmann/json,
 * manages save slots, and provides autosave functionality.
 */
class SaveLoadManager {
public:
    SaveLoadManager();
    ~SaveLoadManager();
    
    /**
     * @brief Initialize the save system (create save directory, etc.)
     */
    void Initialize();
    
    /**
     * @brief Save the current game state to a slot
     * @param slot_name Name of the save slot
     * @param tower_name Name of the tower (for display)
     * @param ecs_world Reference to the ECS world
     * @return Result of the save operation
     */
    SaveLoadResult SaveGame(const std::string& slot_name,
                           const std::string& tower_name,
                           ECSWorld& ecs_world);
    
    /**
     * @brief Load a game state from a slot
     * @param slot_name Name of the save slot
     * @param ecs_world Reference to the ECS world to load into
     * @return Result of the load operation
     */
    SaveLoadResult LoadGame(const std::string& slot_name,
                           ECSWorld& ecs_world);
    
    /**
     * @brief Get list of all available save slots
     * @return Vector of save slot information
     */
    std::vector<SaveSlotInfo> GetSaveSlots() const;
    
    /**
     * @brief Delete a save slot
     * @param slot_name Name of the save slot to delete
     * @return Result of the delete operation
     */
    SaveLoadResult DeleteSlot(const std::string& slot_name);
    
    /**
     * @brief Rename a save slot
     * @param old_name Current slot name
     * @param new_name New slot name
     * @return Result of the rename operation
     */
    SaveLoadResult RenameSlot(const std::string& old_name,
                             const std::string& new_name);
    
    /**
     * @brief Check if a slot name is valid
     * @param slot_name Slot name to check
     * @return True if valid, false otherwise
     */
    bool IsValidSlotName(const std::string& slot_name) const;
    
    /**
     * @brief Check if a slot exists
     * @param slot_name Slot name to check
     * @return True if exists, false otherwise
     */
    bool SlotExists(const std::string& slot_name) const;
    
    /**
     * @brief Enable/disable autosave
     * @param enabled Whether autosave should be enabled
     */
    void SetAutosaveEnabled(bool enabled);
    
    /**
     * @brief Set autosave interval in seconds
     * @param interval Seconds between autosaves
     */
    void SetAutosaveInterval(float interval);
    
    /**
     * @brief Update autosave timer and perform autosave if needed
     * @param delta_time Time elapsed since last update
     * @param ecs_world Reference to the ECS world
     */
    void UpdateAutosave(float delta_time, ECSWorld& ecs_world);
    
    /**
     * @brief Perform an immediate autosave
     * @param ecs_world Reference to the ECS world
     * @return Result of the autosave operation
     */
    SaveLoadResult Autosave(ECSWorld& ecs_world);
    
    /**
     * @brief Get the time since last save
     * @return Seconds since last save
     */
    float GetTimeSinceLastSave() const;
    
    /**
     * @brief Get the last error message
     * @return Last error message
     */
    std::string GetLastErrorMessage() const { return last_error_message_; }
    
    /**
     * @brief Get autosave enabled status
     * @return True if autosave is enabled
     */
    bool IsAutosaveEnabled() const { return autosave_enabled_; }
    
    /**
     * @brief Set the achievement manager for persistence
     * @param manager Pointer to achievement manager
     */
    void SetAchievementManager(AchievementManager* manager);

private:
    std::string GetSavePath(const std::string& slot_name) const;
    std::string GetSaveDirectory() const;
    nlohmann::json SerializeGameState(ECSWorld& ecs_world, const std::string& tower_name);
    bool DeserializeGameState(const nlohmann::json& json, ECSWorld& ecs_world);
    bool ValidateVersion(const nlohmann::json& json) const;
    SaveLoadResult CheckDiskSpace(const std::string& path) const;
    
    std::string save_directory_;
    std::string last_error_message_;
    bool autosave_enabled_;
    float autosave_interval_;
    float time_since_last_save_;
    std::string last_save_slot_;
    AchievementManager* achievement_manager_;  // Optional achievement manager for persistence
    
    static constexpr const char* SAVE_FILE_EXTENSION = ".tfsave";
    static constexpr const char* AUTOSAVE_SLOT_NAME = "_autosave";
    static constexpr const char* GAME_VERSION = "0.1.0";
};

} // namespace Core
} // namespace TowerForge
