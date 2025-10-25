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

TowerForge is a modern, open-source skyscraper simulation game that challenges players to design, build, and manage the ultimate vertical city. Inspired by SimTower, it combines deep simulation with contemporary accessibility and moddability.

### 1.3 Core Design Pillars

1. **Strategic Depth**: Every decision matters — placement, timing, expansion
2. **Emergent Complexity**: Simple rules create intricate, challenging gameplay
3. **Player Empowerment**: Extensive undo/redo, tooltips, analytics, modding
4. **Accessibility First**: Inclusive design for all players
5. **Technical Excellence**: Modern architecture enables scale and performance

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

Players start with an empty lot and modest funds. Their goal: construct a thriving skyscraper that balances profitability, tenant satisfaction, and efficient operations. Each floor presents strategic challenges around facility placement, vertical transportation, and resource management.

### 2.2 Core Fantasy

**"You are the master architect and operations manager of a living, breathing skyscraper."**

Players experience:
- The satisfaction of efficient layout design
- The challenge of rush-hour traffic management
- The reward of watching their tower bustle with life
- The pride of achieving 5-star excellence

### 2.3 Design Influences

| Source | Influence | Implementation |
|--------|-----------|----------------|
| **SimTower (Maxis, 1994)** | Core gameplay loop, facility types | Modernized with QoL features |
| **Yoot Tower (OPeNBooK, 1998)** | Expanded facility variety | Enhanced with staffing and hours |
| **RimWorld** | AI storytelling, emergent gameplay | Person AI with needs and schedules |
| **Factorio** | Production chain optimization | Elevator capacity as bottleneck |
| **Cities: Skylines** | Traffic flow challenges | Vertical traffic (elevators) |
| **The Sims** | Individual agent simulation | Person state machines |

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

**Time Scale**: Simulation runs at adjustable speed (pause, 1x, 2x, 4x, 8x)

**Player Activities (prioritized by frequency)**:

1. **Placement Decisions** (every 30-120 seconds)
   - Select facility type from build menu
   - Choose optimal floor and column position
   - Consider adjacency bonuses and elevator access
   - Confirm construction (funds permitting)

2. **Monitoring** (continuous, glanceable)
   - Watch HUD for funds, population, time, satisfaction
   - Observe person movement and elevator traffic
   - Check notification center for events
   - Read info panels on hover

3. **Problem Solving** (every 2-5 minutes)
   - Address low satisfaction alerts
   - Manage understaffed facilities
   - Optimize elevator coverage
   - Balance revenue vs. expenses

4. **Strategic Planning** (every 5-15 minutes)
   - Review analytics overlays (income breakdown, population)
   - Decide on vertical vs. horizontal expansion
   - Prioritize research tree investments
   - Plan facility placement synergies

5. **Progression** (every 15-60 minutes)
   - Unlock new facility types via research
   - Achieve star rating milestones
   - Complete achievements
   - Unlock game-changing upgrades

### 3.3 Win/Loss Conditions

**Win Conditions** (player-chosen goals):
- Achieve 5-star tower rating
- Reach specific population targets (500, 1000, 2000+ tenants)
- Accumulate wealth ($500,000+)
- Complete all achievements
- Build to maximum height (100+ floors)

**Loss Conditions** (recoverable):
- Bankruptcy (negative balance prevents building)
- Mass tenant exodus (satisfaction <20%)

**Philosophy**: No "game over" — players can always recover through smart decisions.

### 3.4 Difficulty Scaling

**Implicit Difficulty Curve**:
- **Early Game (Floors 1-10)**: Tutorial-like, forgiving economy, simple layouts
- **Mid Game (Floors 11-30)**: Elevator bottlenecks emerge, satisfaction becomes critical
- **Late Game (Floors 31+)**: Tight optimization required, complex multi-shaft planning

**Player-Adjustable Difficulty**:
- Starting funds (Easy: $50,000 | Normal: $25,000 | Hard: $10,000)
- Simulation speed (affects challenge of real-time decisions)
- Tutorial/help system (optional guidance)

---

## 4. Game Systems

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

#### 4.2.2 Facility Mechanics

**Staffing**:
- Facilities with staff requirements show job openings in UI
- Understaffed facilities operate at reduced efficiency or shut down
- Staff are automatically hired from available worker pool (simplified in current version)

**Operating Hours**:
- Facilities outside operating hours do not generate revenue or serve customers
- Hotels operate 24/7; offices typically 09:00-17:00
- Players cannot override hours (design decision: operational realism)

**Economics**:
- **Revenue**: Based on rent, occupancy, or foot traffic
- **Expenses**: Daily operating costs (staff, utilities, maintenance)
- **Quality Multiplier**: Satisfaction affects revenue (0.5x to 2.0x)

**Satisfaction Factors**:
- **Crowding**: >90% occupancy decreases satisfaction; <30% also problematic (feels empty)
- **Noise**: Restaurants, shops, arcades generate noise affecting adjacent facilities
- **Facility Quality**: Inherent quality ratings (e.g., hotels provide +10 satisfaction to neighbors)
- **Wait Times**: Long elevator waits reduce satisfaction

**Placement Rules**:
- **Lobby**: Must be on ground floor (floor 0); typically one per tower
- **Offices**: Not on ground floor (accessibility); require elevator access for floors >1
- **Residentials**: Not on ground floor; benefit from proximity to amenities
- **Retail/Restaurants**: Best on lower floors for foot traffic
- **Hotels**: Can be anywhere; premium floors yield higher rates

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

#### 4.5.2 Economic Feedback Loops

**Positive Feedback**:
- More facilities → More revenue → Faster expansion → More revenue

**Negative Feedback**:
- Expansion → Higher operating costs → Lower profit margins
- Poor satisfaction → Tenants leave → Less revenue → Difficulty expanding

**Bankruptcy Protection**:
- Players cannot build if funds insufficient
- No "game over" — can wait for daily revenue to recover balance

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

### 4.8 Research & Progression System

**Purpose**: Provide long-term goals and unlock new content over time.

#### 4.8.1 Research Tree Structure

**Tiers**:
1. **Tier 1** (Early Game): Basic upgrades, 500 RP each
2. **Tier 2** (Mid Game): Advanced upgrades, 1,000 RP each, requires Tier 1
3. **Tier 3** (Late Game): Elite upgrades, 2,000 RP each, requires Tier 2

**Research Points (RP) Acquisition**:
- Milestone-based awards (e.g., reach 50 tenants → +500 RP)
- Daily income milestones (e.g., $5,000/day → +300 RP)
- Achievement unlocks (varies by achievement)

#### 4.8.2 Research Nodes

| Node Name | Tier | Cost (RP) | Effect |
|-----------|------|----------|--------|
| **Efficient Elevators** | 1 | 500 | +25% elevator speed |
| **High-Capacity Cars** | 1 | 500 | +50% elevator capacity (8→12) |
| **Quick Construction** | 1 | 500 | -25% construction time |
| **Advanced HVAC** | 1 | 500 | +10% satisfaction to all facilities |
| **Retail Expansion** | 2 | 1,000 | Unlocks Arcade, Gym |
| **Hospitality Suite** | 2 | 1,000 | Unlocks Hotel, Conference Hall |
| **Income Boost I** | 2 | 1,000 | +15% revenue from all facilities |
| **Express Elevators** | 3 | 2,000 | Unlocks express elevator type (skip floors) |
| **Flagship Retail** | 3 | 2,000 | Unlocks Flagship Store (high revenue) |

**Design Philosophy**: Research provides meaningful choices, not just "more of everything."

**Reference**: `docs/RESEARCH_TREE_SYSTEM.md`

---

### 4.9 Achievement System

**Purpose**: Provide concrete goals and reward milestones.

#### 4.9.1 Achievement Categories

**Builder** (construction/expansion):
- First Facility
- Reach 10 floors
- Reach 30 floors
- Build every facility type

**Manager** (operations):
- 100 tenants
- 500 tenants
- Maintain 80% satisfaction for 7 days
- Fully staff all facilities

**Economist** (financial):
- Earn $100,000
- Earn $500,000
- $10,000/day profit
- Zero debt for 7 days

**Prestige** (excellence):
- Achieve 3-star rating
- Achieve 5-star rating
- Complete all research
- 100 hours played

**Reference**: `docs/ACHIEVEMENTS.md`

---

### 4.10 UI/UX Systems

#### 4.10.1 HUD (Heads-Up Display)

**Top Bar** (always visible):
- **Funds**: Current balance with color coding (green >$10k, yellow $5-10k, red <$5k)
- **Population**: Total tenants/visitors
- **Time**: Current in-game time (HH:MM) and day of week
- **Speed Control**: Pause, 1x, 2x, 4x buttons

**Side Panels**:
- **Build Menu** (left): Facility types with costs, staff requirements, descriptions
- **Info Panels** (bottom): Contextual information on hover (facilities, people, elevators)
- **Notification Center** (top-right): Alerts for events, milestones, problems

**Overlays** (toggle-able):
- **Research Tree** (R key): Tech tree with progress and costs
- **Achievements** (A key): Achievement list with unlock status
- **Analytics** (click HUD elements): Income/population breakdowns
- **Help System** (F1): Contextual help and tutorials

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

### 5.2 Scenario Design (Future)

**Tutorial Campaign**:
1. First Tower: Build lobby and first office
2. Vertical Expansion: Reach 5 floors
3. Elevator Basics: Install and manage elevators
4. Satisfaction 101: Balance crowding and noise
5. Economic Fundamentals: Achieve profit
6. Star Power: Reach 3-star rating

**Challenge Scenarios**:
- **Rush Hour Chaos**: Manage heavy traffic with limited elevators
- **Budget Crisis**: Start with $5,000, reach profitability
- **Satisfaction Emergency**: Fix failing tower (low satisfaction)
- **Skyline Architect**: Build aesthetic tower (scoring based on design)

### 5.3 Random Events (Future)

**Positive Events**:
- Celebrity visit (temporary satisfaction boost)
- Economic boom (temporary revenue increase)
- Grand opening (free marketing, attracting tenants)

**Negative Events**:
- Elevator maintenance (temporarily disable car)
- Utility outage (reduced satisfaction)
- Economic downturn (reduced revenue)

**Design Philosophy**: Events add variability but never feel punishing (recoverable).

---

## 6. Player Experience

### 6.1 Onboarding

**First-Time User Experience (FTUE)**:
1. **Welcome Screen**: Brief game explanation, "Start Tutorial" vs. "Free Build"
2. **Tutorial Part 1**: Build lobby, explained with tooltips
3. **Tutorial Part 2**: Place first office, observe person spawning
4. **Tutorial Part 3**: Add elevator, watch people use it
5. **Tutorial Part 4**: Monitor satisfaction, adjust placement
6. **Tutorial Complete**: Freedom to experiment

**Design Goals**:
- 10-15 minute tutorial
- Interactive (player builds, not just watches)
- Skippable for experienced players
- Contextual help available via F1 anytime

### 6.2 Difficulty Curve

**Early Game (0-30 minutes)**:
- Forgiving: High starting funds, simple layouts
- Focus: Learn controls, understand basic systems
- Challenges: None (sandbox)

**Mid Game (30-90 minutes)**:
- Moderate: Elevator bottlenecks emerge, satisfaction matters
- Focus: Optimization, strategic placement
- Challenges: Balance revenue vs. expenses, manage traffic

**Late Game (90+ minutes)**:
- High: Tight optimization required, complex multi-shaft layouts
- Focus: Perfection, 5-star rating, achievements
- Challenges: Maximize efficiency, handle scale

### 6.3 Player Motivation

**Intrinsic Motivators**:
- **Mastery**: Learning optimal layouts, traffic patterns
- **Creativity**: Aesthetic tower design, personal style
- **Problem-Solving**: Overcoming bottlenecks, satisfaction issues

**Extrinsic Motivators**:
- **Progression**: Unlocking research, achievements
- **Competition** (future): Leaderboards, community showcases
- **Collection**: Building every facility type, completing content

### 6.4 Pacing

**Session Length**:
- **Short Sessions** (15-30 min): Build a few floors, address issues
- **Medium Sessions** (1-2 hours): Reach next star rating
- **Long Sessions** (2+ hours): Complete achievements, max out tower

**Save/Load**:
- Autosave every 5 minutes
- Quick save/load (F5/F9)
- Multiple manual save slots (20 slots)

**Reference**: `docs/SAVE_LOAD.md`

---

## 7. Progression & Monetization

### 7.1 Progression Systems

**Star Rating** (1-5 stars):
- Primary progression metric
- Based on satisfaction, tenants, floors, income
- Each star unlocks congratulations message and bragging rights

**Research Tree**:
- 9 nodes across 3 tiers
- Unlocks facilities, upgrades, bonuses
- Provides long-term goals

**Achievements**:
- 19 achievements across 4 categories
- Mix of easy (tutorial) and hard (aspirational)
- Persist across saves

### 7.2 Monetization (Open Source Model)

**Current Model**: Free and open source

**Potential Future Monetization** (if project becomes sustainable):
- **Cosmetic DLC**: Themed facility skins (cyberpunk, medieval, etc.)
- **Scenario Packs**: Curated challenge campaigns
- **Soundtrack DLC**: Extended music tracks
- **"Support the Developers" Donation Option**: Voluntary contributions

**Philosophy**: Never paywall gameplay features. All mechanics free forever.

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

## 9. Success Criteria

### 9.1 Gameplay Metrics

- **Tutorial Completion**: >90% of players complete tutorial
- **Session Length**: Average 45+ minutes per session
- **Retention**: 50%+ return after 7 days
- **Star Rating Distribution**: 30% reach 5 stars within 10 hours

### 9.2 Quality Metrics

- **Bugs**: <5 critical bugs in production
- **Crashes**: <1% crash rate
- **Performance**: 60 FPS on minimum spec
- **Accessibility**: WCAG 2.1 AA compliance

### 9.3 Community Metrics

- **GitHub Stars**: 1,000+ stars (measure of interest)
- **Contributors**: 20+ active contributors
- **Mods**: 50+ community mods in first year
- **Downloads**: 100,000+ total downloads

---

## 10. References

### 10.1 Design Influences

- **SimTower** (Maxis, 1994): Original inspiration
- **The Tower** (OPeNBooK, 1994): Japanese original
- **Yoot Tower** (OPeNBooK, 1998): Spiritual sequel
- **Project Highrise** (SomaSim, 2016): Modern take
- **RimWorld** (Ludeon Studios, 2018): AI storytelling
- **Factorio** (Wube Software, 2020): Optimization gameplay
- **Cities: Skylines** (Colossal Order, 2015): Traffic management

### 10.2 Design Principles

**Sid Meier's "Interesting Decisions"**:
- Every facility placement is meaningful
- Trade-offs between expansion and optimization
- No "obviously correct" solution

**John Carmack's Technical Excellence**:
- Modern C++20 architecture
- ECS for performance and maintainability
- Cross-platform from day one

**Shigeru Miyamoto's "Polish"**:
- Smooth animations, responsive controls
- Contextual help, tooltips everywhere
- Undo/redo for forgiving gameplay

**Accessibility Best Practices** (WCAG 2.1):
- Full keyboard navigation
- High-contrast mode
- Scalable text
- No color-only information

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
