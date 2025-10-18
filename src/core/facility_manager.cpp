#include "core/facility_manager.hpp"
#include <iostream>

namespace TowerForge::Core {

    FacilityManager::FacilityManager(flecs::world& world, TowerGrid& grid)
        : world_(world), grid_(grid) {
    }

    flecs::entity FacilityManager::CreateFacility(
        BuildingComponent::Type type,
        int floor,
        int column,
        int width,
        const char* name
    ) const {
        // Use default width if not specified
        if (width <= 0) {
            width = GetDefaultWidth(type);
        }
    
        // Validate that the space is available
        if (!grid_.IsValidPosition(floor, column)) {
            std::cerr << "Failed to create facility: Invalid position (" 
                    << floor << ", " << column << ")" << std::endl;
            return flecs::entity::null();
        }
    
        if (!grid_.IsSpaceAvailable(floor, column, width)) {
            std::cerr << "Failed to create facility: Space not available at (" 
                    << floor << ", " << column << ") with width " << width << std::endl;
            return flecs::entity::null();
        }
    
        // Build floors automatically when placing facility
        // Note: Cost handling should be done by the caller (e.g., placement system)
        BuildFloorsForFacility(floor, column, width);
    
        // Create the entity
        const flecs::entity facility = name ? world_.entity(name) : world_.entity();
    
        // Add BuildingComponent
        int capacity = GetDefaultCapacity(type);
        facility.set<BuildingComponent>({type, floor, column, width, capacity});
    
        // Add GridPosition
        facility.set<GridPosition>({floor, column, width});
    
        // Add FacilityStatus for tracking cleanliness and maintenance
        facility.set<FacilityStatus>({});
    
        // Add CleanlinessStatus for state-based cleanliness tracking
        CleanlinessStatus cleanliness;
        // Set dirty_rate based on facility type
        switch (type) {
            case BuildingComponent::Type::Restaurant:
            case BuildingComponent::Type::Hotel:
                cleanliness.dirty_rate = 1.5f;  // Get dirty faster
                break;
            case BuildingComponent::Type::RetailShop:
            case BuildingComponent::Type::Arcade:
            case BuildingComponent::Type::FlagshipStore:
                cleanliness.dirty_rate = 1.2f;  // Slightly faster
                break;
            case BuildingComponent::Type::Office:
            case BuildingComponent::Type::Gym:
            case BuildingComponent::Type::Theater:
                cleanliness.dirty_rate = 1.0f;  // Normal rate
                break;
            default:
                cleanliness.dirty_rate = 0.8f;  // Slower degradation
                break;
        }
        facility.set<CleanlinessStatus>(cleanliness);
    
        // Place on the grid using the entity ID
        if (!grid_.PlaceFacility(floor, column, width, static_cast<int>(facility.id()))) {
            // If placement fails, destroy the entity and return null
            facility.destruct();
            std::cerr << "Failed to place facility on grid" << std::endl;
            return flecs::entity::null();
        }
    
        return facility;
    }

    bool FacilityManager::RemoveFacility(const flecs::entity facility_entity) const {
        if (!facility_entity.is_alive()) {
            return false;
        }
    
        // Check if entity has GridPosition component
        if (facility_entity.has<GridPosition>()) {
            // Get the grid position to remove from grid
            const auto grid_pos = facility_entity.get<GridPosition>();
            grid_.RemoveFacilityAt(grid_pos.floor, grid_pos.column);
        }
    
        // Destroy the entity
        facility_entity.destruct();
    
        return true;
    }

    bool FacilityManager::RemoveFacilityAt(const int floor, const int column) const {
        // Get the facility ID at this position
        const int facility_id = grid_.GetFacilityAt(floor, column);
        if (facility_id < 0) {
            return false;
        }
    
        // Get the entity
        const flecs::entity facility = world_.entity(static_cast<flecs::entity_t>(facility_id));
    
        // Remove using the other method
        return RemoveFacility(facility);
    }

    int FacilityManager::GetDefaultWidth(const BuildingComponent::Type type) {
        switch (type) {
            case BuildingComponent::Type::Office:
                return 8;  // Office spaces are medium-sized
            case BuildingComponent::Type::Residential:
                return 6;  // Residential condos are medium-sized
            case BuildingComponent::Type::RetailShop:
                return 4;  // Retail shops are smaller
            case BuildingComponent::Type::Lobby:
                return 10; // Lobby spans most of the ground floor
            case BuildingComponent::Type::Restaurant:
                return 6;  // Restaurants are medium-sized
            case BuildingComponent::Type::Hotel:
                return 10; // Hotels are large
            case BuildingComponent::Type::Elevator:
                return 2;  // Elevators are narrow
            case BuildingComponent::Type::Gym:
                return 7;  // Gyms are medium-large
            case BuildingComponent::Type::Arcade:
                return 5;  // Arcades are medium-sized
            case BuildingComponent::Type::Theater:
                return 8;  // Theaters are large
            case BuildingComponent::Type::ConferenceHall:
                return 9;  // Conference halls are large
            case BuildingComponent::Type::FlagshipStore:
                return 12; // Flagship stores are very large
            case BuildingComponent::Type::ManagementOffice:
                return 8;  // Management offices are medium-sized
            case BuildingComponent::Type::SatelliteOffice:
                return 6;  // Satellite offices are smaller
            default:
                return 4;
        }
    }

    int FacilityManager::GetDefaultCapacity(const BuildingComponent::Type type) {
        switch (type) {
            case BuildingComponent::Type::Office:
                return 20;  // Office can hold ~20 workers
            case BuildingComponent::Type::Residential:
                return 4;   // Residential condo for 1 family (~4 people)
            case BuildingComponent::Type::RetailShop:
                return 15;  // Retail shop has moderate customer capacity
            case BuildingComponent::Type::Lobby:
                return 50;  // Lobby can hold many people passing through
            case BuildingComponent::Type::Restaurant:
                return 30;  // Restaurant has high seating capacity
            case BuildingComponent::Type::Hotel:
                return 40;  // Hotel has multiple rooms
            case BuildingComponent::Type::Elevator:
                return 8;   // Elevator holds ~8 people
            case BuildingComponent::Type::Gym:
                return 25;  // Gym has equipment for ~25 people
            case BuildingComponent::Type::Arcade:
                return 20;  // Arcade has space for ~20 people
            case BuildingComponent::Type::Theater:
                return 50;  // Theater has seating for ~50 people
            case BuildingComponent::Type::ConferenceHall:
                return 60;  // Conference hall can hold ~60 people
            case BuildingComponent::Type::FlagshipStore:
                return 40;  // Flagship store has high capacity
            case BuildingComponent::Type::ManagementOffice:
                return 10;  // Management office capacity
            case BuildingComponent::Type::SatelliteOffice:
                return 6;   // Satellite office capacity
            default:
                return 10;
        }
    }

    const char* FacilityManager::GetTypeName(const BuildingComponent::Type type) {
        switch (type) {
            case BuildingComponent::Type::Office:
                return "Office";
            case BuildingComponent::Type::Residential:
                return "Residential";
            case BuildingComponent::Type::RetailShop:
                return "RetailShop";
            case BuildingComponent::Type::Lobby:
                return "Lobby";
            case BuildingComponent::Type::Restaurant:
                return "Restaurant";
            case BuildingComponent::Type::Hotel:
                return "Hotel";
            case BuildingComponent::Type::Elevator:
                return "Elevator";
            case BuildingComponent::Type::Gym:
                return "Gym";
            case BuildingComponent::Type::Arcade:
                return "Arcade";
            case BuildingComponent::Type::Theater:
                return "Theater";
            case BuildingComponent::Type::ConferenceHall:
                return "ConferenceHall";
            case BuildingComponent::Type::FlagshipStore:
                return "FlagshipStore";
            case BuildingComponent::Type::ManagementOffice:
                return "ManagementOffice";
            case BuildingComponent::Type::SatelliteOffice:
                return "SatelliteOffice";
            default:
                return "Unknown";
        }
    }

    unsigned int FacilityManager::GetFacilityColor(const BuildingComponent::Type type) {
        // Return RGBA color as unsigned int (0xRRGGBBAA format)
        switch (type) {
            case BuildingComponent::Type::Office:
                return 0x66BFFF;  // SKYBLUE
            case BuildingComponent::Type::Residential:
                return 0xC87AFF;  // PURPLE
            case BuildingComponent::Type::RetailShop:
                return 0x00E400;  // GREEN
            case BuildingComponent::Type::Lobby:
                return 0xFFCB00;  // GOLD
            case BuildingComponent::Type::Restaurant:
                return 0xE62920;  // RED
            case BuildingComponent::Type::Hotel:
                return 0x002B80;  // DARKBLUE
            case BuildingComponent::Type::Elevator:
                return 0x828282;  // GRAY
            case BuildingComponent::Type::Gym:
                return 0xFF6B35;  // ORANGE
            case BuildingComponent::Type::Arcade:
                return 0xFF00FF;  // MAGENTA
            case BuildingComponent::Type::Theater:
                return 0x8B00FF;  // VIOLET
            case BuildingComponent::Type::ConferenceHall:
                return 0x4A90E2;  // LIGHTBLUE
            case BuildingComponent::Type::FlagshipStore:
                return 0x00CED1;  // TURQUOISE
            case BuildingComponent::Type::ManagementOffice:
                return 0x2C3E50;  // DARK SLATE (executive/professional)
            case BuildingComponent::Type::SatelliteOffice:
                return 0x34495E;  // LIGHTER SLATE
            default:
                return 0x66BFFF;  // Default to SKYBLUE
        }
    }

    BuildingComponent::Type FacilityManager::GetFacilityType(const int facility_entity_id) const {
        // Get the entity from the ID
        const flecs::entity facility = world_.entity(static_cast<flecs::entity_t>(facility_entity_id));
    
        // Check if entity exists and has BuildingComponent
        if (facility.is_alive() && facility.has<BuildingComponent>()) {
            const auto component = facility.get<BuildingComponent>();
            return component.type;
        }
    
        // Default to Office if not found
        return BuildingComponent::Type::Office;
    }

    int FacilityManager::CalculateFloorBuildCost(const int floor, const int column, const int width) const {
        int cost = 0;
    
        // Check each cell to see if floor needs to be built
        for (int i = 0; i < width; ++i) {
            if (!grid_.IsFloorBuilt(floor, column + i)) {
                cost += TowerGrid::GetFloorBuildCost();
            }
        }
    
        return cost;
    }

    bool FacilityManager::BuildFloorsForFacility(const int floor, const int column, const int width) const {
        // Build the floor for the facility placement
        return grid_.BuildFloor(floor, column, width);
    }

    bool FacilityManager::CleanFacility(const flecs::entity facility_entity) const {
        if (!facility_entity.is_alive()) {
            return false;
        }
    
        // Check if the facility has CleanlinessStatus
        if (facility_entity.has<CleanlinessStatus>()) {
            auto cleanliness = facility_entity.get_mut<CleanlinessStatus>();
            cleanliness->Clean();
            return true;
        }
    
        return false;
    }

}
