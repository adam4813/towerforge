#include "ui/window_chrome.h"
#include "ui/batch_renderer/batch_adapter.h"

namespace towerforge::ui {

    WindowChrome::WindowChrome() = default;

    void WindowChrome::RenderFrame(const Rectangle& bounds, const Color border_color) const {
        // Draw main window background
        batch_renderer::adapter::DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            static_cast<int>(bounds.height),
            ColorAlpha(BLACK, 0.8f)
        );

        // Draw title bar background
        batch_renderer::adapter::DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            TITLE_BAR_HEIGHT,
            ColorAlpha(BLACK, 0.9f)
        );

        // Draw top border accent
        batch_renderer::adapter::DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            2,
            border_color
        );
    }

    void WindowChrome::RenderTitleBar(const Rectangle& bounds, const std::string& title) const {
        // Draw title text
        batch_renderer::adapter::DrawText(
            title.c_str(),
            static_cast<int>(bounds.x) + PADDING,
            static_cast<int>(bounds.y) + 5,
            14,
            WHITE
        );
    }

    void WindowChrome::RenderCloseButton(const Rectangle& bounds) const {
        const int button_x = static_cast<int>(bounds.x + bounds.width) - CLOSE_BUTTON_SIZE - 5;
        const int button_y = static_cast<int>(bounds.y) + 5;

        // Draw close button background
        batch_renderer::adapter::DrawRectangle(
            button_x,
            button_y,
            CLOSE_BUTTON_SIZE,
            CLOSE_BUTTON_SIZE,
            ColorAlpha(RED, 0.7f)
        );

        // Draw "X" text
        batch_renderer::adapter::DrawText(
            "X",
            button_x + 3,
            button_y + 1,
            12,
            WHITE
        );
    }

    bool WindowChrome::IsCloseButtonClicked(const int mouse_x, const int mouse_y, const Rectangle& bounds) const {
        const int button_x = static_cast<int>(bounds.x + bounds.width) - CLOSE_BUTTON_SIZE - 5;
        const int button_y = static_cast<int>(bounds.y) + 5;

        return mouse_x >= button_x && mouse_x <= button_x + CLOSE_BUTTON_SIZE &&
               mouse_y >= button_y && mouse_y <= button_y + CLOSE_BUTTON_SIZE;
    }

}
