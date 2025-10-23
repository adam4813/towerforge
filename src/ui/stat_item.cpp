#include "ui/stat_item.h"

namespace towerforge::ui {

    StatItem::StatItem(const std::string& label, const int y_position)
        : label_(label)
        , value_("")
        , value_color_(LIGHTGRAY)
        , y_position_(y_position) {
    }

    void StatItem::SetValue(const std::string& value, const Color color) {
        value_ = value;
        value_color_ = color;
    }

    void StatItem::SetPosition(const int y) {
        y_position_ = y;
    }

    void StatItem::Render(const int x, const int y_base) const {
        DrawText(label_.c_str(), x, y_base + y_position_, 14, LIGHTGRAY);
        DrawText(value_.c_str(), x + 100, y_base + y_position_, 14, value_color_);
    }

}
