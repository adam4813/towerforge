#include "ui/save_load_menu.h"
#include <cstring>
#include <algorithm>

namespace towerforge {
namespace ui {

SaveLoadMenu::SaveLoadMenu()
    : state_(SaveLoadMenuState::Closed),
      is_save_mode_(false),
      selected_slot_index_(-1),
      save_load_manager_(nullptr),
      last_action_(SaveLoadAction::None),
      animation_time_(0.0f) {
    std::memset(rename_buffer_, 0, sizeof(rename_buffer_));
    std::memset(new_save_name_buffer_, 0, sizeof(new_save_name_buffer_));
    std::memset(tower_name_buffer_, 0, sizeof(tower_name_buffer_));
}

SaveLoadMenu::~SaveLoadMenu() {
}

void SaveLoadMenu::Open(bool is_save_mode) {
    is_save_mode_ = is_save_mode;
    state_ = SaveLoadMenuState::Viewing;
    selected_slot_index_ = -1;
    last_action_ = SaveLoadAction::None;
    error_message_ = "";
    RefreshSlots();
}

void SaveLoadMenu::Close() {
    state_ = SaveLoadMenuState::Closed;
    last_action_ = SaveLoadAction::Cancel;
}

void SaveLoadMenu::SetSaveLoadManager(TowerForge::Core::SaveLoadManager* manager) {
    save_load_manager_ = manager;
}

void SaveLoadMenu::SetError(const std::string& message) {
    error_message_ = message;
    state_ = SaveLoadMenuState::Error;
}

void SaveLoadMenu::ClearError() {
    error_message_ = "";
    if (state_ == SaveLoadMenuState::Error) {
        state_ = SaveLoadMenuState::Viewing;
    }
}

std::string SaveLoadMenu::GetSelectedSlotName() const {
    if (selected_slot_index_ >= 0 && selected_slot_index_ < static_cast<int>(slots_.size())) {
        return slots_[selected_slot_index_].slot_name;
    }
    return "";
}

void SaveLoadMenu::RefreshSlots() {
    if (save_load_manager_) {
        slots_ = save_load_manager_->GetSaveSlots();
        
        // Sort by date (most recent first)
        std::sort(slots_.begin(), slots_.end(), [](const auto& a, const auto& b) {
            return a.save_date > b.save_date;
        });
    }
}

void SaveLoadMenu::Update(float delta_time) {
    if (state_ == SaveLoadMenuState::Closed) {
        return;
    }
    
    animation_time_ += delta_time;
}

void SaveLoadMenu::HandleKeyboard() {
    if (state_ == SaveLoadMenuState::Closed) {
        return;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state_ == SaveLoadMenuState::Viewing) {
            Close();
        } else {
            // Cancel any dialogs
            state_ = SaveLoadMenuState::Viewing;
        }
    }
    
    if (state_ == SaveLoadMenuState::Viewing) {
        if (IsKeyPressed(KEY_UP)) {
            selected_slot_index_ = std::max(-1, selected_slot_index_ - 1);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selected_slot_index_ = std::min(static_cast<int>(slots_.size()) - 1, 
                                           selected_slot_index_ + 1);
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (selected_slot_index_ >= 0 && selected_slot_index_ < static_cast<int>(slots_.size())) {
                if (is_save_mode_) {
                    last_action_ = SaveLoadAction::Save;
                } else {
                    state_ = SaveLoadMenuState::ConfirmLoad;
                }
            }
        }
        
        if (IsKeyPressed(KEY_DELETE)) {
            if (selected_slot_index_ >= 0 && selected_slot_index_ < static_cast<int>(slots_.size())) {
                state_ = SaveLoadMenuState::ConfirmDelete;
            }
        }
    }
}

void SaveLoadMenu::HandleMouse(int mouse_x, int mouse_y, bool clicked) {
    if (state_ == SaveLoadMenuState::Closed) {
        return;
    }
    
    // Handle based on current state
    // This will be expanded with button click detection
}

void SaveLoadMenu::Render() {
    if (state_ == SaveLoadMenuState::Closed) {
        return;
    }
    
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    // Semi-transparent background overlay
    DrawRectangle(0, 0, screen_width, screen_height, Fade(BLACK, 0.7f));
    
    // Calculate menu position (centered)
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    // Draw main menu panel
    DrawRectangle(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, DARKGRAY);
    DrawRectangleLines(menu_x, menu_y, MENU_WIDTH, MENU_HEIGHT, WHITE);
    
    // Draw header
    const char* title = is_save_mode_ ? "SAVE GAME" : "LOAD GAME";
    DrawText(title, menu_x + 20, menu_y + 15, 24, WHITE);
    DrawLine(menu_x, menu_y + HEADER_HEIGHT, menu_x + MENU_WIDTH, menu_y + HEADER_HEIGHT, WHITE);
    
    // Render different parts based on state
    if (state_ == SaveLoadMenuState::Error) {
        RenderErrorDialog();
    } else if (state_ == SaveLoadMenuState::ConfirmDelete) {
        RenderConfirmDialog();
    } else if (state_ == SaveLoadMenuState::ConfirmLoad) {
        RenderConfirmDialog();
    } else if (state_ == SaveLoadMenuState::Renaming) {
        RenderRenameDialog();
    } else {
        RenderSlotList();
        RenderActionButtons();
        RenderStatusBar();
    }
}

void SaveLoadMenu::RenderSlotList() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    int list_y = menu_y + HEADER_HEIGHT + 10;
    int list_height = MENU_HEIGHT - HEADER_HEIGHT - STATUS_BAR_HEIGHT - 80 - 20; // Leave room for buttons
    
    // Draw section header
    DrawText("SAVE SLOTS", menu_x + 20, list_y, 16, LIGHTGRAY);
    list_y += 25;
    
    // Draw slots
    int slot_count = std::min(static_cast<int>(slots_.size()), 5);  // Show max 5 slots
    for (int i = 0; i < slot_count; i++) {
        const auto& slot = slots_[i];
        int slot_y = list_y + i * (SLOT_HEIGHT + SLOT_SPACING);
        
        Color bg_color = (i == selected_slot_index_) ? GRAY : DARKGRAY;
        Color text_color = (i == selected_slot_index_) ? YELLOW : WHITE;
        
        // Draw slot background
        DrawRectangle(menu_x + 20, slot_y, MENU_WIDTH - 40, SLOT_HEIGHT, bg_color);
        DrawRectangleLines(menu_x + 20, slot_y, MENU_WIDTH - 40, SLOT_HEIGHT, text_color);
        
        // Draw slot name
        DrawText(slot.slot_name.c_str(), menu_x + 30, slot_y + 5, 18, text_color);
        
        // Draw tower name
        std::string info_line = "Tower: " + slot.tower_name;
        DrawText(info_line.c_str(), menu_x + 30, slot_y + 25, 14, LIGHTGRAY);
        
        // Draw save date and balance
        std::string detail_line = slot.save_date + "  |  $" + 
                                 std::to_string(static_cast<int>(slot.total_balance));
        DrawText(detail_line.c_str(), menu_x + 30, slot_y + 42, 12, LIGHTGRAY);
    }
    
    // Show "New Slot" option if in save mode
    if (is_save_mode_) {
        int new_slot_y = list_y + slot_count * (SLOT_HEIGHT + SLOT_SPACING);
        Color bg_color = (selected_slot_index_ == slot_count) ? GRAY : DARKGRAY;
        Color text_color = (selected_slot_index_ == slot_count) ? YELLOW : GREEN;
        
        DrawRectangle(menu_x + 20, new_slot_y, MENU_WIDTH - 40, SLOT_HEIGHT, bg_color);
        DrawRectangleLines(menu_x + 20, new_slot_y, MENU_WIDTH - 40, SLOT_HEIGHT, text_color);
        DrawText("[New Slot]", menu_x + 30, new_slot_y + 20, 18, text_color);
    }
    
    if (slots_.empty() && !is_save_mode_) {
        DrawText("No save files found", menu_x + MENU_WIDTH / 2 - 70, 
                menu_y + MENU_HEIGHT / 2, 16, GRAY);
    }
}

void SaveLoadMenu::RenderActionButtons() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    int button_y = menu_y + MENU_HEIGHT - STATUS_BAR_HEIGHT - BUTTON_HEIGHT - 15;
    int button_x = menu_x + 20;
    
    // Save/Load button
    const char* main_action = is_save_mode_ ? "Save" : "Load";
    bool can_act = selected_slot_index_ >= 0;
    
    Color button_color = can_act ? DARKGREEN : DARKGRAY;
    DrawRectangle(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, button_color);
    DrawRectangleLines(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    DrawText(main_action, button_x + 25, button_y + 10, 16, WHITE);
    button_x += BUTTON_WIDTH + BUTTON_SPACING;
    
    // Delete button
    bool can_delete = selected_slot_index_ >= 0 && selected_slot_index_ < static_cast<int>(slots_.size());
    button_color = can_delete ? DARKBLUE : DARKGRAY;
    DrawRectangle(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, button_color);
    DrawRectangleLines(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    DrawText("Delete", button_x + 20, button_y + 10, 16, WHITE);
    button_x += BUTTON_WIDTH + BUTTON_SPACING;
    
    // Rename button
    bool can_rename = can_delete;
    button_color = can_rename ? DARKBLUE : DARKGRAY;
    DrawRectangle(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, button_color);
    DrawRectangleLines(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    DrawText("Rename", button_x + 17, button_y + 10, 16, WHITE);
    button_x += BUTTON_WIDTH + BUTTON_SPACING;
    
    // Cancel button
    DrawRectangle(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, MAROON);
    DrawRectangleLines(button_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT, WHITE);
    DrawText("Cancel", button_x + 17, button_y + 10, 16, WHITE);
}

void SaveLoadMenu::RenderStatusBar() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int menu_x = (screen_width - MENU_WIDTH) / 2;
    int menu_y = (screen_height - MENU_HEIGHT) / 2;
    
    int status_y = menu_y + MENU_HEIGHT - STATUS_BAR_HEIGHT;
    
    DrawRectangle(menu_x, status_y, MENU_WIDTH, STATUS_BAR_HEIGHT, BLACK);
    DrawLine(menu_x, status_y, menu_x + MENU_WIDTH, status_y, WHITE);
    
    // Status text
    std::string status_text = "Autosave: ";
    if (save_load_manager_ && save_load_manager_->IsAutosaveEnabled()) {
        status_text += "ON";
        
        float time_since_save = save_load_manager_->GetTimeSinceLastSave();
        int minutes = static_cast<int>(time_since_save / 60.0f);
        int seconds = static_cast<int>(time_since_save) % 60;
        
        status_text += " | Last saved: " + std::to_string(minutes) + "m " + 
                      std::to_string(seconds) + "s ago";
    } else {
        status_text += "OFF";
    }
    
    if (!error_message_.empty()) {
        status_text += " | Error: " + error_message_;
    } else {
        status_text += " | Error: None";
    }
    
    DrawText(status_text.c_str(), menu_x + 10, status_y + 12, 14, LIGHTGRAY);
}

void SaveLoadMenu::RenderConfirmDialog() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int dialog_width = 400;
    int dialog_height = 150;
    int dialog_x = (screen_width - dialog_width) / 2;
    int dialog_y = (screen_height - dialog_height) / 2;
    
    DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, DARKGRAY);
    DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height, WHITE);
    
    const char* message;
    if (state_ == SaveLoadMenuState::ConfirmDelete) {
        message = "Delete this save file?";
    } else {
        message = "Load this save file?";
    }
    
    DrawText(message, dialog_x + 80, dialog_y + 40, 20, WHITE);
    
    if (selected_slot_index_ >= 0 && selected_slot_index_ < static_cast<int>(slots_.size())) {
        DrawText(slots_[selected_slot_index_].slot_name.c_str(), 
                dialog_x + 100, dialog_y + 70, 16, YELLOW);
    }
    
    // Yes button
    int yes_x = dialog_x + 80;
    int yes_y = dialog_y + 100;
    DrawRectangle(yes_x, yes_y, 100, 35, DARKGREEN);
    DrawRectangleLines(yes_x, yes_y, 100, 35, WHITE);
    DrawText("Yes", yes_x + 35, yes_y + 10, 16, WHITE);
    
    // No button
    int no_x = dialog_x + 220;
    int no_y = dialog_y + 100;
    DrawRectangle(no_x, no_y, 100, 35, MAROON);
    DrawRectangleLines(no_x, no_y, 100, 35, WHITE);
    DrawText("No", no_x + 37, no_y + 10, 16, WHITE);
}

void SaveLoadMenu::RenderRenameDialog() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int dialog_width = 400;
    int dialog_height = 180;
    int dialog_x = (screen_width - dialog_width) / 2;
    int dialog_y = (screen_height - dialog_height) / 2;
    
    DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, DARKGRAY);
    DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height, WHITE);
    
    DrawText("Rename Save File", dialog_x + 110, dialog_y + 20, 20, WHITE);
    DrawText("New name:", dialog_x + 30, dialog_y + 60, 16, LIGHTGRAY);
    
    // Text input box
    int input_x = dialog_x + 30;
    int input_y = dialog_y + 85;
    DrawRectangle(input_x, input_y, 340, 30, BLACK);
    DrawRectangleLines(input_x, input_y, 340, 30, WHITE);
    DrawText(rename_buffer_, input_x + 5, input_y + 7, 16, WHITE);
    
    // OK button
    int ok_x = dialog_x + 80;
    int ok_y = dialog_y + 130;
    DrawRectangle(ok_x, ok_y, 100, 35, DARKGREEN);
    DrawRectangleLines(ok_x, ok_y, 100, 35, WHITE);
    DrawText("OK", ok_x + 37, ok_y + 10, 16, WHITE);
    
    // Cancel button
    int cancel_x = dialog_x + 220;
    int cancel_y = dialog_y + 130;
    DrawRectangle(cancel_x, cancel_y, 100, 35, MAROON);
    DrawRectangleLines(cancel_x, cancel_y, 100, 35, WHITE);
    DrawText("Cancel", cancel_x + 22, cancel_y + 10, 16, WHITE);
}

void SaveLoadMenu::RenderErrorDialog() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int dialog_width = 450;
    int dialog_height = 200;
    int dialog_x = (screen_width - dialog_width) / 2;
    int dialog_y = (screen_height - dialog_height) / 2;
    
    DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, DARKGRAY);
    DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height, RED);
    
    DrawText("ERROR", dialog_x + 180, dialog_y + 20, 24, RED);
    
    // Word wrap the error message
    const char* msg = error_message_.c_str();
    DrawText(msg, dialog_x + 30, dialog_y + 70, 16, WHITE);
    
    // OK button
    int ok_x = dialog_x + 175;
    int ok_y = dialog_y + 140;
    DrawRectangle(ok_x, ok_y, 100, 35, DARKBLUE);
    DrawRectangleLines(ok_x, ok_y, 100, 35, WHITE);
    DrawText("OK", ok_x + 37, ok_y + 10, 16, WHITE);
}

} // namespace ui
} // namespace towerforge
