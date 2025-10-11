-- Example Mod: Hotel Research Node
-- This mod adds a research node that unlocks the Hotel facility type
-- Hotels are unlocked after reaching 2-star rating and having at least 50 population

-- Mod metadata
ModInfo = {
    id = "hotel_research",
    name = "Hotel Research Node",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds a research node to unlock Hotel facilities via tower points"
}

-- Log that the mod is loading
TowerForge.Log("Loading Hotel Research mod...")

-- Register the Hotel unlock research node
TowerForge.RegisterResearchNode({
    id = "hotel_unlock",
    name = "Luxury Lodging",
    description = "Unlock hotel facilities for temporary guests",
    icon = "🏨",
    type = "FacilityUnlock",
    
    -- Cost in tower points
    cost = 30,
    
    -- Grid position (row 1, column 3 for mid-tier)
    grid_row = 1,
    grid_column = 3,
    
    -- Effect target - specifies which facility this unlocks
    effect_target = "Hotel",
    
    -- Prerequisites - other research nodes that must be unlocked first
    prerequisites = {"office_unlock"},
    
    -- Conditional prerequisites - tower must meet these requirements
    min_star_rating = 2,        -- Requires 2-star tower rating
    min_population = 50,        -- Requires at least 50 people in tower
    required_facilities = {}     -- No specific facilities required beyond office
})

TowerForge.Log("Hotel research node registered successfully!")
