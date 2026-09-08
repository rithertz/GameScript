#pragma once

#include "gamescript/runtime/Entity.hpp"
#include <vector>
#include <string>
#include <memory>

namespace gamescript::runtime {

class GameWorld {
public:
    GameWorld(int width = 10, int height = 10);

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    Player& getPlayer() { return player_; }
    const Player& getPlayer() const { return player_; }

    const std::vector<Enemy>& getEnemies() const { return enemies_; }
    std::vector<Enemy>& getEnemies() { return enemies_; }

    const std::vector<Obstacle>& getObstacles() const { return obstacles_; }
    const Objective& getObjective() const { return objective_; }

    // World configuration
    void setPlayerPosition(int x, int y, Direction facing = Direction::North);
    void addEnemy(int x, int y, int hp = 50, std::string name = "Enemy");
    void addObstacle(int x, int y);
    void setObjective(int x, int y);

    // Gameplay Commands
    bool movePlayer(int dirCode, int steps);
    bool turnPlayer(int dirCode, int degrees);
    bool playerAttack();
    bool playerDefend();
    bool playerJump();
    bool playerInteract();

    // Sensory Queries
    bool isEnemyNearby(int radius = 2) const;
    bool isHealthLow(int threshold = 30) const;
    bool isObstacleAhead() const;
    int getDistanceToClosestEnemy() const;

    // Action logging
    const std::vector<std::string>& getActionLog() const { return actionLog_; }
    void logAction(const std::string& msg) { actionLog_.push_back(msg); }
    void clearLog() { actionLog_.clear(); }

    bool isOccupied(int x, int y) const;

private:
    int width_;
    int height_;
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Obstacle> obstacles_;
    Objective objective_;
    std::vector<std::string> actionLog_;
};

} // namespace gamescript::runtime
