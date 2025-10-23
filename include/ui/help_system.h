#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace towerforge::ui {

    /**
     * @brief Help context enumeration - determines which help content to show
     */
    enum class HelpContext {
        MainGame,           // General gameplay
        BuildMenu,          // Building and placement
        ResearchTree,       // Research and upgrades
        ModsMenu,           // Mod management
        StaffManagement,    // Staff hiring and management
        Settings,           // Settings screens
        Tutorial,           // Tutorial mode
        PauseMenu,          // Pause menu
        History,            // History/undo panel
        Notifications       // Notification center
    };

    /**
     * @brief Help topic - a single help entry with title and content
     */
    struct HelpTopic {
        std::string title;
        std::string content;
        std::vector<std::string> tips;  // Optional quick tips
        
        HelpTopic() = default;
        HelpTopic(const std::string& t, const std::string& c, const std::vector<std::string>& tips_list = {})
            : title(t), content(c), tips(tips_list) {}
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
         * @brief Handle mouse input
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         * @return True if input was handled
         */
        bool HandleMouse(int mouse_x, int mouse_y, bool clicked);

        /**
         * @brief Check if mouse is over help icon button
         * @param bounds Button bounds to check
         * @return True if mouse is over the help icon
         */
        static bool IsMouseOverHelpIcon(const Rectangle& bounds);

        /**
         * @brief Render a help icon button
         * @param bounds Button bounds
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @return True if button was clicked
         */
        static bool RenderHelpIcon(const Rectangle& bounds, int mouse_x, int mouse_y);

    private:
        void RenderOverlay() const;
        void RenderHeader() const;
        void RenderContent();
        void RenderCloseButton() const;
        void RenderScrollbar() const;

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

        bool visible_;
        HelpContext current_context_;
        float animation_time_;
        float scroll_offset_;
        float max_scroll_;

        // Help content organized by context
        std::unordered_map<HelpContext, std::vector<HelpTopic>> help_content_;

        // UI layout constants
        static constexpr int OVERLAY_WIDTH = 700;
        static constexpr int OVERLAY_HEIGHT = 550;
        static constexpr int HEADER_HEIGHT = 50;
        static constexpr int PADDING = 20;
        static constexpr int CLOSE_BUTTON_SIZE = 20;
        static constexpr int SCROLLBAR_WIDTH = 10;
        static constexpr int HELP_ICON_SIZE = 24;
    };

}
