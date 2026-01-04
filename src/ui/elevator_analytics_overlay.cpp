#include "ui/elevator_analytics_overlay.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    ElevatorAnalyticsOverlay::ElevatorAnalyticsOverlay(const ElevatorAnalytics &data)
        : title_("Elevator Analytics")
          , visible_(false)
          , data_(data)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , content_container_(nullptr) {
    }

    ElevatorAnalyticsOverlay::~ElevatorAnalyticsOverlay() = default;

    void ElevatorAnalyticsOverlay::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        const int panel_x = std::max(0, static_cast<int>(screen_width - OVERLAY_WIDTH) / 2);
        const int panel_y = std::max(0, static_cast<int>(screen_height - OVERLAY_HEIGHT) / 2);

        // Create main panel
        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(OVERLAY_WIDTH), static_cast<float>(OVERLAY_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        main_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Title
        auto title = std::make_unique<Text>(
            0.0f, 0.0f,
            title_,
            UITheme::FONT_SIZE_LARGE,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        main_panel_->AddChild(std::move(title));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(OVERLAY_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Scrollable content container
        constexpr float content_width = OVERLAY_WIDTH - UITheme::PADDING_LARGE * 2;
        constexpr float content_height = OVERLAY_HEIGHT - 120;

        auto content = engine::ui::ContainerBuilder()
                .Size(content_width, content_height)
                .Layout<VerticalLayout>(6.0f, Alignment::Start)
                .Scrollable(ScrollDirection::Vertical)
                .Padding(UITheme::PADDING_SMALL)
                .ClipChildren()
                .Build();

        content_container_ = content.get();
        main_panel_->AddChild(std::move(content));

        // Close button
        auto close_button = std::make_unique<Button>(
            UITheme::BUTTON_WIDTH_SMALL,
            UITheme::BUTTON_HEIGHT_SMALL,
            "Close",
            UITheme::FONT_SIZE_NORMAL
        );
        close_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        close_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        close_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        close_button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        close_button->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                Hide();
                return true;
            }
            return false;
        });
        main_panel_->AddChild(std::move(close_button));

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;

        RebuildContent();
        UpdateLayout();
    }

    void ElevatorAnalyticsOverlay::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (main_panel_) {
            const int panel_x = std::max(0, static_cast<int>(screen_width - OVERLAY_WIDTH) / 2);
            const int panel_y = std::max(0, static_cast<int>(screen_height - OVERLAY_HEIGHT) / 2);
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void ElevatorAnalyticsOverlay::Update(const ElevatorAnalytics &data) {
        data_ = data;
        RebuildContent();
    }

    void ElevatorAnalyticsOverlay::RebuildContent() const {
        if (!content_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        content_container_->ClearChildren();

        // Overall Statistics section
        auto summary_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Overall Statistics ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(summary_header));

        // Total Passengers
        std::ostringstream total_pass;
        total_pass << "Total Passengers: " << data_.total_passengers;
        auto total_pass_text = std::make_unique<Text>(
            0.0f, 0.0f, total_pass.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(WHITE)
        );
        content_container_->AddChild(std::move(total_pass_text));

        // Average Wait Time
        std::ostringstream avg_wait;
        avg_wait << "Avg Wait Time: " << std::fixed << std::setprecision(1)
                << data_.average_wait_time << "s";
        const Color wait_color = data_.average_wait_time < 15.0f
                                     ? GREEN
                                     : (data_.average_wait_time < 30.0f ? YELLOW : RED);
        auto avg_wait_text = std::make_unique<Text>(
            0.0f, 0.0f, avg_wait.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(wait_color)
        );
        content_container_->AddChild(std::move(avg_wait_text));

        // Busiest Floor
        if (data_.busiest_floor_count > 0) {
            std::ostringstream busiest;
            busiest << "Busiest Floor: " << data_.busiest_floor
                    << " (" << data_.busiest_floor_count << " trips)";
            auto busiest_text = std::make_unique<Text>(
                0.0f, 0.0f, busiest.str(), UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(SKYBLUE)
            );
            content_container_->AddChild(std::move(busiest_text));
        }

        // Spacer
        auto spacer1 = std::make_unique<Container>();
        spacer1->SetSize(10, 10);
        content_container_->AddChild(std::move(spacer1));

        // Elevator Performance section
        auto perf_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Elevator Performance ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(perf_header));

        if (data_.elevators.empty()) {
            auto no_data = std::make_unique<Text>(
                0.0f, 0.0f,
                "No elevators installed",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(GRAY)
            );
            content_container_->AddChild(std::move(no_data));
        } else {
            for (const auto &elev: data_.elevators) {
                // Elevator ID
                std::ostringstream elev_id;
                elev_id << "Elevator #" << elev.elevator_id;
                auto elev_id_text = std::make_unique<Text>(
                    0.0f, 0.0f, elev_id.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(SKYBLUE)
                );
                content_container_->AddChild(std::move(elev_id_text));

                // Trips
                std::ostringstream trips;
                trips << "  Trips: " << elev.total_trips;
                auto trips_text = std::make_unique<Text>(
                    0.0f, 0.0f, trips.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(LIGHTGRAY)
                );
                content_container_->AddChild(std::move(trips_text));

                // Passengers
                std::ostringstream passengers;
                passengers << "  Passengers: " << elev.total_passengers_carried;
                auto passengers_text = std::make_unique<Text>(
                    0.0f, 0.0f, passengers.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(LIGHTGRAY)
                );
                content_container_->AddChild(std::move(passengers_text));

                // Utilization
                std::ostringstream util;
                util << "  Utilization: " << std::fixed << std::setprecision(0)
                        << elev.utilization_rate << "%";
                const Color util_color = elev.utilization_rate < 60.0f
                                             ? GREEN
                                             : (elev.utilization_rate < 80.0f ? YELLOW : RED);
                auto util_text = std::make_unique<Text>(
                    0.0f, 0.0f, util.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(util_color)
                );
                content_container_->AddChild(std::move(util_text));

                // Average wait time
                std::ostringstream wait;
                wait << "  Avg Wait: " << std::fixed << std::setprecision(1)
                        << elev.average_wait_time << "s";
                const Color wait_col = elev.average_wait_time < 15.0f
                                           ? GREEN
                                           : (elev.average_wait_time < 30.0f ? YELLOW : RED);
                auto wait_text = std::make_unique<Text>(
                    0.0f, 0.0f, wait.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(wait_col)
                );
                content_container_->AddChild(std::move(wait_text));

                // Small spacer between elevators
                auto elev_spacer = std::make_unique<Container>();
                elev_spacer->SetSize(10, 5);
                content_container_->AddChild(std::move(elev_spacer));
            }
        }

        content_container_->InvalidateComponents();
        content_container_->UpdateComponentsRecursive();

        if (auto *scroll = content_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void ElevatorAnalyticsOverlay::Show() {
        visible_ = true;
        if (!main_panel_) {
            Initialize();
        }
        UpdateLayout();
    }

    void ElevatorAnalyticsOverlay::Show(const ElevatorAnalytics &data) {
        data_ = data;
        RebuildContent();
        Show();
    }

    void ElevatorAnalyticsOverlay::Hide() {
        visible_ = false;
        if (close_callback_) {
            close_callback_();
        }
    }

    void ElevatorAnalyticsOverlay::Render() const {
        if (!visible_ || !main_panel_) return;

        RenderDimOverlay();
        main_panel_->Render();
    }

    void ElevatorAnalyticsOverlay::RenderDimOverlay() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    bool ElevatorAnalyticsOverlay::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void ElevatorAnalyticsOverlay::HandleKeyboard() {
        if (!visible_) return;

        if (IsKeyPressed(KEY_ESCAPE)) {
            Hide();
        }
    }

    void ElevatorAnalyticsOverlay::Shutdown() {
        content_container_ = nullptr;
        main_panel_.reset();
    }
}
