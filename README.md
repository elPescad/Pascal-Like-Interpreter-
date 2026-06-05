# Custom Interpreter & Hybrid Syntax Analyzer
### Programming Assignment 2 — Spring 2026

## Overview
This project implements a robust, single-pass **Recursive Descent Parser and Hybrid Execution Engine** for a strongly typed, block-structured language. Unlike a traditional multi-pass compiler that splits parsing and interpretation into disconnected phases, this engine utilizes contextual status flags (`execute` / `condition`) to validate language grammar while simultaneously processing live runtime execution, console I/O, and variable state mutation.

---

## System Architecture

The interpreter is split into two primary layers: the **Syntactic Control Layer** (the grammar logic) and the **Dynamic Execution Layer** (the type variant storage).

[ Source File Stream ] -> [ Lexical Sync │  (lex.h / Token Stream) ] -> [ The Parser  │  (parser.cpp / Recursive Descent Hierarchy) ] -> [ Value Variant │  (val.cpp / C++ Operator Overloading Matrix) ]

---

## Component Breakdown

### 1. Syntax Analyzer (Parser) — Structural Components
The top-level parser enforces the structural anatomy of programs, handling setup configurations and scoping blocks.

* **Program Roots (`Prog` & `Block`):** Validates the structural boundaries of the script, managing programmatic headers and driving block encapsulation.
* **Declarations & Scoping (`ConstPart`, `VarPart`, `DeclStmt`):** Coordinates environmental allocations. It forces strict isolation between constant variables (`defConst`) and mutable variables (`defVar`), tracking environmental states using localized structures like `declaredType`.
* **Flow Control (`IfStmt`, `CompStmt`, `StructuredStmt`):** Evaluates conditional structures. By dynamically toggling the `execute` boolean parameter, it effectively skips code execution inside dead branches while continuing to perform rigid syntactic checks on the unexecuted tokens.

### 2. State Management, Standard I/O & Expressions
This module bridges memory layout configurations with operational instruction sets.

* **Assignment & Mutability (`AssignStmt` & `Variable`):** Binds evaluated expression states to target variables. It queries memory definitions to guarantee prior variable initialization, blocks constant overriding, and enforces static type-safety checks.
* **Standard Input Handling (`ReadLnStmt` & `VarList`):** Directs raw user input processing via type-safe casting. It leverages `cin` for primitives, standard `getline` routines for full strings, and includes an aggressive case-insensitive filter to handle boolean strings (e.g., evaluating `tRuE` cleanly into a logical true state).
* **Logical Relational Expressions (`Expr` & `ExprList`):** Serves as the primary engine for terminal comparisons (`=`, `<`, `>`). It safely enforces grammar constraints by actively blocking illegal chained expressions (e.g., `a < b < c`).

### 3. Expression Precedence Cascade
To enforce mathematical order of operations without abstract syntax trees (ASTs), the parser utilizes a cascading recursive descent architecture:

SimpleExpr (Additive: +, -, or) -> Term (Multiplicative: *, /, idiv, mod, and) -> SFactor (Unary Signs: +, -, not) -> Factor (Base Primitives, Variables, Parentheses)

* **Operator Precedence (`SimpleExpr` & `Term`):** Implements order-of-operations natively. High-precedence constraints like multiplication, divisions (`/`, `idiv`, `mod`), and logical `and` are encapsulated cleanly within `Term`, while lower-precedence addition, subtraction, and logical `or` wrap around them inside `SimpleExpr`.
* **Unary Operators (`SFactor`):** Safely intercepts and processes structural math polarity (`+` / `-`) or logical evaluation inverted states (`not`).
* **Value Resolution (`Factor`):** The leaf node of the evaluation logic. It converts active primitive tokens (`ICONST`, `RCONST`, `BCONST`, `SCONST`, `CCONST`) into operational execution payload wrappers, maps variables to their stored definitions, and recursively rewinds evaluation logic when encountering parenthetical sub-expressions `( Expr )`.

### 4. Runtime Type Variant Engine (`Value`)
The `Value` object acts as a generic type variant, enabling dynamic runtime execution mechanics within a statically typed C++ workspace.

* **Type Tracking & Validation:** Leverages the explicit `ValType` discriminator enum (`VINT`, `VREAL`, `VSTRING`, `VCHAR`, `VBOOL`, `VERR`) alongside strict accessors. Accessing data with mismatched typings throws an instant native string runtime error.
* **Implicit Widening & Type Casting:** Automatically processes legal cross-type promotions, such as shifting an integer up to a floating-point real during mixed-mode math operations, while throwing strict parsing anomalies for illegal cross-type associations.
* **Overloaded Safety & Fault Injection:** Intercepts critical execution panics. For example, dividing by zero returns an instantaneous error status value (`VERR`), neutralizing compiler crashes.
* **Format-Constrained Serialization:** Overloads the standard `<<` output operator stream, enforcing language-specific string conversions and pinning float representations (`VREAL`) to a uniform precision of two decimal places (`fixed`, `setprecision(2)`).

---

## Unique Language Quirks & Rules

> ### Unary String Trimming
> This language includes a unique syntax feature where applying a unary minus (`-`) operator to a string or character literal acts as a trimming optimization rather than an arithmetic conversion. It forces a complete erasure of trailing whitespace buffers via a specialized `Trim()` method.
> ```pascal
> var s : string;
> begin
>     s := -'Hello World   '; { Evaluates to: "Hello World" }
> end.
> ```

> ### Anti-Chaining Protection
> The expression architecture restricts consecutive relational operators to safeguard logic flows. Writing code like `if x > y > z then` will generate an instantaneous `Illegal Relational Expression` error.

---

## File Manifest

* **`parser.h`** - The definitive programmatic API of the interpreter, translating every major grammatical rule into a unified function signature blueprint.
* **`parser.cpp`** - The foundational recursive descent validation engine and statement execution sequencer.
* **`val.h`** - The dynamic type payload structure declaration, handling type guards and access boundaries.
* **`val.cpp`** - The operator overloading logic matrix, handling runtime math operations, implicit type promotion, and string trimming routines.
