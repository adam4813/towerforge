#pragma once

#include "ui/ui_window.h"
#include <string>
#include <vector>
#include <map>

namespace towerforge::ui {

    /**
     * @brief Structure to hold income breakdown by facility type
     */
    struct IncomeBreakdown {
        struct FacilityTypeRevenue {
            std::string facility_type;
            float hourly_revenue;
            int facility_count;
            int total_tenants;
            float average_occupancy;
        };
        
        std::vector<FacilityTypeRevenue> revenues;
        float total_hourly_revenue = 0.0f;
        float total_operating_costs = 0.0f;
        float net_hourly_profit = 0.0f;
    };

    /**
     * @brief Structure to hold elevator usage statistics
     */
    struct ElevatorAnalytics {
        struct ElevatorStats {
            int elevator_id;
            int total_trips;
            float average_wait_time;
            float utilization_rate;  // Percentage of time elevator is occupied
            int total_passengers_carried;
            std::vector<std::pair<int, int>> floor_traffic;  // floor, passenger count
        };
        
        std::vector<ElevatorStats> elevators;
        int total_passengers = 0;
        float average_wait_time = 0.0f;
        int busiest_floor = 0;
        int busiest_floor_count = 0;
    };

    /**
     * @brief Structure to hold population breakdown
     */
    struct PopulationBreakdown {
        int total_population;
        int employees;
        int visitors;
        int residential_occupancy;
        int residential_capacity;
        float average_satisfaction;
    };

    /**
     * @brief Overlay window for displaying income analytics
     */
    class IncomeAnalyticsOverlay : public UIWindow {
    public:
        IncomeAnalyticsOverlay(const IncomeBreakdown& data);
        void Render() const override;
        void Update(const IncomeBreakdown& data);
        
    protected:
        void RenderContent() const override;
        
    private:
        IncomeBreakdown data_;
    };

    /**
     * @brief Overlay window for displaying elevator analytics
     */
    class ElevatorAnalyticsOverlay : public UIWindow {
    public:
        ElevatorAnalyticsOverlay(const ElevatorAnalytics& data);
        void Render() const override;
        void Update(const ElevatorAnalytics& data);
        
    protected:
        void RenderContent() const override;
        
    private:
        ElevatorAnalytics data_;
    };

    /**
     * @brief Overlay window for displaying population analytics
     */
    class PopulationAnalyticsOverlay : public UIWindow {
    public:
        PopulationAnalyticsOverlay(const PopulationBreakdown& data);
        void Render() const override;
        void Update(const PopulationBreakdown& data);
        
    protected:
        void RenderContent() const override;
        
    private:
        PopulationBreakdown data_;
    };

}
