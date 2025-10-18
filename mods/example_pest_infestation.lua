-- Example Mod: Pest Infestation Event
-- This mod adds a pest infestation event that requires specialized cleaning

-- Mod metadata
ModInfo = {
    id = "pest_infestation",
    name = "Pest Infestation Event Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds pest infestation events that require janitor response"
}

-- Log that the mod is loading
TowerForge.Log("Loading Pest Infestation event mod...")

-- Register the Pest Infestation event type
TowerForge.RegisterEventType({
    id = "pest_infestation",
    name = "Pest Infestation",
    description = "Pests have been spotted in this facility and need to be dealt with",
    
    -- Occurrence parameters
    spawn_chance = 0.001,          -- 0.1% chance per hour per facility
    duration = 600.0,              -- Lasts 10 minutes if not dealt with
    
    -- Staff response
    requires_staff_response = true,
    required_staff_type = "Janitor",
    resolution_time = 30.0,        -- Takes 30 seconds to handle
    
    -- Impact on facility
    satisfaction_penalty = 15.0,   -- Significant satisfaction penalty
    maintenance_damage = 5.0       -- Minor maintenance impact
})

TowerForge.Log("Pest Infestation event type registered successfully!")
TowerForge.Log("Pest infestations reduce satisfaction and require janitor attention")
