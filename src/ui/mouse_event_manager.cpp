#include "ui/mouse_event_manager.h"
#include <algorithm>

namespace towerforge::ui {

    int MouseEventManager::RegisterRegion(const MouseEventRegion& region) {
        const int handle = next_handle_++;
        regions_.emplace_back(handle, region);
        return handle;
    }

    void MouseEventManager::UnregisterRegion(const int handle) {
        regions_.erase(
            std::remove_if(regions_.begin(), regions_.end(),
                          [handle](const Region& r) { return r.handle == handle; }),
            regions_.end()
        );
    }

    void MouseEventManager::UpdateRegionBounds(const int handle, const Rectangle new_bounds) {
        auto it = std::ranges::find_if(regions_, 
            [handle](const Region& r) { return r.handle == handle; });
        
        if (it != regions_.end()) {
            it->data.bounds = new_bounds;
        }
    }

    void MouseEventManager::ProcessMouseEvents(const MouseEvent& event) {
        // Find all regions at the mouse position
        auto regions_at_point = FindRegionsAtPoint(event.x, event.y);

        // First, update hover states for all regions
        for (auto& region : regions_) {
            region.is_hovered = false;
        }

        // Process hover events (highest priority first)
        bool hover_consumed = false;
        for (auto* region : regions_at_point) {
            if (!hover_consumed) {
                region->is_hovered = true;
                if (region->data.on_hover) {
                    hover_consumed = region->data.on_hover(event);
                }
            }
        }

        // Process click events only if a button was pressed
        if (event.left_pressed || event.right_pressed) {
            bool click_consumed = false;
            for (auto* region : regions_at_point) {
                if (!click_consumed && region->data.on_click) {
                    click_consumed = region->data.on_click(event);
                }
                // Stop processing after first consumed click
                if (click_consumed) {
                    break;
                }
            }
        }
    }

    void MouseEventManager::ClearAllRegions() {
        regions_.clear();
    }

    std::vector<MouseEventManager::Region*> MouseEventManager::FindRegionsAtPoint(
        const float x, const float y) {
        
        std::vector<Region*> result;
        
        // Find all regions containing the point
        for (auto& region : regions_) {
            const auto& bounds = region.data.bounds;
            if (x >= bounds.x && x <= bounds.x + bounds.width &&
                y >= bounds.y && y <= bounds.y + bounds.height) {
                result.push_back(&region);
            }
        }
        
        // Sort by priority (highest first)
        std::ranges::sort(result, [](const Region* a, const Region* b) {
            return a->data.priority > b->data.priority;
        });
        
        return result;
    }

}
