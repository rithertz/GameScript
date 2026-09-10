#pragma once

//==============================================================================
// GameWorld.hpp
//
// Defines the runtime game world and its configuration, gameplay commands,
// sensory queries, and action logging functionality.
//==============================================================================

#include "gamescript/runtime/Entity.hpp"
#include <vector>
#include <string>
#include <memory>

namespace gamescript::runtime {

// Represents the game world containing the player, enemies, obstacles, and objective.
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
    // Configures the initial state and objects in the game world.
    void setPlayerPosition(int x, int y, Direction facing = Direction::North);
    void addEnemy(int x, int y, int hp = 50, std::string name = "Enemy");
    void addObstacle(int x, int y);
    void setObjective(int x, int y);

    // Gameplay Commands
    // Executes movement, combat, and interaction commands for the player.
    bool movePlayer(int dirCode, int steps);
    bool turnPlayer(int dirCode, int degrees);
    bool playerAttack();
    bool playerDefend();
    bool playerJump();
    bool playerInteract();

    // Sensory Queries
    // Provides information about the player's surroundings and state.
    bool isEnemyNearby(int radius = 2) const;
    bool isHealthLow(int threshold = 30) const;
    bool isObstacleAhead() const;
    int getDistanceToClosestEnemy() const;

    // Action logging
    const std::vector<std::string>& getActionLog() const { return actionLog_; }
    void logAction(const std::string& msg) { actionLog_.push_back(msg); }
    void clearLog() { actionLog_.clear(); }

    // Checks whether the specified position is occupied in the game world.
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