-- Complete Integration Example: Emergency Response Team
-- This mod demonstrates full integration of custom staff roles and event types

-- Mod metadata
ModInfo = {
    id = "emergency_response_team",
    name = "Emergency Response Team",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds a complete emergency response system with specialized staff and events"
}

TowerForge.Log("=== Loading Emergency Response Team Mod ===")

-- Register a specialized emergency coordinator role
TowerForge.RegisterStaffRole({
    id = "emergency_coordinator",
    name = "Emergency Coordinator",
    work_efficiency = 1.5,         -- Very efficient at handling emergencies
    default_wage = 80.0,           -- Higher pay for specialized role
    shift_start_hour = 0.0,        -- 24/7 coverage
    shift_end_hour = 24.0,
    work_type = "emergency"        -- Handles emergency events
})

TowerForge.Log("Registered Emergency Coordinator role")

-- Register a building inspector role (maintenance specialist)
TowerForge.RegisterStaffRole({
    id = "building_inspector",
    name = "Building Inspector",
    work_efficiency = 1.3,         -- More efficient than regular maintenance
    default_wage = 70.0,
    shift_start_hour = 7.0,        -- 7 AM to 7 PM
    shift_end_hour = 19.0,
    work_type = "maintenance"      -- Performs maintenance work
})

TowerForge.Log("Registered Building Inspector role")

-- Register a night custodian role (cleaning specialist)
TowerForge.RegisterStaffRole({
    id = "night_custodian",
    name = "Night Custodian",
    work_efficiency = 1.4,         -- Very efficient cleaner
    default_wage = 65.0,
    shift_start_hour = 20.0,       -- 8 PM to 4 AM
    shift_end_hour = 4.0,
    work_type = "cleaning"         -- Performs cleaning work
})

TowerForge.Log("Registered Night Custodian role")

-- Register a gas leak event (emergency)
TowerForge.RegisterEventType({
    id = "gas_leak",
    name = "Gas Leak",
    description = "A dangerous gas leak has been detected",
    spawn_chance = 0.0003,         -- 0.03% chance per hour (very rare)
    duration = 0.0,                -- Must be resolved, doesn't go away
    requires_staff_response = true,
    required_staff_type = "emergency_coordinator",
    resolution_time = 20.0,        -- Takes 20 seconds to resolve
    satisfaction_penalty = 25.0,   -- Major satisfaction penalty
    maintenance_damage = 20.0      -- Significant damage to facility
})

TowerForge.Log("Registered Gas Leak event")

-- Register a plumbing failure event (maintenance)
TowerForge.RegisterEventType({
    id = "plumbing_failure",
    name = "Plumbing Failure",
    description = "Pipes have burst and need immediate repair",
    spawn_chance = 0.0008,         -- 0.08% chance per hour
    duration = 900.0,              -- Lasts 15 minutes if not fixed
    requires_staff_response = true,
    required_staff_type = "building_inspector",
    resolution_time = 45.0,        -- Takes 45 seconds to fix
    satisfaction_penalty = 18.0,
    maintenance_damage = 25.0      -- Heavy maintenance damage
})

TowerForge.Log("Registered Plumbing Failure event")

-- Register a major spill event (cleaning)
TowerForge.RegisterEventType({
    id = "major_spill",
    name = "Major Spill",
    description = "A large spill requires immediate cleanup",
    spawn_chance = 0.002,          -- 0.2% chance per hour (uncommon)
    duration = 600.0,              -- Lasts 10 minutes if not cleaned
    requires_staff_response = true,
    required_staff_type = "night_custodian",
    resolution_time = 35.0,        -- Takes 35 seconds to clean
    satisfaction_penalty = 12.0,
    maintenance_damage = 5.0       -- Minor damage
})

TowerForge.Log("Registered Major Spill event")

TowerForge.Log("=== Emergency Response Team Mod Loaded Successfully ===")
TowerForge.Log("Added 3 custom staff roles and 3 custom event types")
TowerForge.Log("Emergency Coordinator, Building Inspector, and Night Custodian are now available!")
TowerForge.Log("Watch out for Gas Leaks, Plumbing Failures, and Major Spills!")
