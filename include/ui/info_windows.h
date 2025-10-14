#pragma once

#include "ui/ui_window.h"
#include "ui/hud.h"
#include <sstream>
#include <iomanip>

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
    };

}
