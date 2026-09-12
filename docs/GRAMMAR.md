# GameScript Formal Grammar (EBNF)

The following grammar defines the syntactic structure of GameScript programs.

```ebnf
Program         ::= Statement* EOF

Statement       ::= VarDeclStmt
                  | PlayerStmt
                  | IfStmt
                  | RepeatStmt
                  | WhileStmt
                  | FunctionDeclStmt
                  | FunctionCallStmt
                  | WhenStmt
                  | ExprStmt
                  | NEWLINE

VarDeclStmt     ::= "set" IDENTIFIER "=" Expression NEWLINE

PlayerStmt      ::= "player" "move" MoveDir Expression NEWLINE
                  | "player" "turn" TurnDir Expression NEWLINE
                  | "player" ActionKind NEWLINE

MoveDir         ::= "forward" | "backward" | "left" | "right"
TurnDir         ::= "left" | "right"
ActionKind      ::= "attack" | "defend" | "jump" | "interact" | "retreat"

IfStmt          ::= "if" Condition ":" Block ("else" ":" Block)?
RepeatStmt      ::= "repeat" Expression ":" Block
WhileStmt       ::= "while" Condition ":" Block
WhenStmt        ::= "when" Condition ":" Block

FunctionDeclStmt ::= "function" IDENTIFIER ("(" ParamList? ")")? ":" Block
FunctionCallStmt ::= "call" IDENTIFIER ("(" ArgList? ")")? NEWLINE

ParamList       ::= IDENTIFIER ("," IDENTIFIER)*
ArgList         ::= Expression ("," Expression)*

Block           ::= NEWLINE* INDENT Statement+ DEDENT

Condition       ::= LogicOr
LogicOr         ::= LogicAnd ("or" LogicAnd)*
LogicAnd        ::= LogicNot ("and" LogicNot)*
LogicNot        ::= ("not")* Relational
Relational      ::= Expression (("==" | "!=" | "<" | "<=" | ">" | ">=") Expression)?

Expression      ::= Term (("+" | "-") Term)*
Term            ::= Factor (("*" | "/" | "%") Factor)*
Factor          ::= ("+" | "-") Factor | Primary

Primary         ::= INTEGER
                  | STRING
                  | "true"
                  | "false"
                  | IDENTIFIER
                  | "(" Condition ")"
                  | SensoryExpr

SensoryExpr     ::= "enemy" "nearby"
                  | "health" "low"
                  | "obstacle" "ahead"
                  | "distance" "to" "enemy"
                  | "health"
````

---

## 1. Function Syntax

Function declarations consist of a function name, an optional parameter list, and an indented statement block.

```gamescript
function strike(distance):
    player move forward distance
    player attack
```

A function may also declare multiple parameters:

```gamescript
function patrol(distance, angle):
    player move forward distance
    player turn right angle
```

Function calls use the `call` keyword:

```gamescript
call strike(3)
call patrol(5, 90)
```

The grammar permits expressions as arguments:

```gamescript
set distance = 5
call strike(distance + 1)
```

---

## 2. Semantic Constraints

The grammar defines the syntactic structure of functions and calls. Additional constraints are checked by the semantic analyzer.

### Function Declarations

* Function names must not be redefined within the same scope.
* Parameter names must be unique within a function.
* Each parameter is currently assigned the `Integer` data type.
* Parameters are introduced into the function's local semantic scope.

### Function Calls

* The called function must exist.
* The number of arguments must exactly match the number of declared parameters.
* Each argument is checked against the corresponding parameter type.
* Arguments with an unknown type are not rejected solely because their type could not be determined.

For example:

```gamescript
function strike(distance):
    player move forward distance

call strike(5)
```

is valid, while:

```gamescript
call strike(true)
```

is rejected because `true` is boolean and `distance` is an integer parameter.

---

## 3. Indentation and Blocks

GameScript uses significant indentation.

A function body therefore requires an indented block:

```gamescript
function strike(distance):
    player move forward distance
    player attack
```

The lexer converts indentation changes into synthetic `INDENT` and `DEDENT` tokens. The parser uses these tokens to construct the corresponding AST block.

---

## 4. Expression Arguments

Function arguments use the existing `Expression` grammar and may therefore contain literals, identifiers, arithmetic expressions, boolean expressions where syntactically supported, and sensory expressions.

Examples:

```gamescript
call strike(5)
```

```gamescript
call strike(distance + 2)
```

The semantic analyzer determines the type of each argument and compares it with the corresponding function parameter type.


---

