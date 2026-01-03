#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Help topic - a single help entry with ID, title, and content
     * 
     * Topics can optionally appear in the navigation sidebar.
     * Topics are organized by category for grouping in navigation.
     */
    struct HelpTopic {
        std::string id; // Unique identifier for this topic
        std::string category; // Category for grouping (e.g., "Gameplay", "Building")
        std::string title; // Display title
        std::string content; // Main content text
        std::vector<std::string> tips; // Optional quick tips
        bool show_in_navigation = true; // Whether to show in nav sidebar
        int sort_order = 0; // Sort order within category (lower = first)

        HelpTopic() = default;

        HelpTopic(const std::string &id, const std::string &category, const std::string &title,
                  const std::string &content, const std::vector<std::string> &tips_list = {},
                  bool in_nav = true, int order = 0)
            : id(id), category(category), title(title), content(content),
              tips(tips_list), show_in_navigation(in_nav), sort_order(order) {
        }
    };

    /**
     * @brief Contextual help system - displays help overlays with navigation
     *
     * Features:
     * - Dynamic topic registration (for mods and different subsystems)
     * - Two-panel layout with navigation sidebar and content area
     * - Topics organized by category
     * - Direct navigation to specific topics by ID
     */
    class HelpSystem {
    public:
        HelpSystem();

        ~HelpSystem();

        /**
         * @brief Initialize the help system UI
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

        // === Topic Registration API ===

        /**
         * @brief Register a new help topic
         * @param topic The help topic to register
         * @return true if registration succeeded, false if ID already exists
         */
        bool RegisterTopic(const HelpTopic &topic);

        /**
         * @brief Register multiple topics at once
         * @param topics Vector of topics to register
         */
        void RegisterTopics(const std::vector<HelpTopic> &topics);

        /**
         * @brief Remove a help topic by ID
         * @param id The topic ID to remove
         * @return true if topic was found and removed
         */
        bool RemoveTopic(const std::string &id);

        /**
         * @brief Update an existing help topic
         * @param topic The updated topic (matched by ID)
         * @return true if topic was found and updated
         */
        bool UpdateTopic(const HelpTopic &topic);

        /**
         * @brief Check if a topic exists
         * @param id The topic ID to check
         */
        bool HasTopic(const std::string &id) const;

        /**
         * @brief Get a topic by ID
         * @param id The topic ID
         * @return Pointer to topic or nullptr if not found
         */
        const HelpTopic *GetTopic(const std::string &id) const;

        /**
         * @brief Get all registered categories
         */
        std::vector<std::string> GetCategories() const;

        /**
         * @brief Get all topics in a category
         * @param category The category name
         */
        std::vector<const HelpTopic *> GetTopicsInCategory(const std::string &category) const;

        // === Display API ===

        /**
         * @brief Show help system with default/first topic
         */
        void Show();

        /**
         * @brief Show help and navigate to a specific topic
         * @param topic_id The topic ID to display
         */
        void ShowTopic(const std::string &topic_id);

        /**
         * @brief Show help for a category (shows first topic in category)
         * @param category The category to display
         */
        void ShowCategory(const std::string &category);

        /**
         * @brief Hide the help overlay
         */
        void Hide();

        /**
         * @brief Toggle help visibility
         */
        void Toggle();

        /**
         * @brief Check if help is visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Get currently displayed topic ID
         */
        const std::string &GetCurrentTopicId() const { return current_topic_id_; }

        /**
         * @brief Process mouse events
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

        void RebuildNavigation();

        void RebuildContent();

        void SelectTopic(const std::string &topic_id);

        void InitializeDefaultTopics();

        bool visible_;
        std::string current_topic_id_;
        std::string current_category_;
        float animation_time_;
        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;

        // All registered topics (id -> topic)
        std::unordered_map<std::string, HelpTopic> topics_;

        // Category order for display
        std::vector<std::string> category_order_;

        // Engine UI elements
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Text *title_text_;
        engine::ui::elements::Container *nav_container_;
        engine::ui::elements::Container *content_container_;
        std::vector<engine::ui::elements::Button *> nav_buttons_;

        // UI layout constants
        static constexpr int OVERLAY_WIDTH = 900;
        static constexpr int OVERLAY_HEIGHT = 600;
        static constexpr int HEADER_HEIGHT = 60;
        static constexpr int NAV_WIDTH = 220;
        static constexpr int PADDING = 20;
    };
}
