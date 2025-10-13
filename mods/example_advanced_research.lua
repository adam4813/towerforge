-- Example Mod: Advanced Research Nodes
-- This mod demonstrates various types of research nodes with different prerequisites

-- Mod metadata
ModInfo = {
    id = "advanced_research",
    name = "Advanced Research Examples",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Example research nodes showcasing conditional unlocks and prerequisites"
}

-- Log that the mod is loading
TowerForge.Log("Loading Advanced Research Examples mod...")

-- Example 1: Theater unlock - requires higher star rating and specific facilities
TowerForge.RegisterResearchNode({
    id = "theater_unlock",
    name = "Entertainment Complex",
    description = "Unlock theater facilities for live performances",
    icon = "🎭",
    type = "FacilityUnlock",
    cost = 50,
    grid_row = 2,
    grid_column = 0,
    effect_target = "Theater",
    prerequisites = {},
    min_star_rating = 3,
    min_population = 100,
    required_facilities = {"Restaurant", "Hotel"}  -- Must have restaurant and hotel built
})

-- Example 2: Gym unlock - requires population and management office
TowerForge.RegisterResearchNode({
    id = "gym_unlock",
    name = "Fitness Center",
    description = "Unlock gym facilities for health and wellness",
    icon = "💪",
    type = "FacilityUnlock",
    cost = 40,
    grid_row = 1,
    grid_column = 4,
    effect_target = "Gym",
    prerequisites = {"shop_unlock"},
    min_star_rating = 2,
    min_population = 75,
    required_facilities = {"ManagementOffice"}  -- Must have a management office
})

-- Example 3: Income boost - requires all basic facilities
TowerForge.RegisterResearchNode({
    id = "premium_services",
    name = "Premium Services",
    description = "Enhanced service quality increases income by 15%",
    icon = "💎",
    type = "IncomeBonus",
    cost = 60,
    grid_row = 2,
    grid_column = 1,
    effect_value = 0.15,  -- +15% income
    prerequisites = {"income_boost"},
    min_star_rating = 4,
    min_population = 150,
    required_facilities = {}
})

-- Example 4: Management efficiency - requires satellite offices
TowerForge.RegisterResearchNode({
    id = "management_efficiency",
    name = "Management Efficiency",
    description = "Reduces operating costs by 15% through better management",
    icon = "📊",
    type = "CostReduction",
    cost = 45,
    grid_row = 2,
    grid_column = 2,
    effect_value = 0.15,  -- 15% cost reduction
    prerequisites = {},
    min_star_rating = 3,
    min_population = 0,
    required_facilities = {"SatelliteOffice"}  -- Must have satellite office built
})

TowerForge.Log("Advanced research nodes registered successfully!")
TowerForge.Log("  - Theater unlock (3-star, 100 pop, needs Restaurant + Hotel)")
TowerForge.Log("  - Gym unlock (2-star, 75 pop, needs ManagementOffice)")
TowerForge.Log("  - Premium Services (+15% income, 4-star, 150 pop)")
TowerForge.Log("  - Management Efficiency (-15% costs, needs SatelliteOffice)")
