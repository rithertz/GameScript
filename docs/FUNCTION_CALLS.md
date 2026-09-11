# GameScript Function Calls

## 1. Overview

GameScript supports user-defined functions with named parameters and explicit function calls.

The feature is implemented across the compiler pipeline:

```text
Function Declaration
        |
        v
      Lexer
        |
        v
      Parser
        |
        v
Semantic Analysis
        |
        v
        IR
        |
        v
Virtual Machine

```

The current implementation validates function declarations and calls during semantic analysis. IR argument passing and runtime parameter binding are implemented separately.

---

## 2. Function Declaration

A function is declared using `function` followed by its name and optional parameters:

```gamescript
function strike(distance):
    player move forward distance
    player attack

```

Multiple parameters are supported:

```gamescript
function patrol(distance, angle):
    player move forward distance
    player turn right angle

```

---

## 3. Function Calls

A function is invoked using `call`:

```gamescript
call strike(3)

```

Multiple arguments are supplied in declaration order:

```gamescript
call patrol(5, 90)

```

Arguments may be expressions:

```gamescript
set distance = 5
call strike(distance + 1)

```

---

## 4. Semantic Validation

The semantic analyzer performs the following checks.

### 4.1 Function Existence

Calls to undefined functions are rejected.

```gamescript
call missing_function(5)

```

This is rejected because `missing_function` has not been declared.

### 4.2 Argument Count

The number of arguments must match the number of parameters.

Given:

```gamescript
function strike(distance):
    player move forward distance

```

This call is valid:

```gamescript
call strike(5)

```

While this call is rejected:

```gamescript
call strike(5, 10)

```

because the function expects one argument.

### 4.3 Argument Types

GameScript function parameters currently use integer types.

```gamescript
function strike(distance):
    player move forward distance

```

The following is rejected:

```gamescript
call strike(true)

```

because `true` has boolean type while `distance` expects an integer.

### 4.4 Duplicate Parameters

Parameter names must be unique within a function.

```gamescript
function patrol(distance, distance):
    player move forward distance

```

This declaration is rejected during semantic analysis.

---

## 5. Symbol Table Representation

Function symbols store:

* Function name
* Function kind
* Return type
* Parameter count
* Parameter types
* Source location

Function parameters are represented as `Parameter` symbols inside the function's local scope.

Currently all GameScript function parameters use:

```text
DataType::Integer

```

---

## 6. Current Implementation Boundary

The semantic layer currently validates the correctness of function calls.

The remaining implementation work is to propagate arguments through the intermediate representation and bind them to parameters during virtual machine execution.

The intended pipeline is:

```text
call strike(3)
      |
      v
Semantic validation
      |
      v
IR CALL strike, 3
      |
      v
VM creates call frame
      |
      v
distance = 3
      |
      v
Execute function body

```

This separation keeps semantic validation independent from runtime execution.


