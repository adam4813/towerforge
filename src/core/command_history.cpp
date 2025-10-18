#include "core/command_history.hpp"

namespace TowerForge::Core {

    CommandHistory::CommandHistory(size_t max_history_size)
        : max_history_size_(max_history_size) {
    }

    bool CommandHistory::ExecuteCommand(std::unique_ptr<ICommand> command, float& available_funds) {
        if (!command) {
            return false;
        }

        const int cost_change = command->GetCostChange();
        
        // Check if we have enough funds for the operation
        if (cost_change < 0 && available_funds < static_cast<float>(-cost_change)) {
            return false;  // Not enough funds
        }

        // Execute the command
        if (!command->Execute()) {
            return false;
        }

        // Adjust funds
        available_funds += cost_change;

        // Add to undo stack
        undo_stack_.emplace_back(std::move(command));

        // Limit stack size
        if (undo_stack_.size() > max_history_size_) {
            undo_stack_.erase(undo_stack_.begin());
        }

        // Clear redo stack when new command is executed
        redo_stack_.clear();

        return true;
    }

    bool CommandHistory::Undo(float& available_funds) {
        if (undo_stack_.empty()) {
            return false;
        }

        // Get the last command
        auto& entry = undo_stack_.back();
        
        // Reverse the cost change (if we paid, we get refund; if we got refund, we pay back)
        const int reverse_cost = -entry.cost_change;
        
        // Check if we have enough funds to undo (important for demolish undo)
        if (reverse_cost < 0 && available_funds < static_cast<float>(-reverse_cost)) {
            return false;  // Not enough funds to undo
        }

        // Undo the command
        if (!entry.command->Undo()) {
            return false;
        }

        // Adjust funds
        available_funds += reverse_cost;

        // Move to redo stack
        redo_stack_.push_back(std::move(entry));
        undo_stack_.pop_back();

        // Limit redo stack size
        if (redo_stack_.size() > max_history_size_) {
            redo_stack_.erase(redo_stack_.begin());
        }

        return true;
    }

    bool CommandHistory::Redo(float& available_funds) {
        if (redo_stack_.empty()) {
            return false;
        }

        // Get the last undone command
        auto& entry = redo_stack_.back();
        
        const int cost_change = entry.cost_change;
        
        // Check if we have enough funds
        if (cost_change < 0 && available_funds < static_cast<float>(-cost_change)) {
            return false;  // Not enough funds
        }

        // Re-execute the command
        if (!entry.command->Execute()) {
            return false;
        }

        // Adjust funds
        available_funds += cost_change;

        // Move back to undo stack
        undo_stack_.push_back(std::move(entry));
        redo_stack_.pop_back();

        return true;
    }

    void CommandHistory::Clear() {
        undo_stack_.clear();
        redo_stack_.clear();
    }

}
