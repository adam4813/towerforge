#include "ui/hud/end_game_summary.h"
#include "ui/hud/hud.h"
#include "ui/ui_theme.h"
#include <iomanip>
#include <sstream>

namespace towerforge::ui {
    EndGameSummary::EndGameSummary() : game_state_(nullptr), last_screen_width_(0), last_screen_height_(0) {
    }

    void EndGameSummary::Update(float delta_time) {
        // Check for window resize
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void EndGameSummary::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (summary_panel_ != nullptr) {
            // Center the panel
            const int panel_x = (screen_width - BOX_WIDTH) / 2;
            const int panel_y = (screen_height - BOX_HEIGHT) / 2;
            summary_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            summary_panel_->SetSize(static_cast<float>(BOX_WIDTH), static_cast<float>(BOX_HEIGHT));
            summary_panel_->InvalidateComponents();
            summary_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void EndGameSummary::Render() const {
        if (!summary_panel_) return;

        RenderDimOverlay();
        summary_panel_->Render();
    }

    void EndGameSummary::Shutdown() {
        summary_panel_.reset();
    }

    bool EndGameSummary::ShouldShow() const {
        return game_state_ && game_state_->rating.stars >= 5;
    }

    void EndGameSummary::RenderDimOverlay() {
        // Dim the background
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(engine::ui::Rectangle(0, 0, screen_width, screen_height),
                                              UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
    }

    void EndGameSummary::Initialize() {
        if (!game_state_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        const int panel_x = (screen_width - BOX_WIDTH) / 2;
        const int panel_y = (screen_height - BOX_HEIGHT) / 2;
        summary_panel_ = std::make_unique<engine::ui::elements::Panel>();
        summary_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        summary_panel_->SetSize(static_cast<float>(BOX_WIDTH), static_cast<float>(BOX_HEIGHT));
        summary_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        summary_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        summary_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        summary_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        const auto &rating = game_state_->rating;

        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "CONGRATULATIONS!",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(GOLD)
        ));
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "*****",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(GOLD)
        ));
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "You've achieved the maximum",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(WHITE)
        ));
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "5-star tower rating!",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(WHITE)
        ));

        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "Final Statistics:",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(SKYBLUE)
        ));

        std::stringstream stats_ss;
        stats_ss << "  Tenants: " << rating.total_tenants;
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            stats_ss.str(),
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(LIGHTGRAY)
        ));

        stats_ss.str("");
        stats_ss << "  Floors: " << rating.total_floors;
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            stats_ss.str(),
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(LIGHTGRAY)
        ));

        stats_ss.str("");
        stats_ss << "  Satisfaction: " << std::fixed << std::setprecision(0) << rating.average_satisfaction << "%";
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            stats_ss.str(),
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(LIGHTGRAY)
        ));

        stats_ss.str("");
        stats_ss << "  Income: $" << std::fixed << std::setprecision(0) << rating.hourly_income << "/hr";
        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            stats_ss.str(),
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(GREEN)
        ));

        summary_panel_->AddChild(std::make_unique<Text>(
            0, 0,
            "(Continue playing to build more!)",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(GRAY)
        ));
    }
} // namespace towerforge::ui
