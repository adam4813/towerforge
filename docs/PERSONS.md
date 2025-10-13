# Persons & Movement (Consolidated)

This document consolidates person-related documentation from:
- `docs/PERSON_MOVEMENT_SYSTEM.md`
- `docs/PERSON_IMPLEMENTATION_SUMMARY.md`

It is the canonical reference for the `Person` component, movement systems, state machine, integration points, visualization, and testing notes.

---

## Overview

The `Person` component models individuals in the tower and drives the movement simulation. People have a small state machine (Idle, Walking, WaitingForElevator, InElevator, AtDestination) and interact with other systems such as the `TimeManager`, `TowerGrid`, `Satisfaction`, and (future) elevator systems.

Key points:
- People can walk horizontally along a floor, wait for elevators, ride elevators, and reach destinations.
- Movement is handled by a set of ECS systems implemented in `src/core/ecs_world.cpp`.
- Visual debugging is provided by the `screenshot_app` and in-game debug panels.


## Person component (structure)

Defined in `include/core/components.hpp`.

```cpp
struct Person {
    std::string name;
    PersonState state;

    // Current location
    int current_floor;
    float current_column; // Float for smooth horizontal movement

    // Destination
    int destination_floor;
    float destination_column;

    // Movement
    float move_speed; // horizontal movement speed (columns per second)
    float wait_time;  // time spent waiting (e.g., for elevator)

    // Needs/goals
    std::string current_need;

    // Key methods
    void SetDestination(int floor, float column, const std::string& need = "Moving");
    bool HasReachedHorizontalDestination() const;
    bool HasReachedVerticalDestination() const;
    const char* GetStateString() const;
};
```

Default parameter guidance:
- `move_speed` default: 2.0 columns/sec
- `wait_time` accumulates while in `WaitingForElevator`


## PersonState enum

```cpp
enum class PersonState {
    Idle,
    Walking,
    WaitingForElevator,
    InElevator,
    AtDestination
};
```

State meanings:
- Idle: no destination
- Walking: moving horizontally on same floor
- WaitingForElevator: waiting for elevator arrival
- InElevator: currently inside an elevator
- AtDestination: reached the final destination


## State transitions

- Idle → Walking (destination set on same floor)
- Idle → WaitingForElevator (destination set on different floor)
- Walking → AtDestination (reached horizontal destination on same floor)
- Walking → WaitingForElevator (reached elevator call point)
- WaitingForElevator → InElevator (elevator arrives)
- InElevator → Walking (arrived at destination floor and need to walk)
- InElevator → AtDestination (arrived at destination floor and column)
- AtDestination → Walking / WaitingForElevator (new destination set)


## Systems (implementation summary)

Four main ECS systems handle person behavior. Implementations live in `src/core/ecs_world.cpp` and register systems with flecs.

1) Person Horizontal Movement System
- Runs on entities with `Person` and when `state == PersonState::Walking`.
- Moves `current_column` toward `destination_column` at `move_speed * delta_time`.
- When horizontal destination reached:
  - If on correct floor: transition to `AtDestination`.
  - If destination is on another floor: transition to `WaitingForElevator`.

2) Person Waiting System
- Runs when `state == PersonState::WaitingForElevator`.
- Accumulates `wait_time`.
- Currently temporary: simulates elevator arrival after a fixed timeout (e.g., 5s).
- On simulated arrival: transition to `InElevator` and reset relevant timers.

3) Person Elevator Riding System
- Runs when `state == PersonState::InElevator`.
- Accumulates travel time; temporary simulation treats travel as a fixed duration (e.g., 3s).
- On simulated arrival: updates `current_floor` to `destination_floor` and:
  - If horizontal walking required: transition to `Walking`.
  - If already at target column: transition to `AtDestination`.

4) Person State Logging System
- Runs at a lower frequency (e.g., every 5 seconds) to log state for debugging.
- Logs fields: name, state, current location, destination, need, wait_time.


## Movement API

- `void SetDestination(int floor, float column, const std::string& need = "Moving")`
  - Determines whether to `Walking` (same floor) or `WaitingForElevator` (different floor).
  - Sets `destination_floor`, `destination_column`, and `current_need`.

- `bool HasReachedHorizontalDestination() const` and `bool HasReachedVerticalDestination() const`
  - Utility checks used by systems to determine transitions.

- `const char* GetStateString() const`
  - Returns human-readable state name for debug displays.


## Spawning examples

Lobby spawn (ground floor):

```cpp
auto person_entity = ecs_world.CreateEntity("PersonName");
Person p("PersonName", 0, 2.0f); // floor 0 (lobby), column 2
p.SetDestination(5, 8.0f, "Going to work");
person_entity.set<Person>(p);
```

Residential spawn (higher floor):

```cpp
auto person_entity = ecs_world.CreateEntity("PersonName");
Person p("PersonName", 10, 5.0f); // floor 10, column 5
p.SetDestination(0, 3.0f, "Going home");
person_entity.set<Person>(p);
```


## Visual representation and debug

- Rendered as colored circles on tower grid (radius ~8 px)
- Color by state: Idle (Light Gray), Walking (Blue), WaitingForElevator (Orange), InElevator (Purple), AtDestination (Green)
- Destination indicator: semi-transparent line + marker
- Debug panel shows name, state, current location, destination, need

See `src/screenshot_app.cpp` for a runnable demo and `screenshots/person_movement_screenshot.png` for example output.


## Integration with other systems

- Satisfaction: `Satisfaction` component affects and is affected by wait times, facility quality, and crowding.
- Elevator (future): `PersonElevatorRequest` will connect person requests to elevator controllers and queueing logic.
- TimeManager: uses consistent delta time for smooth movement and scheduling.
- TowerGrid: floors/columns mapping used for position conversions and world resizing on load.


## Testing

- `src/screenshot_app.cpp` provides an integration demonstration with several people.
- Test cases include same-floor walking, cross-floor trips, and idle behavior.
- Performance: systems are designed to run at 60 FPS; logging is throttled to reduce overhead.

Testing quick-run (example):

```cmd
cd build
cmake --build .
# For headless screenshot app (Linux): xvfb-run -a ./bin/screenshot_app
```


## Future work and improvements

1. Replace temporary elevator timers with the real Elevator system (call/ride/queueing)
2. Add pathfinding to navigate around obstacles and crowds
3. Implement group behavior and dynamic needs (hunger, bathroom, work)
4. Add animation and richer visualization
5. Improve congestion handling (slowdown in crowded areas)


## Files and references

- Implementation: `src/core/ecs_world.cpp`
- Interface: `include/core/components.hpp`
- Demo: `src/screenshot_app.cpp`
- Documentation: this file (consolidated)


---

This file is now the canonical, merged reference for person-related documentation. The original split files `docs/PERSON_MOVEMENT_SYSTEM.md` and `docs/PERSON_IMPLEMENTATION_SUMMARY.md` are preserved in git history and can be removed from the working tree if you prefer to avoid duplication.
