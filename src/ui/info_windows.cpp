#include "ui/info_windows.h"

namespace towerforge {
namespace ui {

// FacilityWindow implementation
FacilityWindow::FacilityWindow(const FacilityInfo& info)
    : UIWindow("Facility Info", 250, 220)
    , info_(info) {
}

void FacilityWindow::Update(const FacilityInfo& info) {
    info_ = info;
    title_ = info.type + " - Floor " + std::to_string(info.floor);
}

void FacilityWindow::Render() {
    RenderFrame(SKYBLUE);
    RenderCloseButton();
    
    int x = x_ + PADDING;
    int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
    // Occupancy
    std::string occupancy = "Occupancy: " + std::to_string(info_.occupancy) + 
                           "/" + std::to_string(info_.max_occupancy);
    DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Revenue
    std::stringstream revenue_ss;
    revenue_ss << "Revenue: $" << std::fixed << std::setprecision(0) << info_.revenue << "/hr";
    DrawText(revenue_ss.str().c_str(), x, y, 14, GREEN);
    y += 20;
    
    // Satisfaction
    std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(info_.satisfaction) + 
                              " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%";
    DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Tenants
    std::string tenants = "Tenants: " + std::to_string(info_.tenant_count) + " workers";
    DrawText(tenants.c_str(), x, y, 14, LIGHTGRAY);
    y += 30;
    
    // Buttons (placeholder)
    DrawRectangle(x, y, 100, 25, DARKGRAY);
    DrawText("[Demolish]", x + 5, y + 5, 14, RED);
    
    DrawRectangle(x + 110, y, 100, 25, DARKGRAY);
    DrawText("[Upgrade]", x + 115, y + 5, 14, YELLOW);
}

std::string FacilityWindow::GetSatisfactionEmoji(float satisfaction) const {
    if (satisfaction >= 80) return ":)";
    if (satisfaction >= 60) return ":|";
    if (satisfaction >= 40) return ":/";
    return ":(";
}

// PersonWindow implementation
PersonWindow::PersonWindow(const PersonInfo& info)
    : UIWindow("Person Info", 250, 210)
    , info_(info) {
}

void PersonWindow::Update(const PersonInfo& info) {
    info_ = info;
    title_ = info.name;
}

void PersonWindow::Render() {
    RenderFrame(YELLOW);
    RenderCloseButton();
    
    int x = x_ + PADDING;
    int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
    // NPC Type
    std::string type = "Type: " + info_.npc_type;
    DrawText(type.c_str(), x, y, 14, SKYBLUE);
    y += 20;
    
    // Status (current activity)
    std::string status = "Status: " + info_.status;
    DrawText(status.c_str(), x, y, 14, GOLD);
    y += 20;
    
    // State
    std::string state = "State: " + info_.state;
    DrawText(state.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Current floor
    std::string current = "Current: Floor " + std::to_string(info_.current_floor);
    DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Destination
    std::string dest = "Destination: Floor " + std::to_string(info_.destination_floor);
    DrawText(dest.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Wait time
    std::stringstream wait_ss;
    wait_ss << "Wait Time: " << std::fixed << std::setprecision(0) << info_.wait_time << "s";
    DrawText(wait_ss.str().c_str(), x, y, 14, info_.wait_time > 30 ? RED : LIGHTGRAY);
    y += 20;
    
    // Satisfaction
    std::string satisfaction = "Satisfaction: " + GetSatisfactionEmoji(info_.satisfaction) + 
                              " " + std::to_string(static_cast<int>(info_.satisfaction)) + "%";
    DrawText(satisfaction.c_str(), x, y, 14, LIGHTGRAY);
}

std::string PersonWindow::GetSatisfactionEmoji(float satisfaction) const {
    if (satisfaction >= 80) return ":)";
    if (satisfaction >= 60) return ":|";
    if (satisfaction >= 40) return ":/";
    return ":(";
}

// ElevatorWindow implementation
ElevatorWindow::ElevatorWindow(const ElevatorInfo& info)
    : UIWindow("Elevator Info", 250, 150 + (info.queue.size() * 20))
    , info_(info) {
}

void ElevatorWindow::Update(const ElevatorInfo& info) {
    info_ = info;
    title_ = "ELEVATOR #" + std::to_string(info.id);
    height_ = 150 + (static_cast<int>(info.queue.size()) * 20);
}

void ElevatorWindow::Render() {
    RenderFrame(PURPLE);
    RenderCloseButton();
    
    int x = x_ + PADDING;
    int y = y_ + TITLE_BAR_HEIGHT + PADDING;
    
    // Current floor and direction
    std::string current = "Current Floor: " + std::to_string(info_.current_floor) + 
                         " " + info_.direction;
    DrawText(current.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Occupancy
    std::string occupancy = "Occupancy: " + std::to_string(info_.occupancy) + 
                           "/" + std::to_string(info_.max_occupancy);
    DrawText(occupancy.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Next stop
    std::string next = "Next Stop: Floor " + std::to_string(info_.next_stop);
    DrawText(next.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Queue length
    std::string queue = "Queue Length: " + std::to_string(info_.queue.size());
    DrawText(queue.c_str(), x, y, 14, LIGHTGRAY);
    y += 20;
    
    // Queue details
    for (const auto& [floor, waiting] : info_.queue) {
        std::string queue_item = "- Floor " + std::to_string(floor) + ": " + 
                                std::to_string(waiting) + " waiting";
        DrawText(queue_item.c_str(), x + 10, y, 12, GRAY);
        y += 20;
    }
}

} // namespace ui
} // namespace towerforge
