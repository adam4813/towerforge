#include "ui/income_analytics_overlay.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    IncomeAnalyticsOverlay::IncomeAnalyticsOverlay(const IncomeBreakdown &data)
        : title_("Income Analytics")
          , visible_(false)
          , data_(data)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , content_container_(nullptr) {
    }

    IncomeAnalyticsOverlay::~IncomeAnalyticsOverlay() = default;

    void IncomeAnalyticsOverlay::Initialize() {
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

    void IncomeAnalyticsOverlay::UpdateLayout() {
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

    void IncomeAnalyticsOverlay::Update(const IncomeBreakdown &data) {
        data_ = data;
        RebuildContent();
    }

    void IncomeAnalyticsOverlay::RebuildContent() const {
        if (!content_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        content_container_->ClearChildren();

        // Revenue Summary section
        auto summary_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Revenue Summary ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(summary_header));

        // Total Revenue
        std::ostringstream total_rev;
        total_rev << "Total Revenue: $" << std::fixed << std::setprecision(2)
                << data_.total_hourly_revenue << "/hr";
        auto total_rev_text = std::make_unique<Text>(
            0.0f, 0.0f, total_rev.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(GREEN)
        );
        content_container_->AddChild(std::move(total_rev_text));

        // Operating Costs
        std::ostringstream total_cost;
        total_cost << "Operating Costs: $" << std::fixed << std::setprecision(2)
                << data_.total_operating_costs << "/hr";
        auto total_cost_text = std::make_unique<Text>(
            0.0f, 0.0f, total_cost.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(ORANGE)
        );
        content_container_->AddChild(std::move(total_cost_text));

        // Net Profit
        std::ostringstream net_profit;
        net_profit << "Net Profit: $" << std::fixed << std::setprecision(2)
                << data_.net_hourly_profit << "/hr";
        const Color profit_color = data_.net_hourly_profit >= 0 ? GREEN : RED;
        auto net_profit_text = std::make_unique<Text>(
            0.0f, 0.0f, net_profit.str(), UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(profit_color)
        );
        content_container_->AddChild(std::move(net_profit_text));

        // Spacer
        auto spacer1 = std::make_unique<Container>();
        spacer1->SetSize(10, 10);
        content_container_->AddChild(std::move(spacer1));

        // Revenue by Type section
        auto type_header = std::make_unique<Text>(
            0.0f, 0.0f,
            "=== Revenue by Type ===",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(GOLD)
        );
        content_container_->AddChild(std::move(type_header));

        if (data_.revenues.empty()) {
            auto no_data = std::make_unique<Text>(
                0.0f, 0.0f,
                "No facilities generating revenue",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(GRAY)
            );
            content_container_->AddChild(std::move(no_data));
        } else {
            for (const auto &rev: data_.revenues) {
                // Facility type and count
                std::ostringstream facility_line;
                facility_line << rev.facility_type << " (" << rev.facility_count << ")";
                auto facility_text = std::make_unique<Text>(
                    0.0f, 0.0f, facility_line.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(SKYBLUE)
                );
                content_container_->AddChild(std::move(facility_text));

                // Revenue
                std::ostringstream rev_line;
                rev_line << "  $" << std::fixed << std::setprecision(2)
                        << rev.hourly_revenue << "/hr";
                auto rev_text = std::make_unique<Text>(
                    0.0f, 0.0f, rev_line.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(GREEN)
                );
                content_container_->AddChild(std::move(rev_text));

                // Tenants and occupancy
                std::ostringstream details_line;
                details_line << "  " << rev.total_tenants << " tenants, "
                        << std::fixed << std::setprecision(0)
                        << rev.average_occupancy << "% occupied";
                auto details_text = std::make_unique<Text>(
                    0.0f, 0.0f, details_line.str(), UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(LIGHTGRAY)
                );
                content_container_->AddChild(std::move(details_text));
            }
        }

        content_container_->InvalidateComponents();
        content_container_->UpdateComponentsRecursive();

        if (auto *scroll = content_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void IncomeAnalyticsOverlay::Show() {
        visible_ = true;
        if (!main_panel_) {
            Initialize();
        }
        UpdateLayout();
    }

    void IncomeAnalyticsOverlay::Show(const IncomeBreakdown &data) {
        data_ = data;
        RebuildContent();
        Show();
    }

    void IncomeAnalyticsOverlay::Hide() {
        visible_ = false;
        if (close_callback_) {
            close_callback_();
        }
    }

    void IncomeAnalyticsOverlay::Render() const {
        if (!visible_ || !main_panel_) return;

        RenderDimOverlay();
        main_panel_->Render();
    }

    void IncomeAnalyticsOverlay::RenderDimOverlay() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    bool IncomeAnalyticsOverlay::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_ || !main_panel_) return false;
        return main_panel_->ProcessMouseEvent(event);
    }

    void IncomeAnalyticsOverlay::HandleKeyboard() {
        if (!visible_) return;

        if (IsKeyPressed(KEY_ESCAPE)) {
            Hide();
        }
    }

    void IncomeAnalyticsOverlay::Shutdown() {
        content_container_ = nullptr;
        main_panel_.reset();
    }
}
