#include "core/scenes/credits_scene.hpp"
#include "core/game.h"
#include "ui/ui_theme.h"
#include <raylib.h>

import engine;

namespace towerforge::core {
    CreditsScene::CreditsScene(Game *game)
        : GameScene(game) {
    }

    void CreditsScene::Initialize() {
        BuildUI();
    }

    void CreditsScene::Shutdown() {
        main_panel_.reset();
    }

    void CreditsScene::BuildUI() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);
        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;

        constexpr int PANEL_WIDTH = 600;
        constexpr int PANEL_HEIGHT = 400;
        const float panel_x = static_cast<float>((screen_width - PANEL_WIDTH) / 2);
        const float panel_y = static_cast<float>((screen_height - PANEL_HEIGHT) / 2);

        // Create main panel
        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(panel_x, panel_y);
        main_panel_->SetSize(static_cast<float>(PANEL_WIDTH), static_cast<float>(PANEL_HEIGHT));
        main_panel_->SetBackgroundColor(ui::UITheme::ToEngineColor(ColorAlpha(ui::UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(ui::UITheme::ToEngineColor(ui::UITheme::PRIMARY));
        main_panel_->SetPadding(ui::UITheme::PADDING_LARGE);

        // Create content container with vertical layout
        auto content = engine::ui::ContainerBuilder()
                .Size(PANEL_WIDTH - ui::UITheme::PADDING_LARGE * 2, PANEL_HEIGHT - ui::UITheme::PADDING_LARGE * 2)
                .Opacity(0)
                .Layout(std::make_unique<VerticalLayout>(ui::UITheme::MARGIN_MEDIUM, Alignment::Center))
                .Build();

        // Title
        auto title = std::make_unique<Text>(
            0.0f, 0.0f,
            "CREDITS",
            ui::UITheme::FONT_SIZE_TITLE,
            ui::UITheme::ToEngineColor(GOLD)
        );
        content->AddChild(std::move(title));

        // Version
        auto version = std::make_unique<Text>(
            0.0f, 0.0f,
            "TowerForge v0.1.0",
            ui::UITheme::FONT_SIZE_LARGE,
            ui::UITheme::ToEngineColor(WHITE)
        );
        content->AddChild(std::move(version));

        // Description
        auto description = std::make_unique<Text>(
            0.0f, 0.0f,
            "A modern SimTower-inspired skyscraper simulation",
            ui::UITheme::FONT_SIZE_NORMAL,
            ui::UITheme::ToEngineColor(LIGHTGRAY)
        );
        content->AddChild(std::move(description));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(ui::UITheme::ToEngineColor(ui::UITheme::PRIMARY));
        divider->SetSize(static_cast<float>(PANEL_WIDTH - ui::UITheme::PADDING_LARGE * 4), 2.0f);
        content->AddChild(std::move(divider));

        // Built with header
        auto built_with = std::make_unique<Text>(
            0.0f, 0.0f,
            "Built with:",
            ui::UITheme::FONT_SIZE_NORMAL,
            ui::UITheme::ToEngineColor(LIGHTGRAY)
        );
        content->AddChild(std::move(built_with));

        // Technologies
        const char *technologies[] = {"C++20", "Raylib (rendering)", "Flecs (ECS framework)"};
        for (const auto *tech: technologies) {
            auto tech_text = std::make_unique<Text>(
                0.0f, 0.0f,
                std::string("- ") + tech,
                ui::UITheme::FONT_SIZE_NORMAL,
                ui::UITheme::ToEngineColor(WHITE)
            );
            content->AddChild(std::move(tech_text));
        }

        // Spacer
        auto spacer = std::make_unique<Panel>();
        spacer->SetSize(1.0f, static_cast<float>(ui::UITheme::MARGIN_LARGE));
        spacer->SetBackgroundColor({0, 0, 0, 0});
        content->AddChild(std::move(spacer));

        // Return instruction
        auto return_text = std::make_unique<Text>(
            0.0f, 0.0f,
            "Press ESC or ENTER to return to menu",
            ui::UITheme::FONT_SIZE_SMALL,
            ui::UITheme::ToEngineColor(GRAY)
        );
        content->AddChild(std::move(return_text));

        main_panel_->AddChild(std::move(content));

        main_panel_->InvalidateComponents();
        main_panel_->UpdateComponentsRecursive();
    }

    void CreditsScene::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            BuildUI();
        }
    }

    void CreditsScene::Update(const float delta_time) {
        UpdateLayout();
    }

    void CreditsScene::Render() {
        ClearBackground(Color{20, 20, 30, 255});

        engine::ui::BatchRenderer::BeginFrame();
        if (main_panel_) {
            main_panel_->Render();
        }
        engine::ui::BatchRenderer::EndFrame();
    }

    void CreditsScene::HandleMouseEvent(const engine::ui::MouseEvent &event) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            game_->SetGameState(GameState::TitleScreen);
        }
    }
}
