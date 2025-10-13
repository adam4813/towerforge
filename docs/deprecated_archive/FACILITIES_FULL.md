# Facilities (Merged)

This file consolidates `docs/FACILITIES.md` and `docs/ADVANCED_FACILITIES.md` into a single facilities reference.

Overview
- Facility types: Lobby, Office, Residential, RetailShop, Restaurant, Hotel, Elevator, plus advanced types (Gym, Arcade, Theater, Conference Hall, Flagship Store).
- Each facility has: type, default width, capacity, staffing requirements (if any), operating hours, build cost, and purpose.

Core fields (BuildingComponent)
- type, floor, width, capacity, current_occupancy, current_staff, job_openings, operating_start_hour, operating_end_hour

Examples
- Lobby: 10 tiles, capacity 50, must be on ground floor.
- Office: 8 tiles, capacity 20, requires elevator access for floors >1.
- Gym: 7 tiles, capacity 25, staff 3, operating 9:00-21:00.

Staffing system
- `GetRequiredEmployees()`, `HasJobOpenings()`, `IsOperational(current_hour)` implemented as described in ADVANCED_FACILITIES.
- Understaffed facilities don't generate revenue or benefits.

Placement & Build
- Place via Build Menu and PlacementSystem; costs and build times enforced. See `PLACEMENT_SYSTEM.md` for details.

Rendering & colors
- Each facility mapped to a color used by renderer and HUD.

Future
- Upgrades, adjacency bonuses, inventory, staff scheduling.

References
- Implementation: `src/core/facility_manager.cpp`, component definitions in `include/core/components.hpp`.

