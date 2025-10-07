#include "core/facility_manager.hpp"
#include <iostream>

namespace TowerForge {
namespace Core {

FacilityManager::FacilityManager(flecs::world& world, TowerGrid& grid)
    : world_(world), grid_(grid) {
}

flecs::entity FacilityManager::CreateFacility(
    BuildingComponent::Type type,
    int floor,
    int column,
    int width,
    const char* name
) {
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
    
    // Create the entity
    flecs::entity facility = name ? world_.entity(name) : world_.entity();
    
    // Add BuildingComponent
    int capacity = GetDefaultCapacity(type);
    facility.set<BuildingComponent>({type, floor, width, capacity});
    
    // Add GridPosition
    facility.set<GridPosition>({floor, column, width});
    
    // Place on the grid using the entity ID
    if (!grid_.PlaceFacility(floor, column, width, static_cast<int>(facility.id()))) {
        // If placement fails, destroy the entity and return null
        facility.destruct();
        std::cerr << "Failed to place facility on grid" << std::endl;
        return flecs::entity::null();
    }
    
    return facility;
}

bool FacilityManager::RemoveFacility(flecs::entity facility_entity) {
    if (!facility_entity.is_alive()) {
        return false;
    }
    
    // Check if entity has GridPosition component
    if (facility_entity.has<GridPosition>()) {
        // Get the grid position to remove from grid
        auto grid_pos = facility_entity.get<GridPosition>();
        grid_.RemoveFacilityAt(grid_pos.floor, grid_pos.column);
    }
    
    // Destroy the entity
    facility_entity.destruct();
    
    return true;
}

bool FacilityManager::RemoveFacilityAt(int floor, int column) {
    // Get the facility ID at this position
    int facility_id = grid_.GetFacilityAt(floor, column);
    if (facility_id < 0) {
        return false;
    }
    
    // Get the entity
    flecs::entity facility = world_.entity(static_cast<flecs::entity_t>(facility_id));
    
    // Remove using the other method
    return RemoveFacility(facility);
}

int FacilityManager::GetDefaultWidth(BuildingComponent::Type type) {
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
        default:
            return 4;
    }
}

int FacilityManager::GetDefaultCapacity(BuildingComponent::Type type) {
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
        default:
            return 10;
    }
}

const char* FacilityManager::GetTypeName(BuildingComponent::Type type) {
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
        default:
            return "Unknown";
    }
}

unsigned int FacilityManager::GetFacilityColor(BuildingComponent::Type type) {
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
        default:
            return 0x66BFFF;  // Default to SKYBLUE
    }
}

BuildingComponent::Type FacilityManager::GetFacilityType(int facility_entity_id) const {
    // Get the entity from the ID
    flecs::entity facility = world_.entity(static_cast<flecs::entity_t>(facility_entity_id));
    
    // Check if entity exists and has BuildingComponent
    if (facility.is_alive() && facility.has<BuildingComponent>()) {
        auto component = facility.get<BuildingComponent>();
        return component.type;
    }
    
    // Default to Office if not found
    return BuildingComponent::Type::Office;
}

} // namespace Core
} // namespace TowerForge
