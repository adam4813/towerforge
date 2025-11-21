#include "core/command.hpp"
#include "core/facility_manager.hpp"
#include "core/tower_grid.hpp"
#include <sstream>

namespace towerforge::core {

    // PlaceFacilityCommand implementation

    PlaceFacilityCommand::PlaceFacilityCommand(
        FacilityManager& facility_mgr,
        TowerGrid& grid,
        const BuildingComponent::Type type,
        const int floor,
        const int column,
        const int width,
        const int cost
    )
        : facility_mgr_(facility_mgr)
        , grid_(grid)
        , type_(type)
        , floor_(floor)
        , column_(column)
        , width_(width)
        , cost_(cost)
        , created_entity_id_(-1) {
    }

    bool PlaceFacilityCommand::Execute() {
        // Create the facility
        const auto entity = facility_mgr_.CreateFacility(type_, floor_, column_, width_);
        if (!entity) {
            return false;
        }

        created_entity_id_ = static_cast<int>(entity.id());
        return true;
    }

    bool PlaceFacilityCommand::Undo() {
        if (created_entity_id_ < 0) {
            return false;
        }

        // Remove the facility that was placed
        return facility_mgr_.RemoveFacilityAt(floor_, column_);
    }

    std::string PlaceFacilityCommand::GetDescription() const {
        std::stringstream ss;
        ss << "Place " << FacilityManager::GetTypeName(type_)
           << " at Floor " << floor_ << ", Column " << column_;
        return ss.str();
    }

    // DemolishFacilityCommand implementation

    DemolishFacilityCommand::DemolishFacilityCommand(
        FacilityManager& facility_mgr,
        TowerGrid& grid,
        const int floor,
        const int column,
        const float recovery_percentage
    )
        : facility_mgr_(facility_mgr)
        , grid_(grid)
        , floor_(floor)
        , column_(column)
        , recovery_percentage_(recovery_percentage)
        , refund_(0)
        , state_captured_(false) {
    }

    bool DemolishFacilityCommand::CaptureFacilityState() {
        if (!grid_.IsOccupied(floor_, column_)) {
            return false;
        }

        const int facility_id = grid_.GetFacilityAt(floor_, column_);
        if (facility_id < 0) {
            return false;
        }

        // Get the facility type
        const auto facility_type = facility_mgr_.GetFacilityType(facility_id);

        // Calculate the width by scanning the grid
        int start_col = column_;
        while (start_col > 0 && grid_.GetFacilityAt(floor_, start_col - 1) == facility_id) {
            start_col--;
        }
        int end_col = column_;
        while (end_col < grid_.GetColumnCount() - 1 &&
               grid_.GetFacilityAt(floor_, end_col + 1) == facility_id) {
            end_col++;
        }
        const int width = end_col - start_col + 1;

        // Get default values for the facility type
        const int capacity = FacilityManager::GetDefaultCapacity(facility_type);

        // Calculate cost based on facility type and width
        // This is a simplified calculation - in a full implementation, 
        // we'd store the actual cost paid
        int facility_cost = 0;
        switch (facility_type) {
            case BuildingComponent::Type::Lobby: facility_cost = 1000; break;
            case BuildingComponent::Type::Office: facility_cost = 5000; break;
            case BuildingComponent::Type::Restaurant: facility_cost = 8000; break;
            case BuildingComponent::Type::RetailShop: facility_cost = 6000; break;
            case BuildingComponent::Type::Hotel: facility_cost = 12000; break;
            case BuildingComponent::Type::Gym: facility_cost = 10000; break;
            case BuildingComponent::Type::Arcade: facility_cost = 9000; break;
            case BuildingComponent::Type::Theater: facility_cost = 15000; break;
            case BuildingComponent::Type::ConferenceHall: facility_cost = 13000; break;
            case BuildingComponent::Type::FlagshipStore: facility_cost = 18000; break;
            case BuildingComponent::Type::Elevator: facility_cost = 15000; break;
            default: facility_cost = 5000; break;
        }

        // Calculate refund
        refund_ = static_cast<int>(facility_cost * recovery_percentage_);

        // Store the facility state
        captured_state_ = FacilityState(
            facility_type,
            floor_,
            start_col,  // Use the actual start column
            width,
            capacity,
            0,  // Current occupancy - simplified
            100.0f,  // Satisfaction - simplified
            facility_cost
        );

        state_captured_ = true;
        return true;
    }

    bool DemolishFacilityCommand::Execute() {
        // Capture the facility state before demolishing
        if (!CaptureFacilityState()) {
            return false;
        }

        // Remove the facility
        return facility_mgr_.RemoveFacilityAt(floor_, column_);
    }

    bool DemolishFacilityCommand::Undo() {
        if (!state_captured_) {
            return false;
        }

        // Restore the facility using the captured state
        const auto entity = facility_mgr_.CreateFacility(
            captured_state_.type,
            captured_state_.floor,
            captured_state_.column,
            captured_state_.width
        );

        return entity.is_alive();
    }

    std::string DemolishFacilityCommand::GetDescription() const {
        if (state_captured_) {
            std::stringstream ss;
            ss << "Demolish " << FacilityManager::GetTypeName(captured_state_.type)
               << " at Floor " << captured_state_.floor 
               << ", Column " << captured_state_.column;
            return ss.str();
        }
        return "Demolish Facility";
    }

}
