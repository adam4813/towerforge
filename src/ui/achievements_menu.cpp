#include "ui/achievements_menu.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/panel_header_overlay.h"
#include "ui/dim_overlay.h"
#include "ui/mouse_interface.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge::ui {

    AchievementsMenu::AchievementsMenu()
        : achievement_manager_(nullptr)
          , current_population_(0)
          , current_income_(0.0f)
          , current_floors_(0)
          , current_satisfaction_(0.0f)
          , scroll_offset_(0.0f)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , close_requested_(false) {
        
        // Create main panel centered on screen
        const Color background_color = ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f);
        main_panel_ = std::make_unique<Panel>(0, 0, MENU_WIDTH, MENU_HEIGHT, background_color, UITheme::BORDER_ACCENT);
        main_panel_->SetPadding(UITheme::PADDING_MEDIUM);
        
        // Create header overlay
        header_overlay_ = std::make_unique<PanelHeaderOverlay>("ACHIEVEMENTS");
        
        // Create dim overlay
        dim_overlay_ = std::make_unique<DimOverlay>();

        // Add back button at the bottom
        constexpr int back_button_y = MENU_HEIGHT - 60;
        auto back_btn = std::make_unique<Button>(
            40.0f,
            static_cast<float>(back_button_y),
            150,
            40,
            "Back",
            UITheme::BUTTON_BACKGROUND,
            UITheme::BUTTON_BORDER
        );
        back_btn->SetClickCallback([this]() {
            close_requested_ = true;
        });
        main_panel_->AddChild(std::move(back_btn));

        // Calculate initial layout
        UpdateLayout();
    }

    AchievementsMenu::~AchievementsMenu() = default;

    void AchievementsMenu::UpdateLayout() {
        last_screen_width_ = GetScreenWidth();
        last_screen_height_ = GetScreenHeight();

        // Center the panel
        const int panel_x = (last_screen_width_ - MENU_WIDTH) / 2;
        const int panel_y = (last_screen_height_ - MENU_HEIGHT) / 2;
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
    }

    void AchievementsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void AchievementsMenu::Render() {
        // Render dim overlay
        dim_overlay_->Render();

        // Render main panel background
        main_panel_->Render();

        // Render header overlay on top
        const Rectangle panel_bounds = main_panel_->GetAbsoluteBounds();
        header_overlay_->Render(
            static_cast<int>(panel_bounds.x),
            static_cast<int>(panel_bounds.y),
            MENU_WIDTH
        );

        // Render progress indicator in header
        if (achievement_manager_) {
            const int unlocked = achievement_manager_->GetUnlockedCount();
            const int total = achievement_manager_->GetTotalCount();
            const float percentage = achievement_manager_->GetProgressPercentage();

            const int text_x = static_cast<int>(panel_bounds.x) + MENU_WIDTH - 150;
            const int text_y = static_cast<int>(panel_bounds.y) + 20;
            
            const std::string progress_text = std::to_string(unlocked) + "/" + std::to_string(total) + 
                                              " (" + std::to_string(static_cast<int>(percentage)) + "%)";
            DrawText(progress_text.c_str(), text_x, text_y, 16, LIGHTGRAY);
        }

        // Render achievement list
        RenderAchievementList();
    }

    void AchievementsMenu::RenderAchievementList() const {
        if (!achievement_manager_) return;

        const Rectangle panel_bounds = main_panel_->GetAbsoluteBounds();
        const int content_x = static_cast<int>(panel_bounds.x) + 40;  // Left margin
        const int content_y = static_cast<int>(panel_bounds.y) + 80;  // Below header
        const int content_width = MENU_WIDTH - 80;  // Left + right margin
        const int content_height = MENU_HEIGHT - 80 - 80;  // Header + footer with back button

        // Set up scissor mode for clipping
        BeginScissorMode(content_x, content_y, content_width, content_height);

        const auto& unlocked = achievement_manager_->GetUnlockedAchievements();
        const auto& locked = achievement_manager_->GetLockedAchievements();

        int y_offset = content_y - static_cast<int>(scroll_offset_);

        // Render unlocked achievements first
        for (const auto& achievement : unlocked) {
            if (y_offset + ITEM_HEIGHT > content_y && y_offset < content_y + content_height) {
                RenderAchievementItem(achievement, y_offset, content_x, content_width, true);
            }
            y_offset += ITEM_HEIGHT + ITEM_PADDING;
        }

        // Render locked achievements
        for (const auto& achievement : locked) {
            if (y_offset + ITEM_HEIGHT > content_y && y_offset < content_y + content_height) {
                RenderAchievementItem(achievement, y_offset, content_x, content_width, false);
            }
            y_offset += ITEM_HEIGHT + ITEM_PADDING;
        }

        EndScissorMode();
    }

    void AchievementsMenu::RenderAchievementItem(const towerforge::core::Achievement& achievement,
                                                  const int y_pos, const int x, const int width, const bool is_unlocked) const {

        // Background
        const Color bg_color = is_unlocked ? ColorAlpha(GOLD, 0.2f) : ColorAlpha(DARKGRAY, 0.2f);
        DrawRectangle(x, y_pos, width, ITEM_HEIGHT, bg_color);
        DrawRectangleLines(x, y_pos, width, ITEM_HEIGHT, is_unlocked ? GOLD : GRAY);

        // Icon placeholder (could be actual icon later)
        const int icon_size = ITEM_HEIGHT - 20;
        const int icon_x = x + 10;
        const int icon_y = y_pos + 10;
        const Color icon_color = is_unlocked ? GOLD : DARKGRAY;
        DrawRectangle(icon_x, icon_y, icon_size, icon_size, ColorAlpha(icon_color, 0.3f));
        DrawRectangleLines(icon_x, icon_y, icon_size, icon_size, icon_color);
        DrawText("?", icon_x + icon_size / 2 - 8, icon_y + icon_size / 2 - 10, 20, icon_color);

        // Achievement name and description
        const int text_x = icon_x + icon_size + 15;
        const int name_y = y_pos + 10;
        DrawText(achievement.name.c_str(), text_x, name_y, 18, is_unlocked ? WHITE : LIGHTGRAY);

        const int desc_y = name_y + 22;
        DrawText(achievement.description.c_str(), text_x, desc_y, 14, LIGHTGRAY);

        // Status (unlocked date or progress)
        const int status_y = desc_y + 18;
        if (is_unlocked) {
            // Show unlock date (format from unlock_time)
            if (achievement.unlock_time.has_value()) {
                const auto time = achievement.unlock_time.value();
                const std::time_t tt = std::chrono::system_clock::to_time_t(time);
                std::tm local_tm;
                localtime_s(&local_tm, &tt);
                
                std::stringstream ss;
                ss << "Unlocked: " << std::put_time(&local_tm, "%Y-%m-%d");
                DrawText(ss.str().c_str(), text_x, status_y, 12, GREEN);
            }
        } else {
            // Show progress
            const std::string progress = GetProgressText(achievement);
            DrawText(progress.c_str(), text_x, status_y, 12, YELLOW);
        }
    }

    std::string AchievementsMenu::GetProgressText(const towerforge::core::Achievement& achievement) const {
        // Calculate progress based on achievement type
        std::stringstream ss;
        
        // Use achievement type enum instead of string
        if (achievement.type == towerforge::core::AchievementType::Population) {
            const int current = current_population_;
            const int required = achievement.requirement;
            ss << "Progress: " << current << "/" << required;
        }
        else if (achievement.type == towerforge::core::AchievementType::Income) {
            const int current = static_cast<int>(current_income_);
            const int required = achievement.requirement;
            ss << "Progress: $" << current << "/$" << required;
        }
        else if (achievement.type == towerforge::core::AchievementType::Floors) {
            const int current = current_floors_;
            const int required = achievement.requirement;
            ss << "Progress: " << current << "/" << required << " floors";
        }
        else if (achievement.type == towerforge::core::AchievementType::Satisfaction) {
            const int current = static_cast<int>(current_satisfaction_ * 100);
            const int required = achievement.requirement;
            ss << "Progress: " << current << "%/" << required << "%";
        }
        else {
            ss << "In Progress...";
        }

        return ss.str();
    }

    bool AchievementsMenu::ProcessMouseEvent(const MouseEvent& event) {
        // Process through panel (handles back button clicks)
        main_panel_->ProcessMouseEvent(event);

        // Handle scrolling with mouse wheel (Raylib's GetMouseWheelMove is global)
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.0f && achievement_manager_) {
            // Calculate max scroll based on content
            const auto& unlocked = achievement_manager_->GetUnlockedAchievements();
            const auto& locked = achievement_manager_->GetLockedAchievements();
            const int total_items = static_cast<int>(unlocked.size() + locked.size());
            const int total_content_height = total_items * (ITEM_HEIGHT + ITEM_PADDING);
            const int visible_height = MENU_HEIGHT - 80 - 80;  // Header + footer
            const float max_scroll = std::max(0.0f, static_cast<float>(total_content_height - visible_height));
            
            scroll_offset_ -= wheel * 20.0f;
            scroll_offset_ = std::clamp(scroll_offset_, 0.0f, max_scroll);
            return true;
        }

        return true;  // Always consume events when menu is open
    }

    void AchievementsMenu::HandleMouse(const int mouse_x, const int mouse_y, const float mouse_wheel) {
        // Legacy wrapper - delegates to modern API
        const MouseEvent event(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            false,
            false,
            false,
            false
        );
        ProcessMouseEvent(event);
        
        // Handle mouse wheel (passed as parameter in legacy API)
        if (mouse_wheel != 0.0f) {
            scroll_offset_ = std::max(0.0f, scroll_offset_ - mouse_wheel * 20.0f);
        }
    }

    bool AchievementsMenu::HandleKeyboard() {
        // Check if back button was clicked
        if (close_requested_) {
            close_requested_ = false;
            return true;  // Signal to close
        }

        // ESC to close menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            return true;  // Signal to close
        }

        // Arrow keys for scrolling
        if (IsKeyDown(KEY_UP)) {
            scroll_offset_ = std::max(0.0f, scroll_offset_ - SCROLL_SPEED * GetFrameTime());
        }
        if (IsKeyDown(KEY_DOWN)) {
            if (achievement_manager_) {
                const auto& unlocked = achievement_manager_->GetUnlockedAchievements();
                const auto& locked = achievement_manager_->GetLockedAchievements();
                const int total_items = static_cast<int>(unlocked.size() + locked.size());
                const int total_content_height = total_items * (ITEM_HEIGHT + ITEM_PADDING);
                const int visible_height = MENU_HEIGHT - 80 - 80;
                const float max_scroll = std::max(0.0f, static_cast<float>(total_content_height - visible_height));
                
                scroll_offset_ = std::min(max_scroll, scroll_offset_ + SCROLL_SPEED * GetFrameTime());
            }
        }

        return false;
    }

    void AchievementsMenu::SetAchievementManager(towerforge::core::AchievementManager* manager) {
        achievement_manager_ = manager;
    }

    void AchievementsMenu::SetGameStats(const int population, const float total_income, 
                                        const int floor_count, const float avg_satisfaction) {
        current_population_ = population;
        current_income_ = total_income;
        current_floors_ = floor_count;
        current_satisfaction_ = avg_satisfaction;
    }

} // namespace towerforge::ui
