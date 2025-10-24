#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include "core/components.hpp"
#include "ui/ui_element.h"

namespace towerforge::ui {
    // Forward declaration
    class NotificationCenter;

    /**
 * @brief Research/Upgrade Tree menu for unlocking new features and bonuses
 * 
 * Displays a grid-based tree of research nodes that can be unlocked
 * using research points. Shows locked/unlocked/upgradable states.
 */
    class ResearchTreeMenu {
    public:
        ResearchTreeMenu();
        ~ResearchTreeMenu();
    
        /**
     * @brief Render the research tree menu overlay
     * @param research_tree Reference to the research tree singleton
     */
        void Render(const TowerForge::Core::ResearchTree& research_tree);
    
        /**
     * @brief Update menu state (called every frame when active)
     * @param delta_time Time elapsed since last frame
     */
        void Update(float delta_time);
    
        /**
     * @brief Handle mouse input for menu interaction
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param clicked Whether mouse was clicked
     * @param research_tree Reference to the research tree singleton
     * @return True if a node was unlocked
     */
        bool HandleMouse(int mouse_x, int mouse_y, bool clicked, 
                         TowerForge::Core::ResearchTree& research_tree) const;
    
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
        void SetNotificationCenter(NotificationCenter* notification_center) { 
            notification_center_ = notification_center; 
        }
        
        /**
         * @brief Process mouse events for confirmation dialogs
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event) const;
    
    private:
        static void RenderOverlay();

        static void RenderHeader(const TowerForge::Core::ResearchTree& research_tree);
        void RenderTreeGrid(const TowerForge::Core::ResearchTree& research_tree);

        static void RenderNode(const TowerForge::Core::ResearchNode& node, int x, int y, bool hovered);

        static void RenderNodeDetails(const TowerForge::Core::ResearchNode& node);
    
        bool visible_;
        float animation_time_;
        int hovered_node_index_;  // Index of currently hovered node (-1 if none)
        NotificationCenter* notification_center_;
        
        // Confirmation dialog for research unlocks (mutable because they change during const HandleMouse)
        mutable std::unique_ptr<ConfirmationDialog> unlock_confirmation_;
        mutable std::string pending_unlock_node_id_;
        mutable TowerForge::Core::ResearchTree* pending_unlock_tree_;
    
        // Menu layout constants
        static constexpr int MENU_WIDTH = 800;
        static constexpr int MENU_HEIGHT = 600;
        static constexpr int HEADER_HEIGHT = 80;
        static constexpr int NODE_SIZE = 100;
        static constexpr int NODE_SPACING = 20;
        static constexpr int GRID_START_X = 100;
        static constexpr int GRID_START_Y = 150;
        static constexpr int DETAILS_PANEL_WIDTH = 300;
    };

}
