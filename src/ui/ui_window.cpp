#include "ui/ui_window.h"

namespace towerforge {
namespace ui {

int UIWindow::next_id_ = 0;

UIWindow::UIWindow(const std::string& title, int width, int height)
    : id_(next_id_++)
    , title_(title)
    , x_(0)
    , y_(0)
    , width_(width)
    , height_(height)
    , z_order_(0) {
}

bool UIWindow::Contains(int x, int y) const {
    return x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + height_;
}

bool UIWindow::IsCloseButtonClicked(int mouse_x, int mouse_y) const {
    int button_x = x_ + width_ - CLOSE_BUTTON_SIZE - 5;
    int button_y = y_ + 5;
    
    return mouse_x >= button_x && mouse_x <= button_x + CLOSE_BUTTON_SIZE &&
           mouse_y >= button_y && mouse_y <= button_y + CLOSE_BUTTON_SIZE;
}

Rectangle UIWindow::GetBounds() const {
    return Rectangle{
        static_cast<float>(x_), 
        static_cast<float>(y_), 
        static_cast<float>(width_), 
        static_cast<float>(height_)
    };
}

void UIWindow::SetPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

void UIWindow::RenderFrame(Color border_color) {
    // Draw main window background
    DrawRectangle(x_, y_, width_, height_, ColorAlpha(BLACK, 0.8f));
    
    // Draw title bar
    DrawRectangle(x_, y_, width_, TITLE_BAR_HEIGHT, ColorAlpha(BLACK, 0.9f));
    DrawRectangle(x_, y_, width_, 2, border_color);
    
    // Draw title text
    DrawText(title_.c_str(), x_ + PADDING, y_ + 5, 14, WHITE);
}

void UIWindow::RenderCloseButton() {
    int button_x = x_ + width_ - CLOSE_BUTTON_SIZE - 5;
    int button_y = y_ + 5;
    
    // Draw close button
    DrawRectangle(button_x, button_y, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE, 
                  ColorAlpha(RED, 0.7f));
    DrawText("X", button_x + 3, button_y + 1, 12, WHITE);
}

} // namespace ui
} // namespace towerforge
