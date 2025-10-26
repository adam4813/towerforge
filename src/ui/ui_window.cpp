#include "ui/ui_window.h"

namespace towerforge::ui {

    int UIWindow::next_id_ = 0;

    UIWindow::UIWindow(const std::string& title, const float width, const float height)
        : Panel(0, 0, width, height, ColorAlpha(BLACK, 0.8f), BLANK)
        , id_(next_id_++)
        , title_(title)
        , z_order_(0)
        , close_callback_(nullptr)
        , chrome_() {
        
        // Windows have internal padding for content
        SetPadding(static_cast<float>(WindowChrome::GetPadding()));
    }

    void UIWindow::Render() const {
        const Rectangle bounds = GetAbsoluteBounds();
        
        // Render window chrome (frame, title bar, close button)
        chrome_.RenderFrame(bounds, GetBorderColor());
        chrome_.RenderTitleBar(bounds, title_);
        chrome_.RenderCloseButton(bounds);
        
        // Render content (children or custom rendering)
        RenderContent();
    }
    
    void UIWindow::RenderContent() const {
        // Default: render all children (Composite pattern)
        Panel::Render();
    }

    void UIWindow::Update(const float delta_time) {
        Panel::Update(delta_time);
    }

    bool UIWindow::ProcessMouseEvent(const MouseEvent& event) {
        // Check if close button was clicked
        if (event.left_pressed) {
            const Rectangle bounds = GetAbsoluteBounds();
            if (chrome_.IsCloseButtonClicked(static_cast<int>(event.x), 
                                            static_cast<int>(event.y), bounds)) {
                if (close_callback_) {
                    close_callback_();
                }
                return true;  // Event consumed
            }
        }
        
        // Delegate to Panel for child event handling
        return Panel::ProcessMouseEvent(event);
    }

    void UIWindow::SetWindowPosition(const float x, const float y) {
        SetRelativePosition(x, y);
    }

}
