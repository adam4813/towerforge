#pragma once

#include <string>
#include <memory>
#include <functional>
#include "core/components.hpp"

import engine;

namespace towerforge::ui {
    /**
     * @brief Details panel for the research tree menu
     * 
     * Displays information about a selected research node including
     * name, status, cost, description, effect, and an unlock button
     * for upgradable nodes.
     */
    class ResearchDetailsPanel {
    public:
        /**
         * @brief Callback type for unlock button click
         * @param node_id The ID of the node to unlock
         */
        using UnlockCallback = std::function<void(const std::string &node_id)>;

        /**
         * @brief Constructor
         * @param width Panel width
         * @param height Panel height
         */
        ResearchDetailsPanel(float width, float height);

        ~ResearchDetailsPanel();

        /**
         * @brief Build the panel UI components
         */
        void Initialize();

        /**
         * @brief Get the root panel element for adding to parent
         */
        engine::ui::elements::Panel *GetPanel() const { return panel_.get(); }

        /**
         * @brief Transfer ownership of the panel
         */
        std::unique_ptr<engine::ui::elements::Panel> TakePanel() { return std::move(panel_); }

        /**
         * @brief Update the panel to display a node's information
         * @param node The research node to display (nullptr to clear)
         */
        void SetNode(const core::ResearchNode *node);

        /**
         * @brief Set the unlock button callback
         */
        void SetUnlockCallback(UnlockCallback callback) { unlock_callback_ = std::move(callback); }

        /**
         * @brief Clear the panel (show "Select a node" message)
         */
        void Clear();

        /**
         * @brief Get the currently displayed node ID
         */
        const std::string &GetSelectedNodeId() const { return selected_node_id_; }

    private:
        float width_;
        float height_;
        std::string selected_node_id_;
        UnlockCallback unlock_callback_;

        // UI components
        std::unique_ptr<engine::ui::elements::Panel> panel_;
        engine::ui::elements::Text *name_text_;
        engine::ui::elements::Text *status_text_;
        engine::ui::elements::Text *cost_text_;
        engine::ui::elements::Text *description_text_;
        engine::ui::elements::Text *effect_text_;
        engine::ui::elements::Text *hint_text_;
        engine::ui::elements::Button *unlock_button_;
    };
}
