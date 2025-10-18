# Facility Adjacency System - Feature Documentation

## Overview

The Facility Adjacency System automatically calculates and applies bonuses or penalties to facilities based on their neighboring facilities. This adds strategic depth to tower design and encourages thoughtful facility placement.

## How It Works

When a facility is placed or removed, the system:
1. Checks all adjacent facilities (left, right, above, below)
2. Calculates any applicable bonuses or penalties
3. Updates the AdjacencyEffects component on affected facilities
4. Displays the effects in the facility info panel

## Adjacency Rules

### Positive Synergies

#### Entertainment & Dining
- **Restaurant + Theater** → Restaurant receives +10% revenue
- **Restaurant + Arcade** → Restaurant receives +8% revenue
- **Theater/Arcade + Restaurant** → Entertainment venue receives +10% traffic

*Strategy:* Place restaurants near entertainment venues to increase both food sales and visitor traffic to shows/games.

#### Residential Amenities
- **Residential + Gym** → Residential receives +5% satisfaction
- **Office + Restaurant** → Office receives +5% satisfaction

*Strategy:* Place amenities near residential and office facilities to boost tenant satisfaction.

#### Shopping Districts
- **Retail Shop + Retail Shop** → Both receive +5% traffic
- **Retail Shop + Flagship Store** → Both receive +5% traffic

*Strategy:* Cluster retail facilities together to create popular shopping districts that attract more customers.

#### Hospitality Synergies
- **Hotel + Restaurant** → Hotel receives +8% satisfaction
- **ConferenceHall + Hotel** → ConferenceHall receives +10% revenue

*Strategy:* Place hotels near restaurants and conference facilities to create hospitality zones with enhanced performance.

### Negative Penalties

#### Noise Complaints
- **Residential + Arcade** → Residential receives -8% satisfaction (noisy arcade)
- **Residential + Theater** → Residential receives -5% satisfaction (noisy theater)

*Strategy:* Keep noisy entertainment venues away from residential facilities, or use the noise as a penalty to balance high-revenue layouts.

## User Interface

### Facility Info Panel

When clicking on a facility, the info panel displays an "Adjacency Effects" section showing:

```
--- Adjacency Effects ---
+10% revenue: next to theater          (green text)
+5% satisfaction: gym nearby            (green text)
-8% satisfaction: noisy arcade nearby   (orange text)
```

- **Green text** indicates bonuses
- **Orange text** indicates penalties
- Effects are automatically recalculated when neighbors change

### Visual Feedback

The window height dynamically adjusts to show all active effects, ensuring players always have a complete view of how placement affects their facilities.

## Strategic Considerations

### Planning Districts

Create specialized zones:
- **Entertainment District**: Theater + Arcade + Restaurant
- **Shopping District**: Multiple Retail Shops + Flagship Store
- **Business District**: Offices + Restaurants
- **Hospitality Zone**: Hotel + Restaurant + ConferenceHall

### Balancing Tradeoffs

Sometimes optimal layouts require accepting penalties:
- High-density towers may need to place arcades near residential
- Maximize revenue by accepting some satisfaction penalties
- Use elevators and offices as "buffers" between incompatible facility types

### Optimizing Existing Towers

When redesigning:
1. Check facility info panels to see current adjacency effects
2. Identify facilities with negative effects
3. Reorganize to maximize positive synergies
4. Consider vertical stacking (above/below) for space-efficient layouts

## Examples

### Example 1: Restaurant District
```
Floor 2: [Restaurant] [Theater]
Effect: Restaurant gets +10% revenue from theater
        Theater gets +10% traffic from restaurant
```

### Example 2: Mixed-Use Building (Problematic)
```
Floor 3: [Residential] [Arcade]
Effect: Residential gets -8% satisfaction (noise penalty)
```

### Example 3: Vertical Synergy
```
Floor 5: [Hotel]
Floor 4: [Restaurant]
Effect: Hotel gets +8% satisfaction from restaurant below
```

### Example 4: Shopping Mall
```
Floor 1: [RetailShop] [RetailShop] [FlagshipStore]
Effect: All three get +5% traffic from clustering
```

## Testing

Run the adjacency test suite to verify functionality:
```bash
./build/native/bin/Debug/test_adjacency
```

This validates:
- Horizontal adjacency detection
- Vertical adjacency detection
- Effect calculation accuracy
- Dynamic updates on placement/removal

## Future Enhancements

Potential additions:
- Distance-based effects (weaker bonuses further away)
- More complex rules (e.g., 3+ facilities creating special synergies)
- Visual indicators on the tower view (colored outlines, icons)
- Mod support for custom adjacency rules
- Time-based effects (noise only during operating hours)
