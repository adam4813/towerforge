# Elevator System (Consolidated)

This document is the canonical, consolidated reference for the TowerForge elevator system. It merges and replaces:
- `docs/ELEVATOR_SYSTEM.md`
- `docs/ELEVATOR_IMPLEMENTATION_SUMMARY.md`

Use this file for architecture, components, state machine, systems behavior, integration notes, rendering guidance, demo usage, testing, and future improvements.

---

## Overview

The Elevator System provides realistic vertical transportation for person entities in the tower simulation. It is implemented with ECS components and systems and integrates tightly with the `Person` movement system.

Key capabilities:
- Multi-car shafts with configurable floors and capacities
- Smooth interpolation between floors (float positions)
- Door transition timings and open durations
- Stop queues and passenger destination tracking
- Assignment algorithm (first-available by default), extensible to SCAN/LOOK
- Visual rendering with occupancy and state overlays

---

## Components

Defined in `include/core/components.hpp`.

### ElevatorShaft
Represents a vertical shaft that contains one or more elevator cars.

Fields:
- `int column` — grid column where shaft is placed
- `int bottom_floor` — lowest served floor (inclusive)
- `int top_floor` — highest served floor (inclusive)
- `int car_count` — number of cars in this shaft

Methods:
- `GetFloorRange()` — total floors served
- `ServesFloor(int floor)` — whether a floor is within range

### ElevatorCar
Represents an individual elevator car with its state machine and queues.

Fields (example):
```cpp
struct ElevatorCar {
    int shaft_entity_id;            // reference to parent shaft entity
    float current_floor;            // float position for smooth movement
    int target_floor;               // next floor to stop at
    ElevatorState state;            // current state

    int max_capacity;               // default e.g. 8
    int current_occupancy;

    std::vector<int> stop_queue;    // sorted list of requested stops
    std::vector<int> passenger_destinations; // destinations of onboard passengers

    float state_timer;              // timer used for transitions
    float door_open_duration;       // default 2.0s
    float door_transition_duration; // default 1.0s
    float floors_per_second;        // default 2.0
};
```

Helper methods:
- `GetStateString()` — debug string for state
- `IsAtFloor()` — check proximity to integer floor
- `GetCurrentFloorInt()` — integer floor
- `HasCapacity()` — check free slots
- `AddStop(int floor)` — insert into queue while keeping order
- `GetNextStop()` — compute next target considering direction
- `RemoveCurrentStop()` — remove current floor from queue

### PersonElevatorRequest
Attached to `Person` entities when they need elevator transport.

Fields (example):
- `int shaft_entity_id` — shaft person is using
- `int car_entity_id` — assigned car (-1 if waiting)
- `int call_floor` — floor person called from
- `int destination_floor` — target floor
- `float wait_time` — accumulated wait
- `bool is_boarding` — boarding flag

Lifecycle:
1. Created when a `Person` needs to change floors
2. Persists while waiting and riding
3. Removed when person exits at destination

---

## ElevatorState (state machine)

```cpp
enum class ElevatorState {
    Idle,
    MovingUp,
    MovingDown,
    DoorsOpening,
    DoorsOpen,
    DoorsClosing
};
```

Typical flow:
```
Idle → MovingUp/MovingDown → DoorsOpening → DoorsOpen → DoorsClosing → Idle
```

Timing defaults:
- `floors_per_second`: 2.0
- `door_transition_duration`: 1.0s
- `door_open_duration`: 2.0s

---

## Systems (behavior)

All systems are registered in `src/core/ecs_world.cpp` and run as `OnUpdate` (per-frame) except where noted.

### 1) Elevator Car Movement System

Purpose: move cars, handle state timers and transitions.

Behavior:
- Idle: if `stop_queue` non-empty, set `target_floor` and transition to MovingUp/MovingDown.
- MovingUp/MovingDown: interpolate `current_floor` toward `target_floor` at `floors_per_second`. On arrival, transition to `DoorsOpening`.
- DoorsOpening: wait `door_transition_duration`, remove current floor from queue, transition to `DoorsOpen`.
- DoorsOpen: wait `door_open_duration` to allow boarding/exiting, then transition to `DoorsClosing`.
- DoorsClosing: wait `door_transition_duration` and then transition to `Idle` (or to movement if queue remains).

### 2) Elevator Call / Assignment System

Purpose: assign waiting people (`PersonElevatorRequest`) to cars and enqueue stops.

Behavior:
- For each waiting person: find cars for the requested shaft and assign to a car using the first-available algorithm.
- Add both call floor and destination floor to the assigned car's `stop_queue` (maintain ordering).
- Update `wait_time` on `PersonElevatorRequest` while waiting.

Future: replace first-available with direction-aware algorithms (SCAN/LOOK) or cost-based assignment.

### 3) Person Elevator Boarding/Exiting System

Purpose: handle boarding and exiting when doors are open.

Behavior:
- Boarding: when person is assigned to car and car is in `DoorsOpen` at call floor and has capacity:
  - Transition person to `InElevator` and set `car_entity_id`.
  - Increment `current_occupancy` and add passenger destination to car's `passenger_destinations` / `stop_queue`.
  - Reset person's `wait_time`.
- Exiting: when person is in elevator and car is in `DoorsOpen` at person's destination:
  - Update person's `current_floor`, decrement occupancy, remove their destination, remove `PersonElevatorRequest`, and transition to `Walking` or `AtDestination`.

### 4) Elevator Logging System (interval)

Runs at a slower interval (e.g., 10s) to log car states, floors, occupancy, and queues for debugging and analytics.

---

## Integration with Person Movement System

- When a `Person` needs to change floors, the enhanced person waiting system creates a `PersonElevatorRequest` and finds suitable shafts.
- If the person is not located at the shaft column, they will walk to the shaft (transition to `Walking`) and then wait.
- If no shaft is available, fallback simulation uses the legacy timed elevator behavior (simulated wait/travel) to preserve demo compatibility.
- The elevator implementation is backward-compatible and incrementally adoptable.

---

## Rendering guidance

Visual cues help debugging and understanding elevator behavior:

- **Shaft rendering**: draw a vertical column per served floor with background and border.
- **Car rendering**: draw a rectangle positioned using `current_floor` and color-coded by state.
- **Occupancy overlay**: show current occupancy as a small number or bar on the car.
- **Door indicators**: draw door slats when opening/open.

Example snippets (Raylib-like):

```cpp
// Shaft background
DrawRectangle(x + 4, y + 4, width - 8, height - 8, Color{60,60,70,255});
DrawRectangleLines(x + 4, y + 4, width - 8, height - 8, Color{100,100,120,255});

// Car draw at float position
int yPos = grid_offset_y + (int)(car.current_floor * cell_height);
DrawRectangle(x + 6, yPos + 6, width - 12, cell_height - 12, state_color);

// Occupancy
DrawText(TextFormat("%d", car.current_occupancy), x + 18, yPos + 14, 16, BLACK);
```

---

## Demo application

Use `src/elevator_demo_app.cpp` to exercise the elevator system in isolation. The demo shows multiple shafts, cars, and people interacting; it renders a live status panel and can automatically save a screenshot.

Run (example, adapted to your environment):

```cmd
cd build
# On headless CI use a virtual framebuffer; on Windows run the demo directly
xvfb-run -a ./bin/elevator_demo_app
```

Screenshot: `docs/elevator_demo_screenshot.png`.

---

## Usage examples

Create a shaft and car and a person who uses the elevator:

```cpp
// Create elevator shaft at column 10, serving floors 0-5
auto shaft = ecs_world.CreateEntity("MainElevator");
shaft.set<ElevatorShaft>({10, 0, 5, 1});

// Create elevator car
auto car = ecs_world.CreateEntity("Car1");
car.set<ElevatorCar>({
    static_cast<int>(shaft.id()),
    0,   // start at ground floor
    8    // capacity: 8 passengers
});

// Create person who will use elevator
auto person = ecs_world.CreateEntity("Alice");
Person alice("Alice", 0, 2.0f);
alice.SetDestination(5, 10.0f, "Going to work");
person.set<Person>(alice);
```

Person lifecycle: walk to shaft → wait → board → ride → exit → walk to destination.

---

## Acceptance criteria & testing

- Elevator cars move between floors and stop at requested levels (smooth movement at configured speed).
- Persons can use elevators to travel vertically, respecting capacity and queueing rules.
- System supports stop queues, passenger destination tracking, and basic scheduling.
- Documented and demoed via `src/elevator_demo_app.cpp`.

Testing notes:
- Demo runs and saves screenshots; visual checks validate state transitions and boarding.
- Logging system provides periodic debug output to verify queue and state behavior.

---

## Performance & extensibility

- Stop queue insertions maintain sorted order for efficient next-stop selection.
- Entity lookups cached when possible to reduce overhead.
- State updates performed only when queue or timers require it.
- Designed to support future scheduling algorithms and group control features.

---

## Files changed / added

- `include/core/components.hpp` — added `ElevatorShaft`, `ElevatorCar`, `PersonElevatorRequest`
- `src/core/ecs_world.cpp` — added elevator systems and enhanced person waiting system
- `src/elevator_demo_app.cpp` — demo app for elevator system
- `docs/elevator_demo_screenshot.png` — demo output
- `docs/ELEVATOR_SYSTEM.md` and `docs/ELEVATOR_IMPLEMENTATION_SUMMARY.md` — historical split docs (marked deprecated)

---

## Future work

1. Smart scheduling (SCAN/LOOK, direction-aware algorithms)
2. Group elevator control and load balancing
3. Express elevators and priority passengers
4. Maintenance modes and analytics
5. Integration with satisfaction, economics, and HUD systems

---

## Conclusion

The elevator system is implemented with realistic behavior, integrates with existing person movement, includes rendering and demo coverage, and is ready for further improvements. The split implementation docs have been merged into this canonical file; the originals are kept for history and can be archived or removed from the working tree.

---

This file consolidates the full elevator documentation. The old split files can be removed from the working tree once you confirm links have been updated.
