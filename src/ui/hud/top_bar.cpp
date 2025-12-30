#include "ui/hud/top_bar.h"
#include "ui/hud/hud.h"
#include "ui/ui_theme.h"
#include "ui/notification_center.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    TopBar::TopBar()
        : game_state_(nullptr)
          , notification_center_(nullptr)
          , income_button_(nullptr)
          , population_button_(nullptr)
          , notification_button_(nullptr)
          , funds_text_(nullptr)
          , population_text_(nullptr)
          , time_text_(nullptr)
          , speed_text_(nullptr)
          , badge_text_(nullptr)
          , last_screen_width_(0) {
    }

    void TopBar::Initialize() {
        BuildPanel();
    }

    void TopBar::BuildPanel() {
        const int screen_width = GetScreenWidth();
        constexpr int TEXT_Y = 10;
        constexpr int FONT_SIZE = 20;

        panel_ = std::make_unique<engine::ui::elements::Panel>();
        panel_->SetRelativePosition(0, 0);
        panel_->SetSize(static_cast<float>(screen_width), static_cast<float>(HEIGHT));
        panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
        panel_->SetBorderColor(UITheme::ToEngineColor(GOLD));
        panel_->SetBorderWidth(0.0f); // We'll draw border manually at bottom

        // Funds text
        auto funds = std::make_unique<engine::ui::elements::Text>(10, TEXT_Y, "", FONT_SIZE,
                                                                  UITheme::ToEngineColor(GREEN));
        funds_text_ = funds.get();
        panel_->AddChild(std::move(funds));

        // Population text
        auto population = std::make_unique<engine::ui::elements::Text>(310, TEXT_Y, "", FONT_SIZE,
                                                                       UITheme::ToEngineColor(WHITE));
        population_text_ = population.get();
        panel_->AddChild(std::move(population));

        // Time text
        auto time = std::make_unique<engine::ui::elements::Text>(510, TEXT_Y, "", FONT_SIZE,
                                                                 UITheme::ToEngineColor(SKYBLUE));
        time_text_ = time.get();
        panel_->AddChild(std::move(time));

        // Speed text
        auto speed = std::make_unique<engine::ui::elements::Text>(710, TEXT_Y, "", FONT_SIZE,
                                                                  UITheme::ToEngineColor(YELLOW));
        speed_text_ = speed.get();
        panel_->AddChild(std::move(speed));

        // Income button (left side) - transparent, shows hover
        auto income_btn = std::make_unique<engine::ui::elements::Button>(280, 30, "", UITheme::FONT_SIZE_SMALL);
        income_btn->SetRelativePosition(10, 5);
        income_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.0f)));
        income_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(GREEN, 0.2f)));
        income_btn->SetBorderColor(UITheme::ToEngineColor(BLANK));
        income_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && income_click_callback_) {
                income_click_callback_();
                return true;
            }
            return false;
        });
        income_button_ = income_btn.get();
        panel_->AddChild(std::move(income_btn));

        // Population button (center-left)
        auto pop_btn = std::make_unique<engine::ui::elements::Button>(180, 30, "", UITheme::FONT_SIZE_SMALL);
        pop_btn->SetRelativePosition(310, 5);
        pop_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(BLACK, 0.0f)));
        pop_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(WHITE, 0.2f)));
        pop_btn->SetBorderColor(UITheme::ToEngineColor(BLANK));
        pop_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && population_click_callback_) {
                population_click_callback_();
                return true;
            }
            return false;
        });
        population_button_ = pop_btn.get();
        panel_->AddChild(std::move(pop_btn));

        // Notification button (right side)
        auto notif_btn = std::make_unique<engine::ui::elements::Button>(70, 30, "N", UITheme::FONT_SIZE_MEDIUM);
        notif_btn->SetRelativePosition(static_cast<float>(screen_width - 80), 5);
        notif_btn->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        notif_btn->SetHoverColor(UITheme::ToEngineColor(GOLD));
        notif_btn->SetBorderColor(UITheme::ToEngineColor(BLANK));
        notif_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        notif_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && notification_click_callback_) {
                notification_click_callback_();
                return true;
            }
            return false;
        });
        notification_button_ = notif_btn.get();
        panel_->AddChild(std::move(notif_btn));

        // Badge text for notification count (initially hidden)
        auto badge = std::make_unique<engine::ui::elements::Text>(
            static_cast<float>(screen_width - 30), 10, "", 10, UITheme::ToEngineColor(WHITE));
        badge_text_ = badge.get();
        panel_->AddChild(std::move(badge));

        panel_->InvalidateComponentsRecursive();
        panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
    }

    void TopBar::Update([[maybe_unused]] float delta_time) {
        if (const int screen_width = GetScreenWidth(); screen_width != last_screen_width_) {
            UpdateLayout();
        }

        // Update notification button color based on visibility
        if (notification_button_ && notification_center_) {
            notification_button_->SetNormalColor(
                UITheme::ToEngineColor(notification_center_->IsVisible() ? GOLD : DARKGRAY));
        }

        // Update text elements with current game state
        UpdateTextElements();
    }

    void TopBar::UpdateTextElements() const {
        if (!game_state_) return;

        // Update funds text
        if (funds_text_) {
            std::stringstream funds_ss;
            funds_ss << std::fixed << std::setprecision(0);
            std::string income_sign = game_state_->income_rate >= 0 ? "+" : "";
            funds_ss << "$" << game_state_->funds << " (" << income_sign << "$" << game_state_->income_rate << "/hr)";
            funds_text_->SetText(funds_ss.str());
        }

        // Update population text
        if (population_text_) {
            std::stringstream pop_ss;
            pop_ss << "Population: " << game_state_->population;
            population_text_->SetText(pop_ss.str());
        }

        // Update time text
        if (time_text_) {
            std::stringstream time_ss;
            time_ss << FormatTime(game_state_->current_time) << " Day " << game_state_->current_day;
            time_text_->SetText(time_ss.str());
        }

        // Update speed text
        if (speed_text_) {
            std::string speed_str;
            if (game_state_->paused) {
                speed_str = "PAUSED";
                speed_text_->SetColor(UITheme::ToEngineColor(RED));
            } else {
                speed_str = std::to_string(game_state_->speed_multiplier) + "x";
                speed_text_->SetColor(UITheme::ToEngineColor(YELLOW));
            }
            speed_text_->SetText(speed_str);
        }

        // Update notification badge
        if (badge_text_ && notification_center_) {
            const int unread_count = notification_center_->GetUnreadCount();
            if (unread_count > 0) {
                std::string count_str = std::to_string(unread_count);
                if (unread_count > 99) count_str = "99+";
                badge_text_->SetText(count_str);
            } else {
                badge_text_->SetText("");
            }
        }
    }

    void TopBar::UpdateLayout() {
        if (!panel_) return;

        const int screen_width = GetScreenWidth();
        panel_->SetSize(static_cast<float>(screen_width), static_cast<float>(HEIGHT));

        if (notification_button_) {
            notification_button_->SetRelativePosition(static_cast<float>(screen_width - 80), 5);
        }

        if (badge_text_) {
            badge_text_->SetRelativePosition(static_cast<float>(screen_width - 30), 10);
        }

        panel_->InvalidateComponentsRecursive();
        panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
    }

    bool TopBar::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        return panel_ ? panel_->ProcessMouseEvent(event) : false;
    }

    void TopBar::Render() const {
        if (!panel_) return;

        const int screen_width = GetScreenWidth();

        // Render panel and all children (including text elements)
        panel_->Render();

        // Draw bottom border
        engine::ui::batch_renderer::BatchRenderer::SubmitLine(0, HEIGHT, screen_width, HEIGHT, 2,
                                                              UITheme::ToEngineColor(GOLD));

        // Render notification badge circle (text is rendered by badge_text_)
        if (notification_center_) {
            if (const int unread_count = notification_center_->GetUnreadCount(); unread_count > 0) {
                const int notif_button_x = screen_width - 80;
                const int badge_x = notif_button_x + 50;
                constexpr int badge_y = 15;
                engine::ui::batch_renderer::BatchRenderer::SubmitCircle(
                    badge_x, badge_y, 10, UITheme::ToEngineColor(RED));
            }
        }
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
