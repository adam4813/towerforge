#include "ui/hud/hud.h"
#include "ui/hud/top_bar.h"
#include "ui/hud/star_rating_panel.h"
#include "ui/hud/end_game_summary.h"
#include "ui/ui_window_manager.h"
#include "ui/info_windows.h"
#include "ui/tooltip.h"
#include "ui/notification_center.h"
#include "ui/analytics_overlay.h"
#include "ui/action_bar.h"
#include "ui/speed_control_panel.h"
#include "ui/mouse_interface.h"
#include "ui/ui_element.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <variant>

namespace towerforge::ui {
    HUD::HUD()
        : action_bar_callback_(nullptr) {
        window_manager_ = std::make_unique<UIWindowManager>();
        tooltip_manager_ = std::make_unique<TooltipManager>();
        notification_center_ = std::make_unique<NotificationCenter>();

        // Create action bar - position will be set in Update()
        action_bar_ = std::make_unique<ActionBar>();
        action_bar_->Initialize();

        // Create speed control panel - position will be set in Update()
        speed_control_panel_ = std::make_unique<SpeedControlPanel>(0, 0, 0, 0);
        speed_control_panel_->SetGameState(&game_state_);

        // Create composed HUD components
        top_bar_ = std::make_unique<TopBar>();
        top_bar_->SetNotificationCenter(notification_center_.get());
        top_bar_->SetGameState(&game_state_);
        top_bar_->SetIncomeClickCallback([this]() {
            RequestIncomeAnalytics();
        });
        top_bar_->SetPopulationClickCallback([this]() {
            RequestPopulationAnalytics();
        });
        top_bar_->SetNotificationClickCallback([this]() {
            ToggleNotificationCenter();
        });

        star_rating_panel_ = std::make_unique<StarRatingPanel>();
        star_rating_panel_->SetGameState(&game_state_);

        end_game_summary_ = std::make_unique<EndGameSummary>();
        end_game_summary_->SetGameState(&game_state_);
    }

    HUD::~HUD() = default;

    void HUD::Update(const float delta_time) {
        // Update notification center
        notification_center_->Update(delta_time);

        // Update window manager (handles repositioning on resize)
        window_manager_->Update(delta_time);

        // Update current info window and reposition on screen resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        std::visit([delta_time, screen_width, screen_height](auto& window) {
            using T = std::decay_t<decltype(window)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                if (window) {
                    window->Update(delta_time);
                    // Reposition to stay centered at bottom
                    const float x = (screen_width - window->GetWidth()) / 2.0f;
                    const float y = screen_height - window->GetHeight() - 60.0f;
                    window->SetPosition(x, y);
                }
            }
        }, current_info_window_);

        // Update composed HUD components (they manage their own position/size)
        if (action_bar_) {
            action_bar_->Update(delta_time);
        }

        if (speed_control_panel_) {
            speed_control_panel_->Update(delta_time);
        }

        if (top_bar_) {
            top_bar_->Update(delta_time);
        }

        if (star_rating_panel_) {
            star_rating_panel_->Update(delta_time);
        }

        if (end_game_summary_) {
            end_game_summary_->Update(delta_time);
        }
    }

    void HUD::Render() {
        // Render composed HUD components
        if (top_bar_) {
            top_bar_->Render();
        }

        if (star_rating_panel_) {
            star_rating_panel_->Render();
        }

        // Render all info windows through the window manager
        window_manager_->Render();

        // Render current info window
        std::visit([](const auto& window) {
            using T = std::decay_t<decltype(window)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                if (window && window->IsVisible()) {
                    window->Render();
                }
            }
        }, current_info_window_);

        // Render notification center toasts in upper-right
        notification_center_->RenderToasts();

        // Render notification center panel if visible
        notification_center_->Render();

        // Render speed controls in lower-left
        if (speed_control_panel_) {
            speed_control_panel_->Render();
        }

        // Render tooltips on top
        tooltip_manager_->Render();

        // Render action bar
        if (action_bar_) {
            action_bar_->Render();
        }

        // Render end-game summary if max stars achieved
        if (end_game_summary_) {
            end_game_summary_->Render();
        }
    }

    void HUD::SetGameState(const GameState &state) {
        game_state_ = state;
    }

    void HUD::ShowFacilityInfo(const FacilityInfo &info) const {
        // Create a new facility window
        auto window = std::make_unique<FacilityWindow>(info);
        
        // Position at bottom center
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float x = (screen_width - window->GetWidth()) / 2.0f;
        const float y = screen_height - window->GetHeight() - 60.0f;  // 60px margin for action bar
        window->SetPosition(x, y);
        
        current_info_window_ = std::move(window);
    }

    void HUD::ShowPersonInfo(const PersonInfo &info) const {
        // Create a new person window
        auto window = std::make_unique<PersonWindow>(info);
        
        // Position at bottom center
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float x = (screen_width - window->GetWidth()) / 2.0f;
        const float y = screen_height - window->GetHeight() - 60.0f;
        window->SetPosition(x, y);
        
        current_info_window_ = std::move(window);
    }

    void HUD::ShowElevatorInfo(const ElevatorInfo &info) const {
        // Create a new elevator window
        auto window = std::make_unique<ElevatorWindow>(info);
        
        // Position at bottom center
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float x = (screen_width - window->GetWidth()) / 2.0f;
        const float y = screen_height - window->GetHeight() - 60.0f;
        window->SetPosition(x, y);
        
        current_info_window_ = std::move(window);
    }

    void HUD::HideInfoPanels() const {
        current_info_window_ = std::monostate{};
        window_manager_->Clear();
    }

    void HUD::AddNotification(Notification::Type type, const std::string &message, float duration) {
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


    bool HUD::ProcessMouseEvent(const MouseEvent &event) {
        // Forward to current info window first (highest priority for close button)
        bool info_window_handled = false;
        std::visit([&event, &info_window_handled](auto& window) {
            using T = std::decay_t<decltype(window)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                if (window && window->IsVisible()) {
                    // Convert towerforge MouseEvent to engine MouseEvent
                    engine::ui::MouseEvent engine_event;
                    engine_event.x = event.x;
                    engine_event.y = event.y;
                    engine_event.left_pressed = event.left_pressed;
                    engine_event.left_released = false;  // Not available in towerforge MouseEvent
                    engine_event.right_pressed = event.right_pressed;
                    engine_event.right_released = false;  // Not available in towerforge MouseEvent
                    
                    if (window->ProcessMouseEvent(engine_event)) {
                        info_window_handled = true;
                    }
                }
            }
        }, current_info_window_);
        
        if (info_window_handled) {
            return true;
        }
        
        // Forward to action bar
        if (action_bar_ && action_bar_->ProcessMouseEvent(event)) {
            return true;
        }

        // Forward to speed control panel
        if (speed_control_panel_ && speed_control_panel_->ProcessMouseEvent(event)) {
            return true;
        }

        // Handle notification center clicks
        if (notification_center_->IsVisible() && event.left_pressed) {
            if (const_cast<NotificationCenter *>(notification_center_.get())->HandleClick(
                static_cast<int>(event.x), static_cast<int>(event.y))) {
                return true;
            }
        }

        // Forward to top bar
        if (top_bar_ && top_bar_->ProcessMouseEvent(event)) {
            return true;
        }

        // Consume any other clicks on top bar
        if (event.left_pressed && event.y <= TOP_BAR_HEIGHT) {
            return true;
        }

        // Handle info window clicks
        if (event.left_pressed && window_manager_->HandleClick(static_cast<int>(event.x), static_cast<int>(event.y))) {
            return true;
        }

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

            // Income tooltip
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
                Tooltip tooltip(
                    "Total population in your tower.\nIncreases as you build residential facilities.\nClick for detailed population analytics.");
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
        const int rating_x = screen_width - StarRatingPanel::WIDTH - 10;
        const int rating_y = TOP_BAR_HEIGHT + 10;

        if (mouse_x >= rating_x && mouse_x <= rating_x + StarRatingPanel::WIDTH &&
            mouse_y >= rating_y && mouse_y <= rating_y + StarRatingPanel::HEIGHT) {
            std::stringstream tooltip_text;
            tooltip_text << "Tower Rating System\n";
            tooltip_text << "Earn stars by:\n";
            tooltip_text << "- Increasing tenant count\n";
            tooltip_text << "- Maintaining high satisfaction\n";
            tooltip_text << "- Expanding your tower\n";
            tooltip_text << "- Generating revenue";
            Tooltip tooltip(tooltip_text.str());
            tooltip_manager_->ShowTooltip(tooltip, rating_x, rating_y, StarRatingPanel::WIDTH, StarRatingPanel::HEIGHT);
            return;
        }

        // Check speed controls
        const int speed_width = SpeedControlPanel::CalculateWidth();
        const int speed_height = SpeedControlPanel::CalculateHeight();
        const int speed_x = 10;
        const int speed_y = screen_height - speed_height - 10;

        if (mouse_x >= speed_x && mouse_x <= speed_x + speed_width &&
            mouse_y >= speed_y && mouse_y <= screen_height - 10) {
            const int button_width = SpeedControlPanel::CalculateButtonWidth();
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

    void HUD::ToggleNotificationCenter() const {
        notification_center_->ToggleVisibility();
    }

    void HUD::ShowIncomeAnalytics(const IncomeBreakdown &data) const {
        auto window = std::make_unique<IncomeAnalyticsOverlay>(data);
        window_manager_->AddAnalyticsWindow(std::move(window));
    }

    void HUD::ShowElevatorAnalytics(const ElevatorAnalytics &data) const {
        auto window = std::make_unique<ElevatorAnalyticsOverlay>(data);
        window_manager_->AddAnalyticsWindow(std::move(window));
    }

    void HUD::ShowPopulationAnalytics(const PopulationBreakdown &data) const {
        auto window = std::make_unique<PopulationAnalyticsOverlay>(data);
        window_manager_->AddAnalyticsWindow(std::move(window));
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
