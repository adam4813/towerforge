# Person Entity and Movement System

## Overview

The Person Entity and Movement System simulates individuals moving through the tower. This is a core gameplay mechanic inspired by SimTower, where people navigate between floors, use elevators, and have specific destinations and needs.

## Person Component

The `Person` component is defined in `include/core/components.hpp` and represents an individual in the tower with state tracking and movement capabilities.

### Structure

```cpp
struct Person {
    std::string name;
    PersonState state;
    
    // Current location
    int current_floor;
    float current_column;     // Float for smooth horizontal movement
    
    // Destination
    int destination_floor;
    float destination_column;
    
    // Movement
    float move_speed;         // Horizontal movement speed (columns per second)
    float wait_time;          // Time spent waiting (e.g., for elevator)
    
    // Needs/goals
    std::string current_need; // What the person is trying to do
};
```

### Fields

- **name**: Unique identifier for the person
- **state**: Current state in the state machine (see State Machine section)
- **current_floor**: The floor the person is currently on (integer)
- **current_column**: The horizontal position on the floor (float for smooth movement)
- **destination_floor**: Target floor where the person wants to go
- **destination_column**: Target horizontal position on destination floor
- **move_speed**: How fast the person moves horizontally (columns per second, default: 2.0)
- **wait_time**: Accumulated waiting time (used for elevator waiting, etc.)
- **current_need**: String describing what the person is trying to do (e.g., "Going to work", "Going home")

## State Machine

The `PersonState` enum defines the possible states a person can be in:

```cpp
enum class PersonState {
    Idle,                  // Standing still, no destination
    Walking,               // Moving horizontally on same floor
    WaitingForElevator,    // Waiting for elevator to arrive
    InElevator,            // Currently in an elevator
    AtDestination          // Reached final destination
};
```

### State Transitions

```
Idle → Walking (when destination set on same floor)
Idle → WaitingForElevator (when destination set on different floor)

Walking → AtDestination (reached horizontal destination on same floor)
Walking → WaitingForElevator (reached elevator call point, need to change floors)

WaitingForElevator → InElevator (elevator arrives)

InElevator → Walking (arrived at destination floor, need to walk to column)
InElevator → AtDestination (arrived at destination floor and column)

AtDestination → Walking (new destination set on same floor)
AtDestination → WaitingForElevator (new destination set on different floor)
```

### State Colors (Visual Representation)

- **Idle**: Light Gray
- **Walking**: Blue
- **WaitingForElevator**: Orange
- **InElevator**: Purple
- **AtDestination**: Green

## Systems

The Person movement is handled by four ECS systems in `src/core/ecs_world.cpp`:

### 1. Person Horizontal Movement System

```cpp
world_.system<Person>()
    .kind(flecs::OnUpdate)
    .each([](flecs::entity e, Person& person) { ... });
```

**Purpose**: Handles walking on the same floor towards a destination

**Behavior**:
- Only active when `state == PersonState::Walking`
- Calculates direction to destination (left or right)
- Moves person at `move_speed` (columns per second)
- When destination reached:
  - If on correct floor: transitions to `AtDestination`
  - If need to change floors: transitions to `WaitingForElevator`

### 2. Person Waiting System

```cpp
world_.system<Person>()
    .kind(flecs::OnUpdate)
    .each([](flecs::entity e, Person& person) { ... });
```

**Purpose**: Tracks wait time for people waiting for elevators

**Behavior**:
- Only active when `state == PersonState::WaitingForElevator`
- Accumulates wait time
- **Temporary**: Simulates elevator arrival after 5 seconds (will be replaced with actual elevator system)
- Transitions to `InElevator` when elevator "arrives"

### 3. Person Elevator Riding System

```cpp
world_.system<Person>()
    .kind(flecs::OnUpdate)
    .each([](flecs::entity e, Person& person) { ... });
```

**Purpose**: Simulates being in an elevator

**Behavior**:
- Only active when `state == PersonState::InElevator`
- Accumulates travel time
- **Temporary**: Simulates elevator travel taking 3 seconds (will be replaced with actual elevator system)
- When "elevator arrives":
  - Updates `current_floor` to `destination_floor`
  - If need to walk: transitions to `Walking`
  - If at exact destination: transitions to `AtDestination`

### 4. Person State Logging System

```cpp
world_.system<const Person>()
    .kind(flecs::OnUpdate)
    .interval(5.0f)
    .each([](flecs::entity e, const Person& person) { ... });
```

**Purpose**: Debug logging of person states

**Behavior**:
- Runs every 5 seconds
- Logs person name, state, current location, destination, and need
- Includes wait time for waiting/elevator states

## Spawning People

### Lobby Entry (Ground Floor Spawn)

```cpp
auto person = ecs_world.CreateEntity("PersonName");
Person p("PersonName", 0, 2.0f);  // Floor 0 (lobby), column 2
p.SetDestination(5, 8.0f, "Going to work");
person.set<Person>(p);
```

### Residential Spawn (Higher Floor)

```cpp
auto person = ecs_world.CreateEntity("PersonName");
Person p("PersonName", 10, 5.0f);  // Floor 10 (residential), column 5
p.SetDestination(0, 3.0f, "Going home");
person.set<Person>(p);
```

## Movement API

### Setting Destinations

```cpp
void SetDestination(int floor, float column, const std::string& need = "Moving")
```

Sets a new destination for the person and automatically transitions to the appropriate state:
- Same floor, different column → `Walking`
- Different floor → `WaitingForElevator`
- Already at destination → `AtDestination`

### Checking Progress

```cpp
bool HasReachedHorizontalDestination() const
bool HasReachedVerticalDestination() const
```

Check if the person has reached their horizontal or vertical destination.

### State Information

```cpp
const char* GetStateString() const
```

Returns a human-readable string for the current state (useful for debugging).

## Visual Representation

People are rendered as colored circles on the tower grid:
- Circle size: 8 pixels radius
- Color varies by state (see State Colors above)
- Destination indicator: Semi-transparent line and circle showing where person is going

### Debug Visualization

The screenshot app displays:
1. **Person circles** on the grid at their current position
2. **Destination lines** showing path from current position to destination
3. **Debug panel** showing:
   - Person name and current state
   - Current location (Floor, Column)
   - Destination (Floor, Column)
   - Current need/goal

## Integration with Other Systems

### Satisfaction System

People can have a `Satisfaction` component that tracks their happiness based on:
- Wait times (elevator delays affect satisfaction)
- Facility quality
- Crowding

### Daily Schedule System

People can have a `DailySchedule` component (from the Actor system) that triggers automatic destination changes at specific times:
- Arrive at work (9:00 AM)
- Lunch break (12:00 PM)
- Leave work (5:00 PM)

### Elevator System (Future)

Currently, elevator behavior is simulated with timers. Future integration will:
- Replace wait timers with actual elevator call system
- Replace travel timers with actual elevator movement
- Track elevator capacity and queuing

## Example Usage

### Creating a Person Going to Work

```cpp
// Create person entity
auto alice = ecs_world.CreateEntity("Alice");

// Create Person component starting in lobby
Person alice_person("Alice", 0, 2.0f, 2.0f);  // floor 0, column 2, speed 2.0
alice_person.SetDestination(5, 10.0f, "Going to work");

// Add components
alice.set<Person>(alice_person);
alice.set<Satisfaction>({80.0f});  // Start with good satisfaction

// Person will:
// 1. Start in state: WaitingForElevator (different floor)
// 2. After 5s: InElevator
// 3. After 3s: Walking (arrived at floor 5, walking to column 10)
// 4. After ~4s: AtDestination (walked from column 2 to 10 at speed 2.0)
```

### Creating a Person Shopping on Same Floor

```cpp
auto bob = ecs_world.CreateEntity("Bob");

Person bob_person("Bob", 3, 5.0f);
bob_person.SetDestination(3, 15.0f, "Going to shop");

bob.set<Person>(bob_person);

// Person will:
// 1. Start in state: Walking (same floor)
// 2. After 5s: AtDestination (walked 10 columns at speed 2.0)
```

## Performance Considerations

- Systems run every frame (60 FPS)
- Horizontal movement is smooth (float-based positions)
- State transitions are immediate (no animation delays)
- Logging system runs at 5-second intervals to reduce console spam

## Future Enhancements

1. **Pathfinding**: Smart navigation around obstacles and crowded areas
2. **Elevator Queueing**: Realistic elevator waiting and boarding
3. **Needs System**: Dynamic needs (hunger, bathroom, work, home)
4. **Group Behavior**: Families or coworkers moving together
5. **Floor Congestion**: Slow down in crowded areas
6. **Animation**: Smooth sprite animations for walking and waiting
7. **AI Personalities**: Different walking speeds and patience levels

## Testing

See `src/screenshot_app.cpp` for a complete example with three people:
- Alice: Going from lobby (0,2) to office (1,8)
- Bob: Going from residential (2,12) to lobby (0,3)
- Charlie: Shopping on same floor (3,5) to (3,15)

Run the screenshot app to see the system in action:
```bash
cd build
xvfb-run -a ./bin/screenshot_app
```

Screenshot output: `docs/person_movement_screenshot.png`

## Dependencies

- **flecs**: ECS framework for component and system management
- **Raylib**: Rendering library for visual representation
- **Core Components**: Position, Velocity, Actor (legacy compatibility)
- **Tower Grid**: For floor/column-based positioning
- **Time Manager**: For simulation time tracking

## License

Part of the TowerForge project. See main README for license information.
