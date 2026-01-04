#include "ui/tooltip.h"
#include "ui/ui_theme.h"
#include <algorithm>
#include <sstream>

import engine;

namespace towerforge::ui {
    // Tooltip implementation
    Tooltip::Tooltip(const std::string &text)
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

    void TooltipManager::ShowTooltip(const Tooltip &tooltip, const int x, const int y, const int width,
                                     const int height) {
        current_tooltip_ = std::make_unique<Tooltip>(tooltip);
        tooltip_x_ = x;
        tooltip_y_ = y;
        element_width_ = width;
        element_height_ = height;
        hover_time_ = 0.0f;
        is_visible_ = false; // Will become visible after hover delay
    }

    void TooltipManager::HideTooltip() {
        current_tooltip_.reset();
        hover_time_ = 0.0f;
        is_visible_ = false;
    }

    bool TooltipManager::IsHovering(const int mouse_x, const int mouse_y, const int x, const int y, const int width,
                                    const int height) {
        return mouse_x >= x && mouse_x <= x + width &&
               mouse_y >= y && mouse_y <= y + height;
    }

    bool TooltipManager::IsHoveringRec(const int mouse_x, const int mouse_y, const Rectangle rect) {
        return CheckCollisionPointRec({static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, rect);
    }

    void TooltipManager::RenderTooltipBox(const std::string &text, int x, int y) const {
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
        for (const auto &line: lines) {
            int line_width = MeasureText(line.c_str(), TOOLTIP_FONT_SIZE);
            max_line_width = std::max(max_line_width, line_width);
        }

        const int tooltip_width = std::min(max_line_width + TOOLTIP_PADDING * 2, TOOLTIP_MAX_WIDTH);
        constexpr int line_height = TOOLTIP_FONT_SIZE + 4;
        const int tooltip_height = static_cast<int>(lines.size()) * line_height + TOOLTIP_PADDING * 2;

        // Adjust position to stay on screen
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (x + tooltip_width > screen_width) {
            x = screen_width - tooltip_width - 5;
        }
        if (x < 5) {
            x = 5;
        }

        if (y + tooltip_height > screen_height) {
            y = tooltip_y_ - tooltip_height - TOOLTIP_OFFSET_Y; // Show above element
        }
        if (y < 5) {
            y = 5;
        }

        // Draw background using engine BatchRenderer
        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(tooltip_width), static_cast<float>(tooltip_height)),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.95f))
        );

        // Draw border using 4 lines
        const auto border_col = UITheme::ToEngineColor(UITheme::PRIMARY);
        engine::ui::BatchRenderer::SubmitLine(x, y, x + tooltip_width, y, 1.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(x + tooltip_width, y, x + tooltip_width, y + tooltip_height, 1.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(x + tooltip_width, y + tooltip_height, x, y + tooltip_height, 1.0f, border_col);
        engine::ui::BatchRenderer::SubmitLine(x, y + tooltip_height, x, y, 1.0f, border_col);

        // Draw text using engine BatchRenderer
        int text_y = y + TOOLTIP_PADDING;
        for (const auto &line: lines) {
            engine::ui::BatchRenderer::SubmitText(line, static_cast<float>(x + TOOLTIP_PADDING),
                                                  static_cast<float>(text_y), TOOLTIP_FONT_SIZE,
                                                  UITheme::ToEngineColor(WHITE));
            text_y += line_height;
        }
    }
}
