#include "ui/hud/star_rating_panel.h"
#include "ui/hud/hud.h"
#include "ui/ui_theme.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    StarRatingPanel::StarRatingPanel()
        : game_state_(nullptr)
          , last_screen_width_(0)
          , last_screen_height_(0) {
    }

    void StarRatingPanel::Initialize() {
        using namespace engine::ui::elements;

        panel_ = std::make_unique<Panel>();
        panel_->SetSize(static_cast<float>(WIDTH), static_cast<float>(HEIGHT));
        panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.8f)));
        panel_->SetBorderColor(UITheme::ToEngineColor(GOLD));
        panel_->SetBorderWidth(2.0f);
        panel_->SetPadding(static_cast<float>(PADDING));

        UpdateLayout();
    }

    void StarRatingPanel::Update(float delta_time) {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void StarRatingPanel::UpdateLayout() {
        if (!panel_) return;

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);
        constexpr int top_bar_height = 40; // TopBar::HEIGHT

        panel_->SetRelativePosition(
            static_cast<float>(screen_width - WIDTH - 10),
            static_cast<float>(top_bar_height + 10)
        );

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void StarRatingPanel::Render() const {
        if (panel_) {
            panel_->Render();
        }
        RenderContent();
    }

    void StarRatingPanel::RenderContent() const {
        if (!game_state_ || !panel_) return;

        const auto bounds = panel_->GetAbsoluteBounds();
        const int panel_x = static_cast<int>(bounds.x);
        const int panel_y = static_cast<int>(bounds.y);
        int x = panel_x + PADDING;
        int y = panel_y + PADDING;

        const auto &rating = game_state_->rating;

        // Title with stars
        std::string stars_display;
        for (int i = 0; i < 5; i++) {
            if (i < rating.stars) {
                stars_display += "*";
            } else {
                stars_display += "o";
            }
        }

        engine::ui::BatchRenderer::SubmitText(stars_display, static_cast<float>(x), static_cast<float>(y),
                                              20, UITheme::ToEngineColor(GOLD));
        engine::ui::BatchRenderer::SubmitText("Tower Rating", static_cast<float>(x + 110), static_cast<float>(y + 2),
                                              16, UITheme::ToEngineColor(WHITE));
        y += 30;

        // Satisfaction
        std::stringstream sat_ss;
        sat_ss << "Satisfaction: " << std::fixed << std::setprecision(0)
                << rating.average_satisfaction << "%";
        engine::ui::BatchRenderer::SubmitText(sat_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                              14, UITheme::ToEngineColor(LIGHTGRAY));
        y += 20;

        // Tenants
        std::stringstream tenants_ss;
        tenants_ss << "Tenants: " << rating.total_tenants;
        engine::ui::BatchRenderer::SubmitText(tenants_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                              14, UITheme::ToEngineColor(LIGHTGRAY));
        y += 20;

        // Floors
        std::stringstream floors_ss;
        floors_ss << "Floors: " << rating.total_floors;
        engine::ui::BatchRenderer::SubmitText(floors_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                              14, UITheme::ToEngineColor(LIGHTGRAY));
        y += 20;

        // Income
        std::stringstream income_ss;
        income_ss << "Income: $" << std::fixed << std::setprecision(0)
                << rating.hourly_income << "/hr";
        engine::ui::BatchRenderer::SubmitText(income_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                              14, UITheme::ToEngineColor(GREEN));
        y += 25;

        // Next milestone (only if not at max stars)
        if (rating.stars < 5) {
            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(panel_x + 5), static_cast<float>(y),
                                      static_cast<float>(WIDTH - 10), 1.0f),
                UITheme::ToEngineColor(DARKGRAY)
            );
            y += 10;

            engine::ui::BatchRenderer::SubmitText("Next star:", static_cast<float>(x), static_cast<float>(y),
                                                  14, UITheme::ToEngineColor(YELLOW));
            y += 20;

            // Show the most relevant requirement
            if (rating.next_star_tenants > 0) {
                if (const int needed = rating.next_star_tenants - rating.total_tenants; needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  +" << needed << " tenants";
                    engine::ui::BatchRenderer::SubmitText(next_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                                          12, UITheme::ToEngineColor(GRAY));
                    y += 18;
                }
            }

            if (rating.next_star_satisfaction > 0) {
                if (const float needed = rating.next_star_satisfaction - rating.average_satisfaction;
                    needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  " << std::fixed << std::setprecision(0)
                            << needed << "% satisfaction";
                    engine::ui::BatchRenderer::SubmitText(next_ss.str(), static_cast<float>(x), static_cast<float>(y),
                                                          12, UITheme::ToEngineColor(GRAY));
                }
            }
        } else {
            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(static_cast<float>(panel_x + 5), static_cast<float>(y),
                                      static_cast<float>(WIDTH - 10), 1.0f),
                UITheme::ToEngineColor(GOLD)
            );
            y += 10;
            engine::ui::BatchRenderer::SubmitText("MAX RATING!", static_cast<float>(x + 45), static_cast<float>(y),
                                                  16, UITheme::ToEngineColor(GOLD));
        }
    }

    bool StarRatingPanel::IsMaxRating() const {
        return game_state_ && game_state_->rating.stars >= 5;
    }
} // namespace towerforge::ui
