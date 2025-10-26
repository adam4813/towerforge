#pragma once

#include "ui/ui_element.h"

namespace towerforge::ui {

    /**
     * @brief Section header component - specialized Label for section titles
     * 
     * Now inherits from Label to integrate into UIElement hierarchy.
     * Provides consistent styling for section headers in info windows.
     */
    class SectionHeader : public Label {
    public:
        /**
         * @brief Construct a section header
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param text Header text
         * @param color Text color (default: YELLOW)
         */
        SectionHeader(float relative_x, float relative_y, const std::string& text, Color color = YELLOW);
    };

}
