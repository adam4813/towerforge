#include "ui/icon_button.h"

namespace towerforge::ui {

    IconButton::IconButton(const std::string& label, const int width, const int height, 
                           const Color bg_color, const Color text_color, 
                           const int x_offset, const int y_position)
        : label_(label)
        , width_(width)
        , height_(height)
        , bg_color_(bg_color)
        , text_color_(text_color)
        , x_offset_(x_offset)
        , y_position_(y_position)
        , visible_(true)
        , callback_(nullptr) {
    }

    void IconButton::SetLabel(const std::string& label) {
        label_ = label;
    }

    void IconButton::SetColors(const Color bg_color, const Color text_color) {
        bg_color_ = bg_color;
        text_color_ = text_color;
    }

    void IconButton::SetPosition(const int x_offset, const int y) {
        x_offset_ = x_offset;
        y_position_ = y;
    }

    void IconButton::SetClickCallback(ClickCallback callback) {
        callback_ = std::move(callback);
    }

    void IconButton::SetVisible(const bool visible) {
        visible_ = visible;
    }

    void IconButton::Render(const int base_x, const int base_y) const {
        if (visible_) {
            DrawRectangle(base_x + x_offset_, base_y + y_position_, width_, height_, bg_color_);
            const int text_x = base_x + x_offset_ + 5;
            const int text_y = base_y + y_position_ + 5;
            DrawText(label_.c_str(), text_x, text_y, 14, text_color_);
        }
    }

}
