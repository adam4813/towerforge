#include "ui/achievements_menu.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace towerforge::ui {

    AchievementsMenu::AchievementsMenu()
        : achievement_manager_(nullptr),
          current_population_(0),
          current_income_(0.0f),
          current_floors_(0),
          current_satisfaction_(0.0f),
          scroll_offset_(0.0f),
          animation_time_(0.0f) {
    }

    AchievementsMenu::~AchievementsMenu() = default;

    void AchievementsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;
    }

    void AchievementsMenu::Render() {
        RenderBackground();
        RenderHeader();
        RenderAchievementList();
        RenderFooter();
    }

    bool AchievementsMenu::HandleKeyboard() {
        // ESC to close menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            return true;
        }

        // Arrow keys for scrolling
        if (IsKeyDown(KEY_UP)) {
            scroll_offset_ = std::max(0.0f, scroll_offset_ - SCROLL_SPEED * GetFrameTime());
        }
        if (IsKeyDown(KEY_DOWN)) {
            scroll_offset_ += SCROLL_SPEED * GetFrameTime();
        }

        return false;
    }

    void AchievementsMenu::HandleMouse(int mouse_x, int mouse_y, const float mouse_wheel) {
        // Handle mouse wheel scrolling
        if (mouse_wheel != 0.0f) {
            scroll_offset_ = std::max(0.0f, scroll_offset_ - mouse_wheel * 20.0f);
        }
    }

    void AchievementsMenu::SetAchievementManager(TowerForge::Core::AchievementManager* manager) {
        achievement_manager_ = manager;
    }

    void AchievementsMenu::SetGameStats(const int population, const float total_income, const int floor_count, const float avg_satisfaction) {
        current_population_ = population;
        current_income_ = total_income;
        current_floors_ = floor_count;
        current_satisfaction_ = avg_satisfaction;
    }

    void AchievementsMenu::RenderBackground() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Semi-transparent dark overlay
        DrawRectangle(0, 0, screen_width, screen_height, Color{0, 0, 0, 200});

        // Main menu background
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;

        // Menu background with border
        DrawRectangle(menu_x - 2, menu_y - 2, MENU_WIDTH + 4, MENU_HEIGHT + 4, GOLD);
        DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, Color{20, 20, 30, 255});
    }

    void AchievementsMenu::RenderHeader() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;

        // Header background
        DrawRectangle(menu_x, menu_y, MENU_WIDTH, HEADER_HEIGHT, Color{30, 30, 40, 255});

        // Title
        const auto title = "ACHIEVEMENTS";
        const int title_width = MeasureText(title, 30);
        DrawText(title, menu_x + (MENU_WIDTH - title_width) / 2, menu_y + 15, 30, GOLD);

        // Progress indicator
        if (achievement_manager_) {
            const int unlocked = achievement_manager_->GetUnlockedCount();
            const int total = achievement_manager_->GetTotalCount();
            const float percentage = achievement_manager_->GetProgressPercentage();

            std::ostringstream oss;
            oss << unlocked << "/" << total << " (" << std::fixed << std::setprecision(0) << percentage << "%)";
            const std::string progress_text = oss.str();

            const int progress_width = MeasureText(progress_text.c_str(), 16);
            DrawText(progress_text.c_str(), menu_x + MENU_WIDTH - progress_width - 15, menu_y + 22, 16, LIGHTGRAY);
        }
    }

    void AchievementsMenu::RenderAchievementList() {
        if (!achievement_manager_) {
            return;
        }

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;

        const int list_y = menu_y + HEADER_HEIGHT;
        constexpr int list_height = MENU_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT;

        // Enable scissor mode for scrolling
        BeginScissorMode(menu_x, list_y, MENU_WIDTH, list_height);

        int y_offset = list_y - static_cast<int>(scroll_offset_);

        // Render unlocked achievements first
        const auto unlocked = achievement_manager_->GetUnlockedAchievements();

        if (!unlocked.empty()) {
            // Section header
            DrawText("Unlocked:", menu_x + 15, y_offset, 20, GREEN);
            y_offset += 30;

            for (const auto& achievement : unlocked) {
                RenderAchievementItem(achievement, y_offset, true);
                y_offset += ITEM_HEIGHT + ITEM_PADDING;
            }

            y_offset += 10;
        }

        // Render locked achievements
        const auto locked = achievement_manager_->GetLockedAchievements();

        if (!locked.empty()) {
            // Section header
            DrawText("Locked:", menu_x + 15, y_offset, 20, DARKGRAY);
            y_offset += 30;

            for (const auto& achievement : locked) {
                RenderAchievementItem(achievement, y_offset, false);
                y_offset += ITEM_HEIGHT + ITEM_PADDING;
            }
        }

        EndScissorMode();
    }

    void AchievementsMenu::RenderAchievementItem(const TowerForge::Core::Achievement& achievement, const int y_pos, const bool is_unlocked) const {
        const int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();
        const int menu_x = (screen_width - MENU_WIDTH) / 2;

        const Color bg_color = is_unlocked ? Color{40, 40, 50, 255} : Color{30, 30, 35, 255};
        const Color text_color = is_unlocked ? WHITE : DARKGRAY;
        const Color icon_color = is_unlocked ? GOLD : DARKGRAY;

        // Background
        DrawRectangle(menu_x + 10, y_pos, MENU_WIDTH - 20, ITEM_HEIGHT, bg_color);

        // Icon/Emoji (larger font size for emojis)
        DrawText(achievement.icon.c_str(), menu_x + 20, y_pos + 15, 40, icon_color);

        // Name
        DrawText(achievement.name.c_str(), menu_x + 80, y_pos + 10, 18, text_color);

        // Description
        DrawText(achievement.description.c_str(), menu_x + 80, y_pos + 35, 14, LIGHTGRAY);

        // Unlock date or progress
        if (is_unlocked) {
            const std::string date_str = "Unlocked: " + achievement.GetUnlockDateString();
            DrawText(date_str.c_str(), menu_x + 80, y_pos + 55, 12, GRAY);
        } else {
            const std::string progress_str = GetProgressText(achievement);
            DrawText(progress_str.c_str(), menu_x + 80, y_pos + 55, 12, DARKGRAY);
        }
    }

    void AchievementsMenu::RenderFooter() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_x = (screen_width - MENU_WIDTH) / 2;
        const int menu_y = (screen_height - MENU_HEIGHT) / 2;
        const int footer_y = menu_y + MENU_HEIGHT - FOOTER_HEIGHT;

        // Footer background
        DrawRectangle(menu_x, footer_y, MENU_WIDTH, FOOTER_HEIGHT, Color{30, 30, 40, 255});

        // Instructions
        const auto instructions = "Press ESC to close | Arrow keys or mouse wheel to scroll";
        const int instructions_width = MeasureText(instructions, 14);
        DrawText(instructions, menu_x + (MENU_WIDTH - instructions_width) / 2, footer_y + 13, 14, LIGHTGRAY);
    }

    std::string AchievementsMenu::GetProgressText(const TowerForge::Core::Achievement& achievement) const {
        std::ostringstream oss;

        switch (achievement.type) {
            case TowerForge::Core::AchievementType::Population:
                oss << "Progress: " << current_population_ << "/" << achievement.requirement;
                break;
            case TowerForge::Core::AchievementType::Income:
                oss << "Progress: $" << std::fixed << std::setprecision(0) << current_income_
                        << "/$" << achievement.requirement;
                break;
            case TowerForge::Core::AchievementType::Floors:
                oss << "Progress: " << current_floors_ << "/" << achievement.requirement << " floors";
                break;
            case TowerForge::Core::AchievementType::Satisfaction:
                oss << "Progress: " << std::fixed << std::setprecision(0) << current_satisfaction_
                        << "%/" << achievement.requirement << "%";
                break;
            case TowerForge::Core::AchievementType::SpecialEvent:
                oss << "Locked";
                break;
            default:
                oss << "Locked";
                break;
        }

        return oss.str();
    }

}
