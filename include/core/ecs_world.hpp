#pragma once

#include <flecs.h>
#include <memory>
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include "core/lua_mod_manager.hpp"

namespace towerforge::core {

    // Forward declaration
    class FacilityManager;
    class LuaModManager;

    /**
 * @brief Wrapper around the flecs ECS world
 * 
 * This class manages the main ECS world instance and provides
 * a clean interface for creating entities, systems, and queries.
 * It serves as the backbone for all simulation logic.
 */
    class ECSWorld {
    public:
        ECSWorld();
        
        /**
     * @brief Construct ECS world with screen-based grid dimensions
     * @param screen_width Width of the screen in pixels
     * @param screen_height Height of the screen in pixels
     * @param cell_width Width of one grid cell in pixels
     * @param cell_height Height of one grid cell in pixels
     */
        ECSWorld(int screen_width, int screen_height, int cell_width, int cell_height);
        
        ~ECSWorld();

        /**
     * @brief Initialize the ECS world and register components/systems
     */
        void Initialize();

        /**
     * @brief Run one simulation frame
     * @param delta_time Time elapsed since last frame in seconds
     * @return true if the simulation should continue, false otherwise
     */
        bool Update(float delta_time) const;

        /**
     * @brief Get the underlying flecs world
     * @return Reference to the flecs world
     */
        flecs::world& GetWorld();

        /**
     * @brief Create a new entity in the world
     * @return The created entity
     */
        flecs::entity CreateEntity(const char* name = nullptr) const;
    
        /**
     * @brief Get the tower grid instance
     * @return Reference to the tower grid
     */
        TowerGrid& GetTowerGrid() const;
    
        /**
     * @brief Get the facility manager instance
     * @return Reference to the facility manager
     */
        FacilityManager& GetFacilityManager() const;
    
        /**
     * @brief Get the Lua mod manager instance
     * @return Reference to the Lua mod manager
     */
        LuaModManager& GetModManager() const;

        /**
     * @brief Apply vertical expansion upgrades from research tree to tower grid
     * Should be called after unlocking a VerticalExpansion research node
     */
        void ApplyVerticalExpansionUpgrades();

    private:
        void RegisterComponents() const;
        void RegisterSystems() const;

        mutable flecs::world world_;
        std::unique_ptr<TowerGrid> tower_grid_;
        std::unique_ptr<FacilityManager> facility_manager_;
        std::unique_ptr<LuaModManager> mod_manager_;
    };

}
