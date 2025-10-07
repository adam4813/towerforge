#include "ui/hud.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge {
namespace ui {

HUD::HUD() 
    : show_facility_panel_(false)
    , show_person_panel_(false)
    , show_elevator_panel_(false) {
}

HUD::~HUD() {
}

void HUD::Update(float delta_time) {
    // Update notifications - remove expired ones
    for (auto it = notifications_.begin(); it != notifications_.end();) {
        it->time_remaining -= delta_time;
        if (it->time_remaining <= 0.0f) {
            it = notifications_.erase(it);
        } else {
            ++it;
        }
    }
}

void HUD::Render() {
    RenderTopBar();
    RenderStarRating();
    
    if (show_facility_panel_) {
        RenderFacilityPanel();
    }
    
    if (show_person_panel_) {
        RenderPersonPanel();
    }
    
    if (show_elevator_panel_) {
        RenderElevatorPanel();
    }
    
    RenderNotifications();
    RenderSpeedControls();
    
    // Render end-game summary if max stars achieved
    if (game_state_.rating.stars >= 5) {
        RenderEndGameSummary();
    }
}

void HUD::SetGameState(const GameState& state) {
    game_state_ = state;
}

void HUD::ShowFacilityInfo(const FacilityInfo& info) {
    facility_info_ = info;
    show_facility_panel_ = true;
    show_person_panel_ = false;
    show_elevator_panel_ = false;
}

void HUD::ShowPersonInfo(const PersonInfo& info) {
    person_info_ = info;
    show_person_panel_ = true;
    show_facility_panel_ = false;
    show_elevator_panel_ = false;
}

void HUD::ShowElevatorInfo(const ElevatorInfo& info) {
    elevator_info_ = info;
    show_elevator_panel_ = true;
    show_facility_panel_ = false;
    show_person_panel_ = false;
}

void HUD::HideInfoPanels() {
    show_facility_panel_ = false;
    show_person_panel_ = false;
    show_elevator_panel_ = false;
}

void HUD::AddNotification(Notification::Type type, const std::string& message, float duration) {
    notifications_.emplace_back(type, message, duration);
    // Keep only the last 5 notifications
    if (notifications_.size() > 5) {
        notifications_.erase(notifications_.begin());
    }
}

bool HUD::HandleClick(int mouse_x, int mouse_y) {
    // Check if click is in speed controls area (bottom right)
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    int speed_x = screen_width - SPEED_CONTROL_WIDTH - 10;
    int speed_y = screen_height - SPEED_CONTROL_HEIGHT - 10;
    
    if (mouse_x >= speed_x && mouse_x <= screen_width - 10 &&
        mouse_y >= speed_y && mouse_y <= screen_height - 10) {
        // Click on speed controls
        return true;
    }
    
    // Check if click is on top bar
    if (mouse_y <= TOP_BAR_HEIGHT) {
        return true;
    }
    
    // Check if click is on info panels
    if (show_facility_panel_ || show_person_panel_ || show_elevator_panel_) {
        int panel_x = screen_width - PANEL_WIDTH - 10;
        int panel_y = TOP_BAR_HEIGHT + 10;
        
        if (mouse_x >= panel_x && mouse_x <= screen_width - 10) {
            return true;
        }
    }
    
    return false;
}

void HUD::RenderTopBar() {
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
}

void HUD::RenderStarRating() {
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
            std::stringstream next_ss;
            int needed = game_state_.rating.next_star_tenants - game_state_.rating.total_tenants;
            if (needed > 0) {
                next_ss << "  +" << needed << " tenants";
                DrawText(next_ss.str().c_str(), x, y, 12, GRAY);
                y += 18;
            }
        }
        
        if (game_state_.rating.next_star_satisfaction > 0) {
            std::stringstream next_ss;
            float needed = game_state_.rating.next_star_satisfaction - game_state_.rating.average_satisfaction;
            if (needed > 0) {
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

void HUD::RenderFacilityPanel() {
    int screen_width = GetScreenWidth();
    int panel_x = screen_width - PANEL_WIDTH - 10;
    int panel_y = TOP_BAR_HEIGHT + 10;
    
    // Draw panel background
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, 200, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, 2, SKYBLUE);
    
    int x = panel_x + PANEL_PADDING;
    int y = panel_y + PANEL_PADDING;
    
    // Title
    std::string title = facility_info_.type + " - Floor " + std::to_string(facility_info_.floor);
    DrawText(title.c_str(), x, y, 16, WHITE);
    y += 25;
    
    // Occupancy
    std::string occupancy = "Occupancy: " + std::to_string(facility_info_.occupancy) + 
                           "/" + std::to_string(facility_info_.max_occupancy);
    DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Revenue
    std::stringstream revenue_ss;
    revenue_ss << "Revenue: $" << std::fixed << std::setprecision(0) << facility_info_.revenue << "/hr";
    DrawText(revenue_ss.str().c_str(), x, y, 14, GREEN);
    y += 20;
    
    // Satisfaction
    std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(facility_info_.satisfaction) + 
                              " " + std::to_string(static_cast<int>(facility_info_.satisfaction)) + "%";
    DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Tenants
    std::string tenants = "Tenants: " + std::to_string(facility_info_.tenant_count) + " workers";
    DrawText(tenants.c_str(), x, y, 14, LIGHTGRAY);
    y += 30;
    
    // Buttons (placeholder)
    DrawRectangle(x, y, 100, 25, DARKGRAY);
    DrawText("[Demolish]", x + 5, y + 5, 14, RED);
    
    DrawRectangle(x + 110, y, 100, 25, DARKGRAY);
    DrawText("[Upgrade]", x + 115, y + 5, 14, YELLOW);
}

void HUD::RenderPersonPanel() {
    int screen_width = GetScreenWidth();
    int panel_x = screen_width - PANEL_WIDTH - 10;
    int panel_y = TOP_BAR_HEIGHT + 10;
    
    // Draw panel background
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, 180, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, 2, YELLOW);
    
    int x = panel_x + PANEL_PADDING;
    int y = panel_y + PANEL_PADDING;
    
    // Title
    std::string title = "PERSON #" + std::to_string(person_info_.id);
    DrawText(title.c_str(), x, y, 16, WHITE);
    y += 25;
    
    // State
    std::string state = "State: " + person_info_.state;
    DrawText(state.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Current floor
    std::string current = "Current: Floor " + std::to_string(person_info_.current_floor);
    DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Destination
    std::string dest = "Destination: Floor " + std::to_string(person_info_.destination_floor);
    DrawText(dest.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Wait time
    std::stringstream wait_ss;
    wait_ss << "Wait Time: " << std::fixed << std::setprecision(0) << person_info_.wait_time << "s";
    DrawText(wait_ss.str().c_str(), x, y, 14, person_info_.wait_time > 30 ? RED : LIGHTGRAY);
    y += 20;
    
    // Needs
    std::string needs = "Needs: " + person_info_.needs;
    DrawText(needs.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Satisfaction
    std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(person_info_.satisfaction) + 
                              " " + std::to_string(static_cast<int>(person_info_.satisfaction)) + "%";
    DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
}

void HUD::RenderElevatorPanel() {
    int screen_width = GetScreenWidth();
    int panel_x = screen_width - PANEL_WIDTH - 10;
    int panel_y = TOP_BAR_HEIGHT + 10;
    
    int panel_height = 150 + (static_cast<int>(elevator_info_.queue.size()) * 20);
    
    // Draw panel background
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, panel_height, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(panel_x, panel_y, PANEL_WIDTH, 2, PURPLE);
    
    int x = panel_x + PANEL_PADDING;
    int y = panel_y + PANEL_PADDING;
    
    // Title
    std::string title = "ELEVATOR #" + std::to_string(elevator_info_.id);
    DrawText(title.c_str(), x, y, 16, WHITE);
    y += 25;
    
    // Current floor and direction
    std::string current = "Current Floor: " + std::to_string(elevator_info_.current_floor) + 
                         " " + elevator_info_.direction;
    DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Occupancy
    std::string occupancy = "Occupancy: " + std::to_string(elevator_info_.occupancy) + 
                           "/" + std::to_string(elevator_info_.max_occupancy);
    DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Next stop
    std::string next = "Next Stop: Floor " + std::to_string(elevator_info_.next_stop);
    DrawText(next.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Queue length
    std::string queue = "Queue Length: " + std::to_string(elevator_info_.queue.size());
    DrawText(queue.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Queue details
    for (const auto& [floor, waiting] : elevator_info_.queue) {
        std::string queue_item = "- Floor " + std::to_string(floor) + ": " + 
                                std::to_string(waiting) + " waiting";
        DrawText(queue_item.c_str(), x + 10, y, 12, GRAY);
        y += 20;
    }
}

void HUD::RenderNotifications() {
    int screen_height = GetScreenHeight();
    int x = 10;
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

void HUD::RenderSpeedControls() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    int x = screen_width - SPEED_CONTROL_WIDTH - 10;
    int y = screen_height - SPEED_CONTROL_HEIGHT - 10;
    
    // Draw background
    DrawRectangle(x, y, SPEED_CONTROL_WIDTH, SPEED_CONTROL_HEIGHT, ColorAlpha(BLACK, 0.7f));
    
    int button_width = 45;
    int button_x = x + 5;
    int button_y = y + 5;
    
    // Pause button
    Color pause_color = game_state_.paused ? RED : DARKGRAY;
    DrawRectangle(button_x, button_y, button_width, 30, pause_color);
    DrawText("||", button_x + 15, button_y + 7, 16, WHITE);
    
    // 1x button
    button_x += button_width + 5;
    Color speed1_color = (!game_state_.paused && game_state_.speed_multiplier == 1) ? GREEN : DARKGRAY;
    DrawRectangle(button_x, button_y, button_width, 30, speed1_color);
    DrawText("1x", button_x + 12, button_y + 7, 16, WHITE);
    
    // 2x button
    button_x += button_width + 5;
    Color speed2_color = (!game_state_.paused && game_state_.speed_multiplier == 2) ? GREEN : DARKGRAY;
    DrawRectangle(button_x, button_y, button_width, 30, speed2_color);
    DrawText("2x", button_x + 12, button_y + 7, 16, WHITE);
    
    // 4x button
    button_x += button_width + 5;
    Color speed4_color = (!game_state_.paused && game_state_.speed_multiplier == 4) ? GREEN : DARKGRAY;
    DrawRectangle(button_x, button_y, button_width, 30, speed4_color);
    DrawText("4x", button_x + 12, button_y + 7, 16, WHITE);
}

std::string HUD::FormatTime(float time) {
    int hours = static_cast<int>(time);
    int minutes = static_cast<int>((time - hours) * 60);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" 
       << std::setw(2) << minutes;
    
    std::string period = hours >= 12 ? " PM" : " AM";
    int display_hours = hours % 12;
    if (display_hours == 0) display_hours = 12;
    
    std::stringstream result;
    result << display_hours << ":" << std::setfill('0') << std::setw(2) << minutes << period;
    
    return result.str();
}

std::string HUD::GetSatisfactionEmoji(float satisfaction) {
    if (satisfaction >= 80) return ":)";
    if (satisfaction >= 60) return ":|";
    if (satisfaction >= 40) return ":/";
    return ":(";
}

void HUD::RenderEndGameSummary() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Semi-transparent overlay
    DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.7f));
    
    // Summary box
    int box_width = 400;
    int box_height = 300;
    int box_x = (screen_width - box_width) / 2;
    int box_y = (screen_height - box_height) / 2;
    
    DrawRectangle(box_x, box_y, box_width, box_height, ColorAlpha(BLACK, 0.95f));
    DrawRectangle(box_x, box_y, box_width, 3, GOLD);
    DrawRectangle(box_x, box_y + box_height - 3, box_width, 3, GOLD);
    
    int x = box_x + 20;
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

} // namespace ui
} // namespace towerforge
