#include "ui/notification_center.h"
#include "core/user_preferences.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace towerforge::ui {

    std::string NotificationEntry::GetTimestampString() const {
        auto time_t_val = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_val;
    
#ifdef _WIN32
        localtime_s(&tm_val, &time_t_val);
#else
        localtime_r(&time_t_val, &tm_val);
#endif
    
        std::ostringstream oss;
        oss << std::put_time(&tm_val, "%H:%M:%S");
        return oss.str();
    }

    NotificationCenter::NotificationCenter()
        : is_visible_(false),
          scroll_offset_(0),
          hovered_index_(-1),
          next_id_(1) {
        // Load notification filter from UserPreferences
        auto& prefs = TowerForge::Core::UserPreferences::GetInstance();
        filter_ = prefs.GetNotificationFilter();
    }

    NotificationCenter::~NotificationCenter() = default;

    void NotificationCenter::SetFilter(const NotificationFilter& filter) {
        filter_ = filter;
        // Save filter to UserPreferences
        TowerForge::Core::UserPreferences::GetInstance().SetNotificationFilter(filter);
    }

    void NotificationCenter::Update(const float delta_time) {
        // Update notifications - remove expired ones that aren't pinned
        for (auto it = notifications_.begin(); it != notifications_.end();) {
            if (it->time_remaining > 0.0f) {
                it->time_remaining -= delta_time;
                if (it->time_remaining <= 0.0f && !it->pinned) {
                    it = notifications_.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void NotificationCenter::Render() {
        if (is_visible_) {
            RenderNotificationPanel();
        }
    }

    void NotificationCenter::RenderToasts() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        constexpr int x = 10;
        int y = screen_height - 10;
    
        // Show up to MAX_TOASTS recent unread notifications
        int toast_count = 0;
        for (auto it = notifications_.rbegin(); it != notifications_.rend() && toast_count < MAX_TOASTS; ++it) {
            if (!it->read && it->time_remaining > 0.0f) {
                y -= TOAST_HEIGHT + TOAST_SPACING;
            
                const Color bg_color = ColorAlpha(GetTypeColor(it->type), 0.9f);
                DrawRectangle(x, y, TOAST_WIDTH, TOAST_HEIGHT, bg_color);
                DrawRectangleLines(x, y, TOAST_WIDTH, TOAST_HEIGHT, ColorAlpha(WHITE, 0.5f));
            
                // Icon
                const char* icon = GetTypeIcon(it->type);
                DrawText(icon, x + 10, y + 10, 24, WHITE);
            
                // Title
                DrawText(it->title.c_str(), x + 45, y + 10, 16, WHITE);
            
                // Message (truncated if too long)
                std::string display_msg = it->message;
                if (display_msg.length() > 50) {
                    display_msg = display_msg.substr(0, 47) + "...";
                }
                DrawText(display_msg.c_str(), x + 45, y + 32, 12, ColorAlpha(WHITE, 0.9f));
            
                // Time remaining indicator
                if (it->time_remaining > 0.0f) {
                    const int bar_width = static_cast<int>((it->time_remaining / 5.0f) * TOAST_WIDTH);
                    DrawRectangle(x, y + TOAST_HEIGHT - 3, bar_width, 3, ColorAlpha(WHITE, 0.7f));
                }
            
                toast_count++;
            }
        }
    }

    std::string NotificationCenter::AddNotification(const std::string& title,
                                                     const std::string& message,
                                                     const NotificationType type,
                                                     const NotificationPriority priority,
                                                     const float duration,
                                                     std::function<void()> on_click) {
        const std::string id = GenerateUniqueId();
        notifications_.emplace_back(id, title, message, type, priority, duration, std::move(on_click));
        return id;
    }

    bool NotificationCenter::RemoveNotification(const std::string& id) {
        auto it = std::find_if(notifications_.begin(), notifications_.end(),
                               [&id](const NotificationEntry& entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            notifications_.erase(it);
            return true;
        }
        return false;
    }

    void NotificationCenter::ClearAll() {
        // Keep only pinned notifications
        notifications_.erase(
            std::remove_if(notifications_.begin(), notifications_.end(),
                           [](const NotificationEntry& entry) { return !entry.pinned; }),
            notifications_.end());
    }

    void NotificationCenter::ClearRead() {
        // Remove read notifications that aren't pinned
        notifications_.erase(
            std::remove_if(notifications_.begin(), notifications_.end(),
                           [](const NotificationEntry& entry) { return entry.read && !entry.pinned; }),
            notifications_.end());
    }

    bool NotificationCenter::PinNotification(const std::string& id) {
        auto it = std::find_if(notifications_.begin(), notifications_.end(),
                               [&id](const NotificationEntry& entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->pinned = true;
            it->time_remaining = -1.0f;  // Pinned notifications don't auto-dismiss
            return true;
        }
        return false;
    }

    bool NotificationCenter::UnpinNotification(const std::string& id) {
        auto it = std::find_if(notifications_.begin(), notifications_.end(),
                               [&id](const NotificationEntry& entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->pinned = false;
            return true;
        }
        return false;
    }

    bool NotificationCenter::MarkAsRead(const std::string& id) {
        auto it = std::find_if(notifications_.begin(), notifications_.end(),
                               [&id](const NotificationEntry& entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->MarkAsRead();
            return true;
        }
        return false;
    }

    int NotificationCenter::GetUnreadCount() const {
        return static_cast<int>(std::count_if(notifications_.begin(), notifications_.end(),
                                              [](const NotificationEntry& entry) { return !entry.read; }));
    }

    bool NotificationCenter::HandleClick(const int mouse_x, const int mouse_y) {
        if (!is_visible_) {
            return false;
        }
    
        const Rectangle bounds = GetBounds();
        if (!CheckCollisionPointRec(Vector2{static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, bounds)) {
            return false;
        }
    
        // Check close button
        const int close_x = static_cast<int>(bounds.x + bounds.width - 30);
        const int close_y = static_cast<int>(bounds.y + 5);
        if (mouse_x >= close_x && mouse_x <= close_x + 20 && mouse_y >= close_y && mouse_y <= close_y + 20) {
            Hide();
            return true;
        }
    
        // Check filter controls area
        if (mouse_y >= bounds.y + 30 && mouse_y <= bounds.y + 30 + FILTER_HEIGHT) {
            const int filter_y = static_cast<int>(bounds.y) + 30;
            const int filter_x = static_cast<int>(bounds.x) + PANEL_PADDING;
            const int button_size = 30;
            int x = filter_x + 50;
        
            struct FilterButton {
                bool* flag;
            };
        
            FilterButton buttons[] = {
                {&filter_.show_info},
                {&filter_.show_warning},
                {&filter_.show_error},
                {&filter_.show_success},
                {&filter_.show_achievement},
                {&filter_.show_event}
            };
        
            for (auto& button : buttons) {
                if (mouse_x >= x && mouse_x <= x + button_size && mouse_y >= filter_y && mouse_y <= filter_y + button_size) {
                    *button.flag = !*button.flag;
                    // Save the updated filter
                    SetFilter(filter_);
                    return true;
                }
                x += button_size + 3;
            }
        
            return true;
        }
    
        // Check notification entries
        int y_offset = static_cast<int>(bounds.y) + 30 + FILTER_HEIGHT + PANEL_PADDING;
        int current_index = 0;
    
        for (auto& notification : notifications_) {
            if (!filter_.ShouldShow(notification)) {
                continue;
            }
        
            if (current_index < scroll_offset_) {
                current_index++;
                continue;
            }
        
            const int entry_y = y_offset + (current_index - scroll_offset_) * (ENTRY_HEIGHT + ENTRY_SPACING);
            if (entry_y >= bounds.y + bounds.height - PANEL_PADDING) {
                break;
            }
        
            if (mouse_y >= entry_y && mouse_y <= entry_y + ENTRY_HEIGHT) {
                // Check action buttons
                const int actions_x = static_cast<int>(bounds.x + bounds.width - 80);
            
                // Pin button
                if (mouse_x >= actions_x && mouse_x <= actions_x + 30 && mouse_y >= entry_y + 5 && mouse_y <= entry_y + 30) {
                    notification.TogglePin();
                    return true;
                }
            
                // Dismiss button
                const int dismiss_x = actions_x + 35;
                if (mouse_x >= dismiss_x && mouse_x <= dismiss_x + 30 && mouse_y >= entry_y + 5 && mouse_y <= entry_y + 30) {
                    RemoveNotification(notification.id);
                    return true;
                }
            
                // Click on notification body
                if (mouse_x < actions_x) {
                    notification.MarkAsRead();
                    if (notification.on_click) {
                        notification.on_click();
                    }
                    return true;
                }
            }
        
            current_index++;
        }
    
        // Check bottom buttons
        const int button_y = static_cast<int>(bounds.y + bounds.height - 35);
        const int button1_x = static_cast<int>(bounds.x) + PANEL_PADDING;
        const int button2_x = button1_x + 120;
        const int button3_x = button2_x + 120;
    
        if (mouse_y >= button_y && mouse_y <= button_y + 25) {
            if (mouse_x >= button1_x && mouse_x <= button1_x + 110) {
                ClearRead();
                return true;
            }
            if (mouse_x >= button2_x && mouse_x <= button2_x + 110) {
                ClearAll();
                return true;
            }
            if (mouse_x >= button3_x && mouse_x <= button3_x + 110) {
                for (auto& notification : notifications_) {
                    notification.MarkAsRead();
                }
                return true;
            }
        }
    
        return true;  // Consumed click
    }

    Rectangle NotificationCenter::GetBounds() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int x = screen_width - PANEL_WIDTH - 20;
        const int y = 60;  // Below top bar
        return Rectangle{static_cast<float>(x), static_cast<float>(y), 
                         static_cast<float>(PANEL_WIDTH), static_cast<float>(PANEL_HEIGHT)};
    }

    void NotificationCenter::RenderNotificationPanel() {
        const Rectangle bounds = GetBounds();
    
        // Background
        DrawRectangle(static_cast<int>(bounds.x), static_cast<int>(bounds.y), 
                      static_cast<int>(bounds.width), static_cast<int>(bounds.height), 
                      ColorAlpha(BLACK, 0.95f));
        DrawRectangleLines(static_cast<int>(bounds.x), static_cast<int>(bounds.y), 
                           static_cast<int>(bounds.width), static_cast<int>(bounds.height), 
                           GOLD);
    
        // Title bar
        DrawText("Notifications", static_cast<int>(bounds.x) + PANEL_PADDING, 
                 static_cast<int>(bounds.y) + 5, 20, GOLD);
    
        // Unread count badge
        const int unread_count = GetUnreadCount();
        if (unread_count > 0) {
            std::string count_str = std::to_string(unread_count);
            const int badge_x = static_cast<int>(bounds.x) + 160;
            const int badge_y = static_cast<int>(bounds.y) + 8;
            DrawCircle(badge_x, badge_y, 12, RED);
            const int text_width = MeasureText(count_str.c_str(), 14);
            DrawText(count_str.c_str(), badge_x - text_width / 2, badge_y - 7, 14, WHITE);
        }
    
        // Close button
        const int close_x = static_cast<int>(bounds.x + bounds.width - 30);
        const int close_y = static_cast<int>(bounds.y + 5);
        DrawText("X", close_x, close_y, 20, RED);
    
        // Render filter controls
        RenderFilterControls();
    
        // Notification list area
        const int list_y = static_cast<int>(bounds.y) + 30 + FILTER_HEIGHT + PANEL_PADDING;
        const int list_height = static_cast<int>(bounds.height) - 30 - FILTER_HEIGHT - PANEL_PADDING * 2;
    
        // Render notifications
        int y_offset = list_y;
        int current_index = 0;
        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();
    
        for (const auto& notification : notifications_) {
            if (!filter_.ShouldShow(notification)) {
                continue;
            }
        
            if (current_index < scroll_offset_) {
                current_index++;
                continue;
            }
        
            const int entry_y = y_offset + (current_index - scroll_offset_) * (ENTRY_HEIGHT + ENTRY_SPACING);
            if (entry_y >= bounds.y + bounds.height - PANEL_PADDING) {
                break;
            }
        
            const Rectangle entry_bounds = {
                bounds.x + PANEL_PADDING,
                static_cast<float>(entry_y),
                bounds.width - PANEL_PADDING * 2,
                static_cast<float>(ENTRY_HEIGHT)
            };
        
            const bool hovered = CheckCollisionPointRec(
                Vector2{static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, 
                entry_bounds);
        
            RenderNotificationEntry(notification, entry_bounds, hovered);
        
            current_index++;
        }
    
        // Scroll indicator if needed
        const int visible_count = list_height / (ENTRY_HEIGHT + ENTRY_SPACING);
        const int filtered_count = static_cast<int>(std::count_if(notifications_.begin(), notifications_.end(),
                                                                   [this](const NotificationEntry& e) { return filter_.ShouldShow(e); }));
        if (filtered_count > visible_count) {
            const int scrollbar_x = static_cast<int>(bounds.x + bounds.width - 10);
            const int scrollbar_height = list_height;
            const int thumb_height = std::max(20, scrollbar_height * visible_count / filtered_count);
            const int thumb_y = list_y + (scrollbar_height - thumb_height) * scroll_offset_ / std::max(1, filtered_count - visible_count);
        
            DrawRectangle(scrollbar_x, list_y, 5, scrollbar_height, ColorAlpha(DARKGRAY, 0.5f));
            DrawRectangle(scrollbar_x, thumb_y, 5, thumb_height, ColorAlpha(LIGHTGRAY, 0.8f));
        }
    
        // Bottom buttons
        const int button_y = static_cast<int>(bounds.y + bounds.height - 35);
        const int button1_x = static_cast<int>(bounds.x) + PANEL_PADDING;
        const int button2_x = button1_x + 120;
        const int button3_x = button2_x + 120;
    
        DrawRectangle(button1_x, button_y, 110, 25, DARKGRAY);
        DrawText("Clear Read", button1_x + 10, button_y + 5, 14, WHITE);
    
        DrawRectangle(button2_x, button_y, 110, 25, DARKGRAY);
        DrawText("Clear All", button2_x + 15, button_y + 5, 14, WHITE);
    
        DrawRectangle(button3_x, button_y, 110, 25, DARKGRAY);
        DrawText("Mark All Read", button3_x + 5, button_y + 5, 14, WHITE);
    }

    void NotificationCenter::RenderNotificationEntry(const NotificationEntry& entry, 
                                                      const Rectangle bounds, 
                                                      const bool hovered) {
        // Background
        Color bg_color = entry.read ? ColorAlpha(DARKGRAY, 0.3f) : ColorAlpha(DARKBLUE, 0.4f);
        if (hovered) {
            bg_color = ColorAlpha(SKYBLUE, 0.3f);
        }
        DrawRectangleRec(bounds, bg_color);
        DrawRectangleLinesEx(bounds, 1, ColorAlpha(GetTypeColor(entry.type), 0.5f));
    
        const int x = static_cast<int>(bounds.x) + 5;
        int y = static_cast<int>(bounds.y) + 5;
    
        // Type icon
        const char* icon = GetTypeIcon(entry.type);
        DrawText(icon, x, y, 20, GetTypeColor(entry.type));
    
        // Title
        DrawText(entry.title.c_str(), x + 30, y, 14, WHITE);
    
        // Pin indicator
        if (entry.pinned) {
            DrawText("P", static_cast<int>(bounds.x + bounds.width - 50), y, 14, YELLOW);
        }
    
        y += 20;
    
        // Message (word-wrapped)
        std::string display_msg = entry.message;
        if (display_msg.length() > 60) {
            display_msg = display_msg.substr(0, 57) + "...";
        }
        DrawText(display_msg.c_str(), x + 30, y, 11, LIGHTGRAY);
    
        y += 18;
    
        // Timestamp
        DrawText(entry.GetTimestampString().c_str(), x + 30, y, 10, GRAY);
    
        // Action buttons (only show on hover)
        if (hovered) {
            const int button_x = static_cast<int>(bounds.x + bounds.width - 80);
            const int button_y = static_cast<int>(bounds.y) + 5;
        
            // Pin/Unpin button
            const char* pin_text = entry.pinned ? "U" : "P";
            DrawRectangle(button_x, button_y, 30, 25, entry.pinned ? YELLOW : DARKGRAY);
            DrawText(pin_text, button_x + 10, button_y + 5, 14, BLACK);
        
            // Dismiss button
            DrawRectangle(button_x + 35, button_y, 30, 25, MAROON);
            DrawText("X", button_x + 45, button_y + 5, 14, WHITE);
        }
    }

    void NotificationCenter::RenderFilterControls() {
        const Rectangle bounds = GetBounds();
        const int filter_y = static_cast<int>(bounds.y) + 30;
        const int filter_x = static_cast<int>(bounds.x) + PANEL_PADDING;
    
        DrawText("Filter:", filter_x, filter_y + 5, 12, LIGHTGRAY);
    
        // Type filter buttons
        const int button_size = 30;
        int x = filter_x + 50;
    
        struct FilterButton {
            const char* label;
            bool* flag;
            Color color;
        };
    
        FilterButton buttons[] = {
            {"I", &filter_.show_info, SKYBLUE},
            {"W", &filter_.show_warning, ORANGE},
            {"E", &filter_.show_error, RED},
            {"S", &filter_.show_success, GREEN},
            {"A", &filter_.show_achievement, GOLD},
            {"V", &filter_.show_event, PURPLE}
        };
    
        for (const auto& button : buttons) {
            const Color btn_color = *button.flag ? button.color : ColorAlpha(DARKGRAY, 0.5f);
            DrawRectangle(x, filter_y, button_size, button_size, btn_color);
            DrawRectangleLines(x, filter_y, button_size, button_size, WHITE);
            DrawText(button.label, x + 10, filter_y + 8, 14, WHITE);
            x += button_size + 3;
        }
    }

    Color NotificationCenter::GetTypeColor(const NotificationType type) const {
        switch (type) {
            case NotificationType::Info:        return SKYBLUE;
            case NotificationType::Warning:     return ORANGE;
            case NotificationType::Error:       return RED;
            case NotificationType::Success:     return GREEN;
            case NotificationType::Achievement: return GOLD;
            case NotificationType::Event:       return PURPLE;
            default:                            return WHITE;
        }
    }

    const char* NotificationCenter::GetTypeIcon(const NotificationType type) const {
        switch (type) {
            case NotificationType::Info:        return "i";
            case NotificationType::Warning:     return "!";
            case NotificationType::Error:       return "X";
            case NotificationType::Success:     return "+";
            case NotificationType::Achievement: return "*";
            case NotificationType::Event:       return "@";
            default:                            return "?";
        }
    }

    std::string NotificationCenter::GenerateUniqueId() {
        return "notif_" + std::to_string(next_id_++);
    }

}
