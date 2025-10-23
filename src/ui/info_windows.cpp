#include "ui/info_windows.h"

namespace towerforge::ui {

    // FacilityWindow implementation
    FacilityWindow::FacilityWindow(const FacilityInfo& info)
        : UIWindow("Facility Info", 250, 300 + static_cast<int>(info.adjacency_effects.size() * 18))
          , info_(info) {
        BuildComponents();
        UpdateComponentValues();
    }

    void FacilityWindow::BuildComponents() {
        int y = 0;
        
        // Stats section
        occupancy_stat_ = std::make_unique<StatItem>("Occupancy:", y);
        y += 20;
        
        revenue_stat_ = std::make_unique<StatItem>("Revenue:", y);
        y += 20;
        
        satisfaction_stat_ = std::make_unique<StatItem>("Satisfaction:", y);
        y += 20;
        
        tenants_stat_ = std::make_unique<StatItem>("Tenants:", y);
        y += 25;
        
        // Status section
        status_header_ = std::make_unique<SectionHeader>("--- Facility Status ---", YELLOW, y);
        y += 20;
        
        cleanliness_state_stat_ = std::make_unique<StatItem>("Status:", y);
        y += 20;
        
        cleanliness_stat_ = std::make_unique<StatItem>("Cleanliness:", y);
        y += 20;
        
        maintenance_stat_ = std::make_unique<StatItem>("Maintenance:", y);
        y += 20;
        
        maintenance_state_stat_ = std::make_unique<StatItem>("Status:", y);
        y += 20;
        
        // Alerts
        fire_alert_ = std::make_unique<AlertBar>("! FIRE - Firefighter needed !", RED, y);
        fire_alert_->SetVisible(false);
        y += 20;
        
        security_alert_ = std::make_unique<AlertBar>("! Security Issue - Guard needed !", ORANGE, y);
        security_alert_->SetVisible(false);
        y += 20;
        
        // Adjacency section
        adjacency_header_ = std::make_unique<SectionHeader>("--- Adjacency Effects ---", GOLD, y);
        
        // Buttons
        demolish_button_ = std::make_unique<IconButton>("[Demolish]", 100, 25, DARKGRAY, RED, 0, 0);
        demolish_button_->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        
        upgrade_button_ = std::make_unique<IconButton>("[Upgrade]", 100, 25, DARKGRAY, YELLOW, 110, 0);
        upgrade_button_->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        
        repair_button_ = std::make_unique<IconButton>("[Repair Now]", 210, 25, ORANGE, BLACK, 0, 0);
        repair_button_->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        repair_button_->SetVisible(false);
    }

    void FacilityWindow::Update(const FacilityInfo& info) {
        info_ = info;
        title_ = info.type + " - Floor " + std::to_string(info.floor);
        
        // Recalculate height based on content
        int height = 300;
        if (!info.adjacency_effects.empty()) {
            height += 30 + static_cast<int>(info.adjacency_effects.size() * 18);
        }
        height_ = height;
        
        UpdateComponentValues();
    }

    void FacilityWindow::UpdateComponentValues() {
        // Update occupancy
        occupancy_stat_->SetValue(
            std::to_string(info_.occupancy) + "/" + std::to_string(info_.max_occupancy),
            LIGHTGRAY
        );
        
        // Update revenue
        std::stringstream revenue_ss;
        revenue_ss << "$" << std::fixed << std::setprecision(0) << info_.revenue << "/hr";
        revenue_stat_->SetValue(revenue_ss.str(), GREEN);
        
        // Update satisfaction
        const std::string sat_emoji = GetSatisfactionEmoji(info_.satisfaction);
        satisfaction_stat_->SetValue(
            sat_emoji + " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%",
            LIGHTGRAY
        );
        
        // Update tenants
        tenants_stat_->SetValue(std::to_string(info_.tenant_count) + " workers", LIGHTGRAY);
        
        // Update cleanliness state
        if (!info_.cleanliness_state.empty()) {
            Color state_color = GREEN;
            if (info_.cleanliness_state == "Dirty") {
                state_color = RED;
            } else if (info_.cleanliness_state == "Needs Cleaning") {
                state_color = YELLOW;
            }
            cleanliness_state_stat_->SetValue(info_.cleanliness_state, state_color);
        }
        
        // Update cleanliness
        const std::string clean_text = info_.cleanliness_rating + " (" + 
                                      std::to_string(static_cast<int>(info_.cleanliness)) + "%)";
        const Color clean_color = info_.cleanliness >= 70.0f ? GREEN : 
                                 (info_.cleanliness >= 50.0f ? YELLOW : RED);
        cleanliness_stat_->SetValue(clean_text, clean_color);
        
        // Update maintenance
        const std::string maint_text = info_.maintenance_rating + " (" + 
                                      std::to_string(static_cast<int>(info_.maintenance_level)) + "%)";
        const Color maint_color = info_.maintenance_level >= 70.0f ? GREEN : 
                                 (info_.maintenance_level >= 50.0f ? YELLOW : RED);
        maintenance_stat_->SetValue(maint_text, maint_color);
        
        // Update maintenance state
        if (!info_.maintenance_state.empty()) {
            Color state_color = GREEN;
            if (info_.is_broken) {
                state_color = RED;
            } else if (info_.needs_repair) {
                state_color = ORANGE;
            }
            maintenance_state_stat_->SetValue(info_.maintenance_state, state_color);
        }
        
        // Update alerts
        fire_alert_->SetVisible(info_.has_fire);
        security_alert_->SetVisible(info_.has_security_issue);
        
        // Update adjacency effects
        adjacency_items_.clear();
        int adj_y = 0;
        for (const auto& effect : info_.adjacency_effects) {
            auto item = std::make_unique<StatItem>("", adj_y);
            
            Color effect_color = LIGHTGRAY;
            if (effect.find("+") != std::string::npos) {
                effect_color = GREEN;
            } else if (effect.find("-") != std::string::npos) {
                effect_color = ORANGE;
            }
            
            item->SetValue(effect, effect_color);
            adjacency_items_.push_back(std::move(item));
            adj_y += 18;
        }
        
        // Update repair button
        if (info_.needs_repair || info_.is_broken) {
            repair_button_->SetVisible(true);
            const Color repair_color = info_.is_broken ? RED : ORANGE;
            const std::string button_text = info_.is_broken ? "[Emergency Repair]" : "[Repair Now]";
            repair_button_->SetColors(repair_color, BLACK);
            repair_button_->SetLabel(button_text);
        } else {
            repair_button_->SetVisible(false);
        }
    }

    void FacilityWindow::Render() {
        RenderFrame(SKYBLUE);
        RenderCloseButton();

        const int x = x_ + PADDING;
        const int y_base = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // Render all stat components
        occupancy_stat_->Render(x, y_base);
        revenue_stat_->Render(x, y_base);
        satisfaction_stat_->Render(x, y_base);
        tenants_stat_->Render(x, y_base);
        
        // Render status section
        status_header_->Render(x, y_base);
        if (!info_.cleanliness_state.empty()) {
            cleanliness_state_stat_->Render(x, y_base);
        }
        cleanliness_stat_->Render(x, y_base);
        maintenance_stat_->Render(x, y_base);
        if (!info_.maintenance_state.empty()) {
            maintenance_state_stat_->Render(x, y_base);
        }
        
        // Render alerts
        fire_alert_->Render(x, y_base);
        security_alert_->Render(x, y_base);
        
        // Render adjacency section
        if (!info_.adjacency_effects.empty()) {
            int adj_y_offset = fire_alert_->IsVisible() ? 20 : 0;
            adj_y_offset += security_alert_->IsVisible() ? 20 : 0;
            adjacency_header_->SetPosition(200 + adj_y_offset);
            adjacency_header_->Render(x, y_base);
            
            for (const auto& item : adjacency_items_) {
                item->Render(x, y_base);
            }
        }
        
        // Render buttons
        int button_y_offset = 240;
        if (!info_.adjacency_effects.empty()) {
            button_y_offset += 30 + static_cast<int>(info_.adjacency_effects.size() * 18);
        }
        
        demolish_button_->SetPosition(0, button_y_offset);
        demolish_button_->Render(x, y_base);
        upgrade_button_->SetPosition(110, button_y_offset);
        upgrade_button_->Render(x, y_base);
        
        if (repair_button_->IsVisible()) {
            repair_button_->SetPosition(0, button_y_offset + 30);
            repair_button_->Render(x, y_base);
        }
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
        BuildComponents();
        UpdateComponentValues();
    }

    void PersonWindow::BuildComponents() {
        int y = 0;
        
        // Basic info
        type_stat_ = std::make_unique<StatItem>("Type:", y);
        y += 20;
        
        archetype_stat_ = std::make_unique<StatItem>("Profile:", y);
        y += 20;
        
        // Staff section
        staff_header_ = std::make_unique<SectionHeader>("--- Staff Info ---", GOLD, y);
        y += 20;
        
        role_stat_ = std::make_unique<StatItem>("Role:", y);
        y += 20;
        
        duty_stat_ = std::make_unique<StatItem>("Status:", y);
        y += 20;
        
        shift_stat_ = std::make_unique<StatItem>("Shift:", y);
        y += 25;
        
        // Status info
        status_stat_ = std::make_unique<StatItem>("Status:", y);
        y += 20;
        
        state_stat_ = std::make_unique<StatItem>("State:", y);
        y += 20;
        
        current_floor_stat_ = std::make_unique<StatItem>("Current:", y);
        y += 20;
        
        dest_floor_stat_ = std::make_unique<StatItem>("Destination:", y);
        y += 20;
        
        wait_time_stat_ = std::make_unique<StatItem>("Wait Time:", y);
        y += 20;
        
        // Needs section
        needs_header_ = std::make_unique<SectionHeader>("--- Visitor Needs ---", YELLOW, y);
        y += 20;
        
        hunger_stat_ = std::make_unique<StatItem>("Hunger:", y);
        y += 18;
        
        entertainment_stat_ = std::make_unique<StatItem>("Entertainment:", y);
        y += 18;
        
        comfort_stat_ = std::make_unique<StatItem>("Comfort:", y);
        y += 18;
        
        shopping_stat_ = std::make_unique<StatItem>("Shopping:", y);
        y += 23;
        
        // Satisfaction
        satisfaction_stat_ = std::make_unique<StatItem>("Satisfaction:", y);
    }

    void PersonWindow::UpdateComponentValues() {
        // Update type
        type_stat_->SetValue(info_.npc_type, SKYBLUE);
        
        // Update archetype (if applicable)
        if (info_.has_needs && !info_.visitor_archetype.empty()) {
            archetype_stat_->SetValue(info_.visitor_archetype, GOLD);
        }
        
        // Update staff info
        if (info_.is_staff) {
            role_stat_->SetValue(info_.staff_role, LIGHTGRAY);
            duty_stat_->SetValue(info_.on_duty ? "On Duty" : "Off Duty", info_.on_duty ? GREEN : GRAY);
            shift_stat_->SetValue(info_.shift_hours, LIGHTGRAY);
        }
        
        // Update status
        status_stat_->SetValue(info_.status, GOLD);
        state_stat_->SetValue(info_.state, LIGHTGRAY);
        current_floor_stat_->SetValue("Floor " + std::to_string(info_.current_floor), LIGHTGRAY);
        dest_floor_stat_->SetValue("Floor " + std::to_string(info_.destination_floor), LIGHTGRAY);
        
        // Update wait time
        if (!info_.is_staff || info_.wait_time > 0) {
            std::stringstream wait_ss;
            wait_ss << std::fixed << std::setprecision(0) << info_.wait_time << "s";
            wait_time_stat_->SetValue(wait_ss.str(), info_.wait_time > 30 ? RED : LIGHTGRAY);
        }
        
        // Update needs
        if (info_.has_needs) {
            auto get_need_color = [](const float need) {
                if (need < 30.0f) return GREEN;
                if (need < 60.0f) return YELLOW;
                return RED;
            };
            
            hunger_stat_->SetValue(std::to_string(static_cast<int>(info_.hunger_need)) + "%", 
                                  get_need_color(info_.hunger_need));
            entertainment_stat_->SetValue(std::to_string(static_cast<int>(info_.entertainment_need)) + "%", 
                                         get_need_color(info_.entertainment_need));
            comfort_stat_->SetValue(std::to_string(static_cast<int>(info_.comfort_need)) + "%", 
                                   get_need_color(info_.comfort_need));
            shopping_stat_->SetValue(std::to_string(static_cast<int>(info_.shopping_need)) + "%", 
                                    get_need_color(info_.shopping_need));
        }
        
        // Update satisfaction
        if (!info_.is_staff) {
            const std::string sat_emoji = GetSatisfactionEmoji(info_.satisfaction);
            satisfaction_stat_->SetValue(sat_emoji + " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%", 
                                        LIGHTGRAY);
        }
    }

    void PersonWindow::Update(const PersonInfo& info) {
        info_ = info;
        title_ = info.name;
        UpdateComponentValues();
    }

    void PersonWindow::Render() {
        RenderFrame(YELLOW);
        RenderCloseButton();

        const int x = x_ + PADDING;
        const int y_base = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // Render basic info
        type_stat_->Render(x, y_base);
        
        if (info_.has_needs && !info_.visitor_archetype.empty()) {
            archetype_stat_->Render(x, y_base);
        }
        
        // Render staff section
        if (info_.is_staff) {
            staff_header_->Render(x, y_base);
            role_stat_->Render(x, y_base);
            duty_stat_->Render(x, y_base);
            shift_stat_->Render(x, y_base);
        }
        
        // Render status info
        status_stat_->Render(x, y_base);
        state_stat_->Render(x, y_base);
        current_floor_stat_->Render(x, y_base);
        dest_floor_stat_->Render(x, y_base);
        
        if (!info_.is_staff || info_.wait_time > 0) {
            wait_time_stat_->Render(x, y_base);
        }
        
        // Render needs section
        if (info_.has_needs) {
            needs_header_->Render(x, y_base);
            hunger_stat_->Render(x, y_base);
            entertainment_stat_->Render(x, y_base);
            comfort_stat_->Render(x, y_base);
            shopping_stat_->Render(x, y_base);
        }
        
        // Render satisfaction
        if (!info_.is_staff) {
            satisfaction_stat_->Render(x, y_base);
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
        BuildComponents();
        UpdateComponentValues();
    }

    void ElevatorWindow::BuildComponents() {
        int y = 0;
        
        current_floor_stat_ = std::make_unique<StatItem>("Current Floor:", y);
        y += 20;
        
        occupancy_stat_ = std::make_unique<StatItem>("Occupancy:", y);
        y += 20;
        
        next_stop_stat_ = std::make_unique<StatItem>("Next Stop:", y);
        y += 20;
        
        queue_length_stat_ = std::make_unique<StatItem>("Queue Length:", y);
        y += 20;
    }

    void ElevatorWindow::UpdateComponentValues() {
        // Update current floor and direction
        const std::string current = std::to_string(info_.current_floor) + " " + info_.direction;
        current_floor_stat_->SetValue(current, LIGHTGRAY);
        
        // Update occupancy
        const std::string occupancy = std::to_string(info_.occupancy) + "/" + std::to_string(info_.max_occupancy);
        occupancy_stat_->SetValue(occupancy, LIGHTGRAY);
        
        // Update next stop
        const std::string next_stop = "Floor " + std::to_string(info_.next_stop);
        next_stop_stat_->SetValue(next_stop, LIGHTGRAY);
        
        // Update queue length
        queue_length_stat_->SetValue(std::to_string(info_.queue.size()), LIGHTGRAY);
        
        // Build queue items
        queue_items_.clear();
        int queue_y = 80;
        for (const auto& [floor, waiting] : info_.queue) {
            auto item = std::make_unique<StatItem>("", queue_y);
            const std::string queue_text = "- Floor " + std::to_string(floor) + ": " + std::to_string(waiting) + " waiting";
            item->SetValue(queue_text, GRAY);
            queue_items_.push_back(std::move(item));
            queue_y += 20;
        }
    }

    void ElevatorWindow::Update(const ElevatorInfo& info) {
        info_ = info;
        title_ = "ELEVATOR #" + std::to_string(info.id);
        height_ = 150 + (static_cast<int>(info.queue.size()) * 20);
        UpdateComponentValues();
    }

    void ElevatorWindow::Render() {
        RenderFrame(PURPLE);
        RenderCloseButton();

        const int x = x_ + PADDING;
        const int y_base = y_ + TITLE_BAR_HEIGHT + PADDING;
    
        // Render stats
        current_floor_stat_->Render(x, y_base);
        occupancy_stat_->Render(x, y_base);
        next_stop_stat_->Render(x, y_base);
        queue_length_stat_->Render(x, y_base);
        
        // Render queue items
        for (const auto& item : queue_items_) {
            item->Render(x + 10, y_base);
        }
    }

}
