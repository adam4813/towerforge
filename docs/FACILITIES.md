# Facility Types Documentation

This document describes the different facility types available in TowerForge and their attributes.

## Core Facility Types

### Office
**Purpose**: Commercial office space for workers  
**Gameplay Role**: Generates income through rent from businesses. Attracts office workers during business hours.  
**Default Attributes**:
- Width: 8 grid cells
- Capacity: 20 people
- Occupancy Pattern: High during business hours (9AM-5PM), low at other times
- Income: Moderate rent income

**Placement Rules**:
- Can be placed on any floor except ground floor
- Requires elevator access for floors above 1
- Multiple offices can exist on the same floor

---

### Residential
**Purpose**: Condominium units for tower residents  
**Gameplay Role**: Provides housing for residents. Generates income through rent or property sales.  
**Default Attributes**:
- Width: 6 grid cells
- Capacity: 4 people (family unit)
- Occupancy Pattern: High during evenings/nights, low during business hours
- Income: Steady rent income

**Placement Rules**:
- Can be placed on any floor except ground floor
- Requires elevator access for floors above 1
- Residents need access to lobby and other facilities

---

### RetailShop
**Purpose**: Retail shops selling goods and services  
**Gameplay Role**: Provides shopping options for residents and visitors. Generates income through sales.  
**Default Attributes**:
- Width: 4 grid cells
- Capacity: 15 people (customers + staff)
- Occupancy Pattern: Moderate during business hours, low at other times
- Income: Variable based on foot traffic

**Placement Rules**:
- Best placed on lower floors for accessibility
- Can be placed on ground floor or above
- Benefits from proximity to residential areas and lobby

---

### Lobby
**Purpose**: Main entrance and exit point for the tower  
**Gameplay Role**: Required facility that serves as the primary access point. All residents, workers, and visitors pass through the lobby.  
**Default Attributes**:
- Width: 10 grid cells
- Capacity: 50 people (transient)
- Occupancy Pattern: Consistent throughout day with peaks during rush hours
- Income: None (infrastructure)

**Placement Rules**:
- **Must** be placed on ground floor (floor 0)
- Only one lobby required per tower
- Should span significant width for traffic flow

---

## Legacy Facility Types

The following facility types are retained for compatibility but are not primary focus types:

### Restaurant
**Purpose**: Food service facility  
**Default Attributes**: Width: 6, Capacity: 30

### Hotel
**Purpose**: Temporary lodging  
**Default Attributes**: Width: 10, Capacity: 40

### Elevator
**Purpose**: Vertical transportation  
**Default Attributes**: Width: 2, Capacity: 8

---

## Technical Implementation

### Data Structure
All facilities are represented using the `BuildingComponent` ECS component with the following fields:
```cpp
struct BuildingComponent {
    Type type;              // Facility type enum
    int floor;              // Floor number (0-based)
    int width;              // Width in grid cells
    int capacity;           // Maximum occupancy
    int current_occupancy;  // Current number of people
};
```

### Facility Creation
Use the `FacilityManager` to create facilities:
```cpp
auto& facility_mgr = ecs_world.GetFacilityManager();
auto lobby = facility_mgr.CreateFacility(
    BuildingComponent::Type::Lobby,
    0,      // floor
    0,      // column
    10,     // width (or 0 for default)
    "MainLobby"  // optional name
);
```

### Facility Removal
```cpp
// Remove by entity
facility_mgr.RemoveFacility(facility_entity);

// Remove by grid position
facility_mgr.RemoveFacilityAt(floor, column);
```

---

## Future Enhancements

Planned additions to the facility system:
- Economic simulation (rent, income, expenses)
- Time-based occupancy patterns
- Facility upgrades and customization
- Special facility requirements (power, water, etc.)
- Facility happiness/satisfaction ratings
- Advanced facility types (gym, pool, parking, etc.)

---

## Visual Design

Each facility type has a distinctive color for easy identification:

| Facility Type | Color Code | Raylib Color | Description |
|--------------|------------|--------------|-------------|
| Office | SKYBLUE | Light blue | Professional, clean look |
| Residential | PURPLE | Purple | Warm, homey feel |
| RetailShop | GREEN | Green | Commercial, vibrant |
| Lobby | GOLD | Gold | Prestigious, welcoming |
| Restaurant | RED | Red | Energetic, food-related |
| Hotel | DARKBLUE | Dark blue | Luxurious, calm |
| Elevator | GRAY | Gray | Utilitarian, neutral |

These colors are used in the rendering system to visually distinguish facilities on the tower grid.
