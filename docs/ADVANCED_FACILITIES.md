# Advanced Facility Types with Staffing Requirements

## Overview

This document describes the advanced facility types available in TowerForge, including their staffing requirements, operating hours, and gameplay characteristics.

## Facility Types

### Entertainment & Leisure

#### Gym (Fitness & Wellness Center)
- **Size**: 7 tiles wide
- **Capacity**: 25 people
- **Staff Required**: 3 employees (trainers/attendants)
- **Operating Hours**: 9:00 AM - 9:00 PM (default)
- **Build Cost**: $10,000
- **Build Time**: 18 seconds
- **Purpose**: Improves tenant happiness and attracts health-conscious visitors
- **Color**: Orange

#### Arcade (Entertainment Venue)
- **Size**: 5 tiles wide
- **Capacity**: 20 people
- **Staff Required**: 2 employees (clerks)
- **Operating Hours**: 9:00 AM - 9:00 PM (default)
- **Build Cost**: $9,000
- **Build Time**: 16 seconds
- **Purpose**: Increases visitor activity and entertainment options
- **Color**: Magenta

#### Theater (Entertainment Venue)
- **Size**: 8 tiles wide
- **Capacity**: 50 people
- **Staff Required**: 3 employees (ushers, staff)
- **Operating Hours**: 9:00 AM - 9:00 PM (default)
- **Build Cost**: $15,000
- **Build Time**: 22 seconds
- **Purpose**: Major entertainment venue, attracts large crowds
- **Color**: Violet

### Business & Events

#### Conference Hall (Event Space)
- **Size**: 9 tiles wide
- **Capacity**: 60 people
- **Staff Required**: 2 employees (event coordinators)
- **Operating Hours**: 9:00 AM - 5:00 PM (default)
- **Build Cost**: $13,000
- **Build Time**: 20 seconds
- **Purpose**: Host events, meetings, and conferences
- **Color**: Light Blue

#### Flagship Store (Large Retail)
- **Size**: 12 tiles wide
- **Capacity**: 40 people
- **Staff Required**: 4 employees (shop staff)
- **Operating Hours**: 9:00 AM - 9:00 PM (default)
- **Build Cost**: $18,000
- **Build Time**: 28 seconds
- **Purpose**: Premium retail space, generates high income
- **Color**: Turquoise

### Hospitality (Updated)

#### Restaurant (Food Service)
- **Size**: 6 tiles wide
- **Capacity**: 30 people
- **Staff Required**: 4 employees (cooks, servers)
- **Operating Hours**: 9:00 AM - 9:00 PM (default)
- **Build Cost**: $8,000
- **Build Time**: 20 seconds
- **Purpose**: Attracts visitors, boosts happiness
- **Color**: Red

#### Hotel (Temporary Lodging)
- **Size**: 10 tiles wide
- **Capacity**: 40 rooms
- **Staff Required**: 5 employees (receptionists, cleaning staff)
- **Operating Hours**: 24/7 (always operational)
- **Build Cost**: $12,000
- **Build Time**: 25 seconds
- **Purpose**: Generates income from guests, provides lodging
- **Color**: Dark Blue

## Staffing System

### How Staffing Works

1. **Minimum Staff Requirement**: Each facility has a minimum number of staff needed to operate
2. **Operational Status**: Facilities only function when:
   - Minimum staff is present (`current_staff >= GetRequiredEmployees()`)
   - Within operating hours
3. **Understaffed Facilities**: Do not generate revenue or provide benefits

### Staff Roles by Facility Type

| Facility Type | Required Staff | Roles |
|--------------|----------------|-------|
| Restaurant | 4 | Cooks, Servers |
| Hotel | 5 | Receptionists, Cleaners |
| Gym | 3 | Trainers, Attendants |
| Arcade | 2 | Clerks |
| Theater | 3 | Ushers, Staff |
| Conference Hall | 2 | Event Coordinators |
| Flagship Store | 4 | Shop Staff |
| Retail Shop | 2 | Shop Staff |
| Office | capacity/5 | Office Workers |

### Operating Hours

- **Default**: 9:00 AM - 5:00 PM (business hours)
- **Entertainment/Retail**: 9:00 AM - 9:00 PM (extended hours)
- **Hotels**: 24/7 (always operational)

Facilities do not operate outside their scheduled hours, even if fully staffed.

## Gameplay Mechanics

### Building a Facility

1. Select the facility type from the build menu
2. Pay the build cost
3. Wait for construction to complete
4. Hire staff to make the facility operational

### Making a Facility Operational

1. **Hire Staff**: Assign the required number of employees
   - Staff can be hired through the employment system
   - Job openings are tracked per facility
2. **Operating Hours**: Facility must be within its operating hours
3. **Check Status**: Use `IsOperational(current_hour)` to verify

### Non-Operational Facilities

When a facility is not operational (understaffed or outside hours):
- No revenue is generated
- No happiness benefits are provided
- Visitors may become dissatisfied
- The facility appears inactive in the UI

## Implementation Notes

### Component Fields

The `BuildingComponent` includes:
- `current_staff`: Number of staff currently assigned
- `job_openings`: Number of unfilled positions
- `operating_start_hour`: Start of operating hours (e.g., 9.0)
- `operating_end_hour`: End of operating hours (e.g., 21.0)

### Key Methods

- `GetRequiredEmployees()`: Returns minimum staff needed
- `HasJobOpenings()`: Check if facility needs more staff
- `IsOperational(current_hour)`: Check if facility is functional

## Future Enhancements

The following features are planned but not yet implemented:
- Staff scheduling and shift management
- Staff skill levels and training
- Facility upgrades affecting staff requirements
- Adjacency bonuses between facilities
- Noise and environmental factors
- Inventory management for retail/restaurants

## Example Usage

```cpp
// Create a new gym facility
auto gym = facility_mgr.CreateFacility(
    BuildingComponent::Type::Gym,
    floor, column
);

// Check staffing requirements
auto& building = gym.get<BuildingComponent>();
int required_staff = building.GetRequiredEmployees(); // Returns 3

// Assign staff
building.current_staff = 3;
building.job_openings = 0;

// Check if operational at 10:00 AM
bool operational = building.IsOperational(10.0f); // Returns true

// Check if operational at 10:00 PM
operational = building.IsOperational(22.0f); // Returns false (outside hours)
```

## Testing

To test the new facility types:
1. Build each facility type through the UI
2. Verify staffing requirements are correct
3. Check that facilities are non-operational when understaffed
4. Verify operating hours are enforced
5. Confirm unique colors are displayed for each type

## References

- See `include/core/components.hpp` for `BuildingComponent` definition
- See `src/core/facility_manager.cpp` for facility defaults
- See `src/ui/build_menu.cpp` for UI integration
