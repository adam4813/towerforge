#include "ui/history_panel.h"
#include "ui/mouse_interface.h"
#include "ui/ui_theme.h"
#include "core/command_history.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>

import engine;

namespace towerforge::ui {
    HistoryPanel::HistoryPanel()
        : visible_(false)
          , scroll_offset_(0)
          , hovered_index_(-1)
          , visible_items_count_(0) {
        // Position in top-right corner
        panel_bounds_ = {
            static_cast<float>(GetScreenWidth() - PANEL_WIDTH - 10),
            static_cast<float>(60),
            static_cast<float>(PANEL_WIDTH),
            static_cast<float>(PANEL_MAX_HEIGHT)
        };

        content_bounds_ = {
            panel_bounds_.x + PADDING,
            panel_bounds_.y + HEADER_HEIGHT + PADDING,
            panel_bounds_.width - PADDING * 2,
            panel_bounds_.height - HEADER_HEIGHT - PADDING * 2
        };
    }

    HistoryPanel::~HistoryPanel() = default;

    void HistoryPanel::Render() {
        if (!visible_) {
            return;
        }

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Update panel position in case screen size changed
        panel_bounds_.x = static_cast<float>(screen_width - PANEL_WIDTH - 10);

        // Draw semi-transparent background
        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(panel_bounds_.x, panel_bounds_.y, panel_bounds_.width, panel_bounds_.height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.85f))
        );

        // Draw border using 4 lines
        const auto border_col = UITheme::ToEngineColor(GOLD);
        engine::ui::BatchRenderer::SubmitLine(panel_bounds_.x, panel_bounds_.y, 
                                              panel_bounds_.x + panel_bounds_.width, panel_bounds_.y, 
                                              2.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(panel_bounds_.x + panel_bounds_.width, panel_bounds_.y, 
                                              panel_bounds_.x + panel_bounds_.width, panel_bounds_.y + panel_bounds_.height, 
                                              2.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(panel_bounds_.x + panel_bounds_.width, panel_bounds_.y + panel_bounds_.height, 
                                              panel_bounds_.x, panel_bounds_.y + panel_bounds_.height, 
                                              2.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(panel_bounds_.x, panel_bounds_.y + panel_bounds_.height, 
                                              panel_bounds_.x, panel_bounds_.y, 
                                              2.0f, border_col);

        // Draw header
        engine::ui::BatchRenderer::SubmitText("Action History",
                                              panel_bounds_.x + PADDING,
                                              panel_bounds_.y + PADDING,
                                              16, border_col);

        // Draw help text
        engine::ui::BatchRenderer::SubmitText("Click to undo/redo",
                                              panel_bounds_.x + PADDING,
                                              panel_bounds_.y + PADDING + 16.0f,
                                              10, UITheme::ToEngineColor(GRAY));

        if (entries_.empty()) {
            engine::ui::BatchRenderer::SubmitText("No actions yet",
                                                  content_bounds_.x + PADDING,
                                                  content_bounds_.y + 20.0f,
                                                  12, UITheme::ToEngineColor(GRAY));
            return;
        }

        // Calculate visible items
        visible_items_count_ = static_cast<int>(content_bounds_.height / ITEM_HEIGHT);
        const int max_scroll = std::max(0, static_cast<int>(entries_.size()) - visible_items_count_);
        scroll_offset_ = std::clamp(scroll_offset_, 0, max_scroll);

        // Draw entries
        int y_offset = static_cast<int>(content_bounds_.y);
        const int start_index = scroll_offset_;
        const int end_index = std::min(static_cast<int>(entries_.size()), start_index + visible_items_count_);

        for (int i = start_index; i < end_index; i++) {
            const auto &entry = entries_[i];

            const Rectangle item_bounds = {
                content_bounds_.x,
                static_cast<float>(y_offset),
                content_bounds_.width,
                static_cast<float>(ITEM_HEIGHT)
            };

            // Highlight on hover
            const bool is_hovered = (i == hovered_index_);
            if (is_hovered) {
                engine::ui::BatchRenderer::SubmitQuad(
                    engine::ui::Rectangle(item_bounds.x, item_bounds.y, item_bounds.width, item_bounds.height),
                    UITheme::ToEngineColor(ColorAlpha(GOLD, 0.2f))
                );
            }

            // Draw separator line
            if (i > start_index) {
                engine::ui::BatchRenderer::SubmitLine(
                    item_bounds.x,
                    item_bounds.y,
                    item_bounds.x + item_bounds.width,
                    item_bounds.y,
                    1.0f,
                    UITheme::ToEngineColor(ColorAlpha(GRAY, 0.5f))
                );
            }

            // Color based on whether it's undo or redo
            const Color action_color = entry.is_redo ? ColorAlpha(SKYBLUE, 0.7f) : WHITE;

            // Draw description
            engine::ui::BatchRenderer::SubmitText(entry.description,
                                                  item_bounds.x + 5.0f,
                                                  item_bounds.y + 5.0f,
                                                  11, UITheme::ToEngineColor(action_color));

            // Draw timestamp
            engine::ui::BatchRenderer::SubmitText(entry.time_str,
                                                  item_bounds.x + 5.0f,
                                                  item_bounds.y + 20.0f,
                                                  9, UITheme::ToEngineColor(GRAY));

            // Draw cost change
            const Color cost_color = entry.cost_change >= 0 ? GREEN : RED;
            std::stringstream cost_ss;
            if (entry.cost_change >= 0) {
                cost_ss << "+$" << entry.cost_change;
            } else {
                cost_ss << "-$" << -entry.cost_change;
            }
            engine::ui::BatchRenderer::SubmitText(cost_ss.str(),
                                                  item_bounds.x + 5.0f,
                                                  item_bounds.y + 33.0f,
                                                  10, UITheme::ToEngineColor(cost_color));

            // Draw redo/undo indicator
            const char *status = entry.is_redo ? "[Can Redo]" : "[Can Undo]";
            const int status_width = MeasureText(status, 9);
            engine::ui::BatchRenderer::SubmitText(status,
                                                  item_bounds.x + item_bounds.width - static_cast<float>(status_width) - 5.0f,
                                                  item_bounds.y + 5.0f,
                                                  9, UITheme::ToEngineColor(entry.is_redo ? SKYBLUE : GOLD));

            y_offset += ITEM_HEIGHT;
        }

        // Draw scroll indicator if needed
        if (static_cast<int>(entries_.size()) > visible_items_count_) {
            const float scroll_bar_height = (static_cast<float>(visible_items_count_) / entries_.size()) *
                                            content_bounds_.height;
            const float scroll_bar_y = content_bounds_.y + (static_cast<float>(scroll_offset_) / entries_.size()) *
                                       content_bounds_.height;

            engine::ui::BatchRenderer::SubmitQuad(
                engine::ui::Rectangle(panel_bounds_.x + panel_bounds_.width - 8.0f,
                                      scroll_bar_y,
                                      4.0f,
                                      scroll_bar_height),
                UITheme::ToEngineColor(ColorAlpha(GOLD, 0.6f))
            );
        }
    }

    int HistoryPanel::HandleClick(const int mouse_x, const int mouse_y) const {
        if (!visible_ || entries_.empty()) {
            return 0;
        }

        // Check if click is within content bounds
        if (mouse_x < content_bounds_.x ||
            mouse_x > content_bounds_.x + content_bounds_.width ||
            mouse_y < content_bounds_.y ||
            mouse_y > content_bounds_.y + content_bounds_.height) {
            return 0;
        }

        // Calculate which item was clicked
        const int relative_y = mouse_y - static_cast<int>(content_bounds_.y);
        const int clicked_index = scroll_offset_ + (relative_y / ITEM_HEIGHT);

        if (clicked_index < 0 || clicked_index >= static_cast<int>(entries_.size())) {
            return 0;
        }

        // Calculate how many steps to undo or redo
        // Find the separator between undo and redo stacks
        int undo_count = 0;
        int redo_count = 0;
        for (size_t i = 0; i < entries_.size(); i++) {
            if (entries_[i].is_redo) {
                redo_count++;
            } else {
                undo_count++;
            }
        }

        if (entries_[clicked_index].is_redo) {
            // Clicked on redo stack - return negative number (redo steps)
            // Count how many redo steps from the end
            int steps_from_end = 0;
            for (int i = static_cast<int>(entries_.size()) - 1; i >= 0; i--) {
                if (!entries_[i].is_redo) break;
                steps_from_end++;
                if (i == clicked_index) {
                    return -steps_from_end; // Negative for redo
                }
            }
        } else {
            // Clicked on undo stack - return positive number (undo steps)
            int steps_from_top = 1;
            for (int i = clicked_index + 1; i < static_cast<int>(entries_.size()); i++) {
                if (entries_[i].is_redo) break;
                steps_from_top++;
            }
            return steps_from_top; // Positive for undo
        }

        return 0;
    }

    void HistoryPanel::UpdateFromHistory(const towerforge::core::CommandHistory &history) {
        entries_.clear();

        // Add undo stack entries (in reverse order, most recent first)
        const auto &undo_stack = history.GetUndoStack();
        for (auto it = undo_stack.rbegin(); it != undo_stack.rend(); ++it) {
            // Format timestamp
            auto time_t = std::chrono::system_clock::to_time_t(it->timestamp);
            std::tm tm = *std::localtime(&time_t);
            std::stringstream time_ss;
            time_ss << std::put_time(&tm, "%H:%M:%S");

            entries_.emplace_back(
                it->description,
                time_ss.str(),
                it->cost_change,
                false // Not redo
            );
        }

        // Add redo stack entries (in order, oldest first)
        const auto &redo_stack = history.GetRedoStack();
        for (auto it = redo_stack.rbegin(); it != redo_stack.rend(); ++it) {
            auto time_t = std::chrono::system_clock::to_time_t(it->timestamp);
            std::tm tm = *std::localtime(&time_t);
            std::stringstream time_ss;
            time_ss << std::put_time(&tm, "%H:%M:%S");

            entries_.emplace_back(
                it->description,
                time_ss.str(),
                it->cost_change,
                true // Is redo
            );
        }

        // Update hovered index based on mouse position
        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();
        hovered_index_ = -1;

        if (IsMouseOver(mouse_x, mouse_y)) {
            const int relative_y = mouse_y - static_cast<int>(content_bounds_.y);
            const int hover_index = scroll_offset_ + (relative_y / ITEM_HEIGHT);
            if (hover_index >= 0 && hover_index < static_cast<int>(entries_.size())) {
                hovered_index_ = hover_index;
            }
        }
    }

    bool HistoryPanel::IsMouseOver(const int mouse_x, const int mouse_y) const {
        return visible_ &&
               CheckCollisionPointRec(
                   Vector2{static_cast<float>(mouse_x), static_cast<float>(mouse_y)},
                   panel_bounds_
               );
    }

    bool HistoryPanel::ProcessMouseEvent(const MouseEvent &event) {
        if (!event.left_pressed) {
            return false; // Only handle clicks
        }

        // Delegate to legacy HandleClick for now
        // TODO: Refactor to use callback pattern instead of return value
        const int steps = HandleClick(static_cast<int>(event.x), static_cast<int>(event.y));
        return steps != 0; // Return true if an action was triggered
    }
}
