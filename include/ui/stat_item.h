#pragma once

#include <string>
#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Reusable component for displaying a label-value stat pair
     */
    class StatItem {
    public:
        StatItem(const std::string& label, int y_position = 0);

        void SetValue(const std::string& value, Color color = LIGHTGRAY);
        void SetPosition(int y);
        void Render(int x, int y_base) const;

    private:
        std::string label_;
        std::string value_;
        Color value_color_;
        int y_position_;
    };

}
