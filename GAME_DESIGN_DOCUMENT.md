# TowerForge Game Design Document

**Version**: 1.0  
**Last Updated**: 2025-10-25  
**Status**: Living Document

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Game Concept](#2-game-concept)
3. [Core Gameplay](#3-core-gameplay)
4. [Game Systems](#4-game-systems)
5. [Content Design](#5-content-design)
6. [Player Experience](#6-player-experience)
7. [Progression & Monetization](#7-progression--monetization)
8. [Technical Scope](#8-technical-scope)
9. [Success Criteria](#9-success-criteria)
10. [References](#10-references)

---

## 1. Executive Summary

### 1.1 Game Overview

**Title**: TowerForge  
**Genre**: Skyscraper Management Simulation  
**Platform**: PC (Windows, Linux, macOS)  
**Target Audience**: Strategy/simulation enthusiasts, ages 18-45  
**Business Model**: Free and Open Source (MIT/similar license)  
**Development Status**: Alpha (core systems functional)

### 1.2 Elevator Pitch

TowerForge is a cozy, open-source skyscraper simulation where players create and nurture their own vertical community at their own pace. Inspired by SimTower and the cozy games movement, it's about creative expression, watching your tower come to life, and building a space that feels uniquely yours.

### 1.3 Core Design Pillars (Cozy Game Philosophy)

1. **Creative Expression**: Every tower is unique — a canvas for architectural and community design
2. **Gentle Simulation**: No failure states, no time pressure — just peaceful building and growth
3. **Player Comfort**: Extensive undo/redo, helpful tooltips, optional guidance, stress-free experience
4. **Accessibility First**: Inclusive design ensuring everyone can enjoy the cozy experience
5. **Community & Life**: Watch your tower's residents live their lives, form routines, and create stories

### 1.4 Unique Selling Points

- Only open-source vertical city sim with modern C++20/ECS architecture
- Comprehensive accessibility features (keyboard nav, high contrast, font scaling)
- Lua modding API for custom content without C++ knowledge
- Unlimited undo/redo with visual command history
- Real-time analytics overlays for data-driven decisions
- True multi-platform support (no compromises)

---

## 2. Game Concept

### 2.1 High-Level Concept

Players start with an empty lot and begin building their dream skyscraper at their own pace. There are no failure states or time limits — just the joy of creating a unique vertical community, watching residents live their daily lives, and shaping a space that reflects your personal style. Each decision is an opportunity for creative expression rather than optimization.

### 2.2 Core Fantasy

**"You are the nurturing creator of a cozy vertical village where people live, work, and thrive."**

Players experience:
- The joy of watching your tower slowly come to life
- The meditative pleasure of arranging spaces and facilities
- The warmth of seeing residents follow their daily routines
- The satisfaction of creating something that feels like home
- The freedom to experiment without fear of failure

### 2.3 Design Influences

| Source | Influence | Implementation |
|--------|-----------|----------------|
| **SimTower (Maxis, 1994)** | Core gameplay loop, facility types | Gentle pacing, no failure states |
| **Yoot Tower (OPeNBooK, 1998)** | Expanded facility variety | Enhanced with personality |
| **Animal Crossing** | Non-competitive, self-expression | No pressure, creative building |
| **Stardew Valley** | Cozy simulation, community focus | Watching lives unfold |
| **A Short Hike** | Gentle exploration, no stress | Player-directed experience |
| **The Sims** | Individual agent simulation | Person routines and stories |
| **Cozy Games Manifesto** | Core philosophy | No failure, meditative pace, warmth |

### 2.4 Scope Boundaries

**In Scope:**
- Single-tower management
- Vertical expansion (ground to 100+ floors, basements)
- Time simulation with day/night cycles
- Person AI with basic needs and satisfaction
- Economic simulation (income, expenses, balance)
- Research and progression systems
- Save/load functionality
- Lua modding support

**Out of Scope (Future):**
- Multiple simultaneous towers
- Multiplayer/competitive modes
- Disasters (fires, earthquakes)
- Detailed interior decoration
- 3D rendering
- Real-time multiplayer

---

## 3. Core Gameplay

### 3.1 Core Gameplay Loop

```
┌──────────────────────────────────────────────┐
│                                              │
│  Build Facilities → Attract Tenants/Visitors│
│         ↓                        ↓           │
│   Earn Revenue         Manage Satisfaction  │
│         ↓                        ↓           │
│    Expand Tower        Optimize Operations  │
│         ↓                        ↓           │
│  Unlock Research     Achieve Star Ratings   │
│         └───────────────┘                    │
└──────────────────────────────────────────────┘
```

### 3.2 Moment-to-Moment Gameplay

**Time Scale**: Simulation runs at your preferred pace (pause anytime, 1x, 2x, 4x, 8x) - take breaks whenever you want

**Player Activities (cozy gameplay moments)**:

1. **Creative Building** (whenever inspiration strikes)
   - Browse facility types at leisure
   - Experiment with different layouts and arrangements
   - Place facilities where they feel right
   - Unlimited undo if you change your mind

2. **Observing & Enjoying** (continuous, meditative)
   - Watch residents go about their daily routines
   - See patterns emerge in elevator movement
   - Notice how your tower changes throughout the day
   - Discover small stories happening in your building

3. **Gentle Adjustments** (as you feel like it)
   - Move things around to improve flow
   - Add amenities where residents might enjoy them
   - Expand when you feel ready
   - Try new facility combinations

4. **Personal Expression** (ongoing)
   - Shape your tower's personality and style
   - Create themed floors or districts
   - Balance aesthetics with functionality (or don't!)
   - Make decisions based on what feels good to you

5. **Discovery** (naturally over time)
   - Unlock new facility types as you grow
   - See your tower earn recognition organically
   - Discover what makes your residents happy
   - Find your own play style and rhythm

### 3.3 Cozy Game Design: No Win/Loss Conditions

**Philosophy**: TowerForge is about the journey, not the destination. There are no win conditions, failure states, or game overs.

**What This Means**:
- **No Bankruptcy**: Funds are a gentle pacing mechanism. If low, revenue slowly accumulates so you can continue building when ready.
- **No Failure States**: Residents may leave if unhappy, but they'll return when conditions improve. No "game over" or permanent consequences.
- **No Time Pressure**: Pause anytime, take breaks, play at your own pace. There's no rushing or deadlines.
- **Infinite Continuation**: After achieving any milestone, keep playing indefinitely. The tower is yours to shape forever.
- **Personal Goals**: Set your own objectives or just enjoy watching your tower evolve organically.

**Milestone Recognition** (optional celebration, not requirements):
- Tower growth milestones (10, 25, 50, 100 floors)
- Community milestones (100, 500, 1000 residents)
- Prosperity moments (steady income, happy residents)
- Unlocking new facilities naturally through play
- Recognition appears as gentle notifications, never demands

### 3.4 Gentle Pacing & Growth

**Natural Progression**:
- **Early Stages**: Few residents, simple needs, slow comfortable pace
- **Growing Community**: More residents bring more activity and life
- **Established Tower**: Complex daily rhythms emerge naturally
- **Mature Metropolis**: A living vertical city with its own character

**Player-Controlled Pace**:
- Starting funds (Comfortable: $50,000 | Standard: $25,000 | Modest: $10,000)
- Simulation speed (adjust anytime to match your mood)
- Optional tutorial system (gentle guidance, never required)
- Pause and save anytime without penalty

---

## 4. Game Systems (Interconnected Cozy Gameplay)

**Design Philosophy**: TowerForge's systems work together to create **organic, emergent cozy gameplay**. Each system is meaningful on its own but creates magic when combined. All systems support the cozy principles of:
- **Optional engagement**: Automate or manage personally
- **Gentle consequences**: Feedback, not punishment
- **Emergent stories**: Systems create narratives naturally
- **Player expression**: Multiple valid approaches
- **Abundance mindset**: Resources support creativity

**How Systems Interconnect**:
```
Community Happiness ←→ Facility Quality ←→ Maintenance & Staff
         ↕                      ↕                      ↕
   Tower Growth  ←→  Economic Prosperity  ←→  Research Unlocks
         ↕                      ↕                      ↕
  Resident Lives ←→  Daily Rhythms  ←→  Seasonal Events
```

**Example Emergent Story**:
> "Your restaurant on floor 5 attracted more visitors. The janitor kept it spotless, earning high satisfaction. Happy diners visited the theater next door. Revenue increased, unlocking research for a gym. You hired a maintenance worker. The gym brought even more community members. Small choices created a thriving entertainment district."

### 4.1 Tower Grid System

**Purpose**: Physical grid representing the tower's spatial layout.

**Design**:
- **Dimensions**: Variable width (default 30 cells) × variable height (up to 100+ floors)
- **Cell Size**: 1 grid cell = 1 facility width unit
- **Floor Indexing**: 0-based (floor 0 = ground/lobby, negative = basements)
- **Dynamic Expansion**: Players add floors above or basements below on-demand

**Mechanics**:
- Building new floors costs $50 per cell
- Facilities must be placed on built floor cells
- Collision detection prevents overlapping facilities
- Spatial queries support adjacency calculations

**Strategic Considerations**:
- Horizontal expansion requires full floor construction (expensive)
- Vertical expansion adds capacity but increases elevator demand
- Floor spacing affects visual density and navigation

**Reference**: `include/core/tower_grid.hpp`, `docs/DYNAMIC_FLOOR_SYSTEM.md`

---

### 4.2 Facility System

**Purpose**: Buildings that provide services, house tenants, or generate income.

#### 4.2.1 Facility Types

| Type | Width | Capacity | Staff | Operating Hours | Build Cost | Build Time | Revenue Model |
|------|-------|----------|-------|----------------|-----------|-----------|---------------|
| **Lobby** | 10 | 50 | 0 | 24/7 | $5,000 | 10s | None (required infrastructure) |
| **Office** | 8 | 20 workers | 20 | 09:00-17:00 | $6,000 | 15s | Rent per worker/day |
| **Residential** | 6 | 4 residents | 0 | 24/7 | $8,000 | 18s | Rent per unit/day |
| **Retail Shop** | 4 | 15 customers | 2 | 09:00-21:00 | $4,000 | 12s | Foot traffic based |
| **Restaurant** | 6 | 30 diners | 4 | 09:00-21:00 | $8,000 | 20s | Per-customer revenue |
| **Hotel** | 10 | 40 rooms | 5 | 24/7 | $12,000 | 25s | Room occupancy |
| **Gym** | 7 | 25 members | 3 | 09:00-21:00 | $10,000 | 18s | Membership fees |
| **Arcade** | 5 | 20 players | 2 | 09:00-21:00 | $9,000 | 16s | Per-use fees |
| **Theater** | 8 | 50 seats | 3 | 09:00-21:00 | $15,000 | 22s | Ticket sales |
| **Conference Hall** | 9 | 60 seats | 2 | 09:00-17:00 | $13,000 | 20s | Event bookings |
| **Flagship Store** | 12 | 40 customers | 4 | 09:00-21:00 | $18,000 | 28s | High-margin retail |

#### 4.2.2 Facility Mechanics (Cozy Management Depth)

**Philosophy**: Facility management adds **meaningful depth** to caring for your tower, not tedious micromanagement. All systems are **optional and automatable** while rewarding thoughtful care.

**Staffing** (Caring for Your Community):
- Facilities need staff to operate (part of community building)
- **Gentle nudges**: Understaffed facilities show job openings but don't fail catastrophically
- **Automation option**: Can auto-hire staff or manage manually (player choice)
- **Emergent stories**: Staff become part of your tower's character

**Operating Hours** (Natural Rhythms):
- Each facility has natural operating hours reflecting real-world rhythms
- Hotels operate 24/7; offices 09:00-17:00; entertainment extends evenings
- Creates organic daily patterns to observe and enjoy
- No pressure to override; embraces natural flow

**Maintenance & Cleanliness** (Nurturing Care):
- **Optional depth**: Facilities naturally accumulate wear and need care
- **Gentle consequences**: Neglected facilities see reduced satisfaction (not revenue collapse)
- **Hiring janitors/maintenance**: Add staff who care for your tower
- **Visual feedback**: See your tower sparkle when well-maintained
- **Cozy framing**: You're nurturing spaces people love

**Economics** (Abundance-Based):
- **Revenue**: Flows steadily from occupied, happy facilities
- **Expenses**: Manageable operating costs (never punitive)
- **Quality Multiplier**: Happiness increases prosperity (0.5x to 2.0x)
- **No bankruptcy**: Income accumulates; you can always afford your next project
- **Growth mindset**: Economy supports expansion, not survival

**Satisfaction Factors** (Thoughtful Placement):
- **Adjacency bonuses**: Thoughtful placement rewarded (restaurant + theater thrive together)
- **Community balance**: Happy residents attract more residents
- **Elevator access**: Smooth transportation improves satisfaction
- **Gentle nudges**: Low satisfaction is feedback, not punishment
- **Player expression**: Many valid approaches to happiness

**Community Safety** (Municipal Services):
- **Fire & Security**: Optional hiring of firefighters, security officers
- **Gentle incidents**: Small fires, minor shoplifting (manageable, not catastrophic)
- **Cozy framing**: Services reinforce tower as protected, cared-for community
- **Emergent stories**: "Security stopped a shoplifter!" "Fire department contained a kitchen fire!"
- **No cascading disasters**: Events are isolated and recoverable

**Placement Guidelines** (Creative Expression):
- **Lobby**: Ground floor, welcoming entrance to your community
- **Offices**: Upper floors for professional atmosphere
- **Residentials**: Anywhere with elevator access; create neighborhoods
- **Retail/Restaurants**: Lower floors for foot traffic, or anywhere you envision
- **Creative freedom**: Guidelines, not rules; express your vision

**Cozy Alignment**:
- ✅ **Optional depth**: Engage as much or little as you want
- ✅ **Automatable**: Can delegate to systems or manage personally
- ✅ **Gentle feedback**: Issues are nudges, not crises
- ✅ **Emergent stories**: Staff and events create narrative
- ✅ **Rewarding thoughtfulness**: Care pays off in happiness and beauty

**Reference**: `include/core/components.hpp` (`BuildingComponent`), `docs/FACILITIES.md`

---

### 4.3 Person AI System

**Purpose**: Simulate individual people moving through the tower with needs and behaviors.

#### 4.3.1 Person State Machine

```
┌─────────┐      ┌─────────┐     ┌──────────────────┐
│  Idle   │─────→│ Walking │────→│ WaitingForElevator│
└─────────┘      └─────────┘     └──────────────────┘
                       ↑                  │
                       │                  ↓
               ┌───────────────┐   ┌───────────┐
               │AtDestination  │←──│InElevator │
               └───────────────┘   └───────────┘
```

**States**:
- **Idle**: Standing still, no destination
- **Walking**: Moving horizontally on the same floor (2.0 columns/second)
- **WaitingForElevator**: At elevator shaft, waiting for car arrival
- **InElevator**: Riding elevator to destination floor
- **AtDestination**: Reached final destination, will idle or get new goal

#### 4.3.2 Person Archetypes

| Archetype | Frequency | Behavior Profile |
|-----------|-----------|------------------|
| **Business Person** | 25% | High hunger need, low entertainment need, office-bound |
| **Tourist** | 25% | High entertainment need, seeks attractions |
| **Shopper** | 25% | High shopping desire, prefers retail areas |
| **Casual Visitor** | 25% | Balanced needs, general wandering |

#### 4.3.3 Person Needs (Future)

**Hunger**: Increases over time, satisfied by restaurants  
**Entertainment**: Decreases over time, satisfied by arcades, theaters  
**Shopping**: Desire to visit retail shops  
**Comfort**: Affected by crowding, noise, wait times

**Design Note**: Current implementation has basic needs placeholder; full needs system planned for future milestone.

#### 4.3.4 Daily Schedules

People follow time-based routines:
- **Weekday Workers**: Arrive 09:00, lunch 12:00, leave 17:00
- **Weekend Visitors**: Idle/explore 10:00-20:00
- **Residents**: Leave for work 08:00-09:00, return 17:00-18:00

**Reference**: `include/core/components.hpp` (`Person`, `DailySchedule`), `docs/PERSONS.md`

---

### 4.4 Elevator System

**Purpose**: Provide vertical transportation for people changing floors.

#### 4.4.1 Architecture

**Components**:
- **ElevatorShaft**: Vertical shaft serving a range of floors (e.g., floors 0-10)
- **ElevatorCar**: Individual car within a shaft, with state machine and passenger tracking
- **PersonElevatorRequest**: Attached to people needing elevator transport

#### 4.4.2 Elevator State Machine

```
┌──────┐     ┌──────────┐     ┌──────────────┐
│ Idle │────→│ Moving   │────→│DoorsOpening  │
└──────┘     │(Up/Down) │     └──────────────┘
     ↑       └──────────┘            │
     │                               ↓
┌──────────────┐              ┌──────────┐
│DoorsClosing  │←─────────────│DoorsOpen │
└──────────────┘              └──────────┘
```

**Timing**:
- **Movement Speed**: 2.0 floors/second (configurable)
- **Door Opening**: 1.0 second
- **Door Open Duration**: 2.0 seconds (allows boarding/exiting)
- **Door Closing**: 1.0 second

#### 4.4.3 Elevator Mechanics

**Capacity Management**:
- Default capacity: 8 passengers
- Cars reject new boarders when at capacity
- Overcrowding leads to longer wait times (satisfaction penalty)

**Stop Queue Management**:
- Stops are inserted in sorted order (by floor number)
- Next stop chosen based on current direction (up or down)
- Destination floors added when passengers board

**Assignment Algorithm** (current):
- **First-Available**: Person requests elevator, first car with capacity is assigned
- Simple but causes inefficiency at scale

**Future Algorithms**:
- **SCAN/LOOK**: Direction-aware assignment (serve all up requests before reversing)
- **Cost-Based**: Minimize total wait time across all passengers
- **Group Control**: Coordinate multiple cars in a shaft

#### 4.4.4 Strategic Considerations

**Elevator Placement**:
- Lobbies require elevator access (except ground floor)
- Offices and residentials on high floors need good elevator coverage
- Multiple shafts reduce wait times but cost money and space

**Bottleneck Management**:
- Rush hours (09:00, 12:00, 17:00) create traffic spikes
- Express elevators (future) serve high floors without stopping at low floors
- Insufficient capacity leads to low satisfaction and tenant departures

**Reference**: `include/core/components.hpp` (`ElevatorShaft`, `ElevatorCar`), `docs/ELEVATOR.md`

---

### 4.5 Economy System

**Purpose**: Track financial state and drive resource management decisions.

#### 4.5.1 Economic Model

**Starting Funds**:
- Easy: $50,000
- Normal: $25,000
- Hard: $10,000

**Revenue Sources**:
- **Facility Rent**: Daily rent collected from tenants (offices, residentials)
- **Service Revenue**: Income from customers (restaurants, hotels, retail)
- **Occupancy Multiplier**: Revenue scales with occupancy rate

**Expenses**:
- **Operating Costs**: Daily expenses per facility (staff, utilities)
- **Construction Costs**: One-time costs to build facilities or expand floors
- **Maintenance** (future): Ongoing upkeep costs

**Quality Multiplier**:
- Satisfaction >70% → 1.2x revenue multiplier
- Satisfaction 30-70% → 1.0x multiplier
- Satisfaction <30% → 0.5x multiplier

**Formula Examples**:
```
Daily Revenue (Office) = (Base Rent × Current Tenants × Quality Multiplier) / Hours per Day
Daily Expense (Office) = Operating Cost

Net Profit = Total Revenue - Total Expenses
Balance = Starting Funds + Cumulative Net Profit
```

#### 4.5.2 Abundance-Based Economic Design (Cozy Economics)

**Core Philosophy**: TowerForge uses an **abundance-based economy** where resources flow freely and players never face bankruptcy or economic collapse. Money is a **gentle pacing mechanism**, not a source of stress.

**Abundance Principles**:
- ✅ **Income flows steadily**: Revenue accumulates even during low activity
- ✅ **No bankruptcy**: Negative balance impossible; just slower growth
- ✅ **Generous starting funds**: $25,000-$50,000 ensures comfortable beginning
- ✅ **Recoverable mistakes**: Every decision leads to learning, never permanent setbacks
- ✅ **Growth mindset**: Economy supports expansion and experimentation

**Positive Growth Loop**:
- More facilities → More revenue → More creative possibilities
- Happy residents → Attract more residents → Thriving community
- Thoughtful placement → Higher satisfaction → Better quality of life

**Gentle Feedback (Not Punishment)**:
- Lower satisfaction → Reduced quality multiplier (0.5x) → Gentle nudge to improve
- Higher expenses → Slower growth → Time to consolidate and care for existing tower
- Both lead to different playstyles, not failure

**No Failure States**:
- Can't go bankrupt (income always accumulates)
- Can pause and wait for resources to accumulate
- No "game over" or forced restarts
- Every tower can eventually prosper

**Economic Design Goals**:
- Money supports creativity and expression
- Scarcity is temporary pacing, never permanent obstacle
- Players focus on building beautiful towers, not spreadsheets
- Economy enables abundance and generosity mindset

**Reference**: `include/core/components.hpp` (`TowerEconomy`, `FacilityEconomics`), `docs/IMPLEMENTATION.md`

---

### 4.6 Time Simulation System

**Purpose**: Enable day/night cycles, scheduled events, and dynamic gameplay.

#### 4.6.1 Time Scale

**Real-Time Mapping**:
- Configurable speed multiplier (default: 60x)
- 1 real second = 60 in-game seconds (1 minute)
- 24 in-game hours = 24 real minutes

**Speed Control**:
- Pause (0x)
- Normal (1x = real-time, mostly for debugging)
- 2x, 4x, 8x speed options in UI

#### 4.6.2 Time-Based Events

**Business Hours**: 09:00-17:00 (offices, conference halls)  
**Extended Hours**: 09:00-21:00 (retail, restaurants, entertainment)  
**24/7 Operations**: Hotels, lobby  

**Daily Cycles**:
- **Morning Rush**: 08:00-09:00 (workers arrive)
- **Lunch Rush**: 12:00-13:00 (restaurant traffic)
- **Evening Rush**: 17:00-18:00 (workers depart)
- **Night Quiet**: 21:00-08:00 (low activity)

**Weekly Cycles**:
- Weekdays (Monday-Friday): Business operations
- Weekends (Saturday-Sunday): Tourist/visitor focus

**Reference**: `include/core/components.hpp` (`TimeManager`, `DailySchedule`)

---

### 4.7 Satisfaction System

**Purpose**: Simulate tenant happiness and drive strategic placement decisions.

#### 4.7.1 Satisfaction Score

**Scale**: 0-100%

**Levels**:
| Score | Level | Tenant Behavior |
|-------|-------|----------------|
| 81-100% | Excellent | Tenants stay, referrals increase |
| 61-80% | Good | Stable occupancy |
| 41-60% | Average | Minor departures |
| 21-40% | Poor | Significant tenant loss |
| 0-20% | Very Poor | Mass exodus |

#### 4.7.2 Satisfaction Factors

**Positive Factors**:
- Nearby high-quality facilities (+5 to +15 per amenity)
- Low wait times for elevators (<30 seconds)
- Good occupancy rate (50-80%)
- Facility-specific bonuses (e.g., gym adds +10 to adjacent offices)

**Negative Factors**:
- Overcrowding (>90% occupancy): -10 satisfaction
- Underpopulation (<30% occupancy): -5 satisfaction
- Noise from adjacent facilities: -5 to -15 (restaurants, arcades)
- Long elevator wait times (>60 seconds): -10 satisfaction
- Understaffed facilities nearby: -5 satisfaction

#### 4.7.3 Satisfaction Effects

**Economic Impact**:
- High satisfaction (>70%) → Quality multiplier 1.2x (more revenue)
- Low satisfaction (<30%) → Quality multiplier 0.5x (less revenue)

**Tenant Dynamics**:
- High satisfaction → Occupancy increases over time
- Low satisfaction → Tenants leave, occupancy decreases

**Reference**: `include/core/components.hpp` (`Satisfaction`), `docs/IMPLEMENTATION.md`

---

### 4.8 Research & Discovery System

**Purpose**: Meaningful progression that unlocks new creative possibilities and enhances your tower's capabilities.

**Cozy Philosophy**: Research in TowerForge is about **discovery and growth**, not grind or gates. It provides:
- **Meaningful choices**: Each unlock expands creative possibilities
- **Natural progression**: Research points flow from natural play, never tedious grinding
- **Player agency**: Choose what to unlock based on your playstyle
- **Abundance mindset**: Points accumulate steadily; you'll unlock everything eventually

#### 4.8.1 Research Tree Structure

**Tiers**:
1. **Tier 1** (Early Discovery): Basic enhancements, 500 RP each
2. **Tier 2** (Expanding Options): Advanced facilities, 1,000 RP each, requires Tier 1
3. **Tier 3** (Full Expression): Premium options, 2,000 RP each, requires Tier 2

**Research Points (RP) Acquisition** (flows naturally):
- **Community Growth**: Reach population milestones → +500 RP
- **Prosperity**: Daily income milestones → +300 RP
- **Time & Care**: Playing and nurturing your tower → steady accumulation
- **Exploration**: Discovering new possibilities → occasional bonuses

**Design Philosophy**: Research points are **abundant and flow naturally**. You're never blocked; just discovering new possibilities at a comfortable pace.

#### 4.8.2 Research Nodes (Meaningful Unlocks)

| Node Name | Tier | Cost (RP) | What It Enables |
|-----------|------|----------|-----------------|
| **Efficient Elevators** | 1 | 500 | Smoother resident movement (+25% speed) |
| **High-Capacity Cars** | 1 | 500 | More residents per trip (8→12 capacity) |
| **Quick Construction** | 1 | 500 | Build facilities faster (-25% time) |
| **Advanced HVAC** | 1 | 500 | Improved comfort (+10% satisfaction) |
| **Retail Expansion** | 2 | 1,000 | Unlocks Arcade & Gym (fun community spaces) |
| **Hospitality Suite** | 2 | 1,000 | Unlocks Hotel & Conference Hall (guests & events) |
| **Income Boost I** | 2 | 1,000 | Better prosperity (+15% revenue for growth) |
| **Express Elevators** | 3 | 2,000 | Premium fast elevators (skip floors) |
| **Flagship Retail** | 3 | 2,000 | Prestigious flagship stores (high-end) |

**Cozy Alignment**:
- ✅ **Meaningful, not tedious**: Each unlock genuinely expands gameplay
- ✅ **Natural accumulation**: Points flow from playing, not grinding
- ✅ **Player choice**: Pick what interests you most
- ✅ **Eventual abundance**: You'll unlock everything given time
- ✅ **No pressure**: Research at your own pace; no optimal path

**Reference**: `docs/RESEARCH_TREE_SYSTEM.md`

---

### 4.9 Milestone & Recognition System

**Purpose**: Celebrate player progress and tower growth without creating pressure or competition.

**Philosophy**: Achievements are gentle celebrations of your journey, not required objectives. They appear as warm notifications that acknowledge your tower's growth, never as demands or requirements.

#### 4.9.1 Growth Milestones (Celebrations, Not Objectives)

**Community Growth**:
- Welcome your first resident
- 50 residents living in your tower
- 100 residents calling your tower home
- A thriving community of 500+
- A bustling metropolis of 1000+

**Architectural Moments**:
- Your first building complete
- 10 floors reaching skyward
- 25 floors of vertical village
- 50 floors of community
- Building every facility type (when you feel like it)

**Community Happiness**:
- A week of resident contentment
- Residents thriving in your care
- Creating a truly welcoming space

**Time Together**:
- Your first day in the tower
- 10 hours of peaceful building
- 50 hours watching your tower live
- 100 hours of cozy creation

**Note**: These milestones appear as gentle notifications - small moments of recognition for your creative journey. They're never shown as a checklist or required progression. You discover them naturally as you play.

**Reference**: `docs/ACHIEVEMENTS.md`

---

### 4.10 UI/UX Systems

#### 4.10.1 HUD (Heads-Up Display)

**Design Philosophy**: Information should be helpful and calming, never demanding or stressful.

**Top Bar** (gentle presence):
- **Funds**: Current balance (no color stress indicators - just information)
- **Population**: Total residents in your community
- **Time**: Current in-game time (HH:MM) and day of week
- **Pace Control**: Pause anytime, adjust speed to your comfort (1x, 2x, 4x)

**Side Panels**:
- **Build Menu** (left): Browse facilities at your leisure - descriptions, ideas, inspiration
- **Info Panels** (bottom): Friendly information when you hover - learn about residents and spaces
- **Gentle Notifications** (top-right): Positive updates, milestone celebrations, never urgent demands

**Optional Views** (toggle whenever you like):
- **Research Tree** (R key): Discover what's unlockable at your own pace
- **Milestones** (M key): Gentle reminders of your journey (never a checklist)
- **Community Insights** (click HUD): Learn about your residents' happiness
- **Help System** (F1): Friendly guidance whenever you want it

#### 4.10.2 Interaction Paradigms

**Build Mode**:
1. Select facility from build menu
2. Hover over grid to preview placement (green = valid, red = invalid)
3. Click to confirm and start construction
4. Facility built over time (progress bar visible)

**Demolish Mode** (D key):
- Click facility to demolish
- 50% cost recovery
- Immediate removal

**Selection & Info**:
- Hover over facility/person/elevator → Info panel appears
- Click for persistent selection (future: detailed property panel)

**Undo/Redo**:
- Ctrl+Z / Ctrl+Y: Step through command history
- H key: Toggle visual history panel showing last 50 actions

#### 4.10.3 Accessibility Features

**Keyboard Navigation**:
- Tab/Shift+Tab: Cycle through UI elements
- Arrow keys: Navigate menus
- Enter/Space: Activate buttons
- ESC: Close dialogs/menus

**Visual Accessibility**:
- **High-Contrast Mode**: Brighter colors, thicker borders
- **Font Scaling**: 50%-300% UI text scaling
- **Color-Blind Support** (future): Icon overlays, pattern differentiation

**Audio Accessibility** (future):
- Screen reader support (NVDA, JAWS)
- Closed captions for audio cues

**Reference**: `UI_DEVELOPMENT_BIBLE.md`, `docs/ACCESSIBILITY.md`

---

## 5. Content Design

### 5.1 Facility Progression

**Unlock Sequence**:
1. **Start**: Lobby, Office, Residential, Retail Shop, Elevator
2. **Research Tier 1**: Restaurant, Hotel
3. **Research Tier 2**: Gym, Arcade, Conference Hall
4. **Research Tier 3**: Theater, Flagship Store

**Design Rationale**:
- Early game focuses on basic revenue generation (offices, shops)
- Mid game introduces amenities that boost satisfaction
- Late game provides premium facilities with high revenue/costs

### 5.2 Gentle Guidance (Optional)

**Tutorial Moments** (never forced):
1. Welcome: "This is your space - build what feels right"
2. First Building: Gentle introduction to placing facilities
3. Community Growth: "Residents are arriving - watch them find their rhythm"
4. Elevator Basics: Simple explanation when relevant
5. Creative Freedom: "Your tower, your way - there's no wrong approach"

**No Challenge Scenarios**: TowerForge is about creation and observation, not challenge completion.

### 5.3 Community Stories & Events

**Emergent Stories** (naturally occurring):
- Residents develop daily routines you can observe
- Busy times and quiet times emerge organically
- Small stories unfold as you watch
- Seasonal celebrations bring the community together

**Gentle Seasonal Events**:
- Spring flower festival (decorative additions available)
- Summer rooftop gathering (residents enjoy outdoor spaces)
- Autumn harvest market (community comes together)
- Winter holiday lights (your tower sparkles)

**Design Philosophy**: Events are optional decorative opportunities, never demands or stressors. Participate if you want, ignore if you don't - both approaches are perfect.
- Grand opening (free marketing, attracting tenants)

**Negative Events**:
- Elevator maintenance (temporarily disable car)
- Utility outage (reduced satisfaction)
- Economic downturn (reduced revenue)

**Design Philosophy**: Events add variability but never feel punishing (recoverable).

---

## 6. Player Experience

### 6.1 Welcoming Onboarding

**First-Time Experience** (gentle and optional):
1. **Warm Welcome**: "Welcome to your tower. Take your time and build what feels right."
2. **Optional Guidance**: Friendly tooltips appear if helpful, dismiss if you prefer to explore
3. **Creative Freedom**: Jump in and start building, or follow gentle suggestions
4. **Your Pace**: No time limits, no pressure, pause anytime

**Design Goals**:
- Welcoming, never overwhelming
- Optional guidance, never forced tutorials
- Learn by doing at your own pace
- Help always available (F1) but never intrusive

### 6.2 Natural Growth Journey

**Beginning** (your first moments):
- Few residents, quiet peaceful pace
- Discover controls through gentle experimentation
- Build confidence in your own way
- No wrong choices, unlimited undo

**Growing Community** (as your tower develops):
- More residents bring more life and activity
- Daily rhythms emerge naturally
- Discover what makes your residents happy
- Your personal building style develops

**Established Tower** (when you're ready):
- Complex daily patterns to observe
- Rich community life unfolding
- New facilities unlock naturally
- Your unique tower personality shines

**Ongoing Journey** (forever):
- No end point, just continued creation
- Seasonal events add variety if you want
- Keep building, keep watching, keep enjoying
- Your tower evolves as you do

### 6.3 What Motivates Cozy Play

**Creative Expression**:
- Design spaces that feel uniquely yours
- Experiment with layouts and aesthetics
- Create themed floors or districts
- Express your personality through your tower

**Nurturing & Care**:
- Watch residents live their daily lives
- Create welcoming, comfortable spaces
- See happiness emerge from your design
- Feel the warmth of a thriving community

**Meditative Observation**:
- Watch patterns emerge over time
- Observe the flow of daily life
- Discover small stories and moments
- Peaceful, calming gameplay

**Personal Discovery**:
- Find your own play style
- Unlock content at your pace
- Set your own goals (or none)
- Play however feels good

### 6.4 Flexible Play Sessions

**Anytime, Any Length**:
- **5 Minutes**: Pop in, place one building, watch for a moment
- **30 Minutes**: Relax and make some changes, observe life
- **An Hour**: Shape a new floor, discover new possibilities  
- **An Evening**: Get lost in creation, watch seasons change
- **Return Anytime**: Your tower waits patiently, never judging

**Savedata**:
- Autosave every 5 minutes (never lose progress)
- Manual save anytime you like
- Multiple save slots for different towers
- Cloud saves (future) for playing across devices

**Reference**: `docs/SAVE_LOAD.md`

---

## 7. Growth & Discovery

### 7.1 Natural Discovery Systems

**Organic Unlocking** (no grind, no gates):
- New facilities unlock as your community naturally grows
- Discovery through play, not completion
- No required objectives or checklists
- Everything accessible at your own pace

**Community Recognition**:
- Gentle notifications celebrate your tower's growth
- "Your community has grown to 50 residents!" 
- "Residents are thriving in your care"
- Recognition, not requirements

**Research as Discovery**:
- Optional system for those who enjoy it
- Unlock new building types and options
- Entirely at your discretion
- Can ignore completely and still enjoy everything

### 7.2 Sharing & Expression (Open Source Philosophy)

**Current Model**: Free and open source forever

**Future Possibilities** (always optional):
- **Themed Building Packs**: Seasonal or aesthetic variations (always free or donation-based)
- **Community Showcase**: Share your tower designs with others (screenshots, tours)
- **Modding Support**: Create and share your own content
- **"Buy Us a Coffee"**: Optional support for developers, never required

**Core Philosophy**: TowerForge is a gift to the community. All gameplay always free. Optional support welcomed but never expected.

---

## 8. Technical Scope

### 8.1 Platform Requirements

**Minimum Spec**:
- CPU: Dual-core 2.0 GHz
- RAM: 2 GB
- GPU: Integrated graphics (OpenGL 3.3 support)
- Storage: 200 MB
- OS: Windows 10, Ubuntu 20.04, macOS 10.15

**Recommended Spec**:
- CPU: Quad-core 3.0 GHz
- RAM: 4 GB
- GPU: Dedicated GPU (any modern)
- Storage: 500 MB (mods, saves)

### 8.2 Performance Targets

**Frame Rate**: 60 FPS sustained with 500+ entities  
**Load Time**: <5 seconds for large saves (1000+ entities)  
**Memory**: <500 MB RAM usage  
**Simulation Scale**: 100+ floors, 2000+ entities without slowdown  

### 8.3 Save System

**Save Format**: JSON (human-readable, moddable)  
**Save Location**: Platform-specific user directory  
**Autosave**: Every 5 minutes  
**Cloud Save** (future): Steam Cloud integration  

**Reference**: `docs/SAVE_LOAD.md`

---

## 9. Success Criteria (Cozy Game Values)

### 9.1 Player Experience Metrics

- **Welcoming Experience**: Players feel comfortable and calm from the start
- **Session Flexibility**: Players feel good about any session length (5 min to 5 hours)
- **Return Comfort**: Players return because they want to, not obligation
- **Creative Satisfaction**: Players express their unique vision and feel proud

### 9.2 Quality & Comfort Metrics

- **Stability**: <5 critical bugs, <1% crash rate
- **Performance**: Smooth 60 FPS on modest hardware
- **Accessibility**: WCAG 2.1 AA compliance - everyone can enjoy
- **Stress-Free**: No aggressive timers, fail states, or punishment mechanics
- **Respectful**: Never demands player's time or attention

### 9.3 Community & Sharing

- **Positive Community**: Sharing creations, not competition
- **Modding Creativity**: Community adds cozy content
- **GitHub Growth**: 1,000+ stars showing interest
- **Downloads**: 100,000+ players finding their cozy space
- **Word of Mouth**: "This game is so relaxing and creative"

---

## 10. References

### 10.1 Design Influences & Philosophy

**Core Inspiration**:
- **SimTower** (Maxis, 1994): Original tower simulation
- **Yoot Tower** (OPeNBooK, 1998): Expanded creative freedom
- **Cozy Games Manifesto** (Lost Garden, 2018): Core design philosophy
- **TowerForge Design Philosophy Wiki**: https://github.com/adam4813/towerforge/wiki/Design-Philosophy:-Cozy-Games

**Cozy Game Principles Applied** (from Project Horseshoe 2017):
- **Safety & Comfort**: No failure states, no time pressure, every decision leads to growth
- **Abundance & Generosity**: Resources flow freely, no zero-sum economics
- **Softness & Gentleness**: Warm aesthetics, slow pacing, intimate scale
- **Agency & Autonomy**: Players control what, when, and how - no forced objectives
- **Connection & Belonging**: NPCs are neighbors and friends, not resources

**Meaningful Progression** (Cozy Alignment):
- Research points unlock creative possibilities (not grind gates)
- Facility management adds optional depth (automatable, not tedious)
- Maintenance and cleanliness reward care (gentle nudges, not punishment)
- Municipal services (fire, security) protect community (not catastrophes)
- All systems support nurturing and expression
- **Gentle Pacing**: Play at your own speed, pause anytime
- **Personal Expression**: Your tower is unique to you
- **Nurturing Care**: Watch your community thrive
- **Meditative Experience**: Calming, peaceful, restorative
- **Warm Aesthetics**: Welcoming visuals and tone

**Other Cozy Influences**:
- **Animal Crossing**: Non-competitive, creative expression
- **Stardew Valley**: Gentle community building
- **A Short Hike**: No stress, pure exploration
- **Unpacking**: Meditative organization and expression
- **The Sims**: Watching lives unfold

### 10.2 Design Principles

**Cozy Game Design Philosophy**:
- **Embrace Not Challenge**: Welcoming, not demanding
- **Expression Not Optimization**: Create, don't min-max
- **Community Not Competition**: Share, don't compete
- **Comfort Not Stress**: Calming, not anxiety-inducing
- **Care Not Control**: Nurture, don't dominate

**John Carmack's Technical Excellence**:
- Modern C++20 architecture
- ECS for performance and maintainability
- Cross-platform from day one

**Inclusive Design** (WCAG 2.1):
- Full keyboard navigation
- High-contrast mode
- Scalable text
- Never exclude anyone from the cozy experience

### 10.3 Technical Documentation

- **Technical Design Document**: `TECHNICAL_DESIGN_DOCUMENT.md`
- **API Reference**: `UI_DEVELOPMENT_BIBLE.md`, `docs/MODDING_API.md`
- **Build Instructions**: `README.md`
- **Testing Guide**: `TESTING.md`
- **Agent Guidelines**: `AGENTS.md`

---

## Appendix A: Glossary

**ECS**: Entity Component System — architecture pattern separating data (components) from behavior (systems)  
**RP**: Research Points — currency for unlocking research tree nodes  
**HUD**: Heads-Up Display — on-screen UI elements (funds, time, etc.)  
**FTUE**: First-Time User Experience — onboarding/tutorial flow  
**WCAG**: Web Content Accessibility Guidelines — accessibility standards  
**QoL**: Quality of Life — features that improve usability (undo, tooltips, etc.)  

---

## Appendix B: Change Log

**Version 1.0** (2025-10-25):
- Initial comprehensive GDD
- Consolidates all existing gameplay documentation
- Defines core systems and content
- Establishes design pillars and success criteria

---

**TowerForge Game Design Document**  
© 2025 TowerForge Contributors  
Licensed under MIT (or similar open source license)

*"Building the future of vertical city simulation."*
