#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include "core/save_load_manager.hpp"

namespace towerforge::core { class ECSWorld; }

namespace towerforge::ui {

    // Forward declaration
    struct MouseEvent;

    /**
 * @brief State of the save/load menu
 */
    enum class SaveLoadMenuState {
        Closed,
        Viewing,
        ConfirmDelete,
        ConfirmLoad,
        Renaming,
        Error
    };

    /**
 * @brief Action requested from the menu
 */
    enum class SaveLoadAction {
        None,
        Save,
        Load,
        Delete,
        Rename,
        Cancel
    };

    /**
 * @brief Save/Load menu matching the mockup design
 * 
 * Provides UI for managing save slots with list view, actions,
 * and status/error display.
 */
    class SaveLoadMenu {
    public:
        SaveLoadMenu();
        ~SaveLoadMenu();
    
        /**
     * @brief Render the save/load menu
     */
        void Render() const;
    
        /**
         * @brief Update menu state (called every frame)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);
    
        /**
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event) const;

        /**
         * @brief Handle keyboard input
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        void HandleKeyboard();
    
        /**
         * @brief Handle mouse input
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         * @deprecated Use ProcessMouseEvent instead
         */
        void HandleMouse(int mouse_x, int mouse_y, bool clicked) const;
    
        /**
     * @brief Open the menu
     * @param is_save_mode True to open in save mode, false for load mode
     */
        void Open(bool is_save_mode);
    
        /**
     * @brief Close the menu
     */
        void Close();
    
        /**
     * @brief Check if menu is open
     */
        bool IsOpen() const { return state_ != SaveLoadMenuState::Closed; }
    
        /**
     * @brief Set the save/load manager
     */
        void SetSaveLoadManager(towerforge::core::SaveLoadManager* manager);
    
        /**
     * @brief Set error message to display
     */
        void SetError(const std::string& message);
    
        /**
     * @brief Clear error message
     */
        void ClearError();
    
        /**
     * @brief Get the last action selected
     */
        SaveLoadAction GetLastAction() const { return last_action_; }
    
        /**
     * @brief Get the selected slot name
     */
        std::string GetSelectedSlotName() const;
    
        /**
     * @brief Refresh the slot list from the manager
     */
        void RefreshSlots();

    private:
        void RenderSlotList() const;
        void RenderActionButtons() const;
        void RenderStatusBar() const;
        void RenderConfirmDialog() const;
        void RenderRenameDialog() const;
        void RenderErrorDialog() const;
    
        SaveLoadMenuState state_;
        bool is_save_mode_;  // true for save, false for load
        int selected_slot_index_;
        std::vector<towerforge::core::SaveSlotInfo> slots_;
        towerforge::core::SaveLoadManager* save_load_manager_;
        std::string error_message_;
        SaveLoadAction last_action_;
    
        // For renaming
        char rename_buffer_[65]{};
    
        // For new save
        char new_save_name_buffer_[65]{};
        char tower_name_buffer_[65]{};
    
        // Layout constants
        static constexpr int MENU_WIDTH = 600;
        static constexpr int MENU_HEIGHT = 500;
        static constexpr int SLOT_HEIGHT = 60;
        static constexpr int SLOT_SPACING = 5;
        static constexpr int BUTTON_WIDTH = 100;
        static constexpr int BUTTON_HEIGHT = 35;
        static constexpr int BUTTON_SPACING = 10;
        static constexpr int STATUS_BAR_HEIGHT = 40;
        static constexpr int HEADER_HEIGHT = 50;
    
        float animation_time_;
    };

}
