#include "ui/info_windows.h"

namespace towerforge::ui {

    // FacilityWindow implementation
    FacilityWindow::FacilityWindow(const FacilityInfo& info)
        : UIWindow("Facility Info", 250, 300)
          , info_(info) {
    }

    void FacilityWindow::Update(const FacilityInfo& info) {
        info_ = info;
        title_ = info.type + " - Floor " + std::to_string(info.floor);
    }

    void FacilityWindow::Render() {
        RenderFrame(SKYBLUE);
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // Occupancy
        const std::string occupancy = "Occupancy: " + std::to_string(info_.occupancy) + 
                                "/" + std::to_string(info_.max_occupancy);
        DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Revenue
        std::stringstream revenue_ss;
        revenue_ss << "Revenue: $" << std::fixed << std::setprecision(0) << info_.revenue << "/hr";
        DrawText(revenue_ss.str().c_str(), x, y, 14, GREEN);
        y += 20;
    
        // Satisfaction
        const std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(info_.satisfaction) + 
                                   " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%";
        DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Tenants
        const std::string tenants = "Tenants: " + std::to_string(info_.tenant_count) + " workers";
        DrawText(tenants.c_str(), x, y, 14, LIGHTGRAY);
        y += 25;
    
        // Status section header
        DrawText("--- Facility Status ---", x, y, 14, YELLOW);
        y += 20;
    
        // Cleanliness
        const std::string cleanliness = "Cleanliness: " + info_.cleanliness_rating + 
                                  " (" + std::to_string(static_cast<int>(info_.cleanliness)) + "%)";
        const Color clean_color = info_.cleanliness >= 70.0f ? GREEN : (info_.cleanliness >= 50.0f ? YELLOW : RED);
        DrawText(cleanliness.c_str(), x, y, 14, clean_color);
        y += 20;
    
        // Maintenance
        const std::string maintenance = "Maintenance: " + info_.maintenance_rating + 
                                  " (" + std::to_string(static_cast<int>(info_.maintenance_level)) + "%)";
        const Color maint_color = info_.maintenance_level >= 70.0f ? GREEN : (info_.maintenance_level >= 50.0f ? YELLOW : RED);
        DrawText(maintenance.c_str(), x, y, 14, maint_color);
        y += 20;
    
        // Alerts
        if (info_.has_fire) {
            DrawText("! FIRE - Firefighter needed !", x, y, 14, RED);
            y += 20;
        }
        if (info_.has_security_issue) {
            DrawText("! Security Issue - Guard needed !", x, y, 14, ORANGE);
            y += 20;
        }
    
        y += 10;
    
        // Buttons (placeholder)
        DrawRectangle(x, y, 100, 25, DARKGRAY);
        DrawText("[Demolish]", x + 5, y + 5, 14, RED);
    
        DrawRectangle(x + 110, y, 100, 25, DARKGRAY);
        DrawText("[Upgrade]", x + 115, y + 5, 14, YELLOW);
    }

    std::string FacilityWindow::GetSatisfactionEmoji(const float satisfaction) {
        if (satisfaction >= 80) return ":)";
        if (satisfaction >= 60) return ":|";
        if (satisfaction >= 40) return ":/";
        return ":(";
    }

    // PersonWindow implementation
    PersonWindow::PersonWindow(const PersonInfo& info)
        : UIWindow("Person Info", 250, 360)
          , info_(info) {
    }

    void PersonWindow::Update(const PersonInfo& info) {
        info_ = info;
        title_ = info.name;
    }

    void PersonWindow::Render() {
        RenderFrame(YELLOW);
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // NPC Type
        const std::string type = "Type: " + info_.npc_type;
        DrawText(type.c_str(), x, y, 14, SKYBLUE);
        y += 20;
    
        // Visitor archetype (if applicable)
        if (info_.has_needs && !info_.visitor_archetype.empty()) {
            const std::string archetype = "Profile: " + info_.visitor_archetype;
            DrawText(archetype.c_str(), x, y, 14, GOLD);
            y += 20;
        }
    
        // Staff-specific information
        if (info_.is_staff) {
            DrawText("--- Staff Info ---", x, y, 14, GOLD);
            y += 20;
        
            const std::string role = "Role: " + info_.staff_role;
            DrawText(role.c_str(), x, y, 14, LIGHTGRAY);
            y += 20;
        
            const std::string duty = std::string("Status: ") + (info_.on_duty ? "On Duty" : "Off Duty");
            DrawText(duty.c_str(), x, y, 14, info_.on_duty ? GREEN : GRAY);
            y += 20;
        
            const std::string shift = "Shift: " + info_.shift_hours;
            DrawText(shift.c_str(), x, y, 14, LIGHTGRAY);
            y += 25;
        }
    
        // Status (current activity)
        const std::string status = "Status: " + info_.status;
        DrawText(status.c_str(), x, y, 14, GOLD);
        y += 20;
    
        // State
        const std::string state = "State: " + info_.state;
        DrawText(state.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Current floor
        const std::string current = "Current: Floor " + std::to_string(info_.current_floor);
        DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Destination
        const std::string dest = "Destination: Floor " + std::to_string(info_.destination_floor);
        DrawText(dest.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Wait time (only show for non-staff or if waiting)
        if (!info_.is_staff || info_.wait_time > 0) {
            std::stringstream wait_ss;
            wait_ss << "Wait Time: " << std::fixed << std::setprecision(0) << info_.wait_time << "s";
            DrawText(wait_ss.str().c_str(), x, y, 14, info_.wait_time > 30 ? RED : LIGHTGRAY);
            y += 20;
        }
    
        // Visitor needs section (if applicable)
        if (info_.has_needs) {
            y += 5;
            DrawText("--- Visitor Needs ---", x, y, 14, YELLOW);
            y += 20;
        
            // Helper function to get need color
            auto get_need_color = [](float need) {
                if (need < 30.0f) return GREEN;
                if (need < 60.0f) return YELLOW;
                return RED;
            };
        
            // Hunger
            std::string hunger_text = "Hunger: " + std::to_string(static_cast<int>(info_.hunger_need)) + "%";
            DrawText(hunger_text.c_str(), x, y, 13, get_need_color(info_.hunger_need));
            y += 18;
        
            // Entertainment
            std::string ent_text = "Entertainment: " + std::to_string(static_cast<int>(info_.entertainment_need)) + "%";
            DrawText(ent_text.c_str(), x, y, 13, get_need_color(info_.entertainment_need));
            y += 18;
        
            // Comfort
            std::string comfort_text = "Comfort: " + std::to_string(static_cast<int>(info_.comfort_need)) + "%";
            DrawText(comfort_text.c_str(), x, y, 13, get_need_color(info_.comfort_need));
            y += 18;
        
            // Shopping
            std::string shopping_text = "Shopping: " + std::to_string(static_cast<int>(info_.shopping_need)) + "%";
            DrawText(shopping_text.c_str(), x, y, 13, get_need_color(info_.shopping_need));
            y += 23;
        }
    
        // Satisfaction (show for all non-staff)
        if (!info_.is_staff) {
            const std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(info_.satisfaction) + 
                                       " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%";
            DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
        }
    }

    std::string PersonWindow::GetSatisfactionEmoji(const float satisfaction) {
        if (satisfaction >= 80) return ":)";
        if (satisfaction >= 60) return ":|";
        if (satisfaction >= 40) return ":/";
        return ":(";
    }

    // ElevatorWindow implementation
    ElevatorWindow::ElevatorWindow(const ElevatorInfo& info)
        : UIWindow("Elevator Info", 250, 150 + (info.queue.size() * 20))
          , info_(info) {
    }

    void ElevatorWindow::Update(const ElevatorInfo& info) {
        info_ = info;
        title_ = "ELEVATOR #" + std::to_string(info.id);
        height_ = 150 + (static_cast<int>(info.queue.size()) * 20);
    }

    void ElevatorWindow::Render() {
        RenderFrame(PURPLE);
        RenderCloseButton();

        const int x = x_ + PADDING;
        int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // Current floor and direction
        const std::string current = "Current Floor: " + std::to_string(info_.current_floor) + 
                              " " + info_.direction;
        DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Occupancy
        const std::string occupancy = "Occupancy: " + std::to_string(info_.occupancy) + 
                                "/" + std::to_string(info_.max_occupancy);
        DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Next stop
        const std::string next = "Next Stop: Floor " + std::to_string(info_.next_stop);
        DrawText(next.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Queue length
        const std::string queue = "Queue Length: " + std::to_string(info_.queue.size());
        DrawText(queue.c_str(), x, y, 14, LIGHTGRAY);
        y += 20;
    
        // Queue details
        for (const auto& [floor, waiting] : info_.queue) {
            std::string queue_item = "- Floor " + std::to_string(floor) + ": " + 
                                     std::to_string(waiting) + " waiting";
            DrawText(queue_item.c_str(), x + 10, y, 12, GRAY);
            y += 20;
        }
    }

}
