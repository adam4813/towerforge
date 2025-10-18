# Facility Cleanliness System

## Overview

The Facility Cleanliness System simulates the natural degradation of facility cleanliness over time and with use, requiring players to manage janitorial staff to maintain tenant satisfaction. This system adds gentle, ongoing management without punitive failures, fitting the cozy management theme of TowerForge.

## Core Components

### CleanlinessStatus Component

The `CleanlinessStatus` component tracks the cleanliness state of each facility using a state-based approach:

```cpp
struct CleanlinessStatus {
    enum class State { Clean, NeedsCleaning, Dirty };
    State status;
    float time_since_last_clean;  // Seconds since last cleaned
    float dirty_rate;              // Degradation rate multiplier
};
```

**States:**
- **Clean**: Facility is in good condition (100% cleanliness)
- **Needs Cleaning**: Facility is getting dirty and should be cleaned soon (60% cleanliness)
- **Dirty**: Facility is dirty and negatively impacts satisfaction (30% cleanliness)

### Degradation Rates by Facility Type

Different facility types get dirty at different rates based on their usage patterns:

| Facility Type | Dirty Rate | Reasoning |
|---------------|------------|-----------|
| Restaurant    | 1.5x       | High food traffic, spills, and customer turnover |
| Hotel         | 1.5x       | Constant guest usage, housekeeping demands |
| Retail Shop   | 1.2x       | Customer foot traffic, product handling |
| Arcade        | 1.2x       | Heavy usage by visitors, equipment wear |
| Flagship Store| 1.2x       | High customer volume |
| Office        | 1.0x       | Normal office environment |
| Gym           | 1.0x       | Regular usage with equipment |
| Theater       | 1.0x       | Periodic events with audience |
| Other         | 0.8x       | Lower traffic facilities |

### Degradation Thresholds

The system uses time-based thresholds that are adjusted by the dirty rate and occupancy:

- **Needs Cleaning**: Base threshold of 30 minutes (1800 seconds)
- **Dirty**: Base threshold of 60 minutes (3600 seconds)

These thresholds are divided by both the dirty_rate and the occupancy factor using the formula:

```
adjusted_threshold = base_threshold / (dirty_rate * occupancy_factor)
```

**Example**: A restaurant at 80% occupancy:
- Occupancy factor: 1.0 + (0.8 * 2.0) = 2.6x
- Adjusted "Needs Cleaning" threshold: 1800 / (1.5 * 2.6) ≈ 462 seconds (7.7 minutes)
- Adjusted "Dirty" threshold: 3600 / (1.5 * 2.6) ≈ 923 seconds (15.4 minutes)

## ECS Systems

### 1. CleanlinessStatus Degradation System

**Interval**: 1 second  
**Purpose**: Updates cleanliness state based on elapsed time and occupancy

```cpp
world_.system<CleanlinessStatus, const BuildingComponent>()
    .interval(1.0f)
    .each([](CleanlinessStatus& cleanliness, const BuildingComponent& facility) {
        float occupancy_factor = 1.0f + (occupancy_rate * 2.0f);
        cleanliness.Update(delta_time, occupancy_factor);
    });
```

### 2. Cleanliness Notification System

**Interval**: 5 seconds  
**Purpose**: Notifies players when facilities become dirty

The system tracks which facilities have been notified to avoid spam:
- Notifies when a facility becomes "Needs Cleaning"
- Notifies again when it becomes "Dirty"
- Clears notification flags when the facility is cleaned

**Example Output:**
```
[NOTIFICATION] Restaurant on Floor 3 needs cleaning.
[NOTIFICATION] Retail Shop on Floor 2 is dirty! Guests are unhappy.
```

### 3. CleanlinessStatus Impact on Satisfaction

**Interval**: 2 seconds  
**Purpose**: Applies gentle penalties/bonuses to tenant satisfaction

| Cleanliness State | Satisfaction Impact |
|-------------------|---------------------|
| Dirty             | -2.0 to quality bonus per update |
| Needs Cleaning    | -0.5 to quality bonus per update |
| Clean             | +0.3 to quality bonus per update |

These impacts are gentle and accumulate gradually, never causing hard failures.

### 4. Staff Cleaning System

**Interval**: 5 seconds  
**Purpose**: Janitors and cleaners automatically clean facilities

The system:
1. Checks for active cleaning staff (Janitors, Cleaners)
2. Finds facilities that need cleaning within their assigned floor/area
3. Cleans one facility per staff member per cycle
4. Logs cleaning actions to the console

**Staff Assignment:**
- Staff can be assigned to specific facilities (`assigned_facility_entity`)
- Staff can be assigned to entire floors (`assigned_floor`)
- Staff with `assigned_floor = -1` work tower-wide

## UI Integration

### Facility Info Window

The facility info window displays cleanliness information:

```
--- Facility Status ---
Status: Needs Cleaning    [Color: Yellow/Red based on state]
Cleanliness: Clean (100%)
Maintenance: Good (85%)
```

### "Clean Now" Button

When a facility needs cleaning (`needs_cleaning = true`), a "Clean Now" button appears:

```
[Clean Now]  [Color: Yellow for "Needs Cleaning", Red for "Dirty"]
```

**Current Status**: The button is displayed as a **visual indicator only**. The UI framework does not yet have click handling implemented for this button. Cleaning is handled automatically by janitor staff through the Staff Cleaning System.

**Future Enhancement**: When UI button click handling is implemented, this button will call `FacilityManager::CleanFacility()` to provide manual cleaning on demand. This requires enhancing the UI window system to support button callbacks.

### Manual Cleaning API

For future button integration or programmatic cleaning:

```cpp
// In FacilityManager
bool CleanFacility(flecs::entity facility_entity);

// Usage example
if (facility_manager.CleanFacility(facility_entity)) {
    std::cout << "Facility cleaned successfully!" << std::endl;
}
```

## Gameplay Impact

### Gentle Management

The cleanliness system is designed to be **cozy and non-punitive**:

1. **No Hard Failures**: Dirty facilities never cause catastrophic problems
2. **Gradual Impact**: Satisfaction decreases slowly, giving players time to respond
3. **Clear Feedback**: Visual states and notifications clearly communicate status
4. **Automation Available**: Janitor staff handle cleaning automatically

### Strategic Considerations

Players need to consider:

1. **Staff Allocation**: Hiring enough janitors for tower size and facility count
2. **Facility Placement**: Grouping high-traffic facilities for efficient cleaning
3. **Shift Coverage**: Ensuring cleaning staff coverage during busy hours
4. **Prioritization**: Restaurants and hotels need more frequent attention

### Recommended Staffing

Following the existing staff calculation system, the formula recommends:

```cpp
// 1 janitor per 3 facilities or per 5 floors (using ceiling division for proper coverage)
int recommended_janitors = max(ceil(facility_count / 3.0), ceil(total_floors / 5.0));
```

**Note**: The actual implementation in `StaffManager::CalculateRecommendedStaff()` uses integer division which may truncate fractional values. For example, 7 facilities would calculate as `(7 + 2) / 3 = 3` janitors using the `(facility_count + 2) / 3` pattern to avoid underestimation.

## Technical Details

### Component Lifecycle

1. **Creation**: `CleanlinessStatus` is attached to all new facilities in `FacilityManager::CreateFacility()`
2. **Update**: The degradation system runs every second
3. **Cleaning**: Staff system or manual cleaning resets the state
4. **Display**: Game code queries the component and populates `FacilityInfo` for UI

### Integration with Existing Systems

The cleanliness system works alongside:

1. **FacilityStatus**: The original component remains for compatibility
2. **Satisfaction System**: Both systems can apply penalties independently
3. **Staff Management**: Uses existing `StaffAssignment` and `StaffManager` components
4. **Notification System**: Uses console output (future: integrate with NotificationCenter)

### Performance Considerations

- Systems use appropriate intervals (1-5 seconds) to avoid per-frame overhead
- Static notification tracking prevents repeated messages
- Component queries are efficient using flecs ECS

## Future Enhancements

Potential improvements for future versions:

1. **Button Click Handling**: Implement actual button functionality for manual cleaning
2. **Visual Indicators**: Add cleanliness visual effects (sparkles, dirt, etc.)
3. **Notification Integration**: Use the existing NotificationCenter for in-game alerts
4. **Cleaning Animations**: Animate staff cleaning actions
5. **Cleaning Tools**: Unlock better cleaning equipment via research tree
6. **Pest Control**: Add related systems for pest infestations in dirty facilities
7. **Inspection Events**: Random health inspections for restaurants
8. **Cleaning Priorities**: Allow players to set cleaning priorities per facility

## Testing Recommendations

To test the cleanliness system:

1. **Create Test Facilities**: Place restaurants, hotels, and shops
2. **Observe Degradation**: Wait and watch facilities degrade over time
3. **Check Notifications**: Verify console notifications appear appropriately
4. **Test Janitor Staff**: Hire janitors and verify automatic cleaning
5. **Monitor Satisfaction**: Observe satisfaction changes with cleanliness states
6. **High Occupancy Test**: Fill facilities and verify faster degradation
7. **Manual Cleaning**: Test the `CleanFacility()` method programmatically

## Related Documentation

- [STAFF_MANAGEMENT.md](STAFF_MANAGEMENT.md) - Staff system and assignment
- [FACILITIES.md](FACILITIES.md) - Facility types and properties
- [HUD.md](HUD.md) - UI display system
- [NOTIFICATION_CENTER.md](NOTIFICATION_CENTER.md) - Notification system (for future integration)

## Summary

The Facility Cleanliness System adds depth to tower management while maintaining the cozy, non-punitive design philosophy of TowerForge. It encourages players to hire and manage staff, provides clear feedback through states and notifications, and integrates seamlessly with existing simulation and satisfaction systems.
