#include "ui/population_analytics_overlay.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    PopulationAnalyticsOverlay::PopulationAnalyticsOverlay(const PopulationBreakdown &data)
        : title_("Population Analytics")
          , visible_(false)
          , data_(data)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , content_container_(nullptr) {
    }

    PopulationAnalyticsOverlay::~PopulationAnalyticsOverlay() = default;

    void PopulationAnalyticsOverlay::Initialize() {
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

    void PopulationAnalyticsOverlay::UpdateLayout() {
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

    void PopulationAnalyticsOverlay::Update(const PopulationBreakdown &data) {
        data_ = data;
        RebuildContent();
    }

    void PopulationAnalyticsOverlay::RebuildContent() const {
        if (!content_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        content_container_->ClearChildren();

        // Population Breakdown section
        auto pop_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Population Breakdown ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(pop_header));

        // Total population
        std::ostringstream total;
        total << "Total Population: " << data_.total_population;
        auto total_text = std::make_unique<Text>(
            0.0f, 0.0f, total.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(WHITE)
        );
        content_container_->AddChild(std::move(total_text));

        // Employees
        std::ostringstream emp;
        emp << "  Employees: " << data_.employees;
        auto emp_text = std::make_unique<Text>(
            0.0f, 0.0f, emp.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(LIGHTGRAY)
        );
        content_container_->AddChild(std::move(emp_text));

        // Visitors
        std::ostringstream vis;
        vis << "  Visitors: " << data_.visitors;
        auto vis_text = std::make_unique<Text>(
            0.0f, 0.0f, vis.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(LIGHTGRAY)
        );
        content_container_->AddChild(std::move(vis_text));

        // Spacer
        auto spacer1 = std::make_unique<Container>();
        spacer1->SetSize(10, 10);
        content_container_->AddChild(std::move(spacer1));

        // Residential section
        auto res_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Residential ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(res_header));

        // Occupancy
        std::ostringstream res_occ;
        res_occ << "Occupancy: " << data_.residential_occupancy
                << " / " << data_.residential_capacity;
        auto res_occ_text = std::make_unique<Text>(
            0.0f, 0.0f, res_occ.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(LIGHTGRAY)
        );
        content_container_->AddChild(std::move(res_occ_text));

        // Occupancy rate
        if (data_.residential_capacity > 0) {
            const float occupancy_rate = (static_cast<float>(data_.residential_occupancy) /
                                          data_.residential_capacity) * 100.0f;
            std::ostringstream rate;
            rate << "Rate: " << std::fixed << std::setprecision(0)
                    << occupancy_rate << "%";
            const Color rate_color = occupancy_rate > 80.0f ? GREEN : (occupancy_rate > 50.0f ? YELLOW : RED);
            auto rate_text = std::make_unique<Text>(
                0.0f, 0.0f, rate.str(), UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(rate_color)
            );
            content_container_->AddChild(std::move(rate_text));
        } else {
            auto no_res = std::make_unique<Text>(
                0.0f, 0.0f,
                "No residential facilities",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(GRAY)
            );
            content_container_->AddChild(std::move(no_res));
        }

        // Spacer
        auto spacer2 = std::make_unique<Container>();
        spacer2->SetSize(10, 10);
        content_container_->AddChild(std::move(spacer2));

        // Satisfaction section
        auto sat_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Satisfaction ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(sat_header));

        // Average satisfaction
        std::ostringstream sat;
        sat << "Average: " << std::fixed << std::setprecision(0)
                << data_.average_satisfaction << "%";
        const Color sat_color = data_.average_satisfaction >= 70.0f
                                    ? GREEN
                                    : (data_.average_satisfaction >= 50.0f ? YELLOW : RED);
        auto sat_text = std::make_unique<Text>(
            0.0f, 0.0f, sat.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(sat_color)
        );
        content_container_->AddChild(std::move(sat_text));

        content_container_->InvalidateComponents();
        content_container_->UpdateComponentsRecursive();

        if (auto *scroll = content_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void PopulationAnalyticsOverlay::Show() {
        visible_ = true;
        if (!main_panel_) {
            Initialize();
        }
        UpdateLayout();
    }

    void PopulationAnalyticsOverlay::Show(const PopulationBreakdown &data) {
        data_ = data;
        RebuildContent();
        Show();
    }

    void PopulationAnalyticsOverlay::Hide() {
        visible_ = false;
        if (close_callback_) {
            close_callback_();
        }
    }

    void PopulationAnalyticsOverlay::Render() const {
        if (!visible_ || !main_panel_) return;

        RenderDimOverlay();
        main_panel_->Render();
    }

    void PopulationAnalyticsOverlay::RenderDimOverlay() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    bool PopulationAnalyticsOverlay::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void PopulationAnalyticsOverlay::HandleKeyboard() {
        if (!visible_) return;

        if (IsKeyPressed(KEY_ESCAPE)) {
            Hide();
        }
    }

    void PopulationAnalyticsOverlay::Shutdown() {
        content_container_ = nullptr;
        main_panel_.reset();
    }
}
