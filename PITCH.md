# TowerForge: The Next Generation Skyscraper Simulator

## 🏢 One-Line Pitch
**A modern, open-source reimagining of SimTower that combines deep simulation with player accessibility, built on cutting-edge C++20 architecture.**

---

## 🎯 The Vision

TowerForge is the spiritual successor to Maxis's SimTower (1994) — but for the modern era. We're building the definitive skyscraper management simulation: one that respects the genre's roots while embracing contemporary game design principles and technical excellence.

### Why Now?

- **Genre Revival**: Tower/vertical city sims are underserved; SimTower's charm remains unmatched 30 years later
- **Technical Innovation**: Modern C++20 + ECS architecture enables unprecedented scale and moddability
- **Open Development**: Full source availability means a game that evolves with its community
- **Cross-Platform**: Windows, Linux, macOS from day one

---

## 🎮 Core Gameplay Loop

1. **Build** → Construct facilities (offices, shops, residences, amenities)
2. **Manage** → Balance tenant satisfaction, staffing, and vertical transportation
3. **Optimize** → Improve traffic flow, expand intelligently, unlock upgrades
4. **Prosper** → Earn star ratings, achieve milestones, expand your architectural empire

### The Hook

**Every floor is a puzzle.** Facility placement isn't just about space — it's about:
- Elevator access patterns
- Tenant satisfaction from nearby amenities
- Traffic flow during rush hours
- Revenue optimization through strategic adjacency
- Vertical expansion vs. economic constraints

---

## 🌟 What Makes TowerForge Special?

### 1. **True Simulation Depth**
- **People AI**: Every person has needs, destinations, schedules, and personality archetypes
- **Economic Model**: Real-time revenue/expense tracking with quality multipliers
- **Elevator Physics**: Realistic vertical transportation with capacity management and queuing
- **Satisfaction Systems**: Tenants respond to crowding, noise, facility quality, and wait times

### 2. **Modern Accessibility**
- Full keyboard navigation
- High-contrast mode
- Scalable UI (50%-300%)
- Contextual tooltips
- Tutorial system
- Comprehensive help system

### 3. **Extensibility by Design**
- **Lua Modding API**: Custom facilities and visitor types without touching C++
- **ECS Architecture**: Easy to add systems and behaviors
- **Data-Driven Content**: Facilities, achievements, research nodes defined in data
- **Open Source**: Community contributions welcomed

### 4. **Quality of Life**
- Unlimited undo/redo with visual history
- Research tree for progression
- Achievement system for goals
- Robust save/load system
- Real-time analytics overlays
- Notification center

---

## 📊 Market Position

### Target Audience
- **Primary**: PC strategy/simulation enthusiasts (25-45 years old)
- **Secondary**: Former SimTower/Yoot Tower fans seeking modern alternative
- **Tertiary**: Management game fans (Cities: Skylines, Factorio, RimWorld)

### Comparable Titles
| Game | Year | Strengths | TowerForge Advantage |
|------|------|-----------|---------------------|
| SimTower | 1994 | Original charm | Modern UX, moddability, active development |
| Project Highrise | 2016 | Visual polish | Deeper simulation, open source |
| Yoot Tower | 1998 | Expanded gameplay | Cross-platform, performance, accessibility |

### Differentiation
- **Only open-source vertical city sim** with modern architecture
- **Best-in-class accessibility features** in the genre
- **Lua modding** for easy content creation
- **Entity Component System** enables unprecedented scale

---

## 🔧 Technical Excellence

### Architecture Highlights
- **Language**: C++20 (cutting-edge features: ranges, concepts, modules)
- **ECS Framework**: flecs (industry-proven, high-performance)
- **Rendering**: Raylib (2D vector graphics, hardware-accelerated)
- **Build System**: CMake + vcpkg (reproducible, cross-platform)
- **Testing**: Comprehensive suite (integration, E2E, unit tests)

### Performance
- **Scalable**: Thousands of entities simulated simultaneously
- **Efficient**: Smart culling, batch rendering, lazy updates
- **Responsive**: 60 FPS target on modest hardware

### Development Practices
- Gang of Four design patterns throughout
- RAII and modern C++ memory safety
- Composition over inheritance
- Declarative, reactive UI architecture
- CI/CD with multi-platform builds

---

## 🗺️ Roadmap Snapshot

### ✅ **Core Foundation (Current)**
- ECS world with 20+ component types
- Person movement with 5-state AI
- Elevator system with realistic physics
- 12 facility types with staffing
- Economic simulation (revenue, costs, satisfaction)
- Research tree and achievements
- Save/load system
- Interactive building with undo/redo
- Star rating progression

### 🚧 **Next Milestones**
- Audio system (music, SFX, ambient)
- Enhanced visitor needs (hunger, entertainment, shopping)
- Staff management and scheduling
- Advanced facility upgrades
- Random events and scenarios
- Tutorial campaign
- Steam integration

### 🔮 **Future Vision**
- Multiplayer tower competitions
- Procedural event generation
- Advanced traffic simulation
- Weather and time-of-day effects
- Modding workshop/repository
- Mobile ports (iOS/Android)

---

## 📈 Success Metrics

### Technical
- **Build Success**: 100% CI pass rate across platforms
- **Test Coverage**: >80% on critical systems
- **Performance**: 60 FPS with 500+ entities
- **Load Times**: <5 seconds for large saves

### Player Experience
- **Onboarding**: 90%+ tutorial completion rate
- **Retention**: Average 10+ hours played (Steam metrics)
- **Satisfaction**: 85%+ positive reviews
- **Accessibility**: WCAG AA compliance

### Community
- **Contributors**: 20+ active contributors
- **Mods**: 50+ community mods in first year
- **Community**: 5,000+ Discord members
- **Reach**: 100,000+ downloads in first year

---

## 💡 Why This Matters

**Preservation of Gaming History**: SimTower defined a genre but is effectively lost to modern players. TowerForge ensures this gameplay style survives and evolves.

**Technical Showcase**: Demonstrates that C++20 + ECS can deliver both performance *and* developer ergonomics, serving as reference architecture for future sims.

**Open Gaming Future**: Proves open-source game development can match commercial quality while empowering communities.

**Accessibility Leadership**: Sets new standards for inclusive design in management sims.

---

## 🎬 Call to Action

**Play**: Download pre-built binaries from GitHub Actions  
**Contribute**: Fork, build, submit PRs — all skill levels welcome  
**Extend**: Create mods with Lua — no C++ required  
**Discuss**: Join our community — Discord/GitHub Discussions  

---

## 📚 Learn More

- **Repository**: https://github.com/adam4813/towerforge
- **Documentation**: See `GAME_DESIGN_DOCUMENT.md` and `TECHNICAL_DESIGN_DOCUMENT.md`
- **Build Instructions**: See `README.md`
- **Contributing**: See `CONTRIBUTING.md`

---

**TowerForge**: *Building the future of vertical city simulation.*

*Inspired by Maxis. Built by the community. Open to everyone.*
