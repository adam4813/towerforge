#include "ui/ui_window_manager.h"
#include "ui/mouse_interface.h"

import engine;

namespace towerforge::ui {
    UIWindowManager::UIWindowManager()
        : next_z_order_(0)
          , is_info_window_(false)
          , last_screen_width_(0)
          , last_screen_height_(0) {
    }

    void UIWindowManager::Update(const float delta_time) {
        std::uint32_t current_width;
        std::uint32_t current_height;

        engine::rendering::GetRenderer().GetFramebufferSize(current_width, current_height);

        // Check if screen was resized
        if (current_width != last_screen_width_ || current_height != last_screen_height_) {
            last_screen_width_ = current_width;
            last_screen_height_ = current_height;

            // Reposition info windows
            if (is_info_window_ && !windows_.empty()) {
                for (const auto &window: windows_) {
                    CalculateInfoWindowPosition(window.get());
                }
            }
        }

        // Update all windows
        for (const auto &window: windows_) {
            window->Update(delta_time);
        }
    }

    int UIWindowManager::AddWindow(std::unique_ptr<UIWindow> window) {
        const int window_id = window->GetId();

        // Set close callback
        window->SetCloseCallback([this, window_id]() {
            RemoveWindow(window_id);
        });

        // Calculate position for the new window
        CalculateWindowPosition(window.get());

        // Set z-order
        window->SetZOrder(next_z_order_++);

        // Add window to the list
        windows_.push_back(std::move(window));

        return window_id;
    }

    int UIWindowManager::AddInfoWindow(std::unique_ptr<UIWindow> window) {
        // Single-window modal system: close any existing windows first
        Clear();

        is_info_window_ = true;

        const int window_id = window->GetId();

        // Set close callback
        window->SetCloseCallback([this, window_id]() {
            RemoveWindow(window_id);
        });

        // Calculate centered position at bottom
        CalculateInfoWindowPosition(window.get());

        // Set z-order
        window->SetZOrder(next_z_order_++);

        // Add window to the list
        windows_.push_back(std::move(window));

        return window_id;
    }

    int UIWindowManager::AddAnalyticsWindow(std::unique_ptr<UIWindow> window) {
        const std::string title = window->GetTitle();

        // Check if a window with this title already exists
        for (const auto &existing: windows_) {
            if (existing->GetTitle() == title) {
                // Window already open, bring to front
                BringToFront(existing->GetId());
                return existing->GetId();
            }
        }

        // No existing window, add as normal window
        return AddWindow(std::move(window));
    }

    void UIWindowManager::RemoveWindow(int window_id) {
        std::erase_if(windows_,
                      [window_id](const std::unique_ptr<UIWindow> &window) {
                          return window->GetId() == window_id;
                      });

        // Update z-orders after removal
        UpdateZOrders();
    }

    void UIWindowManager::Clear() {
        windows_.clear();
        next_z_order_ = 0;
        is_info_window_ = false;
    }

    void UIWindowManager::Render() const {
        // Sort windows by z-order (lower values first)
        std::vector<UIWindow *> sorted_windows;
        sorted_windows.reserve(windows_.size());

        for (const auto &window: windows_) {
            sorted_windows.push_back(window.get());
        }

        std::sort(sorted_windows.begin(), sorted_windows.end(),
                  [](const UIWindow *a, const UIWindow *b) {
                      return a->GetZOrder() < b->GetZOrder();
                  });

        // Render windows in z-order
        for (UIWindow *window: sorted_windows) {
            window->Render();
        }
    }

    bool UIWindowManager::HandleClick(const int mouse_x, const int mouse_y) {
        // Create mouse event
        const MouseEvent event(static_cast<float>(mouse_x), static_cast<float>(mouse_y),
                               false, false, true, false);

        // Check windows in reverse z-order (top windows first)
        std::vector<UIWindow *> sorted_windows;
        sorted_windows.reserve(windows_.size());

        for (const auto &window: windows_) {
            sorted_windows.push_back(window.get());
        }

        std::ranges::sort(sorted_windows,
                          [](const UIWindow *a, const UIWindow *b) {
                              return a->GetZOrder() > b->GetZOrder();
                          });

        for (UIWindow *window: sorted_windows) {
            if (window->Contains(static_cast<float>(mouse_x), static_cast<float>(mouse_y))) {
                // Process mouse event (handles close button + children)
                if (window->ProcessMouseEvent(event)) {
                    // Window handled event (might be close button)
                    return true;
                }

                // Bring clicked window to front
                BringToFront(window->GetId());
                return true;
            }
        }

        return false;
    }

    UIWindow *UIWindowManager::GetWindow(const int window_id) const {
        for (const auto &window: windows_) {
            if (window->GetId() == window_id) {
                return window.get();
            }
        }
        return nullptr;
    }

    void UIWindowManager::BringToFront(const int window_id) {
        UIWindow *target_window = GetWindow(window_id);
        if (!target_window) return;

        // Set this window's z-order to be the highest
        target_window->SetZOrder(next_z_order_++);

        // Update other windows' z-orders if needed
        UpdateZOrders();
    }

    void UIWindowManager::CalculateWindowPosition(UIWindow *window) const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Default position: right side of screen
        int default_x = screen_width - static_cast<int>(window->GetAbsoluteBounds().width) - INITIAL_X_OFFSET;
        int default_y = INITIAL_Y_OFFSET;

        // If there are existing windows, cascade the new window
        if (!windows_.empty()) {
            // Find the topmost window
            const UIWindow *top_window = nullptr;
            int max_z = -1;

            for (const auto &w: windows_) {
                if (w->GetZOrder() > max_z) {
                    max_z = w->GetZOrder();
                    top_window = w.get();
                }
            }

            if (top_window) {
                const Rectangle top_bounds = top_window->GetAbsoluteBounds();

                // Position new window slightly offset from the top window
                default_x = static_cast<int>(top_bounds.x) + WINDOW_SPACING;
                default_y = static_cast<int>(top_bounds.y) + WINDOW_SPACING;

                // If the cascaded position goes off-screen, wrap back to initial position
                if (default_x + window->GetAbsoluteBounds().width > screen_width ||
                    default_y + window->GetAbsoluteBounds().height > screen_height) {
                    default_x = screen_width - static_cast<int>(window->GetAbsoluteBounds().width) - INITIAL_X_OFFSET;
                    default_y = INITIAL_Y_OFFSET;
                }
            }
        }

        window->SetWindowPosition(static_cast<float>(default_x), static_cast<float>(default_y));
    }

    void UIWindowManager::CalculateInfoWindowPosition(UIWindow *window) const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Center horizontally
        const int x = (screen_width - static_cast<int>(window->GetAbsoluteBounds().width)) / 2;

        // Position at bottom with margin for action bar
        const int y = screen_height - static_cast<int>(window->GetAbsoluteBounds().height) - BOTTOM_MARGIN;

        window->SetWindowPosition(static_cast<float>(x), static_cast<float>(y));
    }

    void UIWindowManager::UpdateZOrders() {
        // Compact z-orders to prevent overflow
        std::vector<UIWindow *> sorted_windows;
        sorted_windows.reserve(windows_.size());

        for (const auto &window: windows_) {
            sorted_windows.push_back(window.get());
        }

        std::ranges::sort(sorted_windows,
                          [](const UIWindow *a, const UIWindow *b) {
                              return a->GetZOrder() < b->GetZOrder();
                          });

        for (size_t i = 0; i < sorted_windows.size(); ++i) {
            sorted_windows[i]->SetZOrder(static_cast<int>(i));
        }

        next_z_order_ = static_cast<int>(sorted_windows.size());
    }
}
