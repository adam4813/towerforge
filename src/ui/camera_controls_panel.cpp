#include "ui/camera_controls_panel.h"
#include <algorithm>

namespace towerforge::ui {
    CameraControlsPanel::CameraControlsPanel() {
        BuildComponents();
    }

    void CameraControlsPanel::BuildComponents() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int width = CalculateWidth();
        const int height = CalculateHeight();
        const int y = CalculateY();

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetSize(static_cast<float>(width), static_cast<float>(height));
        main_panel_->SetRelativePosition(10, static_cast<float>(y));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->SetBorderWidth(1.0f);
        main_panel_->SetPadding(static_cast<float>(PADDING));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(4.0f, Alignment::Start)
        );

        // Title
        auto title = std::make_unique<Text>(
            0, 0, "CAMERA", 12, UITheme::ToEngineColor(YELLOW)
        );
        main_panel_->AddChild(std::move(title));

        // Control hints
        const int text_size = 10;
        auto hint1 = std::make_unique<Text>(0, 0, "Pan: Mid-Click+Drag", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint1));

        auto hint2 = std::make_unique<Text>(0, 0, "     or Arrow Keys", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint2));

        auto hint3 = std::make_unique<Text>(0, 0, "Zoom: Mouse Wheel", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint3));

        auto hint4 = std::make_unique<Text>(0, 0, "      or +/- keys", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint4));

        auto hint5 = std::make_unique<Text>(0, 0, "Reset: Home", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint5));

        auto hint6 = std::make_unique<Text>(0, 0, "Follow: F", text_size, UITheme::ToEngineColor(LIGHTGRAY));
        main_panel_->AddChild(std::move(hint6));

        // Zoom slider
        const int slider_width = width - PADDING * 2;
        auto slider = std::make_unique<Slider>(
            0, 0, slider_width, SLIDER_HEIGHT,
            min_zoom_, max_zoom_
        );
        slider->SetValue(current_zoom_);
        slider->SetLabel("Zoom");
        slider->SetShowValue(true);
        slider->SetTrackColor(UITheme::ToEngineColor(DARKGRAY));
        slider->SetFillColor(UITheme::ToEngineColor(GREEN));
        slider->SetThumbColor(UITheme::ToEngineColor(LIGHTGRAY));
        slider->SetValueChangedCallback([this](float value) {
            current_zoom_ = value;
            if (zoom_callback_) {
                zoom_callback_(value);
            }
        });
        zoom_slider_ = slider.get();
        main_panel_->AddChild(std::move(slider));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void CameraControlsPanel::Update(float /*delta_time*/) {
        if (!main_panel_) return;

        // Update position and size on resize
        const int width = CalculateWidth();
        const int height = CalculateHeight();
        const int y = CalculateY();

        main_panel_->SetRelativePosition(10, static_cast<float>(y));
        main_panel_->SetSize(static_cast<float>(width), static_cast<float>(height));

        // Update slider value from external zoom changes (e.g., mouse wheel)
        if (zoom_slider_) {
            zoom_slider_->SetValue(current_zoom_);
        }
    }

    void CameraControlsPanel::Render() const {
        if (main_panel_) {
            main_panel_->Render();
        }
    }

    bool CameraControlsPanel::ProcessMouseEvent(const MouseEvent &event) {
        if (!main_panel_) return false;

        // Convert towerforge MouseEvent to engine MouseEvent
        engine::ui::MouseEvent engine_event;
        engine_event.x = event.x;
        engine_event.y = event.y;
        engine_event.left_pressed = event.left_pressed;
        engine_event.left_released = false;
        engine_event.right_pressed = event.right_pressed;
        engine_event.right_released = false;
        engine_event.left_down = event.left_down;
        engine_event.right_down = event.right_down;

        // Let the panel handle the event (will propagate to slider)
        if (main_panel_->ProcessMouseEvent(engine_event)) {
            return true;
        }

        // Consume clicks within panel bounds to prevent camera pan
        if (event.left_pressed && IsMouseOver(static_cast<int>(event.x), static_cast<int>(event.y))) {
            return true;
        }

        return false;
    }

    bool CameraControlsPanel::IsMouseOver(int mouse_x, int mouse_y) const {
        const int width = CalculateWidth();
        const int height = CalculateHeight();
        const int y = CalculateY();

        return mouse_x >= 10 && mouse_x <= 10 + width &&
               mouse_y >= y && mouse_y <= y + height;
    }

    void CameraControlsPanel::SetZoomRange(float min_zoom, float max_zoom) {
        min_zoom_ = min_zoom;
        max_zoom_ = max_zoom;
        // Note: Slider range is set at construction time
        // If range changes, rebuild components
        if (zoom_slider_) {
            BuildComponents();
        }
    }

    int CameraControlsPanel::CalculateWidth() const {
        const int screen_width = GetScreenWidth();
        const int max_width = static_cast<int>(screen_width * MAX_WIDTH_PERCENT);
        return std::min(BASE_WIDTH, max_width);
    }

    int CameraControlsPanel::CalculateHeight() const {
        const int width = CalculateWidth();
        return static_cast<int>(BASE_HEIGHT * (static_cast<float>(width) / BASE_WIDTH));
    }

    int CameraControlsPanel::CalculateY() const {
        const int screen_height = GetScreenHeight();
        const int height = CalculateHeight();
        // Position above speed controls (60px margin for action bar + speed panel height)
        return screen_height - height - 60;
    }
} // namespace towerforge::ui
