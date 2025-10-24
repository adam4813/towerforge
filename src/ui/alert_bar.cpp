#include "ui/alert_bar.h"

namespace towerforge::ui {

    AlertBar::AlertBar(const std::string& message, const Color color, const int y_position)
        : message_(message)
        , color_(color)
        , y_position_(y_position)
        , visible_(true) {
    }

    void AlertBar::SetMessage(const std::string& message) {
        message_ = message;
    }

    void AlertBar::SetColor(const Color color) {
        color_ = color;
    }

    void AlertBar::SetPosition(const int y) {
        y_position_ = y;
    }

    void AlertBar::SetVisible(const bool visible) {
        visible_ = visible;
    }

    void AlertBar::Render(const int x, const int y_base) const {
        if (visible_) {
            DrawText(message_.c_str(), x, y_base + y_position_, 14, color_);
        }
    }

}
