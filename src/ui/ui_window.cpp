#include "ui/ui_window.h"
#include "ui/batch_renderer/batch_adapter.h"

namespace towerforge::ui {

    int UIWindow::next_id_ = 0;

    UIWindow::UIWindow(const std::string& title, const int width, const int height)
        : id_(next_id_++)
          , title_(title)
          , x_(0)
          , y_(0)
          , width_(width)
          , height_(height)
          , z_order_(0) {
    }

    bool UIWindow::Contains(const int x, const int y) const {
        return x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + height_;
    }

    bool UIWindow::IsCloseButtonClicked(const int mouse_x, const int mouse_y) const {
        const int button_x = x_ + width_ - CLOSE_BUTTON_SIZE - 5;
        const int button_y = y_ + 5;
    
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

    void UIWindow::SetPosition(const int x, const int y) {
        x_ = x;
        y_ = y;
    }

    void UIWindow::RenderFrame(const Color border_color) const {
        // Draw main window background
        batch_renderer::adapter::DrawRectangle(x_, y_, width_, height_, ColorAlpha(BLACK, 0.8f));
    
        // Draw title bar
        batch_renderer::adapter::DrawRectangle(x_, y_, width_, TITLE_BAR_HEIGHT, ColorAlpha(BLACK, 0.9f));
        batch_renderer::adapter::DrawRectangle(x_, y_, width_, 2, border_color);
    
        // Draw title text
        batch_renderer::adapter::DrawText(title_.c_str(), x_ + PADDING, y_ + 5, 14, WHITE);
    }

    void UIWindow::RenderCloseButton() const {
        const int button_x = x_ + width_ - CLOSE_BUTTON_SIZE - 5;
        const int button_y = y_ + 5;
    
        // Draw close button
        batch_renderer::adapter::DrawRectangle(button_x, button_y, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE, 
                      ColorAlpha(RED, 0.7f));
        batch_renderer::adapter::DrawText("X", button_x + 3, button_y + 1, 12, WHITE);
    }

}
