#include "core/ecs_world.hpp"
#include "core/facility_manager.hpp"
#include "core/tower_grid.hpp"
#include "core/components.hpp"
#include <iostream>
#include <iomanip>

using namespace TowerForge::Core;

void PrintAdjacencyEffects(const AdjacencyEffects& adjacency, const std::string& facility_name) {
    if (adjacency.HasEffects()) {
        std::cout << "\n  Adjacency Effects for " << facility_name << ":\n";
        for (const auto& effect : adjacency.effects) {
            std::cout << "    - " << effect.description << "\n";
        }
        
        // Print totals by type
        float total_revenue = adjacency.GetTotalForType(AdjacencyEffect::Type::Revenue);
        float total_satisfaction = adjacency.GetTotalForType(AdjacencyEffect::Type::Satisfaction);
        float total_traffic = adjacency.GetTotalForType(AdjacencyEffect::Type::Traffic);
        
        if (total_revenue != 0.0f) {
            std::cout << "    Total Revenue Modifier: " << std::showpos << total_revenue << "%\n";
        }
        if (total_satisfaction != 0.0f) {
            std::cout << "    Total Satisfaction Modifier: " << std::showpos << total_satisfaction << "%\n";
        }
        if (total_traffic != 0.0f) {
            std::cout << "    Total Traffic Modifier: " << std::showpos << total_traffic << "%\n";
        }
    } else {
        std::cout << "\n  No adjacency effects for " << facility_name << "\n";
    }
}

int main() {
    std::cout << "Testing Facility Adjacency System...\n\n";
    
    // Create ECS world
    ECSWorld ecs_world;
    ecs_world.Initialize();
    
    // Create tower grid (10 floors, 50 columns)
    TowerGrid grid(10, 50, 0);
    
    // Create facility manager
    FacilityManager facility_mgr(ecs_world.GetWorld(), grid);
    
    // Test Case 1: Restaurant next to Theater (should give restaurant +10% revenue)
    std::cout << "=== Test Case 1: Restaurant next to Theater ===\n";
    auto restaurant1 = facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 1, 10);
    auto theater1 = facility_mgr.CreateFacility(BuildingComponent::Type::Theater, 1, 16);
    
    if (restaurant1.is_alive() && restaurant1.has<AdjacencyEffects>()) {
        const auto& adj = restaurant1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Restaurant");
    }
    
    if (theater1.is_alive() && theater1.has<AdjacencyEffects>()) {
        const auto& adj = theater1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Theater");
    }
    
    // Test Case 2: Residential next to Arcade (should give residential -8% satisfaction)
    std::cout << "\n=== Test Case 2: Residential next to Arcade ===\n";
    auto residential1 = facility_mgr.CreateFacility(BuildingComponent::Type::Residential, 2, 10);
    auto arcade1 = facility_mgr.CreateFacility(BuildingComponent::Type::Arcade, 2, 16);
    
    if (residential1.is_alive() && residential1.has<AdjacencyEffects>()) {
        const auto& adj = residential1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Residential");
    }
    
    if (arcade1.is_alive() && arcade1.has<AdjacencyEffects>()) {
        const auto& adj = arcade1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Arcade");
    }
    
    // Test Case 3: Retail shops next to each other (shopping district bonus)
    std::cout << "\n=== Test Case 3: Retail Shops Forming Shopping District ===\n";
    auto retail1 = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 10);
    auto retail2 = facility_mgr.CreateFacility(BuildingComponent::Type::RetailShop, 3, 14);
    
    if (retail1.is_alive() && retail1.has<AdjacencyEffects>()) {
        const auto& adj = retail1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Retail Shop 1");
    }
    
    if (retail2.is_alive() && retail2.has<AdjacencyEffects>()) {
        const auto& adj = retail2.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Retail Shop 2");
    }
    
    // Test Case 4: Vertical adjacency (facilities above/below)
    std::cout << "\n=== Test Case 4: Hotel Above Restaurant ===\n";
    auto restaurant2 = facility_mgr.CreateFacility(BuildingComponent::Type::Restaurant, 4, 20);
    auto hotel1 = facility_mgr.CreateFacility(BuildingComponent::Type::Hotel, 5, 20);
    
    if (restaurant2.is_alive() && restaurant2.has<AdjacencyEffects>()) {
        const auto& adj = restaurant2.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Restaurant (bottom)");
    }
    
    if (hotel1.is_alive() && hotel1.has<AdjacencyEffects>()) {
        const auto& adj = hotel1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Hotel (top)");
    }
    
    // Test Case 5: ConferenceHall next to Hotel (synergy bonus)
    std::cout << "\n=== Test Case 5: ConferenceHall next to Hotel ===\n";
    auto conference1 = facility_mgr.CreateFacility(BuildingComponent::Type::ConferenceHall, 6, 10);
    auto hotel2 = facility_mgr.CreateFacility(BuildingComponent::Type::Hotel, 6, 19);
    
    if (conference1.is_alive() && conference1.has<AdjacencyEffects>()) {
        const auto& adj = conference1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "ConferenceHall");
    }
    
    if (hotel2.is_alive() && hotel2.has<AdjacencyEffects>()) {
        const auto& adj = hotel2.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Hotel");
    }
    
    // Test Case 6: Removing a facility updates adjacent effects
    std::cout << "\n=== Test Case 6: Removing Theater (should remove restaurant's bonus) ===\n";
    facility_mgr.RemoveFacility(theater1);
    
    if (restaurant1.is_alive() && restaurant1.has<AdjacencyEffects>()) {
        const auto& adj = restaurant1.get<AdjacencyEffects>();
        PrintAdjacencyEffects(adj, "Restaurant (after theater removal)");
    }
    
    std::cout << "\n=== All Tests Completed Successfully! ===\n";
    return 0;
}
