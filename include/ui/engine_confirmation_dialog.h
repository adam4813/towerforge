#pragma once

#include <string>
#include <memory>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Engine-based confirmation dialog for modal confirmations
     * 
     * A modal dialog that displays a title, message, and confirm/cancel buttons.
     * Uses the citrus engine UI components for consistent styling.
     */
    class EngineConfirmationDialog {
    public:
        using ConfirmCallback = std::function<void()>;
        using CancelCallback = std::function<void()>;

        /**
         * @brief Construct a confirmation dialog
         * @param title Dialog title
         * @param message Dialog message
         * @param confirm_text Text for confirm button (default: "Confirm")
         * @param cancel_text Text for cancel button (default: "Cancel")
         */
        EngineConfirmationDialog(const std::string &title,
                                 const std::string &message,
                                 const std::string &confirm_text = "Confirm",
                                 const std::string &cancel_text = "Cancel");

        ~EngineConfirmationDialog();

        /**
         * @brief Build the dialog UI components
         */
        void Initialize();

        /**
         * @brief Render the dialog
         */
        void Render() const;

        /**
         * @brief Update dialog state
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Process mouse events
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Set confirm callback
         */
        void SetConfirmCallback(ConfirmCallback callback) { confirm_callback_ = std::move(callback); }

        /**
         * @brief Set cancel callback
         */
        void SetCancelCallback(CancelCallback callback) { cancel_callback_ = std::move(callback); }

        /**
         * @brief Check if dialog is visible
         */
        bool IsVisible() const { return visible_; }

        /**
         * @brief Show the dialog
         */
        void Show();

        /**
         * @brief Hide the dialog
         */
        void Hide();

        /**
         * @brief Update message text
         */
        void SetMessage(const std::string &message);

        /**
         * @brief Update title text
         */
        void SetTitle(const std::string &title);

    private:
        void UpdateLayout();

        std::string title_;
        std::string message_;
        std::string confirm_text_;
        std::string cancel_text_;

        bool visible_;
        float animation_time_;
        int last_screen_width_;
        int last_screen_height_;

        // UI components
        std::unique_ptr<engine::ui::elements::Panel> panel_;
        engine::ui::elements::Text *title_text_;
        engine::ui::elements::Text *message_text_;
        engine::ui::elements::Button *confirm_button_;
        engine::ui::elements::Button *cancel_button_;

        ConfirmCallback confirm_callback_;
        CancelCallback cancel_callback_;

        // Layout constants
        static constexpr int DIALOG_WIDTH = 420;
        static constexpr int DIALOG_HEIGHT = 200;
        static constexpr int BUTTON_WIDTH = 130;
        static constexpr int BUTTON_HEIGHT = 40;
    };
}
