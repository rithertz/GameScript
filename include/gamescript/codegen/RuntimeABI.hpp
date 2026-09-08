#pragma once

#include "gamescript/runtime/GameWorld.hpp"
#include <cstdint>

namespace gamescript::codegen {

// Active game world binding for C runtime ABI
void bindActiveWorld(runtime::GameWorld* world);
runtime::GameWorld* getActiveWorld();

} // namespace gamescript::codegen

// C-linkage functions for LLVM / JIT runtime communication
extern "C" {
    void gs_runtime_move(int32_t dir, int64_t steps);
    void gs_runtime_turn(int32_t dir, int64_t degrees);
    void gs_runtime_action(int32_t actionCode);
    int64_t gs_runtime_sensor(int32_t sensorId);
    void gs_runtime_print_int(int64_t val);
}
