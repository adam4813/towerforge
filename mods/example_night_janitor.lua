-- Example Mod: Night Janitor Staff Role
-- This mod adds a custom night janitor role with enhanced efficiency during nighttime

-- Mod metadata
ModInfo = {
    id = "night_janitor",
    name = "Night Janitor Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds a specialized night janitor role with increased efficiency during nighttime hours"
}

-- Log that the mod is loading
TowerForge.Log("Loading Night Janitor mod...")

-- Register the Night Janitor staff role
TowerForge.RegisterStaffRole({
    id = "night_janitor",
    name = "Night Janitor",
    
    -- Work characteristics
    work_efficiency = 1.25,        -- 25% more efficient than regular janitors
    default_wage = 60.0,           -- Slightly higher wage for night shift
    
    -- Night shift hours (10 PM to 6 AM)
    shift_start_hour = 22.0,
    shift_end_hour = 6.0,
    
    -- Work behavior
    work_type = "cleaning"         -- Performs cleaning tasks
})

TowerForge.Log("Night Janitor staff role registered successfully!")
TowerForge.Log("Night janitors work from 10 PM to 6 AM with 25% increased efficiency")
