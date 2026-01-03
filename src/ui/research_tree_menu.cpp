#include "ui/research_tree_menu.h"
#include "ui/notification_center.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <string>
#include <sstream>
#include <map>

import engine;

namespace towerforge::ui {
    ResearchTreeMenu::ResearchTreeMenu()
        : visible_(false)
          , animation_time_(0.0f)
          , selected_node_index_(-1)
          , notification_center_(nullptr)
          , tooltip_manager_(nullptr)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , tab_container_(nullptr)
          , close_button_(nullptr)
          , pending_unlock_tree_(nullptr) {
        // Dialog will be created on-demand when unlock is requested
    }

    ResearchTreeMenu::~ResearchTreeMenu() = default;

    void ResearchTreeMenu::Initialize(const core::ResearchTree &research_tree) {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;
 std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Calculate centered position
        const float panel_x = static_cast<float>((screen_width - MENU_WIDTH) / 2);
        const float panel_y = static_cast<float>((screen_height - MENU_HEIGHT) / 2);

        // Create main panel with vertical layout
        main_panel_ = std::make_unique<engine::ui::elements::Panel>();
        main_panel_->SetRelativePosition(panel_x, panel_y);
        main_panel_->SetSize(static_cast<float>(MENU_WIDTH), static_cast<float>(MENU_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        main_panel_->SetPadding(0);

        // Create header container
        auto header = engine::ui::ContainerBuilder()
                .Size(MENU_WIDTH, HEADER_HEIGHT)
                .Opacity(0)
                .Layout(std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center))
                .Padding(UITheme::PADDING_MEDIUM)
                .Build();

        // Title
        auto title_text = std::make_unique<Text>(
            0, 0,
            "RESEARCH & UPGRADES",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        header->AddChild(std::move(title_text));

        // Stats row - Tower points, total earned, generation rate
        std::stringstream stats_ss;
        stats_ss << "Tower Points: " << research_tree.tower_points
                << "  |  Total Earned: " << research_tree.total_points_earned
                << "  |  " << static_cast<int>(research_tree.tower_points_per_hour) << " pts/hr";
        auto stats_text = std::make_unique<Text>(
            0, 0,
            stats_ss.str(),
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        header->AddChild(std::move(stats_text));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(MENU_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        header->AddChild(std::move(divider));

        main_panel_->AddChild(std::move(header));

        // Create tab container for research categories (narrower to make room for details panel)
        constexpr float tab_content_height = static_cast<float>(MENU_HEIGHT - HEADER_HEIGHT - CLOSE_BUTTON_SIZE - 20);
        tab_container_ = new TabContainer(
            static_cast<float>(GRID_AREA_WIDTH),
            tab_content_height
        );
        tab_container_->SetRelativePosition(0, HEADER_HEIGHT);
        tab_container_->SetTabBarHeight(TAB_HEIGHT);
        tab_container_->SetTabBarColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_DARK, 0.9f)));
        tab_container_->SetActiveTabColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        tab_container_->SetInactiveTabColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.6f)));
        tab_container_->SetTabTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
        tab_container_->SetContentBackgroundColor(UITheme::ToEngineColor(UITheme::BACKGROUND_PANEL));

        // Group nodes by type for tabs
        std::map<core::ResearchNodeType, std::vector<const core::ResearchNode *> > nodes_by_type;
        for (const auto &node: research_tree.nodes) {
            nodes_by_type[node.type].push_back(&node);
        }

        // Create tabs for each research type
        const std::vector<std::pair<core::ResearchNodeType, std::string> > categories = {
            {core::ResearchNodeType::ElevatorSpeed, "Elevator"},
            {core::ResearchNodeType::IncomeBonus, "Income"},
            {core::ResearchNodeType::SatisfactionBonus, "Satisfaction"},
            {core::ResearchNodeType::ConstructionSpeed, "Construction"},
            {core::ResearchNodeType::FacilityUnlock, "Facilities"},
            {core::ResearchNodeType::CostReduction, "Costs"}
        };

        for (const auto &[type, name]: categories) {
            auto it = nodes_by_type.find(type);
            std::vector<const core::ResearchNode *> type_nodes;
            if (it != nodes_by_type.end()) {
                type_nodes = it->second;
            }
            auto content = CreateCategoryContent(type, type_nodes, GRID_AREA_WIDTH);
            tab_container_->AddTab(name, std::move(content));
        }

        // Set tab changed callback
        tab_container_->SetTabChangedCallback([this](size_t, const std::string &) {
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
            // Clear selection when switching tabs
            if (details_panel_) {
                details_panel_->Clear();
            }
        });

        main_panel_->AddChild(std::unique_ptr<TabContainer>(tab_container_));

        // Create details panel on the right side
        constexpr float details_height = static_cast<float>(MENU_HEIGHT - HEADER_HEIGHT - 20);
        details_panel_ = std::make_unique<ResearchDetailsPanel>(
            static_cast<float>(DETAILS_PANEL_WIDTH - 10),
            details_height - 10
        );
        details_panel_->Initialize();
        details_panel_->SetUnlockCallback([this](const std::string &node_id) {
            OnUnlockRequested(node_id);
        });

        // Position the panel
        if (auto *panel = details_panel_->GetPanel()) {
            panel->SetRelativePosition(
                static_cast<float>(GRID_AREA_WIDTH + 5),
                static_cast<float>(HEADER_HEIGHT + 5)
            );
        }
        main_panel_->AddChild(details_panel_->TakePanel());

        // Create close button
        auto close_btn = std::make_unique<engine::ui::elements::Button>(
            CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE,
            "X",
            UITheme::FONT_SIZE_MEDIUM
        );
        close_btn->SetRelativePosition(
            static_cast<float>(MENU_WIDTH - CLOSE_BUTTON_SIZE - 10),
            static_cast<float>(MENU_HEIGHT - CLOSE_BUTTON_SIZE - 10)
        );
        close_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.6f)));
        close_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(RED, 0.8f)));
        close_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_DEFAULT));
        close_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        close_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
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

    std::unique_ptr<engine::ui::elements::Container> ResearchTreeMenu::CreateCategoryContent(
        core::ResearchNodeType type,
        const std::vector<const core::ResearchNode *> &nodes,
        const int content_width) {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        const float width = static_cast<float>(content_width);
        constexpr float height = static_cast<float>(MENU_HEIGHT - HEADER_HEIGHT - TAB_HEIGHT - CLOSE_BUTTON_SIZE - 30);

        auto content = engine::ui::ContainerBuilder()
                .Size(width, height)
                .Fill()
                .Layout(std::make_unique<GridLayout>(NODES_PER_ROW, GRID_PADDING, GRID_PADDING))
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Padding(GRID_PADDING)
                .Build();

        // Add node buttons for this category
        for (const auto *node: nodes) {
            // Skip hidden nodes
            if (node->state == core::ResearchNodeState::Hidden) {
                continue;
            }

            std::stringstream label;
            label << node->GetDisplayIcon() << "\n" << node->name.substr(0, 10);
            if (node->name.length() > 10) label << "...";
            label << "\n" << node->cost << " pts";

            auto button = std::make_unique<engine::ui::elements::Button>(
                NODE_BUTTON_SIZE, NODE_BUTTON_SIZE,
                label.str(),
                UITheme::FONT_SIZE_SMALL
            );

            const Color bg_color = GetNodeStateColor(node->state);
            const Color border_color = GetNodeBorderColor(node->state);

            button->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(bg_color, 0.7f)));
            button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(bg_color, 0.9f)));
            button->SetPressedColor(UITheme::ToEngineColor(bg_color));
            button->SetBorderColor(UITheme::ToEngineColor(border_color));
            button->SetTextColor(UITheme::ToEngineColor(WHITE));

            const std::string node_id = node->id;
            button->SetClickCallback([this, node_id](const engine::ui::MouseEvent &event) {
                if (event.left_pressed) {
                    // Select the node and update the details panel
                    if (pending_unlock_tree_ && details_panel_) {
                        if (const auto *found_node = pending_unlock_tree_->FindNode(node_id)) {
                            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
                            details_panel_->SetNode(found_node);
                        }
                    }
                    return true;
                }
                return false;
            });

            // Add hover callback for details panel and tooltips
            button->SetHoverCallback([this, node_id, btn_ptr = button.get()](const engine::ui::MouseEvent &) {
                // Update the details panel
                if (pending_unlock_tree_ && details_panel_) {
                    if (const auto *found_node = pending_unlock_tree_->FindNode(node_id)) {
                        details_panel_->SetNode(found_node);

                        // Also show tooltip if manager is available
                        if (tooltip_manager_) {
                            const std::string tooltip_text = BuildNodeTooltip(*found_node);
                            const auto bounds = btn_ptr->GetAbsoluteBounds();
                            tooltip_manager_->ShowTooltip(
                                Tooltip(tooltip_text),
                                static_cast<int>(bounds.x),
                                static_cast<int>(bounds.y),
                                static_cast<int>(bounds.width),
                                static_cast<int>(bounds.height)
                            );
                        }
                    }
                }
                return true;
            });

            node_buttons_.push_back(button.get());
            content->AddChild(std::move(button));
        }

        content->InvalidateComponentsRecursive();
        content->UpdateComponentsRecursive();

        return content;
    }

    void ResearchTreeMenu::OnUnlockRequested(const std::string &node_id) {
        if (!pending_unlock_tree_) return;

        if (const auto *found_node = pending_unlock_tree_->FindNode(node_id);
            found_node && found_node->state == core::ResearchNodeState::Upgradable) {
            pending_unlock_node_id_ = node_id;

            std::string message = "Unlock '" + found_node->name + "' for " +
                                  std::to_string(found_node->cost) + " tower points?";

            unlock_confirmation_ = std::make_unique<EngineConfirmationDialog>(
                "Confirm Research Unlock",
                message,
                "Unlock",
                "Cancel"
            );
            unlock_confirmation_->Initialize();
            unlock_confirmation_->SetConfirmCallback([this]() {
                if (pending_unlock_tree_ && !pending_unlock_node_id_.empty()) {
                    if (const bool unlocked = pending_unlock_tree_->UnlockNode(pending_unlock_node_id_);
                        unlocked && notification_center_) {
                        notification_center_->AddNotification(
                            "Research Unlocked",
                            "Successfully unlocked: " + pending_unlock_node_id_,
                            NotificationType::Success,
                            NotificationPriority::Medium,
                            5.0f
                        );
                        // Update the details panel to reflect new state
                        if (details_panel_) {
                            if (const auto *updated_node = pending_unlock_tree_->FindNode(pending_unlock_node_id_)) {
                                details_panel_->SetNode(updated_node);
                            }
                        }
                    }
                    pending_unlock_node_id_.clear();
                }
            });
            unlock_confirmation_->Show();
        }
    }

    void ResearchTreeMenu::Shutdown() {
        node_buttons_.clear();
        tab_container_ = nullptr;
        close_button_ = nullptr;
        details_panel_.reset();
        unlock_confirmation_.reset();
        main_panel_.reset();
    }

    void ResearchTreeMenu::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (main_panel_) {
            const float panel_x = static_cast<float>((screen_width - MENU_WIDTH) / 2);
            const float panel_y = static_cast<float>((screen_height - MENU_HEIGHT) / 2);
            main_panel_->SetRelativePosition(panel_x, panel_y);
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void ResearchTreeMenu::Update(const float delta_time) {
        if (!visible_) return;

        animation_time_ += delta_time;

        // Check for window resize
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }

        // Update confirmation dialog
        if (unlock_confirmation_) {
            unlock_confirmation_->Update(delta_time);
        }

        if (main_panel_) {
            main_panel_->UpdateComponentsRecursive(delta_time);
        }
    }

    void ResearchTreeMenu::Render(const core::ResearchTree &research_tree) {
        if (!visible_ || !main_panel_) return;

        // Store reference for click handlers
        pending_unlock_tree_ = const_cast<core::ResearchTree *>(&research_tree);

        RenderDimOverlay();
        main_panel_->Render();
        main_panel_->RenderComponentsRecursive();

        // Render confirmation dialog if visible
        if (unlock_confirmation_ && unlock_confirmation_->IsVisible()) {
            unlock_confirmation_->Render();
        }
    }

    void ResearchTreeMenu::RenderDimOverlay() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    bool ResearchTreeMenu::ProcessMouseEvent(const engine::ui::MouseEvent &event) const {
        if (!visible_) return false;

        // If confirmation dialog is visible, route events to it first
        if (unlock_confirmation_ && unlock_confirmation_->IsVisible()) {
            return unlock_confirmation_->ProcessMouseEvent(event);
        }

        if (main_panel_) {
            return main_panel_->ProcessMouseEvent(event);
        }

        return false;
    }

    void ResearchTreeMenu::HandleKeyboard() {
        // ESC or R to close
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_R)) {
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
            visible_ = false;
        }

        // F1 for help
        if (IsKeyPressed(KEY_F1)) {
            // Help system integration would go here
        }
    }

    void ResearchTreeMenu::RebuildUI(const core::ResearchTree &research_tree) {
        Shutdown();
        Initialize(research_tree);
    }

    Color ResearchTreeMenu::GetNodeStateColor(const core::ResearchNodeState state) {
        switch (state) {
            case core::ResearchNodeState::Hidden:
                return Color{20, 20, 25, 255};
            case core::ResearchNodeState::Locked:
                return Color{60, 60, 70, 255};
            case core::ResearchNodeState::Upgradable:
                return Color{100, 80, 30, 255};
            case core::ResearchNodeState::Unlocked:
                return Color{30, 80, 50, 255};
            default:
                return DARKGRAY;
        }
    }

    Color ResearchTreeMenu::GetNodeBorderColor(const core::ResearchNodeState state) {
        switch (state) {
            case core::ResearchNodeState::Hidden:
                return Color{40, 40, 45, 255};
            case core::ResearchNodeState::Locked:
                return DARKGRAY;
            case core::ResearchNodeState::Upgradable:
                return GOLD;
            case core::ResearchNodeState::Unlocked:
                return LIME;
            default:
                return GRAY;
        }
    }

    std::string ResearchTreeMenu::BuildNodeTooltip(const core::ResearchNode &node) {
        std::stringstream ss;
        ss << node.name << "\n";
        ss << "Status: " << node.GetStateString() << "\n";
        ss << "Cost: " << node.cost << " tower points\n";

        if (!node.description.empty()) {
            ss << "\n" << node.description << "\n";
        }

        if (!node.prerequisites.empty()) {
            ss << "\nRequires: ";
            for (size_t i = 0; i < node.prerequisites.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << node.prerequisites[i];
            }
            ss << "\n";
        }

        // Effect description
        if (node.effect_value != 0.0f || !node.effect_target.empty()) {
            ss << "\nEffect: ";
            switch (node.type) {
                case core::ResearchNodeType::ElevatorSpeed:
                    ss << "+" << static_cast<int>(node.effect_value * 100) << "% elevator speed";
                    break;
                case core::ResearchNodeType::ElevatorCapacity:
                    ss << "+" << static_cast<int>(node.effect_value) << " capacity";
                    break;
                case core::ResearchNodeType::IncomeBonus:
                    ss << "+" << static_cast<int>(node.effect_value * 100) << "% income";
                    break;
                case core::ResearchNodeType::SatisfactionBonus:
                    ss << "+" << static_cast<int>(node.effect_value) << " satisfaction";
                    break;
                case core::ResearchNodeType::ConstructionSpeed:
                    ss << "+" << static_cast<int>(node.effect_value * 100) << "% build speed";
                    break;
                case core::ResearchNodeType::CostReduction:
                    ss << "-" << static_cast<int>(node.effect_value * 100) << "% costs";
                    break;
                case core::ResearchNodeType::FacilityUnlock:
                    ss << "Unlocks " << node.effect_target;
                    break;
                default: ;
            }
        }

        if (node.state == core::ResearchNodeState::Upgradable) {
            ss << "\n\nClick to unlock!";
        }

        return ss.str();
    }
}
