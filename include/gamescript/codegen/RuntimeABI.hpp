#pragma once

//==============================================================================
// RuntimeABI.hpp
//
// Defines the runtime interface used to connect generated LLVM code with the
// GameScript runtime and active game world.
//==============================================================================

#include "gamescript/runtime/GameWorld.hpp"
#include <cstdint>

namespace gamescript::codegen {

// Binds the active game world used by the generated runtime functions.
void bindActiveWorld(runtime::GameWorld* world);

// Returns the currently active game world.
runtime::GameWorld* getActiveWorld();

} // namespace gamescript::codegen

// C-linkage functions for LLVM / JIT runtime communication.
extern "C" {
    // Moves the active game world in the specified direction.
    void gs_runtime_move(int32_t dir, int64_t steps);

    // Turns the active game world in the specified direction.
    void gs_runtime_turn(int32_t dir, int64_t degrees);

    // Performs a game action using the specified action code.
    void gs_runtime_action(int32_t actionCode);

    // Reads a sensor value from the active game world.
    int64_t gs_runtime_sensor(int32_t sensorId);

    // Prints an integer value through the GameScript runtime.
    void gs_runtime_print_int(int64_t val);
}