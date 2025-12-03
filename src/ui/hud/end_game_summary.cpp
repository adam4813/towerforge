#include "ui/hud/end_game_summary.h"
#include "ui/hud/hud.h"
#include <sstream>
#include <iomanip>

namespace towerforge::ui {

    EndGameSummary::EndGameSummary()
        : Panel(0, 0, BOX_WIDTH, BOX_HEIGHT, ColorAlpha(BLACK, 0.95f), BLANK)
        , game_state_(nullptr) {
        UpdatePosition();
    }

    void EndGameSummary::Update(float delta_time) {
        Panel::Update(delta_time);
        UpdatePosition();
    }

    void EndGameSummary::UpdatePosition() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        SetRelativePosition(
            static_cast<float>((screen_width - BOX_WIDTH) / 2),
            static_cast<float>((screen_height - BOX_HEIGHT) / 2)
        );
    }

    void EndGameSummary::Render() const {
        if (!ShouldShow()) return;

        RenderOverlay();
        RenderContent();
    }

    bool EndGameSummary::ShouldShow() const {
        return game_state_ && game_state_->rating.stars >= 5;
    }

    void EndGameSummary::RenderOverlay() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent overlay
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
    }

    void EndGameSummary::RenderContent() const {
        if (!game_state_) return;

        const Rectangle bounds = GetPanelBounds();
        const int box_x = static_cast<int>(bounds.x);
        const int box_y = static_cast<int>(bounds.y);

        // Draw box background
        DrawRectangle(box_x, box_y, BOX_WIDTH, BOX_HEIGHT, ColorAlpha(BLACK, 0.95f));
        DrawRectangle(box_x, box_y, BOX_WIDTH, 3, GOLD);
        DrawRectangle(box_x, box_y + BOX_HEIGHT - 3, BOX_WIDTH, 3, GOLD);

        const int x = box_x + 20;
        int y = box_y + 20;

        const auto& rating = game_state_->rating;

        // Title
        DrawText("CONGRATULATIONS!", x + 50, y, 24, GOLD);
        y += 40;

        // Stars
        DrawText("*****", x + 140, y, 32, GOLD);
        y += 50;

        // Achievement message
        DrawText("You've achieved the maximum", x + 40, y, 16, WHITE);
        y += 25;
        DrawText("5-star tower rating!", x + 90, y, 16, WHITE);
        y += 40;

        // Final statistics
        DrawText("Final Statistics:", x + 20, y, 14, SKYBLUE);
        y += 25;

        std::stringstream stats_ss;
        stats_ss << "  Tenants: " << rating.total_tenants;
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Floors: " << rating.total_floors;
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Satisfaction: " << std::fixed << std::setprecision(0)
                 << rating.average_satisfaction << "%";
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Income: $" << std::fixed << std::setprecision(0)
                 << rating.hourly_income << "/hr";
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, GREEN);
        y += 30;

        // Continue message
        DrawText("(Continue playing to build more!)", x + 55, y, 12, GRAY);
    }

} // namespace towerforge::ui
