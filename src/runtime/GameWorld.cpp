#include "gamescript/runtime/GameWorld.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace gamescript::runtime {

std::string directionToString(Direction dir) {
    switch (dir) {
        case Direction::North: return "North";
        case Direction::East:  return "East";
        case Direction::South: return "South";
        case Direction::West:  return "West";
    }
    return "North";
}

GameWorld::GameWorld(int width, int height)
    : width_(width), height_(height) {
    player_.x = 1;
    player_.y = 1;
    player_.facing = Direction::North;
    player_.hp = 100;

    objective_.x = width - 2;
    objective_.y = height - 2;
}

void GameWorld::setPlayerPosition(int x, int y, Direction facing) {
    player_.x = std::clamp(x, 0, width_ - 1);
    player_.y = std::clamp(y, 0, height_ - 1);
    player_.facing = facing;
}

void GameWorld::addEnemy(int x, int y, int hp, std::string name) {
    Enemy enemy;
    enemy.x = std::clamp(x, 0, width_ - 1);
    enemy.y = std::clamp(y, 0, height_ - 1);
    enemy.hp = hp;
    enemy.maxHp = hp;
    enemy.name = std::move(name);
    enemies_.push_back(enemy);
}

void GameWorld::addObstacle(int x, int y) {
    obstacles_.push_back({x, y});
}

void GameWorld::setObjective(int x, int y) {
    objective_.x = x;
    objective_.y = y;
}

bool GameWorld::isOccupied(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return true;
    for (const auto& obs : obstacles_) {
        if (obs.x == x && obs.y == y) return true;
    }
    return false;
}

bool GameWorld::movePlayer(int dirCode, int steps) {
    if (steps <= 0) return true;

    // Calculate move vector based on player's facing direction and relative move code:
    // dirCode: 0 = Forward, 1 = Backward, 2 = Left, 3 = Right
    Direction moveDir = player_.facing;

    if (dirCode == 1) { // Backward
        moveDir = static_cast<Direction>((static_cast<int>(player_.facing) + 2) % 4);
    } else if (dirCode == 2) { // Left
        moveDir = static_cast<Direction>((static_cast<int>(player_.facing) + 3) % 4);
    } else if (dirCode == 3) { // Right
        moveDir = static_cast<Direction>((static_cast<int>(player_.facing) + 1) % 4);
    }

    int dx = 0, dy = 0;
    switch (moveDir) {
        case Direction::North: dy = -1; break;
        case Direction::South: dy = 1; break;
        case Direction::East:  dx = 1; break;
        case Direction::West:  dx = -1; break;
    }

    int actualSteps = 0;
    for (int i = 0; i < steps; ++i) {
        int nextX = player_.x + dx;
        int nextY = player_.y + dy;
        if (!isOccupied(nextX, nextY)) {
            player_.x = nextX;
            player_.y = nextY;
            actualSteps++;
        } else {
            break;
        }
    }

    std::ostringstream oss;
    oss << "Player moved " << actualSteps << "/" << steps << " steps in direction " << directionToString(moveDir)
        << " -> now at (" << player_.x << ", " << player_.y << ")";
    logAction(oss.str());
    return actualSteps > 0;
}

bool GameWorld::turnPlayer(int dirCode, int degrees) {
    int turns90 = (degrees / 90) % 4;
    int current = static_cast<int>(player_.facing);

    if (dirCode == 0) { // Turn Left (counter-clockwise)
        current = (current + 4 - turns90) % 4;
    } else { // Turn Right (clockwise)
        current = (current + turns90) % 4;
    }

    player_.facing = static_cast<Direction>(current);
    std::ostringstream oss;
    oss << "Player turned " << (dirCode == 0 ? "Left" : "Right") << " " << degrees
        << " deg -> facing " << directionToString(player_.facing);
    logAction(oss.str());
    return true;
}

bool GameWorld::playerAttack() {
    int targetX = player_.x;
    int targetY = player_.y;

    switch (player_.facing) {
        case Direction::North: targetY--; break;
        case Direction::South: targetY++; break;
        case Direction::East:  targetX++; break;
        case Direction::West:  targetX--; break;
    }

    for (auto& enemy : enemies_) {
        if (enemy.isAlive) {
            // Hit directly in front or adjacent
            int dist = std::abs(enemy.x - targetX) + std::abs(enemy.y - targetY);
            if (dist == 0 || (std::abs(enemy.x - player_.x) <= 1 && std::abs(enemy.y - player_.y) <= 1)) {
                enemy.hp -= player_.attackPower;
                std::ostringstream oss;
                oss << "Player attacked " << enemy.name << " for " << player_.attackPower << " dmg! (Enemy HP: "
                    << std::max(0, enemy.hp) << "/" << enemy.maxHp << ")";
                if (enemy.hp <= 0) {
                    enemy.isAlive = false;
                    oss << " - " << enemy.name << " was defeated!";
                }
                logAction(oss.str());
                return true;
            }
        }
    }

    logAction("Player swung attack into empty air.");
    return false;
}

bool GameWorld::playerDefend() {
    player_.isDefending = true;
    logAction("Player raised shield in defense stance.");
    return true;
}

bool GameWorld::playerJump() {
    int dx = 0, dy = 0;
    switch (player_.facing) {
        case Direction::North: dy = -2; break;
        case Direction::South: dy = 2; break;
        case Direction::East:  dx = 2; break;
        case Direction::West:  dx = -2; break;
    }

    int targetX = player_.x + dx;
    int targetY = player_.y + dy;
    if (targetX >= 0 && targetX < width_ && targetY >= 0 && targetY < height_) {
        player_.x = targetX;
        player_.y = targetY;
        std::ostringstream oss;
        oss << "Player jumped forward 2 tiles -> landed at (" << player_.x << ", " << player_.y << ")";
        logAction(oss.str());
        return true;
    }

    logAction("Player attempted jump but was blocked by world boundary.");
    return false;
}

bool GameWorld::playerInteract() {
    if (player_.x == objective_.x && player_.y == objective_.y) {
        objective_.isCompleted = true;
        logAction("Player interacted with objective [X] -> OBJECTIVE COMPLETED!");
        return true;
    }

    logAction("Player interacted with surroundings.");
    return false;
}

bool GameWorld::isEnemyNearby(int radius) const {
    for (const auto& enemy : enemies_) {
        if (enemy.isAlive) {
            int dist = std::abs(enemy.x - player_.x) + std::abs(enemy.y - player_.y);
            if (dist <= radius) return true;
        }
    }
    return false;
}

bool GameWorld::isHealthLow(int threshold) const {
    return player_.hp <= threshold;
}

bool GameWorld::isObstacleAhead() const {
    int nextX = player_.x;
    int nextY = player_.y;
    switch (player_.facing) {
        case Direction::North: nextY--; break;
        case Direction::South: nextY++; break;
        case Direction::East:  nextX++; break;
        case Direction::West:  nextX--; break;
    }
    return isOccupied(nextX, nextY);
}

int GameWorld::getDistanceToClosestEnemy() const {
    int minDist = 999;
    for (const auto& enemy : enemies_) {
        if (enemy.isAlive) {
            int dist = std::abs(enemy.x - player_.x) + std::abs(enemy.y - player_.y);
            minDist = std::min(minDist, dist);
        }
    }
    return minDist == 999 ? 0 : minDist;
}

} // namespace gamescript::runtime
