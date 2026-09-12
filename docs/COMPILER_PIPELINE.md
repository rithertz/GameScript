# GameScript Compiler Pipeline

The GameScript compiler follows a standard ahead-of-time (AOT) and intermediate representation pipeline.

```
+-------------------+
| GameScript Source |
+-------------------+
          |
          v
+-------------------+
|  Lexical Analysis |   --> Tokens with INDENT/DEDENT & SourceSpans
+-------------------+
          |
          v
+-------------------+
| Recursive Descent |   --> Abstract Syntax Tree (AST)
|      Parser       |
+-------------------+
          |
          v
+-------------------+
| Semantic Analysis |   --> Scope Resolution, Symbol Table, Type Inference
+-------------------+
          |
          v
+-------------------+
|    IR Builder     |   --> GameScript 3-Address Linear IR (Basic Blocks)
+-------------------+
          |
          v
+-------------------+
|   Pass Manager    |   --> Constant Folding, Propagation, DCE, Redundant Elim
|   (Optimizer)     |
+-------------------+
          |
          +-------------------------------+
          |                               |
          v                               v
+-------------------+           +-------------------+
|    LLVM Codegen   |           |  Virtual Machine  |
| (LLVM IR / Native)|           | (2D Simulation)   |
+-------------------+           +-------------------+
```

## Phase Breakdown

1. **Lexical Analysis (`Lexer`)**:
   - Stream reader with character lookahead.
   - Computes indentation delta using an internal `indentStack_`, emitting synthetic `TokenType::Indent` and `TokenType::Dedent`.
   - Attaches precise `SourceSpan` (line, column, byte offset) to each `Token`.

2. **Parsing (`Parser`)**:
   - Implements recursive-descent parsing with panic-mode error recovery and synchronization.
   - Enforces grammar constructs, transforming token sequences into typed AST nodes (`Stmt` and `Expr` hierarchies).

3. **Semantic Analysis (`SemanticAnalyzer`)**:
   - AST Visitor pattern traversing the AST.
   - Maintains a scoped `SymbolTable` (supporting lexical parent scopes).
   - Validates identifier declarations before use.
   - Prohibits re-declarations within the same scope level.
   - Registers function declarations and their parameter information.
   - Rejects duplicate function parameters.
   - Checks function-call argument count.
   - Checks each function-call argument against its corresponding parameter type.
   - Verifies condition types.

4. **Intermediate Representation (`IRBuilder`)**:
   - Lowers AST statements into linear 3-address instructions organized into `BasicBlock` structures within an `IRModule`.
   - Generates virtual SSA-style registers (`%0`, `%1`, `%2`) for intermediate expression evaluation.
   - Converts nested conditionals and loops into explicit jump/branch control-flow graphs (`BRANCH`, `BRANCH_COND`, `LABEL`).

5. **Optimization Passes (`PassManager`)**:
   - **Constant Folding**: Evaluates constant arithmetic/relational expressions at compile time.
   - **Constant Propagation**: Substitutes known constant values through register chains and variable stores.
   - **Dead Code Elimination**: Removes instructions whose destination registers are never referenced.
   - **Redundant Instruction Elimination**: Strips zero-distance moves and zero-degree turns.

6. **Code Generation (`LLVMCodegen`)**:
   - Generates clean, strongly typed LLVM IR module definitions.
   - Binds game operations to the external C-compatible Runtime ABI (`@gs_runtime_move`, `@gs_runtime_turn`, `@gs_runtime_action`, `@gs_runtime_sensor`).
   - Produces standard LLVM IR ready for compilation via `llc` or `clang`.

7. **Runtime & Simulation (`GameWorld` & `VirtualMachine`)**:
   - 2D grid world tracking Player state (x, y, orientation, HP, alive/dead), Enemies, Obstacles, and Action Logs.
   - Step-by-step bytecode execution with visual ASCII rendering.
