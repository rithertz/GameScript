#include "gamescript/codegen/RuntimeABI.hpp"
#include <iostream>

namespace gamescript::codegen {

static runtime::GameWorld* g_activeWorld = nullptr;

void bindActiveWorld(runtime::GameWorld* world) {
    g_activeWorld = world;
}

runtime::GameWorld* getActiveWorld() {
    return g_activeWorld;
}

} // namespace gamescript::codegen

extern "C" {

void gs_runtime_move(int32_t dir, int64_t steps) {
    auto* world = gamescript::codegen::getActiveWorld();
    if (world) {
        world->movePlayer(dir, static_cast<int>(steps));
    }
}

void gs_runtime_turn(int32_t dir, int64_t degrees) {
    auto* world = gamescript::codegen::getActiveWorld();
    if (world) {
        world->turnPlayer(dir, static_cast<int>(degrees));
    }
}

void gs_runtime_action(int32_t actionCode) {
    auto* world = gamescript::codegen::getActiveWorld();
    if (world) {
        switch (actionCode) {
            case 0: world->playerAttack(); break;
            case 1: world->playerDefend(); break;
            case 2: world->playerJump(); break;
            case 3: world->playerInteract(); break;
            case 4: world->movePlayer(1, 2); break;
        }
    }
}

int64_t gs_runtime_sensor(int32_t sensorId) {
    auto* world = gamescript::codegen::getActiveWorld();
    if (!world) return 0;

    switch (sensorId) {
        case 0: return world->isEnemyNearby() ? 1 : 0;
        case 1: return world->isHealthLow() ? 1 : 0;
        case 2: return world->isObstacleAhead() ? 1 : 0;
        case 3: return world->getDistanceToClosestEnemy();
        case 4: return world->getPlayer().hp;
    }
    return 0;
}

void gs_runtime_print_int(int64_t val) {
    std::cout << "[Runtime Log] " << val << std::endl;
}

}
