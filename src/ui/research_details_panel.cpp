#include "ui/research_details_panel.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <string>

import engine;

namespace towerforge::ui {
    ResearchDetailsPanel::ResearchDetailsPanel(const float width, const float height)
        : width_(width)
          , height_(height)
          , name_text_(nullptr)
          , status_text_(nullptr)
          , cost_text_(nullptr)
          , description_text_(nullptr)
          , effect_text_(nullptr)
          , hint_text_(nullptr)
          , unlock_button_(nullptr) {
    }

    ResearchDetailsPanel::~ResearchDetailsPanel() = default;

    void ResearchDetailsPanel::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Create the main panel
        panel_ = std::make_unique<Panel>();
        panel_->SetSize(width_, height_);
        panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_DARK, 0.9f)));
        panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        panel_->SetPadding(static_cast<float>(UITheme::PADDING_SMALL));

        // Content container with vertical layout (for text elements)
        auto content = engine::ui::ContainerBuilder()
                .Size(width_ - UITheme::PADDING_SMALL * 2, height_ - 60) // Leave room for button at bottom
                .Opacity(0)
                .Layout(std::make_unique<VerticalLayout>(20.0f, Alignment::Start))
                .Padding(0)
                .Build();

        // Title: "Node Details"
        auto title = std::make_unique<Text>(
            0, 0,
            "Node Details",
            UITheme::FONT_SIZE_MEDIUM,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        content->AddChild(std::move(title));

        // Divider
        auto div = std::make_unique<Divider>();
        div->SetColor(UITheme::ToEngineColor(UITheme::BORDER_SUBTLE));
        div->SetSize(width_ - 30, 1);
        content->AddChild(std::move(div));

        // Node name
        auto name = std::make_unique<Text>(
            0, 0,
            "Select a node",
            UITheme::FONT_SIZE_NORMAL,
            UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
        );
        name_text_ = name.get();
        content->AddChild(std::move(name));

        // Status
        auto status = std::make_unique<Text>(
            0, 0,
            "",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        status_text_ = status.get();
        content->AddChild(std::move(status));

        // Cost
        auto cost = std::make_unique<Text>(
            0, 0,
            "",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        cost_text_ = cost.get();
        content->AddChild(std::move(cost));

        // Description
        auto desc = std::make_unique<Text>(
            0, 0,
            "",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        description_text_ = desc.get();
        content->AddChild(std::move(desc));

        // Effect
        auto effect = std::make_unique<Text>(
            0, 0,
            "",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::SUCCESS)
        );
        effect_text_ = effect.get();
        content->AddChild(std::move(effect));

        // Hint
        auto hint = std::make_unique<Text>(
            0, 0,
            "",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::WARNING)
        );
        hint_text_ = hint.get();
        content->AddChild(std::move(hint));

        content->InvalidateComponentsRecursive();
        content->UpdateComponentsRecursive();
        panel_->AddChild(std::move(content));

        // Unlock button - positioned at the bottom of the panel
        constexpr float button_width = 180.0f;
        constexpr float button_height = 35.0f;
        auto unlock_btn = std::make_unique<Button>(
            button_width, button_height,
            "Unlock Research",
            UITheme::FONT_SIZE_NORMAL
        );
        // Position at bottom center of panel
        unlock_btn->SetRelativePosition(
            (width_ - button_width) / 2.0f - UITheme::PADDING_SMALL,
            height_ - button_height - UITheme::PADDING_MEDIUM
        );
        unlock_btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.7f)));
        unlock_btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.9f)));
        unlock_btn->SetPressedColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        unlock_btn->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        unlock_btn->SetTextColor(UITheme::ToEngineColor(WHITE));
        unlock_btn->SetVisible(false);

        unlock_btn->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed && !selected_node_id_.empty() && unlock_callback_) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuConfirm);
                unlock_callback_(selected_node_id_);
                return true;
            }
            return false;
        });

        unlock_button_ = unlock_btn.get();
        panel_->AddChild(std::move(unlock_btn));
    }

    void ResearchDetailsPanel::Clear() {
        selected_node_id_.clear();
        if (name_text_) name_text_->SetText("Select a node");
        if (status_text_) status_text_->SetText("");
        if (cost_text_) cost_text_->SetText("");
        if (description_text_) description_text_->SetText("");
        if (effect_text_) effect_text_->SetText("");
        if (hint_text_) hint_text_->SetText("");
        if (unlock_button_) unlock_button_->SetVisible(false);
    }

    void ResearchDetailsPanel::SetNode(const core::ResearchNode *node) {
        if (!node) {
            Clear();
            return;
        }

        selected_node_id_ = node->id;

        // Name
        if (name_text_) {
            name_text_->SetText(node->name);
        }

        // Status with color
        if (status_text_) {
            std::string status_str = std::string("Status: ") + node->GetStateString();
            status_text_->SetText(status_str);
            switch (node->state) {
                case core::ResearchNodeState::Unlocked:
                    status_text_->SetColor(UITheme::ToEngineColor(UITheme::SUCCESS));
                    break;
                case core::ResearchNodeState::Upgradable:
                    status_text_->SetColor(UITheme::ToEngineColor(UITheme::WARNING));
                    break;
                default:
                    status_text_->SetColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
                    break;
            }
        }

        // Cost
        if (cost_text_) {
            cost_text_->SetText("Cost: " + std::to_string(node->cost) + " pts");
        }

        // Description (truncated if too long)
        if (description_text_) {
            if (!node->description.empty()) {
                std::string desc = node->description;
                if (desc.length() > 80) {
                    desc = desc.substr(0, 77) + "...";
                }
                description_text_->SetText(desc);
            } else {
                description_text_->SetText("");
            }
        }

        // Effect
        if (effect_text_) {
            std::string effect_str;
            if (node->effect_value != 0.0f || !node->effect_target.empty()) {
                switch (node->type) {
                    case core::ResearchNodeType::ElevatorSpeed:
                        effect_str = "+" + std::to_string(static_cast<int>(node->effect_value * 100)) + "% speed";
                        break;
                    case core::ResearchNodeType::ElevatorCapacity:
                        effect_str = "+" + std::to_string(static_cast<int>(node->effect_value)) + " capacity";
                        break;
                    case core::ResearchNodeType::IncomeBonus:
                        effect_str = "+" + std::to_string(static_cast<int>(node->effect_value * 100)) + "% income";
                        break;
                    case core::ResearchNodeType::SatisfactionBonus:
                        effect_str = "+" + std::to_string(static_cast<int>(node->effect_value)) + " satisfaction";
                        break;
                    case core::ResearchNodeType::ConstructionSpeed:
                        effect_str = "+" + std::to_string(static_cast<int>(node->effect_value * 100)) + "% build speed";
                        break;
                    case core::ResearchNodeType::CostReduction:
                        effect_str = "-" + std::to_string(static_cast<int>(node->effect_value * 100)) + "% costs";
                        break;
                    case core::ResearchNodeType::FacilityUnlock:
                        effect_str = "Unlocks: " + node->effect_target;
                        break;
                    case core::ResearchNodeType::VerticalExpansion:
                        break;
                }
            }
            effect_text_->SetText(effect_str);
        }

        // Show/hide unlock button and hint based on state
        if (node->state == core::ResearchNodeState::Upgradable) {
            if (hint_text_) hint_text_->SetText("");
            if (unlock_button_) unlock_button_->SetVisible(true);
        } else {
            if (hint_text_) {
                if (node->state == core::ResearchNodeState::Locked) {
                    hint_text_->SetText("Prerequisites not met");
                } else if (node->state == core::ResearchNodeState::Unlocked) {
                    hint_text_->SetText("Already unlocked");
                } else {
                    hint_text_->SetText("");
                }
            }
            if (unlock_button_) unlock_button_->SetVisible(false);
        }
    }
}
