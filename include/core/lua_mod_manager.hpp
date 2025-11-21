#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

// Forward declare lua_State to avoid including lua headers in header
struct lua_State;

namespace towerforge::core {

    // Forward declarations
    class ECSWorld;
    struct BuildingComponent;

    /**
 * @brief Metadata about a loaded mod
 */
    struct ModInfo {
        std::string id;             // Unique identifier
        std::string name;           // Display name
        std::string version;        // Version string
        std::string author;         // Author name
        std::string description;    // Description
        std::string filepath;       // Path to the mod file
        bool enabled;               // Whether the mod is currently active
        bool loaded_successfully;   // Whether the mod loaded without errors
        std::string error_message;  // Error message if loading failed
    
        ModInfo() : enabled(true), loaded_successfully(false) {}
    };

    /**
 * @brief Custom facility type definition from Lua
 */
    struct LuaFacilityType {
        std::string id;                 // Unique identifier
        std::string name;               // Display name
        int default_width;              // Default width in grid cells
        int default_capacity;           // Default maximum occupancy
        int required_staff;             // Number of staff needed
        float operating_start_hour;     // Start of operating hours
        float operating_end_hour;       // End of operating hours
        float base_rent;                // Base rent per tenant per day
        float operating_cost;           // Daily operating cost
        unsigned int color;             // Rendering color (RGBA)
    
        LuaFacilityType()
            : default_width(8),
              default_capacity(20),
              required_staff(0),
              operating_start_hour(9.0f),
              operating_end_hour(17.0f),
              base_rent(100.0f),
              operating_cost(20.0f),
              color(0xFFFFFFFF) {}
    };

    /**
 * @brief Custom visitor type definition from Lua
 */
    struct LuaVisitorType {
        std::string id;                 // Unique identifier
        std::string name;               // Display name
        float move_speed;               // Movement speed (columns per second)
        float min_visit_duration;       // Minimum visit time (seconds)
        float max_visit_duration;       // Maximum visit time (seconds)
        std::vector<std::string> preferred_facilities;  // Facility types they prefer to visit
    
        LuaVisitorType()
            : move_speed(2.0f),
              min_visit_duration(60.0f),
              max_visit_duration(300.0f) {}
    };

    /**
 * @brief Custom staff role definition from Lua
 */
    struct LuaStaffRole {
        std::string id;                 // Unique identifier
        std::string name;               // Display name
        float work_efficiency;          // Work speed multiplier (default 1.0)
        float default_wage;             // Daily wage (default $50)
        float shift_start_hour;         // Default shift start (default 9.0)
        float shift_end_hour;           // Default shift end (default 17.0)
        std::string work_type;          // "cleaning", "maintenance", "emergency", or "custom"
        
        // Custom work behavior (Lua function name to call)
        std::string work_function;      // Name of Lua function to call for work
    
        LuaStaffRole()
            : work_efficiency(1.0f),
              default_wage(50.0f),
              shift_start_hour(9.0f),
              shift_end_hour(17.0f),
              work_type("custom") {}
    };

    /**
 * @brief Custom event type definition from Lua
 */
    struct LuaEventType {
        std::string id;                 // Unique identifier
        std::string name;               // Display name
        std::string description;        // Event description
        float spawn_chance;             // Chance per hour per facility (0.0-1.0)
        float duration;                 // How long event lasts (seconds, 0 = instant)
        bool requires_staff_response;   // Whether staff need to respond
        std::string required_staff_type;// Staff type that can resolve it
        float resolution_time;          // Time for staff to resolve (seconds)
        
        // Effects
        float satisfaction_penalty;     // Satisfaction reduction while active
        float maintenance_damage;       // Maintenance damage when resolved
        
        // Lua callbacks (function names)
        std::string on_spawn_function;  // Called when event spawns
        std::string on_resolve_function;// Called when event is resolved
    
        LuaEventType()
            : spawn_chance(0.001f),
              duration(300.0f),
              requires_staff_response(true),
              resolution_time(10.0f),
              satisfaction_penalty(5.0f),
              maintenance_damage(10.0f) {}
    };

    /**
 * @brief Manages loading and execution of Lua mods
 * 
 * The LuaModManager handles:
 * - Loading Lua scripts from the mods/ directory
 * - Registering custom facilities and visitor types
 * - Sandboxing Lua scripts for security
 * - Error handling for faulty scripts
 * - Enabling/disabling mods at runtime
 */
    class LuaModManager {
    public:
        LuaModManager();
        ~LuaModManager();
    
        // Prevent copying
        LuaModManager(const LuaModManager&) = delete;
        LuaModManager& operator=(const LuaModManager&) = delete;
    
        /**
     * @brief Initialize the Lua environment and set up API bindings
     * 
     * @param ecs_world Pointer to the ECS world for integration
     * @return true if initialization succeeded
     */
        bool Initialize(ECSWorld* ecs_world);
    
        /**
     * @brief Load all mods from the specified directory
     * 
     * @param mods_directory Path to the mods directory (default: "mods/")
     * @return Number of mods successfully loaded
     */
        int LoadMods(const std::string& mods_directory = "mods/");
    
        /**
     * @brief Load a single mod from a file
     * 
     * @param filepath Path to the Lua mod file
     * @return true if the mod loaded successfully
     */
        bool LoadMod(const std::string& filepath);
    
        /**
     * @brief Enable a mod by ID
     * 
     * @param mod_id The mod's unique identifier
     * @return true if the mod was enabled
     */
        bool EnableMod(const std::string& mod_id);
    
        /**
     * @brief Disable a mod by ID
     * 
     * @param mod_id The mod's unique identifier
     * @return true if the mod was disabled
     */
        bool DisableMod(const std::string& mod_id);
    
        /**
     * @brief Get a list of all loaded mods
     * 
     * @return Vector of mod metadata
     */
        const std::vector<ModInfo>& GetLoadedMods() const { return loaded_mods_; }
    
        /**
     * @brief Get a registered custom facility type
     * 
     * @param id The facility type ID
     * @return Pointer to the facility type, or nullptr if not found
     */
        const LuaFacilityType* GetCustomFacilityType(const std::string& id) const;
    
        /**
     * @brief Get all registered custom facility types
     * 
     * @return Map of facility type IDs to facility type data
     */
        const std::unordered_map<std::string, LuaFacilityType>& GetCustomFacilityTypes() const {
            return custom_facility_types_;
        }
    
        /**
     * @brief Get a registered custom visitor type
     * 
     * @param id The visitor type ID
     * @return Pointer to the visitor type, or nullptr if not found
     */
        const LuaVisitorType* GetCustomVisitorType(const std::string& id) const;
    
        /**
     * @brief Get all registered custom visitor types
     * 
     * @return Map of visitor type IDs to visitor type data
     */
        const std::unordered_map<std::string, LuaVisitorType>& GetCustomVisitorTypes() const {
            return custom_visitor_types_;
        }
    
        /**
     * @brief Get a registered custom staff role
     * 
     * @param id The staff role ID
     * @return Pointer to the staff role, or nullptr if not found
     */
        const LuaStaffRole* GetCustomStaffRole(const std::string& id) const;
    
        /**
     * @brief Get all registered custom staff roles
     * 
     * @return Map of staff role IDs to staff role data
     */
        const std::unordered_map<std::string, LuaStaffRole>& GetCustomStaffRoles() const {
            return custom_staff_roles_;
        }
    
        /**
     * @brief Get a registered custom event type
     * 
     * @param id The event type ID
     * @return Pointer to the event type, or nullptr if not found
     */
        const LuaEventType* GetCustomEventType(const std::string& id) const;
    
        /**
     * @brief Get all registered custom event types
     * 
     * @return Map of event type IDs to event type data
     */
        const std::unordered_map<std::string, LuaEventType>& GetCustomEventTypes() const {
            return custom_event_types_;
        }
    
        /**
     * @brief Shutdown the Lua environment
     */
        void Shutdown();
    
    private:
        /**
     * @brief Set up the sandboxed Lua environment
     */
        void SetupSandbox() const;
    
        /**
     * @brief Register C++ API functions that Lua can call
     */
        void RegisterAPI() const;
    
        /**
     * @brief Register a custom facility type from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_RegisterFacilityType(lua_State* L);
    
        /**
     * @brief Register a custom visitor type from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_RegisterVisitorType(lua_State* L);
    
        /**
     * @brief Register a research node from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_RegisterResearchNode(lua_State* L);
    
        /**
     * @brief Register a custom staff role from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_RegisterStaffRole(lua_State* L);
    
        /**
     * @brief Register a custom event type from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_RegisterEventType(lua_State* L);
    
        /**
     * @brief Log a message from Lua
     * 
     * Called by Lua scripts via the API
     */
        static int Lua_Log(lua_State* L);
    
        /**
     * @brief Get the LuaModManager instance from a lua_State
     */
        static LuaModManager* GetManager(lua_State* L);
    
        lua_State* lua_state_;
        ECSWorld* ecs_world_;
    
        std::vector<ModInfo> loaded_mods_;
        std::unordered_map<std::string, LuaFacilityType> custom_facility_types_;
        std::unordered_map<std::string, LuaVisitorType> custom_visitor_types_;
        std::unordered_map<std::string, LuaStaffRole> custom_staff_roles_;
        std::unordered_map<std::string, LuaEventType> custom_event_types_;
    };

}
