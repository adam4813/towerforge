-- Example Mod: HVAC Technician Staff Role
-- This mod adds a specialized HVAC (Heating, Ventilation, Air Conditioning) technician

-- Mod metadata
ModInfo = {
    id = "hvac_technician",
    name = "HVAC Technician Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds HVAC technicians who specialize in climate control systems"
}

-- Log that the mod is loading
TowerForge.Log("Loading HVAC Technician mod...")

-- Register the HVAC Technician staff role
TowerForge.RegisterStaffRole({
    id = "hvac_tech",
    name = "HVAC Technician",
    
    -- Work characteristics
    work_efficiency = 1.5,         -- Very efficient at specific maintenance tasks
    default_wage = 75.0,           -- Higher wage for specialized skill
    
    -- Standard business hours
    shift_start_hour = 8.0,
    shift_end_hour = 17.0,
    
    -- Work behavior
    work_type = "maintenance"      -- Performs maintenance tasks
})

TowerForge.Log("HVAC Technician staff role registered successfully!")
TowerForge.Log("HVAC technicians provide specialized climate control maintenance")
