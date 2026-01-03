#include "ui/help_system.h"
#include "ui/ui_theme.h"
#include "audio/audio_manager.h"
#include <cmath>
#include <algorithm>

import engine;

namespace towerforge::ui {
    HelpSystem::HelpSystem()
        : visible_(false)
          , animation_time_(0.0f)
          , last_screen_width_(0)
          , last_screen_height_(0)
          , title_text_(nullptr)
          , nav_container_(nullptr)
          , content_container_(nullptr) {
    }

    HelpSystem::~HelpSystem() = default;

    void HelpSystem::Initialize() {
        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        // Initialize default help topics
        InitializeDefaultTopics();

        std::uint32_t screen_width;
        std::uint32_t screen_height;
        engine::rendering::GetRenderer().GetFramebufferSize(screen_width, screen_height);

        // Create main panel
        const int panel_x = std::max(0, static_cast<int>(screen_width - OVERLAY_WIDTH) / 2);
        const int panel_y = std::max(0, static_cast<int>(screen_height - OVERLAY_HEIGHT) / 2);

        main_panel_ = std::make_unique<Panel>();
        main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
        main_panel_->SetSize(static_cast<float>(OVERLAY_WIDTH), static_cast<float>(OVERLAY_HEIGHT));
        main_panel_->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_PANEL, 0.95f)));
        main_panel_->SetBorderColor(UITheme::ToEngineColor(UITheme::INFO));
        main_panel_->SetPadding(static_cast<float>(PADDING));
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

        // Add instructions
        auto instructions = std::make_unique<Text>(
            0, 0,
            "Press F1 or ESC to close",
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        main_panel_->AddChild(std::move(instructions));

        // Add divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::INFO));
        divider->SetSize(OVERLAY_WIDTH - PADDING * 2, 2);
        main_panel_->AddChild(std::move(divider));

        // Create horizontal container for nav + content
        constexpr float body_width = OVERLAY_WIDTH - PADDING * 2;
        constexpr float body_height = OVERLAY_HEIGHT - HEADER_HEIGHT - 100;

        auto body_container = engine::ui::ContainerBuilder()
                .Size(body_width, body_height)
                .Layout<HorizontalLayout>(static_cast<float>(UITheme::MARGIN_SMALL), Alignment::Start)
                .Build();

        // Navigation sidebar (scrollable list of topics)
        auto nav = engine::ui::ContainerBuilder()
                .Size(static_cast<float>(NAV_WIDTH), body_height)
                .Layout<VerticalLayout>(4.0f, Alignment::Start)
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Padding(static_cast<float>(UITheme::PADDING_SMALL))
                .Build();

        nav->SetBackgroundColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BACKGROUND_DARK, 0.5f)));
        nav_container_ = nav.get();
        body_container->AddChild(std::move(nav));

        // Content area (scrollable topic content)
        constexpr float content_width = body_width - NAV_WIDTH - UITheme::MARGIN_SMALL;

        auto content = engine::ui::ContainerBuilder()
                .Size(content_width, body_height)
                .Layout<VerticalLayout>(static_cast<float>(UITheme::PADDING_MEDIUM), Alignment::Start)
                .Scrollable(ScrollDirection::Vertical)
                .ClipChildren()
                .Padding(static_cast<float>(UITheme::PADDING_MEDIUM))
                .Build();

        content_container_ = content.get();
        body_container->AddChild(std::move(content));

        main_panel_->AddChild(std::move(body_container));

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
            const int panel_x = std::max(0, static_cast<int>(screen_width - OVERLAY_WIDTH) / 2);
            const int panel_y = std::max(0, static_cast<int>(screen_height - OVERLAY_HEIGHT) / 2);
            main_panel_->SetRelativePosition(static_cast<float>(panel_x), static_cast<float>(panel_y));
            main_panel_->InvalidateComponents();
            main_panel_->UpdateComponentsRecursive();
        }

        last_screen_width_ = screen_width;
        last_screen_height_ = screen_height;
    }

    // === Topic Registration API ===

    bool HelpSystem::RegisterTopic(const HelpTopic& topic) {
        if (topics_.contains(topic.id)) {
            return false;
        }
        topics_[topic.id] = topic;
        
        // Add category to order list if new
        if (std::find(category_order_.begin(), category_order_.end(), topic.category) == category_order_.end()) {
            category_order_.push_back(topic.category);
        }
        
        return true;
    }

    void HelpSystem::RegisterTopics(const std::vector<HelpTopic>& topics) {
        for (const auto& topic : topics) {
            RegisterTopic(topic);
        }
    }

    bool HelpSystem::RemoveTopic(const std::string& id) {
        return topics_.erase(id) > 0;
    }

    bool HelpSystem::UpdateTopic(const HelpTopic& topic) {
        auto it = topics_.find(topic.id);
        if (it == topics_.end()) {
            return false;
        }
        it->second = topic;
        return true;
    }

    bool HelpSystem::HasTopic(const std::string& id) const {
        return topics_.contains(id);
    }

    const HelpTopic* HelpSystem::GetTopic(const std::string& id) const {
        auto it = topics_.find(id);
        return it != topics_.end() ? &it->second : nullptr;
    }

    std::vector<std::string> HelpSystem::GetCategories() const {
        return category_order_;
    }

    std::vector<const HelpTopic*> HelpSystem::GetTopicsInCategory(const std::string& category) const {
        std::vector<const HelpTopic*> result;
        for (const auto& [id, topic] : topics_) {
            if (topic.category == category) {
                result.push_back(&topic);
            }
        }
        // Sort by sort_order
        std::sort(result.begin(), result.end(), [](const HelpTopic* a, const HelpTopic* b) {
            return a->sort_order < b->sort_order;
        });
        return result;
    }

    void HelpSystem::RebuildNavigation() {
        if (!nav_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        nav_container_->ClearChildren();
        nav_buttons_.clear();

        constexpr float button_width = NAV_WIDTH - UITheme::PADDING_MEDIUM;
        constexpr float button_height = 28.0f;

        for (const auto& category : category_order_) {
            // Category header
            auto cat_header = std::make_unique<Text>(
                0, 0,
                category,
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(UITheme::INFO)
            );
            nav_container_->AddChild(std::move(cat_header));

            // Topics in this category
            auto topics_in_cat = GetTopicsInCategory(category);
            for (const auto* topic : topics_in_cat) {
                if (!topic->show_in_navigation) continue;

                const bool is_selected = (topic->id == current_topic_id_);

                auto btn = std::make_unique<Button>(
                    button_width,
                    button_height,
                    topic->title,
                    UITheme::FONT_SIZE_SMALL
                );

                if (is_selected) {
                    // Highlight selected button
                    btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.4f)));
                    btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.5f)));
                    btn->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_PRIMARY));
                    btn->SetBorderColor(UITheme::ToEngineColor(UITheme::PRIMARY));
                } else {
                    btn->SetNormalColor(UITheme::ToEngineColor(ColorAlpha(UITheme::BUTTON_BACKGROUND, 0.5f)));
                    btn->SetHoverColor(UITheme::ToEngineColor(ColorAlpha(UITheme::PRIMARY, 0.3f)));
                    btn->SetTextColor(UITheme::ToEngineColor(UITheme::TEXT_SECONDARY));
                    btn->SetBorderColor(UITheme::ToEngineColor(UITheme::BORDER_SUBTLE));
                }

                std::string topic_id = topic->id;
                btn->SetClickCallback([this, topic_id](const engine::ui::MouseEvent& event) {
                    if (event.left_pressed) {
                        audio::AudioManager::GetInstance().PlaySFX(audio::AudioCue::MenuClick);
                        SelectTopic(topic_id);
                        return true;
                    }
                    return false;
                });

                nav_buttons_.push_back(btn.get());
                nav_container_->AddChild(std::move(btn));
            }

            // Spacer between categories
            auto spacer = std::make_unique<Container>();
            spacer->SetSize(button_width, 8);
            nav_container_->AddChild(std::move(spacer));
        }

        nav_container_->InvalidateComponents();
        nav_container_->UpdateComponentsRecursive();

        if (auto* scroll = nav_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void HelpSystem::RebuildContent() {
        if (!content_container_) return;

        using namespace engine::ui::components;
        using namespace engine::ui::elements;

        content_container_->ClearChildren();

        const HelpTopic* topic = GetTopic(current_topic_id_);
        if (!topic) return;

        constexpr float content_width = OVERLAY_WIDTH - NAV_WIDTH - PADDING * 3;

        // Topic title
        auto title = std::make_unique<Text>(
            0, 0,
            topic->title,
            UITheme::FONT_SIZE_MEDIUM,
            UITheme::ToEngineColor(UITheme::PRIMARY)
        );
        content_container_->AddChild(std::move(title));

        // Divider
        auto divider = std::make_unique<Divider>();
        divider->SetColor(UITheme::ToEngineColor(UITheme::PRIMARY));
        divider->SetSize(content_width - UITheme::PADDING_LARGE, 1);
        content_container_->AddChild(std::move(divider));

        // Topic content
        auto content_text = std::make_unique<Text>(
            0, 0,
            topic->content,
            UITheme::FONT_SIZE_SMALL,
            UITheme::ToEngineColor(UITheme::TEXT_SECONDARY)
        );
        content_container_->AddChild(std::move(content_text));

        // Tips
        if (!topic->tips.empty()) {
            auto spacer = std::make_unique<Container>();
            spacer->SetSize(content_width, 10);
            content_container_->AddChild(std::move(spacer));

            auto tips_header = std::make_unique<Text>(
                0, 0,
                "Quick Tips:",
                UITheme::FONT_SIZE_SMALL,
                UITheme::ToEngineColor(UITheme::INFO)
            );
            content_container_->AddChild(std::move(tips_header));

            for (const auto& tip : topic->tips) {
                auto tip_text = std::make_unique<Text>(
                    0, 0,
                    "• " + tip,
                    UITheme::FONT_SIZE_SMALL,
                    UITheme::ToEngineColor(UITheme::TEXT_PRIMARY)
                );
                content_container_->AddChild(std::move(tip_text));
            }
        }

        content_container_->InvalidateComponents();
        content_container_->UpdateComponentsRecursive();

        if (auto* scroll = content_container_->GetComponent<ScrollComponent>()) {
            scroll->CalculateContentSizeFromChildren();
        }
    }

    void HelpSystem::SelectTopic(const std::string& topic_id) {
        if (!HasTopic(topic_id)) return;
        
        current_topic_id_ = topic_id;
        const auto* topic = GetTopic(topic_id);
        if (topic) {
            current_category_ = topic->category;
        }
        
        RebuildNavigation();
        RebuildContent();
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

    void HelpSystem::Show() {
        visible_ = true;
        animation_time_ = 0.0f;

        // Select first topic if none selected
        if (current_topic_id_.empty() && !topics_.empty()) {
            // Get first topic from first category
            if (!category_order_.empty()) {
                auto topics_in_cat = GetTopicsInCategory(category_order_[0]);
                if (!topics_in_cat.empty()) {
                    SelectTopic(topics_in_cat[0]->id);
                }
            }
        } else {
            RebuildContent();
        }

        RebuildNavigation();
        UpdateLayout();
    }

    void HelpSystem::ShowTopic(const std::string& topic_id) {
        visible_ = true;
        animation_time_ = 0.0f;
        SelectTopic(topic_id);
        RebuildNavigation();
        UpdateLayout();
    }

    void HelpSystem::ShowCategory(const std::string& category) {
        auto topics_in_cat = GetTopicsInCategory(category);
        if (!topics_in_cat.empty()) {
            ShowTopic(topics_in_cat[0]->id);
        } else {
            Show();
        }
    }

    void HelpSystem::Hide() {
        visible_ = false;
    }

    void HelpSystem::Toggle() {
        if (visible_) {
            Hide();
        } else {
            Show();
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
        nav_container_ = nullptr;
        content_container_ = nullptr;
        nav_buttons_.clear();
        main_panel_.reset();
    }

    void HelpSystem::InitializeDefaultTopics() {
        // Getting Started
        RegisterTopic({
            "welcome", "Getting Started", "Welcome to TowerForge",
            "Build and manage your own skyscraper! Place facilities, manage tenants, hire staff, and grow your tower to achieve a 5-star rating.",
            {"Start by building a lobby on the ground floor", "Add businesses and shops to generate income", "Build residential condos to attract permanent tenants", "Use elevators and stairs to transport people between floors"},
            true, 0
        });

        RegisterTopic({
            "controls", "Getting Started", "Basic Controls",
            "ESC - Pause menu | F1 - Toggle help | R - Research tree | N - Notifications | H - History panel | Mouse wheel - Zoom camera | Arrow keys - Pan camera",
            {"Click on facilities or people to view detailed information", "Left-click to select and place facilities from the build menu", "Right-click to cancel placement mode"},
            true, 1
        });

        RegisterTopic({
            "management", "Getting Started", "Managing Your Tower",
            "Monitor your funds, population, and tower rating. Keep tenants satisfied by providing services, maintaining cleanliness, and ensuring good elevator coverage. Hire staff to clean facilities and perform maintenance.",
            {"Watch your funds - don't overspend on construction", "Balance income from businesses with maintenance costs", "Higher satisfaction leads to better ratings and more tenants"},
            true, 2
        });

        // Building
        RegisterTopic({
            "facilities", "Building", "Building Facilities",
            "Select a facility from the build menu and click on the grid to place it. Each facility has a cost, size, and specific purpose. Make sure you have enough funds before building.",
            {"Lobby - Required on ground floor, serves as entrance", "Office - Generates income from business tenants", "Retail Shop - Generates income and provides shopping", "Condo - Houses permanent residents", "Elevator - Transports people vertically", "Stairs - Basic vertical transport (cheaper than elevators)"},
            true, 0
        });

        RegisterTopic({
            "floors", "Building", "Floor Management",
            "Use the Add Floor and Add Basement buttons to expand your tower vertically. Each new floor costs money based on the tower width. Build floors strategically to accommodate new facilities.",
            {"Ground floor (Floor 0) is where you start", "Basements go below ground (negative floor numbers)", "Larger towers cost more per floor"},
            true, 1
        });

        RegisterTopic({
            "undo-redo", "Building", "Undo and Redo",
            "Made a mistake? Use the Undo button to reverse your last placement. Redo allows you to reapply undone actions. Note: undoing returns a portion of the facility cost.",
            {"Keyboard shortcuts: Ctrl+Z for undo, Ctrl+Y for redo", "Undo history is preserved as long as you don't close the game", "Some actions cannot be undone once time has passed"},
            true, 2
        });

        // Research
        RegisterTopic({
            "research-system", "Research", "Research System",
            "Unlock new facilities, upgrades, and bonuses by spending research points. Research points are earned by reaching milestones and progressing through the game. Each research node has prerequisites that must be unlocked first.",
            {"Click on a node to see its details and unlock cost", "Locked nodes show what you need to unlock them", "Some research unlocks new facility types", "Other research provides passive bonuses"},
            true, 0
        });

        RegisterTopic({
            "earning-research", "Research", "Earning Research Points",
            "Research points are awarded when you achieve specific milestones: reaching new star ratings, hitting population targets, or completing special challenges. Plan your research path carefully!",
            {"Early game: focus on unlocking essential facilities", "Mid game: invest in economic bonuses to increase income", "Late game: unlock prestige upgrades for maximum efficiency"},
            true, 1
        });

        // Staff
        RegisterTopic({
            "hiring-staff", "Staff", "Hiring Staff",
            "Staff members perform essential maintenance tasks to keep your tower running smoothly. Janitors clean facilities, maintenance workers repair equipment, and security guards ensure safety.",
            {"Janitor - Cleans facilities to maintain satisfaction", "Maintenance - Repairs equipment and prevents breakdowns", "Security - Reduces crime and improves tenant safety", "Concierge - Improves lobby service and first impressions"},
            true, 0
        });

        RegisterTopic({
            "staff-efficiency", "Staff", "Staff Efficiency",
            "Staff members work during assigned shifts (e.g., 9 AM - 5 PM). They prioritize facilities based on need - for example, janitors clean dirtier facilities first. Hire enough staff to cover all floors and shifts.",
            {"Watch for low cleanliness or maintenance warnings", "Add staff if facilities are not being serviced in time", "Balance staff costs with the benefits they provide"},
            true, 1
        });

        // Mods
        RegisterTopic({
            "mod-support", "Mods", "Mod Support",
            "TowerForge supports Lua-based mods that can add new facilities, modify game mechanics, or introduce custom content. Browse installed mods, enable or disable them, and view mod details.",
            {"Place mod files in the 'mods' directory", "Enable mods by clicking the checkbox next to each mod", "Some mods may require a game restart to take effect", "Check mod descriptions for compatibility information"},
            true, 0
        });

        RegisterTopic({
            "creating-mods", "Mods", "Creating Mods",
            "Create your own mods using Lua scripting. Mods can register new facility types, modify economic formulas, add custom events, and more. See the modding documentation for API details.",
            {"Start with the example mods in the 'mods' folder", "Use the mod manager to test your changes", "Join the community to share your creations"},
            true, 1
        });

        // Settings
        RegisterTopic({
            "game-settings", "Settings", "Game Settings",
            "Customize your gameplay experience through various settings. Adjust audio levels, change display options, configure controls, and enable accessibility features.",
            {"Audio - Control master volume, music, and sound effects", "Display - Adjust resolution, fullscreen mode, and graphics quality", "Accessibility - Enable colorblind modes, text scaling, and UI simplification", "Gameplay - Modify difficulty, autosave frequency, and simulation speed"},
            true, 0
        });
    }
}
