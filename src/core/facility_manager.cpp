#include "core/facility_manager.hpp"
#include <iostream>

namespace towerforge::core {

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
    
        // Add AdjacencyEffects component
        facility.set<AdjacencyEffects>({});
    
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
    
        // Add MaintenanceStatus for tracking maintenance state
        MaintenanceStatus maintenance;
        // Set degrade_rate based on facility type (facilities with more moving parts degrade faster)
        switch (type) {
            case BuildingComponent::Type::Elevator:
                maintenance.degrade_rate = 2.0f;  // Elevators need frequent maintenance
                break;
            case BuildingComponent::Type::Arcade:
            case BuildingComponent::Type::Theater:
                maintenance.degrade_rate = 1.5f;  // Entertainment venues with equipment
                break;
            case BuildingComponent::Type::Restaurant:
            case BuildingComponent::Type::Hotel:
            case BuildingComponent::Type::Gym:
                maintenance.degrade_rate = 1.3f;  // Facilities with specialized equipment
                break;
            case BuildingComponent::Type::Office:
            case BuildingComponent::Type::RetailShop:
            case BuildingComponent::Type::FlagshipStore:
                maintenance.degrade_rate = 1.0f;  // Normal maintenance rate
                break;
            default:
                maintenance.degrade_rate = 0.8f;  // Slower degradation for simpler facilities
                break;
        }
        facility.set<MaintenanceStatus>(maintenance);
    
        // Place on the grid using the entity ID
        if (!grid_.PlaceFacility(floor, column, width, static_cast<int>(facility.id()))) {
            // If placement fails, destroy the entity and return null
            facility.destruct();
            std::cerr << "Failed to place facility on grid" << std::endl;
            return flecs::entity::null();
        }
    
        // Update adjacency effects for this facility and adjacent facilities
        UpdateAdjacencyEffects(facility);
        UpdateAdjacentFacilityEffects(floor, column, width);
    
        return facility;
    }

    bool FacilityManager::RemoveFacility(const flecs::entity facility_entity) const {
        if (!facility_entity.is_alive()) {
            return false;
        }
    
        // Get position before removing to update adjacent facilities
        int floor = -1;
        int column = -1;
        int width = 0;
    
        // Check if entity has GridPosition component
        if (facility_entity.has<GridPosition>()) {
            // Get the grid position to remove from grid
            const auto grid_pos = facility_entity.get<GridPosition>();
            floor = grid_pos.floor;
            column = grid_pos.column;
            width = grid_pos.width;
            grid_.RemoveFacilityAt(grid_pos.floor, grid_pos.column);
        }
    
        // Destroy the entity
        facility_entity.destruct();
    
        // Update adjacency effects for adjacent facilities
        if (floor >= 0 && column >= 0 && width > 0) {
            UpdateAdjacentFacilityEffects(floor, column, width);
        }
    
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
            auto& cleanliness = facility_entity.ensure<CleanlinessStatus>();
            cleanliness.Clean();
            return true;
        }
    
        return false;
    }

    bool FacilityManager::RepairFacility(const flecs::entity facility_entity) const {
        if (!facility_entity.is_alive()) {
            return false;
        }
    
        // Check if the facility has MaintenanceStatus
        if (facility_entity.has<MaintenanceStatus>()) {
            auto& maintenance = facility_entity.ensure<MaintenanceStatus>();
            maintenance.Repair();
            return true;
        }
    
        return false;
    }

    bool FacilityManager::SetAutoRepair(const flecs::entity facility_entity, const bool enabled) const {
        if (!facility_entity.is_alive()) {
            return false;
        }
    
        // Check if the facility has MaintenanceStatus
        if (facility_entity.has<MaintenanceStatus>()) {
            auto& maintenance = facility_entity.ensure<MaintenanceStatus>();
            maintenance.auto_repair_enabled = enabled;
            return true;
        }
    
        return false;
    }

    void FacilityManager::UpdateAdjacencyEffects(const flecs::entity facility_entity) const {
        if (!facility_entity.is_alive() || !facility_entity.has<BuildingComponent>() || 
            !facility_entity.has<GridPosition>()) {
            return;
        }

        const auto building = facility_entity.get<BuildingComponent>();
        const auto grid_pos = facility_entity.get<GridPosition>();

        // Get or create AdjacencyEffects component
        auto& adjacency = facility_entity.ensure<AdjacencyEffects>();
        adjacency.Clear();

        // Check neighbors on the same floor (left and right)
        // Left neighbor
        if (grid_pos.column > 0) {
            const int left_facility_id = grid_.GetFacilityAt(grid_pos.floor, grid_pos.column - 1);
            if (left_facility_id >= 0) {
                const auto neighbor_type = GetFacilityType(left_facility_id);
                if (const auto effect = CalculateAdjacencyEffect(building.type, neighbor_type)) {
                    adjacency.AddEffect(*effect);
                }
            }
        }

        // Right neighbor
        const int right_column = grid_pos.column + grid_pos.width;
        if (right_column < grid_.GetColumnCount()) {
            const int right_facility_id = grid_.GetFacilityAt(grid_pos.floor, right_column);
            if (right_facility_id >= 0) {
                const auto neighbor_type = GetFacilityType(right_facility_id);
                if (const auto effect = CalculateAdjacencyEffect(building.type, neighbor_type)) {
                    adjacency.AddEffect(*effect);
                }
            }
        }

        // Check neighbor above
        const int floor_above = grid_pos.floor + 1;
        if (floor_above <= grid_.GetHighestFloorIndex()) {
            // Check all columns this facility occupies
            for (int col = grid_pos.column; col < grid_pos.column + grid_pos.width; ++col) {
                const int above_facility_id = grid_.GetFacilityAt(floor_above, col);
                if (above_facility_id >= 0) {
                    const auto neighbor_type = GetFacilityType(above_facility_id);
                    if (const auto effect = CalculateAdjacencyEffect(building.type, neighbor_type)) {
                        // Only add if we haven't already added this neighbor type from above
                        bool already_added = false;
                        for (const auto& existing : adjacency.effects) {
                            if (existing.source_type == GetTypeName(neighbor_type) && 
                                existing.description.find("above") != std::string::npos) {
                                already_added = true;
                                break;
                            }
                        }
                        if (!already_added) {
                            adjacency.AddEffect(*effect);
                        }
                    }
                    break; // Only check once for facility above
                }
            }
        }

        // Check neighbor below
        const int floor_below = grid_pos.floor - 1;
        if (floor_below >= grid_.GetLowestFloorIndex()) {
            // Check all columns this facility occupies
            for (int col = grid_pos.column; col < grid_pos.column + grid_pos.width; ++col) {
                const int below_facility_id = grid_.GetFacilityAt(floor_below, col);
                if (below_facility_id >= 0) {
                    const auto neighbor_type = GetFacilityType(below_facility_id);
                    if (const auto effect = CalculateAdjacencyEffect(building.type, neighbor_type)) {
                        // Only add if we haven't already added this neighbor type from below
                        bool already_added = false;
                        for (const auto& existing : adjacency.effects) {
                            if (existing.source_type == GetTypeName(neighbor_type) && 
                                existing.description.find("below") != std::string::npos) {
                                already_added = true;
                                break;
                            }
                        }
                        if (!already_added) {
                            adjacency.AddEffect(*effect);
                        }
                    }
                    break; // Only check once for facility below
                }
            }
        }
    }

    void FacilityManager::UpdateAdjacentFacilityEffects(const int floor, const int column, const int width) const {
        // Update left neighbor
        if (column > 0) {
            const int left_facility_id = grid_.GetFacilityAt(floor, column - 1);
            if (left_facility_id >= 0) {
                const auto left_entity = world_.entity(static_cast<flecs::entity_t>(left_facility_id));
                if (left_entity.is_alive()) {
                    UpdateAdjacencyEffects(left_entity);
                }
            }
        }

        // Update right neighbor
        const int right_column = column + width;
        if (right_column < grid_.GetColumnCount()) {
            const int right_facility_id = grid_.GetFacilityAt(floor, right_column);
            if (right_facility_id >= 0) {
                const auto right_entity = world_.entity(static_cast<flecs::entity_t>(right_facility_id));
                if (right_entity.is_alive()) {
                    UpdateAdjacencyEffects(right_entity);
                }
            }
        }

        // Update facilities above
        const int floor_above = floor + 1;
        if (floor_above <= grid_.GetHighestFloorIndex()) {
            for (int col = column; col < column + width; ++col) {
                const int above_facility_id = grid_.GetFacilityAt(floor_above, col);
                if (above_facility_id >= 0) {
                    const auto above_entity = world_.entity(static_cast<flecs::entity_t>(above_facility_id));
                    if (above_entity.is_alive()) {
                        UpdateAdjacencyEffects(above_entity);
                    }
                    break; // Only update once per facility above
                }
            }
        }

        // Update facilities below
        const int floor_below = floor - 1;
        if (floor_below >= grid_.GetLowestFloorIndex()) {
            for (int col = column; col < column + width; ++col) {
                const int below_facility_id = grid_.GetFacilityAt(floor_below, col);
                if (below_facility_id >= 0) {
                    const auto below_entity = world_.entity(static_cast<flecs::entity_t>(below_facility_id));
                    if (below_entity.is_alive()) {
                        UpdateAdjacencyEffects(below_entity);
                    }
                    break; // Only update once per facility below
                }
            }
        }
    }

    std::optional<AdjacencyEffect> FacilityManager::CalculateAdjacencyEffect(
        const BuildingComponent::Type facility_type,
        const BuildingComponent::Type neighbor_type
    ) {
        // Define adjacency rules here
        // Positive values are bonuses, negative values are penalties

        // Restaurant next to entertainment (Theater, Arcade) - revenue bonus
        if (facility_type == BuildingComponent::Type::Restaurant) {
            if (neighbor_type == BuildingComponent::Type::Theater) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Revenue,
                    10.0f,
                    GetTypeName(neighbor_type),
                    "+10% revenue: next to theater"
                );
            }
            if (neighbor_type == BuildingComponent::Type::Arcade) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Revenue,
                    8.0f,
                    GetTypeName(neighbor_type),
                    "+8% revenue: next to arcade"
                );
            }
        }

        // Entertainment next to Restaurant - traffic bonus
        if (facility_type == BuildingComponent::Type::Theater || 
            facility_type == BuildingComponent::Type::Arcade) {
            if (neighbor_type == BuildingComponent::Type::Restaurant) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Traffic,
                    10.0f,
                    GetTypeName(neighbor_type),
                    "+10% traffic: next to restaurant"
                );
            }
        }

        // Residential next to Arcade - satisfaction penalty (noise)
        if (facility_type == BuildingComponent::Type::Residential) {
            if (neighbor_type == BuildingComponent::Type::Arcade) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Satisfaction,
                    -8.0f,
                    GetTypeName(neighbor_type),
                    "-8% satisfaction: noisy arcade nearby"
                );
            }
            if (neighbor_type == BuildingComponent::Type::Theater) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Satisfaction,
                    -5.0f,
                    GetTypeName(neighbor_type),
                    "-5% satisfaction: noisy theater nearby"
                );
            }
        }

        // Residential next to Gym - satisfaction bonus (amenity)
        if (facility_type == BuildingComponent::Type::Residential) {
            if (neighbor_type == BuildingComponent::Type::Gym) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Satisfaction,
                    5.0f,
                    GetTypeName(neighbor_type),
                    "+5% satisfaction: gym nearby"
                );
            }
        }

        // Office next to Restaurant - satisfaction bonus (convenience)
        if (facility_type == BuildingComponent::Type::Office) {
            if (neighbor_type == BuildingComponent::Type::Restaurant) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Satisfaction,
                    5.0f,
                    GetTypeName(neighbor_type),
                    "+5% satisfaction: restaurant nearby"
                );
            }
        }

        // Retail shops benefit from being near other retail (shopping district)
        if (facility_type == BuildingComponent::Type::RetailShop || 
            facility_type == BuildingComponent::Type::FlagshipStore) {
            if (neighbor_type == BuildingComponent::Type::RetailShop || 
                neighbor_type == BuildingComponent::Type::FlagshipStore) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Traffic,
                    5.0f,
                    GetTypeName(neighbor_type),
                    "+5% traffic: shopping district"
                );
            }
        }

        // Hotel next to Restaurant - satisfaction bonus
        if (facility_type == BuildingComponent::Type::Hotel) {
            if (neighbor_type == BuildingComponent::Type::Restaurant) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Satisfaction,
                    8.0f,
                    GetTypeName(neighbor_type),
                    "+8% satisfaction: restaurant nearby"
                );
            }
        }

        // ConferenceHall next to Hotel - synergy bonus
        if (facility_type == BuildingComponent::Type::ConferenceHall) {
            if (neighbor_type == BuildingComponent::Type::Hotel) {
                return AdjacencyEffect(
                    AdjacencyEffect::Type::Revenue,
                    10.0f,
                    GetTypeName(neighbor_type),
                    "+10% revenue: hotel nearby"
                );
            }
        }

        // No effect found
        return std::nullopt;
    }

}
