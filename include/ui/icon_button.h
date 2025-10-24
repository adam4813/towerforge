#pragma once

#include <string>
#include <functional>
#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Reusable button component with color customization
     */
    class IconButton {
    public:
        using ClickCallback = std::function<void()>;

        IconButton(const std::string& label, int width, int height, Color bg_color, Color text_color, int x_offset = 0, int y_position = 0);

        void SetLabel(const std::string& label);
        void SetColors(Color bg_color, Color text_color);
        void SetPosition(int x_offset, int y);
        void SetClickCallback(ClickCallback callback);
        void SetVisible(bool visible);
        bool IsVisible() const { return visible_; }
        void Render(int base_x, int base_y) const;

    private:
        std::string label_;
        int width_;
        int height_;
        Color bg_color_;
        Color text_color_;
        int x_offset_;
        int y_position_;
        bool visible_;
        ClickCallback callback_;
    };

}
