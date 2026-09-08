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
