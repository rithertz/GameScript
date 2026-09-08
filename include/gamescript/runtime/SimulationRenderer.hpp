#pragma once

#include "gamescript/runtime/GameWorld.hpp"
#include <string>
#include <iostream>

namespace gamescript::runtime {

class SimulationRenderer {
public:
    static std::string renderToString(const GameWorld& world);
    static void render(const GameWorld& world, std::ostream& os = std::cout);
};

} // namespace gamescript::runtime
