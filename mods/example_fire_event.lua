-- Example Mod: Electrical Fire Event
-- This mod adds a custom fire event type that can occur in facilities

-- Mod metadata
ModInfo = {
    id = "electrical_fire",
    name = "Electrical Fire Event Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds electrical fire events that require firefighter response"
}

-- Log that the mod is loading
TowerForge.Log("Loading Electrical Fire event mod...")

-- Register the Electrical Fire event type
TowerForge.RegisterEventType({
    id = "electrical_fire",
    name = "Electrical Fire",
    description = "An electrical malfunction has caused a fire in this facility",
    
    -- Occurrence parameters
    spawn_chance = 0.0005,         -- 0.05% chance per hour per facility (rare)
    duration = 0.0,                -- Instant event, doesn't go away without staff
    
    -- Staff response
    requires_staff_response = true,
    required_staff_type = "Firefighter",
    resolution_time = 15.0,        -- Takes 15 seconds to extinguish
    
    -- Impact on facility
    satisfaction_penalty = 20.0,   -- Major satisfaction hit while active
    maintenance_damage = 15.0      -- Causes more damage than regular fire
})

TowerForge.Log("Electrical Fire event type registered successfully!")
TowerForge.Log("Electrical fires are rare but cause more damage than regular fires")
