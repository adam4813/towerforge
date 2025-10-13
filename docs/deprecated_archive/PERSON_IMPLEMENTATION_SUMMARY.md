# Person Entity and Movement System - Implementation Summary

## Overview

This document summarizes the implementation of the Person Entity and Basic Movement System for TowerForge, completed as part of issue #11.

## What Was Implemented

### 1. Person Component (`include/core/components.hpp`)

A new `Person` component was added with the following features:
- **State machine** with 5 states: Idle, Walking, WaitingForElevator, InElevator, AtDestination
- **Position tracking**: Current floor (int) and column (float for smooth movement)
- **Destination tracking**: Target floor and column
- **Movement parameters**: Configurable speed, wait time tracking
- **Needs/goals**: String field describing current objective

**Key Methods**:
- `SetDestination(floor, column, need)` - Sets destination and auto-transitions state
- `HasReachedHorizontalDestination()` - Checks if at target column
- `HasReachedVerticalDestination()` - Checks if at target floor
- `GetStateString()` - Returns human-readable state name for debugging

### 2. PersonState Enum

```cpp
enum class PersonState {
    Idle,                  // Standing still, no destination
    Walking,               // Moving horizontally on same floor
    WaitingForElevator,    // Waiting for elevator to arrive
    InElevator,            // Currently in an elevator
    AtDestination          // Reached final destination
};
```

### 3. ECS Systems (`src/core/ecs_world.cpp`)

Four new systems were added to handle person movement and state transitions:

#### Person Horizontal Movement System
- Moves people left/right on same floor
- Smooth floating-point movement (2.0 columns/second default)
- Auto-transitions to appropriate state when destination reached

#### Person Waiting System
- Tracks elevator wait time
- Temporary simulation: elevator "arrives" after 5 seconds
- Will be replaced with actual elevator system integration

#### Person Elevator Riding System
- Simulates being in elevator
- Temporary simulation: travel takes 3 seconds
- Updates current floor on arrival
- Transitions to Walking or AtDestination based on final position

#### Person State Logging System
- Debug logging every 5 seconds
- Shows name, state, position, destination, and current need
- Includes wait time for elevator-related states

### 4. Visual Representation (`src/screenshot_app.cpp`)

People are rendered as colored circles on the tower grid:
- **Idle**: Light Gray
- **Walking**: Blue
- **WaitingForElevator**: Orange
- **InElevator**: Purple
- **AtDestination**: Green

**Debug Visualization**:
- Destination indicator: Semi-transparent line from person to destination
- Debug panel showing real-time person information
- State legend for quick reference

### 5. Spawning System

Two spawning patterns implemented:

**Lobby Entry (Ground Floor)**:
```cpp
Person p("Name", 0, 2.0f);  // Floor 0, column 2
p.SetDestination(5, 8.0f, "Going to work");
```

**Residential Spawn (Higher Floor)**:
```cpp
Person p("Name", 10, 5.0f);  // Floor 10, column 5
p.SetDestination(0, 3.0f, "Going home");
```

### 6. Documentation

Created comprehensive documentation:
- **PERSON_MOVEMENT_SYSTEM.md**: Complete technical documentation
- **person_movement_screenshot.png**: Visual demonstration of the system

## State Machine Documentation

### State Transitions

```
Idle → Walking (destination on same floor)
Idle → WaitingForElevator (destination on different floor)

Walking → AtDestination (reached destination, same floor)
Walking → WaitingForElevator (reached call point, need elevator)

WaitingForElevator → InElevator (elevator arrives)

InElevator → Walking (arrived at floor, need to walk)
InElevator → AtDestination (arrived at exact destination)

AtDestination → Walking (new destination, same floor)
AtDestination → WaitingForElevator (new destination, different floor)
```

### State Behavior

Each state has specific behavior handled by the systems:
- **Idle**: No movement, waiting for destination
- **Walking**: Horizontal movement at `move_speed` columns/second
- **WaitingForElevator**: Accumulates wait time, transitions after timeout
- **InElevator**: Accumulates travel time, transitions after timeout
- **AtDestination**: No movement, ready for new destination

## Acceptance Criteria Met

✅ **Can spawn people at specific locations**
- Implemented lobby entry and residential spawn patterns
- Flexible API for any floor/column combination

✅ **People can walk horizontally on floors**
- Smooth movement system with configurable speed
- Automatic pathfinding (direct line to destination)
- Collision-free movement

✅ **People can request floor changes**
- Setting destination on different floor triggers elevator waiting
- Sets up for future elevator system integration
- Simulated elevator behavior for testing

✅ **Visual feedback shows movement and state**
- Color-coded circles for different states
- Destination indicators
- Real-time debug panel

✅ **State machine is documented**
- Complete documentation in PERSON_MOVEMENT_SYSTEM.md
- State transition diagrams
- Example usage code

## Integration with Existing Systems

### Satisfaction System
People can have `Satisfaction` component that tracks:
- Wait time penalties (elevator delays)
- Facility quality bonuses
- Crowding penalties

### Time Management System
Compatible with existing `TimeManager`:
- Uses delta time for smooth movement
- Can integrate with daily schedules
- Time-based behavior possible

### Tower Grid System
Works with existing grid infrastructure:
- Floor-based positioning
- Column-based horizontal movement
- Ready for collision detection integration

## Future Work (Not in Scope)

The following features are intentionally left for future iterations:
1. **Actual Elevator System**: Replace simulated wait/travel times
2. **Pathfinding**: Smart navigation around obstacles
3. **Needs System**: Dynamic needs (hunger, bathroom, work)
4. **Group Behavior**: Families moving together
5. **Congestion**: Slow down in crowded areas
6. **Animation**: Sprite-based walking animations
7. **AI Personalities**: Different speeds and patience levels

## Testing

### Screenshot App Demonstration
The `screenshot_app` includes 4 test people:
- **Alice**: Lobby to Office (cross-floor movement)
- **Bob**: Residential to Lobby (going home)
- **Charlie**: Shop to Shop (same-floor movement)
- **Diana**: Idle (no destination)

### Build and Test
```bash
cd build
cmake --build .
xvfb-run -a ./bin/screenshot_app
```

### Verification
- ✅ Builds without errors or warnings
- ✅ All systems register correctly
- ✅ People spawn at correct locations
- ✅ Movement is smooth and accurate
- ✅ State transitions work as expected
- ✅ Debug visualization displays correctly

## Performance

- Systems run at 60 FPS (every frame)
- No performance issues with 4+ people
- Logging throttled to 5-second intervals
- Efficient float-based movement calculations

## Code Quality

- Follows C++20 standards
- Uses modern C++ idioms (auto, constexpr where appropriate)
- RAII for resource management
- Comprehensive inline documentation
- Meaningful variable and function names
- Modular, maintainable design

## Files Changed

1. `include/core/components.hpp` - Added Person component and PersonState enum
2. `src/core/ecs_world.cpp` - Added 4 movement systems and component registration
3. `src/screenshot_app.cpp` - Added visual representation and test cases
4. `docs/PERSON_MOVEMENT_SYSTEM.md` - Technical documentation
5. `docs/person_movement_screenshot.png` - Visual demonstration

## Dependencies Met

✅ Tower Grid System (Issue #9) - Uses grid for positioning
✅ Basic Facilities (Issue #10) - People can interact with facilities

## Conclusion

The Person Entity and Basic Movement System has been successfully implemented with all acceptance criteria met. The system provides a solid foundation for future gameplay features like elevator management, needs systems, and advanced AI behavior.

The implementation is:
- **Complete**: All required features implemented
- **Documented**: Comprehensive documentation and examples
- **Tested**: Working demonstration in screenshot_app
- **Maintainable**: Clean, modular code following project standards
- **Extensible**: Ready for future enhancements

---

# DEPRECATED: Merged into `docs/PERSONS.md`.

See docs/PERSONS.md for the merged reference.
