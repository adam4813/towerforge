#pragma once

#include <string>
#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Reusable component for displaying alert messages with appropriate colors
     */
    class AlertBar {
    public:
        AlertBar(const std::string& message, Color color, int y_position = 0);

        void SetMessage(const std::string& message);
        void SetColor(Color color);
        void SetPosition(int y);
        void SetVisible(bool visible);
        bool IsVisible() const { return visible_; }
        void Render(int x, int y_base) const;

    private:
        std::string message_;
        Color color_;
        int y_position_;
        bool visible_;
    };

}
