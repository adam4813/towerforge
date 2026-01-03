#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/components.hpp"
#include "ui/tooltip.h"
#include "ui/research_details_panel.h"
#include "ui/engine_confirmation_dialog.h"

import engine;

namespace towerforge::ui {
    // Forward declarations
    class NotificationCenter;
    class TooltipManager;

    /**
     * @brief Research/Upgrade Tree menu for unlocking new features and bonuses
     * 
     * Displays a grid-based tree of research nodes that can be unlocked
     * using tower points. Uses engine UI components with GridLayout for
     * node display and TabContainer for categorization.
     */
    class ResearchTreeMenu {
    public:
        ResearchTreeMenu();

        ~ResearchTreeMenu();

        /**
         * @brief Initialize UI components - must be called before use
         * @param research_tree Reference to the research tree for node data
         */
        void Initialize(const core::ResearchTree &research_tree);

        /**
         * @brief Clean up UI resources
         */
        void Shutdown();

        /**
         * @brief Render the research tree menu overlay
         * @param research_tree Reference to the research tree singleton
         */
        void Render(const core::ResearchTree &research_tree);

        /**
         * @brief Update menu state (called every frame when active)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Check if menu is visible
         * @return True if visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Set visibility of menu
         * @param visible Whether menu should be visible
         */
        void SetVisible(const bool visible) { visible_ = visible; }

        /**
         * @brief Toggle visibility
         */
        void Toggle() { visible_ = !visible_; }

        /**
         * @brief Set notification center for unlock feedback
         */
        void SetNotificationCenter(NotificationCenter *notification_center) {
            notification_center_ = notification_center;
        }

        /**
         * @brief Set tooltip manager for node information
         */
        void SetTooltipManager(TooltipManager *tooltip_manager) {
            tooltip_manager_ = tooltip_manager;
        }

        /**
         * @brief Process mouse events using engine event system
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Handle keyboard input
         */
        void HandleKeyboard();

        /**
         * @brief Rebuild UI when research tree changes
         * @param research_tree Reference to the research tree
         */
        void RebuildUI(const core::ResearchTree &research_tree);

    private:
        /**
         * @brief Update layout for responsive positioning
         */
        void UpdateLayout();

        /**
         * @brief Render dim overlay behind menu
         */
        static void RenderDimOverlay();

        /**
         * @brief Create content panel for a research category
         */
        std::unique_ptr<engine::ui::elements::Container> CreateCategoryContent(
            core::ResearchNodeType type,
            const std::vector<const core::ResearchNode *> &nodes,
            int content_width);

        /**
         * @brief Handle unlock button click from details panel
         */
        void OnUnlockRequested(const std::string &node_id);

        /**
         * @brief Get color for node based on state
         */
        static Color GetNodeStateColor(core::ResearchNodeState state);

        /**
         * @brief Get border color for node based on state
         */
        static Color GetNodeBorderColor(core::ResearchNodeState state);

        /**
         * @brief Build tooltip text for a research node
         */
        static std::string BuildNodeTooltip(const core::ResearchNode &node);

        // State
        bool visible_;
        float animation_time_;
        int selected_node_index_;
        NotificationCenter *notification_center_;
        TooltipManager *tooltip_manager_;

        // Screen tracking for responsive layout
        int last_screen_width_;
        int last_screen_height_;

        // Engine UI components
        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::TabContainer *tab_container_;
        engine::ui::elements::Button *close_button_;
        std::vector<engine::ui::elements::Button *> node_buttons_;

        // Details panel
        std::unique_ptr<ResearchDetailsPanel> details_panel_;

        // Confirmation dialog for research unlocks
        std::unique_ptr<EngineConfirmationDialog> unlock_confirmation_;
        std::string pending_unlock_node_id_;
        core::ResearchTree *pending_unlock_tree_;

        // Menu layout constants
        static constexpr int MENU_WIDTH = 800;
        static constexpr int MENU_HEIGHT = 600;
        static constexpr int TAB_HEIGHT = 40;
        static constexpr int HEADER_HEIGHT = 90;
        static constexpr int NODE_BUTTON_SIZE = 100;
        static constexpr int NODES_PER_ROW = 3;
        static constexpr int GRID_PADDING = 12;
        static constexpr int CLOSE_BUTTON_SIZE = 30;
        static constexpr int DETAILS_PANEL_WIDTH = 220;
        static constexpr int GRID_AREA_WIDTH = MENU_WIDTH - DETAILS_PANEL_WIDTH - 10;
    };
}
