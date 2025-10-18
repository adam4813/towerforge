#include "ui/analytics_overlay.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge::ui {

    // IncomeAnalyticsOverlay implementation
    IncomeAnalyticsOverlay::IncomeAnalyticsOverlay(const IncomeBreakdown& data)
        : UIWindow("Income Analytics", 350, 400)
          , data_(data) {
    }

    void IncomeAnalyticsOverlay::Update(const IncomeBreakdown& data) {
        data_ = data;
    }

    void IncomeAnalyticsOverlay::Render() {
        RenderFrame(ColorAlpha(DARKBLUE, 0.95f));
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;

        // Summary section
        DrawText("=== Revenue Summary ===", x, y, 14, GOLD);
        y += 22;

        std::stringstream total_rev;
        total_rev << "Total Revenue: $" << std::fixed << std::setprecision(2) 
                  << data_.total_hourly_revenue << "/hr";
        DrawText(total_rev.str().c_str(), x, y, 13, GREEN);
        y += 20;

        std::stringstream total_cost;
        total_cost << "Operating Costs: $" << std::fixed << std::setprecision(2) 
                   << data_.total_operating_costs << "/hr";
        DrawText(total_cost.str().c_str(), x, y, 13, ORANGE);
        y += 20;

        std::stringstream net_profit;
        net_profit << "Net Profit: $" << std::fixed << std::setprecision(2) 
                   << data_.net_hourly_profit << "/hr";
        const Color profit_color = data_.net_hourly_profit >= 0 ? GREEN : RED;
        DrawText(net_profit.str().c_str(), x, y, 13, profit_color);
        y += 25;

        // Breakdown by facility type
        DrawText("=== Revenue by Type ===", x, y, 14, GOLD);
        y += 22;

        if (data_.revenues.empty()) {
            DrawText("No facilities generating revenue", x, y, 12, GRAY);
        } else {
            for (const auto& rev : data_.revenues) {
                // Facility type and count
                std::stringstream facility_line;
                facility_line << rev.facility_type << " (" << rev.facility_count << ")";
                DrawText(facility_line.str().c_str(), x, y, 12, SKYBLUE);
                y += 18;

                // Revenue
                std::stringstream rev_line;
                rev_line << "  $" << std::fixed << std::setprecision(2) 
                         << rev.hourly_revenue << "/hr";
                DrawText(rev_line.str().c_str(), x + 10, y, 11, GREEN);
                y += 16;

                // Tenants and occupancy
                std::stringstream details_line;
                details_line << "  " << rev.total_tenants << " tenants, " 
                             << std::fixed << std::setprecision(0) 
                             << rev.average_occupancy << "% occupied";
                DrawText(details_line.str().c_str(), x + 10, y, 11, LIGHTGRAY);
                y += 20;
            }
        }
    }

    // ElevatorAnalyticsOverlay implementation
    ElevatorAnalyticsOverlay::ElevatorAnalyticsOverlay(const ElevatorAnalytics& data)
        : UIWindow("Elevator Analytics", 380, 450)
          , data_(data) {
    }

    void ElevatorAnalyticsOverlay::Update(const ElevatorAnalytics& data) {
        data_ = data;
    }

    void ElevatorAnalyticsOverlay::Render() {
        RenderFrame(ColorAlpha(DARKPURPLE, 0.95f));
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;

        // Summary section
        DrawText("=== Overall Statistics ===", x, y, 14, GOLD);
        y += 22;

        std::stringstream total_pass;
        total_pass << "Total Passengers: " << data_.total_passengers;
        DrawText(total_pass.str().c_str(), x, y, 13, WHITE);
        y += 20;

        std::stringstream avg_wait;
        avg_wait << "Avg Wait Time: " << std::fixed << std::setprecision(1) 
                 << data_.average_wait_time << "s";
        const Color wait_color = data_.average_wait_time < 15.0f ? GREEN : 
                                (data_.average_wait_time < 30.0f ? YELLOW : RED);
        DrawText(avg_wait.str().c_str(), x, y, 13, wait_color);
        y += 20;

        if (data_.busiest_floor_count > 0) {
            std::stringstream busiest;
            busiest << "Busiest Floor: " << data_.busiest_floor 
                    << " (" << data_.busiest_floor_count << " trips)";
            DrawText(busiest.str().c_str(), x, y, 13, SKYBLUE);
            y += 25;
        } else {
            y += 5;
        }

        // Per-elevator breakdown
        DrawText("=== Elevator Performance ===", x, y, 14, GOLD);
        y += 22;

        if (data_.elevators.empty()) {
            DrawText("No elevators installed", x, y, 12, GRAY);
        } else {
            for (const auto& elev : data_.elevators) {
                // Elevator ID
                std::stringstream elev_id;
                elev_id << "Elevator #" << elev.elevator_id;
                DrawText(elev_id.str().c_str(), x, y, 12, SKYBLUE);
                y += 18;

                // Trips
                std::stringstream trips;
                trips << "  Trips: " << elev.total_trips;
                DrawText(trips.str().c_str(), x + 10, y, 11, LIGHTGRAY);
                y += 16;

                // Passengers
                std::stringstream passengers;
                passengers << "  Passengers: " << elev.total_passengers_carried;
                DrawText(passengers.str().c_str(), x + 10, y, 11, LIGHTGRAY);
                y += 16;

                // Utilization
                std::stringstream util;
                util << "  Utilization: " << std::fixed << std::setprecision(0) 
                     << elev.utilization_rate << "%";
                const Color util_color = elev.utilization_rate < 60.0f ? GREEN : 
                                        (elev.utilization_rate < 80.0f ? YELLOW : RED);
                DrawText(util.str().c_str(), x + 10, y, 11, util_color);
                y += 16;

                // Average wait time
                std::stringstream wait;
                wait << "  Avg Wait: " << std::fixed << std::setprecision(1) 
                     << elev.average_wait_time << "s";
                const Color wait_col = elev.average_wait_time < 15.0f ? GREEN : 
                                      (elev.average_wait_time < 30.0f ? YELLOW : RED);
                DrawText(wait.str().c_str(), x + 10, y, 11, wait_col);
                y += 20;
            }
        }
    }

    // PopulationAnalyticsOverlay implementation
    PopulationAnalyticsOverlay::PopulationAnalyticsOverlay(const PopulationBreakdown& data)
        : UIWindow("Population Analytics", 320, 280)
          , data_(data) {
    }

    void PopulationAnalyticsOverlay::Update(const PopulationBreakdown& data) {
        data_ = data;
    }

    void PopulationAnalyticsOverlay::Render() {
        RenderFrame(ColorAlpha(DARKGREEN, 0.95f));
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;

        // Title
        DrawText("=== Population Breakdown ===", x, y, 14, GOLD);
        y += 22;

        // Total population
        std::stringstream total;
        total << "Total Population: " << data_.total_population;
        DrawText(total.str().c_str(), x, y, 13, WHITE);
        y += 22;

        // Employees
        std::stringstream emp;
        emp << "Employees: " << data_.employees;
        DrawText(emp.str().c_str(), x + 10, y, 12, LIGHTGRAY);
        y += 20;

        // Visitors
        std::stringstream vis;
        vis << "Visitors: " << data_.visitors;
        DrawText(vis.str().c_str(), x + 10, y, 12, LIGHTGRAY);
        y += 25;

        // Residential section
        DrawText("=== Residential ===", x, y, 14, GOLD);
        y += 22;

        std::stringstream res_occ;
        res_occ << "Occupancy: " << data_.residential_occupancy 
                << " / " << data_.residential_capacity;
        DrawText(res_occ.str().c_str(), x, y, 12, LIGHTGRAY);
        y += 20;

        if (data_.residential_capacity > 0) {
            const float occupancy_rate = (static_cast<float>(data_.residential_occupancy) / 
                                         data_.residential_capacity) * 100.0f;
            std::stringstream rate;
            rate << "Rate: " << std::fixed << std::setprecision(0) 
                 << occupancy_rate << "%";
            const Color rate_color = occupancy_rate > 80.0f ? GREEN : 
                                    (occupancy_rate > 50.0f ? YELLOW : RED);
            DrawText(rate.str().c_str(), x, y, 12, rate_color);
            y += 25;
        } else {
            DrawText("No residential facilities", x, y, 12, GRAY);
            y += 25;
        }

        // Average satisfaction
        DrawText("=== Satisfaction ===", x, y, 14, GOLD);
        y += 22;

        std::stringstream sat;
        sat << "Average: " << std::fixed << std::setprecision(0) 
            << data_.average_satisfaction << "%";
        const Color sat_color = data_.average_satisfaction >= 70.0f ? GREEN : 
                               (data_.average_satisfaction >= 50.0f ? YELLOW : RED);
        DrawText(sat.str().c_str(), x, y, 12, sat_color);
    }

}
