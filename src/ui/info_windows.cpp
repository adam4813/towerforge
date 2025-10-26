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
        const float padding = static_cast<float>(WindowChrome::GetPadding());
        float y = 0;
        
        // Stats section
        auto occupancy = std::make_unique<StatItem>(padding, y, "Occupancy:");
        occupancy_stat_ = occupancy.get();
        AddChild(std::move(occupancy));
        y += 20;
        
        auto revenue = std::make_unique<StatItem>(padding, y, "Revenue:");
        revenue_stat_ = revenue.get();
        AddChild(std::move(revenue));
        y += 20;
        
        auto satisfaction = std::make_unique<StatItem>(padding, y, "Satisfaction:");
        satisfaction_stat_ = satisfaction.get();
        AddChild(std::move(satisfaction));
        y += 20;
        
        auto tenants = std::make_unique<StatItem>(padding, y, "Tenants:");
        tenants_stat_ = tenants.get();
        AddChild(std::move(tenants));
        y += 25;
        
        // Status section
        auto status_header = std::make_unique<SectionHeader>(padding, y, "--- Facility Status ---");
        status_header_ = status_header.get();
        AddChild(std::move(status_header));
        y += 20;
        
        auto cleanliness_state = std::make_unique<StatItem>(padding, y, "Status:");
        cleanliness_state_stat_ = cleanliness_state.get();
        AddChild(std::move(cleanliness_state));
        y += 20;
        
        auto cleanliness = std::make_unique<StatItem>(padding, y, "Cleanliness:");
        cleanliness_stat_ = cleanliness.get();
        AddChild(std::move(cleanliness));
        y += 20;
        
        auto maintenance = std::make_unique<StatItem>(padding, y, "Maintenance:");
        maintenance_stat_ = maintenance.get();
        AddChild(std::move(maintenance));
        y += 20;
        
        auto maintenance_state = std::make_unique<StatItem>(padding, y, "Status:");
        maintenance_state_stat_ = maintenance_state.get();
        AddChild(std::move(maintenance_state));
        y += 20;
        
        // Alerts (not UIElements, still manual)
        fire_alert_ = std::make_unique<AlertBar>("! FIRE - Firefighter needed !", RED, y);
        fire_alert_->SetVisible(false);
        y += 20;
        
        security_alert_ = std::make_unique<AlertBar>("! Security Issue - Guard needed !", ORANGE, y);
        security_alert_->SetVisible(false);
        y += 20;
        
        // Adjacency section
        auto adjacency_header = std::make_unique<SectionHeader>(padding, y, "--- Adjacency Effects ---", GOLD);
        adjacency_header_ = adjacency_header.get();
        AddChild(std::move(adjacency_header));
        
        // Create button panel
        auto button_panel = std::make_unique<Panel>(padding, 0, 230, 60, BLANK, BLANK);
        button_panel_ = button_panel.get();
        
        // Buttons - added as children to button_panel
        auto demolish = std::make_unique<IconButton>(0, 0, 100, 25, "[Demolish]", DARKGRAY, RED);
        demolish->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        demolish_button_ = demolish.get();
        button_panel_->AddChild(std::move(demolish));
        
        auto upgrade = std::make_unique<IconButton>(110, 0, 100, 25, "[Upgrade]", DARKGRAY, YELLOW);
        upgrade->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        upgrade_button_ = upgrade.get();
        button_panel_->AddChild(std::move(upgrade));
        
        auto repair = std::make_unique<IconButton>(0, 30, 210, 25, "[Repair Now]", ORANGE, BLACK);
        repair->SetClickCallback([]() {
            // TODO: Wire to game logic
        });
        repair_button_ = repair.get();
        button_panel_->AddChild(std::move(repair));
        
        AddChild(std::move(button_panel));
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
        
        // Update adjacency effects - remove old dynamic children
        for (auto* item : adjacency_items_) {
            RemoveChild(item);
        }
        adjacency_items_.clear();
        
        // Add new dynamic children for adjacency effects
        const float padding = static_cast<float>(WindowChrome::GetPadding());
        float adj_y_base = adjacency_header_->GetRelativeBounds().y + 25;
        float adj_y = 0;
        
        for (const auto& effect : info_.adjacency_effects) {
            auto item = std::make_unique<StatItem>(padding, adj_y_base + adj_y, "");
            
            Color effect_color = LIGHTGRAY;
            if (effect.find("+") != std::string::npos) {
                effect_color = GREEN;
            } else if (effect.find("-") != std::string::npos) {
                effect_color = ORANGE;
            }
            
            item->SetValue(effect, effect_color);
            adjacency_items_.push_back(item.get());
            AddChild(std::move(item));
            adj_y += 18;
        }
        
        // Update repair button
        if (info_.needs_repair || info_.is_broken) {
            const Color repair_color = info_.is_broken ? RED : ORANGE;
            const std::string button_text = info_.is_broken ? "[Emergency Repair]" : "[Repair Now]";
            repair_button_->SetBackgroundColor(repair_color);
            repair_button_->SetTextColor(BLACK);
            repair_button_->SetLabel(button_text);
            repair_button_->SetEnabled(true);
        } else {
            repair_button_->SetEnabled(false);
            repair_button_->SetBackgroundColor(ColorAlpha(BLANK, 0.0f));
            repair_button_->SetTextColor(ColorAlpha(BLANK, 0.0f));
            repair_button_->SetBorderColor(ColorAlpha(BLANK, 0.0f));
        }
    }

    void FacilityWindow::Render() const {
        // Call parent to render chrome + content
        UIWindow::Render();
    }
    
    void FacilityWindow::RenderContent() const {
        // Render all children automatically
        Panel::Render();
        
        // Manual rendering only for non-UIElement components (AlertBar)
        const Rectangle bounds = GetAbsoluteBounds();
        const int x = static_cast<int>(bounds.x) + WindowChrome::GetPadding();
        const int y_base = static_cast<int>(bounds.y) + WindowChrome::GetTitleBarHeight() 
                         + WindowChrome::GetPadding();
        
        fire_alert_->Render(x, y_base);
        security_alert_->Render(x, y_base);
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
        const float padding = static_cast<float>(WindowChrome::GetPadding());
        float y = 0;
        
        // Basic info
        auto type_stat = std::make_unique<StatItem>(padding, y, "Type:");
        type_stat_ = type_stat.get();
        AddChild(std::move(type_stat));
        y += 20;
        
        auto archetype_stat = std::make_unique<StatItem>(padding, y, "Profile:");
        archetype_stat_ = archetype_stat.get();
        AddChild(std::move(archetype_stat));
        y += 20;
        
        // Staff section
        auto staff_header = std::make_unique<SectionHeader>(padding, y, "--- Staff Info ---", GOLD);
        staff_header_ = staff_header.get();
        AddChild(std::move(staff_header));
        y += 20;
        
        auto role_stat = std::make_unique<StatItem>(padding, y, "Role:");
        role_stat_ = role_stat.get();
        AddChild(std::move(role_stat));
        y += 20;
        
        auto duty_stat = std::make_unique<StatItem>(padding, y, "Status:");
        duty_stat_ = duty_stat.get();
        AddChild(std::move(duty_stat));
        y += 20;
        
        auto shift_stat = std::make_unique<StatItem>(padding, y, "Shift:");
        shift_stat_ = shift_stat.get();
        AddChild(std::move(shift_stat));
        y += 25;
        
        // Status info
        auto status_stat = std::make_unique<StatItem>(padding, y, "Status:");
        status_stat_ = status_stat.get();
        AddChild(std::move(status_stat));
        y += 20;
        
        auto state_stat = std::make_unique<StatItem>(padding, y, "State:");
        state_stat_ = state_stat.get();
        AddChild(std::move(state_stat));
        y += 20;
        
        auto current_floor_stat = std::make_unique<StatItem>(padding, y, "Current:");
        current_floor_stat_ = current_floor_stat.get();
        AddChild(std::move(current_floor_stat));
        y += 20;
        
        auto dest_floor_stat = std::make_unique<StatItem>(padding, y, "Destination:");
        dest_floor_stat_ = dest_floor_stat.get();
        AddChild(std::move(dest_floor_stat));
        y += 20;
        
        auto wait_time_stat = std::make_unique<StatItem>(padding, y, "Wait Time:");
        wait_time_stat_ = wait_time_stat.get();
        AddChild(std::move(wait_time_stat));
        y += 20;
        
        // Needs section
        auto needs_header = std::make_unique<SectionHeader>(padding, y, "--- Visitor Needs ---", YELLOW);
        needs_header_ = needs_header.get();
        AddChild(std::move(needs_header));
        y += 20;
        
        auto hunger_stat = std::make_unique<StatItem>(padding, y, "Hunger:");
        hunger_stat_ = hunger_stat.get();
        AddChild(std::move(hunger_stat));
        y += 18;
        
        auto entertainment_stat = std::make_unique<StatItem>(padding, y, "Entertainment:");
        entertainment_stat_ = entertainment_stat.get();
        AddChild(std::move(entertainment_stat));
        y += 18;
        
        auto comfort_stat = std::make_unique<StatItem>(padding, y, "Comfort:");
        comfort_stat_ = comfort_stat.get();
        AddChild(std::move(comfort_stat));
        y += 18;
        
        auto shopping_stat = std::make_unique<StatItem>(padding, y, "Shopping:");
        shopping_stat_ = shopping_stat.get();
        AddChild(std::move(shopping_stat));
        y += 23;
        
        // Satisfaction
        auto satisfaction_stat = std::make_unique<StatItem>(padding, y, "Satisfaction:");
        satisfaction_stat_ = satisfaction_stat.get();
        AddChild(std::move(satisfaction_stat));
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
        SetTitle(info.name);
        UpdateComponentValues();
    }

    void PersonWindow::Render() const {
        UIWindow::Render();
    }
    
    void PersonWindow::RenderContent() const {
        // Render all children automatically
        Panel::Render();
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
        const float padding = static_cast<float>(WindowChrome::GetPadding());
        float y = 0;
        
        auto current_floor = std::make_unique<StatItem>(padding, y, "Current Floor:");
        current_floor_stat_ = current_floor.get();
        AddChild(std::move(current_floor));
        y += 20;
        
        auto occupancy = std::make_unique<StatItem>(padding, y, "Occupancy:");
        occupancy_stat_ = occupancy.get();
        AddChild(std::move(occupancy));
        y += 20;
        
        auto next_stop = std::make_unique<StatItem>(padding, y, "Next Stop:");
        next_stop_stat_ = next_stop.get();
        AddChild(std::move(next_stop));
        y += 20;
        
        auto queue_length = std::make_unique<StatItem>(padding, y, "Queue Length:");
        queue_length_stat_ = queue_length.get();
        AddChild(std::move(queue_length));
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
        
        // Update dynamic queue items - remove old children
        for (auto* item : queue_items_) {
            RemoveChild(item);
        }
        queue_items_.clear();
        
        // Add new dynamic children for queue
        const float padding = static_cast<float>(WindowChrome::GetPadding()) + 10;  // Indent queue items
        float queue_y = 80;
        for (const auto& [floor, waiting] : info_.queue) {
            auto item = std::make_unique<StatItem>(padding, queue_y, "");
            const std::string queue_text = "- Floor " + std::to_string(floor) + ": " + std::to_string(waiting) + " waiting";
            item->SetValue(queue_text, GRAY);
            queue_items_.push_back(item.get());
            AddChild(std::move(item));
            queue_y += 20;
        }
    }

    void ElevatorWindow::Update(const ElevatorInfo& info) {
        info_ = info;
        SetTitle("ELEVATOR #" + std::to_string(info.id));
        
        // Update window size for queue
        const float new_height = 150 + (static_cast<float>(info.queue.size()) * 20);
        SetSize(GetRelativeBounds().width, new_height);
        
        UpdateComponentValues();
    }

    void ElevatorWindow::Render() const {
        UIWindow::Render();
    }
    
    void ElevatorWindow::RenderContent() const {
        // Render all children automatically
        Panel::Render();
    }

}
