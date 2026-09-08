#include "gamescript/runtime/SimulationRenderer.hpp"
#include <sstream>

namespace gamescript::runtime {

std::string SimulationRenderer::renderToString(const GameWorld& world) {
    std::ostringstream oss;
    int w = world.getWidth();
    int h = world.getHeight();

    // Top border
    oss << "+-" << std::string(w * 2, '-') << "+\n";

    for (int y = 0; y < h; ++y) {
        oss << "| ";
        for (int x = 0; x < w; ++x) {
            bool rendered = false;

            // Player
            if (world.getPlayer().x == x && world.getPlayer().y == y) {
                oss << "P" << world.getPlayer().getFacingSymbol();
                rendered = true;
            }

            // Enemies
            if (!rendered) {
                for (const auto& enemy : world.getEnemies()) {
                    if (enemy.isAlive && enemy.x == x && enemy.y == y) {
                        oss << "E ";
                        rendered = true;
                        break;
                    }
                }
            }

            // Obstacles
            if (!rendered) {
                for (const auto& obs : world.getObstacles()) {
                    if (obs.x == x && obs.y == y) {
                        oss << "# ";
                        rendered = true;
                        break;
                    }
                }
            }

            // Objective
            if (!rendered) {
                if (world.getObjective().x == x && world.getObjective().y == y) {
                    oss << (world.getObjective().isCompleted ? "O " : "X ");
                    rendered = true;
                }
            }

            // Empty tile
            if (!rendered) {
                oss << ". ";
            }
        }
        oss << "|\n";
    }

    // Bottom border
    oss << "+-" << std::string(w * 2, '-') << "+\n";

    // Player Status
    const auto& p = world.getPlayer();
    oss << "Player Status: Pos=(" << p.x << ", " << p.y << ") | Facing=" << directionToString(p.facing)
        << " | HP=" << p.hp << "/" << p.maxHp << " | Defending=" << (p.isDefending ? "YES" : "NO") << "\n";

    // Recent action logs
    const auto& logs = world.getActionLog();
    if (!logs.empty()) {
        oss << "--- Action History ---\n";
        size_t startIdx = (logs.size() > 5) ? logs.size() - 5 : 0;
        for (size_t i = startIdx; i < logs.size(); ++i) {
            oss << " [" << (i + 1) << "] " << logs[i] << "\n";
        }
    }

    return oss.str();
}

void SimulationRenderer::render(const GameWorld& world, std::ostream& os) {
    os << renderToString(world);
}

} // namespace gamescript::runtime
