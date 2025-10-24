#pragma once

#include <string>
#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Reusable component for section headers in info windows
     */
    class SectionHeader {
    public:
        SectionHeader(const std::string& text, Color color = YELLOW, int y_position = 0);

        void SetText(const std::string& text);
        void SetColor(Color color);
        void SetPosition(int y);
        void Render(int x, int y_base) const;

    private:
        std::string text_;
        Color color_;
        int y_position_;
    };

}
