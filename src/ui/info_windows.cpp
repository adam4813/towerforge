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
        float y = 0;
        
        // Stats section
        occupancy_stat_ = std::make_unique<StatItem>(0, y, "Occupancy:");
        y += 20;
        
        revenue_stat_ = std::make_unique<StatItem>(0, y, "Revenue:");
        y += 20;
        
        satisfaction_stat_ = std::make_unique<StatItem>(0, y, "Satisfaction:");
        y += 20;
        
        tenants_stat_ = std::make_unique<StatItem>(0, y, "Tenants:");
        y += 25;
        
        // Status section
        status_header_ = std::make_unique<SectionHeader>(0, y, "--- Facility Status ---", YELLOW);
        y += 20;
        
        cleanliness_state_stat_ = std::make_unique<StatItem>(0, y, "Status:");
        y += 20;
        
        cleanliness_stat_ = std::make_unique<StatItem>(0, y, "Cleanliness:");
        y += 20;
        
        maintenance_stat_ = std::make_unique<StatItem>(0, y, "Maintenance:");
        y += 20;
        
        maintenance_state_stat_ = std::make_unique<StatItem>(0, y, "Status:");
        y += 20;
        
        // Alerts
        fire_alert_ = std::make_unique<AlertBar>("! FIRE - Firefighter needed !", RED, y);
        fire_alert_->SetVisible(false);
        y += 20;
        
        security_alert_ = std::make_unique<AlertBar>("! Security Issue - Guard needed !", ORANGE, y);
        security_alert_->SetVisible(false);
        y += 20;
        
        // Adjacency section
        adjacency_header_ = std::make_unique<SectionHeader>(0, y, "--- Adjacency Effects ---", GOLD);
        
        // Create button panel for proper UI hierarchy
        button_panel_ = std::make_unique<Panel>(0, 0, 230, 60, BLANK, BLANK);
        
        // Buttons - now added as children to button_panel
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
        float adj_y = 0;
        for (const auto& effect : info_.adjacency_effects) {
            auto item = std::make_unique<StatItem>(0, adj_y, "");
            
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
        
        // Update repair button (now uses Button interface)
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
        const Rectangle bounds = GetAbsoluteBounds();
        const float x = bounds.x + WindowChrome::GetPadding();
        const float y_base = bounds.y + WindowChrome::GetTitleBarHeight() + WindowChrome::GetPadding();
    
        // Helper lambda to position and render UIElement-based components
        auto renderAt = [x, y_base](UIElement* elem) {
            elem->SetRelativePosition(x, y_base + elem->GetRelativeBounds().y);
            elem->Render();
        };
        
        // Render all stat components (now UIElements)
        renderAt(occupancy_stat_.get());
        renderAt(revenue_stat_.get());
        renderAt(satisfaction_stat_.get());
        renderAt(tenants_stat_.get());
        
        // Render status section
        renderAt(status_header_.get());
        
        if (!info_.cleanliness_state.empty()) {
            renderAt(cleanliness_state_stat_.get());
        }
        
        renderAt(cleanliness_stat_.get());
        renderAt(maintenance_stat_.get());
        
        if (!info_.maintenance_state.empty()) {
            renderAt(maintenance_state_stat_.get());
        }
        
        // Render alerts
        const int x_int = static_cast<int>(x);
        const int y_base_int = static_cast<int>(y_base);
        fire_alert_->Render(x_int, y_base_int);
        security_alert_->Render(x_int, y_base_int);
        
        // Render adjacency section
        if (!info_.adjacency_effects.empty()) {
            const int adj_y_offset = (fire_alert_->IsVisible() ? 20 : 0) + 
                                    (security_alert_->IsVisible() ? 20 : 0);
            adjacency_header_->SetRelativePosition(x, y_base + 200 + adj_y_offset);
            adjacency_header_->Render();
            
            for (const auto& item : adjacency_items_) {
                renderAt(item.get());
            }
        }
        
        // Render button panel (now uses Panel hierarchy)
        int button_y_offset = 240;
        if (!info_.adjacency_effects.empty()) {
            button_y_offset += 30 + static_cast<int>(info_.adjacency_effects.size() * 18);
        }
        
        button_panel_->SetRelativePosition(x, y_base + button_y_offset);
        button_panel_->Render();
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
        float y = 0;
        
        // Basic info
        type_stat_ = std::make_unique<StatItem>(0, y, "Type:");
        y += 20;
        
        archetype_stat_ = std::make_unique<StatItem>(0, y, "Profile:");
        y += 20;
        
        // Staff section
        staff_header_ = std::make_unique<SectionHeader>(0, y, "--- Staff Info ---", GOLD);
        y += 20;
        
        role_stat_ = std::make_unique<StatItem>(0, y, "Role:");
        y += 20;
        
        duty_stat_ = std::make_unique<StatItem>(0, y, "Status:");
        y += 20;
        
        shift_stat_ = std::make_unique<StatItem>(0, y, "Shift:");
        y += 25;
        
        // Status info
        status_stat_ = std::make_unique<StatItem>(0, y, "Status:");
        y += 20;
        
        state_stat_ = std::make_unique<StatItem>(0, y, "State:");
        y += 20;
        
        current_floor_stat_ = std::make_unique<StatItem>(0, y, "Current:");
        y += 20;
        
        dest_floor_stat_ = std::make_unique<StatItem>(0, y, "Destination:");
        y += 20;
        
        wait_time_stat_ = std::make_unique<StatItem>(0, y, "Wait Time:");
        y += 20;
        
        // Needs section
        needs_header_ = std::make_unique<SectionHeader>(0, y, "--- Visitor Needs ---", YELLOW);
        y += 20;
        
        hunger_stat_ = std::make_unique<StatItem>(0, y, "Hunger:");
        y += 18;
        
        entertainment_stat_ = std::make_unique<StatItem>(0, y, "Entertainment:");
        y += 18;
        
        comfort_stat_ = std::make_unique<StatItem>(0, y, "Comfort:");
        y += 18;
        
        shopping_stat_ = std::make_unique<StatItem>(0, y, "Shopping:");
        y += 23;
        
        // Satisfaction
        satisfaction_stat_ = std::make_unique<StatItem>(0, y, "Satisfaction:");
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
        const Rectangle bounds = GetAbsoluteBounds();
        const float x = bounds.x + WindowChrome::GetPadding();
        const float y_base = bounds.y + WindowChrome::GetTitleBarHeight() + WindowChrome::GetPadding();
    
        // Helper lambda to position and render UIElement-based components
        auto renderAt = [x, y_base](UIElement* elem) {
            elem->SetRelativePosition(x, y_base + elem->GetRelativeBounds().y);
            elem->Render();
        };
        
        // Render basic info
        renderAt(type_stat_.get());
        
        if (info_.has_needs && !info_.visitor_archetype.empty()) {
            renderAt(archetype_stat_.get());
        }
        
        // Render staff section
        if (info_.is_staff) {
            renderAt(staff_header_.get());
            renderAt(role_stat_.get());
            renderAt(duty_stat_.get());
            renderAt(shift_stat_.get());
        }
        
        // Render status info
        renderAt(status_stat_.get());
        renderAt(state_stat_.get());
        renderAt(current_floor_stat_.get());
        renderAt(dest_floor_stat_.get());
        
        if (!info_.is_staff || info_.wait_time > 0) {
            renderAt(wait_time_stat_.get());
        }
        
        // Render needs section
        if (info_.has_needs) {
            renderAt(needs_header_.get());
            renderAt(hunger_stat_.get());
            renderAt(entertainment_stat_.get());
            renderAt(comfort_stat_.get());
            renderAt(shopping_stat_.get());
        }
        
        // Render satisfaction
        if (!info_.is_staff) {
            renderAt(satisfaction_stat_.get());
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
        float y = 0;
        
        current_floor_stat_ = std::make_unique<StatItem>(0, y, "Current Floor:");
        y += 20;
        
        occupancy_stat_ = std::make_unique<StatItem>(0, y, "Occupancy:");
        y += 20;
        
        next_stop_stat_ = std::make_unique<StatItem>(0, y, "Next Stop:");
        y += 20;
        
        queue_length_stat_ = std::make_unique<StatItem>(0, y, "Queue Length:");
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
        float queue_y = 80;
        for (const auto& [floor, waiting] : info_.queue) {
            auto item = std::make_unique<StatItem>(0, queue_y, "");
            const std::string queue_text = "- Floor " + std::to_string(floor) + ": " + std::to_string(waiting) + " waiting";
            item->SetValue(queue_text, GRAY);
            queue_items_.push_back(std::move(item));
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
        const Rectangle bounds = GetAbsoluteBounds();
        const float x = bounds.x + WindowChrome::GetPadding();
        const float y_base = bounds.y + WindowChrome::GetTitleBarHeight() + WindowChrome::GetPadding();
    
        // Helper lambda to position and render UIElement-based components
        auto renderAt = [x, y_base](UIElement* elem) {
            elem->SetRelativePosition(x, y_base + elem->GetRelativeBounds().y);
            elem->Render();
        };
        
        // Render stats
        renderAt(current_floor_stat_.get());
        renderAt(occupancy_stat_.get());
        renderAt(next_stop_stat_.get());
        renderAt(queue_length_stat_.get());
        
        // Render queue items (with indent)
        for (const auto& item : queue_items_) {
            item->SetRelativePosition(x + 10, y_base + item->GetRelativeBounds().y);
            item->Render();
        }
    }

}
