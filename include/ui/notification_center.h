#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

namespace towerforge::ui {

    /**
 * @brief Types of notifications
 */
    enum class NotificationType {
        Info,        // General information
        Warning,     // Warnings that need attention
        Error,       // Errors or critical issues
        Success,     // Successful actions or achievements
        Achievement, // Achievement unlocked
        Event        // Game events
    };

    /**
 * @brief Priority levels for notifications
 */
    enum class NotificationPriority {
        Low,
        Medium,
        High,
        Critical
    };

    /**
 * @brief A single notification entry
 */
    struct NotificationEntry {
        std::string id;                  // Unique identifier
        std::string title;               // Notification title
        std::string message;             // Detailed message
        NotificationType type;           // Type of notification
        NotificationPriority priority;   // Priority level
        bool pinned;                     // Whether notification is pinned
        bool read;                       // Whether notification has been read
        std::chrono::system_clock::time_point timestamp; // When created
        std::function<void()> on_click;  // Callback when notification is clicked
        float time_remaining;            // Time until auto-dismiss (-1 for no auto-dismiss)
    
        NotificationEntry(const std::string& id_val,
                          const std::string& title_val,
                          const std::string& message_val,
                          const NotificationType type_val,
                          const NotificationPriority priority_val = NotificationPriority::Medium,
                          const float duration = 5.0f,
                          std::function<void()> callback = nullptr)
            : id(id_val),
              title(title_val),
              message(message_val),
              type(type_val),
              priority(priority_val),
              pinned(false),
              read(false),
              timestamp(std::chrono::system_clock::now()),
              on_click(std::move(callback)),
              time_remaining(duration) {}
    
        /**
     * @brief Get formatted timestamp string
     */
        std::string GetTimestampString() const;
    
        /**
     * @brief Mark as read
     */
        void MarkAsRead() { read = true; }
    
        /**
     * @brief Toggle pinned state
     */
        void TogglePin() { pinned = !pinned; }
    };

    /**
 * @brief Filter options for displaying notifications
 */
    struct NotificationFilter {
        bool show_info = true;
        bool show_warning = true;
        bool show_error = true;
        bool show_success = true;
        bool show_achievement = true;
        bool show_event = true;
        bool show_read = true;
        bool show_unread = true;
        bool show_pinned = true;
        bool show_unpinned = true;
    
        bool ShouldShow(const NotificationEntry& entry) const {
            // Check type filter
            bool type_match = false;
            switch (entry.type) {
                case NotificationType::Info:        type_match = show_info; break;
                case NotificationType::Warning:     type_match = show_warning; break;
                case NotificationType::Error:       type_match = show_error; break;
                case NotificationType::Success:     type_match = show_success; break;
                case NotificationType::Achievement: type_match = show_achievement; break;
                case NotificationType::Event:       type_match = show_event; break;
            }
            if (!type_match) return false;
        
            // Check read/unread filter
            if (entry.read && !show_read) return false;
            if (!entry.read && !show_unread) return false;
        
            // Check pinned filter
            if (entry.pinned && !show_pinned) return false;
            if (!entry.pinned && !show_unpinned) return false;
        
            return true;
        }
    };

    /**
 * @brief Notification Center for managing and displaying notifications
 */
    class NotificationCenter {
    public:
        NotificationCenter();
        ~NotificationCenter();
    
        /**
     * @brief Update notification center state
     * @param delta_time Time elapsed since last frame
     */
        void Update(float delta_time);
    
        /**
     * @brief Render the notification center
     */
        void Render();
    
        /**
     * @brief Render notification toasts (small popups)
     */
        void RenderToasts();
    
        /**
     * @brief Add a new notification
     * @param title Notification title
     * @param message Notification message
     * @param type Type of notification
     * @param priority Priority level
     * @param duration Time until auto-dismiss (use -1 for no auto-dismiss)
     * @param on_click Optional callback when notification is clicked
     * @return ID of the created notification
     */
        std::string AddNotification(const std::string& title,
                                     const std::string& message,
                                     NotificationType type,
                                     NotificationPriority priority = NotificationPriority::Medium,
                                     float duration = 5.0f,
                                     std::function<void()> on_click = nullptr);
    
        /**
     * @brief Remove a notification by ID
     * @param id Notification ID
     * @return true if notification was found and removed
     */
        bool RemoveNotification(const std::string& id);
    
        /**
     * @brief Clear all notifications
     */
        void ClearAll();
    
        /**
     * @brief Clear all read notifications
     */
        void ClearRead();
    
        /**
     * @brief Pin a notification
     * @param id Notification ID
     * @return true if notification was found
     */
        bool PinNotification(const std::string& id);
    
        /**
     * @brief Unpin a notification
     * @param id Notification ID
     * @return true if notification was found
     */
        bool UnpinNotification(const std::string& id);
    
        /**
     * @brief Mark a notification as read
     * @param id Notification ID
     * @return true if notification was found
     */
        bool MarkAsRead(const std::string& id);
    
        /**
     * @brief Toggle notification center visibility
     */
        void ToggleVisibility() { is_visible_ = !is_visible_; }
    
        /**
     * @brief Show notification center
     */
        void Show() { is_visible_ = true; }
    
        /**
     * @brief Hide notification center
     */
        void Hide() { is_visible_ = false; }
    
        /**
     * @brief Check if notification center is visible
     */
        bool IsVisible() const { return is_visible_; }
    
        /**
     * @brief Get current filter settings
     */
        NotificationFilter& GetFilter() { return filter_; }
    
        /**
     * @brief Set filter settings
     */
        void SetFilter(const NotificationFilter& filter) { filter_ = filter; }
    
        /**
     * @brief Get count of unread notifications
     */
        int GetUnreadCount() const;
    
        /**
     * @brief Get count of all notifications
     */
        int GetTotalCount() const { return static_cast<int>(notifications_.size()); }
    
        /**
     * @brief Handle mouse click
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @return true if click was handled
     */
        bool HandleClick(int mouse_x, int mouse_y);
    
        /**
     * @brief Get notification center bounds
     */
        Rectangle GetBounds() const;

    private:
        void RenderNotificationPanel();
        void RenderNotificationEntry(const NotificationEntry& entry, Rectangle bounds, bool hovered);
        void RenderFilterControls();
        Color GetTypeColor(NotificationType type) const;
        const char* GetTypeIcon(NotificationType type) const;
        std::string GenerateUniqueId();
    
        std::vector<NotificationEntry> notifications_;
        NotificationFilter filter_;
        bool is_visible_;
        int scroll_offset_;
        int hovered_index_;
    
        // UI layout constants
        static constexpr int PANEL_WIDTH = 400;
        static constexpr int PANEL_HEIGHT = 600;
        static constexpr int PANEL_PADDING = 10;
        static constexpr int ENTRY_HEIGHT = 80;
        static constexpr int ENTRY_SPACING = 5;
        static constexpr int TOAST_WIDTH = 350;
        static constexpr int TOAST_HEIGHT = 70;
        static constexpr int TOAST_SPACING = 10;
        static constexpr int MAX_TOASTS = 3;
        static constexpr int FILTER_HEIGHT = 40;
    
        // Unique ID counter
        int next_id_;
    };

}
