#include "ui/help_system.h"
#include <cmath>
#include <algorithm>

namespace towerforge::ui {

    HelpSystem::HelpSystem()
        : visible_(false)
        , current_context_(HelpContext::MainGame)
        , animation_time_(0.0f)
        , scroll_offset_(0.0f)
        , max_scroll_(0.0f) {
    }

    HelpSystem::~HelpSystem() = default;

    void HelpSystem::Initialize() {
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
    }

    void HelpSystem::Update(const float delta_time) {
        if (visible_) {
            animation_time_ += delta_time;
        }
    }

    void HelpSystem::Render() {
        if (!visible_) return;

        RenderOverlay();
        RenderHeader();
        RenderContent();
        RenderCloseButton();
        if (max_scroll_ > 0) {
            RenderScrollbar();
        }
    }

    void HelpSystem::Show(const HelpContext context) {
        current_context_ = context;
        visible_ = true;
        animation_time_ = 0.0f;
        scroll_offset_ = 0.0f;
    }

    void HelpSystem::Hide() {
        visible_ = false;
    }

    void HelpSystem::Toggle() {
        visible_ = !visible_;
        if (visible_) {
            animation_time_ = 0.0f;
            scroll_offset_ = 0.0f;
        }
    }

    bool HelpSystem::HandleMouse(const int mouse_x, const int mouse_y, const bool clicked) {
        if (!visible_) return false;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        // Check if click is outside overlay (close help)
        if (clicked) {
            if (mouse_x < overlay_x || mouse_x > overlay_x + OVERLAY_WIDTH ||
                mouse_y < overlay_y || mouse_y > overlay_y + OVERLAY_HEIGHT) {
                Hide();
                return true;
            }

            // Check close button
            const int close_x = overlay_x + OVERLAY_WIDTH - CLOSE_BUTTON_SIZE - 10;
            const int close_y = overlay_y + 10;
            if (mouse_x >= close_x && mouse_x <= close_x + CLOSE_BUTTON_SIZE &&
                mouse_y >= close_y && mouse_y <= close_y + CLOSE_BUTTON_SIZE) {
                Hide();
                return true;
            }
        }

        // Handle scrolling
        const float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scroll_offset_ -= wheel * 30.0f;
            scroll_offset_ = std::clamp(scroll_offset_, 0.0f, max_scroll_);
        }

        return true;  // Consume all mouse events when help is visible
    }

    bool HelpSystem::IsMouseOverHelpIcon(const Rectangle& bounds) {
        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();
        return CheckCollisionPointRec({static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, bounds);
    }

    bool HelpSystem::RenderHelpIcon(const Rectangle& bounds, const int mouse_x, const int mouse_y) {
        const bool hovered = CheckCollisionPointRec({static_cast<float>(mouse_x), static_cast<float>(mouse_y)}, bounds);
        
        // Draw circle background
        const Color bg_color = hovered ? ColorAlpha(SKYBLUE, 0.6f) : ColorAlpha(DARKBLUE, 0.4f);
        DrawCircle(
            static_cast<int>(bounds.x + bounds.width / 2),
            static_cast<int>(bounds.y + bounds.height / 2),
            bounds.width / 2,
            bg_color
        );
        
        // Draw border
        const Color border_color = hovered ? SKYBLUE : BLUE;
        DrawCircleLines(
            static_cast<int>(bounds.x + bounds.width / 2),
            static_cast<int>(bounds.y + bounds.height / 2),
            bounds.width / 2,
            border_color
        );
        
        // Draw "?" text
        const char* text = "?";
        const int text_size = static_cast<int>(bounds.width * 0.7f);
        const int text_width = MeasureText(text, text_size);
        DrawText(
            text,
            static_cast<int>(bounds.x + (bounds.width - text_width) / 2),
            static_cast<int>(bounds.y + (bounds.height - text_size) / 2),
            text_size,
            WHITE
        );
        
        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void HelpSystem::RenderOverlay() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        // Draw dim background over entire screen
        DrawRectangle(0, 0, screen_width, screen_height, ColorAlpha(BLACK, 0.5f));

        // Draw main help window
        DrawRectangle(overlay_x, overlay_y, OVERLAY_WIDTH, OVERLAY_HEIGHT, ColorAlpha(BLACK, 0.95f));
        
        // Draw border with subtle glow effect
        const float pulse = static_cast<float>(sin(animation_time_ * 2.0) * 0.1 + 0.9);
        DrawRectangleLines(overlay_x, overlay_y, OVERLAY_WIDTH, OVERLAY_HEIGHT, ColorAlpha(SKYBLUE, pulse));
        DrawRectangleLines(overlay_x + 1, overlay_y + 1, OVERLAY_WIDTH - 2, OVERLAY_HEIGHT - 2, ColorAlpha(SKYBLUE, pulse * 0.5f));
    }

    void HelpSystem::RenderHeader() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        // Draw header background
        DrawRectangle(overlay_x, overlay_y, OVERLAY_WIDTH, HEADER_HEIGHT, ColorAlpha(SKYBLUE, 0.2f));
        DrawLine(overlay_x, overlay_y + HEADER_HEIGHT, overlay_x + OVERLAY_WIDTH, overlay_y + HEADER_HEIGHT, SKYBLUE);

        // Get context name
        std::string context_name;
        switch (current_context_) {
            case HelpContext::MainGame: context_name = "Gameplay Help"; break;
            case HelpContext::BuildMenu: context_name = "Building Help"; break;
            case HelpContext::ResearchTree: context_name = "Research Tree Help"; break;
            case HelpContext::ModsMenu: context_name = "Mods Help"; break;
            case HelpContext::StaffManagement: context_name = "Staff Management Help"; break;
            case HelpContext::Settings: context_name = "Settings Help"; break;
            case HelpContext::Tutorial: context_name = "Tutorial Help"; break;
            case HelpContext::PauseMenu: context_name = "Pause Menu Help"; break;
            case HelpContext::History: context_name = "History Help"; break;
            case HelpContext::Notifications: context_name = "Notifications Help"; break;
        }

        // Draw title
        constexpr int title_font_size = 24;
        const int title_width = MeasureText(context_name.c_str(), title_font_size);
        DrawText(context_name.c_str(), overlay_x + (OVERLAY_WIDTH - title_width) / 2, 
                 overlay_y + (HEADER_HEIGHT - title_font_size) / 2, title_font_size, SKYBLUE);

        // Draw "Press F1 or ESC to close" hint
        const char* hint = "Press F1 or ESC to close";
        constexpr int hint_size = 12;
        DrawText(hint, overlay_x + PADDING, overlay_y + OVERLAY_HEIGHT - 20, hint_size, GRAY);
    }

    void HelpSystem::RenderContent() {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        const int content_x = overlay_x + PADDING;
        const int content_y = overlay_y + HEADER_HEIGHT + PADDING;
        const int content_width = OVERLAY_WIDTH - PADDING * 2 - SCROLLBAR_WIDTH - 5;
        const int content_height = OVERLAY_HEIGHT - HEADER_HEIGHT - PADDING * 2 - 30;

        // Enable scissor mode for scrolling
        BeginScissorMode(content_x, content_y, content_width, content_height);

        int y_offset = content_y - static_cast<int>(scroll_offset_);

        // Get help topics for current context
        const auto& topics = help_content_[current_context_];

        for (const auto& topic : topics) {
            // Draw topic title
            constexpr int title_size = 18;
            DrawText(topic.title.c_str(), content_x, y_offset, title_size, GOLD);
            y_offset += title_size + 10;

            // Draw topic content (word wrap)
            const int max_chars_per_line = content_width / 8;  // Approximate
            std::string wrapped_content = topic.content;
            
            // Simple word wrapping
            size_t line_start = 0;
            while (line_start < wrapped_content.length()) {
                size_t line_end = line_start + max_chars_per_line;
                if (line_end >= wrapped_content.length()) {
                    line_end = wrapped_content.length();
                } else {
                    // Find last space before max width
                    size_t last_space = wrapped_content.rfind(' ', line_end);
                    if (last_space != std::string::npos && last_space > line_start) {
                        line_end = last_space;
                    }
                }
                
                const std::string line = wrapped_content.substr(line_start, line_end - line_start);
                DrawText(line.c_str(), content_x + 10, y_offset, 14, LIGHTGRAY);
                y_offset += 18;
                line_start = line_end + 1;
            }

            y_offset += 5;

            // Draw tips if available
            if (!topic.tips.empty()) {
                DrawText("Quick Tips:", content_x + 10, y_offset, 14, SKYBLUE);
                y_offset += 18;

                for (const auto& tip : topic.tips) {
                    const std::string bullet = "• " + tip;
                    DrawText(bullet.c_str(), content_x + 20, y_offset, 13, WHITE);
                    y_offset += 16;
                }
            }

            y_offset += 15;  // Space between topics
        }

        EndScissorMode();

        // Calculate max scroll
        const int total_content_height = y_offset - (content_y - static_cast<int>(scroll_offset_));
        max_scroll_ = std::max(0.0f, static_cast<float>(total_content_height - content_height));
    }

    void HelpSystem::RenderCloseButton() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        const int close_x = overlay_x + OVERLAY_WIDTH - CLOSE_BUTTON_SIZE - 10;
        const int close_y = overlay_y + 10;

        const int mouse_x = GetMouseX();
        const int mouse_y = GetMouseY();
        const bool hovered = mouse_x >= close_x && mouse_x <= close_x + CLOSE_BUTTON_SIZE &&
                           mouse_y >= close_y && mouse_y <= close_y + CLOSE_BUTTON_SIZE;

        const Color bg_color = hovered ? ColorAlpha(RED, 0.5f) : ColorAlpha(DARKGRAY, 0.3f);
        DrawRectangle(close_x, close_y, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE, bg_color);
        DrawRectangleLines(close_x, close_y, CLOSE_BUTTON_SIZE, CLOSE_BUTTON_SIZE, hovered ? RED : GRAY);
        DrawText("X", close_x + 5, close_y + 2, 14, WHITE);
    }

    void HelpSystem::RenderScrollbar() const {
        if (max_scroll_ <= 0) return;

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const int overlay_x = (screen_width - OVERLAY_WIDTH) / 2;
        const int overlay_y = (screen_height - OVERLAY_HEIGHT) / 2;

        const int scrollbar_x = overlay_x + OVERLAY_WIDTH - SCROLLBAR_WIDTH - PADDING;
        const int scrollbar_y = overlay_y + HEADER_HEIGHT + PADDING;
        const int scrollbar_height = OVERLAY_HEIGHT - HEADER_HEIGHT - PADDING * 2 - 30;

        // Draw scrollbar track
        DrawRectangle(scrollbar_x, scrollbar_y, SCROLLBAR_WIDTH, scrollbar_height, ColorAlpha(DARKGRAY, 0.3f));

        // Draw scrollbar thumb
        const float scroll_ratio = scroll_offset_ / max_scroll_;
        const float thumb_height_ratio = static_cast<float>(scrollbar_height) / (scrollbar_height + max_scroll_);
        const int thumb_height = std::max(20, static_cast<int>(scrollbar_height * thumb_height_ratio));
        const int thumb_y = scrollbar_y + static_cast<int>(scroll_ratio * (scrollbar_height - thumb_height));

        DrawRectangle(scrollbar_x, thumb_y, SCROLLBAR_WIDTH, thumb_height, ColorAlpha(SKYBLUE, 0.7f));
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
