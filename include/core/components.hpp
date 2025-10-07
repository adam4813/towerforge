#pragma once

#include <string>

namespace TowerForge {
namespace Core {

/**
 * @brief Component for entities with a position in 2D space
 */
struct Position {
    float x;
    float y;

    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @brief Component for entities with velocity
 */
struct Velocity {
    float dx;
    float dy;

    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
};

/**
 * @brief Component for actors (people) in the building
 * 
 * Actors represent people who move around the building,
 * use elevators, and interact with facilities.
 */
struct Actor {
    std::string name;
    int floor_destination;  // Target floor
    float speed;            // Movement speed

    Actor(const std::string& name = "Actor", int dest = 0, float spd = 1.0f)
        : name(name), floor_destination(dest), speed(spd) {}
};

/**
 * @brief Component for building components (offices, restaurants, etc.)
 * 
 * BuildingComponents represent the various facilities and rooms
 * that can be placed in the tower.
 */
struct BuildingComponent {
    enum class Type {
        Office,
        Restaurant,
        Shop,
        Hotel,
        Elevator,
        Lobby
    };

    Type type;
    int floor;              // Which floor this component is on
    int width;              // Width in tiles
    int capacity;           // Maximum occupancy
    int current_occupancy;  // Current number of people

    BuildingComponent(Type t = Type::Office, int f = 0, int w = 1, int cap = 10)
        : type(t), floor(f), width(w), capacity(cap), current_occupancy(0) {}
};

} // namespace Core
} // namespace TowerForge
