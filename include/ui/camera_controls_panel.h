#pragma once

#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include <functional>
#include <memory>

import engine;

namespace towerforge::ui {
    /**
     * @brief Camera controls panel for displaying camera info and zoom control
     * 
     * Declarative UI component using citrus engine Panel and elements.
     * Displays camera control hints and provides an interactive zoom slider.
     */
    class CameraControlsPanel {
    public:
        using ZoomCallback = std::function<void(float zoom)>;

        CameraControlsPanel();

        ~CameraControlsPanel() = default;

        /**
         * @brief Update panel state and position
         */
        void Update(float delta_time);

        /**
         * @brief Render the panel
         */
        void Render() const;

        /**
         * @brief Process mouse events
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent &event);

        /**
         * @brief Check if mouse is over the panel
         */
        bool IsMouseOver(int mouse_x, int mouse_y) const;

        /**
         * @brief Set the current zoom level for display
         */
        void SetZoom(float zoom) { current_zoom_ = zoom; }

        /**
         * @brief Set callback for zoom changes via slider
         */
        void SetZoomCallback(ZoomCallback callback) { zoom_callback_ = std::move(callback); }

        /**
         * @brief Set min/max zoom values
         */
        void SetZoomRange(float min_zoom, float max_zoom);

    private:
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;

        // Slider element for zoom control
        engine::ui::elements::Slider *zoom_slider_ = nullptr;

        float current_zoom_ = 1.0f;
        float min_zoom_ = 0.5f;
        float max_zoom_ = 2.0f;
        ZoomCallback zoom_callback_;

        // Responsive sizing constants
        static constexpr int BASE_WIDTH = 165;
        static constexpr int BASE_HEIGHT = 140; // Slightly taller to accommodate slider
        static constexpr float MAX_WIDTH_PERCENT = 0.20f;
        static constexpr int PADDING = 8;
        static constexpr int SLIDER_HEIGHT = 24;

        void BuildComponents();

        // Calculate responsive dimensions
        int CalculateWidth() const;

        int CalculateHeight() const;

        int CalculateY() const;
    };
} // namespace towerforge::ui
