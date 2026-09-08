# GameScript Compiler Optimizations

The GameScript optimizer implements a modular `PassManager` architecture executing AST-independent optimization passes over the intermediate representation.

---

## 1. Constant Folding Pass (`ConstantFoldingPass`)

### Motivation
Arithmetic and relational operations performed on known integer/boolean constants should be resolved at compile time to eliminate runtime calculations.

### Before Optimization:
```text
%0 = CONST_INT 2
%1 = CONST_INT 3
%2 = ADD %0, %1
STORE distance, %2
```

### After Optimization:
```text
%2 = CONST_INT 5  ; constant-folded: ADD 2, 3
STORE distance, %2
```

---

## 2. Constant Propagation Pass (`ConstantPropagationPass`)

### Motivation
When variables or registers hold known constant values, replacing subsequent references to those variables directly with the literal constant simplifies instructions and enables further dead code elimination.

### Before Optimization:
```text
%0 = CONST_INT 10
STORE speed, %0
%1 = LOAD speed
GAME_MOVE 0, %1
```

### After Optimization:
```text
%0 = CONST_INT 10
STORE speed, %0
%1 = CONST_INT 10  ; propagated constant from speed
GAME_MOVE 0, 10
```

---

## 3. Dead Code Elimination (`DeadCodeEliminationPass`)

### Motivation
Instructions that calculate intermediate register values that are never used in subsequent instructions or side-effect operations consume unnecessary space and execution cycles.

### Before Optimization:
```text
%0 = CONST_INT 42
%1 = ADD %0, 10
GAME_MOVE 0, 5
```

### After Optimization:
```text
GAME_MOVE 0, 5
```

---

## 4. Redundant Instruction Elimination (`RedundantInstEliminationPass`)

### Motivation
Game domain operations that result in no state change (e.g. moving 0 steps forward or turning 0 degrees) can be safely stripped from the instruction stream.

### Before Optimization:
```text
GAME_MOVE 0, 5
GAME_MOVE 0, 0
GAME_TURN 1, 0
GAME_ACTION 0
```

### After Optimization:
```text
GAME_MOVE 0, 5
GAME_ACTION 0
```
