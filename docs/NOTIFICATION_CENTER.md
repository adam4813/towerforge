# Notification Center — Centralized Event & Alert Management

## Quick Summary

The Notification Center is a comprehensive system for displaying game events, warnings, achievements, and errors in TowerForge. It provides users with the ability to filter, dismiss, pin, and interact with notifications. Notifications can be clickable to focus relevant UI areas or trigger game actions.

**Location:** `include/ui/notification_center.h`, `src/ui/notification_center.cpp`

---

## Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Usage](#usage)
4. [Data Structures](#data-structures)
5. [UI Components](#ui-components)
6. [Integration](#integration)
7. [Examples](#examples)
8. [Implementation Details](#implementation-details)

---

## 1. Overview

The Notification Center provides a centralized location for all game notifications, replacing the simple toast-only system with a rich, interactive panel. Users can:

- View all notifications in one place
- Filter by type (Info, Warning, Error, Success, Achievement, Event)
- Pin important notifications to prevent auto-dismissal
- Dismiss individual or bulk notifications
- Click notifications to trigger contextual actions
- See unread notification count at a glance

### Key Components

1. **Notification Panel** - Scrollable panel showing all notifications
2. **Toast System** - Small popup notifications for recent events
3. **Top Bar Button** - Quick access with unread count badge
4. **Filter Controls** - Type-based filtering system

---

## 2. Features

### Core Features

- **Multiple Notification Types:**
  - Info: General information
  - Warning: Attention-required warnings
  - Error: Critical issues
  - Success: Successful actions
  - Achievement: Achievement unlocks
  - Event: Game events

- **Priority Levels:**
  - Low
  - Medium
  - High
  - Critical

- **Interactive Features:**
  - Click notifications to trigger callbacks
  - Pin/unpin notifications
  - Mark as read/unread
  - Dismiss individual notifications
  - Bulk actions (clear all, clear read, mark all read)

- **Filtering:**
  - Filter by notification type
  - Show/hide read notifications
  - Show/hide pinned notifications

- **Auto-dismissal:**
  - Configurable timeout for each notification
  - Pinned notifications never auto-dismiss
  - Achievements default to no auto-dismiss

---

## 3. Usage

### Keyboard Shortcuts

- **N** - Toggle notification center visibility

### Mouse Interactions

- **Click Notification Button** (top bar) - Toggle notification center
- **Click Notification Entry** - Mark as read and trigger callback
- **Click Pin Button** - Pin/unpin notification
- **Click Dismiss Button** (X) - Remove notification
- **Click Filter Buttons** - Toggle type filters
- **Click Bottom Buttons** - Bulk actions

---

## 4. Data Structures

### NotificationEntry

```cpp
struct NotificationEntry {
    std::string id;                  // Unique identifier
    std::string title;               // Notification title
    std::string message;             // Detailed message
    NotificationType type;           // Type of notification
    NotificationPriority priority;   // Priority level
    bool pinned;                     // Whether notification is pinned
    bool read;                       // Whether notification has been read
    std::chrono::system_clock::time_point timestamp; // When created
    std::function<void()> on_click;  // Callback when notification is clicked
    float time_remaining;            // Time until auto-dismiss (-1 for no auto-dismiss)
};
```

### NotificationFilter

```cpp
struct NotificationFilter {
    bool show_info = true;
    bool show_warning = true;
    bool show_error = true;
    bool show_success = true;
    bool show_achievement = true;
    bool show_event = true;
    bool show_read = true;
    bool show_unread = true;
    bool show_pinned = true;
    bool show_unpinned = true;
};
```

---

## 5. UI Components

### Notification Panel

- **Location:** Right side of screen, below top bar
- **Size:** 400x600 pixels
- **Background:** Semi-transparent black (95% opacity)
- **Border:** Gold

#### Panel Elements:

1. **Header:**
   - Title: "Notifications"
   - Unread count badge (red circle)
   - Close button (X)

2. **Filter Bar:**
   - Type filter buttons (I, W, E, S, A, V)
   - Color-coded by type

3. **Notification List:**
   - Scrollable area
   - Each entry shows:
     - Type icon
     - Title
     - Message (truncated)
     - Timestamp
     - Pin indicator (if pinned)
     - Action buttons (on hover)

4. **Bottom Action Bar:**
   - "Clear Read" button
   - "Clear All" button
   - "Mark All Read" button

### Toast Notifications

- **Location:** Bottom left corner
- **Size:** 350x70 pixels per toast
- **Max Visible:** 3 toasts at once
- **Duration:** Configurable per notification (default 5 seconds)
- **Features:**
  - Type-colored background
  - Icon indicator
  - Title and message
  - Progress bar showing time remaining

### Top Bar Button

- **Location:** Top right of screen
- **Size:** 70x30 pixels
- **Icon:** "N"
- **Badge:** Red circle with unread count
- **States:**
  - Normal: Dark gray background
  - Active (panel open): Gold background

---

## 6. Integration

### Adding Notifications

From game code:

```cpp
// Get notification center from HUD
auto* nc = hud->GetNotificationCenter();

// Add a simple notification
nc->AddNotification(
    "Title",
    "Message",
    NotificationType::Info,
    NotificationPriority::Medium,
    5.0f  // Duration in seconds
);

// Add a clickable notification
nc->AddNotification(
    "Achievement Unlocked",
    "You've reached 100 tenants!",
    NotificationType::Achievement,
    NotificationPriority::High,
    -1.0f,  // No auto-dismiss
    []() {
        // Open achievements menu when clicked
        achievements_menu->Open();
    }
);
```

### Legacy Compatibility

The HUD's `AddNotification` method automatically forwards notifications to both the legacy toast system and the new notification center:

```cpp
hud->AddNotification(Notification::Type::Success, "Facility placed!", 3.0f);
```

---

## 7. Examples

### Example 1: Achievement Notification

When an achievement is unlocked:

```cpp
nc->AddNotification(
    achievement.name,
    achievement.description,
    NotificationType::Achievement,
    NotificationPriority::High,
    -1.0f,  // Never auto-dismiss
    [this]() {
        // Open achievements menu when clicked
        achievements_menu_.Open();
    }
);
```

### Example 2: Warning Notification

Low funds warning:

```cpp
nc->AddNotification(
    "Low Funds",
    "Warning: Your funds are running low. Build income-generating facilities!",
    NotificationType::Warning,
    NotificationPriority::Medium,
    10.0f  // Show for 10 seconds
);
```

### Example 3: Error Notification

Failed action:

```cpp
nc->AddNotification(
    "Cannot Build",
    "Not enough funds to build this facility.",
    NotificationType::Error,
    NotificationPriority::Low,
    3.0f
);
```

### Example 4: Event Notification

Special event:

```cpp
nc->AddNotification(
    "Rush Hour",
    "Rush hour has started! Expect increased elevator traffic.",
    NotificationType::Event,
    NotificationPriority::Medium,
    8.0f,
    [this, floor]() {
        // Focus camera on ground floor when clicked
        camera->FocusOnFloor(floor);
    }
);
```

---

## 8. Implementation Details

### File Structure

```
include/ui/notification_center.h  - Header with class definitions
src/ui/notification_center.cpp    - Implementation
```

### Key Methods

#### NotificationCenter

- `AddNotification()` - Add a new notification
- `RemoveNotification()` - Remove by ID
- `PinNotification()` - Pin a notification
- `UnpinNotification()` - Unpin a notification
- `MarkAsRead()` - Mark as read
- `ClearAll()` - Remove all except pinned
- `ClearRead()` - Remove all read except pinned
- `ToggleVisibility()` - Show/hide panel
- `HandleClick()` - Process mouse clicks
- `Render()` - Render the panel
- `RenderToasts()` - Render toast notifications

#### NotificationEntry

- `GetTimestampString()` - Format timestamp for display
- `MarkAsRead()` - Mark as read
- `TogglePin()` - Toggle pinned state

### Visual Design

#### Colors by Type

- Info: Sky Blue (`SKYBLUE`)
- Warning: Orange (`ORANGE`)
- Error: Red (`RED`)
- Success: Green (`GREEN`)
- Achievement: Gold (`GOLD`)
- Event: Purple (`PURPLE`)

#### Icons by Type

- Info: "i"
- Warning: "!"
- Error: "X"
- Success: "+"
- Achievement: "*"
- Event: "@"

### Performance Considerations

1. **Memory Management:**
   - Old notifications auto-removed after timeout
   - Pinned notifications persist until manually dismissed
   - Maximum toast limit prevents overflow

2. **Rendering:**
   - Only visible notifications are rendered
   - Toasts use alpha blending for smooth appearance
   - Scrolling handled efficiently

3. **User Experience:**
   - Unread count provides quick status
   - Filter system reduces clutter
   - Pinning prevents losing important notifications
   - Clickable notifications provide contextual navigation

---

## Integration with Other Systems

### HUD Integration

The notification center is integrated into the HUD:

1. Created in HUD constructor
2. Updated in HUD::Update()
3. Rendered in HUD::Render()
4. Accessible via HUD::GetNotificationCenter()

### Achievement System Integration

Achievements automatically generate notification center entries with:
- Achievement type
- High priority
- No auto-dismiss
- Clickable to open achievements menu

### Game Events

Game events can trigger notifications:
- Facility placement/demolition
- Floor/basement addition
- Financial milestones
- Time-based events
- Tutorial steps

---

## Future Enhancements

Potential improvements:

1. **Notification History:**
   - Persistent log of all notifications
   - Search and filter history
   - Export to file

2. **Notification Categories:**
   - Group by source (facility, person, system)
   - Smart grouping of similar notifications

3. **Custom Notification Templates:**
   - Rich text formatting
   - Images and icons
   - Progress bars for ongoing tasks

4. **Notification Sounds:**
   - Different sounds per type/priority
   - Volume control per category

5. **Notification Rules:**
   - User-configurable filters
   - Auto-pin certain types
   - Auto-dismiss after read

6. **Mobile Notifications:**
   - In multiplayer mode, notifications about tower status
   - Push notifications when not actively playing

---

## Troubleshooting

### Notifications Not Appearing

1. Check that notification center is initialized in HUD
2. Verify notification type isn't filtered out
3. Check that duration isn't 0 or negative (unless pinned)

### Click Not Working

1. Ensure callback function is valid
2. Check that HandleClick is being called
3. Verify notification panel bounds

### Performance Issues

1. Limit number of active notifications
2. Reduce toast count (MAX_TOASTS constant)
3. Clear old read notifications periodically

---

## Related Documentation

- [HUD.md](HUD.md) - Main HUD system
- [ACHIEVEMENTS.md](ACHIEVEMENTS.md) - Achievement system
- [TOOLTIP_SYSTEM.md](TOOLTIP_SYSTEM.md) - Tooltip system

---

## Version History

- **v1.0** (October 2025) - Initial implementation
  - Basic notification center panel
  - Toast system
  - Type filtering
  - Pin/dismiss functionality
  - Clickable notifications
  - Achievement integration
