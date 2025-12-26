#include "ui/build_menu.h"
#include "ui/tooltip.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "ui/help_system.h"
#include "ui/action_bar.h"
#include "audio/audio_manager.h"
#include <sstream>
#include <algorithm>

import engine;

namespace towerforge::ui {
    BuildMenu::BuildMenu()
        : selected_facility_(-1)
          , visible_(false)
          , tutorial_mode_(false)
          , tooltip_manager_(nullptr)
          , facility_selected_callback_(nullptr)
          , close_callback_(nullptr)
          , current_category_(FacilityCategory::Core)
          , last_screen_width_(0)
          , last_screen_height_(0) {
        // Initialize facility types with costs, properties, and categories
        facility_types_.emplace_back("Lobby", "L", 1000, 10, GOLD, FacilityCategory::Core);
        facility_types_.emplace_back("Elevator", "E", 15000, 2, GRAY, FacilityCategory::Core);

        facility_types_.emplace_back("Shop", "S", 6000, 4, GREEN, FacilityCategory::Commercial);
        facility_types_.emplace_back("Restaurant", "R", 8000, 6, RED, FacilityCategory::Commercial);

        facility_types_.emplace_back("Hotel", "H", 12000, 10, PURPLE, FacilityCategory::Residential);

        facility_types_.emplace_back("Gym", "G", 10000, 7, ORANGE, FacilityCategory::Entertainment);
        facility_types_.emplace_back("Arcade", "A", 9000, 5, MAGENTA, FacilityCategory::Entertainment);
        facility_types_.emplace_back("Theater", "T", 15000, 8, VIOLET, FacilityCategory::Entertainment);

        facility_types_.emplace_back("Office", "O", 5000, 8, SKYBLUE, FacilityCategory::Professional);
        facility_types_.emplace_back("Conference", "C", 13000, 9, SKYBLUE, FacilityCategory::Professional);
        facility_types_.emplace_back("Flagship", "F", 18000, 12, Color{0, 206, 209, 255},
                                     FacilityCategory::Professional);
    }

    BuildMenu::~BuildMenu() = default;

    void BuildMenu::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_width = ActionBar::CalculateBarWidth();
        const float x = static_cast<float>((screen_width - menu_width) / 2);
        const float y = static_cast<float>(screen_height - MENU_HEIGHT - 60);

        // Create main panel
        main_panel_ = std::make_unique<engine::ui::elements::Panel>();
        main_panel_->SetRelativePosition(x, y);
        main_panel_->SetSize(static_cast<float>(menu_width), static_cast<float>(MENU_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_ACCENT));
        main_panel_->SetPadding(0);

        // Create tab container for categories
        tab_container_ = std::make_unique<TabContainer>(
            static_cast<float>(menu_width),
            static_cast<float>(MENU_HEIGHT - CLOSE_BUTTON_SIZE - 10)
        );
        tab_container_->SetTabBarHeight(TAB_HEIGHT);
        tab_container_->SetTabBarColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_DARK, 0.9f)));
        tab_container_->SetActiveTabColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        tab_container_->SetInactiveTabColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.6f)));
        tab_container_->SetTabTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
        tab_container_->SetContentBackgroundColor(UITheme::ToEngineColor(UITheme::BACKGROUND_PANEL));
        tab_container_->AddComponent<ConstraintComponent>(Anchor::Fill());

        // Add tabs for each category with content
        const std::vector<std::pair<FacilityCategory, std::string> > categories = {
            {FacilityCategory::Core, "Core"},
            {FacilityCategory::Commercial, "Commercial"},
            {FacilityCategory::Residential, "Residential"},
            {FacilityCategory::Entertainment, "Entertainment"},
            {FacilityCategory::Professional, "Professional"}
        };

        for (const auto &[category, name]: categories) {
            auto content = CreateCategoryContent(category, menu_width);
            tab_container_->AddTab(name, std::move(content));
        }

        // Set tab changed callback
        tab_container_->SetTabChangedCallback([this](size_t index, const std::string &) {
            current_category_ = static_cast<FacilityCategory>(index);
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
        });
        main_panel_->AddChild(std::move(tab_container_));

        // Create close button
        auto close_btn = std::make_unique<engine::ui::elements::Button>(
            CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE,
            "X",
            UITheme::FONT_SIZE_MEDIUM
        );
        close_btn->SetRelativePosition(
            static_cast<float>(menu_width - CLOSE_BUTTON_SIZE - 10),
            static_cast<float>(MENU_HEIGHT - CLOSE_BUTTON_SIZE - 5)
        );
        close_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.6f)));
        close_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.8f)));
        close_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_DEFAULT));
        close_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        close_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                if (close_callback_) {
                    close_callback_();
                }
                selected_facility_ = -1;
                visible_ = false;
                return true;
            }
            return false;
        });
        close_button_ = close_btn.get();
        main_panel_->AddChild(std::move(close_btn));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    std::unique_ptr<engine::ui::elements::Container> BuildMenu::CreateCategoryContent(
        FacilityCategory category, int menu_width) {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const float content_width = static_cast<float>(menu_width);
        const float content_height = static_cast<float>(MENU_HEIGHT - TAB_HEIGHT - CLOSE_BUTTON_SIZE - 10);

        auto content = engine::ui::ContainerBuilder()
                .Size(content_width, content_height)
                .Fill()
                .Layout(std::make_unique<GridLayout>(FACILITIES_PER_ROW, GRID_PADDING, GRID_PADDING))
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Padding(GRID_PADDING)
                .Build();

        // Add facility buttons for this category
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            if (facility_types_[i].category == category) {
                const FacilityType &facility = facility_types_[i];

                std::stringstream label;
                label << facility.name << "\n$" << facility.cost;

                auto button = std::make_unique<engine::ui::elements::Button>(
                    FACILITY_BUTTON_SIZE, FACILITY_BUTTON_SIZE,
                    label.str(),
                    UITheme::FONT_SIZE_SMALL
                );

                button->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(facility.color, 0.6f)));
                button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(facility.color, 0.8f)));
                button->SetPressedColor(UITheme::ToEngineColor(facility.color));
                button->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_DEFAULT));
                button->SetTextColor(UITheme::ToEngineColor(WHITE));

                const int facility_index = static_cast<int>(i);
                button->SetClickCallback([this, facility_index](const engine::ui::MouseEvent &event) {
                    if (event.left_pressed) {
                        audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                        selected_facility_ = facility_index;
                        if (facility_selected_callback_) {
                            facility_selected_callback_(facility_index);
                        }
                        return true;
                    }
                    return false;
                });

                content->AddChild(std::move(button));
            }
        }

        content->InvalidateComponentsRecursive();
        content->UpdateComponentsRecursive();

        return content;
    }

    void BuildMenu::UpdateLayout() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int menu_width = ActionBar::CalculateBarWidth();
        const float x = static_cast<float>((screen_width - menu_width) / 2);
        const float y = static_cast<float>(screen_height - MENU_HEIGHT - 60);

        main_panel_->SetRelativePosition(x, y);
        main_panel_->SetSize(static_cast<float>(menu_width), static_cast<float>(MENU_HEIGHT));

        // Update close button position
        if (close_button_) {
            close_button_->SetRelativePosition(
                static_cast<float>(menu_width - CLOSE_BUTTON_SIZE - 10),
                static_cast<float>(MENU_HEIGHT - CLOSE_BUTTON_SIZE - 5)
            );
        }

        main_panel_->InvalidateComponentsRecursive();
        main_panel_->UpdateComponentsRecursive();

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void BuildMenu::Update(const float delta_time) {
        if (!visible_ || !main_panel_) return;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();

        // Check for window resize
        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        main_panel_->UpdateComponentsRecursive(delta_time);
    }

    bool BuildMenu::ProcessMouseEvent(const MouseEvent &event) const {
        if (!visible_ || !main_panel_) return false;

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

    void BuildMenu::Render(bool can_undo, bool can_redo, bool demolish_mode) const {
        if (!visible_ || !main_panel_) return;

        main_panel_->Render();
        main_panel_->RenderComponentsRecursive();
    }

    std::vector<int> BuildMenu::GetFacilitiesForCategory(const FacilityCategory category) const {
        std::vector<int> indices;
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            if (facility_types_[i].category == category) {
                indices.push_back(static_cast<int>(i));
            }
        }
        return indices;
    }

    std::string BuildMenu::GetCategoryName(const FacilityCategory category) const {
        switch (category) {
            case FacilityCategory::Core: return "Core";
            case FacilityCategory::Commercial: return "Commercial";
            case FacilityCategory::Residential: return "Residential";
            case FacilityCategory::Entertainment: return "Entertainment";
            case FacilityCategory::Professional: return "Professional";
            default: return "Unknown";
        }
    }

    void BuildMenu::UpdateTooltips(int mouse_x, int mouse_y, float current_funds) const {
        // Tooltips now handled by button hover events
    }

    void BuildMenu::RenderTooltips() const {
        if (tooltip_manager_) {
            tooltip_manager_->Render();
        }
    }
}
