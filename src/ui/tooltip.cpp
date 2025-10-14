#include "ui/tooltip.h"
#include <algorithm>
#include <sstream>

namespace towerforge::ui {

    // Tooltip implementation
    Tooltip::Tooltip(const std::string& text)
        : static_text_(text)
          , text_generator_(nullptr)
          , is_dynamic_(false)
          , visible_(true) {
    }

    Tooltip::Tooltip(const std::function<std::string()> &text_generator)
          : text_generator_(text_generator)
          , is_dynamic_(true)
          , visible_(true) {
    }

    std::string Tooltip::GetText() const {
        if (!visible_) {
            return "";
        }

        if (is_dynamic_ && text_generator_) {
            return text_generator_();
        }
        return static_text_;
    }

    bool Tooltip::ShouldShow() const {
        return visible_ && !GetText().empty();
    }

    // TooltipManager implementation
    TooltipManager::TooltipManager()
        : current_tooltip_(nullptr)
          , tooltip_x_(0)
          , tooltip_y_(0)
          , element_width_(0)
          , element_height_(0)
          , hover_time_(0.0f)
          , is_visible_(false)
          , keyboard_focus_id_(-1) {
    }

    TooltipManager::~TooltipManager() = default;

    void TooltipManager::Update(const int mouse_x, const int mouse_y) {
        if (!current_tooltip_) {
            return;
        }

        // Check if still hovering over the element
        if (IsHovering(mouse_x, mouse_y, tooltip_x_, tooltip_y_, element_width_, element_height_)) {
            hover_time_ += GetFrameTime();

            // Show tooltip after delay
            if (hover_time_ >= HOVER_DELAY) {
                is_visible_ = true;
            }
        } else {
            // Reset if no longer hovering
            HideTooltip();
        }
    }

    void TooltipManager::Render() const {
        if (!is_visible_ || !current_tooltip_ || !current_tooltip_->ShouldShow()) {
            return;
        }

        const std::string text = current_tooltip_->GetText();
        if (text.empty()) {
            return;
        }

        // Position tooltip below the element
        const int render_x = tooltip_x_;
        const int render_y = tooltip_y_ + element_height_ + TOOLTIP_OFFSET_Y;

        RenderTooltipBox(text, render_x, render_y);
    }

    void TooltipManager::ShowTooltip(const Tooltip& tooltip, const int x, const int y, const int width, const int height) {
        current_tooltip_ = std::make_unique<Tooltip>(tooltip);
        tooltip_x_ = x;
        tooltip_y_ = y;
        element_width_ = width;
        element_height_ = height;
        hover_time_ = 0.0f;
        is_visible_ = false;  // Will become visible after hover delay
    }

    void TooltipManager::HideTooltip() {
        current_tooltip_.reset();
        hover_time_ = 0.0f;
        is_visible_ = false;
    }

    bool TooltipManager::IsHovering(const int mouse_x, const int mouse_y, const int x, const int y, const int width, const int height) {
        return mouse_x >= x && mouse_x <= x + width &&
               mouse_y >= y && mouse_y <= y + height;
    }

    void TooltipManager::RenderTooltipBox(const std::string& text, int x, int y) const {
        // Word wrap the text
        std::vector<std::string> lines;
        std::istringstream iss(text);
        std::string word;
        std::string current_line;

        while (iss >> word) {
            std::string test_line = current_line.empty() ? word : current_line + " " + word;

            if (const int text_width = MeasureText(test_line.c_str(), TOOLTIP_FONT_SIZE);
                text_width > TOOLTIP_MAX_WIDTH - TOOLTIP_PADDING * 2) {
                if (!current_line.empty()) {
                    lines.push_back(current_line);
                    current_line = word;
                } else {
                    // Single word is too long, just use it
                    lines.push_back(word);
                    current_line = "";
                }
            } else {
                current_line = test_line;
            }
        }

        if (!current_line.empty()) {
            lines.push_back(current_line);
        }

        // Calculate tooltip dimensions
        int max_line_width = 0;
        for (const auto& line : lines) {
            int line_width = MeasureText(line.c_str(), TOOLTIP_FONT_SIZE);
            max_line_width = std::max(max_line_width, line_width);
        }

        const int tooltip_width = std::min(max_line_width + TOOLTIP_PADDING * 2, TOOLTIP_MAX_WIDTH);
        constexpr int line_height = TOOLTIP_FONT_SIZE + 4;
        const int tooltip_height = static_cast<int>(lines.size()) * line_height + TOOLTIP_PADDING * 2;

        // Adjust position to stay on screen
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        if (x + tooltip_width > screen_width) {
            x = screen_width - tooltip_width - 5;
        }
        if (x < 5) {
            x = 5;
        }

        if (y + tooltip_height > screen_height) {
            y = tooltip_y_ - tooltip_height - TOOLTIP_OFFSET_Y;  // Show above element
        }
        if (y < 5) {
            y = 5;
        }

        // Draw background
        DrawRectangle(x, y, tooltip_width, tooltip_height, ColorAlpha(BLACK, 0.95f));
        DrawRectangleLines(x, y, tooltip_width, tooltip_height, GOLD);

        // Draw text
        int text_y = y + TOOLTIP_PADDING;
        for (const auto& line : lines) {
            DrawText(line.c_str(), x + TOOLTIP_PADDING, text_y, TOOLTIP_FONT_SIZE, WHITE);
            text_y += line_height;
        }
    }

}
