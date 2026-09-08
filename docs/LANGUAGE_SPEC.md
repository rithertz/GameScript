# GameScript Language Specification (v1.0)

## 1. Overview
**GameScript** is an indentation-sensitive, domain-specific programming language engineered to describe game character behavior and autonomous agents through human-readable commands.

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
```
Declares a variable in the local lexical scope and binds it to the evaluated expression.

### 3.2 Player Commands
```gamescript
player move (forward | backward | left | right) <expression>
player turn (left | right) <expression>
player (attack | defend | jump | interact | retreat)
```

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
```gamescript
function <identifier>[(<param1>, <param2>, ...)]:
    <statement_block>

call <identifier>[(<arg1>, <arg2>, ...)]
```

---

## 4. Sensory Conditions & Expressions

### 4.1 Sensors
- `enemy nearby` : Evaluates to `true` if an enemy entity is within proximity radius (<= 2 tiles).
- `health low` : Evaluates to `true` if player HP is <= 30%.
- `obstacle ahead` : Evaluates to `true` if a blocking tile is immediately in front.
- `distance to enemy` : Returns integer Manhattan distance to closest active enemy.

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
