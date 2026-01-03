#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

import engine;

namespace towerforge::ui {
    /**
     * @brief Help context enumeration - determines which help content to show
     */
    enum class HelpContext {
        MainGame, // General gameplay
        BuildMenu, // Building and placement
        ResearchTree, // Research and upgrades
        ModsMenu, // Mod management
        StaffManagement, // Staff hiring and management
        Settings, // Settings screens
        Tutorial, // Tutorial mode
        PauseMenu, // Pause menu
        History, // History/undo panel
        Notifications // Notification center
    };

    /**
     * @brief Help topic - a single help entry with title and content
     */
    struct HelpTopic {
        std::string title;
        std::string content;
        std::vector<std::string> tips; // Optional quick tips

        HelpTopic() = default;

        HelpTopic(const std::string &t, const std::string &c, const std::vector<std::string> &tips_list = {})
            : title(t), content(c), tips(tips_list) {
        }
    };

    /**
     * @brief Contextual help system - displays help overlays and quick reference
     *
     * Provides context-sensitive help accessible via F1 key or help icons.
     * Shows relevant mechanics and controls based on current screen/menu.
     */
    class HelpSystem {
    public:
        HelpSystem();

        ~HelpSystem();

        /**
         * @brief Initialize help content for all contexts
         */
        void Initialize();

        /**
         * @brief Update help system state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Render help overlay
         */
        void Render();

        /**
         * @brief Show help for specific context
         * @param context The help context to display
         */
        void Show(HelpContext context);

        /**
         * @brief Hide the help overlay
         */
        void Hide();

        /**
         * @brief Toggle help visibility for current context
         */
        void Toggle();

        /**
         * @brief Check if help is visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Set current context (for automatic context detection)
         * @param context The current game context
         */
        void SetCurrentContext(const HelpContext context) { current_context_ = context; }

        /**
         * @brief Get current context
         */
        HelpContext GetCurrentContext() const { return current_context_; }

        /**
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event);

        /**
         * @brief Handle keyboard input
         */
        void HandleKeyboard();

        /**
         * @brief Shutdown and cleanup UI resources
         */
        void Shutdown();

    private:
        void RenderDimOverlay() const;

        void UpdateLayout();

        void RebuildContent();

        void InitializeMainGameHelp();

        void InitializeBuildMenuHelp();

        void InitializeResearchTreeHelp();

        void InitializeModsMenuHelp();

        void InitializeStaffManagementHelp();

        void InitializeSettingsHelp();

        void InitializeTutorialHelp();

        void InitializePauseMenuHelp();

        void InitializeHistoryHelp();

        void InitializeNotificationsHelp();

        std::string GetContextName(HelpContext context) const;

        bool visible_;
        HelpContext current_context_;
        float animation_time_;
        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;

        // Help content organized by context
        std::unordered_map<HelpContext, std::vector<HelpTopic> > help_content_;

        // Engine UI elements
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Text *title_text_;
        engine::ui::elements::Container *content_container_;

        // UI layout constants
        static constexpr int OVERLAY_WIDTH = 700;
        static constexpr int OVERLAY_HEIGHT = 550;
        static constexpr int HEADER_HEIGHT = 60;
        static constexpr int PADDING = 20;
        static constexpr int TOPIC_SPACING = 15;
    };
}
