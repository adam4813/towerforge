#include "core/lua_mod_manager.hpp"
#include "core/ecs_world.hpp"
#include "core/components.hpp"
#include <lua.hpp>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace TowerForge {
namespace Core {

// Key for storing the manager pointer in Lua registry
static const char* MANAGER_REGISTRY_KEY = "TowerForge_ModManager";

LuaModManager::LuaModManager()
    : lua_state_(nullptr), ecs_world_(nullptr) {
}

LuaModManager::~LuaModManager() {
    Shutdown();
}

bool LuaModManager::Initialize(ECSWorld* ecs_world) {
    if (!ecs_world) {
        std::cerr << "LuaModManager: Cannot initialize with null ECS world" << std::endl;
        return false;
    }
    
    ecs_world_ = ecs_world;
    
    // Create new Lua state
    lua_state_ = luaL_newstate();
    if (!lua_state_) {
        std::cerr << "LuaModManager: Failed to create Lua state" << std::endl;
        return false;
    }
    
    // Open standard libraries (but we'll sandbox them)
    luaL_openlibs(lua_state_);
    
    // Store the manager pointer in the Lua registry
    lua_pushlightuserdata(lua_state_, this);
    lua_setfield(lua_state_, LUA_REGISTRYINDEX, MANAGER_REGISTRY_KEY);
    
    // Set up sandboxing
    SetupSandbox();
    
    // Register API functions
    RegisterAPI();
    
    std::cout << "LuaModManager: Initialized successfully" << std::endl;
    return true;
}

void LuaModManager::SetupSandbox() {
    if (!lua_state_) return;
    
    // Remove dangerous functions from the global environment
    // This prevents mods from accessing the file system or executing system commands
    const char* dangerous_globals[] = {
        "dofile",
        "loadfile",
        "require",  // We'll provide a safer version if needed
        nullptr
    };
    
    for (int i = 0; dangerous_globals[i] != nullptr; ++i) {
        lua_pushnil(lua_state_);
        lua_setglobal(lua_state_, dangerous_globals[i]);
    }
    
    // Restrict the 'os' library
    lua_getglobal(lua_state_, "os");
    if (lua_istable(lua_state_, -1)) {
        const char* dangerous_os[] = {
            "execute",
            "exit",
            "remove",
            "rename",
            "tmpname",
            "getenv",
            "setlocale",
            nullptr
        };
        
        for (int i = 0; dangerous_os[i] != nullptr; ++i) {
            lua_pushnil(lua_state_);
            lua_setfield(lua_state_, -2, dangerous_os[i]);
        }
    }
    lua_pop(lua_state_, 1);
    
    // Restrict the 'io' library (completely remove it for safety)
    lua_pushnil(lua_state_);
    lua_setglobal(lua_state_, "io");
    
    // Restrict the 'package' library
    lua_pushnil(lua_state_);
    lua_setglobal(lua_state_, "package");
    
    std::cout << "LuaModManager: Sandbox configured" << std::endl;
}

void LuaModManager::RegisterAPI() {
    if (!lua_state_) return;
    
    // Create the TowerForge API table
    lua_newtable(lua_state_);
    
    // Register functions
    lua_pushcfunction(lua_state_, Lua_RegisterFacilityType);
    lua_setfield(lua_state_, -2, "RegisterFacilityType");
    
    lua_pushcfunction(lua_state_, Lua_RegisterVisitorType);
    lua_setfield(lua_state_, -2, "RegisterVisitorType");
    
    lua_pushcfunction(lua_state_, Lua_RegisterResearchNode);
    lua_setfield(lua_state_, -2, "RegisterResearchNode");
    
    lua_pushcfunction(lua_state_, Lua_Log);
    lua_setfield(lua_state_, -2, "Log");
    
    // Set the table as a global called "TowerForge"
    lua_setglobal(lua_state_, "TowerForge");
    
    std::cout << "LuaModManager: API registered" << std::endl;
}

int LuaModManager::LoadMods(const std::string& mods_directory) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(mods_directory)) {
        std::cout << "LuaModManager: Mods directory '" << mods_directory 
                  << "' does not exist, creating it..." << std::endl;
        fs::create_directories(mods_directory);
        return 0;
    }
    
    int loaded_count = 0;
    
    // Iterate through all .lua files in the mods directory
    for (const auto& entry : fs::directory_iterator(mods_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            if (LoadMod(entry.path().string())) {
                loaded_count++;
            }
        }
    }
    
    std::cout << "LuaModManager: Loaded " << loaded_count << " mods from " 
              << mods_directory << std::endl;
    
    return loaded_count;
}

bool LuaModManager::LoadMod(const std::string& filepath) {
    if (!lua_state_) {
        std::cerr << "LuaModManager: Lua state not initialized" << std::endl;
        return false;
    }
    
    ModInfo mod_info;
    mod_info.filepath = filepath;
    
    // Extract filename for default ID
    namespace fs = std::filesystem;
    fs::path path(filepath);
    std::string filename = path.stem().string();
    mod_info.id = filename;
    mod_info.name = filename;  // Default, can be overridden by mod
    
    std::cout << "LuaModManager: Loading mod from " << filepath << std::endl;
    
    // Load and execute the Lua file
    int result = luaL_dofile(lua_state_, filepath.c_str());
    
    if (result != LUA_OK) {
        // Get error message
        const char* error_msg = lua_tostring(lua_state_, -1);
        mod_info.error_message = error_msg ? error_msg : "Unknown error";
        mod_info.loaded_successfully = false;
        
        std::cerr << "LuaModManager: Error loading mod '" << filepath << "': " 
                  << mod_info.error_message << std::endl;
        
        lua_pop(lua_state_, 1);  // Pop error message
        loaded_mods_.push_back(mod_info);
        return false;
    }
    
    // Try to get mod metadata from global ModInfo table
    lua_getglobal(lua_state_, "ModInfo");
    if (lua_istable(lua_state_, -1)) {
        // Get id
        lua_getfield(lua_state_, -1, "id");
        if (lua_isstring(lua_state_, -1)) {
            mod_info.id = lua_tostring(lua_state_, -1);
        }
        lua_pop(lua_state_, 1);
        
        // Get name
        lua_getfield(lua_state_, -1, "name");
        if (lua_isstring(lua_state_, -1)) {
            mod_info.name = lua_tostring(lua_state_, -1);
        }
        lua_pop(lua_state_, 1);
        
        // Get version
        lua_getfield(lua_state_, -1, "version");
        if (lua_isstring(lua_state_, -1)) {
            mod_info.version = lua_tostring(lua_state_, -1);
        }
        lua_pop(lua_state_, 1);
        
        // Get author
        lua_getfield(lua_state_, -1, "author");
        if (lua_isstring(lua_state_, -1)) {
            mod_info.author = lua_tostring(lua_state_, -1);
        }
        lua_pop(lua_state_, 1);
        
        // Get description
        lua_getfield(lua_state_, -1, "description");
        if (lua_isstring(lua_state_, -1)) {
            mod_info.description = lua_tostring(lua_state_, -1);
        }
        lua_pop(lua_state_, 1);
    }
    lua_pop(lua_state_, 1);  // Pop ModInfo table
    
    mod_info.loaded_successfully = true;
    mod_info.enabled = true;
    
    loaded_mods_.push_back(mod_info);
    
    std::cout << "LuaModManager: Successfully loaded mod '" << mod_info.name 
              << "' (ID: " << mod_info.id << ")" << std::endl;
    
    return true;
}

bool LuaModManager::EnableMod(const std::string& mod_id) {
    for (auto& mod : loaded_mods_) {
        if (mod.id == mod_id) {
            mod.enabled = true;
            std::cout << "LuaModManager: Enabled mod '" << mod.name << "'" << std::endl;
            return true;
        }
    }
    return false;
}

bool LuaModManager::DisableMod(const std::string& mod_id) {
    for (auto& mod : loaded_mods_) {
        if (mod.id == mod_id) {
            mod.enabled = false;
            std::cout << "LuaModManager: Disabled mod '" << mod.name << "'" << std::endl;
            return true;
        }
    }
    return false;
}

const LuaFacilityType* LuaModManager::GetCustomFacilityType(const std::string& id) const {
    auto it = custom_facility_types_.find(id);
    if (it != custom_facility_types_.end()) {
        return &it->second;
    }
    return nullptr;
}

const LuaVisitorType* LuaModManager::GetCustomVisitorType(const std::string& id) const {
    auto it = custom_visitor_types_.find(id);
    if (it != custom_visitor_types_.end()) {
        return &it->second;
    }
    return nullptr;
}

void LuaModManager::Shutdown() {
    if (lua_state_) {
        lua_close(lua_state_);
        lua_state_ = nullptr;
    }
    
    loaded_mods_.clear();
    custom_facility_types_.clear();
    custom_visitor_types_.clear();
    
    std::cout << "LuaModManager: Shutdown complete" << std::endl;
}

LuaModManager* LuaModManager::GetManager(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, MANAGER_REGISTRY_KEY);
    LuaModManager* manager = static_cast<LuaModManager*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return manager;
}

int LuaModManager::Lua_RegisterFacilityType(lua_State* L) {
    LuaModManager* manager = GetManager(L);
    if (!manager) {
        luaL_error(L, "Failed to get mod manager");
        return 0;
    }
    
    // Expect a table as the first argument
    if (!lua_istable(L, 1)) {
        luaL_error(L, "RegisterFacilityType expects a table as argument");
        return 0;
    }
    
    LuaFacilityType facility;
    
    // Get id (required)
    lua_getfield(L, 1, "id");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Facility type must have an 'id' field (string)");
        return 0;
    }
    facility.id = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get name (required)
    lua_getfield(L, 1, "name");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Facility type must have a 'name' field (string)");
        return 0;
    }
    facility.name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get optional fields
    lua_getfield(L, 1, "default_width");
    if (lua_isnumber(L, -1)) {
        facility.default_width = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "default_capacity");
    if (lua_isnumber(L, -1)) {
        facility.default_capacity = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "required_staff");
    if (lua_isnumber(L, -1)) {
        facility.required_staff = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "operating_start_hour");
    if (lua_isnumber(L, -1)) {
        facility.operating_start_hour = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "operating_end_hour");
    if (lua_isnumber(L, -1)) {
        facility.operating_end_hour = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "base_rent");
    if (lua_isnumber(L, -1)) {
        facility.base_rent = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "operating_cost");
    if (lua_isnumber(L, -1)) {
        facility.operating_cost = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "color");
    if (lua_isnumber(L, -1)) {
        facility.color = static_cast<unsigned int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    // Register the facility type
    manager->custom_facility_types_[facility.id] = facility;
    
    std::cout << "LuaModManager: Registered custom facility type '" 
              << facility.name << "' (ID: " << facility.id << ")" << std::endl;
    
    return 0;
}

int LuaModManager::Lua_RegisterVisitorType(lua_State* L) {
    LuaModManager* manager = GetManager(L);
    if (!manager) {
        luaL_error(L, "Failed to get mod manager");
        return 0;
    }
    
    // Expect a table as the first argument
    if (!lua_istable(L, 1)) {
        luaL_error(L, "RegisterVisitorType expects a table as argument");
        return 0;
    }
    
    LuaVisitorType visitor;
    
    // Get id (required)
    lua_getfield(L, 1, "id");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Visitor type must have an 'id' field (string)");
        return 0;
    }
    visitor.id = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get name (required)
    lua_getfield(L, 1, "name");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Visitor type must have a 'name' field (string)");
        return 0;
    }
    visitor.name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get optional fields
    lua_getfield(L, 1, "move_speed");
    if (lua_isnumber(L, -1)) {
        visitor.move_speed = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "min_visit_duration");
    if (lua_isnumber(L, -1)) {
        visitor.min_visit_duration = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "max_visit_duration");
    if (lua_isnumber(L, -1)) {
        visitor.max_visit_duration = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    // Get preferred_facilities array
    lua_getfield(L, 1, "preferred_facilities");
    if (lua_istable(L, -1)) {
        int len = lua_rawlen(L, -1);
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (lua_isstring(L, -1)) {
                visitor.preferred_facilities.push_back(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    
    // Register the visitor type
    manager->custom_visitor_types_[visitor.id] = visitor;
    
    std::cout << "LuaModManager: Registered custom visitor type '" 
              << visitor.name << "' (ID: " << visitor.id << ")" << std::endl;
    
    return 0;
}

int LuaModManager::Lua_RegisterResearchNode(lua_State* L) {
    LuaModManager* manager = GetManager(L);
    if (!manager) {
        luaL_error(L, "Failed to get mod manager");
        return 0;
    }
    
    if (!manager->ecs_world_) {
        luaL_error(L, "ECS world not available");
        return 0;
    }
    
    // Expect a table as the first argument
    if (!lua_istable(L, 1)) {
        luaL_error(L, "RegisterResearchNode expects a table as argument");
        return 0;
    }
    
    // Get the research tree from the ECS world
    auto world = manager->ecs_world_->GetWorld();
    if (!world.has<ResearchTree>()) {
        luaL_error(L, "Research tree not initialized");
        return 0;
    }
    
    auto research_tree = world.get_mut<ResearchTree>();
    ResearchNode node;
    
    // Get id (required)
    lua_getfield(L, 1, "id");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Research node must have an 'id' field (string)");
        return 0;
    }
    node.id = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get name (required)
    lua_getfield(L, 1, "name");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Research node must have a 'name' field (string)");
        return 0;
    }
    node.name = lua_tostring(L, -1);
    lua_pop(L, 1);
    
    // Get description
    lua_getfield(L, 1, "description");
    if (lua_isstring(L, -1)) {
        node.description = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    // Get icon
    lua_getfield(L, 1, "icon");
    if (lua_isstring(L, -1)) {
        node.icon = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    // Get node type (required) - must be a string like "FacilityUnlock", "IncomeBonus", etc.
    lua_getfield(L, 1, "type");
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "Research node must have a 'type' field (string)");
        return 0;
    }
    std::string type_str = lua_tostring(L, -1);
    if (type_str == "FacilityUnlock") {
        node.type = ResearchNodeType::FacilityUnlock;
    } else if (type_str == "ElevatorSpeed") {
        node.type = ResearchNodeType::ElevatorSpeed;
    } else if (type_str == "ElevatorCapacity") {
        node.type = ResearchNodeType::ElevatorCapacity;
    } else if (type_str == "IncomeBonus") {
        node.type = ResearchNodeType::IncomeBonus;
    } else if (type_str == "SatisfactionBonus") {
        node.type = ResearchNodeType::SatisfactionBonus;
    } else if (type_str == "ConstructionSpeed") {
        node.type = ResearchNodeType::ConstructionSpeed;
    } else if (type_str == "CostReduction") {
        node.type = ResearchNodeType::CostReduction;
    } else {
        luaL_error(L, "Unknown research node type: %s", type_str.c_str());
        return 0;
    }
    lua_pop(L, 1);
    
    // Get cost (tower points required)
    lua_getfield(L, 1, "cost");
    if (lua_isnumber(L, -1)) {
        node.cost = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    // Get grid position
    lua_getfield(L, 1, "grid_row");
    if (lua_isnumber(L, -1)) {
        node.grid_row = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "grid_column");
    if (lua_isnumber(L, -1)) {
        node.grid_column = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    // Get effect value
    lua_getfield(L, 1, "effect_value");
    if (lua_isnumber(L, -1)) {
        node.effect_value = static_cast<float>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    
    // Get effect target
    lua_getfield(L, 1, "effect_target");
    if (lua_isstring(L, -1)) {
        node.effect_target = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    // Get prerequisites array
    lua_getfield(L, 1, "prerequisites");
    if (lua_istable(L, -1)) {
        int len = lua_rawlen(L, -1);
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (lua_isstring(L, -1)) {
                node.prerequisites.push_back(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    
    // Get conditional prerequisites
    lua_getfield(L, 1, "min_star_rating");
    if (lua_isnumber(L, -1)) {
        node.min_star_rating = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "min_population");
    if (lua_isnumber(L, -1)) {
        node.min_population = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    
    // Get required facilities array
    lua_getfield(L, 1, "required_facilities");
    if (lua_istable(L, -1)) {
        int len = lua_rawlen(L, -1);
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (lua_isstring(L, -1)) {
                node.required_facilities.push_back(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    
    // Add the node to the research tree
    research_tree->nodes.push_back(node);
    research_tree->UpdateNodeStates();
    
    std::cout << "LuaModManager: Registered research node '" 
              << node.name << "' (ID: " << node.id << ")" << std::endl;
    
    return 0;
}

int LuaModManager::Lua_Log(lua_State* L) {
    int n = lua_gettop(L);  // Number of arguments
    
    std::string message;
    for (int i = 1; i <= n; ++i) {
        if (lua_isstring(L, i)) {
            if (i > 1) message += " ";
            message += lua_tostring(L, i);
        }
    }
    
    std::cout << "[Lua Mod] " << message << std::endl;
    
    return 0;
}

} // namespace Core
} // namespace TowerForge
