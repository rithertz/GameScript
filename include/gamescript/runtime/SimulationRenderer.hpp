#pragma once

//==============================================================================
// SimulationRenderer.hpp
//
// Defines the renderer used to convert the runtime game world into a
// human-readable simulation output.
//==============================================================================

#include "gamescript/runtime/GameWorld.hpp"
#include <string>
#include <iostream>

namespace gamescript::runtime {

// Provides functionality for rendering the game world for simulation output.
class SimulationRenderer {
public:
    // Converts the game world into a string representation.
    static std::string renderToString(const GameWorld& world);

    // Renders the game world to the specified output stream.
    static void render(const GameWorld& world, std::ostream& os = std::cout);
};

} // namespace gamescript::runtime