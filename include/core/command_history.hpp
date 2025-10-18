#pragma once

#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include "core/command.hpp"

namespace TowerForge::Core {

    /**
     * @brief Entry in the command history with timestamp and description
     */
    struct HistoryEntry {
        std::unique_ptr<ICommand> command;
        std::string description;
        std::chrono::system_clock::time_point timestamp;
        int cost_change;  // Negative for cost, positive for refund

        HistoryEntry(std::unique_ptr<ICommand> cmd)
            : command(std::move(cmd))
            , description(command->GetDescription())
            , timestamp(std::chrono::system_clock::now())
            , cost_change(command->GetCostChange()) {
        }
    };

    /**
     * @brief Manages command history for undo/redo operations
     * 
     * Maintains two stacks: undo stack and redo stack.
     * When a new command is executed, it's added to the undo stack and redo stack is cleared.
     */
    class CommandHistory {
    public:
        explicit CommandHistory(size_t max_history_size = 50);

        /**
         * @brief Execute a command and add it to history
         * @param command The command to execute
         * @param available_funds Current available funds (for validation)
         * @return true if command executed successfully, false otherwise
         */
        bool ExecuteCommand(std::unique_ptr<ICommand> command, float& available_funds);

        /**
         * @brief Undo the last command
         * @param available_funds Current available funds (will be adjusted)
         * @return true if undo succeeded, false if nothing to undo
         */
        bool Undo(float& available_funds);

        /**
         * @brief Redo the last undone command
         * @param available_funds Current available funds (for validation and adjustment)
         * @return true if redo succeeded, false if nothing to redo
         */
        bool Redo(float& available_funds);

        /**
         * @brief Check if undo is available
         */
        bool CanUndo() const { return !undo_stack_.empty(); }

        /**
         * @brief Check if redo is available
         */
        bool CanRedo() const { return !redo_stack_.empty(); }

        /**
         * @brief Get the undo stack (for UI display)
         */
        const std::vector<HistoryEntry>& GetUndoStack() const { return undo_stack_; }

        /**
         * @brief Get the redo stack (for UI display)
         */
        const std::vector<HistoryEntry>& GetRedoStack() const { return redo_stack_; }

        /**
         * @brief Clear all history
         */
        void Clear();

        /**
         * @brief Get the maximum history size
         */
        size_t GetMaxHistorySize() const { return max_history_size_; }

    private:
        std::vector<HistoryEntry> undo_stack_;
        std::vector<HistoryEntry> redo_stack_;
        size_t max_history_size_;
    };

}
