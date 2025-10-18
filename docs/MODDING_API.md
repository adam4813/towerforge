# TowerForge Lua Modding API Documentation

Welcome to the TowerForge Modding API! This guide will help you create custom facilities, visitor types, staff roles, and event types for TowerForge using Lua scripting.

## Table of Contents
- [Getting Started](#getting-started)
- [Mod Structure](#mod-structure)
- [API Reference](#api-reference)
  - [TowerForge.RegisterFacilityType](#towerforgeregisterfacilitytype)
  - [TowerForge.RegisterVisitorType](#towerforgeregistervisitortype)
  - [TowerForge.RegisterStaffRole](#towerforgeregisterstaffrole)
  - [TowerForge.RegisterEventType](#towerforgeregistereventtype)
  - [TowerForge.Log](#towerforgelog)
- [Examples](#examples)
- [Security and Sandboxing](#security-and-sandboxing)

## Getting Started

1. Create a new `.lua` file in the `mods/` directory
2. Define your mod's metadata using the `ModInfo` table
3. Use the TowerForge API functions to register custom content
4. Launch the game to see your mod in action!

## Mod Structure

Every mod should start with a `ModInfo` table that describes the mod:

```lua
ModInfo = {
    id = "my_mod",                    -- Unique identifier (required)
    name = "My Awesome Mod",          -- Display name (required)
    version = "1.0.0",                -- Version string (optional)
    author = "Your Name",             -- Author name (optional)
    description = "Description here"  -- Mod description (optional)
}
```

## API Reference

### TowerForge.RegisterFacilityType

Registers a new facility type that can be built in the tower.

**Syntax:**
```lua
TowerForge.RegisterFacilityType({
    id = "unique_id",           -- Required: Unique identifier (string)
    name = "Display Name",      -- Required: Display name (string)
    
    -- Optional: Size and capacity
    default_width = 8,          -- Default: 8 grid cells
    default_capacity = 20,      -- Default: 20 people
    
    -- Optional: Staffing requirements
    required_staff = 2,         -- Default: 0 employees needed
    
    -- Optional: Operating hours (24-hour format)
    operating_start_hour = 9.0,   -- Default: 9 AM
    operating_end_hour = 17.0,    -- Default: 5 PM
    
    -- Optional: Economics
    base_rent = 100.0,          -- Default: $100 per tenant per day
    operating_cost = 20.0,      -- Default: $20 daily operating cost
    
    -- Optional: Visual appearance
    color = 0xFFFFFFFF          -- Default: White (0xRRGGBBAA format)
})
```

**Parameters:**
- `id` (string, required): Unique identifier for this facility type
- `name` (string, required): Human-readable name displayed in the UI
- `default_width` (number, optional): Default width in grid cells (default: 8)
- `default_capacity` (number, optional): Maximum occupancy (default: 20)
- `required_staff` (number, optional): Number of employees needed (default: 0)
- `operating_start_hour` (number, optional): Opening time in 24-hour format (default: 9.0)
- `operating_end_hour` (number, optional): Closing time in 24-hour format (default: 17.0)
- `base_rent` (number, optional): Revenue per tenant per day (default: 100.0)
- `operating_cost` (number, optional): Daily operating expenses (default: 20.0)
- `color` (number, optional): RGBA color in hex format 0xRRGGBBAA (default: 0xFFFFFFFF)

**Color Format:**
Colors are specified as hexadecimal numbers in RGBA format:
- `0xFF0000FF` = Red (fully opaque)
- `0x00FF00FF` = Green (fully opaque)
- `0x0000FFFF` = Blue (fully opaque)
- `0xFFFF00FF` = Yellow (fully opaque)
- `0xFF00FFFF` = Magenta (fully opaque)
- `0x00FFFFFF` = Cyan (fully opaque)

**Example:**
```lua
TowerForge.RegisterFacilityType({
    id = "bookstore",
    name = "Bookstore",
    default_width = 6,
    default_capacity = 15,
    required_staff = 2,
    operating_start_hour = 10.0,
    operating_end_hour = 20.0,
    base_rent = 120.0,
    operating_cost = 30.0,
    color = 0x8B4513FF  -- Brown color
})
```

---

### TowerForge.RegisterVisitorType

Registers a new visitor type that can spawn and visit the tower.

**Syntax:**
```lua
TowerForge.RegisterVisitorType({
    id = "unique_id",           -- Required: Unique identifier (string)
    name = "Display Name",      -- Required: Display name (string)
    
    -- Optional: Movement behavior
    move_speed = 2.0,           -- Default: 2.0 columns per second
    
    -- Optional: Visit duration
    min_visit_duration = 60.0,  -- Default: 60 seconds (1 minute)
    max_visit_duration = 300.0, -- Default: 300 seconds (5 minutes)
    
    -- Optional: Facility preferences
    preferred_facilities = {    -- Default: empty array
        "Office",
        "RetailShop",
        "Restaurant"
    }
})
```

**Parameters:**
- `id` (string, required): Unique identifier for this visitor type
- `name` (string, required): Human-readable name displayed in the UI
- `move_speed` (number, optional): Movement speed in columns per second (default: 2.0)
- `min_visit_duration` (number, optional): Minimum time in tower in seconds (default: 60.0)
- `max_visit_duration` (number, optional): Maximum time in tower in seconds (default: 300.0)
- `preferred_facilities` (table, optional): Array of facility type IDs this visitor prefers (default: empty)

**Example:**
```lua
TowerForge.RegisterVisitorType({
    id = "student",
    name = "Student",
    move_speed = 2.5,
    min_visit_duration = 120.0,
    max_visit_duration = 480.0,
    preferred_facilities = {
        "bookstore",  -- Custom facility from another mod
        "Office",
        "Lobby"
    }
})
```

---

### TowerForge.RegisterStaffRole

Registers a new custom staff role that can be hired to work in the tower.

**Syntax:**
```lua
TowerForge.RegisterStaffRole({
    id = "unique_id",              -- Required: Unique identifier (string)
    name = "Display Name",         -- Required: Display name (string)
    
    -- Optional: Work characteristics
    work_efficiency = 1.0,         -- Default: 1.0 (normal speed)
    default_wage = 50.0,           -- Default: $50 per day
    
    -- Optional: Shift hours (24-hour format)
    shift_start_hour = 9.0,        -- Default: 9 AM
    shift_end_hour = 17.0,         -- Default: 5 PM
    
    -- Optional: Work behavior
    work_type = "custom",          -- Default: "custom" (options: "cleaning", "maintenance", "emergency", "custom")
    work_function = "MyWorkFunc"   -- Default: "" (Lua function name for custom work behavior)
})
```

**Parameters:**
- `id` (string, required): Unique identifier for this staff role
- `name` (string, required): Human-readable name displayed in the UI
- `work_efficiency` (number, optional): Work speed multiplier, higher = faster (default: 1.0)
- `default_wage` (number, optional): Daily wage per staff member (default: 50.0)
- `shift_start_hour` (number, optional): Shift start time in 24-hour format (default: 9.0)
- `shift_end_hour` (number, optional): Shift end time in 24-hour format (default: 17.0)
- `work_type` (string, optional): Type of work performed: "cleaning", "maintenance", "emergency", or "custom" (default: "custom")
- `work_function` (string, optional): Name of Lua function to call for custom work behavior (default: "")

**Work Types:**
- `"cleaning"`: Staff will automatically clean facilities when cleanliness drops below 70%
- `"maintenance"`: Staff will automatically maintain facilities when maintenance drops below 70%
- `"emergency"`: Staff will respond to fires and security issues
- `"custom"`: Staff will call the specified Lua function for custom behavior

**Example:**
```lua
TowerForge.RegisterStaffRole({
    id = "night_janitor",
    name = "Night Janitor",
    work_efficiency = 1.25,        -- 25% more efficient
    default_wage = 60.0,           -- Higher wage for night shift
    shift_start_hour = 22.0,       -- 10 PM
    shift_end_hour = 6.0,          -- 6 AM
    work_type = "cleaning"
})
```

---

### TowerForge.RegisterEventType

Registers a new custom event type that can occur in facilities (e.g., fires, security issues, equipment failures).

**Syntax:**
```lua
TowerForge.RegisterEventType({
    id = "unique_id",              -- Required: Unique identifier (string)
    name = "Display Name",         -- Required: Display name (string)
    
    -- Optional: Event description
    description = "Description",   -- Default: "" (what the event represents)
    
    -- Optional: Occurrence parameters
    spawn_chance = 0.001,          -- Default: 0.001 (0.1% chance per hour per facility)
    duration = 300.0,              -- Default: 300 seconds (how long event lasts if not resolved)
    
    -- Optional: Staff response
    requires_staff_response = true,        -- Default: true (whether staff can resolve it)
    required_staff_type = "Firefighter",   -- Default: "" (staff type that can resolve it)
    resolution_time = 10.0,                -- Default: 10 seconds (time for staff to resolve)
    
    -- Optional: Impact on facility
    satisfaction_penalty = 5.0,    -- Default: 5.0 (satisfaction reduction while active)
    maintenance_damage = 10.0,     -- Default: 10.0 (maintenance damage when resolved)
    
    -- Optional: Lua callbacks
    on_spawn_function = "OnSpawn",     -- Default: "" (function called when event spawns)
    on_resolve_function = "OnResolve"  -- Default: "" (function called when event is resolved)
})
```

**Parameters:**
- `id` (string, required): Unique identifier for this event type
- `name` (string, required): Human-readable name displayed in the UI
- `description` (string, optional): Description of what the event represents (default: "")
- `spawn_chance` (number, optional): Probability per hour per facility (0.0-1.0) (default: 0.001)
- `duration` (number, optional): How long event lasts in seconds, 0 = permanent until resolved (default: 300.0)
- `requires_staff_response` (boolean, optional): Whether staff need to resolve it (default: true)
- `required_staff_type` (string, optional): Staff type that can resolve it (e.g., "Firefighter", "Security", "Janitor") (default: "")
- `resolution_time` (number, optional): Time in seconds for staff to resolve (default: 10.0)
- `satisfaction_penalty` (number, optional): Satisfaction reduction while event is active (default: 5.0)
- `maintenance_damage` (number, optional): Maintenance damage caused when event occurs or is resolved (default: 10.0)
- `on_spawn_function` (string, optional): Name of Lua function to call when event spawns (default: "")
- `on_resolve_function` (string, optional): Name of Lua function to call when event is resolved (default: "")

**Example:**
```lua
TowerForge.RegisterEventType({
    id = "electrical_fire",
    name = "Electrical Fire",
    description = "An electrical malfunction has caused a fire",
    spawn_chance = 0.0005,         -- 0.05% chance per hour (rare)
    duration = 0.0,                -- Permanent until extinguished
    requires_staff_response = true,
    required_staff_type = "Firefighter",
    resolution_time = 15.0,        -- Takes 15 seconds to extinguish
    satisfaction_penalty = 20.0,   -- Major satisfaction penalty
    maintenance_damage = 15.0      -- Causes significant damage
})
```

---

### TowerForge.Log

Logs a message to the console for debugging purposes.

**Syntax:**
```lua
TowerForge.Log(message1, message2, ...)
```

**Parameters:**
- One or more values to log (strings, numbers, etc.)

**Example:**
```lua
TowerForge.Log("Mod initialized successfully!")
TowerForge.Log("Registered", 5, "new facility types")
```

---

## Examples

### Example 1: Coffee Shop Facility

```lua
-- Mod metadata
ModInfo = {
    id = "coffee_shop",
    name = "Coffee Shop Mod",
    version = "1.0.0",
    author = "Coffee Lover",
    description = "Adds a cozy coffee shop facility"
}

-- Register the facility
TowerForge.RegisterFacilityType({
    id = "coffee_shop",
    name = "Coffee Shop",
    default_width = 5,
    default_capacity = 10,
    required_staff = 2,
    operating_start_hour = 6.0,   -- Opens at 6 AM
    operating_end_hour = 22.0,    -- Closes at 10 PM
    base_rent = 90.0,
    operating_cost = 25.0,
    color = 0x6F4E37FF  -- Coffee brown
})

TowerForge.Log("Coffee Shop mod loaded!")
```

### Example 2: VIP Visitor Type

```lua
-- Mod metadata
ModInfo = {
    id = "vip_visitor",
    name = "VIP Visitor Mod",
    version = "1.0.0",
    author = "Prestige Developer",
    description = "Adds VIP visitors who have high standards"
}

-- Register the visitor type
TowerForge.RegisterVisitorType({
    id = "vip",
    name = "VIP Visitor",
    move_speed = 1.0,              -- VIPs move slowly
    min_visit_duration = 300.0,    -- Stay at least 5 minutes
    max_visit_duration = 900.0,    -- Up to 15 minutes
    preferred_facilities = {
        "Hotel",
        "Restaurant",
        "Office"
    }
})

TowerForge.Log("VIP Visitor mod loaded!")
```

### Example 3: Comprehensive Mod with Multiple Additions

```lua
-- Mod metadata
ModInfo = {
    id = "entertainment_pack",
    name = "Entertainment Pack",
    version = "1.0.0",
    author = "Fun Times Studio",
    description = "Adds entertainment facilities and visitors"
}

TowerForge.Log("Loading Entertainment Pack...")

-- Register facilities
TowerForge.RegisterFacilityType({
    id = "bowling_alley",
    name = "Bowling Alley",
    default_width = 12,
    default_capacity = 30,
    required_staff = 4,
    operating_start_hour = 10.0,
    operating_end_hour = 24.0,
    base_rent = 200.0,
    operating_cost = 60.0,
    color = 0x9400D3FF  -- Purple
})

TowerForge.RegisterFacilityType({
    id = "mini_golf",
    name = "Mini Golf",
    default_width = 15,
    default_capacity = 20,
    required_staff = 2,
    operating_start_hour = 9.0,
    operating_end_hour = 21.0,
    base_rent = 150.0,
    operating_cost = 40.0,
    color = 0x00FF00FF  -- Green
})

-- Register visitor types
TowerForge.RegisterVisitorType({
    id = "party_goer",
    name = "Party Goer",
    move_speed = 3.0,
    min_visit_duration = 240.0,
    max_visit_duration = 600.0,
    preferred_facilities = {
        "bowling_alley",
        "mini_golf",
        "Restaurant"
    }
})

TowerForge.Log("Entertainment Pack loaded successfully!")
```

### Example 5: Night Janitor Staff Role

```lua
-- Mod: Night Janitor
ModInfo = {
    id = "night_janitor",
    name = "Night Janitor Mod",
    version = "1.0.0",
    author = "Community",
    description = "Adds night janitors with enhanced efficiency"
}

TowerForge.RegisterStaffRole({
    id = "night_janitor",
    name = "Night Janitor",
    work_efficiency = 1.25,        -- 25% more efficient
    default_wage = 60.0,
    shift_start_hour = 22.0,       -- 10 PM
    shift_end_hour = 6.0,          -- 6 AM
    work_type = "cleaning"
})

TowerForge.Log("Night Janitor registered!")
```

### Example 6: HVAC Technician Staff Role

```lua
-- Mod: HVAC Technician
ModInfo = {
    id = "hvac_tech",
    name = "HVAC Technician Mod",
    version = "1.0.0",
    author = "Community",
    description = "Specialized climate control maintenance"
}

TowerForge.RegisterStaffRole({
    id = "hvac_tech",
    name = "HVAC Technician",
    work_efficiency = 1.5,         -- Very efficient
    default_wage = 75.0,           -- Higher pay for specialization
    shift_start_hour = 8.0,
    shift_end_hour = 17.0,
    work_type = "maintenance"
})

TowerForge.Log("HVAC Technician registered!")
```

### Example 7: Electrical Fire Event

```lua
-- Mod: Electrical Fire Event
ModInfo = {
    id = "electrical_fire",
    name = "Electrical Fire Event",
    version = "1.0.0",
    author = "Community",
    description = "Adds dangerous electrical fire events"
}

TowerForge.RegisterEventType({
    id = "electrical_fire",
    name = "Electrical Fire",
    description = "Electrical malfunction caused a fire",
    spawn_chance = 0.0005,         -- Rare event
    duration = 0.0,                -- Doesn't go away without staff
    requires_staff_response = true,
    required_staff_type = "Firefighter",
    resolution_time = 15.0,
    satisfaction_penalty = 20.0,   -- Major penalty
    maintenance_damage = 15.0      -- Significant damage
})

TowerForge.Log("Electrical Fire event registered!")
```

### Example 8: Pest Infestation Event

```lua
-- Mod: Pest Infestation Event
ModInfo = {
    id = "pest_infestation",
    name = "Pest Infestation Event",
    version = "1.0.0",
    author = "Community",
    description = "Adds pest infestation events"
}

TowerForge.RegisterEventType({
    id = "pest_infestation",
    name = "Pest Infestation",
    description = "Pests spotted in facility",
    spawn_chance = 0.001,          -- Uncommon
    duration = 600.0,              -- 10 minutes
    requires_staff_response = true,
    required_staff_type = "Janitor",
    resolution_time = 30.0,        -- Takes 30 seconds to resolve
    satisfaction_penalty = 15.0,
    maintenance_damage = 5.0       -- Minor damage
})

TowerForge.Log("Pest Infestation event registered!")
```

---

## Security and Sandboxing

For security reasons, Lua mods run in a sandboxed environment with restricted access:

**Allowed:**
- ✅ Registering custom facilities, visitor types, staff roles, and event types
- ✅ Logging messages
- ✅ Basic Lua operations (math, string manipulation, tables, etc.)
- ✅ Defining mod metadata

**Restricted/Disabled:**
- ❌ File system access (`io` library removed)
- ❌ System command execution (`os.execute` disabled)
- ❌ Loading external files (`dofile`, `loadfile`, `require` disabled)
- ❌ Accessing environment variables
- ❌ Network access

This ensures that mods cannot harm your system or compromise security while still allowing creative content creation.

**Error Handling:**
If your mod encounters an error, it will:
1. Log the error message to the console
2. Mark the mod as failed to load
3. Continue loading other mods without crashing the game

Always test your mods and check the console for error messages!

---

## Best Practices

1. **Use unique IDs**: Make sure your facility and visitor type IDs are unique to avoid conflicts with other mods
2. **Provide metadata**: Always include complete `ModInfo` with name, version, author, and description
3. **Test thoroughly**: Test your mod with different configurations before sharing
4. **Balance gameplay**: Consider game balance when setting economic values
5. **Use meaningful names**: Give your facilities and visitors descriptive, immersive names
6. **Log important events**: Use `TowerForge.Log()` to help with debugging
7. **Document your mod**: Add comments to explain what your mod does

---

## Troubleshooting

**Mod not loading:**
- Check the console for error messages
- Verify your `.lua` file is in the `mods/` directory
- Ensure your Lua syntax is correct
- Make sure required fields (`id` and `name`) are provided

**Mod loaded but not working:**
- Verify the mod is enabled in the Mods menu
- Check that your IDs are unique and don't conflict with existing content
- Review the console logs for any warnings

**Color not displaying correctly:**
- Make sure you're using the correct hex format: 0xRRGGBBAA
- Remember the last two digits (AA) are the alpha channel (FF = fully opaque)

---

## Contributing

Want to share your mod with the community? Consider:
- Sharing your mod file on the TowerForge forums
- Creating a GitHub repository for your mod collection
- Writing a guide for other modders
- Helping improve this documentation

Happy modding! 🏗️🎮
