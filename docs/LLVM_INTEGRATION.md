# GameScript LLVM IR Code Generation & ABI

## 1. Overview
The GameScript compiler emits typed, SSA-form LLVM Intermediate Representation. GameScript character actions and environment sensory queries interface with the simulation runtime via a clean C-compatible Host Runtime ABI.

---

## 2. Host Runtime ABI Specification

The host runtime exports the following C function symbols:

```c
// Movement & Orientation
void gs_runtime_move(int32_t direction, int64_t steps);
void gs_runtime_turn(int32_t direction, int64_t degrees);

// Combat & Actions
void gs_runtime_action(int32_t actionCode);

// Environment Sensors
int64_t gs_runtime_sensor(int32_t sensorId);

// Debug Logging
void gs_runtime_print_int(int64_t val);
```

### Direction Codes:
- `0`: Forward
- `1`: Backward
- `2`: Left
- `3`: Right

### Action Codes:
- `0`: Attack
- `1`: Defend
- `2`: Jump
- `3`: Interact
- `4`: Retreat

### Sensor IDs:
- `0`: `enemy nearby` (Boolean 0/1)
- `1`: `health low` (Boolean 0/1)
- `2`: `obstacle ahead` (Boolean 0/1)
- `3`: `distance to enemy` (Integer steps)
- `4`: `health` (Player current HP)

---

## 3. Generated LLVM IR Example

For the GameScript source:
```gamescript
set speed = 5
player move forward speed
if enemy nearby:
    player attack
```

The GameScript compiler generates:
```llvm
; ModuleID = 'main'
source_filename = "main.gs"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

; --- GameScript Host Runtime ABI Declarations ---
declare void @gs_runtime_move(i32 noundef %dir, i64 noundef %steps)
declare void @gs_runtime_turn(i32 noundef %dir, i64 noundef %degrees)
declare void @gs_runtime_action(i32 noundef %actionCode)
declare i64 @gs_runtime_sensor(i32 noundef %sensorId)
declare void @gs_runtime_print_int(i64 noundef %val)

define void @main() {
entry:
  %var_speed = alloca i64, align 8
  %0 = add i64 0, 5
  store i64 5, ptr %var_speed, align 8
  %1 = load i64, ptr %var_speed, align 8
  call void @gs_runtime_move(i32 0, i64 5)
  %2 = call i64 @gs_runtime_sensor(i32 0)
  %3 = icmp ne i64 %2, 0
  br i1 %3, label %then_1, label %join_2
then_1:
  call void @gs_runtime_action(i32 0)
  br label %join_2
join_2:
  ret void
}
```

---

## 4. Compiling LLVM IR to Native Binary

Using standard LLVM tools (`clang` / `llc`):

```bash
# 1. Compile GameScript to LLVM IR
gamescript program.gs -l -c program.ll

# 2. Compile LLVM IR + Runtime ABI to executable
clang++ program.ll src/codegen/RuntimeABI.cpp src/runtime/GameWorld.cpp -o game_program.exe
```
