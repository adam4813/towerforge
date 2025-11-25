#include <gtest/gtest.h>
#include "core/command_history.hpp"
#include "core/command.hpp"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"
#include <flecs.h>

using namespace towerforge::core;

// Mock command for testing
class MockCommand : public ICommand {
public:
    MockCommand(bool will_succeed, int cost, const std::string& desc)
        : will_succeed_(will_succeed), cost_(cost), description_(desc), executed_(false), undone_(false) {}

    bool Execute() override {
        executed_ = true;
        return will_succeed_;
    }

    bool Undo() override {
        undone_ = true;
        return will_succeed_;
    }

    std::string GetDescription() const override {
        return description_;
    }

    int GetCostChange() const override {
        return cost_;
    }

    bool WasExecuted() const { return executed_; }
    bool WasUndone() const { return undone_; }

private:
    bool will_succeed_;
    int cost_;
    std::string description_;
    bool executed_;
    bool undone_;
};

// Unit tests for CommandHistory
// These tests verify undo/redo functionality and command stack management

class CommandHistoryUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
        history = std::make_unique<CommandHistory>(10);
        funds = 1000.0f;
    }

    std::unique_ptr<CommandHistory> history;
    float funds;
};

TEST_F(CommandHistoryUnitTest, InitialState) {
    EXPECT_FALSE(history->CanUndo());
    EXPECT_FALSE(history->CanRedo());
    EXPECT_EQ(history->GetUndoStack().size(), 0);
    EXPECT_EQ(history->GetRedoStack().size(), 0);
}

TEST_F(CommandHistoryUnitTest, ExecuteCommand) {
    auto cmd = std::make_unique<MockCommand>(true, -100, "Test Command");
    
    bool result = history->ExecuteCommand(std::move(cmd), funds);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(history->CanUndo());
    EXPECT_FALSE(history->CanRedo());
    EXPECT_EQ(history->GetUndoStack().size(), 1);
    EXPECT_FLOAT_EQ(funds, 900.0f);
}

TEST_F(CommandHistoryUnitTest, ExecuteFailedCommand) {
    auto cmd = std::make_unique<MockCommand>(false, -100, "Failed Command");
    
    bool result = history->ExecuteCommand(std::move(cmd), funds);
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(history->CanUndo());
    EXPECT_FLOAT_EQ(funds, 1000.0f);  // Funds unchanged
}

TEST_F(CommandHistoryUnitTest, UndoCommand) {
    auto cmd = std::make_unique<MockCommand>(true, -100, "Test Command");
    history->ExecuteCommand(std::move(cmd), funds);
    
    ASSERT_TRUE(history->CanUndo());
    EXPECT_FLOAT_EQ(funds, 900.0f);
    
    bool undo_result = history->Undo(funds);
    
    EXPECT_TRUE(undo_result);
    EXPECT_FALSE(history->CanUndo());
    EXPECT_TRUE(history->CanRedo());
    EXPECT_FLOAT_EQ(funds, 1000.0f);  // Refunded
}

TEST_F(CommandHistoryUnitTest, RedoCommand) {
    auto cmd = std::make_unique<MockCommand>(true, -100, "Test Command");
    history->ExecuteCommand(std::move(cmd), funds);
    history->Undo(funds);
    
    ASSERT_TRUE(history->CanRedo());
    EXPECT_FLOAT_EQ(funds, 1000.0f);
    
    bool redo_result = history->Redo(funds);
    
    EXPECT_TRUE(redo_result);
    EXPECT_TRUE(history->CanUndo());
    EXPECT_FALSE(history->CanRedo());
    EXPECT_FLOAT_EQ(funds, 900.0f);
}

TEST_F(CommandHistoryUnitTest, MultipleUndoRedo) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -100, "Cmd 1"), funds);
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -200, "Cmd 2"), funds);
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -150, "Cmd 3"), funds);
    
    EXPECT_FLOAT_EQ(funds, 550.0f);
    EXPECT_EQ(history->GetUndoStack().size(), 3);
    
    // Undo all
    history->Undo(funds);  // 550 + 150 = 700
    history->Undo(funds);  // 700 + 200 = 900
    history->Undo(funds);  // 900 + 100 = 1000
    
    EXPECT_FLOAT_EQ(funds, 1000.0f);
    EXPECT_FALSE(history->CanUndo());
    EXPECT_TRUE(history->CanRedo());
    EXPECT_EQ(history->GetRedoStack().size(), 3);
    
    // Redo all
    history->Redo(funds);  // 1000 - 100 = 900
    history->Redo(funds);  // 900 - 200 = 700
    history->Redo(funds);  // 700 - 150 = 550
    
    EXPECT_FLOAT_EQ(funds, 550.0f);
    EXPECT_TRUE(history->CanUndo());
    EXPECT_FALSE(history->CanRedo());
}

TEST_F(CommandHistoryUnitTest, NewCommandClearsRedoStack) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -100, "Cmd 1"), funds);
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -200, "Cmd 2"), funds);
    
    history->Undo(funds);
    
    EXPECT_TRUE(history->CanRedo());
    EXPECT_EQ(history->GetRedoStack().size(), 1);
    
    // Execute new command - should clear redo stack
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -150, "Cmd 3"), funds);
    
    EXPECT_FALSE(history->CanRedo());
    EXPECT_EQ(history->GetRedoStack().size(), 0);
}

TEST_F(CommandHistoryUnitTest, MaxHistorySizeEnforced) {
    auto small_history = std::make_unique<CommandHistory>(3);
    float small_funds = 1000.0f;
    
    // Add 5 commands (exceeds max of 3)
    for (int i = 0; i < 5; ++i) {
        small_history->ExecuteCommand(
            std::make_unique<MockCommand>(true, -10, "Cmd"), 
            small_funds
        );
    }
    
    // Should only keep last 3
    EXPECT_EQ(small_history->GetUndoStack().size(), 3);
}

TEST_F(CommandHistoryUnitTest, ClearHistory) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -100, "Cmd 1"), funds);
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -200, "Cmd 2"), funds);
    history->Undo(funds);
    
    EXPECT_TRUE(history->CanUndo());
    EXPECT_TRUE(history->CanRedo());
    
    history->Clear();
    
    EXPECT_FALSE(history->CanUndo());
    EXPECT_FALSE(history->CanRedo());
    EXPECT_EQ(history->GetUndoStack().size(), 0);
    EXPECT_EQ(history->GetRedoStack().size(), 0);
}

TEST_F(CommandHistoryUnitTest, InsufficientFundsForRedo) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -500, "Expensive Cmd"), funds);
    EXPECT_FLOAT_EQ(funds, 500.0f);
    
    history->Undo(funds);
    EXPECT_FLOAT_EQ(funds, 1000.0f);
    
    // Reduce funds below cost
    funds = 300.0f;
    
    // Redo should fail due to insufficient funds
    bool redo_result = history->Redo(funds);
    
    EXPECT_FALSE(redo_result);
    EXPECT_FLOAT_EQ(funds, 300.0f);  // Funds unchanged
    EXPECT_TRUE(history->CanRedo());  // Still available to retry
}

TEST_F(CommandHistoryUnitTest, HistoryEntryMetadata) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -123, "Test Command"), funds);
    
    const auto& undo_stack = history->GetUndoStack();
    ASSERT_EQ(undo_stack.size(), 1);
    
    const auto& entry = undo_stack[0];
    EXPECT_EQ(entry.description, "Test Command");
    EXPECT_EQ(entry.cost_change, -123);
    
    // Timestamp should be recent
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - entry.timestamp);
    EXPECT_LT(diff.count(), 5);  // Within 5 seconds
}

TEST_F(CommandHistoryUnitTest, PositiveCostChange) {
    // Positive cost change (e.g., selling/refund)
    history->ExecuteCommand(std::make_unique<MockCommand>(true, 200, "Refund"), funds);
    
    EXPECT_FLOAT_EQ(funds, 1200.0f);
    
    history->Undo(funds);
    EXPECT_FLOAT_EQ(funds, 1000.0f);
}

TEST_F(CommandHistoryUnitTest, SequentialUndoRedoCycles) {
    history->ExecuteCommand(std::make_unique<MockCommand>(true, -100, "Cmd"), funds);
    
    // Multiple undo/redo cycles
    for (int i = 0; i < 5; ++i) {
        history->Undo(funds);
        EXPECT_FLOAT_EQ(funds, 1000.0f);
        
        history->Redo(funds);
        EXPECT_FLOAT_EQ(funds, 900.0f);
    }
}

TEST_F(CommandHistoryUnitTest, UndoWithNoHistory) {
    EXPECT_FALSE(history->CanUndo());
    
    bool result = history->Undo(funds);
    
    EXPECT_FALSE(result);
    EXPECT_FLOAT_EQ(funds, 1000.0f);
}

TEST_F(CommandHistoryUnitTest, RedoWithNoHistory) {
    EXPECT_FALSE(history->CanRedo());
    
    bool result = history->Redo(funds);
    
    EXPECT_FALSE(result);
    EXPECT_FLOAT_EQ(funds, 1000.0f);
}
