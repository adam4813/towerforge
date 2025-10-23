#pragma once

#include <memory>
#include <string>
#include "core/components.hpp"

namespace TowerForge::Core {

    // Forward declarations
    class FacilityManager;
    class TowerGrid;

    /**
     * @brief Command interface for undo/redo operations
     * 
     * Implements the Command pattern to encapsulate facility management actions
     * with full state restoration capability.
     */
    class ICommand {
    public:
        virtual ~ICommand() = default;

        /**
         * @brief Execute the command
         * @return true if execution succeeded, false otherwise
         */
        virtual bool Execute() = 0;

        /**
         * @brief Undo the command
         * @return true if undo succeeded, false otherwise
         */
        virtual bool Undo() = 0;

        /**
         * @brief Get a human-readable description of the command
         */
        virtual std::string GetDescription() const = 0;

        /**
         * @brief Get the cost/refund associated with this command
         * @return Negative for cost, positive for refund
         */
        virtual int GetCostChange() const = 0;
    };

    /**
     * @brief Captured state of a facility for restoration
     */
    struct FacilityState {
        BuildingComponent::Type type;
        int floor;
        int column;
        int width;
        int capacity;
        int current_occupancy;
        float satisfaction_score;
        int facility_cost;

        FacilityState() = default;

        FacilityState(const BuildingComponent::Type t, const int f, const int c, const int w, const int cap, const int occ, const float sat, const int cost)
            : type(t), floor(f), column(c), width(w), capacity(cap), 
              current_occupancy(occ), satisfaction_score(sat), facility_cost(cost) {}
    };

    /**
     * @brief Command to place a facility
     */
    class PlaceFacilityCommand : public ICommand {
    public:
        PlaceFacilityCommand(
            FacilityManager& facility_mgr,
            TowerGrid& grid,
            BuildingComponent::Type type,
            int floor,
            int column,
            int width,
            int cost
        );

        bool Execute() override;
        bool Undo() override;
        std::string GetDescription() const override;
        int GetCostChange() const override { return -cost_; }

        int GetCreatedEntityId() const { return created_entity_id_; }

    private:
        FacilityManager& facility_mgr_;
        TowerGrid& grid_;
        BuildingComponent::Type type_;
        int floor_;
        int column_;
        int width_;
        int cost_;
        int created_entity_id_;
    };

    /**
     * @brief Command to demolish a facility
     */
    class DemolishFacilityCommand : public ICommand {
    public:
        DemolishFacilityCommand(
            FacilityManager& facility_mgr,
            TowerGrid& grid,
            int floor,
            int column,
            float recovery_percentage = 0.5f
        );

        bool Execute() override;
        bool Undo() override;
        std::string GetDescription() const override;
        int GetCostChange() const override { return refund_; }

    private:
        bool CaptureFacilityState();
        
        FacilityManager& facility_mgr_;
        TowerGrid& grid_;
        int floor_;
        int column_;
        float recovery_percentage_;
        int refund_;
        FacilityState captured_state_;
        bool state_captured_;
    };

}
