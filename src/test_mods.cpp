#include "core/lua_mod_manager.hpp"
#include "core/ecs_world.hpp"
#include <iostream>

using namespace towerforge::core;

int main() {
    std::cout << "Testing Lua Mod Manager..." << std::endl;
    
    // Create ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Get the mod manager
    const LuaModManager& mod_mgr = ecs_world.GetModManager();
    
    // Check loaded mods
    const auto& mods = mod_mgr.GetLoadedMods();
    std::cout << "\nTotal mods loaded: " << mods.size() << std::endl;
    
    for (const auto& mod : mods) {
        std::cout << "\nMod: " << mod.name << std::endl;
        std::cout << "  ID: " << mod.id << std::endl;
        std::cout << "  Version: " << mod.version << std::endl;
        std::cout << "  Author: " << mod.author << std::endl;
        std::cout << "  Description: " << mod.description << std::endl;
        std::cout << "  Loaded: " << (mod.loaded_successfully ? "YES" : "NO") << std::endl;
        if (!mod.loaded_successfully) {
            std::cout << "  Error: " << mod.error_message << std::endl;
        }
    }
    
    // Check registered facility types
    const auto& facilities = mod_mgr.GetCustomFacilityTypes();
    std::cout << "\nTotal custom facility types: " << facilities.size() << std::endl;
    for (const auto& [id, facility] : facilities) {
        std::cout << "\nFacility: " << facility.name << " (ID: " << id << ")" << std::endl;
        std::cout << "  Width: " << facility.default_width << std::endl;
        std::cout << "  Capacity: " << facility.default_capacity << std::endl;
        std::cout << "  Staff Required: " << facility.required_staff << std::endl;
        std::cout << "  Operating Hours: " << facility.operating_start_hour 
                  << " - " << facility.operating_end_hour << std::endl;
        std::cout << "  Economics: Rent=$" << facility.base_rent 
                  << ", Cost=$" << facility.operating_cost << std::endl;
    }
    
    // Check registered visitor types
    const auto& visitors = mod_mgr.GetCustomVisitorTypes();
    std::cout << "\nTotal custom visitor types: " << visitors.size() << std::endl;
    for (const auto& [id, visitor] : visitors) {
        std::cout << "\nVisitor: " << visitor.name << " (ID: " << id << ")" << std::endl;
        std::cout << "  Move Speed: " << visitor.move_speed << std::endl;
        std::cout << "  Visit Duration: " << visitor.min_visit_duration 
                  << " - " << visitor.max_visit_duration << " seconds" << std::endl;
        std::cout << "  Preferred Facilities: ";
        for (size_t i = 0; i < visitor.preferred_facilities.size(); ++i) {
            std::cout << visitor.preferred_facilities[i];
            if (i < visitor.preferred_facilities.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
}
