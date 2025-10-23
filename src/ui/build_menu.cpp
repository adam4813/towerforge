#include "ui/build_menu.h"
#include "ui/tooltip.h"
#include "ui/ui_element.h"
#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include "ui/tab_bar.h"
#include "ui/grid_panel.h"
#include "ui/help_system.h"
#include <sstream>
#include <algorithm>

namespace towerforge::ui {
    BuildMenu::BuildMenu()
        : selected_facility_(-1)
        , visible_(false)
        , tutorial_mode_(false)
        , tooltip_manager_(nullptr)
        , facility_selected_callback_(nullptr)
        , close_callback_(nullptr)
        , current_category_(FacilityCategory::Core) {
        
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
        facility_types_.emplace_back("Flagship", "F", 18000, 12, Color{0, 206, 209, 255}, FacilityCategory::Professional);

        BuildUI();
    }

    BuildMenu::~BuildMenu() = default;

    void BuildMenu::BuildUI() {
        // Calculate position - centered above action bar
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float x = (screen_width - MENU_WIDTH) / 2.0f;
        const float y = screen_height - MENU_HEIGHT - 60.0f; // 60px from bottom (above action bar)

        // Create root panel
        root_panel_ = std::make_unique<Panel>(
            x, y, MENU_WIDTH, MENU_HEIGHT,
            ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f),
            UITheme::BORDER_ACCENT
        );
        root_panel_->SetPadding(0);

        // Create category tab bar
        const std::vector<std::string> tab_labels = {"Core", "Commercial", "Residential", "Entertainment", "Professional"};
        auto bar = std::make_unique<TabBar>(0, 0, MENU_WIDTH, TAB_HEIGHT, tab_labels);
        bar->SetTabSelectedCallback([this](int tab_index) {
            SwitchCategory(static_cast<FacilityCategory>(tab_index));
        });
        tab_bar_ = bar.get();
        root_panel_->AddChild(std::move(bar));

        // Create facility grid panel (scrollable)
        auto grid = std::make_unique<GridPanel>(
            0, TAB_HEIGHT,
            MENU_WIDTH, MENU_HEIGHT - TAB_HEIGHT - CLOSE_BUTTON_SIZE - 10,
            FACILITIES_PER_ROW, FACILITY_BUTTON_SIZE, GRID_PADDING
        );
        grid->SetItemSelectedCallback([this](const int data_index) {
            selected_facility_ = data_index;
            if (facility_selected_callback_) {
                facility_selected_callback_(data_index);
            }
        });
        facility_grid_ = grid.get();
        root_panel_->AddChild(std::move(grid));

        // Create close button
        close_button_ = std::make_unique<Button>(
            MENU_WIDTH - CLOSE_BUTTON_SIZE - 10,
            MENU_HEIGHT - CLOSE_BUTTON_SIZE - 5,
            CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE,
            "X",
            ColorAlpha(RED, 0.6f),
            UITheme::BORDER_DEFAULT
        );
        close_button_->SetClickCallback([this]() {
            if (close_callback_) {
                close_callback_();
            }
            selected_facility_ = -1;
            visible_ = false;
        });
        close_button_->SetFontSize(16);
        root_panel_->AddChild(std::move(close_button_));

        // Populate initial category
        SwitchCategory(current_category_);
    }

    void BuildMenu::SwitchCategory(FacilityCategory category) {
        current_category_ = category;
        
        if (!tab_bar_ || !facility_grid_) return;
        
        // Update active tab
        tab_bar_->SetActiveTab(static_cast<int>(category));
        
        // Clear and repopulate grid
        facility_grid_->ClearItems();
        
        for (size_t i = 0; i < facility_types_.size(); ++i) {
            if (facility_types_[i].category == category) {
                const FacilityType& facility = facility_types_[i];
                
                // Create button label with name and cost
                std::stringstream label;
                label << facility.name << "\n$" << facility.cost;
                
                facility_grid_->AddItem(label.str(), static_cast<int>(i), facility.color);
            }
        }
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

    void BuildMenu::Update(const float delta_time) const {
        if (!visible_ || !root_panel_) return;
        
        // Update position to stay above action bar (centered)
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float x = (screen_width - MENU_WIDTH) / 2.0f;
        const float y = screen_height - MENU_HEIGHT - 60.0f; // 60px from bottom (above action bar)
        root_panel_->SetRelativePosition(x, y);
        
        root_panel_->Update(delta_time);
        
        if (tab_bar_) {
            tab_bar_->Update(delta_time);
        }
        
        if (facility_grid_) {
            facility_grid_->Update(delta_time);
        }
    }

    bool BuildMenu::ProcessMouseEvent(const MouseEvent& event) const {
        if (!visible_ || !root_panel_) return false;
        
        return root_panel_->ProcessMouseEvent(event);
    }

    void BuildMenu::Render(bool can_undo, bool can_redo, bool demolish_mode) const {
        if (!visible_ || !root_panel_) return;
        
        root_panel_->Render();
    }

    int BuildMenu::HandleClick(int mouse_x, int mouse_y, bool can_undo, bool can_redo) {
        // Deprecated - use ProcessMouseEvent instead
        return -1;
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
