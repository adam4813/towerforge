# Notification Center - Visual Reference

This document describes the visual appearance and layout of the notification center in TowerForge.

## Top Bar Notification Button

**Location:** Top right corner of the screen, in the top bar

**Appearance:**
```
┌────────────────────────────────────────────────────────┐
│ [$10000 (+$500/hr)] [Population: 0] [8:30 AM Day 1] [N]│ ← Top Bar
└────────────────────────────────────────────────────────┘
                                                         ↑
                                           Notification Button
```

**Button States:**
- **Inactive (gray):** Notification center is closed
- **Active (gold):** Notification center is open
- **With badge:** Shows red circle with unread count (e.g., "5")

**Hover tooltip:** "Notification Center\nHotkey: N\n[X] unread notifications"

---

## Notification Center Panel

**Location:** Right side of screen, below top bar

**Size:** 400px wide × 600px tall

**Layout:**
```
┌────────────────────────────────────────────────────┐
│ Notifications                    (5)            X  │ ← Header
├────────────────────────────────────────────────────┤
│ Filter: [I][W][E][S][A][V]                         │ ← Filter bar
├────────────────────────────────────────────────────┤
│ ┌────────────────────────────────────────────┐    │
│ │ *  Achievement Unlocked          P    X    │    │ ← Entry
│ │    You've reached 100 tenants!             │    │
│ │    12:34:56                                │    │
│ └────────────────────────────────────────────┘    │
│                                                    │
│ ┌────────────────────────────────────────────┐    │
│ │ !  Low Funds                       P    X  │    │ ← Entry
│ │    Warning: Funds running low              │    │
│ │    12:30:15                                │    │
│ └────────────────────────────────────────────┘    │
│                                                    │
│ ┌────────────────────────────────────────────┐    │
│ │ i  Welcome to TowerForge       P    X      │    │ ← Entry
│ │    Start building your empire!             │    │
│ │    12:00:00                                │    │
│ └────────────────────────────────────────────┘    │
│                                             ▲      │
│                                             █      │ ← Scrollbar
│                                             ▼      │
├────────────────────────────────────────────────────┤
│ [Clear Read] [Clear All] [Mark All Read]          │ ← Actions
└────────────────────────────────────────────────────┘
```

### Header
- **Title:** "Notifications" in gold color
- **Unread Badge:** Red circle with count (if unread > 0)
- **Close Button:** Red "X" to close the panel

### Filter Bar
- **Label:** "Filter:" in light gray
- **Type Buttons:** 6 buttons for filtering by type:
  - **I** (Info) - Sky blue
  - **W** (Warning) - Orange
  - **E** (Error) - Red
  - **S** (Success) - Green
  - **A** (Achievement) - Gold
  - **V** (Event) - Purple
- Active filters are colored, inactive are dark gray

### Notification Entries

Each entry has:
1. **Type Icon** (left side):
   - Info: "i"
   - Warning: "!"
   - Error: "X"
   - Success: "+"
   - Achievement: "*"
   - Event: "@"

2. **Title** (top): Notification title in white

3. **Message** (middle): Truncated message in light gray

4. **Timestamp** (bottom left): Time in format "HH:MM:SS"

5. **Pin Indicator** (if pinned): "P" in yellow

6. **Action Buttons** (on hover only):
   - **Pin/Unpin button:** "P" or "U" button
   - **Dismiss button:** "X" button in red

**Entry Colors:**
- **Background (unread):** Semi-transparent dark blue
- **Background (read):** Semi-transparent dark gray
- **Background (hover):** Semi-transparent sky blue
- **Border:** Type color (semi-transparent)

### Bottom Action Bar

Three buttons:
1. **Clear Read** - Remove all read notifications (except pinned)
2. **Clear All** - Remove all notifications (except pinned)
3. **Mark All Read** - Mark all notifications as read

---

## Toast Notifications

**Location:** Bottom left corner of screen

**Size:** 350px wide × 70px tall per toast

**Max Visible:** 3 toasts at once

**Appearance:**
```
┌──────────────────────────────────────────────┐
│ +  Facility Placed!                         │
│    Office facility placed on floor 5        │
│ ████████████████░░░░░░░░░░░░░░░░░░░░░░░░   │ ← Progress bar
└──────────────────────────────────────────────┘
```

**Features:**
- Type-colored background (90% opacity)
- Type icon (left side)
- Title (top)
- Message (bottom, truncated if too long)
- Progress bar showing time remaining

**Colors by Type:**
- Info: Sky blue
- Warning: Orange
- Error: Red
- Success: Green
- Achievement: Gold
- Event: Purple

---

## Visual Design Specifications

### Colors

| Type        | Color       | Hex/RGB           |
|-------------|-------------|-------------------|
| Info        | Sky Blue    | SKYBLUE (raylib)  |
| Warning     | Orange      | ORANGE (raylib)   |
| Error       | Red         | RED (raylib)      |
| Success     | Green       | GREEN (raylib)    |
| Achievement | Gold        | GOLD (raylib)     |
| Event       | Purple      | PURPLE (raylib)   |

### UI Elements

| Element              | Color        | Opacity |
|----------------------|--------------|---------|
| Panel Background     | Black        | 95%     |
| Panel Border         | Gold         | 100%    |
| Unread Badge         | Red          | 100%    |
| Read Entry BG        | Dark Gray    | 30%     |
| Unread Entry BG      | Dark Blue    | 40%     |
| Hover Entry BG       | Sky Blue     | 30%     |
| Toast Background     | Type Color   | 90%     |

### Typography

- **Title (Panel Header):** 20px, Gold
- **Entry Title:** 14px, White
- **Entry Message:** 11px, Light Gray
- **Timestamp:** 10px, Gray
- **Filter Labels:** 12px, Light Gray
- **Filter Buttons:** 14px, White
- **Action Buttons:** 14px, White
- **Toast Title:** 16px, White
- **Toast Message:** 12px, White (90% opacity)

### Spacing

- Panel Padding: 10px
- Entry Height: 80px
- Entry Spacing: 5px
- Filter Bar Height: 40px
- Button Size: 30x30px (filter buttons)
- Badge Radius: 10-12px

---

## Interaction States

### Notification Entry States

1. **Default (Unread)**
   - Dark blue background (40% opacity)
   - White title
   - Light gray message
   - No action buttons visible

2. **Default (Read)**
   - Dark gray background (30% opacity)
   - White title
   - Light gray message
   - No action buttons visible

3. **Hover**
   - Sky blue background (30% opacity)
   - Action buttons visible (Pin, Dismiss)
   - Slightly brighter text

4. **Pinned**
   - Yellow "P" indicator
   - Won't auto-dismiss
   - Won't be removed by "Clear All"

### Button States

1. **Normal**
   - Dark gray background
   - White text

2. **Active/Hover**
   - Slightly brighter
   - Cursor changes to pointer

3. **Filter Active**
   - Type color background
   - White text

4. **Filter Inactive**
   - Dark gray background (50% opacity)
   - White text (dimmed)

---

## Keyboard & Mouse Interactions

### Keyboard

- **N Key:** Toggle notification center visibility (works anytime)

### Mouse

1. **Click Notification Button (Top Bar):**
   - Toggles notification center panel

2. **Click Notification Entry:**
   - Marks as read
   - Triggers callback (if set)
   - Visual feedback (highlight)

3. **Click Pin Button:**
   - Toggles pin state
   - Prevents auto-dismissal when pinned

4. **Click Dismiss Button:**
   - Removes notification immediately
   - No confirmation

5. **Click Filter Button:**
   - Toggles filter for that type
   - Updates visible notifications immediately

6. **Click Bottom Action Buttons:**
   - Clear Read: Removes all read except pinned
   - Clear All: Removes all except pinned
   - Mark All Read: Marks all as read

7. **Scroll:**
   - Mouse wheel scrolls notification list
   - Scrollbar appears when content overflows

---

## Usage Examples

### Example 1: Achievement Notification

When player unlocks "First Tenants" achievement:

```
┌────────────────────────────────────────────┐
│ *  First Tenants                  P    X   │
│    Have 10 tenants in your tower          │
│    14:23:45                               │
└────────────────────────────────────────────┘
```
- Gold border (achievement type)
- Never auto-dismisses (duration = -1)
- Clickable to open achievements menu

### Example 2: Warning Notification

When funds drop below threshold:

```
┌────────────────────────────────────────────┐
│ !  Low Funds                      P    X   │
│    Warning: Your funds are running low.   │
│    Build income facilities!               │
│    14:20:30                               │
└────────────────────────────────────────────┘
```
- Orange border (warning type)
- Medium priority
- Auto-dismisses after 10 seconds

### Example 3: Success Toast

When facility is placed:

```
┌──────────────────────────────────────────────┐
│ +  Facility Placed!                         │
│    Office facility placed on floor 5        │
│ ████████████████░░░░░░░░░░░░░░░░░░░░░░░░   │
└──────────────────────────────────────────────┘
```
- Green background (success type)
- Shows for 3 seconds
- Progress bar indicates time remaining

---

## Accessibility Features

1. **Color-Coded Types:** Different colors help quickly identify notification types
2. **Icons:** Visual icons supplement colors for color-blind users
3. **Keyboard Shortcut:** Quick access via keyboard (N key)
4. **Persistent Storage:** Important notifications can be pinned
5. **Unread Count:** Badge shows how many unread notifications
6. **Timestamps:** Shows when notification was created
7. **Hover Actions:** Actions only appear on hover to reduce clutter

---

## Best Practices for Adding Notifications

1. **Choose Appropriate Type:**
   - Use Info for general information
   - Use Warning for things that need attention
   - Use Error for critical issues
   - Use Success for positive feedback
   - Use Achievement for unlocks
   - Use Event for special game events

2. **Set Appropriate Priority:**
   - Low: Minor information
   - Medium: Normal notifications
   - High: Important updates
   - Critical: Urgent issues

3. **Choose Good Durations:**
   - Quick feedback: 2-3 seconds
   - Normal info: 5 seconds
   - Important warnings: 8-10 seconds
   - Achievements: Never auto-dismiss (-1)

4. **Add Callbacks When Useful:**
   - Link to relevant UI (achievements menu, facility info, etc.)
   - Focus camera on relevant area
   - Open appropriate panels

5. **Write Clear Messages:**
   - Keep titles short (1-3 words)
   - Make messages descriptive but concise
   - Include actionable information when possible

---

This visual reference should help understand the notification center's appearance and functionality without requiring actual screenshots.
