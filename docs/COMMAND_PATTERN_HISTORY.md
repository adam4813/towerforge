# Command Pattern and History System

## Overview

TowerForge implements a robust Command Pattern for managing undo/redo operations with full state restoration. This system provides:

- **Complete Action Encapsulation**: Each action (place, demolish) is a self-contained command object
- **Full State Restoration**: Demolish operations capture complete facility state for accurate restoration
- **Funds Validation**: All operations validate available funds before execution
- **Visual History Panel**: Interactive UI for reviewing and navigating action history
- **Scalable Architecture**: Easy to extend with new command types

## Architecture

### Command Interface

The `ICommand` interface defines the contract for all commands:

```cpp
class ICommand {
public:
    virtual bool Execute() = 0;      // Execute the command
    virtual bool Undo() = 0;         // Undo the command
    virtual std::string GetDescription() const = 0;  // Human-readable description
    virtual int GetCostChange() const = 0;  // Cost change (negative=cost, positive=refund)
};
```

### Command Implementations

#### PlaceFacilityCommand

Encapsulates facility placement with:
- Facility type, position, and size
- Cost calculation
- Entity creation tracking
- Automatic cleanup on undo

**Execute**: Creates facility entity via FacilityManager
**Undo**: Removes facility at stored position

#### DemolishFacilityCommand

Encapsulates facility demolition with:
- **State Capture**: Stores complete facility state before demolition
  - Facility type
  - Position and dimensions
  - Capacity
  - Original cost (for accurate restoration)
- **Refund Calculation**: 50% of original cost (configurable)
- **Full Restoration**: Re-creates facility with original properties on undo

**Execute**: Captures state, then removes facility
**Undo**: Restores facility using captured state

### Command History

The `CommandHistory` class manages undo/redo stacks with:

- **Size Limits**: Configurable maximum history size (default: 50 actions)
- **Funds Validation**: Checks available funds before executing/undoing commands
- **Timestamp Tracking**: Records when each action was performed
- **Metadata Storage**: Keeps description and cost change for UI display

#### Key Operations

**ExecuteCommand(command, funds)**:
1. Validates funds are sufficient for the operation
2. Executes the command
3. Adjusts funds
4. Adds to undo stack
5. Clears redo stack

**Undo(funds)**:
1. Validates funds are sufficient to reverse the operation
2. Undoes the last command
3. Adjusts funds (reverses cost change)
4. Moves command to redo stack

**Redo(funds)**:
1. Validates funds are sufficient
2. Re-executes the command
3. Adjusts funds
4. Moves command back to undo stack

### History Panel UI

The `HistoryPanel` provides visual access to the command history:

#### Features

- **Timeline View**: Shows actions in chronological order
- **Visual Indicators**:
  - White text: Actions in undo stack
  - Blue text: Actions in redo stack
  - Green/Red cost display: Refunds and costs
- **Timestamps**: HH:MM:SS format for each action
- **Scrollable**: Handles large histories
- **Interactive**: Click any entry to undo/redo to that point

#### Layout

- **Position**: Top-right corner (adjusts to screen size)
- **Size**: 280px wide, up to 400px tall
- **Background**: Semi-transparent black with gold border
- **Items**: 50px height per entry

#### Usage

- **Toggle**: Press `H` key
- **Navigate**: Click on any history entry
- **Multiple Actions**: Click earlier entries to undo multiple actions at once
- **Visual Feedback**: Hovered items are highlighted

## Integration with Game

### Placement System

The `PlacementSystem` uses `CommandHistory` instead of manual undo/redo stacks:

```cpp
bool PlaceFacility(int floor, int column, int type, float& funds) {
    auto command = std::make_unique<PlaceFacilityCommand>(
        facility_mgr_, grid_, type, floor, column, width, cost
    );
    return command_history_.ExecuteCommand(std::move(command), funds);
}

bool DemolishFacility(int floor, int column, float& funds) {
    auto command = std::make_unique<DemolishFacilityCommand>(
        facility_mgr_, grid_, floor, column, RECOVERY_PERCENTAGE
    );
    return command_history_.ExecuteCommand(std::move(command), funds);
}
```

### Game Loop Integration

In the game's update loop:

```cpp
// Update history panel display
if (history_panel_->IsVisible()) {
    history_panel_->UpdateFromHistory(placement_system_->GetCommandHistory());
}

// Handle history panel clicks
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && history_panel_->IsMouseOver(x, y)) {
    int steps = history_panel_->HandleClick(x, y);
    if (steps > 0) {
        // Undo multiple actions
        for (int i = 0; i < steps; i++) {
            placement_system_->Undo(funds);
        }
    } else if (steps < 0) {
        // Redo multiple actions
        for (int i = 0; i < -steps; i++) {
            placement_system_->Redo(funds);
        }
    }
}
```

## Benefits

### For Players

- **Confidence**: Freely experiment knowing actions can be undone
- **Error Recovery**: Quickly fix mistakes
- **Visual Feedback**: See complete action history with timestamps
- **Multiple Undo**: Jump back several actions at once
- **Funds Awareness**: Clear indication of cost/refund for each action

### For Developers

- **Clean Code**: Separation of action logic from UI
- **Extensibility**: Easy to add new command types
- **Testability**: Commands can be unit tested independently
- **Maintainability**: Clear structure and responsibilities
- **Debugging**: Complete action history aids debugging

## Future Enhancements

Possible extensions to the command system:

1. **Compound Commands**: Group multiple actions into a single undo operation
2. **Macro Recording**: Record and replay sequences of actions
3. **Persistent History**: Save/load action history with game state
4. **Action Filtering**: Show only specific types of actions in history panel
5. **Advanced Commands**: 
   - Bulk facility placement
   - Floor expansion/contraction
   - Facility upgrades
   - Staff hiring/firing
6. **History Search**: Search/filter actions by type, cost, or time
7. **Action Statistics**: Show metrics (total spent, actions per session, etc.)

## Implementation Files

- `include/core/command.hpp` - Command interface and implementations
- `src/core/command.cpp` - Command implementation details
- `include/core/command_history.hpp` - Command history manager
- `src/core/command_history.cpp` - History management implementation
- `include/ui/history_panel.h` - History panel UI
- `src/ui/history_panel.cpp` - History panel implementation
- `include/ui/placement_system.h` - Integration with placement system
- `src/ui/placement_system.cpp` - Placement system using commands

## Related Documentation

- [Placement System](PLACEMENT_SYSTEM.md) - Overall placement system documentation
- [Game Class Architecture](GAME_CLASS_ARCHITECTURE.md) - Integration with game loop
- [Facilities](FACILITIES.md) - Facility types and properties

---

*Document created: October 18, 2025*
*Last updated: October 18, 2025*
