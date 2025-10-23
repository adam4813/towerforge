#pragma once

#include "ui/ui_window.h"
#include "ui/hud.h"
#include "ui/stat_item.h"
#include "ui/alert_bar.h"
#include "ui/section_header.h"
#include "ui/icon_button.h"
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>

namespace towerforge::ui {

    /**
 * @brief Window for displaying facility information
 */
    class FacilityWindow : public UIWindow {
    public:
        FacilityWindow(const FacilityInfo& info);
        void Render() override;
        void Update(const FacilityInfo& info);
    
    private:
        FacilityInfo info_;

        // Component members
        std::unique_ptr<StatItem> occupancy_stat_;
        std::unique_ptr<StatItem> revenue_stat_;
        std::unique_ptr<StatItem> satisfaction_stat_;
        std::unique_ptr<StatItem> tenants_stat_;
        std::unique_ptr<SectionHeader> status_header_;
        std::unique_ptr<StatItem> cleanliness_state_stat_;
        std::unique_ptr<StatItem> cleanliness_stat_;
        std::unique_ptr<StatItem> maintenance_stat_;
        std::unique_ptr<StatItem> maintenance_state_stat_;
        std::unique_ptr<AlertBar> fire_alert_;
        std::unique_ptr<AlertBar> security_alert_;
        std::unique_ptr<SectionHeader> adjacency_header_;
        std::vector<std::unique_ptr<StatItem>> adjacency_items_;
        std::unique_ptr<IconButton> demolish_button_;
        std::unique_ptr<IconButton> upgrade_button_;
        std::unique_ptr<IconButton> repair_button_;

        void BuildComponents();
        void UpdateComponentValues();
        static std::string GetSatisfactionEmoji(float satisfaction);
    };

    /**
 * @brief Window for displaying person information
 */
    class PersonWindow : public UIWindow {
    public:
        PersonWindow(const PersonInfo& info);
        void Render() override;
        void Update(const PersonInfo& info);
    
    private:
        PersonInfo info_;

        // Component members
        std::unique_ptr<StatItem> type_stat_;
        std::unique_ptr<StatItem> archetype_stat_;
        std::unique_ptr<SectionHeader> staff_header_;
        std::unique_ptr<StatItem> role_stat_;
        std::unique_ptr<StatItem> duty_stat_;
        std::unique_ptr<StatItem> shift_stat_;
        std::unique_ptr<StatItem> status_stat_;
        std::unique_ptr<StatItem> state_stat_;
        std::unique_ptr<StatItem> current_floor_stat_;
        std::unique_ptr<StatItem> dest_floor_stat_;
        std::unique_ptr<StatItem> wait_time_stat_;
        std::unique_ptr<SectionHeader> needs_header_;
        std::unique_ptr<StatItem> hunger_stat_;
        std::unique_ptr<StatItem> entertainment_stat_;
        std::unique_ptr<StatItem> comfort_stat_;
        std::unique_ptr<StatItem> shopping_stat_;
        std::unique_ptr<StatItem> satisfaction_stat_;

        void BuildComponents();
        void UpdateComponentValues();
        static std::string GetSatisfactionEmoji(float satisfaction);
    };

    /**
 * @brief Window for displaying elevator information
 */
    class ElevatorWindow : public UIWindow {
    public:
        ElevatorWindow(const ElevatorInfo& info);
        void Render() override;
        void Update(const ElevatorInfo& info);
    
    private:
        ElevatorInfo info_;

        // Component members
        std::unique_ptr<StatItem> current_floor_stat_;
        std::unique_ptr<StatItem> occupancy_stat_;
        std::unique_ptr<StatItem> next_stop_stat_;
        std::unique_ptr<StatItem> queue_length_stat_;
        std::vector<std::unique_ptr<StatItem>> queue_items_;

        void BuildComponents();
        void UpdateComponentValues();
    };

}
