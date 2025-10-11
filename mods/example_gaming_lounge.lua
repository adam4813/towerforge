-- Example Mod: Gaming Lounge Facility
-- This mod adds a new facility type: Gaming Lounge
-- A gaming lounge is an entertainment facility where visitors can play video games

-- Mod metadata
ModInfo = {
    id = "gaming_lounge",
    name = "Gaming Lounge Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds a gaming lounge facility where visitors can enjoy video games"
}

-- Log that the mod is loading
TowerForge.Log("Loading Gaming Lounge mod...")

-- Register the Gaming Lounge facility type
TowerForge.RegisterFacilityType({
    id = "gaming_lounge",
    name = "Gaming Lounge",
    
    -- Size and capacity
    default_width = 10,           -- Larger facility for gaming equipment
    default_capacity = 25,        -- Can hold 25 gamers
    
    -- Staffing
    required_staff = 3,           -- Needs 3 staff members (attendants, tech support)
    
    -- Operating hours (12 PM to midnight)
    operating_start_hour = 12.0,
    operating_end_hour = 24.0,
    
    -- Economics
    base_rent = 180.0,            -- Revenue per customer per day
    operating_cost = 50.0,        -- Daily operating costs (power, maintenance)
    
    -- Visual (color as 0xRRGGBBAA in hex)
    -- Orange color for gaming
    color = 0xFF8C00FF
})

TowerForge.Log("Gaming Lounge facility type registered successfully!")
