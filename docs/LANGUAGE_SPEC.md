# GameScript Language Specification (v1.0)

## 1. Overview

**GameScript** is an indentation-sensitive, domain-specific programming language engineered to describe game character behavior and autonomous agents through human-readable commands.

The language is designed around simple game-oriented statements while following a conventional compiler architecture. Source programs are processed through lexical analysis, parsing, semantic analysis, intermediate representation, optimization, and runtime execution.

---

## 2. Lexical Structure

### 2.1 Identifiers & Keywords

- **Identifiers**: Match `[a-zA-Z_][a-zA-Z0-9_]*`.
- **Keywords**: `set`, `player`, `move`, `turn`, `attack`, `defend`, `jump`, `interact`, `retreat`, `if`, `else`, `repeat`, `while`, `function`, `call`, `when`, `not`, `and`, `or`, `true`, `false`, `forward`, `backward`, `left`, `right`, `enemy`, `nearby`, `health`, `low`, `obstacle`, `ahead`, `distance`, `to`.

### 2.2 Literals

- **Integers**: 64-bit signed decimal integers (e.g., `0`, `5`, `90`, `100`).
- **Strings**: Double-quoted strings with escape characters `\n`, `\t`, `\"`, `\\`.
- **Booleans**: `true`, `false`.

### 2.3 Comments & Whitespace

- Single-line comments start with `#` and extend to the end of the line.
- Indentation is significant (using spaces or tabs), generating synthetic `INDENT` and `DEDENT` tokens to delimit code blocks.

---

## 3. Statements

### 3.1 Variable Declaration (`set`)

```gamescript
set <identifier> = <expression>
````

Declares a variable in the local lexical scope and binds it to the evaluated expression.

Example:

```gamescript
set distance = 5
player move forward distance
```

### 3.2 Player Commands

```gamescript
player move (forward | backward | left | right) <expression>
player turn (left | right) <expression>
player (attack | defend | jump | interact | retreat)
```

Movement and turning expressions may contain literals, variables, arithmetic expressions, and supported sensory expressions where applicable.

### 3.3 Control Flow

#### Conditional (`if` / `else`)

```gamescript
if <condition>:
    <statement_block>
else:
    <statement_block>
```

#### Iteration (`repeat` & `while`)

```gamescript
repeat <count_expression>:
    <statement_block>

while <condition>:
    <statement_block>
```

### 3.4 Functions & Subroutines

Functions allow reusable blocks of GameScript behavior to be declared with optional named parameters.

```gamescript
function <identifier>[(<param1>, <param2>, ...)]:
    <statement_block>

call <identifier>[(<arg1>, <arg2>, ...)]
```

Example:

```gamescript
function strike(distance):
    player move forward distance
    player attack

call strike(3)
```

Multiple parameters are supported:

```gamescript
function patrol(distance, angle):
    player move forward distance
    player turn right angle

call patrol(5, 90)
```

Arguments are supplied in the same order as the corresponding parameters.

#### Function Rules

* Function names must be unique within the current scope.
* Parameter names must be unique within a function.
* Parameters are currently typed as integers.
* The number of arguments in a function call must exactly match the number of declared parameters.
* Each argument is checked against its corresponding parameter type during semantic analysis.
* Function parameters are available as variables within the function body.
* Calls to undefined functions are rejected during semantic analysis.
* Function return values are not currently supported; functions are treated as `void` operations.

---

## 4. Sensory Conditions & Expressions

### 4.1 Sensors

* `enemy nearby` : Evaluates to `true` if an enemy entity is within proximity radius (<= 2 tiles).
* `health low` : Evaluates to `true` if player HP is <= 30%.
* `obstacle ahead` : Evaluates to `true` if a blocking tile is immediately in front.
* `distance to enemy` : Returns integer Manhattan distance to closest active enemy.
* `health` : Returns the player's current health as an integer value.

### 4.2 Operators & Precedence

From lowest to highest precedence:

1. `or`
2. `and`
3. `not`
4. Relational (`==`, `!=`, `<`, `<=`, `>`, `>=`)
5. Additive (`+`, `-`)
6. Multiplicative (`*`, `/`, `%`)
7. Unary (`+`, `-`, `not`)
8. Grouping (`(...)`)

---

## 5. Semantic Constraints

The syntax of a GameScript program is defined by the formal grammar, while additional validity rules are enforced during semantic analysis.

The semantic analyzer validates:

* Variable declarations and identifier resolution.
* Duplicate declarations within the same scope.
* Function declarations and function name uniqueness.
* Duplicate parameter names within a function.
* Function-call argument count.
* Function-call argument types.
* Condition expression types.
* Function call references to existing function declarations.

Semantic errors are reported with source locations through the compiler diagnostic system.

---

## 6. Function Parameter Types

GameScript currently represents function parameters using the integer data type.

For example:

```gamescript
function move_player(distance):
    player move forward distance
```

Here, `distance` is an integer parameter.

Passing a boolean argument is rejected:

```gamescript
call move_player(true)
```

The semantic analyzer compares each argument with the corresponding parameter type.

Multiple parameters are checked independently:

```gamescript
function patrol(distance, angle):
    player move forward distance
    player turn right angle

call patrol(5, 90)
```

Both arguments must satisfy their corresponding integer parameter types.

---

## 7. Current Function Implementation Boundary

Function declarations and calls are represented in the AST and validated during semantic analysis.

The current semantic layer provides:

* Function symbol registration.
* Parameter symbol registration.
* Parameter type information.
* Duplicate parameter detection.
* Argument-count validation.
* Argument-type validation.

Function argument propagation through the intermediate representation and runtime parameter binding are implemented as subsequent compiler stages.



---

