# Visitor Needs & Satisfaction System

## Overview
The Visitor Needs & Satisfaction System adds depth to the tower simulation by giving visitors individual personalities and needs that drive their behavior.

## Features

### Visitor Archetypes
Each visitor is assigned one of four personality types when they spawn:

1. **Business Person** (25% of visitors)
   - Higher hunger needs (gets hungry faster)
   - Lower entertainment needs
   - Focused on efficiency

2. **Tourist** (25% of visitors)
   - High entertainment needs
   - Moderate comfort needs
   - Seeks fun and sightseeing experiences

3. **Shopper** (25% of visitors)
   - High shopping desires
   - Lower initial hunger
   - Primarily interested in retail

4. **Casual Visitor** (25% of visitors)
   - Balanced needs across all categories
   - General visitor behavior

### Need Types
Each visitor tracks four types of needs that grow over time:

- **Hunger**: Satisfied by restaurants
- **Entertainment**: Satisfied by arcades and theaters
- **Comfort**: Satisfied by hotels
- **Shopping**: Satisfied by retail shops and flagship stores

### Need Mechanics
- Needs start at different levels based on archetype
- Needs increase over time at archetype-specific rates
- When a need exceeds 60%, visitors actively seek matching facilities
- Interacting with facilities reduces the corresponding need
- Facilities reduce needs by approximately 40 points over 15-30 seconds

### Satisfaction System
- Visitor satisfaction is calculated based on how well their needs are being met
- Lower unmet needs = higher satisfaction
- Critically high needs (>80%) cause frustration and satisfaction penalties
- Visitors may leave early if all needs are satisfied (<30% on all needs)
- Visitors will definitely leave when maximum visit duration is reached

## User Interface

### Person Info Panel
When you click on a visitor, the Person Info panel displays:

**Basic Information:**
- Name
- NPC Type: "Visitor"
- Profile: The visitor's archetype (e.g., "Tourist")

**Current Status:**
- Current activity and state
- Current floor and destination
- Wait time (if applicable)

**Visitor Needs Section:**
A dedicated section showing all four needs with color-coded values:
- **Green**: Need below 30% (satisfied)
- **Yellow**: Need 30-60% (moderate)
- **Red**: Need above 60% (urgent)

**Satisfaction Score:**
- Overall satisfaction percentage with emoji indicator
- :) = 80%+
- :| = 60-80%
- :/ = 40-60%
- :( = below 40%

## Gameplay Impact

### Player Strategy
- **Facility Placement**: Strategic placement of restaurants, shops, and entertainment venues
- **Tower Composition**: Balance different facility types to serve diverse visitor needs
- **Revenue**: Satisfied visitors stay longer and generate more income
- **Tower Rating**: Average visitor satisfaction affects tower rating and progression

### Emergent Behavior
- Visitors naturally flow between facilities based on their needs
- Different visitor types create varied traffic patterns
- Peak demand times for different facility types
- Natural feedback on tower design effectiveness

## Technical Details

### Component Structure
```cpp
struct VisitorNeeds {
    float hunger;           // 0-100
    float entertainment;    // 0-100
    float comfort;          // 0-100
    float shopping;         // 0-100
    VisitorArchetype archetype;
};
```

### Systems
1. **Visitor Needs Growth**: Increases needs over time
2. **Needs-Driven Behavior**: Directs visitors to appropriate facilities
3. **Facility Interaction**: Handles need reduction during facility use
4. **Satisfaction Calculation**: Updates satisfaction based on unmet needs
5. **Needs Display**: Updates UI status text

### Performance
- Needs update every 1 second
- Behavior checks every 5 seconds
- Minimal performance impact
- Scales well with visitor count

## Testing Instructions

### Manual Testing
1. **Start the game** and build a tower with various facilities
2. **Create facilities**:
   - At least one restaurant
   - At least one retail shop
   - At least one entertainment venue (arcade or theater)
   - At least one hotel

3. **Spawn visitors** naturally or wait for automatic spawning

4. **Click on visitors** to open Person Info panel

5. **Observe**:
   - Different visitor archetypes appear
   - Need levels change over time (increase when not at facility)
   - Visitors seek facilities when needs are high
   - Needs decrease when visitors interact with facilities
   - Satisfaction changes based on need levels

6. **Test edge cases**:
   - Visitor with all needs low (<30%) should leave soon after interaction
   - Visitor with very high need (>80%) should have low satisfaction
   - Different archetypes should show different need patterns

### Expected Behavior
- Business People should get hungry quickly
- Tourists should seek entertainment frequently
- Shoppers should prioritize retail facilities
- Casual visitors should have balanced behavior
- All visitors should show green/yellow/red need indicators in UI
- Satisfaction should correlate inversely with average need levels

## Future Enhancements
- Additional visitor archetypes
- More sophisticated need interactions
- Need-based revenue multipliers
- Visitor preferences for specific facilities
- Social needs (groups of visitors)
- Time-of-day need variations
