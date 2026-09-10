#pragma once

//==============================================================================
// Entity.hpp
//
// Defines runtime entities, directions, obstacles, and objectives used by
// the GameScript simulation environment.
//==============================================================================

#include <string>
#include <sstream>

namespace gamescript::runtime {

// Represents the four cardinal directions used in the game world.
enum class Direction {
    North = 0,
    East = 1,
    South = 2,
    West = 3
};

// Converts a direction to its string representation.
std::string directionToString(Direction dir);
inline std::ostream& operator<<(std::ostream& os, Direction dir) { return os << directionToString(dir); }

// Represents a generic game entity with position and health information.
struct Entity {
    int x = 0;
    int y = 0;
    int hp = 100;
    int maxHp = 100;
    bool isAlive = true;
};

// Represents the player and its movement and combat state.
struct Player : public Entity {
    Direction facing = Direction::North;
    bool isDefending = false;
    int attackPower = 25;

    // Returns the symbol corresponding to the player's facing direction.
    std::string getFacingSymbol() const {
        switch (facing) {
            case Direction::North: return "^";
            case Direction::East:  return ">";
            case Direction::South: return "v";
            case Direction::West:  return "<";
        }
        return "^";
    }
};

// Represents an enemy and its combat properties.
struct Enemy : public Entity {
    std::string name = "Goblin";
    int attackPower = 15;
};

// Represents an impassable object at a position in the game world.
struct Obstacle {
    int x = 0;
    int y = 0;
};

// Represents a target location and its completion state.
struct Objective {
    int x = 0;
    int y = 0;
    bool isCompleted = false;
};

} // namespace gamescript::runtime