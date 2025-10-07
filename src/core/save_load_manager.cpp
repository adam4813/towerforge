#include "core/save_load_manager.hpp"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/achievement_manager.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/statvfs.h>
#endif

namespace TowerForge {
namespace Core {

SaveLoadManager::SaveLoadManager()
    : autosave_enabled_(true),
      autosave_interval_(120.0f),  // Default: 2 minutes
      time_since_last_save_(0.0f),
      achievement_manager_(nullptr) {
}

SaveLoadManager::~SaveLoadManager() {
}

void SaveLoadManager::Initialize() {
    // Determine save directory based on platform
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata) {
        save_directory_ = std::string(appdata) + "/TowerForge/saves";
    } else {
        save_directory_ = "./saves";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        save_directory_ = std::string(home) + "/.towerforge/saves";
    } else {
        save_directory_ = "./saves";
    }
#endif
    
    // Create save directory if it doesn't exist
    try {
        std::filesystem::create_directories(save_directory_);
        std::cout << "Save directory: " << save_directory_ << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create save directory: " << e.what() << std::endl;
        save_directory_ = "./saves";
        std::filesystem::create_directories(save_directory_);
    }
}

std::string SaveLoadManager::GetSavePath(const std::string& slot_name) const {
    return save_directory_ + "/" + slot_name + SAVE_FILE_EXTENSION;
}

std::string SaveLoadManager::GetSaveDirectory() const {
    return save_directory_;
}

SaveLoadResult SaveLoadManager::SaveGame(const std::string& slot_name,
                                         const std::string& tower_name,
                                         ECSWorld& ecs_world) {
    // Validate slot name
    if (!IsValidSlotName(slot_name)) {
        return SaveLoadResult::Failure(SaveLoadError::InvalidSlotName,
                                      "Invalid slot name: contains illegal characters");
    }
    
    // Check disk space
    SaveLoadResult disk_check = CheckDiskSpace(save_directory_);
    if (!disk_check.success) {
        last_error_message_ = disk_check.error_message;
        return disk_check;
    }
    
    try {
        // Serialize game state
        nlohmann::json json = SerializeGameState(ecs_world, tower_name);
        
        // Write to file
        std::string file_path = GetSavePath(slot_name);
        std::ofstream file(file_path);
        if (!file.is_open()) {
            return SaveLoadResult::Failure(SaveLoadError::PermissionDenied,
                                          "Failed to create save file");
        }
        
        file << std::setw(2) << json << std::endl;
        file.close();
        
        std::cout << "Game saved to: " << file_path << std::endl;
        time_since_last_save_ = 0.0f;
        last_save_slot_ = slot_name;
        
        return SaveLoadResult::Success();
    } catch (const std::exception& e) {
        last_error_message_ = std::string("Save failed: ") + e.what();
        return SaveLoadResult::Failure(SaveLoadError::UnknownError, last_error_message_);
    }
}

SaveLoadResult SaveLoadManager::LoadGame(const std::string& slot_name,
                                         ECSWorld& ecs_world) {
    std::string file_path = GetSavePath(slot_name);
    
    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        return SaveLoadResult::Failure(SaveLoadError::FileNotFound,
                                      "Save file not found: " + slot_name);
    }
    
    try {
        // Read file
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return SaveLoadResult::Failure(SaveLoadError::PermissionDenied,
                                          "Failed to open save file");
        }
        
        nlohmann::json json;
        file >> json;
        file.close();
        
        // Validate version
        if (!ValidateVersion(json)) {
            return SaveLoadResult::Failure(SaveLoadError::VersionMismatch,
                                          "Save file version is incompatible");
        }
        
        // Deserialize game state
        if (!DeserializeGameState(json, ecs_world)) {
            return SaveLoadResult::Failure(SaveLoadError::CorruptFile,
                                          "Failed to load game state - file may be corrupt");
        }
        
        std::cout << "Game loaded from: " << file_path << std::endl;
        return SaveLoadResult::Success();
        
    } catch (const nlohmann::json::exception& e) {
        last_error_message_ = std::string("Failed to parse save file: ") + e.what();
        return SaveLoadResult::Failure(SaveLoadError::CorruptFile, last_error_message_);
    } catch (const std::exception& e) {
        last_error_message_ = std::string("Load failed: ") + e.what();
        return SaveLoadResult::Failure(SaveLoadError::UnknownError, last_error_message_);
    }
}

std::vector<SaveSlotInfo> SaveLoadManager::GetSaveSlots() const {
    std::vector<SaveSlotInfo> slots;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(save_directory_)) {
            if (entry.is_regular_file() && entry.path().extension() == SAVE_FILE_EXTENSION) {
                std::string slot_name = entry.path().stem().string();
                
                // Try to read metadata from file
                try {
                    std::ifstream file(entry.path());
                    nlohmann::json json;
                    file >> json;
                    file.close();
                    
                    SaveSlotInfo info(slot_name, entry.path().string());
                    if (json.contains("metadata")) {
                        auto& meta = json["metadata"];
                        info.tower_name = meta.value("tower_name", "Unknown");
                        info.save_date = meta.value("save_date", "");
                        info.game_version = meta.value("game_version", "0.1.0");
                        info.current_day = meta.value("current_day", 1);
                        info.current_time = meta.value("current_time", 8.0f);
                        info.total_balance = meta.value("total_balance", 0.0f);
                        info.population = meta.value("population", 0);
                    }
                    slots.push_back(info);
                } catch (const std::exception& e) {
                    // If we can't read the file, still add it to the list with default info
                    SaveSlotInfo info(slot_name, entry.path().string());
                    slots.push_back(info);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning save directory: " << e.what() << std::endl;
    }
    
    return slots;
}

SaveLoadResult SaveLoadManager::DeleteSlot(const std::string& slot_name) {
    if (!SlotExists(slot_name)) {
        return SaveLoadResult::Failure(SaveLoadError::FileNotFound,
                                      "Save slot does not exist");
    }
    
    try {
        std::string file_path = GetSavePath(slot_name);
        std::filesystem::remove(file_path);
        std::cout << "Deleted save slot: " << slot_name << std::endl;
        return SaveLoadResult::Success();
    } catch (const std::exception& e) {
        last_error_message_ = std::string("Failed to delete slot: ") + e.what();
        return SaveLoadResult::Failure(SaveLoadError::UnknownError, last_error_message_);
    }
}

SaveLoadResult SaveLoadManager::RenameSlot(const std::string& old_name,
                                          const std::string& new_name) {
    if (!SlotExists(old_name)) {
        return SaveLoadResult::Failure(SaveLoadError::FileNotFound,
                                      "Source save slot does not exist");
    }
    
    if (SlotExists(new_name)) {
        return SaveLoadResult::Failure(SaveLoadError::SlotAlreadyExists,
                                      "Target slot name already exists");
    }
    
    if (!IsValidSlotName(new_name)) {
        return SaveLoadResult::Failure(SaveLoadError::InvalidSlotName,
                                      "Invalid slot name: contains illegal characters");
    }
    
    try {
        std::string old_path = GetSavePath(old_name);
        std::string new_path = GetSavePath(new_name);
        std::filesystem::rename(old_path, new_path);
        std::cout << "Renamed save slot: " << old_name << " -> " << new_name << std::endl;
        return SaveLoadResult::Success();
    } catch (const std::exception& e) {
        last_error_message_ = std::string("Failed to rename slot: ") + e.what();
        return SaveLoadResult::Failure(SaveLoadError::UnknownError, last_error_message_);
    }
}

bool SaveLoadManager::IsValidSlotName(const std::string& slot_name) const {
    if (slot_name.empty() || slot_name.length() > 64) {
        return false;
    }
    
    // Check for illegal characters
    for (char c : slot_name) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || 
            c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            return false;
        }
    }
    
    return true;
}

bool SaveLoadManager::SlotExists(const std::string& slot_name) const {
    std::string file_path = GetSavePath(slot_name);
    return std::filesystem::exists(file_path);
}

void SaveLoadManager::SetAutosaveEnabled(bool enabled) {
    autosave_enabled_ = enabled;
    std::cout << "Autosave " << (enabled ? "enabled" : "disabled") << std::endl;
}

void SaveLoadManager::SetAutosaveInterval(float interval) {
    autosave_interval_ = interval;
    std::cout << "Autosave interval set to " << interval << " seconds" << std::endl;
}

void SaveLoadManager::UpdateAutosave(float delta_time, ECSWorld& ecs_world) {
    if (!autosave_enabled_) {
        return;
    }
    
    time_since_last_save_ += delta_time;
    
    if (time_since_last_save_ >= autosave_interval_) {
        Autosave(ecs_world);
    }
}

SaveLoadResult SaveLoadManager::Autosave(ECSWorld& ecs_world) {
    std::cout << "Performing autosave..." << std::endl;
    SaveLoadResult result = SaveGame(AUTOSAVE_SLOT_NAME, "Autosave", ecs_world);
    if (result.success) {
        std::cout << "Autosave completed successfully" << std::endl;
    } else {
        std::cout << "Autosave failed: " << result.error_message << std::endl;
    }
    return result;
}

float SaveLoadManager::GetTimeSinceLastSave() const {
    return time_since_last_save_;
}

void SaveLoadManager::SetAchievementManager(AchievementManager* manager) {
    achievement_manager_ = manager;
}

nlohmann::json SaveLoadManager::SerializeGameState(ECSWorld& ecs_world,
                                                   const std::string& tower_name) {
    nlohmann::json json;
    
    // Metadata
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    json["metadata"] = {
        {"game_version", GAME_VERSION},
        {"save_date", ss.str()},
        {"tower_name", tower_name}
    };
    
    // Get singleton components from ECS
    flecs::world& world = ecs_world.GetWorld();
    
    // Serialize TimeManager
    if (world.has<TimeManager>()) {
        const auto& time_mgr = world.get<TimeManager>();
        json["time"] = {
            {"current_hour", time_mgr.current_hour},
            {"current_day", time_mgr.current_day},
            {"current_week", time_mgr.current_week},
            {"simulation_speed", time_mgr.simulation_speed},
            {"hours_per_second", time_mgr.hours_per_second}
        };
        
        // Add to metadata for UI display
        json["metadata"]["current_day"] = time_mgr.current_day;
        json["metadata"]["current_time"] = time_mgr.current_hour;
    }
    
    // Serialize TowerEconomy
    if (world.has<TowerEconomy>()) {
        const auto& economy = world.get<TowerEconomy>();
        json["economy"] = {
            {"total_balance", economy.total_balance},
            {"total_revenue", economy.total_revenue},
            {"total_expenses", economy.total_expenses},
            {"daily_revenue", economy.daily_revenue},
            {"daily_expenses", economy.daily_expenses},
            {"last_processed_day", economy.last_processed_day}
        };
        
        // Add to metadata for UI display
        json["metadata"]["total_balance"] = economy.total_balance;
    }
    
    // Serialize TowerGrid
    TowerGrid& grid = ecs_world.GetTowerGrid();
    json["grid"] = {
        {"floors", grid.GetFloorCount()},
        {"columns", grid.GetColumnCount()}
    };
    
    // Serialize entities and components
    nlohmann::json entities = nlohmann::json::array();
    int population = 0;
    
    // Serialize all entities with their components
    world.each([&](flecs::entity e) {
        nlohmann::json entity;
        entity["id"] = static_cast<uint64_t>(e.id());
        entity["name"] = e.name() ? e.name() : "";
        
        // Position component
        if (e.has<Position>()) {
            const auto& pos = e.get<Position>();
            entity["position"] = {{"x", pos.x}, {"y", pos.y}};
        }
        
        // Velocity component
        if (e.has<Velocity>()) {
            const auto& vel = e.get<Velocity>();
            entity["velocity"] = {{"dx", vel.dx}, {"dy", vel.dy}};
        }
        
        // Actor component
        if (e.has<Actor>()) {
            const auto& actor = e.get<Actor>();
            entity["actor"] = {
                {"name", actor.name},
                {"floor_destination", actor.floor_destination},
                {"speed", actor.speed}
            };
        }
        
        // Person component
        if (e.has<Person>()) {
            const auto& person = e.get<Person>();
            entity["person"] = {
                {"name", person.name},
                {"state", static_cast<int>(person.state)},
                {"current_floor", person.current_floor},
                {"current_column", person.current_column},
                {"destination_floor", person.destination_floor},
                {"destination_column", person.destination_column},
                {"move_speed", person.move_speed},
                {"wait_time", person.wait_time},
                {"current_need", person.current_need}
            };
            population++;
        }
        
        // BuildingComponent
        if (e.has<BuildingComponent>()) {
            const auto& building = e.get<BuildingComponent>();
            entity["building"] = {
                {"type", static_cast<int>(building.type)},
                {"floor", building.floor},
                {"width", building.width},
                {"capacity", building.capacity},
                {"current_occupancy", building.current_occupancy}
            };
        }
        
        // GridPosition component
        if (e.has<GridPosition>()) {
            const auto& grid_pos = e.get<GridPosition>();
            entity["grid_position"] = {
                {"floor", grid_pos.floor},
                {"column", grid_pos.column},
                {"width", grid_pos.width}
            };
        }
        
        // Satisfaction component
        if (e.has<Satisfaction>()) {
            const auto& sat = e.get<Satisfaction>();
            entity["satisfaction"] = {
                {"score", sat.satisfaction_score},
                {"wait_time_penalty", sat.wait_time_penalty},
                {"crowding_penalty", sat.crowding_penalty},
                {"noise_penalty", sat.noise_penalty},
                {"quality_bonus", sat.quality_bonus}
            };
        }
        
        // FacilityEconomics component
        if (e.has<FacilityEconomics>()) {
            const auto& econ = e.get<FacilityEconomics>();
            entity["facility_economics"] = {
                {"base_rent", econ.base_rent},
                {"revenue", econ.revenue},
                {"operating_cost", econ.operating_cost},
                {"quality_multiplier", econ.quality_multiplier},
                {"max_tenants", econ.max_tenants},
                {"current_tenants", econ.current_tenants}
            };
        }
        
        // ElevatorShaft component
        if (e.has<ElevatorShaft>()) {
            const auto& shaft = e.get<ElevatorShaft>();
            entity["elevator_shaft"] = {
                {"column", shaft.column},
                {"bottom_floor", shaft.bottom_floor},
                {"top_floor", shaft.top_floor},
                {"car_count", shaft.car_count}
            };
        }
        
        // ElevatorCar component
        if (e.has<ElevatorCar>()) {
            const auto& car = e.get<ElevatorCar>();
            entity["elevator_car"] = {
                {"shaft_entity_id", car.shaft_entity_id},
                {"current_floor", car.current_floor},
                {"target_floor", car.target_floor},
                {"state", static_cast<int>(car.state)},
                {"max_capacity", car.max_capacity},
                {"current_occupancy", car.current_occupancy},
                {"stop_queue", car.stop_queue},
                {"passenger_destinations", car.passenger_destinations},
                {"state_timer", car.state_timer},
                {"door_open_duration", car.door_open_duration},
                {"door_transition_duration", car.door_transition_duration},
                {"floors_per_second", car.floors_per_second}
            };
        }
        
        // PersonElevatorRequest component
        if (e.has<PersonElevatorRequest>()) {
            const auto& req = e.get<PersonElevatorRequest>();
            entity["elevator_request"] = {
                {"shaft_entity_id", req.shaft_entity_id},
                {"car_entity_id", req.car_entity_id},
                {"call_floor", req.call_floor},
                {"destination_floor", req.destination_floor},
                {"wait_time", req.wait_time},
                {"is_boarding", req.is_boarding}
            };
        }
        
        // DailySchedule component
        if (e.has<DailySchedule>()) {
            const auto& schedule = e.get<DailySchedule>();
            nlohmann::json weekday_actions = nlohmann::json::array();
            for (const auto& action : schedule.weekday_schedule) {
                weekday_actions.push_back({
                    {"type", static_cast<int>(action.type)},
                    {"trigger_hour", action.trigger_hour}
                });
            }
            
            nlohmann::json weekend_actions = nlohmann::json::array();
            for (const auto& action : schedule.weekend_schedule) {
                weekend_actions.push_back({
                    {"type", static_cast<int>(action.type)},
                    {"trigger_hour", action.trigger_hour}
                });
            }
            
            entity["schedule"] = {
                {"weekday_schedule", weekday_actions},
                {"weekend_schedule", weekend_actions},
                {"last_triggered_hour", schedule.last_triggered_hour}
            };
        }
        
        // Only add entity if it has components
        if (entity.size() > 2) {  // More than just id and name
            entities.push_back(entity);
        }
    });
    
    json["entities"] = entities;
    json["metadata"]["population"] = population;
    
    // Serialize achievements if achievement manager is set
    if (achievement_manager_) {
        json["achievements"] = achievement_manager_->Serialize();
    }
    
    return json;
}

bool SaveLoadManager::DeserializeGameState(const nlohmann::json& json,
                                           ECSWorld& ecs_world) {
    try {
        flecs::world& world = ecs_world.GetWorld();
        
        // Clear existing entities (except singletons)
        world.defer_begin();
        
        world.each([](flecs::entity e) {
            // Skip singleton components
            if (!e.has<TimeManager>() && !e.has<TowerEconomy>()) {
                e.destruct();
            }
        });
        
        world.defer_end();
        
        // Deserialize TimeManager
        if (json.contains("time")) {
            auto& time_json = json["time"];
            TimeManager time_mgr;
            time_mgr.current_hour = time_json.value("current_hour", 8.0f);
            time_mgr.current_day = time_json.value("current_day", 0);
            time_mgr.current_week = time_json.value("current_week", 1);
            time_mgr.simulation_speed = time_json.value("simulation_speed", 1.0f);
            time_mgr.hours_per_second = time_json.value("hours_per_second", 1.0f);
            world.set<TimeManager>(time_mgr);
        }
        
        // Deserialize TowerEconomy
        if (json.contains("economy")) {
            auto& econ_json = json["economy"];
            TowerEconomy economy;
            economy.total_balance = econ_json.value("total_balance", 10000.0f);
            economy.total_revenue = econ_json.value("total_revenue", 0.0f);
            economy.total_expenses = econ_json.value("total_expenses", 0.0f);
            economy.daily_revenue = econ_json.value("daily_revenue", 0.0f);
            economy.daily_expenses = econ_json.value("daily_expenses", 0.0f);
            economy.last_processed_day = econ_json.value("last_processed_day", -1);
            world.set<TowerEconomy>(economy);
        }
        
        // Deserialize TowerGrid dimensions
        if (json.contains("grid")) {
            auto& grid_json = json["grid"];
            int floors = grid_json.value("floors", 10);
            int columns = grid_json.value("columns", 20);
            
            TowerGrid& grid = ecs_world.GetTowerGrid();
            // Resize grid to match saved state
            while (grid.GetFloorCount() < floors) {
                grid.AddFloor();
            }
            while (grid.GetColumnCount() < columns) {
                grid.AddColumn();
            }
        }
        
        // Deserialize entities
        if (json.contains("entities")) {
            for (const auto& entity_json : json["entities"]) {
                std::string name = entity_json.value("name", "");
                flecs::entity e = name.empty() ? world.entity() : world.entity(name.c_str());
                
                // Position
                if (entity_json.contains("position")) {
                    auto& pos_json = entity_json["position"];
                    e.set<Position>({
                        pos_json.value("x", 0.0f),
                        pos_json.value("y", 0.0f)
                    });
                }
                
                // Velocity
                if (entity_json.contains("velocity")) {
                    auto& vel_json = entity_json["velocity"];
                    e.set<Velocity>({
                        vel_json.value("dx", 0.0f),
                        vel_json.value("dy", 0.0f)
                    });
                }
                
                // Actor
                if (entity_json.contains("actor")) {
                    auto& actor_json = entity_json["actor"];
                    Actor actor;
                    actor.name = actor_json.value("name", "Actor");
                    actor.floor_destination = actor_json.value("floor_destination", 0);
                    actor.speed = actor_json.value("speed", 1.0f);
                    e.set<Actor>(actor);
                }
                
                // Person
                if (entity_json.contains("person")) {
                    auto& person_json = entity_json["person"];
                    Person person;
                    person.name = person_json.value("name", "Person");
                    person.state = static_cast<PersonState>(person_json.value("state", 0));
                    person.current_floor = person_json.value("current_floor", 0);
                    person.current_column = person_json.value("current_column", 0.0f);
                    person.destination_floor = person_json.value("destination_floor", 0);
                    person.destination_column = person_json.value("destination_column", 0.0f);
                    person.move_speed = person_json.value("move_speed", 2.0f);
                    person.wait_time = person_json.value("wait_time", 0.0f);
                    person.current_need = person_json.value("current_need", "Idle");
                    e.set<Person>(person);
                }
                
                // BuildingComponent
                if (entity_json.contains("building")) {
                    auto& building_json = entity_json["building"];
                    BuildingComponent building;
                    building.type = static_cast<BuildingComponent::Type>(building_json.value("type", 0));
                    building.floor = building_json.value("floor", 0);
                    building.width = building_json.value("width", 1);
                    building.capacity = building_json.value("capacity", 10);
                    building.current_occupancy = building_json.value("current_occupancy", 0);
                    e.set<BuildingComponent>(building);
                }
                
                // GridPosition
                if (entity_json.contains("grid_position")) {
                    auto& grid_pos_json = entity_json["grid_position"];
                    e.set<GridPosition>({
                        grid_pos_json.value("floor", 0),
                        grid_pos_json.value("column", 0),
                        grid_pos_json.value("width", 1)
                    });
                }
                
                // Satisfaction
                if (entity_json.contains("satisfaction")) {
                    auto& sat_json = entity_json["satisfaction"];
                    Satisfaction sat;
                    sat.satisfaction_score = sat_json.value("score", 75.0f);
                    sat.wait_time_penalty = sat_json.value("wait_time_penalty", 0.0f);
                    sat.crowding_penalty = sat_json.value("crowding_penalty", 0.0f);
                    sat.noise_penalty = sat_json.value("noise_penalty", 0.0f);
                    sat.quality_bonus = sat_json.value("quality_bonus", 0.0f);
                    e.set<Satisfaction>(sat);
                }
                
                // FacilityEconomics
                if (entity_json.contains("facility_economics")) {
                    auto& econ_json = entity_json["facility_economics"];
                    FacilityEconomics econ;
                    econ.base_rent = econ_json.value("base_rent", 100.0f);
                    econ.revenue = econ_json.value("revenue", 0.0f);
                    econ.operating_cost = econ_json.value("operating_cost", 20.0f);
                    econ.quality_multiplier = econ_json.value("quality_multiplier", 1.0f);
                    econ.max_tenants = econ_json.value("max_tenants", 10);
                    econ.current_tenants = econ_json.value("current_tenants", 0);
                    e.set<FacilityEconomics>(econ);
                }
                
                // ElevatorShaft
                if (entity_json.contains("elevator_shaft")) {
                    auto& shaft_json = entity_json["elevator_shaft"];
                    e.set<ElevatorShaft>({
                        shaft_json.value("column", 0),
                        shaft_json.value("bottom_floor", 0),
                        shaft_json.value("top_floor", 0),
                        shaft_json.value("car_count", 1)
                    });
                }
                
                // ElevatorCar
                if (entity_json.contains("elevator_car")) {
                    auto& car_json = entity_json["elevator_car"];
                    ElevatorCar car;
                    car.shaft_entity_id = car_json.value("shaft_entity_id", -1);
                    car.current_floor = car_json.value("current_floor", 0.0f);
                    car.target_floor = car_json.value("target_floor", 0);
                    car.state = static_cast<ElevatorState>(car_json.value("state", 0));
                    car.max_capacity = car_json.value("max_capacity", 8);
                    car.current_occupancy = car_json.value("current_occupancy", 0);
                    car.state_timer = car_json.value("state_timer", 0.0f);
                    car.door_open_duration = car_json.value("door_open_duration", 2.0f);
                    car.door_transition_duration = car_json.value("door_transition_duration", 1.0f);
                    car.floors_per_second = car_json.value("floors_per_second", 2.0f);
                    
                    if (car_json.contains("stop_queue")) {
                        car.stop_queue = car_json["stop_queue"].get<std::vector<int>>();
                    }
                    if (car_json.contains("passenger_destinations")) {
                        car.passenger_destinations = car_json["passenger_destinations"].get<std::vector<int>>();
                    }
                    
                    e.set<ElevatorCar>(car);
                }
                
                // PersonElevatorRequest
                if (entity_json.contains("elevator_request")) {
                    auto& req_json = entity_json["elevator_request"];
                    e.set<PersonElevatorRequest>({
                        req_json.value("shaft_entity_id", -1),
                        req_json.value("call_floor", 0),
                        req_json.value("destination_floor", 0)
                    });
                    
                    // Note: car_entity_id, wait_time, is_boarding will be reset
                }
                
                // DailySchedule
                if (entity_json.contains("schedule")) {
                    auto& schedule_json = entity_json["schedule"];
                    DailySchedule schedule;
                    
                    if (schedule_json.contains("weekday_schedule")) {
                        for (const auto& action_json : schedule_json["weekday_schedule"]) {
                            ScheduledAction action;
                            action.type = static_cast<ScheduledAction::Type>(action_json.value("type", 0));
                            action.trigger_hour = action_json.value("trigger_hour", 9.0f);
                            schedule.weekday_schedule.push_back(action);
                        }
                    }
                    
                    if (schedule_json.contains("weekend_schedule")) {
                        for (const auto& action_json : schedule_json["weekend_schedule"]) {
                            ScheduledAction action;
                            action.type = static_cast<ScheduledAction::Type>(action_json.value("type", 0));
                            action.trigger_hour = action_json.value("trigger_hour", 9.0f);
                            schedule.weekend_schedule.push_back(action);
                        }
                    }
                    
                    schedule.last_triggered_hour = schedule_json.value("last_triggered_hour", -1.0f);
                    e.set<DailySchedule>(schedule);
                }
            }
        }
        
        // Deserialize achievements if present
        if (json.contains("achievements") && achievement_manager_) {
            achievement_manager_->Deserialize(json["achievements"]);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Deserialization error: " << e.what() << std::endl;
        return false;
    }
}

bool SaveLoadManager::ValidateVersion(const nlohmann::json& json) const {
    if (!json.contains("metadata") || !json["metadata"].contains("game_version")) {
        return false;
    }
    
    std::string file_version = json["metadata"]["game_version"];
    
    // For now, we accept any 0.1.x version
    // In the future, implement more sophisticated version checking
    if (file_version.substr(0, 3) == "0.1") {
        return true;
    }
    
    return file_version == GAME_VERSION;
}

SaveLoadResult SaveLoadManager::CheckDiskSpace(const std::string& path) const {
    try {
#ifdef _WIN32
        ULARGE_INTEGER free_bytes;
        if (GetDiskFreeSpaceExA(path.c_str(), &free_bytes, nullptr, nullptr)) {
            // Check if we have at least 10MB free
            if (free_bytes.QuadPart < 10 * 1024 * 1024) {
                return SaveLoadResult::Failure(SaveLoadError::DiskFull,
                                              "Insufficient disk space");
            }
        }
#else
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat) == 0) {
            unsigned long long free_bytes = stat.f_bavail * stat.f_frsize;
            // Check if we have at least 10MB free
            if (free_bytes < 10 * 1024 * 1024) {
                return SaveLoadResult::Failure(SaveLoadError::DiskFull,
                                              "Insufficient disk space");
            }
        }
#endif
        return SaveLoadResult::Success();
    } catch (const std::exception& e) {
        // If we can't check, assume we have space
        return SaveLoadResult::Success();
    }
}

} // namespace Core
} // namespace TowerForge
