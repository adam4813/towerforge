#include "ui/help_system.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <cmath>
#include <algorithm>

import engine;

namespace towerforge::ui {
    HelpSystem::HelpSystem()
        : visible_(false)
          , current_context_(HelpContext::MainGame)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , title_text_(nullptr)
          , content_container_(nullptr) {
    }

    HelpSystem::~HelpSystem() = default;

    void HelpSystem::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Initialize help content
        InitializeMainGameHelp();
        InitializeBuildMenuHelp();
        InitializeResearchTreeHelp();
        InitializeModsMenuHelp();
        InitializeStaffManagementHelp();
        InitializeSettingsHelp();
        InitializeTutorialHelp();
        InitializePauseMenuHelp();
        InitializeHistoryHelp();
        InitializeNotificationsHelp();

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Create main panel
        const int panel_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int panel_y = (screen_height - OVERLAY_HEIGHT) / 2;

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(OVERLAY_WIDTH), static_cast<float>(OVERLAY_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::INFO));
        main_panel_->SetPadding(static_cast<float>(UITheme::PADDING_LARGE));
        main_panel_->AddComponent<LayoutComponent>(
            std::make_unique<VerticalLayout>(UITheme::MARGIN_SMALL, Alignment::Center)
        );

        // Add title
        auto title = std::make_unique<Text>(
            0, 0,
            "Help",
            UITheme::FONT_SIZE_TITLE,
            UITheme::ToEngineColor(UITheme::INFO)
        );
        title_text_ = title.get();
        main_panel_->AddChild(std::move(title));

        // Add divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::INFO));
        divider->SetSize(OVERLAY_WIDTH - UITheme::PADDING_LARGE * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Add instructions
        auto instructions = std::make_unique<Text>(
            0, 0,
            "Press F1 or ESC to close",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        main_panel_->AddChild(std::move(instructions));

        // Create scrollable content container
        constexpr float content_width = OVERLAY_WIDTH - UITheme::PADDING_LARGE * 2;
        constexpr float content_height = OVERLAY_HEIGHT - HEADER_HEIGHT - 100;

        auto content = engine::ui::ContainerBuilder()
                .Size(content_width, content_height)
                .Layout<VerticalLayout>(UITheme::PADDING_MEDIUM, Alignment::Start)
                .Scrollable(ScrollDirection::Vertical)
                .Padding(UITheme::PADDING_SMALL)
                .ClipChildren()
                .Build();

        content_container_ = content.get();
        main_panel_->AddChild(std::move(content));

        // Add close button
        auto close_button = std::make_unique<Button>(
            UITheme::BUTTON_WIDTH_MEDIUM,
            UITheme::BUTTON_HEIGHT_MEDIUM,
            "Close",
            UITheme::FONT_SIZE_NORMAL
        );
        close_button->SetBorderColor(UITheme::ToEngineColor(UITheme::BUTTON_BORDER));
        close_button->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
        close_button->SetNormalColor(UITheme::ToEngineColor(UITheme::BUTTON_BACKGROUND));
        close_button->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::INFO, 0.3f)));
        close_button->SetClickCallback([this](const engine::ui::MouseEvent &event) {
            if (event.left_pressed) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                Hide();
                return true;
            }
            return false;
        });
        main_panel_->AddChild(std::move(close_button));

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;

        UpdateLayout();
    }

    void HelpSystem::UpdateLayout() {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (main_panel_) {
            const int panel_x = (screen_width - OVERLAY_WIDTH) / 2;
            const int panel_y = (screen_height - OVERLAY_HEIGHT) / 2;
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    void HelpSystem::RebuildContent() {
        if (!content_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        content_container_->ClearChildren();

        const auto &topics = help_content_[current_context_];
        constexpr float topic_width = OVERLAY_WIDTH - UITheme::PADDING_LARGE * 4;

        for (const auto &topic: topics) {
            // Create container for each topic with proper spacing
            auto topic_container = engine::ui::ContainerBuilder()
                    .Size(topic_width, 200) // Give reasonable height for content
                    .Layout<VerticalLayout>(8.0f, Alignment::Start)
                    .Opacity(0)
                    .Padding(UITheme::PADDING_SMALL)
                    .Build();

            // Topic title
            auto title = std::make_unique<Text>(
                0, 0,
                topic.title,
                UITheme::FONT_SIZE_MEDIUM,
                UITheme::ToEngineColor(UITheme::PRIMARY)
            );
            topic_container->AddChild(std::move(title));

            // Topic content
            auto content_text = std::make_unique<Text>(
                0, 0,
                topic.content,
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
            );
            topic_container->AddChild(std::move(content_text));

            // Tips
            if (!topic.tips.empty()) {
                // Add spacer before tips
                auto spacer = std::make_unique<Container>();
                spacer->SetSize(topic_width, 5);
                topic_container->AddChild(std::move(spacer));

                auto tips_header = std::make_unique<Text>(
                    0, 0,
                    "Quick Tips:",
                    UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(UITheme::INFO)
                );
                topic_container->AddChild(std::move(tips_header));

                for (const auto &tip: topic.tips) {
                    auto tip_text = std::make_unique<Text>(
                        0, 0,
                        "• " + tip,
                        UITheme::FONT_SIZE_SMALL,
                        UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
                    );
                    topic_container->AddChild(std::move(tip_text));
                }
            }

            content_container_->AddChild(std::move(topic_container));

            // Add divider between topics
            auto topic_divider = std::make_unique<Divider>();
            topic_divider->SetColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BORDER_SUBTLE, 0.5f)));
            topic_divider->SetSize(topic_width - UITheme::PADDING_LARGE, 1);
            content_container_->AddChild(std::move(topic_divider));
        }

        content_container_->InvalidateComponents();
        content_container_->UpdateComponentsRecursive();

        // Update scroll component content size
        if (auto *scroll = content_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    std::string HelpSystem::GetContextName(HelpContext context) const {
        switch (context) {
            case HelpContext::MainGame: return "Gameplay Help";
            case HelpContext::BuildMenu: return "Building Help";
            case HelpContext::ResearchTree: return "Research Tree Help";
            case HelpContext::ModsMenu: return "Mods Help";
            case HelpContext::StaffManagement: return "Staff Management Help";
            case HelpContext::Settings: return "Settings Help";
            case HelpContext::Tutorial: return "Tutorial Help";
            case HelpContext::PauseMenu: return "Pause Menu Help";
            case HelpContext::History: return "History Help";
            case HelpContext::Notifications: return "Notifications Help";
            default: return "Help";
        }
    }

    void HelpSystem::Update(const float delta_time) {
        if (!visible_ || !main_panel_) return;

        animation_time_ += delta_time;

        // Check for window resize
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        if (screen_width != last_screen_width_ || screen_height != last_screen_height_) {
            UpdateLayout();
        }
    }

    void HelpSystem::Render() {
        if (!visible_ || !main_panel_) return;

        RenderDimOverlay();
        main_panel_->Render();
    }

    void HelpSystem::RenderDimOverlay() const {
        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        engine::ui::BatchRenderer::SubmitQuad(
            engine::ui::Rectangle(0, 0, screen_width, screen_height),
            UITheme::ToEngineColor(ColorAlpha(BLACK, 0.7f))
        );
    }

    void HelpSystem::Show(const HelpContext context) {
        current_context_ = context;
        visible_ = true;
        animation_time_ = 0.0f;

        // Update title
        if (title_text_) {
            title_text_->SetText(GetContextName(context));
        }

        // Rebuild content for new context
        RebuildContent();
    }

    void HelpSystem::Hide() {
        visible_ = false;
    }

    void HelpSystem::Toggle() {
        if (visible_) {
            Hide();
        } else {
            Show(current_context_);
        }
    }

    bool HelpSystem::ProcessMouseEvent(const engine::ui::MouseEvent &event) {
        if (!visible_ || !main_panel_) return false;

        // Check for click outside panel to close
        if (event.left_pressed) {
            auto bounds = main_panel_->GetAbsoluteBounds();
            if (event.x < bounds.x || event.x > bounds.x + bounds.width ||
                event.y < bounds.y || event.y > bounds.y + bounds.height) {
                audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
                Hide();
                return true;
            }
        }

        return main_panel_->ProcessMouseEvent(event);
    }

    void HelpSystem::HandleKeyboard() {
        if (!visible_) return;

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_F1)) {
            audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClose);
            Hide();
        }
    }

    void HelpSystem::Shutdown() {
        title_text_ = nullptr;
        content_container_ = nullptr;
        main_panel_.reset();
    }

    void HelpSystem::InitializeMainGameHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Welcome to TowerForge",
            "Build and manage your own skyscraper! Place facilities, manage tenants, hire staff, and grow your tower to achieve a 5-star rating.",
            std::vector<std::string>{
                "Start by building a lobby on the ground floor",
                "Add businesses and shops to generate income",
                "Build residential condos to attract permanent tenants",
                "Use elevators and stairs to transport people between floors"
            }
        );

        topics.emplace_back(
            "Basic Controls",
            "ESC - Pause menu | F1 - Toggle help | R - Research tree | N - Notifications | H - History panel | Mouse wheel - Zoom camera | Arrow keys - Pan camera",
            std::vector<std::string>{
                "Click on facilities or people to view detailed information",
                "Left-click to select and place facilities from the build menu",
                "Right-click to cancel placement mode"
            }
        );

        topics.emplace_back(
            "Managing Your Tower",
            "Monitor your funds, population, and tower rating. Keep tenants satisfied by providing services, maintaining cleanliness, and ensuring good elevator coverage. Hire staff to clean facilities and perform maintenance.",
            std::vector<std::string>{
                "Watch your funds - don't overspend on construction",
                "Balance income from businesses with maintenance costs",
                "Higher satisfaction leads to better ratings and more tenants"
            }
        );

        help_content_[HelpContext::MainGame] = topics;
    }

    void HelpSystem::InitializeBuildMenuHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Building Facilities",
            "Select a facility from the build menu and click on the grid to place it. Each facility has a cost, size, and specific purpose. Make sure you have enough funds before building.",
            std::vector<std::string>{
                "Lobby - Required on ground floor, serves as entrance",
                "Office - Generates income from business tenants",
                "Retail Shop - Generates income and provides shopping for visitors",
                "Condo - Houses permanent residents",
                "Elevator - Transports people vertically (connects multiple floors)",
                "Stairs - Basic vertical transport (cheaper than elevators)"
            }
        );

        topics.emplace_back(
            "Floor Management",
            "Use the Add Floor and Add Basement buttons to expand your tower vertically. Each new floor costs money based on the tower width. Build floors strategically to accommodate new facilities.",
            std::vector<std::string>{
                "Ground floor (Floor 0) is where you start",
                "Basements go below ground (negative floor numbers)",
                "Larger towers cost more per floor"
            }
        );

        topics.emplace_back(
            "Undo and Redo",
            "Made a mistake? Use the Undo button to reverse your last placement. Redo allows you to reapply undone actions. Note: undoing returns a portion of the facility cost.",
            std::vector<std::string>{
                "Keyboard shortcuts: Ctrl+Z for undo, Ctrl+Y for redo",
                "Undo history is preserved as long as you don't close the game",
                "Some actions cannot be undone once time has passed"
            }
        );

        help_content_[HelpContext::BuildMenu] = topics;
    }

    void HelpSystem::InitializeResearchTreeHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Research System",
            "Unlock new facilities, upgrades, and bonuses by spending research points. Research points are earned by reaching milestones and progressing through the game. Each research node has prerequisites that must be unlocked first.",
            std::vector<std::string>{
                "Click on a node to see its details and unlock cost",
                "Locked nodes show what you need to unlock them",
                "Some research unlocks new facility types",
                "Other research provides passive bonuses (e.g., reduced costs, higher satisfaction)"
            }
        );

        topics.emplace_back(
            "Earning Research Points",
            "Research points are awarded when you achieve specific milestones: reaching new star ratings, hitting population targets, or completing special challenges. Plan your research path carefully!",
            std::vector<std::string>{
                "Early game: focus on unlocking essential facilities",
                "Mid game: invest in economic bonuses to increase income",
                "Late game: unlock prestige upgrades for maximum efficiency"
            }
        );

        help_content_[HelpContext::ResearchTree] = topics;
    }

    void HelpSystem::InitializeModsMenuHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Mod Support",
            "TowerForge supports Lua-based mods that can add new facilities, modify game mechanics, or introduce custom content. Browse installed mods, enable or disable them, and view mod details.",
            std::vector<std::string>{
                "Place mod files in the 'mods' directory",
                "Enable mods by clicking the checkbox next to each mod",
                "Some mods may require a game restart to take effect",
                "Check mod descriptions for compatibility information"
            }
        );

        topics.emplace_back(
            "Creating Mods",
            "Create your own mods using Lua scripting. Mods can register new facility types, modify economic formulas, add custom events, and more. See the modding documentation for API details.",
            std::vector<std::string>{
                "Start with the example mods in the 'mods' folder",
                "Use the mod manager to test your changes",
                "Join the community to share your creations"
            }
        );

        help_content_[HelpContext::ModsMenu] = topics;
    }

    void HelpSystem::InitializeStaffManagementHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Hiring Staff",
            "Staff members perform essential maintenance tasks to keep your tower running smoothly. Janitors clean facilities, maintenance workers repair equipment, and security guards ensure safety. Each staff member works specific shifts and has associated costs.",
            std::vector<std::string>{
                "Janitor - Cleans facilities to maintain satisfaction",
                "Maintenance - Repairs equipment and prevents breakdowns",
                "Security - Reduces crime and improves tenant safety",
                "Concierge - Improves lobby service and first impressions"
            }
        );

        topics.emplace_back(
            "Staff Efficiency",
            "Staff members work during assigned shifts (e.g., 9 AM - 5 PM). They prioritize facilities based on need - for example, janitors clean dirtier facilities first. Hire enough staff to cover all floors and shifts.",
            std::vector<std::string>{
                "Watch for low cleanliness or maintenance warnings",
                "Add staff if facilities are not being serviced in time",
                "Balance staff costs with the benefits they provide"
            }
        );

        help_content_[HelpContext::StaffManagement] = topics;
    }

    void HelpSystem::InitializeSettingsHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Game Settings",
            "Customize your gameplay experience through various settings. Adjust audio levels, change display options, configure controls, and enable accessibility features.",
            std::vector<std::string>{
                "Audio - Control master volume, music, and sound effects",
                "Display - Adjust resolution, fullscreen mode, and graphics quality",
                "Accessibility - Enable colorblind modes, text scaling, and UI simplification",
                "Gameplay - Modify difficulty, autosave frequency, and simulation speed"
            }
        );

        topics.emplace_back(
            "Saving Settings",
            "All settings changes are saved automatically. Your preferences persist across game sessions. You can reset to defaults at any time from the settings menu.",
            std::vector<std::string>{}
        );

        help_content_[HelpContext::Settings] = topics;
    }

    void HelpSystem::InitializeTutorialHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Tutorial Mode",
            "The tutorial guides you through building your first tower. Follow the step-by-step instructions to learn the basics of facility placement, management, and progression.",
            std::vector<std::string>{
                "You can skip the tutorial at any time",
                "Tutorial progress is tracked - you can resume later",
                "Completing the tutorial unlocks a small bonus"
            }
        );

        help_content_[HelpContext::Tutorial] = topics;
    }

    void HelpSystem::InitializePauseMenuHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Pause Menu",
            "Access game options, save/load your progress, view achievements, and return to the main menu. The game is paused while this menu is open.",
            std::vector<std::string>{
                "Save often to avoid losing progress",
                "Use quick save (F5) to save without opening the menu",
                "Settings changes take effect immediately"
            }
        );

        help_content_[HelpContext::PauseMenu] = topics;
    }

    void HelpSystem::InitializeHistoryHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "History Panel",
            "The history panel shows recent actions you've taken (facility placements, demolitions, etc.). Click on an entry to quickly undo or redo that action.",
            std::vector<std::string>{
                "Press H to toggle the history panel",
                "Hover over entries to see action details",
                "Click to undo/redo specific actions in bulk"
            }
        );

        help_content_[HelpContext::History] = topics;
    }

    void HelpSystem::InitializeNotificationsHelp() {
        std::vector<HelpTopic> topics;

        topics.emplace_back(
            "Notification Center",
            "View important alerts, achievements, and game events. Notifications are categorized by priority and can be clicked for more details or to take action.",
            std::vector<std::string>{
                "Press N to toggle the notification center",
                "Red notifications indicate critical issues (fires, low funds)",
                "Green notifications indicate positive events (achievements, milestones)",
                "Click on notifications to dismiss them or take action"
            }
        );

        help_content_[HelpContext::Notifications] = topics;
    }
}
