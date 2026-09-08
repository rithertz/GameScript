#pragma once

#include <string>
#include <sstream>

namespace gamescript::runtime {

enum class Direction {
    North = 0,
    East = 1,
    South = 2,
    West = 3
};

std::string directionToString(Direction dir);
inline std::ostream& operator<<(std::ostream& os, Direction dir) { return os << directionToString(dir); }

struct Entity {
    int x = 0;
    int y = 0;
    int hp = 100;
    int maxHp = 100;
    bool isAlive = true;
};

struct Player : public Entity {
    Direction facing = Direction::North;
    bool isDefending = false;
    int attackPower = 25;

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

struct Enemy : public Entity {
    std::string name = "Goblin";
    int attackPower = 15;
};

struct Obstacle {
    int x = 0;
    int y = 0;
};

struct Objective {
    int x = 0;
    int y = 0;
    bool isCompleted = false;
};

} // namespace gamescript::runtime
