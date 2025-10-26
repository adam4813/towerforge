#include "ui/icon_button.h"

namespace towerforge::ui {

    IconButton::IconButton(const float relative_x, const float relative_y, 
                           const float width, const float height,
                           const std::string& label, const Color bg_color, const Color text_color)
        : Button(relative_x, relative_y, width, height, label, bg_color, WHITE) {
        SetTextColor(text_color);
        SetFontSize(14);
    }

}
