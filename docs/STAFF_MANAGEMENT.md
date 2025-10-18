# Tower Staff Management System

## Overview

The Staff Management System introduces tower staff roles that automatically maintain facilities, respond to emergencies, and improve tenant satisfaction. The system emphasizes cozy, non-punitive gameplay where staff enhance the tower experience without creating stressful micromanagement.

## Staff Roles

### Janitor
- **Primary Function**: Cleans facilities when cleanliness drops below 70%
- **Coverage**: Tower-wide or floor-specific assignment
- **Effect**: Restores cleanliness by 30% per cleaning cycle
- **Recommended Ratio**: 1 janitor per 3 facilities (minimum) or 1 per 5 floors for tower-wide coverage

### Maintenance Technician
- **Primary Function**: Maintains facilities when maintenance drops below 70%
- **Coverage**: Tower-wide or floor-specific assignment
- **Effect**: Restores maintenance level by 25% per cycle
- **Recommended Ratio**: 1 maintenance tech per 5 facilities (minimum)

### Firefighter
- **Primary Function**: Extinguishes fires in facilities
- **Coverage**: Tower-wide, 24/7 availability
- **Effect**: Immediately extinguishes fires; facilities continue operating but require cleaning afterward
- **Recommended Ratio**: 1 firefighter per 10 floors (minimum)
- **Note**: Fires are isolated events that don't spread; facilities remain operational during fires but may have reduced satisfaction

### Security Guard
- **Primary Function**: Resolves security issues (shoplifters, disturbances)
- **Coverage**: Tower-wide or floor-specific assignment
- **Effect**: Resolves security issues, minimally impacts visitor satisfaction
- **Recommended Ratio**: 1 guard per 15 facilities or 1 per 10 floors (minimum)
- **Note**: Security issues don't affect player directly but may slightly reduce visitor satisfaction

### Cleaner (Specialized)
- **Primary Function**: Advanced cleaning specialist
- **Coverage**: Facility-specific or floor-specific assignment
- **Effect**: More efficient than janitors for high-traffic facilities
- **Recommended Ratio**: 1 cleaner per 5 high-traffic facilities (optimal)

### Repairer (Specialized)
- **Primary Function**: Advanced maintenance specialist
- **Coverage**: Facility-specific or floor-specific assignment
- **Effect**: More efficient than general maintenance for complex facilities
- **Recommended Ratio**: 1 repairer per 7 facilities (optimal)

## Components

### StaffAssignment
Located in `include/core/components.hpp`

```cpp
struct StaffAssignment {
    StaffRole role;                  // Staff role type
    int assigned_floor;              // Floor assignment (-1 for tower-wide)
    int assigned_facility_entity;    // Specific facility (-1 for floor-wide)
    float shift_start_time;          // Hour when shift starts (0-24)
    float shift_end_time;            // Hour when shift ends (0-24)
    bool is_active;                  // Currently on duty
    bool auto_assigned;              // Auto-assigned by system
    float work_efficiency;           // 0.0-1.0 work speed multiplier
};
```

### FacilityStatus
Located in `include/core/components.hpp`

```cpp
struct FacilityStatus {
    float cleanliness;               // 0-100, degrades with use
    float maintenance_level;         // 0-100, degrades over time
    bool has_fire;                   // Active fire emergency
    bool has_security_issue;         // Active security problem
    float time_since_cleaning;       // Seconds since last cleaned
    float time_since_maintenance;    // Seconds since last maintained
    float degradation_rate;          // Degradation speed multiplier
};
```

### StaffManager (Singleton)
Located in `include/core/components.hpp`

```cpp
struct StaffManager {
    int total_staff_count;           // Total employed staff
    int firefighters;                // Count by role
    int security_guards;
    int janitors;
    int maintenance_staff;
    int cleaners;
    int repairers;
    float total_staff_wages;         // Daily wages
    bool auto_hire_enabled;          // Auto-hire when needed
};
```

## Systems

All systems are registered in `src/core/ecs_world.cpp`:

### Facility Status Degradation System
- **Frequency**: Every 1 second
- **Function**: Updates facility cleanliness and maintenance based on occupancy
- **Details**: 
  - Cleanliness degrades faster with higher occupancy
  - Maintenance degrades slowly over time
  - Both clamped to 0-100 range

### Staff Shift Management System
- **Frequency**: Every 1 second
- **Function**: Activates/deactivates staff based on shift hours
- **Details**: 
  - Checks if current time falls within shift hours
  - Handles overnight shifts (e.g., 22:00-06:00)
  - Updates `is_active` status and logs shift changes

### Staff Cleaning System
- **Frequency**: Every 5 seconds
- **Function**: Janitors and cleaners automatically clean facilities
- **Details**:
  - Only processes active staff
  - Finds facilities needing cleaning (cleanliness < 70%)
  - Respects floor/facility assignments
  - Cleans one facility per cycle per staff member
  - Logs cleaning activities to console

### Staff Maintenance System
- **Frequency**: Every 8 seconds (slower than cleaning)
- **Function**: Maintenance staff and repairers maintain facilities
- **Details**:
  - Only processes active maintenance staff
  - Finds facilities needing maintenance (level < 70%)
  - Respects floor/facility assignments
  - Maintains one facility per cycle per staff member
  - Logs maintenance activities to console

### Staff Firefighting System
- **Frequency**: Every 2 seconds (urgent response)
- **Function**: Firefighters respond to and extinguish fires
- **Details**:
  - Only active firefighters respond
  - Finds facilities with active fires
  - Immediately extinguishes fire
  - Fire causes 10% maintenance damage
  - Logs firefighting activities to console

### Staff Security System
- **Frequency**: Every 3 seconds
- **Function**: Security guards resolve security issues
- **Details**:
  - Only active security guards respond
  - Finds facilities with security issues
  - Immediately resolves issue
  - Logs security activities to console

### Facility Status Impact System
- **Frequency**: Every 2 seconds
- **Function**: Updates satisfaction based on facility conditions
- **Details**:
  - Poor cleanliness (<50%) slightly reduces satisfaction quality bonus
  - Poor maintenance (<50%) slightly reduces satisfaction quality bonus
  - Excellent conditions (≥90% both) provide small satisfaction bonus
  - Changes are gentle and non-punitive

### Staff Manager Update System
- **Frequency**: Every 5 seconds
- **Function**: Counts staff by role and calculates wages
- **Details**:
  - Counts all staff entities with StaffAssignment component
  - Updates role-specific counters
  - Calculates total wages (hardcoded at $50/day per staff member; may be configurable in future)

### Staff Wage System
- **Frequency**: Every 1 second
- **Function**: Deducts staff wages from tower economy
- **Details**:
  - Adds wages to daily expenses
  - Spreads cost over 24 hours ($50/day per staff = ~$0.0006/second)
  - Integrates with existing economy system

### Staff Status Reporting System
- **Frequency**: Every 30 seconds
- **Function**: Reports staff counts and wages to console
- **Details**:
  - Shows total staff count
  - Lists counts by role (only non-zero)
  - Shows daily wage total

## User Interface

### Facility Info Window
When clicking on a facility, the info window now displays:
- **Cleanliness**: Rating and percentage (color-coded: green ≥70%, yellow ≥50%, red <50%)
- **Maintenance**: Rating and percentage (color-coded: green ≥70%, yellow ≥50%, red <50%)
- **Fire Alert**: "! FIRE - Firefighter needed !" (red text)
- **Security Alert**: "! Security Issue - Guard needed !" (orange text)

### Person Info Window
When clicking on a staff member, the info window displays:
- **Type**: "Staff"
- **Role**: Staff role name (Janitor, Maintenance, etc.)
- **Status**: "On Duty" (green) or "Off Duty" (gray)
- **Shift Hours**: "HH:00 - HH:00" format
- **Current Activity**: What the staff is doing
- Standard person information (floor, destination, state)

## Hiring Staff

Staff are created as entities with both `Person` and `StaffAssignment` components:

```cpp
// Example: Hire a janitor
const auto janitor = world.entity("Bob the Janitor");
janitor.set<Person>({"Bob", 0, 3.0f, 2.0f, NPCType::Employee});
janitor.set<StaffAssignment>({StaffRole::Janitor, -1, 6.0f, 18.0f});
// -1 for floor means tower-wide assignment
// 6.0f to 18.0f means 6 AM to 6 PM shift
```

### Starter Tower Staff
The starter tower automatically creates three staff members:
1. **Bob the Janitor**: Tower-wide, 6 AM - 6 PM
2. **Carlos the Maintenance Tech**: Tower-wide, 8 AM - 5 PM
3. **Dana the Firefighter**: Tower-wide, 24/7

## Gameplay Balance

### Non-Punitive Design
- Poor facility conditions provide gentle satisfaction penalties, not hard failures
- Facilities continue operating regardless of cleanliness/maintenance
- Fires and security issues are isolated events that don't cascade
- Staff wages are reasonable and don't create financial pressure

### Cozy Automation
- Staff automatically work within their shift hours
- Players don't need to micromanage individual tasks
- Status is visible but doesn't require constant attention
- Auto-hire can be enabled for hands-off management (future enhancement)

### Satisfaction Impact
- Cleanliness < 50%: Small reduction in quality bonus
- Maintenance < 50%: Small reduction in quality bonus
- Excellent conditions (≥90%): Small satisfaction bonus
- Changes are gradual and recoverable

## Lua Modding Integration

The staff management system is fully integrated with TowerForge's Lua modding API, allowing modders to create custom staff roles and facility events that work seamlessly with the built-in systems.

### Custom Staff Roles

Custom staff roles registered via Lua are automatically integrated:

1. **Automatic Instantiation**: When mods are loaded, the game creates one staff member for each custom role in the starter tower
2. **Work Type Integration**: Custom roles specify a `work_type` ("cleaning", "maintenance", "emergency", or "custom")
3. **System Compatibility**: Built-in ECS systems recognize custom staff by their work type, not their specific role
4. **Efficiency & Wages**: Custom roles can have custom efficiency multipliers and wage rates

**Example: Custom Night Janitor**
```lua
TowerForge.RegisterStaffRole({
    id = "night_janitor",
    name = "Night Janitor",
    work_efficiency = 1.25,        -- 25% more efficient
    default_wage = 60.0,
    shift_start_hour = 22.0,       -- 10 PM
    shift_end_hour = 6.0,          -- 6 AM
    work_type = "cleaning"         -- Integrated with cleaning system
})
```

### Custom Event Types

Custom facility events registered via Lua are tracked in the `FacilityStatus` component:

1. **Event Tracking**: Events are stored in `active_custom_events` vector
2. **Staff Response**: Events specify which staff type can resolve them
3. **Satisfaction Impact**: Events apply configurable satisfaction penalties while active
4. **Maintenance Damage**: Events can cause maintenance damage when spawned or resolved

**Example: Custom Gas Leak Event**
```lua
TowerForge.RegisterEventType({
    id = "gas_leak",
    name = "Gas Leak",
    spawn_chance = 0.0003,         -- Rare event
    requires_staff_response = true,
    required_staff_type = "emergency_coordinator",
    satisfaction_penalty = 25.0,   -- Major penalty
    maintenance_damage = 20.0
})
```

### How Integration Works

#### Staff Assignment Component
The `StaffAssignment` component supports both built-in and custom roles:

```cpp
struct StaffAssignment {
    StaffRole role;                  // Built-in role enum
    std::string custom_role_id;      // Custom role ID from Lua (empty if built-in)
    std::string work_type;           // "cleaning", "maintenance", "emergency", "custom"
    // ... other fields
    
    bool IsCustomRole() const;       // Check if custom
    bool DoesCleaningWork() const;   // Check work type
    bool DoesMaintenanceWork() const;
    bool DoesEmergencyWork() const;
};
```

#### Facility Status Component
The `FacilityStatus` component tracks both built-in and custom events:

```cpp
struct FacilityStatus {
    bool has_fire;                           // Built-in fire event
    bool has_security_issue;                 // Built-in security event
    std::vector<std::string> active_custom_events;  // Custom event IDs
    
    void AddCustomEvent(const std::string& event_id);
    void RemoveCustomEvent(const std::string& event_id);
    bool HasCustomEvent(const std::string& event_id) const;
};
```

#### ECS Systems
All staff systems check work type instead of specific roles:

```cpp
// Cleaning system works with ANY staff that has work_type = "cleaning"
if (!assignment.DoesCleaningWork()) return;  // Includes custom cleaning staff

// Maintenance system works with ANY staff that has work_type = "maintenance"
if (!assignment.DoesMaintenanceWork()) return;  // Includes custom maintenance staff

// Emergency system works with ANY staff that has work_type = "emergency"
if (!assignment.DoesEmergencyWork()) return;  // Includes custom emergency staff
```

### Complete Integration Example

See `mods/example_emergency_response_team.lua` for a complete example that demonstrates:
- 3 custom staff roles (Emergency Coordinator, Building Inspector, Night Custodian)
- 3 custom event types (Gas Leak, Plumbing Failure, Major Spill)
- Full integration with work types and event resolution
- Different efficiency levels, wages, and shift hours
- Events that require specific custom staff types

## Future Enhancements

### Planned Features
- **Auto-Hire**: Automatically hire recommended staff based on facility count
- **Staff Panel**: Dedicated UI for viewing and managing all staff
- **Staff Training**: Improve work efficiency over time
- **Specializations**: Advanced staff roles for specific facility types
- **Staff Satisfaction**: Staff happiness affects work efficiency
- **Schedule Templates**: Quick-apply shift schedules
- **Emergency Response**: Priority system for fires and security issues
- **Custom Work Behaviors**: Lua callbacks for custom staff work functions
- **Event Callbacks**: Lua functions called when events spawn/resolve
- **NotificationCenter Integration**: Connect staff activities to the NotificationCenter for in-game notifications

## Technical Notes

### Performance
- Systems use interval-based updates (1-8 seconds) to reduce overhead
- Staff work on one facility per cycle to distribute work naturally
- Facility status uses simple float math for degradation
- No pathfinding or complex AI required

### Integration
- Staff wages integrate with existing `TowerEconomy` system
- Facility status uses existing `Satisfaction` system for impact
- Staff use existing `Person` component for basic entity properties
- Custom Lua staff and events are automatically integrated via work type matching
- Staff activities currently log to console; future integration with `NotificationCenter` is planned

### Extensibility
- Custom staff roles can be added via Lua without modifying C++ code
- Custom events can be added via Lua with configurable parameters
- Work type system allows arbitrary staff behaviors
- Facility status can track unlimited custom events
- New systems can be added following existing patterns

## Example Usage

### Creating Custom Staff
```cpp
// Create a cleaner assigned to a specific floor
const auto cleaner = world.entity("Jane the Cleaner");
cleaner.set<Person>({"Jane", 2, 4.0f, 2.0f, NPCType::Employee});
cleaner.set<StaffAssignment>({
    StaffRole::Cleaner,
    2,        // Floor 2 only
    -1,       // Any facility on floor
    7.0f,     // 7 AM start
    15.0f,    // 3 PM end
});
```

### Checking Facility Status
```cpp
// Query all facilities that need attention
world.each<FacilityStatus, BuildingComponent>(
    [](flecs::entity e, const FacilityStatus& status, const BuildingComponent& facility) {
        if (status.NeedsCleaning()) {
            std::cout << "Facility on floor " << facility.floor << " needs cleaning!" << std::endl;
        }
        if (status.NeedsMaintenance()) {
            std::cout << "Facility on floor " << facility.floor << " needs maintenance!" << std::endl;
        }
    });
```

### Triggering Events (Future)
```cpp
// Trigger a fire in a facility (for testing/events)
facility_entity.get_mut<FacilityStatus>()->has_fire = true;

// Trigger a security issue
facility_entity.get_mut<FacilityStatus>()->has_security_issue = true;
```

## Conclusion

The Staff Management System enhances TowerForge with automated facility maintenance and emergency response, all while maintaining the game's cozy, non-punitive design philosophy. Staff work quietly in the background, improving conditions without demanding constant player attention. The system is extensible, performant, and fully integrated with existing game systems.
