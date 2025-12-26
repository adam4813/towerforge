#pragma once

#include "ui/ui_theme.h"
#include "ui/mouse_interface.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Menu options available in the pause menu
     */
    enum class PauseMenuOption {
        Resume,
        SaveGame,
        LoadGame,
        Settings,
        Mods,
        QuitToTitle
    };

    /**
     * @brief Pause menu class for in-game menu overlay
     * 
     * Displays when ESC is pressed during gameplay, pausing the simulation
     * and providing options to save, load, change settings, or quit.
     * Uses citrus engine UI elements for rendering.
     */
    class PauseMenu {
    public:
        using OptionCallback = std::function<void(PauseMenuOption)>;
        using QuitCallback = std::function<void()>;

        PauseMenu();

        ~PauseMenu();

        /**
         * @brief Initialize the menu components
         */
        void Initialize();

        /**
         * @brief Shutdown and cleanup resources
         */
        void Shutdown();

        /**
         * @brief Render the pause menu overlay
         */
        void Render() const;

        /**
         * @brief Update menu state (called every frame when active)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Process mouse events
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent &event) const;

        /**
         * @brief Handle keyboard input for menu navigation
         */
        void HandleKeyboard() const;

        /**
         * @brief Set callback for option selection
         */
        void SetOptionCallback(const OptionCallback &callback) { option_callback_ = callback; }

        /**
         * @brief Set callback for quit confirmation result
         */
        void SetQuitConfirmationCallback(const QuitCallback &callback) { quit_confirmation_callback_ = callback; }

        /**
         * @brief Show/hide confirmation dialog for quitting
         * @param show Whether to show the confirmation dialog
         */
        void ShowQuitConfirmation(const bool show) { show_quit_confirmation_ = show; }

        /**
         * @brief Check if quit confirmation is showing
         * @return True if confirmation dialog is visible
         */
        bool IsQuitConfirmationShowing() const { return show_quit_confirmation_; }

        /**
         * @brief Handle input for quit confirmation dialog
         * @return 1 for confirm, 0 for cancel, -1 for no action
         */
        int HandleQuitConfirmation() const;

    private:
        void UpdateLayout();

        void RenderDimOverlay() const;

        void RenderIndicator() const;

        void RenderQuitConfirmation() const;

        mutable int selected_option_;
        float animation_time_;
        mutable bool show_quit_confirmation_;
        mutable int quit_confirmation_selection_;
        int last_screen_width_;
        int last_screen_height_;
        OptionCallback option_callback_;
        QuitCallback quit_confirmation_callback_;

        // Menu layout constants
        static constexpr int MENU_ITEM_HEIGHT = 50;
        static constexpr int MENU_ITEM_SPACING = 10;
        static constexpr int MENU_WIDTH = 350;
        static constexpr int HEADER_HEIGHT = 100;

        // Menu options
        struct MenuItem {
            std::string label;
            PauseMenuOption option;
        };

        std::vector<MenuItem> menu_items_;

        // Engine UI panel and buttons
        std::unique_ptr<engine::ui::elements::Panel> pause_panel_;
        std::vector<engine::ui::elements::Button *> menu_item_buttons_;

        // Quit confirmation dialog
        std::unique_ptr<engine::ui::elements::Panel> confirmation_panel_;
        engine::ui::elements::Button *cancel_button_ = nullptr;
        engine::ui::elements::Button *confirm_button_ = nullptr;

        void BuildConfirmationDialog();

        void UpdateConfirmationLayout();

        void UpdateConfirmationButtonStyles() const;
    };
}
