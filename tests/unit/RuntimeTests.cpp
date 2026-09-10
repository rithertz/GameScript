#include "TestHarness.hpp"
#include "gamescript/lexer/Lexer.hpp"
#include "gamescript/parser/Parser.hpp"
#include "gamescript/ir/IRBuilder.hpp"
#include "gamescript/runtime/GameWorld.hpp"
#include "gamescript/runtime/VirtualMachine.hpp"
#include "gamescript/runtime/SimulationRenderer.hpp"

using namespace gamescript;
using namespace gamescript::runtime;

GS_TEST(RuntimeTests, PlayerMovementAndTurning) {
    GameWorld world(10, 10);
    world.setPlayerPosition(2, 2, Direction::North);

    world.movePlayer(0, 2); // Forward (North) 2 steps: (2, 2) -> (2, 0)
    GS_ASSERT_EQ(world.getPlayer().x, 2);
    GS_ASSERT_EQ(world.getPlayer().y, 0);

    world.turnPlayer(1, 90); // Turn Right 90 deg -> East
    GS_ASSERT_EQ(world.getPlayer().facing, Direction::East);

    world.movePlayer(0, 3); // Forward (East) 3 steps: (2, 0) -> (5, 0)
    GS_ASSERT_EQ(world.getPlayer().x, 5);
    GS_ASSERT_EQ(world.getPlayer().y, 0);
}

GS_TEST(RuntimeTests, CombatAndSensorEvaluation) {
    GameWorld world(10, 10);
    world.setPlayerPosition(3, 3, Direction::North);
    world.addEnemy(3, 2, 50, "Goblin"); // Directly in front of player

    GS_ASSERT(world.isEnemyNearby(2));
    GS_ASSERT_EQ(world.getDistanceToClosestEnemy(), 1);

    // Player attacks
    bool hit = world.playerAttack();
    GS_ASSERT(hit);
    GS_ASSERT_EQ(world.getEnemies()[0].hp, 25);

    // Attack again to defeat
    world.playerAttack();
    GS_ASSERT_EQ(world.getEnemies()[0].hp, 0);
    GS_ASSERT(!world.getEnemies()[0].isAlive);
    GS_ASSERT(!world.isEnemyNearby(2));
}

GS_TEST(RuntimeTests, EndToEndProgramVMExecution) {
    std::string source = 
        "set speed = 2\n"
        "player move forward speed\n"
        "player turn right 90\n"
        "if enemy nearby:\n"
        "    player attack\n"
        "else:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "e2e.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GameWorld world(10, 10);
    world.setPlayerPosition(1, 5, Direction::North);
    world.addEnemy(1, 3, 30, "Orc"); // Nearby to (1, 5)

    VirtualMachine vm(world);
    bool success = vm.execute(*module);
    GS_ASSERT(success);

    // Player started at (1, 5) facing North.
    // Moved forward 2 -> (1, 3) where enemy was, blocked at (1, 4) or reached (1, 3)
    // Turned right 90 -> facing East.
    // Enemy is nearby -> attacked!
    GS_ASSERT_EQ(world.getPlayer().facing, Direction::East);
    GS_ASSERT(!world.getActionLog().empty());
}

GS_TEST(RuntimeTests, MovementDirectionsAndBoundaries) {
    GameWorld world(5, 5);
    world.setPlayerPosition(2, 2, Direction::North);

    // Forward: North
    GS_ASSERT(world.movePlayer(0, 1));
    GS_ASSERT_EQ(world.getPlayer().x, 2);
    GS_ASSERT_EQ(world.getPlayer().y, 1);

    // Right relative to North: East
    GS_ASSERT(world.movePlayer(3, 1));
    GS_ASSERT_EQ(world.getPlayer().x, 3);
    GS_ASSERT_EQ(world.getPlayer().y, 1);

    // Backward relative to North: South
    GS_ASSERT(world.movePlayer(1, 1));
    GS_ASSERT_EQ(world.getPlayer().x, 3);
    GS_ASSERT_EQ(world.getPlayer().y, 2);

    // Move beyond boundary should stop at edge
    world.setPlayerPosition(0, 0, Direction::North);
    GS_ASSERT(!world.movePlayer(0, 1));
    GS_ASSERT_EQ(world.getPlayer().x, 0);
    GS_ASSERT_EQ(world.getPlayer().y, 0);
}

GS_TEST(RuntimeTests, ObstacleCollision) {
    GameWorld world(10, 10);
    world.setPlayerPosition(3, 3, Direction::East);
    world.addObstacle(4, 3);

    GS_ASSERT(world.isObstacleAhead());

    bool moved = world.movePlayer(0, 2);

    GS_ASSERT(!moved);
    GS_ASSERT_EQ(world.getPlayer().x, 3);
    GS_ASSERT_EQ(world.getPlayer().y, 3);
}

GS_TEST(RuntimeTests, PlayerActions) {
    GameWorld world(10, 10);
    world.setPlayerPosition(5, 5, Direction::North);

    GS_ASSERT(world.playerDefend());
    GS_ASSERT(world.getPlayer().isDefending);

    GS_ASSERT(world.playerJump());
    GS_ASSERT_EQ(world.getPlayer().y, 3);

    GS_ASSERT(world.playerInteract() == false);

    GS_ASSERT(!world.getActionLog().empty());
}

GS_TEST(RuntimeTests, ObjectiveInteraction) {
    GameWorld world(10, 10);
    world.setObjective(4, 4);
    world.setPlayerPosition(4, 4, Direction::North);

    GS_ASSERT(world.playerInteract());
    GS_ASSERT(world.getObjective().isCompleted);

    GS_ASSERT(!world.getActionLog().empty());
}

GS_TEST(RuntimeTests, HealthSensorThreshold) {
    GameWorld world(10, 10);
    world.setPlayerPosition(2, 2);

    world.getPlayer().hp = 100;
    GS_ASSERT(!world.isHealthLow());

    world.getPlayer().hp = 30;
    GS_ASSERT(world.isHealthLow());

    world.getPlayer().hp = 10;
    GS_ASSERT(world.isHealthLow());

    GS_ASSERT(world.isHealthLow(10));
    GS_ASSERT(!world.isHealthLow(9));
}

GS_TEST(RuntimeTests, EnemyDistanceAndDeadEnemySensor) {
    GameWorld world(10, 10);
    world.setPlayerPosition(2, 2);
    world.addEnemy(2, 5, 40, "Goblin");
    world.addEnemy(8, 8, 40, "Orc");

    GS_ASSERT_EQ(world.getDistanceToClosestEnemy(), 3);
    GS_ASSERT(world.isEnemyNearby(3));
    GS_ASSERT(!world.isEnemyNearby(2));

    world.getEnemies()[0].isAlive = false;

    GS_ASSERT_EQ(world.getDistanceToClosestEnemy(), 12);
    GS_ASSERT(!world.isEnemyNearby(3));
}

GS_TEST(RuntimeTests, ConditionalSensorExecution) {
    std::string source =
        "if enemy nearby:\n"
        "    player attack\n"
        "else:\n"
        "    player move forward 2\n";

    Lexer lexer(source, "sensor_runtime.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GameWorld world(10, 10);
    world.setPlayerPosition(2, 5, Direction::North);
    world.addEnemy(2, 4, 30, "Goblin");

    VirtualMachine vm(world);
    GS_ASSERT(vm.execute(*module));

    // Enemy is nearby and directly ahead, so the attack branch should run.
    GS_ASSERT_EQ(world.getEnemies()[0].hp, 5);
    GS_ASSERT_EQ(world.getPlayer().y, 5);
}

GS_TEST(RuntimeTests, RepeatLoopExecution) {
    std::string source =
        "repeat 3:\n"
        "    player move forward 1\n";

    Lexer lexer(source, "repeat_runtime.gs");
    Parser parser(lexer.tokenize());
    auto program = parser.parseProgram();

    ir::IRBuilder builder;
    auto module = builder.build(*program);

    GameWorld world(10, 10);
    world.setPlayerPosition(2, 5, Direction::North);

    VirtualMachine vm(world);
    GS_ASSERT(vm.execute(*module));

    GS_ASSERT_EQ(world.getPlayer().x, 2);
    GS_ASSERT_EQ(world.getPlayer().y, 2);
    GS_ASSERT_EQ(world.getActionLog().size(), static_cast<size_t>(3));
}