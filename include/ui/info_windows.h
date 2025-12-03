#pragma once

#include "ui/ui_window.h"
#include "ui/ui_element.h"
#include "hud/hud.h"
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
        void Render() const override;
        void Update(const FacilityInfo& info);
    
    protected:
        void RenderContent() const override;
        
    private:
        FacilityInfo info_;

        // Component members (ownership transferred to UIWindow via AddChild)
        StatItem* occupancy_stat_;
        StatItem* revenue_stat_;
        StatItem* satisfaction_stat_;
        StatItem* tenants_stat_;
        SectionHeader* status_header_;
        StatItem* cleanliness_state_stat_;
        StatItem* cleanliness_stat_;
        StatItem* maintenance_stat_;
        StatItem* maintenance_state_stat_;
        std::unique_ptr<AlertBar> fire_alert_;
        std::unique_ptr<AlertBar> security_alert_;
        SectionHeader* adjacency_header_;
        std::vector<StatItem*> adjacency_items_;
        
        // Button panel and raw pointers to buttons
        Panel* button_panel_;
        IconButton* demolish_button_;
        IconButton* upgrade_button_;
        IconButton* repair_button_;

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
        void Render() const override;
        void Update(const PersonInfo& info);
    
    protected:
        void RenderContent() const override;
        
    private:
        PersonInfo info_;

        // Component members (ownership transferred to UIWindow via AddChild)
        StatItem* type_stat_;
        StatItem* archetype_stat_;
        SectionHeader* staff_header_;
        StatItem* role_stat_;
        StatItem* duty_stat_;
        StatItem* shift_stat_;
        StatItem* status_stat_;
        StatItem* state_stat_;
        StatItem* current_floor_stat_;
        StatItem* dest_floor_stat_;
        StatItem* wait_time_stat_;
        SectionHeader* needs_header_;
        StatItem* hunger_stat_;
        StatItem* entertainment_stat_;
        StatItem* comfort_stat_;
        StatItem* shopping_stat_;
        StatItem* satisfaction_stat_;

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
        void Render() const override;
        void Update(const ElevatorInfo& info);
    
    protected:
        void RenderContent() const override;
        
    private:
        ElevatorInfo info_;

        // Component members (ownership transferred to UIWindow via AddChild)
        StatItem* current_floor_stat_;
        StatItem* occupancy_stat_;
        StatItem* next_stop_stat_;
        StatItem* queue_length_stat_;
        std::vector<StatItem*> queue_items_;

        void BuildComponents();
        void UpdateComponentValues();
    };

}
