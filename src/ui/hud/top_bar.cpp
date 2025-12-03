#include "ui/hud/top_bar.h"
#include "ui/hud/hud.h"
#include "ui/ui_element.h"
#include "ui/notification_center.h"
#include <sstream>
#include <iomanip>

namespace towerforge::ui {

    TopBar::TopBar()
        : Panel(0, 0, static_cast<float>(GetScreenWidth()), HEIGHT,
                ColorAlpha(BLACK, 0.7f), BLANK)
        , game_state_(nullptr)
        , notification_center_(nullptr)
        , income_button_(nullptr)
        , population_button_(nullptr)
        , notification_button_(nullptr) {
        BuildButtons();
    }

    void TopBar::BuildButtons() {
        // Income button (left side)
        auto income_btn = std::make_unique<Button>(
            10, 5, 280, 30, "",
            ColorAlpha(BLACK, 0.0f),
            ColorAlpha(GREEN, 0.2f)
        );
        income_btn->SetClickCallback([this]() {
            if (income_click_callback_) income_click_callback_();
        });
        income_button_ = income_btn.get();
        AddChild(std::move(income_btn));

        // Population button (center)
        auto pop_btn = std::make_unique<Button>(
            310, 5, 180, 30, "",
            ColorAlpha(BLACK, 0.0f),
            ColorAlpha(WHITE, 0.2f)
        );
        pop_btn->SetClickCallback([this]() {
            if (population_click_callback_) population_click_callback_();
        });
        population_button_ = pop_btn.get();
        AddChild(std::move(pop_btn));

        // Notification button (right side - position updated in Update)
        auto notif_btn = std::make_unique<Button>(
            static_cast<float>(GetScreenWidth() - 80), 5, 70, 30, "",
            ColorAlpha(DARKGRAY, 1.0f),
            GOLD
        );
        notif_btn->SetClickCallback([this]() {
            if (notification_click_callback_) notification_click_callback_();
        });
        notification_button_ = notif_btn.get();
        AddChild(std::move(notif_btn));
    }

    void TopBar::Update(float delta_time) {
        Panel::Update(delta_time);

        // Update size on resize
        const int screen_width = GetScreenWidth();
        SetSize(static_cast<float>(screen_width), HEIGHT);

        // Update notification button position and color
        if (notification_button_) {
            notification_button_->SetRelativePosition(
                static_cast<float>(screen_width - 80), 5);

            // Update color based on notification center visibility
            const bool nc_visible = notification_center_ && notification_center_->IsVisible();
            const Color button_color = nc_visible ? GOLD : ColorAlpha(DARKGRAY, 1.0f);
            notification_button_->SetBackgroundColor(button_color);
        }

        // Update child buttons
        if (income_button_) income_button_->Update(delta_time);
        if (population_button_) population_button_->Update(delta_time);
        if (notification_button_) notification_button_->Update(delta_time);
    }

    void TopBar::Render() const {
        const int screen_width = GetScreenWidth();

        // Draw background
        DrawRectangle(0, 0, screen_width, HEIGHT, ColorAlpha(BLACK, 0.7f));
        DrawRectangle(0, HEIGHT - 2, screen_width, 2, GOLD);

        // Render content (text)
        RenderContent();

        // Render child buttons
        Panel::Render();

        // Render custom notification button content
        if (notification_button_) {
            const int notif_button_x = screen_width - 80;
            constexpr int notif_button_y = 5;

            DrawText("N", notif_button_x + 10, notif_button_y + 7, 16, WHITE);

            if (notification_center_) {
                const int unread_count = notification_center_->GetUnreadCount();
                if (unread_count > 0) {
                    const int badge_x = notif_button_x + 50;
                    const int badge_y = notif_button_y + 10;
                    DrawCircle(badge_x, badge_y, 10, RED);
                    std::string count_str = std::to_string(unread_count);
                    if (unread_count > 99) count_str = "99+";
                    const int text_width = MeasureText(count_str.c_str(), 10);
                    DrawText(count_str.c_str(), badge_x - text_width / 2, badge_y - 5, 10, WHITE);
                }
            }
        }
    }

    void TopBar::RenderContent() const {
        if (!game_state_) return;

        int x = 10;
        constexpr int y = 10;

        // Draw funds
        std::stringstream funds_ss;
        funds_ss << std::fixed << std::setprecision(0);
        std::string income_sign = game_state_->income_rate >= 0 ? "+" : "";
        funds_ss << "$" << game_state_->funds << " (" << income_sign << "$" << game_state_->income_rate << "/hr)";
        DrawText(funds_ss.str().c_str(), x, y, 20, GREEN);

        // Draw population
        x += 300;
        std::stringstream pop_ss;
        pop_ss << "Population: " << game_state_->population;
        DrawText(pop_ss.str().c_str(), x, y, 20, WHITE);

        // Draw time
        x += 200;
        std::stringstream time_ss;
        time_ss << FormatTime(game_state_->current_time) << " Day " << game_state_->current_day;
        DrawText(time_ss.str().c_str(), x, y, 20, SKYBLUE);

        // Draw speed indicator
        x += 200;
        std::string speed_text;
        if (game_state_->paused) {
            speed_text = "PAUSED";
        } else {
            speed_text = std::to_string(game_state_->speed_multiplier) + "x";
        }
        DrawText(speed_text.c_str(), x, y, 20, game_state_->paused ? RED : YELLOW);
    }

    std::string TopBar::FormatTime(const float time) {
        const int hours = static_cast<int>(time);
        const int minutes = static_cast<int>((time - hours) * 60);

        const std::string period = hours >= 12 ? " PM" : " AM";
        int display_hours = hours % 12;
        if (display_hours == 0) display_hours = 12;

        std::stringstream result;
        result << display_hours << ":" << std::setfill('0') << std::setw(2) << minutes << period;

        return result.str();
    }

} // namespace towerforge::ui
