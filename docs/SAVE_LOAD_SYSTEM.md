# Save/Load System Implementation

## Overview

This document describes the implementation of the save/load system for TowerForge using nlohmann/json for serialization.

## Architecture

### Core Components

1. **SaveLoadManager** (`include/core/save_load_manager.hpp`, `src/core/save_load_manager.cpp`)
   - Handles all save/load operations
   - Manages save slots (create, delete, rename)
   - Implements autosave functionality
   - Provides error handling and validation

2. **SaveLoadMenu** (`include/ui/save_load_menu.h`, `src/ui/save_load_menu.cpp`)
   - User interface for save/load operations
   - Displays save slot list with metadata
   - Provides interactive buttons for actions
   - Shows status bar with autosave information

### Save File Format

Save files use JSON format with the `.tfsave` extension.

#### File Structure

```json
{
  "metadata": {
    "game_version": "0.1.0",
    "save_date": "2025-10-07 14:30:00",
    "tower_name": "My Tower",
    "current_day": 5,
    "current_time": 14.5,
    "total_balance": 25000.0,
    "population": 50
  },
  "time": {
    "current_hour": 14.5,
    "current_day": 5,
    "current_week": 1,
    "simulation_speed": 1.0,
    "hours_per_second": 60.0
  },
  "economy": {
    "total_balance": 25000.0,
    "total_revenue": 5000.0,
    "total_expenses": 2000.0,
    "daily_revenue": 500.0,
    "daily_expenses": 200.0,
    "last_processed_day": 4
  },
  "grid": {
    "floors": 10,
    "columns": 20
  },
  "entities": [
    {
      "id": 12345,
      "name": "Person1",
      "person": {
        "name": "John Doe",
        "state": 0,
        "current_floor": 2,
        "current_column": 5.5,
        ...
      },
      ...
    }
  ]
}
```

### Serialized Components

The following ECS components are serialized:

1. **Global Singletons:**
   - TimeManager - simulation time and speed
   - TowerEconomy - financial state

2. **Entity Components:**
   - Position - 2D world position
   - Velocity - movement velocity
   - Actor - basic actor information
   - Person - detailed person state and movement
   - BuildingComponent - facility type and properties
   - GridPosition - grid-based position
   - Satisfaction - tenant satisfaction metrics
   - FacilityEconomics - facility financial data
   - ElevatorShaft - elevator shaft configuration
   - ElevatorCar - elevator car state
   - PersonElevatorRequest - elevator usage tracking
   - DailySchedule - scheduled actions

3. **TowerGrid:**
   - Floor and column counts
   - Grid is resized on load to match saved dimensions

## Save Slot Management

### Slot Storage

- **Default Location (Linux/Mac):** `~/.towerforge/saves/`
- **Default Location (Windows):** `%APPDATA%/TowerForge/saves/`
- **Fallback:** `./saves/` in current directory

### Slot Naming

- Valid characters: alphanumeric, spaces, underscores, hyphens
- Maximum length: 64 characters
- Reserved names: `_autosave` (used for automatic saves)

### Slot Operations

1. **Create/Save:**
   - Validates slot name
   - Checks disk space (requires >10MB free)
   - Serializes complete game state
   - Writes JSON file with metadata

2. **Load:**
   - Validates file exists
   - Checks version compatibility
   - Deserializes game state
   - Restores ECS world

3. **Delete:**
   - Confirms slot exists
   - Removes save file from disk

4. **Rename:**
   - Validates new name
   - Checks new name doesn't exist
   - Renames file on disk

## Autosave System

### Configuration

- **Default Interval:** 120 seconds (2 minutes)
- **Slot Name:** `_autosave`
- **Default State:** Enabled

### Autosave Triggers

1. **Periodic:** Every N seconds (configurable)
2. **On Quit:** When exiting the game (future enhancement)
3. **Before Risky Actions:** Before major changes (future enhancement)

### Status Display

The status bar shows:
- Autosave enabled/disabled status
- Time since last save (minutes and seconds)
- Error status

## Error Handling

### Error Types

1. **DiskFull** - Insufficient disk space (< 10MB)
2. **CorruptFile** - JSON parsing or structure error
3. **VersionMismatch** - Incompatible save file version
4. **FileNotFound** - Save file doesn't exist
5. **PermissionDenied** - Cannot read/write file
6. **InvalidSlotName** - Illegal characters in name
7. **SlotAlreadyExists** - Name conflict on rename
8. **UnknownError** - Unexpected error

### Error Display

Errors are displayed in:
- Error dialog (modal popup)
- Status bar (persistent message)
- Console output (for debugging)

## Version Compatibility

### Version Format

- Semantic versioning: `MAJOR.MINOR.PATCH`
- Current version: `0.1.0`

### Compatibility Rules

- **0.1.x versions:** All compatible within 0.1 series
- **Future versions:** May require migration logic

### Version Mismatch Handling

When loading a save from a different version:
1. Check version in metadata
2. If incompatible, show error message
3. User must update game or discard save

## UI Design

### Save/Load Menu Layout

```
┌────────────────────────────────────────────────┐
│  SAVE GAME / LOAD GAME                         │
├────────────────────────────────────────────────┤
│  SAVE SLOTS                                    │
│  ┌────────────────────────────────────────┐  │
│  │ [Slot 1]         Tower: My Tower        │  │
│  │ 2025-10-07 14:30  |  $25000            │  │
│  ├────────────────────────────────────────┤  │
│  │ [Slot 2]         Tower: Test Tower     │  │
│  │ 2025-10-06 10:15  |  $15000            │  │
│  ├────────────────────────────────────────┤  │
│  │ [New Slot]  (Save Mode Only)           │  │
│  └────────────────────────────────────────┘  │
│                                                │
│  [Save/Load] [Delete] [Rename] [Cancel]       │
├────────────────────────────────────────────────┤
│ Autosave: ON | Last saved: 2m 15s ago         │
│ Error: None                                    │
└────────────────────────────────────────────────┘
```

### Controls

- **Arrow Keys:** Navigate slot list
- **Enter:** Select slot (save or confirm load)
- **Delete:** Delete selected slot
- **Escape:** Close menu or cancel dialog
- **Mouse:** Click buttons and slots

## Integration Points

### With Main Menu

- "Load Game" option opens SaveLoadMenu in load mode
- Integrates with new game flow

### With Pause Menu

- "Save Game" option opens SaveLoadMenu in save mode
- "Load Game" option opens SaveLoadMenu in load mode

### With ECSWorld

- SaveLoadManager accesses ECSWorld for serialization
- Deserializ ation creates/updates entities directly
- TowerGrid is resized to match saved dimensions

## Future Enhancements

1. **Cloud Saves:** Sync saves across devices
2. **Compression:** Compress save files to reduce size
3. **Backup System:** Keep N previous autosaves
4. **Quick Save/Load:** F5/F9 hotkeys
5. **Save Thumbnails:** Screenshot preview of tower
6. **Migration System:** Automatic save file upgrading
7. **Encryption:** Optional save file encryption
8. **Statistics:** Track playtime, achievements in saves

## Testing Checklist

- [x] Save file creation
- [ ] Load saved game
- [ ] Delete save slot
- [ ] Rename save slot
- [ ] Autosave functionality
- [ ] Version compatibility
- [ ] Error handling
- [ ] Disk space checking
- [ ] Corrupt file handling
- [ ] UI navigation
- [ ] Status bar updates

## Known Limitations

1. **Entity IDs:** Entity IDs are not preserved between saves
2. **Pending Actions:** In-flight actions may not be fully restored
3. **UI State:** UI preferences are not saved
4. **Settings:** Game settings are not included in saves (use separate config)

## File Size Estimates

- **Minimal save:** ~5-10 KB (empty tower)
- **Small tower:** ~50-100 KB (10 floors, few entities)
- **Large tower:** ~500 KB - 1 MB (20+ floors, many entities)
- **Autosave overhead:** Same as manual save

## Dependencies

- **nlohmann/json:** JSON serialization library
- **std::filesystem:** Cross-platform file operations
- **Raylib:** UI rendering
- **flecs:** ECS framework (serialized data source)
