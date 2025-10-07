# Elevator System

## Overview

The Elevator System provides vertical transportation for person entities in the tower simulation. It consists of elevator shafts that serve multiple floors and elevator cars that move within those shafts, picking up and dropping off passengers based on requests.

## Architecture

The elevator system is built using three main ECS components and four integrated systems that work together to provide realistic elevator behavior.

### Components

#### 1. ElevatorShaft

Represents a vertical shaft that contains one or more elevator cars. The shaft defines the physical space and floors served by the elevator system.

```cpp
struct ElevatorShaft {
    int column;              // Grid column where shaft is located
    int bottom_floor;        // Lowest floor served
    int top_floor;           // Highest floor served
    int car_count;           // Number of cars in this shaft
};
```

**Fields:**
- **column**: The horizontal position in the tower grid where the shaft is located
- **bottom_floor**: The lowest floor this shaft serves (inclusive)
- **top_floor**: The highest floor this shaft serves (inclusive)
- **car_count**: Number of elevator cars operating in this shaft

**Methods:**
- `GetFloorRange()`: Returns the total number of floors served
- `ServesFloor(int floor)`: Checks if a given floor is within the shaft's service range

#### 2. ElevatorCar

Represents an individual elevator car that moves within a shaft, handling passenger transport and scheduling.

```cpp
struct ElevatorCar {
    int shaft_entity_id;      // Reference to the shaft this car belongs to
    float current_floor;      // Current position (float for smooth movement)
    int target_floor;         // Next floor destination
    ElevatorState state;      // Current state in the state machine
    
    // Capacity and occupancy
    int max_capacity;         // Maximum number of passengers
    int current_occupancy;    // Current number of passengers
    
    // Queue management
    std::vector<int> stop_queue;              // Floors where car needs to stop
    std::vector<int> passenger_destinations;  // Destination floors of passengers
    
    // Timing
    float state_timer;        // Timer for current state
    float door_open_duration; // How long doors stay open (seconds)
    float door_transition_duration;  // How long doors take to open/close
    float floors_per_second;  // Movement speed
};
```

**State Machine:**
```cpp
enum class ElevatorState {
    Idle,           // Stationary, no pending requests
    MovingUp,       // Moving upward between floors
    MovingDown,     // Moving downward between floors
    DoorsOpening,   // Doors are opening
    DoorsOpen,      // Doors are open, passengers can board/exit
    DoorsClosing    // Doors are closing
};
```

**Default Timings:**
- Door open duration: 2.0 seconds
- Door transition duration: 1.0 second
- Movement speed: 2.0 floors per second
- Default capacity: 8 passengers

**Key Methods:**
- `GetStateString()`: Returns the current state as a string for debugging
- `IsAtFloor()`: Checks if the car is precisely at a floor (within tolerance)
- `GetCurrentFloorInt()`: Returns the current floor as an integer
- `HasCapacity()`: Checks if the car can accept more passengers
- `AddStop(int floor)`: Adds a floor to the stop queue (maintains sorted order)
- `GetNextStop()`: Returns the next stop based on current direction
- `RemoveCurrentStop()`: Removes current floor from the stop queue

#### 3. PersonElevatorRequest

Attached to Person entities when they need to use an elevator. Links a person to a specific shaft and tracks their boarding status.

```cpp
struct PersonElevatorRequest {
    int shaft_entity_id;      // Which shaft the person is using
    int car_entity_id;        // Which car the person is in (-1 if waiting)
    int call_floor;           // Floor where person called the elevator
    int destination_floor;    // Where person wants to go
    float wait_time;          // How long person has been waiting
    bool is_boarding;         // True if person is currently boarding
};
```

**Lifecycle:**
1. Created when a person enters `WaitingForElevator` state and needs to change floors
2. Persists while person is waiting or riding the elevator
3. Removed when person exits the elevator at their destination floor

## Systems

The elevator system consists of four ECS systems that handle different aspects of elevator operation:

### 1. Elevator Car Movement System

**Purpose:** Handles elevator movement between floors and state transitions

**Update Frequency:** Every frame (OnUpdate)

**Behavior:**

- **Idle State:**
  - Checks if there are stops in the queue
  - If stops exist, sets target floor and transitions to MovingUp or MovingDown
  - If already at target floor, transitions to DoorsOpening

- **MovingUp/MovingDown States:**
  - Moves car towards target floor at configured speed
  - Smoothly interpolates between floors
  - When arriving at target floor, transitions to DoorsOpening

- **DoorsOpening State:**
  - Waits for door_transition_duration (default 1 second)
  - Transitions to DoorsOpen when complete
  - Removes current floor from stop queue

- **DoorsOpen State:**
  - Waits for door_open_duration (default 2 seconds)
  - Allows passengers to board and exit
  - Transitions to DoorsClosing when timer expires

- **DoorsClosing State:**
  - Waits for door_transition_duration (default 1 second)
  - Transitions to Idle when complete

### 2. Elevator Call System

**Purpose:** Assigns people waiting for elevators to appropriate elevator cars

**Update Frequency:** Every frame (OnUpdate)

**Behavior:**

- Processes all Person entities with PersonElevatorRequest components
- Updates wait time for people waiting
- For people not yet assigned to a car:
  - Finds cars belonging to the requested shaft
  - Assigns the person to the first available car (simple algorithm)
  - Adds both the call floor and destination floor to the car's stop queue
  
**Future Improvements:**
- Implement smarter scheduling (closest car, direction-based assignment)
- Load balancing across multiple cars in the same shaft
- Priority queuing for VIP passengers

### 3. Person Elevator Boarding System

**Purpose:** Handles people getting on and off elevators

**Update Frequency:** Every frame (OnUpdate)

**Behavior:**

**Boarding:**
- Checks if person is waiting and assigned car has arrived
- Verifies car is in DoorsOpen state
- Verifies car is at the person's call floor
- Verifies car has capacity
- If all conditions met:
  - Transitions person to InElevator state
  - Increments car occupancy
  - Adds destination to car's passenger destinations
  - Resets person wait time

**Exiting:**
- Checks if person is in elevator and car has arrived at destination
- Verifies car is in DoorsOpen state
- Verifies car is at the person's destination floor
- If conditions met:
  - Updates person's current floor
  - Decrements car occupancy
  - Removes destination from car's passenger list
  - Removes PersonElevatorRequest component from person
  - Transitions person to Walking or AtDestination state

### 4. Elevator Logging System

**Purpose:** Debug logging for elevator state monitoring

**Update Frequency:** Every 10 seconds (interval)

**Behavior:**
- Logs elevator car state, floor position, and occupancy
- Displays current stop queue for debugging
- Helps diagnose elevator scheduling and movement issues

## Integration with Person Movement System

The elevator system integrates seamlessly with the existing Person Movement System:

### Person Waiting System

When a person needs to change floors:

1. Person transitions to `WaitingForElevator` state
2. System creates a `PersonElevatorRequest` component
3. System searches for suitable elevator shafts that serve both current and destination floors
4. If shaft found and person is not at shaft column:
   - Person transitions to `Walking` state
   - Person walks to the elevator shaft column
   - When arriving at shaft, person transitions back to `WaitingForElevator`
5. If no shaft found, falls back to old behavior (backward compatibility)

### Fallback Behavior

For backward compatibility with existing code, the system includes fallback behavior:

- People in `InElevator` state without `PersonElevatorRequest` use simulated 3-second travel time
- People in `WaitingForElevator` state without a shaft available use simulated 5-second wait time

This ensures existing demos and tests continue to work while new code can leverage the full elevator system.

## Usage Examples

### Creating an Elevator System

```cpp
// Create elevator shaft serving floors 0-5 at column 10
auto shaft = ecs_world.CreateEntity("MainElevatorShaft");
shaft.set<ElevatorShaft>({
    10,  // column
    0,   // bottom_floor
    5,   // top_floor
    1    // car_count
});

// Create elevator car for this shaft
auto car = ecs_world.CreateEntity("Elevator1");
car.set<ElevatorCar>({
    static_cast<int>(shaft.id()),  // shaft_entity_id
    0,   // start at floor 0
    8    // capacity: 8 passengers
});
```

### Multiple Cars in One Shaft

```cpp
// Create shaft with multiple cars
auto shaft = ecs_world.CreateEntity("ExpressElevatorShaft");
shaft.set<ElevatorShaft>({5, 0, 10, 2});  // 2 cars

// Create two cars
auto car1 = ecs_world.CreateEntity("ExpressA");
car1.set<ElevatorCar>({static_cast<int>(shaft.id()), 0, 10});

auto car2 = ecs_world.CreateEntity("ExpressB");
car2.set<ElevatorCar>({static_cast<int>(shaft.id()), 5, 10});  // Start at floor 5
```

### Creating People Who Use Elevators

```cpp
// Person starting in lobby, going to office on floor 5
auto person = ecs_world.CreateEntity("Alice");
Person alice("Alice", 0, 2.0f);  // Floor 0, column 2
alice.SetDestination(5, 10.0f, "Going to work");
person.set<Person>(alice);

// Person will automatically:
// 1. Walk to nearest elevator shaft
// 2. Wait for elevator to arrive
// 3. Board the elevator
// 4. Ride to floor 5
// 5. Exit and walk to column 10
// 6. Reach destination
```

### Manual Elevator Control

```cpp
// Manually add a stop to an elevator
auto car_entity = ecs_world.GetWorld().entity("Elevator1");
if (car_entity.is_valid() && car_entity.has<ElevatorCar>()) {
    ElevatorCar& car = car_entity.ensure<ElevatorCar>();
    car.AddStop(3);  // Add floor 3 to stop queue
}
```

## Rendering

Elevator shafts and cars should be rendered to provide visual feedback:

### Shaft Rendering

```cpp
// Draw elevator shaft as vertical column
for (int floor = shaft.bottom_floor; floor <= shaft.top_floor; ++floor) {
    int x = grid_offset_x + shaft.column * cell_width;
    int y = grid_offset_y + floor * cell_height;
    
    // Draw shaft background
    DrawRectangle(x + 4, y + 4, cell_width - 8, cell_height - 8, 
                  Color{60, 60, 70, 255});
    DrawRectangleLines(x + 4, y + 4, cell_width - 8, cell_height - 8, 
                       Color{100, 100, 120, 255});
}
```

### Car Rendering

```cpp
// Color based on state
Color car_color;
switch (car.state) {
    case ElevatorState::Idle:           car_color = GRAY; break;
    case ElevatorState::MovingUp:       car_color = SKYBLUE; break;
    case ElevatorState::MovingDown:     car_color = PURPLE; break;
    case ElevatorState::DoorsOpening:
    case ElevatorState::DoorsClosing:   car_color = YELLOW; break;
    case ElevatorState::DoorsOpen:      car_color = GREEN; break;
}

// Draw car at current floor position
int x = grid_offset_x + shaft.column * cell_width;
int y = grid_offset_y + (int)(car.current_floor * cell_height);
DrawRectangle(x + 6, y + 6, cell_width - 12, cell_height - 12, car_color);

// Draw occupancy indicator
if (car.current_occupancy > 0) {
    DrawText(TextFormat("%d", car.current_occupancy), x + 18, y + 14, 16, BLACK);
}

// Draw door indicator when doors are opening/open/closing
if (car.state == ElevatorState::DoorsOpen || 
    car.state == ElevatorState::DoorsOpening ||
    car.state == ElevatorState::DoorsClosing) {
    DrawRectangle(x + 8, y + 8, 4, cell_height - 16, BLACK);
    DrawRectangle(x + cell_width - 12, y + 8, 4, cell_height - 16, BLACK);
}
```

## Testing

See `src/elevator_demo_app.cpp` for a complete demonstration with:
- Two elevator shafts at different locations
- Multiple elevator cars
- Four people using elevators to travel between floors
- Visual representation of elevator states and movement

Run the demo:
```bash
cd build
xvfb-run -a ./bin/elevator_demo_app
```

Screenshot output: `docs/elevator_demo_screenshot.png`

## Performance Considerations

- **Stop Queue Management:** The stop queue is maintained in sorted order for efficient next-stop calculation
- **Entity Lookups:** Shaft and car entities are cached by ID to minimize lookup overhead
- **State Transitions:** State machine updates happen only when necessary, avoiding unnecessary calculations
- **Boarding Logic:** Boarding checks are performed only for people actively waiting or in elevators

## Future Enhancements

### Planned Features
- **Smart Scheduling:** Implement SCAN/LOOK algorithms for efficient stop ordering
- **Group Elevator Control:** Coordinate multiple cars in the same shaft for optimal service
- **Express Elevators:** Fast elevators that skip certain floors
- **Priority System:** VIP passengers get faster service
- **Maintenance Mode:** Elevators can be taken offline for maintenance
- **Capacity Alerts:** Visual and audio feedback when elevators are full
- **Wait Time Tracking:** Statistics on average wait times per floor

### Integration Points
- **Satisfaction System:** Long wait times reduce passenger satisfaction
- **Economics System:** Elevator maintenance costs and efficiency impact tower economics
- **HUD System:** Display elevator status in the information panels
- **Analytics:** Track elevator usage patterns for optimization

## Dependencies

- **flecs**: ECS framework for component and system management
- **Raylib**: Rendering library for visual representation
- **Core Components**: Person, Position, Satisfaction
- **Tower Grid**: For spatial management and placement
- **Time Manager**: For simulation time tracking

## License

Part of the TowerForge project. See main README for license information.
