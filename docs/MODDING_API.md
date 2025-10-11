# TowerForge Lua Modding API Documentation

Welcome to the TowerForge Modding API! This guide will help you create custom facilities and visitor types for TowerForge using Lua scripting.

## Table of Contents
- [Getting Started](#getting-started)
- [Mod Structure](#mod-structure)
- [API Reference](#api-reference)
  - [TowerForge.RegisterFacilityType](#towerforgeregisterfacilitytype)
  - [TowerForge.RegisterVisitorType](#towerforgeregistervisitortype)
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

---

## Security and Sandboxing

For security reasons, Lua mods run in a sandboxed environment with restricted access:

**Allowed:**
- ✅ Registering custom facilities and visitor types
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
