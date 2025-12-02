#include "ui/achievements_menu.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "audio/audio_manager.h"
#include <sstream>
#include <iomanip>

import engine;

namespace towerforge::ui {
    AchievementsMenu::AchievementsMenu()
        : close_callback_(nullptr)
          , current_population_(0)
          , current_income_(0.0f)
          , current_floors_(0)
          , current_satisfaction_(0.0f)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0) {
    }

    AchievementsMenu::~AchievementsMenu() = default;

    void AchievementsMenu::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Create main panel centered on screen
        const int panel_x = (screen_width - MENU_WIDTH) / 2;
        const int panel_y = (screen_height - MENU_HEIGHT) / 2;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        main_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Add title text
        auto title_text = std::make_unique<Text>(
            0, 0,
            "ACHIEVEMENTS",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
        );
        main_panel_->AddChild(std::move(title_text));

        // Add divider below title
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(MENU_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Create scrollable container for achievement list
        constexpr int content_height = MENU_HEIGHT - HEADER_HEIGHT - 80; // Header + footer with back button
        constexpr float content_width = MENU_WIDTH - UITheme::PADDING_LARGE * 2;

        auto list_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(content_width, content_height)
                .Layout(std::make_unique<VerticalLayout>(ITEM_PADDING, Alignment::Center))
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Build();

        achievement_list_container_ = list_container.get();
        main_panel_->AddChild(std::move(list_container));

        const int unlocked = achievement_manager_->GetUnlockedCount();
        const int total = achievement_manager_->GetTotalCount();
        const float percentage = achievement_manager_->GetProgressPercentage();
        const std::string progress_text = std::to_string(unlocked) + "/" + std::to_string(total) +
                                          " (" + std::to_string(static_cast<int>(percentage)) + "%)";

        auto progress_label = std::make_unique<Text>(
            0, 0,
            progress_text,
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        main_panel_->AddChild(std::move(progress_label));

        // Add back button at the bottom
        auto back_btn = std::make_unique<Button>(
            150, 40,
            "Back",
            UITheme::FONT_SIZE_MEDIUM
        );
        back_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        back_btn->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        back_btn->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        back_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
        back_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                if (close_callback_) {
                    close_callback_();
                }
                return true;
            }
            return false;
        });
        main_panel_->AddChild(std::move(back_btn));

        UpdateLayout();

        // Build the achievement list if manager is available
        RebuildAchievementList();
    }

    void AchievementsMenu::Shutdown() {
        achievement_list_container_ = nullptr;
        main_panel_.reset();
    }

    void AchievementsMenu::UpdateLayout() {
        last_screen_width_ = GetScreenWidth();
        last_screen_height_ = GetScreenHeight();

        if (main_panel_ != nullptr) {
            // Center the panel
            const int panel_x = (last_screen_width_ - MENU_WIDTH) / 2;
            const int panel_y = (last_screen_height_ - MENU_HEIGHT) / 2;
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }
    }

    void AchievementsMenu::Update(const float delta_time) {
        animation_time_ += delta_time;

        // Check for window resize
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
        main_panel_->UpdateComponentsRecursive(delta_time);
    }

    void AchievementsMenu::RenderDimOverlay() {
        // Dim the background
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        engine::ui::BatchRenderer::SubmitQuad(engine::ui::Rectangle(0, 0, screen_width, screen_height),
                                              UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f)));
    }

    void AchievementsMenu::Render() const {
        RenderDimOverlay();
        main_panel_->Render();
    }

    void AchievementsMenu::RebuildAchievementList() const {
        if (!achievement_list_container_ || !achievement_manager_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Clear existing children by removing them one by one
        while (!achievement_list_container_->GetChildren().empty()) {
            achievement_list_container_->RemoveChild(achievement_list_container_->GetChildren().front().get());
        }

        constexpr float item_width = MENU_WIDTH - UITheme::PADDING_LARGE * 2 - 20; // Account for scrollbar

        // Add unlocked achievements first
        for (const auto &achievement: achievement_manager_->GetUnlockedAchievements()) {
            achievement_list_container_->AddChild(CreateAchievementItem(achievement, true, item_width));
        }

        // Add locked achievements
        for (const auto &achievement: achievement_manager_->GetLockedAchievements()) {
            achievement_list_container_->AddChild(CreateAchievementItem(achievement, false, item_width));
        }

        achievement_list_container_->InvalidateComponents();
        achievement_list_container_->UpdateComponentsRecursive();

        // Update scroll component content size
        if (auto *scroll = achievement_list_container_->GetComponent<engine::ui::components::ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    std::unique_ptr<engine::ui::elements::Container> AchievementsMenu::CreateAchievementItem(
        const towerforge::core::Achievement &achievement,
        const bool is_unlocked,
        const float item_width) const {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Choose colors based on unlock status
        const Color name_color = is_unlocked ? WHITE : LIGHTGRAY;

        auto item_container = engine::ui::ContainerBuilder()
                .Size(item_width, ITEM_HEIGHT)
                .Background(UITheme::ToEngineColor(ColorAlpha(DARKGRAY, 0.2f)))
                .Border(1.0f, UITheme::ToEngineColor(is_unlocked ? GOLD : GRAY))
                .Padding(10)
                .Layout(std::make_unique<HorizontalLayout>(10, Alignment::Start))
                .Build();

        // Icon placeholder
        constexpr int icon_size = ITEM_HEIGHT - 20;
        auto icon_container = engine::ui::ContainerBuilder()
                .Size(icon_size, icon_size)
                .Background(UITheme::ToEngineColor(DARKGRAY))
                .Border(1.0f, UITheme::ToEngineColor(GRAY))
                .Layout(std::make_unique<StackLayout>(Alignment::Center, Alignment::Center))
                .Build();

        auto icon_text = std::make_unique<Text>(
            0, 0, "?", UITheme::FONT_SIZE_LARGE, UITheme::ToEngineColor(name_color)
        );
        icon_container->AddChild(std::move(icon_text));
        item_container->AddChild(std::move(icon_container));

        // Text content container
        auto text_container = engine::ui::ContainerBuilder()
                .Opacity(0)
                .Size(item_width - icon_size - 30, ITEM_HEIGHT - 20)
                .Layout(std::make_unique<VerticalLayout>(2, Alignment::Start))
                .Build();

        auto name_text = std::make_unique<Text>(
            0, 0, achievement.name, UITheme::FONT_SIZE_MEDIUM, UITheme::ToEngineColor(name_color)
        );
        text_container->AddChild(std::move(name_text));

        auto desc_text = std::make_unique<Text>(
            0, 0, achievement.description, UITheme::FONT_SIZE_SMALL, UITheme::ToEngineColor(LIGHTGRAY)
        );
        text_container->AddChild(std::move(desc_text));

        // Status line: unlock date for unlocked, progress for locked
        if (is_unlocked && achievement.unlock_time.has_value()) {
            const auto time = achievement.unlock_time.value();
            const std::time_t tt = std::chrono::system_clock::to_time_t(time);
            std::tm local_tm{};
            localtime_s(&local_tm, &tt);

            std::stringstream ss;
            ss << "Unlocked: " << std::put_time(&local_tm, "%Y-%m-%d");
            auto date_text = std::make_unique<Text>(
                0, 0, ss.str(), UITheme::FONT_SIZE_SMALL, UITheme::ToEngineColor(GREEN)
            );
            text_container->AddChild(std::move(date_text));
        } else if (!is_unlocked) {
            const std::string progress = GetProgressText(achievement);
            auto progress_text_element = std::make_unique<Text>(
                0, 0, progress, UITheme::FONT_SIZE_SMALL, UITheme::ToEngineColor(YELLOW)
            );
            text_container->AddChild(std::move(progress_text_element));
        }

        item_container->AddChild(std::move(text_container));
        return item_container;
    }

    std::string AchievementsMenu::GetProgressText(const towerforge::core::Achievement &achievement) const {
        // Calculate progress based on achievement type
        std::stringstream ss;

        // Use achievement type enum instead of string
        if (achievement.type == towerforge::core::AchievementType::Population) {
            const int current = current_population_;
            const int required = achievement.requirement;
            ss << "Progress: " << current << "/" << required;
        } else if (achievement.type == towerforge::core::AchievementType::Income) {
            const int current = static_cast<int>(current_income_);
            const int required = achievement.requirement;
            ss << "Progress: $" << current << "/$" << required;
        } else if (achievement.type == towerforge::core::AchievementType::Floors) {
            const int current = current_floors_;
            const int required = achievement.requirement;
            ss << "Progress: " << current << "/" << required << " floors";
        } else if (achievement.type == towerforge::core::AchievementType::Satisfaction) {
            const int current = static_cast<int>(current_satisfaction_ * 100);
            const int required = achievement.requirement;
            ss << "Progress: " << current << "%/" << required << "%";
        } else {
            ss << "In Progress...";
        }

        return ss.str();
    }

    bool AchievementsMenu::ProcessMouseEvent(const MouseEvent &event) const {
        const float wheel = GetMouseWheelMove();
        return main_panel_->ProcessMouseEvent({
            event.x,
            event.y,
            event.left_down,
            event.right_down,
            event.left_pressed,
            event.right_pressed,
            wheel
        });
    }

    void AchievementsMenu::HandleKeyboard() const {
        // ESC to close menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (close_callback_) {
                close_callback_();
            }
        }
    }

    void AchievementsMenu::SetCloseCallback(const CloseCallback &callback) {
        close_callback_ = callback;
    }

    void AchievementsMenu::SetAchievementManager(towerforge::core::AchievementManager *manager) {
        achievement_manager_ = manager;
        if (achievement_list_container_) {
            RebuildAchievementList();
        }
    }

    void AchievementsMenu::SetGameStats(const int population, const float total_income,
                                        const int floor_count, const float avg_satisfaction) {
        current_population_ = population;
        current_income_ = total_income;
        current_floors_ = floor_count;
        current_satisfaction_ = avg_satisfaction;

        // Rebuild the list to reflect updated progress
        if (achievement_list_container_ && achievement_manager_) {
            RebuildAchievementList();
        }
    }
} // namespace towerforge::ui
