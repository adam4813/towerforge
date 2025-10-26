#include "ui/section_header.h"

namespace towerforge::ui {

    SectionHeader::SectionHeader(const float relative_x, const float relative_y, 
                                 const std::string& text, const Color color)
        : Label(relative_x, relative_y, text, 14, color, Label::Alignment::Left) {
    }

}
