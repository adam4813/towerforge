#pragma once

#include "ui/ui_element.h"

namespace towerforge::ui {

    /**
     * @brief Label-value stat pair component
     * 
     * Inherits from UIElement (not Panel) as it's a compound element without
     * visual container features. Composes two Label children for label and value.
     * 
     * Enables tooltips, click handlers, and full UIElement hierarchy integration
     * without the overhead of Panel's background/border/padding features.
     */
    class StatItem : public UIElement {
    public:
        /**
         * @brief Construct a stat item
         * @param relative_x X position relative to parent
         * @param relative_y Y position relative to parent
         * @param label Label text (left side)
         */
        StatItem(float relative_x, float relative_y, const std::string& label);

        /**
         * @brief Set the value text and color (reactive update)
         * @param value Value text (right side)
         * @param color Value text color
         */
        void SetValue(const std::string& value, Color color = LIGHTGRAY);
        
        /**
         * @brief Render the stat item and its children (labels)
         */
        void Render() const override;

    private:
        Label* label_;  // Raw pointer to label (owned by UIElement)
        Label* value_;  // Raw pointer to value (owned by UIElement)
    };

}
