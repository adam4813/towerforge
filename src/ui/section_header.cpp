#include "ui/section_header.h"

namespace towerforge::ui {

    SectionHeader::SectionHeader(const std::string& text, const Color color, const int y_position)
        : text_(text)
        , color_(color)
        , y_position_(y_position) {
    }

    void SectionHeader::SetText(const std::string& text) {
        text_ = text;
    }

    void SectionHeader::SetColor(const Color color) {
        color_ = color;
    }

    void SectionHeader::SetPosition(const int y) {
        y_position_ = y;
    }

    void SectionHeader::Render(const int x, const int y_base) const {
        DrawText(text_.c_str(), x, y_base + y_position_, 14, color_);
    }

}
