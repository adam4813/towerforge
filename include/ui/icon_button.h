#pragma once

#include "ui_element.h"

namespace towerforge::ui {

    /**
     * @brief Button component with color customization
     * 
     * Now inherits from Button, gaining full UI hierarchy integration,
     * automatic mouse handling, tooltip support, and keyboard navigation.
     */
    class IconButton : public Button {
    public:
        /**
         * @brief Construct an IconButton
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param width Width of the button
         * @param height Height of the button
         * @param label Button label text
         * @param bg_color Background color
         * @param text_color Text color
         */
        IconButton(float relative_x, float relative_y, float width, float height,
                   const std::string& label, Color bg_color, Color text_color);
    };

}
