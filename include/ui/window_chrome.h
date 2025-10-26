#pragma once

#include <raylib.h>
#include <string>

namespace towerforge::ui {

    /**
     * @brief Strategy class for window chrome rendering (title bar, close button, frame)
     * 
     * Encapsulates the visual decoration of windows using the Strategy pattern.
     * This allows windows to delegate chrome rendering without inheritance,
     * and enables reuse across different window types (modals, dialogs, tooltips).
     */
    class WindowChrome {
    public:
        WindowChrome();
        ~WindowChrome() = default;

        /**
         * @brief Render the window frame (background and border)
         * @param bounds Window bounds
         * @param border_color Color for the border/accent
         */
        void RenderFrame(const Rectangle& bounds, Color border_color) const;

        /**
         * @brief Render the title bar with title text
         * @param bounds Window bounds
         * @param title Title text to display
         */
        void RenderTitleBar(const Rectangle& bounds, const std::string& title) const;

        /**
         * @brief Render the close button
         * @param bounds Window bounds
         */
        void RenderCloseButton(const Rectangle& bounds) const;

        /**
         * @brief Check if the close button was clicked
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param bounds Window bounds
         * @return true if close button was clicked
         */
        bool IsCloseButtonClicked(int mouse_x, int mouse_y, const Rectangle& bounds) const;

        /**
         * @brief Get the height of the title bar
         */
        static constexpr int GetTitleBarHeight() { return TITLE_BAR_HEIGHT; }

        /**
         * @brief Get the padding inside the window
         */
        static constexpr int GetPadding() { return PADDING; }

        /**
         * @brief Get the size of the close button
         */
        static constexpr int GetCloseButtonSize() { return CLOSE_BUTTON_SIZE; }

    private:
        static constexpr int TITLE_BAR_HEIGHT = 25;
        static constexpr int CLOSE_BUTTON_SIZE = 15;
        static constexpr int PADDING = 10;
    };

}
