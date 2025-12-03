#include "ui/hud/star_rating_panel.h"
#include "ui/hud/hud.h"
#include <sstream>
#include <iomanip>

namespace towerforge::ui {

    StarRatingPanel::StarRatingPanel()
        : Panel(0, 0, WIDTH, HEIGHT, ColorAlpha(BLACK, 0.8f), BLANK)
        , game_state_(nullptr) {
        // Position will be updated in Update()
    }

    void StarRatingPanel::Update(float delta_time) {
        Panel::Update(delta_time);

        // Update position on resize (top-right corner)
        const int screen_width = GetScreenWidth();
        constexpr int top_bar_height = 40;  // TopBar::HEIGHT
        SetRelativePosition(
            static_cast<float>(screen_width - WIDTH - 10),
            static_cast<float>(top_bar_height + 10)
        );
    }

    void StarRatingPanel::Render() const {
        const Rectangle bounds = GetPanelBounds();

        // Draw background
        DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            static_cast<int>(bounds.height),
            ColorAlpha(BLACK, 0.8f)
        );
        DrawRectangle(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.width),
            2, GOLD
        );

        RenderContent();
    }

    void StarRatingPanel::RenderContent() const {
        if (!game_state_) return;

        const Rectangle bounds = GetPanelBounds();
        const int panel_x = static_cast<int>(bounds.x);
        const int panel_y = static_cast<int>(bounds.y);
        int x = panel_x + PADDING;
        int y = panel_y + PADDING;

        const auto& rating = game_state_->rating;

        // Title with stars
        std::string stars_display;
        for (int i = 0; i < 5; i++) {
            if (i < rating.stars) {
                stars_display += "*";
            } else {
                stars_display += "o";
            }
        }

        DrawText(stars_display.c_str(), x, y, 20, GOLD);
        DrawText("Tower Rating", x + 110, y + 2, 16, WHITE);
        y += 30;

        // Satisfaction
        std::stringstream sat_ss;
        sat_ss << "Satisfaction: " << std::fixed << std::setprecision(0)
               << rating.average_satisfaction << "%";
        DrawText(sat_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Tenants
        std::stringstream tenants_ss;
        tenants_ss << "Tenants: " << rating.total_tenants;
        DrawText(tenants_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Floors
        std::stringstream floors_ss;
        floors_ss << "Floors: " << rating.total_floors;
        DrawText(floors_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Income
        std::stringstream income_ss;
        income_ss << "Income: $" << std::fixed << std::setprecision(0)
                  << rating.hourly_income << "/hr";
        DrawText(income_ss.str().c_str(), x, y, 14, GREEN);
        y += 25;

        // Next milestone (only if not at max stars)
        if (rating.stars < 5) {
            DrawRectangle(panel_x + 5, y, WIDTH - 10, 1, DARKGRAY);
            y += 10;

            DrawText("Next star:", x, y, 14, YELLOW);
            y += 20;

            // Show the most relevant requirement
            if (rating.next_star_tenants > 0) {
                if (const int needed = rating.next_star_tenants - rating.total_tenants; needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  +" << needed << " tenants";
                    DrawText(next_ss.str().c_str(), x, y, 12, GRAY);
                    y += 18;
                }
            }

            if (rating.next_star_satisfaction > 0) {
                if (const float needed = rating.next_star_satisfaction - rating.average_satisfaction;
                    needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  " << std::fixed << std::setprecision(0)
                            << needed << "% satisfaction";
                    DrawText(next_ss.str().c_str(), x, y, 12, GRAY);
                }
            }
        } else {
            DrawRectangle(panel_x + 5, y, WIDTH - 10, 1, GOLD);
            y += 10;
            DrawText("MAX RATING!", x + 45, y, 16, GOLD);
        }
    }

    bool StarRatingPanel::IsMaxRating() const {
        return game_state_ && game_state_->rating.stars >= 5;
    }

} // namespace towerforge::ui
