# Analytics Overlays Feature

## Overview
The analytics overlays provide detailed breakdowns of your tower's statistics and performance. These overlays can be accessed by clicking on relevant areas in the HUD.

## Available Analytics

### Income Analytics
**How to Access**: Click on the funds/income display in the top-left corner of the HUD.

**Information Displayed**:
- Total hourly revenue across all facilities
- Operating costs per hour
- Net profit/loss per hour
- Revenue breakdown by facility type:
  - Facility count of each type
  - Total hourly revenue per type
  - Number of tenants
  - Average occupancy rate

This overlay helps you understand which facility types are most profitable and where you should focus your building strategy.

### Population Analytics
**How to Access**: Click on the population count in the top bar (appears after the income display).

**Information Displayed**:
- Total population in your tower
- Breakdown by type:
  - Number of employees
  - Number of visitors
- Residential statistics:
  - Current residential occupancy
  - Total residential capacity
  - Occupancy rate
- Average satisfaction across all people

This overlay helps you understand your tower's population dynamics and residential needs.

### Elevator Analytics (Coming Soon)
This overlay will show detailed statistics about elevator usage, wait times, and efficiency.

## User Interface

### Clickable Areas
The following HUD elements are now clickable to show analytics:
- **Funds Display**: Shows income analytics
- **Population Display**: Shows population analytics

### Tooltips
When hovering over these areas, tooltips now indicate that you can click for detailed analytics:
- Income tooltip: "Click for detailed income analytics."
- Population tooltip: "Click for detailed population analytics."

## Technical Details

### Data Collection
Analytics data is collected in real-time from the ECS (Entity Component System):
- Income data aggregates information from all facilities with `FacilityEconomics` components
- Population data queries all entities with `Person` components
- Data is grouped by type and aggregated for easy visualization

### Window Management
Analytics overlays are managed by the existing `UIWindowManager` system:
- Multiple overlays can be open simultaneously
- Overlays can be closed using the X button in the top-right corner
- New clicks will open additional instances (useful for comparing different time points)

## Future Enhancements
Planned improvements for analytics overlays:
- Historical data tracking (graphs over time)
- Elevator usage analytics with heat maps
- Facility efficiency comparisons
- Satisfaction trend analysis
- Export analytics to CSV for external analysis.
