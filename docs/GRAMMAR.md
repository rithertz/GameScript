# GameScript Formal Grammar (EBNF)

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

FunctionDeclStmt::= "function" IDENTIFIER ("(" ParamList? ")")? ":" Block
FunctionCallStmt::= "call" IDENTIFIER ("(" ArgList? ")")? NEWLINE

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
```
