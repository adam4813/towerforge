-- Example Mod: Tourist Visitor Type
-- This mod adds a new visitor type: Tourist
-- Tourists visit the tower to see the sights, take photos, and shop

-- Mod metadata
ModInfo = {
    id = "tourist_visitor",
    name = "Tourist Visitor Type Mod",
    version = "1.0.0",
    author = "TowerForge Community",
    description = "Adds tourist visitors who explore the tower and visit shops"
}

-- Log that the mod is loading
TowerForge.Log("Loading Tourist Visitor Type mod...")

-- Register the Tourist visitor type
TowerForge.RegisterVisitorType({
    id = "tourist",
    name = "Tourist",
    
    -- Movement (tourists move slower as they look around)
    move_speed = 1.5,
    
    -- Visit duration (tourists stay longer to explore)
    min_visit_duration = 180.0,   -- At least 3 minutes
    max_visit_duration = 600.0,   -- Up to 10 minutes
    
    -- Preferred facilities (what tourists like to visit)
    preferred_facilities = {
        "RetailShop",      -- Shop for souvenirs
        "Restaurant",      -- Try local food
        "gaming_lounge",   -- Visit entertainment facilities (if gaming lounge mod is active)
        "Lobby"            -- Spend time in the lobby
    }
})

TowerForge.Log("Tourist visitor type registered successfully!")
