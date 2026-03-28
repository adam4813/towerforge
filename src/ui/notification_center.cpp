#include "ui/notification_center.h"
#include "ui/ui_theme.h"
#include "core/user_preferences.hpp"
#include "audio/audio_manager.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

import engine;

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
          notification_list_container_(nullptr),
          filter_container_(nullptr),
          last_screen_width_(0),
          last_screen_height_(0),
          next_id_(1) {
        // Load notification filter from UserPreferences
        auto &prefs = towerforge::core::UserPreferences::GetInstance();
        filter_ = prefs.GetNotificationFilter();
    }

    NotificationCenter::~NotificationCenter() = default;

    void NotificationCenter::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Create main panel positioned in upper-right
        const int panel_x = screen_width - PANEL_WIDTH - 20;
        constexpr int panel_y = 60; // Below top bar

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(PANEL_WIDTH), static_cast<float>(PANEL_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        main_panel_->SetPadding(static_cast<float>(PANEL_PADDING));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Title row with close button
        auto title_row = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(PANEL_WIDTH - PANEL_PADDING * 2, HEADER_HEIGHT)
                .Layout(std::make_unique<HorizontalLayout>(UITheme::MARGIN_SMALL, Alignment::Center))
                .Build();

        auto title_text = std::make_unique<Text>(
            0, 0,
            "Notifications",
            UITheme::FONT_SIZE_MEDIUM,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        title_row->AddChild(std::move(title_text));

        // Spacer
        auto spacer = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(PANEL_WIDTH - PANEL_PADDING * 2 - 150, 1)
                .Build();
        title_row->AddChild(std::move(spacer));

        // Close button
        auto close_btn = std::make_unique<Button>(
            30, 25,
            "X",
            UITheme::FONT_SIZE_MEDIUM
        );
        close_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::ERROR));
        close_btn->SetTextColor(UITheme::ToEngineColor(UITheme::ERROR));
        close_btn->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        close_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::ERROR, 0.3f)));
        close_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                Hide();
                return true;
            }
            return false;
        });
        title_row->AddChild(std::move(close_btn));
        main_panel_->AddChild(std::move(title_row));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(PANEL_WIDTH - PANEL_PADDING * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Filter controls row
        auto filter_row = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(PANEL_WIDTH - PANEL_PADDING * 2, FILTER_HEIGHT)
                .Layout(std::make_unique<HorizontalLayout>(3, Alignment::Center))
                .Build();

        auto filter_label = std::make_unique<Text>(
            0, 0,
            "Filter:",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        filter_row->AddChild(std::move(filter_label));

        // Filter buttons
        struct FilterDef {
            const char *label;
            bool *flag;
            Color color;
        };

        FilterDef filter_defs[] = {
            {"I", &filter_.show_info, UITheme::INFO},
            {"W", &filter_.show_warning, UITheme::WARNING},
            {"E", &filter_.show_error, UITheme::ERROR},
            {"S", &filter_.show_success, UITheme::SUCCESS},
            {"A", &filter_.show_achievement, UITheme::PRIMARY},
            {"V", &filter_.show_event, PURPLE}
        };

        filter_buttons_.clear();
        for (auto &def: filter_defs) {
            auto btn = std::make_unique<Button>(
                30, 30,
                def.label,
                UITheme::FONT_SIZE_SMALL
            );
            const Color btn_color = *def.flag ? def.color : ColorAlpha(DARKGRAY, 0.5f);
            btn->SetNormalColor(UITheme::ToEngineColor(btn_color));
            btn->SetBorderColor(UITheme::ToEngineColor(WHITE));
            btn->SetTextColor(UITheme::ToEngineColor(WHITE));
            btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(def.color, 0.7f)));

            bool *flag_ptr = def.flag;
            Color active_color = def.color;
            btn->SetClickCallback(
                [this, flag_ptr, active_color, btn_raw = btn.get()](const engine::ui::MouseEvent &event) {
                    if (event.left_pressed) {
                        *flag_ptr = !*flag_ptr;
                        const Color new_color = *flag_ptr ? active_color : ColorAlpha(DARKGRAY, 0.5f);
                        btn_raw->SetNormalColor(UITheme::ToEngineColor(new_color));
                        SetFilter(filter_);
                        RebuildNotificationList();
                        return true;
                    }
                    return false;
                });

            filter_buttons_.push_back(btn.get());
            filter_row->AddChild(std::move(btn));
        }

        filter_container_ = filter_row.get();
        main_panel_->AddChild(std::move(filter_row));

        // Scrollable notification list
        constexpr float list_width = PANEL_WIDTH - PANEL_PADDING * 2;
        constexpr float list_height = PANEL_HEIGHT - HEADER_HEIGHT - FILTER_HEIGHT - 80;

        auto list_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(list_width, list_height)
                .Layout(std::make_unique<VerticalLayout>(ENTRY_SPACING, Alignment::Center))
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Build();

        notification_list_container_ = list_container.get();
        main_panel_->AddChild(std::move(list_container));

        // Bottom buttons row
        auto button_row = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(PANEL_WIDTH - PANEL_PADDING * 2, 35)
                .Layout(std::make_unique<HorizontalLayout>(5, Alignment::Center))
                .Build();

        // Clear Read button
        auto clear_read_btn = std::make_unique<Button>(
            100, 25,
            "Clear Read",
            UITheme::FONT_SIZE_SMALL
        );
        clear_read_btn->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        clear_read_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        clear_read_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        clear_read_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        clear_read_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                ClearRead();
                RebuildNotificationList();
                return true;
            }
            return false;
        });
        button_row->AddChild(std::move(clear_read_btn));

        // Clear All button
        auto clear_all_btn = std::make_unique<Button>(
            100, 25,
            "Clear All",
            UITheme::FONT_SIZE_SMALL
        );
        clear_all_btn->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        clear_all_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        clear_all_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        clear_all_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        clear_all_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                ClearAll();
                RebuildNotificationList();
                return true;
            }
            return false;
        });
        button_row->AddChild(std::move(clear_all_btn));

        // Mark All Read button
        auto mark_read_btn = std::make_unique<Button>(
            110, 25,
            "Mark All Read",
            UITheme::FONT_SIZE_SMALL
        );
        mark_read_btn->SetNormalColor(UITheme::ToEngineColor(DARKGRAY));
        mark_read_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        mark_read_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        mark_read_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        mark_read_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                for (auto &notification: notifications_) {
                    notification.MarkAsRead();
                }
                RebuildNotificationList();
                return true;
            }
            return false;
        });
        button_row->AddChild(std::move(mark_read_btn));

        main_panel_->AddChild(std::move(button_row));

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;

        UpdateLayout();
        RebuildNotificationList();
    }

    void NotificationCenter::Shutdown() {
        filter_buttons_.clear();
        notification_list_container_ = nullptr;
        filter_container_ = nullptr;
        main_panel_.reset();
    }

    void NotificationCenter::SetFilter(const NotificationFilter &filter) {
        filter_ = filter;
        // Save filter to UserPreferences
        towerforge::core::UserPreferences::GetInstance().SetNotificationFilter(filter);
    }

    void NotificationCenter::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (main_panel_) {
            const int panel_x = screen_width - PANEL_WIDTH - 20;
            constexpr int panel_y = 60;
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void NotificationCenter::Update(const float delta_time) {
        // Update notifications - remove expired ones that aren't pinned
        bool needs_rebuild = false;
        for (auto it = notifications_.begin(); it != notifications_.end();) {
            // Animate fade-in
            if (it->animation_progress < 1.0f) {
                it->animation_progress += delta_time * UITheme::ANIMATION_SPEED_FAST;
                if (it->animation_progress > 1.0f) {
                    it->animation_progress = 1.0f;
                }
            }

            // Update time remaining
            if (it->time_remaining > 0.0f) {
                it->time_remaining -= delta_time;
                if (it->time_remaining <= 0.0f && !it->pinned) {
                    it = notifications_.erase(it);
                    needs_rebuild = true;
                    continue;
                }
            }
            ++it;
        }

        // Check for window resize
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        if (main_panel_ && is_visible_) {
            main_panel_->UpdateComponentsRecursive(delta_time);
        }

        if (needs_rebuild && notification_list_container_) {
            RebuildNotificationList();
        }
    }

    void NotificationCenter::Render() const {
        if (!is_visible_ || !main_panel_) {
            return;
        }
        main_panel_->Render();
    }

    void NotificationCenter::RenderToasts() const {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        const int x = screen_width - TOAST_WIDTH - 10;
        int y = UITheme::PADDING_SMALL + 50;

        int toast_count = 0;
        for (auto it = notifications_.rbegin(); it != notifications_.rend() && toast_count < MAX_TOASTS; ++it) {
            if (!it->read && it->time_remaining > 0.0f) {
                const float alpha = it->animation_progress;
                const auto bg_color = engine::ui::Color::Alpha(GetTypeColor(it->type), 0.9f * alpha);
                const Color border_color = ColorAlpha(UITheme::TEXT_PRIMARY, 0.5f * alpha);

                engine::ui::BatchRenderer::SubmitQuad(
                    engine::ui::Rectangle(static_cast<float>(x), static_cast<float>(y),
                                          static_cast<float>(TOAST_WIDTH), static_cast<float>(TOAST_HEIGHT)),
                    bg_color
                );

                const auto border_col = UITheme::ToEngineColor(border_color);
                engine::ui::BatchRenderer::SubmitLine(x, y, x + TOAST_WIDTH, y, UITheme::BORDER_THIN, border_col);
                engine::ui::BatchRenderer::SubmitLine(x + TOAST_WIDTH, y, x + TOAST_WIDTH, y + TOAST_HEIGHT,
                                                      UITheme::BORDER_THIN, border_col);
                engine::ui::BatchRenderer::SubmitLine(x + TOAST_WIDTH, y + TOAST_HEIGHT, x, y + TOAST_HEIGHT,
                                                      UITheme::BORDER_THIN, border_col);
                engine::ui::BatchRenderer::SubmitLine(x, y + TOAST_HEIGHT, x, y, UITheme::BORDER_THIN, border_col);

                const char *icon = GetTypeIcon(it->type);
                engine::ui::BatchRenderer::SubmitText(icon, static_cast<float>(x + 5), static_cast<float>(y + 5),
                                                      14, UITheme::ToEngineColor(
                                                          ColorAlpha(UITheme::TEXT_PRIMARY, alpha)));

                engine::ui::BatchRenderer::SubmitText(it->title, static_cast<float>(x + 25), static_cast<float>(y + 5),
                                                      12, UITheme::ToEngineColor(
                                                          ColorAlpha(UITheme::TEXT_PRIMARY, alpha)));

                std::string display_msg = it->message;
                if (display_msg.length() > 25) {
                    display_msg = display_msg.substr(0, 22) + "...";
                }
                engine::ui::BatchRenderer::SubmitText(display_msg, static_cast<float>(x + 25),
                                                      static_cast<float>(y + 18),
                                                      10, UITheme::ToEngineColor(
                                                          ColorAlpha(UITheme::TEXT_SECONDARY, 0.9f * alpha)));

                if (it->time_remaining > 0.0f) {
                    const int bar_width = static_cast<int>(
                        (it->time_remaining / UITheme::NOTIFICATION_DURATION_NORMAL) * TOAST_WIDTH);
                    engine::ui::BatchRenderer::SubmitQuad(
                        engine::ui::Rectangle(static_cast<float>(x), static_cast<float>(y + TOAST_HEIGHT - 2),
                                              static_cast<float>(bar_width), 2.0f),
                        UITheme::ToEngineColor(ColorAlpha(UITheme::TEXT_PRIMARY, 0.7f * alpha))
                    );
                }

                y += TOAST_HEIGHT + TOAST_SPACING;
                toast_count++;
            }
        }
    }

    std::string NotificationCenter::AddNotification(const std::string &title,
                                                    const std::string &message,
                                                    const NotificationType type,
                                                    const NotificationPriority priority,
                                                    const float duration,
                                                    std::function<void()> on_click) {
        const std::string id = GenerateUniqueId();
        notifications_.emplace_back(id, title, message, type, priority, duration, std::move(on_click));

        if (notification_list_container_) {
            RebuildNotificationList();
        }
        return id;
    }

    bool NotificationCenter::RemoveNotification(const std::string &id) {
        const auto it = std::ranges::find_if(notifications_,
                                             [&id](const NotificationEntry &entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            notifications_.erase(it);
            if (notification_list_container_) {
                RebuildNotificationList();
            }
            return true;
        }
        return false;
    }

    void NotificationCenter::ClearAll() {
        // Keep only pinned notifications
        std::erase_if(notifications_,
                      [](const NotificationEntry &entry) { return !entry.pinned; });
    }

    void NotificationCenter::ClearRead() {
        // Remove read notifications that aren't pinned
        std::erase_if(notifications_,
                      [](const NotificationEntry &entry) { return entry.read && !entry.pinned; });
    }

    bool NotificationCenter::PinNotification(const std::string &id) {
        const auto it = std::ranges::find_if(notifications_,
                                             [&id](const NotificationEntry &entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->pinned = true;
            it->time_remaining = -1.0f;
            if (notification_list_container_) {
                RebuildNotificationList();
            }
            return true;
        }
        return false;
    }

    bool NotificationCenter::UnpinNotification(const std::string &id) {
        const auto it = std::ranges::find_if(notifications_,
                                             [&id](const NotificationEntry &entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->pinned = false;
            if (notification_list_container_) {
                RebuildNotificationList();
            }
            return true;
        }
        return false;
    }

    bool NotificationCenter::MarkAsRead(const std::string &id) {
        const auto it = std::ranges::find_if(notifications_,
                                             [&id](const NotificationEntry &entry) { return entry.id == id; });
        if (it != notifications_.end()) {
            it->MarkAsRead();
            return true;
        }
        return false;
    }

    int NotificationCenter::GetUnreadCount() const {
        return static_cast<int>(std::ranges::count_if(notifications_,
                                                      [](const NotificationEntry &entry) { return !entry.read; }));
    }

    bool NotificationCenter::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!is_visible_ || !main_panel_) {
            return false;
        }
        return main_panel_->ProcessMouseEvent(event);
    }

    void NotificationCenter::RebuildNotificationList() {
        if (!notification_list_container_) {
            return;
        }

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        notification_list_container_->ClearChildren();

        constexpr float item_width = PANEL_WIDTH - PANEL_PADDING * 2 - 20;

        if (notifications_.empty()) {
            auto empty_text = std::make_unique<Text>(
                0, 0,
                "No notifications",
                UITheme::FONT_SIZE_MEDIUM,
                UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
            );
            notification_list_container_->AddChild(std::move(empty_text));
        } else {
            for (auto &notification: notifications_) {
                if (!filter_.ShouldShow(notification)) {
                    continue;
                }
                notification_list_container_->AddChild(CreateNotificationItem(notification, item_width));
            }
        }

        notification_list_container_->InvalidateComponents();
        notification_list_container_->UpdateComponentsRecursive();

        if (auto *scroll = notification_list_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    std::unique_ptr<engine::ui::elements::Container> NotificationCenter::CreateNotificationItem(
        NotificationEntry &entry, const float item_width) {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const auto type_color = GetTypeColor(entry.type);
        const Color bg_color = entry.read ? ColorAlpha(DARKGRAY, 0.3f) : ColorAlpha(DARKBLUE, 0.4f);

        auto item_container = engine::ui::ContainerBuilder()
                .Size(item_width, ENTRY_HEIGHT)
                .Background(UITheme::ToEngineColor(bg_color))
                .Border(1.0f, engine::ui::Color::Alpha(type_color, 0.5f))
                .Padding(5)
                .Layout(std::make_unique<VerticalLayout>(2, Alignment::Start))
                .Build();

        // Title row with icon
        auto title_row = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(item_width - 10, 20)
                .Layout(std::make_unique<HorizontalLayout>(5, Alignment::Start))
                .Build();

        // Type icon
        auto icon_text = std::make_unique<Text>(
            0, 0,
            GetTypeIcon(entry.type),
            UITheme::FONT_SIZE_MEDIUM,
            type_color
        );
        title_row->AddChild(std::move(icon_text));

        // Title
        auto title_text = std::make_unique<Text>(
            0, 0,
            entry.title,
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(WHITE)
        );
        title_row->AddChild(std::move(title_text));

        // Pin indicator
        if (entry.pinned) {
            auto pin_text = std::make_unique<Text>(
                0, 0,
                "P",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(YELLOW)
            );
            title_row->AddChild(std::move(pin_text));
        }

        item_container->AddChild(std::move(title_row));

        // Message (truncated)
        std::string display_msg = entry.message;
        if (display_msg.length() > 50) {
            display_msg = display_msg.substr(0, 47) + "...";
        }
        auto message_text = std::make_unique<Text>(
            0, 0,
            display_msg,
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(LIGHTGRAY)
        );
        item_container->AddChild(std::move(message_text));

        // Timestamp
        auto timestamp_text = std::make_unique<Text>(
            0, 0,
            entry.GetTimestampString(),
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(GRAY)
        );
        item_container->AddChild(std::move(timestamp_text));

        // Action buttons row
        auto action_row = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(item_width - 10, 20)
                .Layout(std::make_unique<HorizontalLayout>(5, Alignment::End))
                .Build();

        // Pin/Unpin button
        const char *pin_label = entry.pinned ? "Unpin" : "Pin";
        auto pin_btn = std::make_unique<Button>(
            40, 18,
            pin_label,
            UITheme::FONT_SIZE_SMALL
        );
        pin_btn->SetNormalColor(UITheme::ToEngineColor(entry.pinned ? YELLOW : DARKGRAY));
        pin_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        pin_btn->SetTextColor(UITheme::ToEngineColor(entry.pinned ? BLACK : WHITE));
        pin_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(YELLOW, 0.5f)));

        std::string entry_id = entry.id;
        pin_btn->SetClickCallback([this, entry_id](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                const auto it = std::ranges::find_if(notifications_,
                                                     [&entry_id](const NotificationEntry &e) {
                                                         return e.id == entry_id;
                                                     });
                if (it != notifications_.end()) {
                    it->TogglePin();
                    if (it->pinned) {
                        it->time_remaining = -1.0f;
                    }
                    RebuildNotificationList();
                }
                return true;
            }
            return false;
        });
        action_row->AddChild(std::move(pin_btn));

        // Dismiss button
        auto dismiss_btn = std::make_unique<Button>(
            20, 18,
            "X",
            UITheme::FONT_SIZE_SMALL
        );
        dismiss_btn->SetNormalColor(UITheme::ToEngineColor(MAROON));
        dismiss_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        dismiss_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        dismiss_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.7f)));

        dismiss_btn->SetClickCallback([this, entry_id](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                RemoveNotification(entry_id);
                return true;
            }
            return false;
        });
        action_row->AddChild(std::move(dismiss_btn));

        item_container->AddChild(std::move(action_row));

        // Set click callback on the whole container for marking as read
        std::function<void()> on_click_callback = entry.on_click;
        item_container->SetClickCallback([this, entry_id, on_click_callback](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                MarkAsRead(entry_id);
                if (on_click_callback) {
                    on_click_callback();
                }
                RebuildNotificationList();
                return true;
            }
            return false;
        });

        return item_container;
    }

    engine::ui::Color NotificationCenter::GetTypeColor(const NotificationType type) {
        switch (type) {
            case NotificationType::Info: return UITheme::ToEngineColor(UITheme::INFO);
            case NotificationType::Warning: return UITheme::ToEngineColor(UITheme::WARNING);
            case NotificationType::Error: return UITheme::ToEngineColor(UITheme::ERROR);
            case NotificationType::Success: return UITheme::ToEngineColor(UITheme::SUCCESS);
            case NotificationType::Achievement: return UITheme::ToEngineColor(UITheme::PRIMARY);
            case NotificationType::Event: return UITheme::ToEngineColor(PURPLE);
            default: return UITheme::ToEngineColor(UITheme::TEXT_PRIMARY);
        }
    }

    const char *NotificationCenter::GetTypeIcon(const NotificationType type) {
        switch (type) {
            case NotificationType::Info: return "i";
            case NotificationType::Warning: return "!";
            case NotificationType::Error: return "X";
            case NotificationType::Success: return "+";
            case NotificationType::Achievement: return "*";
            case NotificationType::Event: return "@";
            default: return "?";
        }
    }

    std::string NotificationCenter::GenerateUniqueId() {
        return "notif_" + std::to_string(next_id_++);
    }
}
