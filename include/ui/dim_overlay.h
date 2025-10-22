#pragma once

#include <raylib.h>

namespace towerforge::ui {

    /**
     * @brief Reusable dim overlay for modal-like backgrounds
     * 
     * Renders a semi-transparent overlay that dims the screen behind panels/modals.
     * Used by settings menus, dialogs, and other overlay UI elements.
     */
    class DimOverlay {
    public:
        explicit DimOverlay(float opacity = 0.7f);
        
        /**
         * @brief Render the dim overlay covering the entire screen
         */
        void Render() const;
        
        void SetOpacity(float opacity) { opacity_ = opacity; }
        void SetColor(Color color) { color_ = color; }
        
    private:
        Color color_;
        float opacity_;
    };

}
