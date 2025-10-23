#include "ui/hud.h"
#include "ui/ui_window_manager.h"
#include "ui/info_windows.h"
#include "ui/tooltip.h"
#include "ui/notification_center.h"
#include "ui/analytics_overlay.h"
#include "ui/action_bar.h"
#include "ui/mouse_interface.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge::ui {

    HUD::HUD()
        : action_bar_callback_(nullptr) {
        window_manager_ = std::make_unique<UIWindowManager>();
        tooltip_manager_ = std::make_unique<TooltipManager>();
        notification_center_ = std::make_unique<NotificationCenter>();
        
        // Create action bar at bottom of screen - fixed width, centered
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int bar_width = ActionBar::CalculateBarWidth();
        const int bar_x = (screen_width - bar_width) / 2;
        
        action_bar_ = std::make_unique<ActionBar>(
            bar_x,
            screen_height - ACTION_BAR_HEIGHT,
            bar_width,
            ACTION_BAR_HEIGHT
        );
    }

    HUD::~HUD() = default;

    void HUD::Update(const float delta_time) {
        // Update notifications - remove expired ones
        for (auto it = notifications_.begin(); it != notifications_.end();) {
            it->time_remaining -= delta_time;
            if (it->time_remaining <= 0.0f) {
                it = notifications_.erase(it);
            } else {
                ++it;
            }
        }
    
        // Update notification center
        notification_center_->Update(delta_time);
        
        // Update window manager (handles repositioning on resize)
        window_manager_->Update(delta_time);
        
        // Update action bar
        if (action_bar_) {
            action_bar_->Update(delta_time);
            
            // Update position if screen resized - keep centered
            const int screen_width = GetScreenWidth();
            const int screen_height = GetScreenHeight();
            const int bar_width = ActionBar::CalculateBarWidth();
            const int bar_x = (screen_width - bar_width) / 2;
            action_bar_->SetRelativePosition(bar_x, screen_height - ACTION_BAR_HEIGHT);
        }
    }

    void HUD::Render() {
        RenderTopBar();
        RenderStarRating();

        // Render all info windows through the window manager
        window_manager_->Render();

        // Render legacy notifications (for backward compatibility)
        RenderNotifications();
    
        // Render notification center toasts
        notification_center_->RenderToasts();
    
        // Render notification center panel if visible
        notification_center_->Render();
    
        RenderSpeedControls();

        // Render tooltips on top
        tooltip_manager_->Render();

        // Render action bar
        if (action_bar_) {
            action_bar_->Render();
        }

        // Render end-game summary if max stars achieved
        if (game_state_.rating.stars >= 5) {
            RenderEndGameSummary();
        }
    }

    void HUD::SetGameState(const GameState& state) {
        game_state_ = state;
    }

    void HUD::ShowFacilityInfo(const FacilityInfo& info) const {
        // Create a new facility window and add it to the window manager
        auto window = std::make_unique<FacilityWindow>(info);
        window_manager_->AddInfoWindow(std::move(window));
    }

    void HUD::ShowPersonInfo(const PersonInfo& info) const {
        // Create a new person window and add it to the window manager
        auto window = std::make_unique<PersonWindow>(info);
        window_manager_->AddInfoWindow(std::move(window));
    }

    void HUD::ShowElevatorInfo(const ElevatorInfo& info) const {
        // Create a new elevator window and add it to the window manager
        auto window = std::make_unique<ElevatorWindow>(info);
        window_manager_->AddInfoWindow(std::move(window));
    }

    void HUD::HideInfoPanels() const {
        window_manager_->Clear();
    }

    void HUD::AddNotification(Notification::Type type, const std::string& message, float duration) {
        // Add to legacy notification system for backward compatibility
        notifications_.emplace_back(type, message, duration);
        // Keep only the last 5 notifications
        if (notifications_.size() > 5) {
            notifications_.erase(notifications_.begin());
        }
    
        // Also add to new notification center
        NotificationType nc_type;
        switch (type) {
            case Notification::Type::Warning:
                nc_type = NotificationType::Warning;
                break;
            case Notification::Type::Success:
                nc_type = NotificationType::Success;
                break;
            case Notification::Type::Info:
                nc_type = NotificationType::Info;
                break;
            case Notification::Type::Error:
                nc_type = NotificationType::Error;
                break;
            default:
                nc_type = NotificationType::Info;
                break;
        }
    
        notification_center_->AddNotification("Notification", message, nc_type, 
                                               NotificationPriority::Medium, duration);
    }

    bool HUD::HandleClick(const int mouse_x, const int mouse_y) const {
        // Check notification center first if visible (need to cast away const for interaction)
        if (notification_center_->IsVisible()) {
            if (const_cast<NotificationCenter*>(notification_center_.get())->HandleClick(mouse_x, mouse_y)) {
                return true;
            }
        }
    
        // Check if click is in speed controls area (bottom right)
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        const int speed_x = screen_width - SPEED_CONTROL_WIDTH - 10;
        const int speed_y = screen_height - SPEED_CONTROL_HEIGHT - 10;

        if (mouse_x >= speed_x && mouse_x <= screen_width - 10 &&
            mouse_y >= speed_y && mouse_y <= screen_height - 10) {
            // Click on speed controls
            return true;
        }

        // Check if click is on top bar
        if (mouse_y <= TOP_BAR_HEIGHT) {
            // Check income area (left side)
            if (IsMouseOverIncomeArea(mouse_x, mouse_y)) {
                const_cast<HUD*>(this)->RequestIncomeAnalytics();
                return true;
            }
        
            // Check population area
            if (IsMouseOverPopulationArea(mouse_x, mouse_y)) {
                const_cast<HUD*>(this)->RequestPopulationAnalytics();
                return true;
            }
        
            // Check notification center button
            const int notif_button_x = screen_width - 80;
            const int notif_button_y = 5;
            const int notif_button_width = 70;
            const int notif_button_height = 30;
        
            if (mouse_x >= notif_button_x && mouse_x <= notif_button_x + notif_button_width &&
                mouse_y >= notif_button_y && mouse_y <= notif_button_y + notif_button_height) {
                const_cast<HUD*>(this)->ToggleNotificationCenter();
                return true;
            }
        
            return true;
        }

        // Check if click is on any info window
        if (window_manager_->HandleClick(mouse_x, mouse_y)) {
            return true;
        }

        return false;
    }

    bool HUD::ProcessMouseEvent(const MouseEvent& event) {
        // Forward to action bar first
        if (action_bar_ && action_bar_->ProcessMouseEvent(event)) {
            return true;
        }
        
        // Forward to window manager if needed
        // (UIWindowManager doesn't have ProcessMouseEvent yet)
        
        return false;
    }

    void HUD::UpdateTooltips(int mouse_x, int mouse_y) const {
        // Update tooltip manager
        tooltip_manager_->Update(mouse_x, mouse_y);

        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        // Check top bar elements
        if (mouse_y <= TOP_BAR_HEIGHT) {
            int x = 10;

            // Funds tooltip
            if (mouse_x >= x && mouse_x <= x + 280) {
                std::stringstream tooltip_text;
                tooltip_text << "Current funds and hourly income rate.\n";
                tooltip_text << "Build facilities to increase income.\n";
                tooltip_text << "Click for detailed income analytics.";
                Tooltip tooltip(tooltip_text.str());
                tooltip_manager_->ShowTooltip(tooltip, x, 0, 280, TOP_BAR_HEIGHT);
                return;
            }

            x += 300;
            // Population tooltip
            if (mouse_x >= x && mouse_x <= x + 180) {
                Tooltip tooltip("Total population in your tower.\nIncreases as you build residential facilities.\nClick for detailed population analytics.");
                tooltip_manager_->ShowTooltip(tooltip, x, 0, 180, TOP_BAR_HEIGHT);
                return;
            }

            x += 200;
            // Time tooltip
            if (mouse_x >= x && mouse_x <= x + 180) {
                Tooltip tooltip("Current time and day.\nTime progresses based on simulation speed.");
                tooltip_manager_->ShowTooltip(tooltip, x, 0, 180, TOP_BAR_HEIGHT);
                return;
            }

            x += 200;
            // Speed indicator tooltip
            if (mouse_x >= x && mouse_x <= x + 100) {
                std::stringstream tooltip_text;
                if (game_state_.paused) {
                    tooltip_text << "Simulation is PAUSED.\nUse speed controls to resume.";
                } else {
                    tooltip_text << "Current simulation speed: " << game_state_.speed_multiplier << "x\n";
                    tooltip_text << "Use speed controls to adjust.";
                }
                Tooltip tooltip(tooltip_text.str());
                tooltip_manager_->ShowTooltip(tooltip, x, 0, 100, TOP_BAR_HEIGHT);
                return;
            }
        
            // Notification center button tooltip
            int notif_button_x = screen_width - 80;
            if (mouse_x >= notif_button_x && mouse_x <= notif_button_x + 70) {
                std::stringstream tooltip_text;
                tooltip_text << "Notification Center\n";
                tooltip_text << "Hotkey: N\n";
                int unread = notification_center_->GetUnreadCount();
                if (unread > 0) {
                    tooltip_text << unread << " unread notification" << (unread > 1 ? "s" : "");
                }
                Tooltip tooltip(tooltip_text.str());
                tooltip_manager_->ShowTooltip(tooltip, notif_button_x, 0, 70, TOP_BAR_HEIGHT);
                return;
            }
        }

        // Check star rating panel
        int rating_x = screen_width - STAR_RATING_WIDTH - 10;
        int rating_y = TOP_BAR_HEIGHT + 10;

        if (mouse_x >= rating_x && mouse_x <= rating_x + STAR_RATING_WIDTH &&
            mouse_y >= rating_y && mouse_y <= rating_y + STAR_RATING_HEIGHT) {
            std::stringstream tooltip_text;
            tooltip_text << "Tower Rating System\n";
            tooltip_text << "Earn stars by:\n";
            tooltip_text << "- Increasing tenant count\n";
            tooltip_text << "- Maintaining high satisfaction\n";
            tooltip_text << "- Expanding your tower\n";
            tooltip_text << "- Generating revenue";
            Tooltip tooltip(tooltip_text.str());
            tooltip_manager_->ShowTooltip(tooltip, rating_x, rating_y, STAR_RATING_WIDTH, STAR_RATING_HEIGHT);
            return;
        }

        // Check speed controls
        int speed_x = screen_width - SPEED_CONTROL_WIDTH - 10;
        int speed_y = screen_height - SPEED_CONTROL_HEIGHT - 10;

        if (mouse_x >= speed_x && mouse_x <= screen_width - 10 &&
            mouse_y >= speed_y && mouse_y <= screen_height - 10) {

            int button_width = 45;
            int button_x = speed_x + 5;

            // Pause button
            if (mouse_x >= button_x && mouse_x <= button_x + button_width) {
                Tooltip tooltip("Pause/Resume simulation\nHotkey: SPACE");
                tooltip_manager_->ShowTooltip(tooltip, button_x, speed_y + 5, button_width, 30);
                return;
            }

            // 1x button
            button_x += button_width + 5;
            if (mouse_x >= button_x && mouse_x <= button_x + button_width) {
                Tooltip tooltip("Set simulation speed to 1x (normal speed)");
                tooltip_manager_->ShowTooltip(tooltip, button_x, speed_y + 5, button_width, 30);
                return;
            }

            // 2x button
            button_x += button_width + 5;
            if (mouse_x >= button_x && mouse_x <= button_x + button_width) {
                Tooltip tooltip("Set simulation speed to 2x (fast speed)");
                tooltip_manager_->ShowTooltip(tooltip, button_x, speed_y + 5, button_width, 30);
                return;
            }

            // 4x button
            button_x += button_width + 5;
            if (mouse_x >= button_x && mouse_x <= button_x + button_width) {
                Tooltip tooltip("Set simulation speed to 4x (very fast speed)");
                tooltip_manager_->ShowTooltip(tooltip, button_x, speed_y + 5, button_width, 30);
                return;
            }
        }

        // No tooltip to show
        tooltip_manager_->HideTooltip();
    }

    void HUD::RenderTopBar() const {
        int screen_width = GetScreenWidth();

        // Draw top bar background
        DrawRectangle(0, 0, screen_width, TOP_BAR_HEIGHT, ColorAlpha(BLACK, 0.7f));
        DrawRectangle(0, TOP_BAR_HEIGHT - 2, screen_width, 2, GOLD);

        int x = 10;
        int y = 10;

        // Draw funds
        std::stringstream funds_ss;
        funds_ss << std::fixed << std::setprecision(0);
        std::string income_sign = game_state_.income_rate >= 0 ? "+" : "";
        funds_ss << "$" << game_state_.funds << " (" << income_sign << "$" << game_state_.income_rate << "/hr)";
        DrawText(funds_ss.str().c_str(), x, y, 20, GREEN);

        // Draw population
        x += 300;
        std::stringstream pop_ss;
        pop_ss << "Population: " << game_state_.population;
        DrawText(pop_ss.str().c_str(), x, y, 20, WHITE);

        // Draw time
        x += 200;
        std::stringstream time_ss;
        time_ss << FormatTime(game_state_.current_time) << " Day " << game_state_.current_day;
        DrawText(time_ss.str().c_str(), x, y, 20, SKYBLUE);

        // Draw speed indicator
        x += 200;
        std::string speed_text;
        if (game_state_.paused) {
            speed_text = "PAUSED";
        } else {
            speed_text = std::to_string(game_state_.speed_multiplier) + "x";
        }
        DrawText(speed_text.c_str(), x, y, 20, game_state_.paused ? RED : YELLOW);
    
        // Draw notification center button
        const int notif_button_x = screen_width - 80;
        const int notif_button_y = 5;
        const int notif_button_width = 70;
        const int notif_button_height = 30;
    
        // Button background
        const Color button_color = notification_center_->IsVisible() ? GOLD : DARKGRAY;
        DrawRectangle(notif_button_x, notif_button_y, notif_button_width, notif_button_height, button_color);
        DrawRectangleLines(notif_button_x, notif_button_y, notif_button_width, notif_button_height, WHITE);
    
        // Notification icon and count
        const int unread_count = notification_center_->GetUnreadCount();
        DrawText("N", notif_button_x + 10, notif_button_y + 7, 16, WHITE);
    
        if (unread_count > 0) {
            // Draw badge with count
            const int badge_x = notif_button_x + 50;
            const int badge_y = notif_button_y + 10;
            DrawCircle(badge_x, badge_y, 10, RED);
            std::string count_str = std::to_string(unread_count);
            if (unread_count > 99) count_str = "99+";
            const int text_width = MeasureText(count_str.c_str(), 10);
            DrawText(count_str.c_str(), badge_x - text_width / 2, badge_y - 5, 10, WHITE);
        }
    }

    void HUD::RenderStarRating() const {
        int screen_width = GetScreenWidth();
        int panel_x = screen_width - STAR_RATING_WIDTH - 10;
        int panel_y = TOP_BAR_HEIGHT + 10;

        // Draw panel background
        DrawRectangle(panel_x, panel_y, STAR_RATING_WIDTH, STAR_RATING_HEIGHT, ColorAlpha(BLACK, 0.8f));
        DrawRectangle(panel_x, panel_y, STAR_RATING_WIDTH, 2, GOLD);

        int x = panel_x + PANEL_PADDING;
        int y = panel_y + PANEL_PADDING;

        // Title with stars
        std::string stars_display;
        for (int i = 0; i < 5; i++) {
            if (i < game_state_.rating.stars) {
                stars_display += "*";  // Filled star
            } else {
                stars_display += "o";  // Empty star (using 'o' as placeholder)
            }
        }

        DrawText(stars_display.c_str(), x, y, 20, GOLD);
        DrawText("Tower Rating", x + 110, y + 2, 16, WHITE);
        y += 30;

        // Satisfaction
        std::stringstream sat_ss;
        sat_ss << "Satisfaction: " << std::fixed << std::setprecision(0)
                << game_state_.rating.average_satisfaction << "%";
        DrawText(sat_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Tenants
        std::stringstream tenants_ss;
        tenants_ss << "Tenants: " << game_state_.rating.total_tenants;
        DrawText(tenants_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Floors
        std::stringstream floors_ss;
        floors_ss << "Floors: " << game_state_.rating.total_floors;
        DrawText(floors_ss.str().c_str(), x, y, 14, LIGHTGRAY);
        y += 20;

        // Income
        std::stringstream income_ss;
        income_ss << "Income: $" << std::fixed << std::setprecision(0)
                << game_state_.rating.hourly_income << "/hr";
        DrawText(income_ss.str().c_str(), x, y, 14, GREEN);
        y += 25;

        // Next milestone (only if not at max stars)
        if (game_state_.rating.stars < 5) {
            DrawRectangle(panel_x + 5, y, STAR_RATING_WIDTH - 10, 1, DARKGRAY);
            y += 10;

            std::string next_star = "Next star:";
            DrawText(next_star.c_str(), x, y, 14, YELLOW);
            y += 20;

            // Show the most relevant requirement
            if (game_state_.rating.next_star_tenants > 0) {
                if (int needed = game_state_.rating.next_star_tenants - game_state_.rating.total_tenants; needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  +" << needed << " tenants";
                    DrawText(next_ss.str().c_str(), x, y, 12, GRAY);
                    y += 18;
                }
            }

            if (game_state_.rating.next_star_satisfaction > 0) {
                if (float needed = game_state_.rating.next_star_satisfaction - game_state_.rating.average_satisfaction; needed > 0) {
                    std::stringstream next_ss;
                    next_ss << "  " << std::fixed << std::setprecision(0)
                            << needed << "% satisfaction";
                    DrawText(next_ss.str().c_str(), x, y, 12, GRAY);
                    y += 18;
                }
            }
        } else {
            DrawRectangle(panel_x + 5, y, STAR_RATING_WIDTH - 10, 1, GOLD);
            y += 10;
            DrawText("MAX RATING!", x + 45, y, 16, GOLD);
        }
    }

    void HUD::RenderNotifications() {
        const int screen_height = GetScreenHeight();
        constexpr int x = 10;
        int y = screen_height - 10;

        // Render notifications from bottom to top
        for (auto it = notifications_.rbegin(); it != notifications_.rend(); ++it) {
            y -= NOTIFICATION_HEIGHT + 5;

            Color bg_color;
            const char* icon;

            switch (it->type) {
                case Notification::Type::Warning:
                    bg_color = ColorAlpha(ORANGE, 0.8f);
                    icon = "!";
                    break;
                case Notification::Type::Success:
                    bg_color = ColorAlpha(GREEN, 0.8f);
                    icon = "+";
                    break;
                case Notification::Type::Info:
                    bg_color = ColorAlpha(SKYBLUE, 0.8f);
                    icon = "i";
                    break;
                case Notification::Type::Error:
                    bg_color = ColorAlpha(RED, 0.8f);
                    icon = "X";
                    break;
            }

            DrawRectangle(x, y, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT, bg_color);
            DrawText(icon, x + 5, y + 5, 20, WHITE);
            DrawText(it->message.c_str(), x + 30, y + 7, 14, WHITE);
        }
    }

    void HUD::RenderSpeedControls() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        const int x = screen_width - SPEED_CONTROL_WIDTH - 10;
        const int y = screen_height - SPEED_CONTROL_HEIGHT - 10;

        // Draw background
        DrawRectangle(x, y, SPEED_CONTROL_WIDTH, SPEED_CONTROL_HEIGHT, ColorAlpha(BLACK, 0.7f));

        constexpr int button_width = 45;
        int button_x = x + 5;
        const int button_y = y + 5;

        // Pause button
        const Color pause_color = game_state_.paused ? RED : DARKGRAY;
        DrawRectangle(button_x, button_y, button_width, 30, pause_color);
        DrawText("||", button_x + 15, button_y + 7, 16, WHITE);

        // 1x button
        button_x += button_width + 5;
        const Color speed1_color = (!game_state_.paused && game_state_.speed_multiplier == 1) ? GREEN : DARKGRAY;
        DrawRectangle(button_x, button_y, button_width, 30, speed1_color);
        DrawText("1x", button_x + 12, button_y + 7, 16, WHITE);

        // 2x button
        button_x += button_width + 5;
        const Color speed2_color = (!game_state_.paused && game_state_.speed_multiplier == 2) ? GREEN : DARKGRAY;
        DrawRectangle(button_x, button_y, button_width, 30, speed2_color);
        DrawText("2x", button_x + 12, button_y + 7, 16, WHITE);

        // 4x button
        button_x += button_width + 5;
        const Color speed4_color = (!game_state_.paused && game_state_.speed_multiplier == 4) ? GREEN : DARKGRAY;
        DrawRectangle(button_x, button_y, button_width, 30, speed4_color);
        DrawText("4x", button_x + 12, button_y + 7, 16, WHITE);
    }

    std::string HUD::FormatTime(const float time) {
        const int hours = static_cast<int>(time);
        const int minutes = static_cast<int>((time - hours) * 60);

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << hours << ":"
                << std::setw(2) << minutes;

        const std::string period = hours >= 12 ? " PM" : " AM";
        int display_hours = hours % 12;
        if (display_hours == 0) display_hours = 12;

        std::stringstream result;
        result << display_hours << ":" << std::setfill('0') << std::setw(2) << minutes << period;

        return result.str();
    }

    void HUD::RenderEndGameSummary() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent overlay
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));

        // Summary box
        constexpr int box_width = 400;
        constexpr int box_height = 300;
        const int box_x = (screen_width - box_width) / 2;
        const int box_y = (screen_height - box_height) / 2;

        DrawRectangle(box_x, box_y, box_width, box_height, ColorAlpha(BLACK, 0.95f));
        DrawRectangle(box_x, box_y, box_width, 3, GOLD);
        DrawRectangle(box_x, box_y + box_height - 3, box_width, 3, GOLD);

        const int x = box_x + 20;
        int y = box_y + 20;

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
        std::stringstream stats_ss;
        stats_ss << "Final Statistics:";
        DrawText(stats_ss.str().c_str(), x + 20, y, 14, SKYBLUE);
        y += 25;

        stats_ss.str("");
        stats_ss << "  Tenants: " << game_state_.rating.total_tenants;
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Floors: " << game_state_.rating.total_floors;
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Satisfaction: " << std::fixed << std::setprecision(0)
                << game_state_.rating.average_satisfaction << "%";
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, LIGHTGRAY);
        y += 20;

        stats_ss.str("");
        stats_ss << "  Income: $" << std::fixed << std::setprecision(0)
                << game_state_.rating.hourly_income << "/hr";
        DrawText(stats_ss.str().c_str(), x + 30, y, 14, GREEN);
        y += 30;

        // Continue message
        DrawText("(Continue playing to build more!)", x + 55, y, 12, GRAY);
    }

    void HUD::ToggleNotificationCenter() const {
        notification_center_->ToggleVisibility();
    }

    void HUD::ShowIncomeAnalytics(const IncomeBreakdown& data) const {
        auto window = std::make_unique<IncomeAnalyticsOverlay>(data);
        window_manager_->AddWindow(std::move(window));
    }

    void HUD::ShowElevatorAnalytics(const ElevatorAnalytics& data) const {
        auto window = std::make_unique<ElevatorAnalyticsOverlay>(data);
        window_manager_->AddWindow(std::move(window));
    }

    void HUD::ShowPopulationAnalytics(const PopulationBreakdown& data) const {
        auto window = std::make_unique<PopulationAnalyticsOverlay>(data);
        window_manager_->AddWindow(std::move(window));
    }

    void HUD::SetIncomeAnalyticsCallback(std::function<IncomeBreakdown()> callback) {
        income_analytics_callback_ = std::move(callback);
    }

    void HUD::SetPopulationAnalyticsCallback(std::function<PopulationBreakdown()> callback) {
        population_analytics_callback_ = std::move(callback);
    }

    void HUD::RequestIncomeAnalytics() const {
        if (income_analytics_callback_) {
            const IncomeBreakdown data = income_analytics_callback_();
            ShowIncomeAnalytics(data);
        }
    }

    void HUD::RequestPopulationAnalytics() const {
        if (population_analytics_callback_) {
            const PopulationBreakdown data = population_analytics_callback_();
            ShowPopulationAnalytics(data);
        }
    }

    bool HUD::IsMouseOverIncomeArea(const int mouse_x, const int mouse_y) const {
        // Income is displayed at position x=10, y=10 in top bar
        // Approximately 280 pixels wide
        return mouse_y <= TOP_BAR_HEIGHT && mouse_x >= 10 && mouse_x <= 290;
    }

    bool HUD::IsMouseOverPopulationArea(const int mouse_x, const int mouse_y) const {
        // Population is displayed at position x=310, y=10 in top bar
        // Approximately 180 pixels wide
        return mouse_y <= TOP_BAR_HEIGHT && mouse_x >= 310 && mouse_x <= 490;
    }

    void HUD::SetActionBarCallback(ActionBarCallback callback) {
        action_bar_callback_ = callback;
        
        if (action_bar_) {
            action_bar_->SetActionCallback([this, callback](ActionBar::Action action) {
                if (callback) {
                    callback(static_cast<int>(action));
                }
            });
        }
    }

}
