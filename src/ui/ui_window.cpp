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
        const Rectangle bounds = GetBounds();
        return chrome_.IsCloseButtonClicked(mouse_x, mouse_y, bounds);
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
        const Rectangle bounds = GetBounds();
        
        // Delegate to chrome strategy
        chrome_.RenderFrame(bounds, border_color);
        chrome_.RenderTitleBar(bounds, title_);
    }

    void UIWindow::RenderCloseButton() const {
        const Rectangle bounds = GetBounds();
        
        // Delegate to chrome strategy
        chrome_.RenderCloseButton(bounds);
    }

}
